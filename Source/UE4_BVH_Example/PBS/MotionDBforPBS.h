
#pragma once

#include "PBS/SketchedQuery.h"
#include "PBS/MotionFrame.h"
#include "PBS/SearchResult.h"
#include "PBS/Registeration.h"
#include "Motion/MotionContainer.h"

namespace PBS
{


class MotionDBforPBS : public ml::MotionContainer
{
public:
	MotionDBforPBS();
	virtual void addMotion(ml::Motion *motion, std::string name="");

	bool Search(const SketchedQuery& query, SearchResult &out_result) const;
	bool Search(const SketchedQuery& query, int n, std::vector<SearchResult> &out_results);

	virtual bool loadBinaryFile(std::string filename);
	virtual void readBinary(std::ifstream &out);

	void SampleFrames();

protected:
	void sampleFrames(ml::Motion *motion);

	bool Compare(const PBS::TargetPointSet &target_point_set, const MotionFrame &m_frame, SearchResult &out_result) const;
	bool Compare(const SketchedQuery& query, const MotionFrame &m_frame, SearchResult &out_result);

	void BuildTargetPointSet(const SketchedQuery& query, PBS::TargetPointSet &out_target_point_set) const;

	void BuildObjectPointSet(const SketchedQuery& query, const MotionFrame &m_frame
						, PBS::ObjectPointSet &out_object_point_set) const;
	void BuildObjectPointSet(const PBS::TargetPointSet &in_target_point_set, const MotionFrame &m_frame
						, PBS::ObjectPointSet &out_object_point_set) const;

	void MoveObjectPointSet(PBS::ObjectPointSet &object_point_set, cml::vector3d t) const;
	void ScaleObjectPointSet(PBS::ObjectPointSet &object_point_set, double s);


protected:
	MotionFrames m_frames;
	//static const double traject_sampling_step; // 4 inch or about 10 cm.

};





};










