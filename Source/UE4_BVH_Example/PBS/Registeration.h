
#pragma once

#include "Motion/motion.h"
#include "PBS/PBS.h"
#include "PBS/Query.h"
#include "PBS/SearchResult.h"
#include "PBS/RigidTransform.h"
#include "PBS/SketchedQuery.h"

namespace PBS
{

class TargetPoint : public cml::vector3d
{
public:

	SketchedQuery::BodyJointName joint;
	
	void SetValue(const cml::vector3d &v)
	{
		(*this)[0] = v[0];
		(*this)[1] = v[1];
		(*this)[2] = v[2];
	}
};

class TargetPointSet : public std::vector<TargetPoint>
{
public:
};


class ObjectPoint : public cml::vector3d
{
public:
	void SetValue(const cml::vector3d &v)
	{
		(*this)[0] = v[0];
		(*this)[1] = v[1];
		(*this)[2] = v[2];
	}

};

class ObjectPointSet : public std::vector<ObjectPoint>
{
public:
};


//////////////////////////////////////////////////////////////////////////
// Function registerObjectToTarget. 
//
// This function calculates a transformation (scale, rotataiton, translation), 
// which registers the object-point-set to the target-point-set.

// The inputs are the object-point-set and the target-point-set with same size.
// Every point of the object-point-set is 3D vector, which is a sampled point 
// on the target rigid object in the world coordinate.
// There are two types of target-point in the target-point-set.
// One is 3D-target-point which is defined in the world coordinate.
// The other is 2D-target-point which is defined on the 2D view plane.

bool RegisterObjectToTarget(const ObjectPointSet &in_obj_point_set, const TargetPointSet &in_tar_point_set, RigidTransform &out, double &out_error);

bool RegisterObjectToTarget_UE4(const ObjectPointSet &in_obj_point_set, const TargetPointSet &in_tar_point_set, RigidTransform &out, double &out_error);

};