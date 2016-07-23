#ifndef class_Boundary_H
#define class_Boundary_H


#include "MapAlgorithms.h"


using namespace std;


class Boundary
{
	LONG_T boundaryId;
	string name;
	int adminLevel;
	
	
	// not necessary ready rings
	vector<Way*> outerWays;// = new ArrayList<Way>(1);
	vector<Way*> innerWays;// = new ArrayList<Way>(0);
	bool closedWay;
public:
	Boundary(bool closedWay){this->closedWay=closedWay;}
	bool isClosedWay() {
		return closedWay;
	}
	
	void setClosedWay(bool closedWay) {
		this->closedWay = closedWay;
	}
	bool computeIsClosedWay() {
		if (getOuterWays().size() > 0) {
			// now we try to merge the ways until we have only one
			int oldSize = 0;
			while (getOuterWays().size() != oldSize && !getOuterWays().empty()) {
				oldSize = getOuterWays().size();
				mergeOuterWays();
			}
			if (!getOuterWays().empty()) {
				// there is one way and last element is equal to the first...
				vector<Node*>& nodes = getOuterWays()[0]->getNodes();
				closedWay = getOuterWays().size() == 1 && nodes[0]->getId() == nodes[nodes.size() - 1]->getId();
			}
		} else {
			closedWay = false;
		}
		return closedWay;
	}
	void mergeOuterWays() {
		Way *way = getOuterWays()[0];
		vector<Node*>& nodes = way->getNodes();
		if (!nodes.empty()) {
			int nodesSize = nodes.size();
			Node *first = nodes[0];
			Node *last = nodes[nodesSize-1];
			int size = getOuterWays().size();
			for (int i = size-1; i >= 1; i--) {
				//try to find way, that matches the one ...
				Way *anotherWay = getOuterWays()[i];
				if (anotherWay->getNodes().empty()) {
					//remove empty one...
					stdH::removeItemAtIndex<Way*>(getOuterWays(),i );//getOuterWays().remove(i);
				} else {
					if (anotherWay->getNodes()[0]->getId() == first->getId()) {
						//reverese this way and add it to the actual
						stdH::reverseV(anotherWay->getNodes());//Collections.reverse(anotherWay->getNodes());
						stdH::addAll<Node*>(0,way->getNodes(),anotherWay->getNodes());//way->getNodes().addAll(0,anotherWay->getNodes());
						stdH::removeItemAtIndex<Way*>(getOuterWays(),i );//getOuterWays().remove(i);
					} else if (anotherWay->getNodes()[0]->getId() == last->getId()) {
						stdH::addAll<Node*>(way->getNodes(),anotherWay->getNodes());//way->getNodes().addAll(anotherWay->getNodes());
						stdH::removeItemAtIndex<Way*>(getOuterWays(),i );//getOuterWays().remove(i);
					} else if (anotherWay->getNodes()[anotherWay->getNodes().size()-1]->getId() == first->getId()) {
						//add at begging
						stdH::addAll<Node*>(0,way->getNodes(),anotherWay->getNodes());//way->getNodes().addAll(0,anotherWay->getNodes());
						stdH::removeItemAtIndex<Way*>(getOuterWays(),i );//getOuterWays().remove(i);
					} else if (anotherWay->getNodes()[anotherWay->getNodes().size()-1]->getId() == last->getId()) {
						stdH::reverseV(anotherWay->getNodes());//Collections.reverse(anotherWay->getNodes());
						stdH::addAll<Node*>(way->getNodes(),anotherWay->getNodes());//way->getNodes().addAll(anotherWay->getNodes());
						stdH::removeItemAtIndex<Way*>(getOuterWays(),i );//getOuterWays().remove(i);
					}
				}
			}
		} else {
			//remove way with no nodes!
			stdH::removeItemAtIndex<Way*>(getOuterWays(),0 );//getOuterWays().remove(0);
		}
	}
	bool containsPoint(LatLon_S *point) {
		return containsPoint(point->getLatitude(), point->getLongitude());
	}
	
	bool containsPoint(double latitude, double longitude) {
		int intersections = 0;
		for(int i=0;i<outerWays.size();i++/*Way w : outerWays*/){
			Way *w=outerWays[i];
			for(int i=0; i<w->getNodes().size() - 1; i++){
				if(MapAlgorithms::ray_intersect_lon(w->getNodes()[i], w->getNodes()[i+1], latitude, longitude) != -360.0){
					intersections ++;
				}
			}
		}
		for(int i=0;i<innerWays.size();i++/*Way w : outerWays*/){
			Way *w=innerWays[i];
			for(int i=0; i<w->getNodes().size() - 1; i++){
				if(MapAlgorithms::ray_intersect_lon(w->getNodes()[i], w->getNodes()[i+1], latitude, longitude) != -360.0){
					intersections ++;
				}
			}
		}

		return intersections % 2 == 1;
	}
	
	LatLon_S *getCenterPoint(){
		vector<Node*> points;// = new ArrayList<Node>();
		for(int i=0;i<outerWays.size();i++/*Way w : outerWays*/){
			Way *w=outerWays[i];
			stdH::addAll<Node*>(points,w->getNodes());
		}
		for(int i=0;i<innerWays.size();i++/*Way w : outerWays*/){
			Way *w=innerWays[i];
			stdH::addAll<Node*>(points,w->getNodes());
		}
		return __MapUtils_getWeightCenterForNodes(points);
	}

	vector<Way*> &getOuterWays() {
		return outerWays;
	}
	
	vector<Way*>& getInnerWays() {
		return innerWays;
	}

	//-----------------------------
	void setOuterWays(Way *w) {
		outerWays.push_back(w);
	}

	void setInnerWays(Way *w) {
		innerWays.push_back(w);
	}
	//----------------------------
	
	LONG_T getBoundaryId() {
		return boundaryId;
	}
	
	void setBoundaryId(LONG_T boundaryId) {
		this->boundaryId = boundaryId;
	}

	string getName() {
		return name;
	}

	void setName(string name) {
		this->name = name;
	}

	int getAdminLevel() {
		return adminLevel;
	}

	void setAdminLevel(int adminLevel) {
		this->adminLevel = adminLevel;
	}
	
};  

#endif  

