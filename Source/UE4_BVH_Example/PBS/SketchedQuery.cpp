
#include "PBS/SketchedQuery.h"
#include "Motion/motion.h"

namespace PBS
{

SketchedQuery::SketchedQuery()
{
}


ml::JointTag
SketchedQuery::MapSketchToPmBody(SketchedQuery::BodyJointName s_joint)
{
	switch ( s_joint )
	{
	//case SketchedQuery::J_HEAD:	return PmHuman::HEAD;
	case SketchedQuery::J_NECK:	return ml::NECK;
	case SketchedQuery::J_SPIN:	return ml::SPINE2;
	case SketchedQuery::J_PELV:	return ml::PELVIS;
	case SketchedQuery::J_LELB:	return ml::L_ELBOW;
	case SketchedQuery::J_RELB:	return ml::R_ELBOW;
	case SketchedQuery::J_LPAM:	return ml::L_WRIST;
	case SketchedQuery::J_RPAM:	return ml::R_WRIST;
	case SketchedQuery::J_LKNE:	return ml::L_KNEE;
	case SketchedQuery::J_RKNE:	return ml::R_KNEE;
	case SketchedQuery::J_LFOT:	return ml::L_FOOT;
	case SketchedQuery::J_RFOT:	return ml::R_FOOT;
	case SketchedQuery::J_HEAD:	return ml::HEAD;

	default:
		break;;
	}

	return ml::UNKNOWN;
}

SketchedQuery::BodyJointName 
SketchedQuery::MapPmToSketchBody(ml::JointTag pm_joint)
{
	switch ( pm_joint )
	{
	//case SketchedQuery::J_HEAD:	return PmHuman::HEAD;
	case ml::NECK:			return SketchedQuery::J_NECK;
	case ml::SPINE2:		return SketchedQuery::J_SPIN;
	case ml::PELVIS:		return SketchedQuery::J_PELV;
	case ml::L_ELBOW:		return SketchedQuery::J_LELB;
	case ml::R_ELBOW:		return SketchedQuery::J_RELB;
	case ml::L_WRIST:		return SketchedQuery::J_LPAM;
	case ml::R_WRIST:		return SketchedQuery::J_RPAM;
	case ml::L_KNEE:		return SketchedQuery::J_LKNE;
	case ml::R_KNEE:		return SketchedQuery::J_RKNE;
	case ml::L_FOOT:		return SketchedQuery::J_LFOT;
	case ml::R_FOOT:		return SketchedQuery::J_RFOT;
	case ml::HEAD:			return SketchedQuery::J_HEAD;

	default:
		break;
	}

	return J_NONE;
}







void
SketchedQuery::AddJointConstraint(SketchedQuery::BodyJointName name, cml::vector3d p)
{
	if ((int)name < 0 || (int)name >= num_joints) return;

	joint_constraints_.push_back(JointConstraint(name, p));
}



void
SketchedQuery::DeleteJointConstraint(BodyJointName name)
{
	for (unsigned int i = 0; i < joint_constraints_.size(); i++)
	{
		if (joint_constraints_[i].name_ == name)
		{
			joint_constraints_.erase(joint_constraints_.begin() + i);
			i--;
		}
	}
}

void
SketchedQuery::SelectJointConstraintNear(cml::vector3d p)
{
	int closest = -1;
	double closest_d = 0;

	for (unsigned int i = 0; i < joint_constraints_.size(); i++)
	{
		double d = cml::length(joint_constraints_[i].p_ - p); 
		if ( closest == -1 || closest_d > d )
		{
			closest = i;
			closest_d = d;
		}
	}

	if (closest_d < 5)
	{
		selected_constraint_ = closest;
	}
	else
	{
		selected_constraint_ = -1;
	}
}

void
SketchedQuery::DeselectJointConstraint()
{
	selected_constraint_ = -1;
}


void 
SketchedQuery::DeleteSelectedJointConstraint()
{
	if (selected_constraint_ >= 0 && selected_constraint_ < (int)joint_constraints_.size())
	{
		joint_constraints_.erase(joint_constraints_.begin() + selected_constraint_);
	}

	selected_constraint_ = -1;

}

bool
SketchedQuery::IsSelectedJointConstraint(int i) const
{
	if (selected_constraint_ >= 0 && selected_constraint_ < (int)joint_constraints_.size())
	{
		if (selected_constraint_ == i) return true;
	}


	return false;
}

bool
SketchedQuery::HasASelectedJointConstraint() const
{
	if (selected_constraint_ >= 0 && selected_constraint_ < (int)joint_constraints_.size())
	{
		return true;
	}


	return false;
}

void
SketchedQuery::MoveSelectedJointConstraint(cml::vector3d p)
{
	if (selected_constraint_ >= 0 && selected_constraint_ < (int)joint_constraints_.size())
	{
		joint_constraints_[selected_constraint_].p_ = p;
	}
}

void
SketchedQuery::Clear()
{
	joint_constraints_.clear();
}

bool
SketchedQuery::Empty() const
{
	return joint_constraints_.empty();
}


bool
SketchedQuery::HasJointConstraint(SketchedQuery::BodyJointName name) const
{
	for (unsigned int i = 0; i < joint_constraints_.size(); i++)
	{
		if (joint_constraints_[i].name_ == name)
		{
			return true;
		}
	}

	return false;
}

const SketchedQuery::JointConstraint&
SketchedQuery::GetJointConstraint(int i) const
{
	return joint_constraints_[i];
}

const SketchedQuery::JointConstraint&
SketchedQuery::GetSelectedJointConstraint() const
{
	return joint_constraints_[selected_constraint_];
}


unsigned int
SketchedQuery::CountJointConstraints() const
{
	return joint_constraints_.size();
}

unsigned int
SketchedQuery::CountJointConstraints(BodyJointName name) const
{
	int count = 0;
	for (unsigned int i = 0; i < joint_constraints_.size(); i++)
	{
		if (joint_constraints_[i].name_ == name)
		{
			count++;
		}
	}

	return count;
}


void
SketchedQuery::WriteBinary(std::ostream &out) const
{
	unsigned int size = joint_constraints_.size();

	out.write((char*)&size, sizeof(size));

	for (unsigned int i = 0; i < size; i++)
	{
		joint_constraints_[i].WriteBinary(out);
	}
}


void
SketchedQuery::ReadBinary(std::istream &in)
{
	unsigned int size;

	in.read((char*)&size, sizeof(size));

	joint_constraints_.resize(size);

	for (unsigned int i = 0; i < size; i++)
	{
		joint_constraints_[i].ReadBinary(in);
	}
}
};