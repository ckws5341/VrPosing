
#pragma once

#include "PBS/RigidTransform.h"
#include "Motion/motion.h"

namespace PBS
{

class SearchResult
{
public:
	SearchResult(){ motion_=0; }

	cml::transf GetRegisteringTransf() const;
	cml::vector3d GetRegisteredPmJoint(int j) const;
	cml::vector3d GetRegisteredPmJoint(ml::JointTag j_tag) const;

	ml::Motion *motion_;
	int frame_;

	RigidTransform registering_rigid_transf_;
	//::vector to_origin;	

	double error_;
	
};

};










