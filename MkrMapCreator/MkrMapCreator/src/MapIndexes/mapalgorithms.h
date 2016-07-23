#ifndef MapAlgorithms_H
#define MapAlgorithms_H


#include "../common/oCls.h"
#include <math.h>
#include <float.h>


using namespace std;

class MapAlgorithms
{
public:
// try to intersect from left to right
	static double ray_intersect_lon(Node *node, Node *node2, double latitude, double longitude) {
		// a node below
		Node *a = node->getLatitude() < node2->getLatitude() ? node : node2;
		// b node above
		Node *b = a == node2 ? node : node2;
		if (latitude == a->getLatitude() || latitude == b->getLatitude()) {
			latitude += 0.00000001;
		}
		if (latitude < a->getLatitude() || latitude > b->getLatitude()) {
			return -360.0;
		} else {
			if (longitude < /*Math.min*/std::min(a->getLongitude(), b->getLongitude())) {
				return -360.0;
			} else {
				if (a->getLongitude() == b->getLongitude() && longitude == a->getLongitude()) {
					// the node on the boundary !!!
					return longitude;
				}
				// that tested on all cases (left/right)
				double lon = b->getLongitude() -(b->getLatitude() - latitude) * (b->getLongitude() - a->getLongitude()) / (b->getLatitude() - a->getLatitude());
				if (lon <= longitude) {
					return lon;
				} else {
					return -360.0;
				}
			}
		}
	}

};

#endif  //#ifndef MapAlgorithms_H

