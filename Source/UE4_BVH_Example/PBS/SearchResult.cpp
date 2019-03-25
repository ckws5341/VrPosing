

#include "SearchResult.h"
#include <functional>
#include <algorithm>
#include "PBS/MotionDBforPBS.h"

namespace PBS
{

cml::vector3d
SearchResult::GetRegisteredPmJoint(int j) const
{
	cml::vector3d p = motion_->posture(frame_).GetGlobalTranslation(j);

	p = cml::Rotate(registering_rigid_transf_.r(), p);
	p += registering_rigid_transf_.t();

	return p;
}

cml::vector3d
SearchResult::GetRegisteredPmJoint(ml::JointTag j_tag) const
{
	cml::vector3d p = motion_->posture(frame_).GetGlobalTranslation(j_tag);

	p = cml::Rotate(registering_rigid_transf_.r(), p);
	p += registering_rigid_transf_.t();

	return p;
}


cml::transf
SearchResult::GetRegisteringTransf() const
{
	cml::transf it;
	it.identity();
	if ( motion_ == 0 ) return it;

	cml::transf t;

	/*t = cml::MatrixRotationQuaternion(registering_rigid_transf_.r());
	t = t * cml::MatrixTranslation(registering_rigid_transf_.t());*/

	t = cml::MatrixTranslation(registering_rigid_transf_.t());
	t = t*cml::MatrixRotationQuaternion(registering_rigid_transf_.r());

	return t;
}


};



