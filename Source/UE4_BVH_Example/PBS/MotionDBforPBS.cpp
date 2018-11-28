

#include "MotionDBforPBS.h"
#include <functional>
#include <algorithm>
#include "PBS/Registeration.h"
#include <time.h>
#include "EngineUtils.h"

#define UE4

namespace PBS
{


//const double
//MotionDBforPBS::traject_sampling_step = 4.0;



MotionDBforPBS::MotionDBforPBS()
{
}

void 
MotionDBforPBS::addMotion(ml::Motion *motion, std::string name)
{
	ml::MotionContainer::addMotion(motion, name);

	//sampleFrames(motion);
}


void MotionDBforPBS::SampleFrames()
{
	m_frames.clear();

	for ( auto &d : motions_ )
	{
		sampleFrames(d);
	}
}


void
MotionDBforPBS::sampleFrames(ml::Motion *motion)
{
	std::vector<int> valleys;
	ml::SelectKeyFramesByKineticEnergy(motion, valleys, 3);

	for ( int i=0; i<(int)valleys.size(); i++ )
	{
		m_frames.push_back(MotionFrame(motion, valleys[i]));
	}
}






bool
MotionDBforPBS::Search(const SketchedQuery& query, SearchResult &out_result) const
{
	TargetPointSet target_points;
	BuildTargetPointSet(query, target_points);

	int first = true;
	bool succeed_ever = false;

	for ( int i=0; i<(int)m_frames.size(); i++ )
	{
		SearchResult tmp_result;
		
		bool flag_success = Compare(target_points, m_frames[i], tmp_result);

		if ( flag_success )
		{
			succeed_ever = true;

			if ( first )
				out_result = tmp_result;
			else if ( out_result.error_ > tmp_result.error_ )
				out_result = tmp_result;

			first = false;
		}

	}


	// Performance Measuring
	if ( false )
	{
		int num_frames[3] = {3875, 10000, 100000};
		
		for ( int ff=0; ff<3; ff++ )
		{
			int tries = 50;

			int ii =0;
			clock_t start_t = clock();

			for ( int k=0; k<tries; k++ )
			{
				for ( int ii=0; ii<num_frames[ff]; ii++ )
				{
					int i = ii % (int)m_frames.size();
					SearchResult tmp_result;
		
					bool flag_success = Compare(target_points, m_frames[i], tmp_result);

					if ( flag_success )
					{
						if ( first )
							out_result = tmp_result;
						else if ( out_result.error_ > tmp_result.error_ )
							out_result = tmp_result;

						first = false;
					}

				}
			}
			clock_t end_t = clock();

		

			std::cout << "num of points: " << query.CountJointConstraints() << std::endl;
			std::cout << "num of frames: " << num_frames[ff] << std::endl;
			std::cout << (double)(end_t-start_t)/tries << " ms" << std::endl;
			std::cout <<  std::endl;
			std::cout <<  std::endl;
		}
	}


	return succeed_ever;
}



bool
MotionDBforPBS::Search(const SketchedQuery& query, int n, std::vector<SearchResult> &out_results)
{
	out_results.clear();

	TargetPointSet target_points;
	BuildTargetPointSet(query, target_points);

	for ( int i=0; i<(int)m_frames.size(); i++ )
	{

		SearchResult tmp_result;
		
		double registering_scale = 1.0;

		bool flag_success = Compare(target_points, m_frames[i], tmp_result);

		if ( flag_success )
		{
			out_results.push_back(tmp_result);
		}

	}

	struct compareSearchResult : public std::binary_function<SearchResult, SearchResult, bool>
	{
		bool operator()( SearchResult lhs, SearchResult rhs)
		{
			return (lhs.error_ < rhs.error_);
		}
	};
	std::sort(out_results.begin(), out_results.end(), compareSearchResult());

	if ( (int)out_results.size() > n )
		out_results.resize(n);

	if ( out_results.size() == 0 ) return false;


	return true;
}




bool
MotionDBforPBS::Compare(const PBS::TargetPointSet &target_point_set, const MotionFrame &m_frame, SearchResult &out_result) const
{

	if (target_point_set.empty()) return false;

	PBS::ObjectPointSet object_point_set;
	PBS::RigidTransform registering_transf;
	
	double error;

	cml::vector3d to_origin_the_motion = -1 * cml::plane_vec3( m_frame.getGlobalTranslation(ml::PELVIS) );

	BuildObjectPointSet(target_point_set, m_frame, object_point_set);
	MoveObjectPointSet(object_point_set, to_origin_the_motion);
	
#ifdef UE4
	bool result = RegisterObjectToTarget_UE4(object_point_set, target_point_set, registering_transf, error);
#else
	bool result = RegisterObjectToTarget(object_point_set, target_point_set, registering_transf, error);
#endif
	if ( !result ) return result;

	registering_transf.t() += cml::Rotate(registering_transf.r(), to_origin_the_motion);

	out_result.motion_ = m_frame.motion;
	out_result.frame_ = m_frame.frame;
	out_result.error_ = error;
	out_result.registering_rigid_transf_ = registering_transf;
	

	return result;
}


bool
MotionDBforPBS::Compare(const SketchedQuery& query, const MotionFrame &m_frame, SearchResult &out_result)
{
	if (query.Empty() ) return false;

	PBS::TargetPointSet target_point_set;
	BuildTargetPointSet(query, target_point_set);

	return Compare(target_point_set, m_frame, out_result);
}



void 
MotionDBforPBS::BuildTargetPointSet(const SketchedQuery& query, PBS::TargetPointSet &out_target_point_set) const
{
	out_target_point_set.clear();

	for ( unsigned int i=0; i<query.CountJointConstraints(); i++ )
	{
		const SketchedQuery::JointConstraint &c = query.GetJointConstraint(i);
		TargetPoint t;
		t.joint = c.name_;
		t.SetValue( c.p_ );

		out_target_point_set.push_back(t);
	}

}

void 
MotionDBforPBS::BuildObjectPointSet(const PBS::TargetPointSet &in_target_point_set, const MotionFrame &m_frame
								, PBS::ObjectPointSet &out_object_point_set) const
{
	out_object_point_set.clear();

	for (int i = 0; i<(int)in_target_point_set.size(); i++)
	{
		ml::JointTag pm_joint_id = PBS::SketchedQuery::MapSketchToPmBody( in_target_point_set[i].joint );

		if ( pm_joint_id == ml::HEAD )
		{
			// We regard PmHuman::Head as the nose.
			// As a user set a nose position, he or she can direct where the character have to look at.

			ObjectPoint o;
			o.SetValue( m_frame.getNoseGlobalTranslation() );
			out_object_point_set.push_back(o);
		}
		else
		{
			ObjectPoint o;
			o.SetValue( m_frame.getGlobalTranslation( pm_joint_id ) );
			out_object_point_set.push_back(o);
		}
	} 
}

void 
MotionDBforPBS::BuildObjectPointSet(const SketchedQuery& query, const MotionFrame &m_frame
								, PBS::ObjectPointSet &out_object_point_set) const
{
	
	PBS::TargetPointSet t;
	BuildTargetPointSet(query, t);
	
	BuildObjectPointSet(t, m_frame, out_object_point_set);

}


void
MotionDBforPBS::MoveObjectPointSet(PBS::ObjectPointSet &object_point_set, cml::vector3d t) const
{
	for ( int i=0; i<(int)object_point_set.size(); i++ )
	{
		object_point_set[i][0] += t[0];//*target_point_set[i].world_scale;
		object_point_set[i][1] += t[1];//*target_point_set[i].world_scale;
		object_point_set[i][2] += t[2];//*target_point_set[i].world_scale;
	}
}


void
MotionDBforPBS::ScaleObjectPointSet(PBS::ObjectPointSet &object_point_set, double s)
{
	for ( int i=0; i<(int)object_point_set.size(); i++ )
	{
		object_point_set[i][0] *= s;//*target_point_set[i].world_scale;
		object_point_set[i][1] *= s;//*target_point_set[i].world_scale;
		object_point_set[i][2] *= s;//*target_point_set[i].world_scale;
	}
}





bool
MotionDBforPBS::loadBinaryFile(std::string filename)
{
	std::ifstream fin(filename, std::ios::binary);
	if ( fin.fail() )
	{
		fin.close();
		return false;
	}
	readBinary(fin);
	fin.close();
	return true;
}

void
MotionDBforPBS::readBinary(std::ifstream &in)
{
	MotionContainer::loadBinary(in);

	m_frames.clear();

	int total_frame = 0;
	for ( auto &d : motions_)
	{
		sampleFrames(d);

		total_frame += d->size();
	}

	printf("total frames: %d\n", total_frame);
	printf("sig frames: %d\n", (int)m_frames.size());
}



};



