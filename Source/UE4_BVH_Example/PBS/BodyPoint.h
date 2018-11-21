#pragma once

#include "cml/cml.h"

namespace PBS
{

class BodyPoint
{
public:
	BodyPoint(void);
	~BodyPoint(void);

	cml::vector3d point;
	int pm_joint_id;
};


};
