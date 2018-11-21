#include "PBS/Query.h"

namespace PBS
{

Query::Query(void)
{
	translation_type = HORIZONTAL;
}


Query::~Query(void)
{
}


void
Query::clear()
{
	body_points.clear();
}


};