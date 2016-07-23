#ifndef _FCNAV_MAP_OBJECTS
#define _FCNAV_MAP_OBJECTS

#include "common.h"
#include "mapObjects.h"

void deleteObjects(std::vector <MapDataObject* > & v)
{
	for(size_t i = 0; i< v.size(); i++)
	{
		delete v.at(i);
	}
	v.clear();
}

#endif /*_FCNAV_MAP_OBJECTS*/
