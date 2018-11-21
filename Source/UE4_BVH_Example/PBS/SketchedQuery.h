
#pragma once
#include "CmlExt/CmlExt.h"
#include "PBS/PBS.h"
#include "Motion/motion.h"
#include <vector>


namespace PBS
{

class SketchedQuery
{
public:
	static const int num_joints = 14;
	enum BodyJointName { J_HEAD, J_LEYE, J_REYE, J_NECK, J_SPIN, J_PELV, J_LELB, J_RELB, J_LPAM, J_RPAM, J_LKNE, J_RKNE, J_LFOT, J_RFOT, J_NONE };

	static bool isLeftArm(BodyJointName name)
	{
		switch ( name )
		{
		case J_LELB:
		case J_LPAM:
			return true;
		default:
			return false;
		}
	}

	static bool isRightArm(BodyJointName name)
	{
		switch ( name )
		{
		case J_RELB:
		case J_RPAM:
			return true;
		default:
			return false;
		}
	}

	static bool isLeftLeg(BodyJointName name)
	{
		switch ( name )
		{
		case J_LKNE:
		case J_LFOT:
			return true;
		default:
			return false;
		}
	}

	static bool isRightLeg(BodyJointName name)
	{
		switch ( name )
		{
		case J_RKNE:
		case J_RFOT:
			return true;
		default:
			return false;
		}
	}
	static bool isLeft(BodyJointName name)
	{
		switch ( name )
		{
		case J_LELB:
		case J_LPAM:
		case J_LKNE:
		case J_LFOT:
			return true;
		default:
			return false;
		}
	}

	

	static bool isRight(BodyJointName name)
	{
		switch ( name )
		{
		case J_RELB:
		case J_RPAM:
		case J_RKNE:
		case J_RFOT:
			return true;
		default:
			return false;
		}
	}

	
	static ml::JointTag MapSketchToPmBody(BodyJointName sf_joint);
	static BodyJointName MapPmToSketchBody(ml::JointTag pm_joint);

	
	class JointConstraint
	{
	public:
		JointConstraint()
		{
			name_ = J_NONE;
			p_ = cml::vector3d(0, 0, 0);
		}

		JointConstraint(BodyJointName n, cml::vector3d p)
		{
			name_ = n;
			p_ = p;
		}
		BodyJointName name_;
		cml::vector3d p_;

		void WriteBinary(std::ostream &out) const
		{
			out.write((char*)&name_, sizeof(name_));
			out.write((char*)&p_, sizeof(p_));
		}

		void ReadBinary(std::istream &in) 
		{
			in.read((char*)&name_, sizeof(name_));
			in.read((char*)&p_, sizeof(p_));
		}
	};



	SketchedQuery();
	
	
	void AddJointConstraint(BodyJointName name, cml::vector3d p);
	void DeleteJointConstraint(BodyJointName name);

	void SelectJointConstraint(int i) { selected_constraint_ = i; }
	void SelectJointConstraintNear(cml::vector3d p);
	void DeselectJointConstraint();
	bool IsSelectedJointConstraint(int i) const;
	bool HasASelectedJointConstraint() const;
	void DeleteSelectedJointConstraint();
	void MoveSelectedJointConstraint(cml::vector3d p);

	bool HasJointConstraint(BodyJointName name) const;
	const JointConstraint& GetJointConstraint(int i) const;
	const JointConstraint& GetSelectedJointConstraint() const;

	void Clear();
	bool Empty() const;


	

	unsigned int CountJointConstraints() const;
	unsigned int CountJointConstraints(BodyJointName name) const;


	void WriteBinary(std::ostream &out) const;
	void ReadBinary(std::istream &out);

protected:
	int selected_constraint_;
	std::vector<JointConstraint> joint_constraints_;
};


};