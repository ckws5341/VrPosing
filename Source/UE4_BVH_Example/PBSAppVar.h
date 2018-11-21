#pragma once

#include "PBS/SketchedQuery.h"
#include "PBS/MotionDBforPBS.h"

class PBSAppVar
{
private:
	PBSAppVar();
	~PBSAppVar();
	static PBSAppVar *singleton_;
	PBS::MotionDBforPBS *motion_db_;

public:
	PBS::SketchedQuery::BodyJointName selected_body_joint;


	const PBS::MotionDBforPBS *motino_db() const { return motion_db_; }

	bool flag_plus_IK_;
	bool flag_IK_only_;
	bool flag_hide_char_;

public:
	static PBSAppVar* getSingleton();
	static void DestroySingleton();
};

