#pragma once

#include "PBS/BodyPoint.h"
#include <vector>



namespace PBS
{

class Query
{
public:
	enum {VERTICAL, HORIZONTAL};	// Translation Type.

	Query(void);
	~Query(void);

	void clear();

	std::vector< BodyPoint > body_points;

	int translation_type;
};

};