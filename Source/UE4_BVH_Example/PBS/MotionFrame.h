
#pragma once


#include "Motion/motion.h"
#include "Motion/posture.h"

namespace PBS
{

class MotionFrame
{
public:
	MotionFrame()
	{
		motion = 0;
		frame = -1;
	}

	MotionFrame(ml::Motion *m, int f)
	{
		motion = m;
		frame = f;
	}

	ml::Motion *motion;
	int frame;

public:
	const ml::Posture& getPosture() const { return motion->posture(frame); }
	cml::vector3d getGlobalTranslation(int j) const 
	{
		return getPosture().GetGlobalTranslation(j);
	}
	cml::vector3d getGlobalTranslation(ml::JointTag j_tag) const
	{
		return getPosture().GetGlobalTranslation(j_tag);
	}

	cml::vector3d getNoseGlobalTranslation() const
	{
		if ( motion->body()->HasTag(ml::NECK) )
		{
			cml::vector3d nose_v(0, 7, 10);

			return cml::Rotate( getPosture().GetGlobalRotationQ(ml::NECK), nose_v ) + getPosture().GetGlobalTranslation(ml::NECK);
		}

		return cml::vector3d(0, 0, 0);
	}
};


class MotionFrames : public std::vector<MotionFrame>
{
public:
};




};










