#include "RigidTransform.h"
#include <math.h>


namespace PBS
{

	RigidTransform::RigidTransform()
	{
		t_.set(0, 0, 0);
		r_.identity();
	}

	RigidTransform::RigidTransform(const RigidTransform& T)
	{
		t_ = T.t_;
		r_ = T.r_;
	}

	RigidTransform::RigidTransform(cml::vector3d t, cml::quaterniond r)
	{
		t_ = t;
		r_ = r;
	}


	cml::matrix44d
		RigidTransform::GetMat44() const
	{
		return cml::MatrixTranslation(t_) * cml::MatrixRotationQuaternion(r_);
	}


	void
		RigidTransform::Inverse()
	{
		r_.inverse();
		t_ = cml::Rotate(r_, -1 * t_);
	}

	RigidTransform
		RigidTransform::GetInverse() const
	{
		RigidTransform b(*this);
		b.Inverse();
		return b;
	}


};