
#pragma once 
#include "CmlExt/CmlExt.h"

namespace PBS
{

class RigidTransform
{
public:
	RigidTransform();
	RigidTransform(const RigidTransform& T);
	RigidTransform(cml::vector3d t, cml::quaterniond r);



	RigidTransform& operator=(const RigidTransform& a)
	{
		t_ = a.t_;
		r_ = a.r_;
		return *this;
	}

	RigidTransform operator*(const RigidTransform& b)
	{
		RigidTransform &a = *this;
		RigidTransform c;
		c.r_ = a.r_ * b.r_;
		c.t_ = a.t_ + cml::Rotate(a.r_, b.t_);
		return c;
	}

	static RigidTransform Lerp(const RigidTransform& a, const RigidTransform& b, double t)
	{
		return RigidTransform(cml::lerp(a.t_, b.t_, t), cml::slerp(a.r_, b.r_, t));
	}
	void Inverse();


	cml::matrix44d GetMat44() const;
	RigidTransform GetInverse() const;

	void t(cml::vector3d t) { t_ = t; }
	cml::vector3d t() const { return t_; }
	cml::vector3d& t() { return t_; }

	void r(cml::quaterniond r) { r_ = r; }
	cml::quaterniond r() const { return r_; }
	cml::quaterniond& r() { return r_; }

protected:
	cml::vector3d t_;
	cml::quaterniond r_;
};

};
