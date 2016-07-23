

#include "oCls.h"
#include <io.h>

//this function should be placed to MapUtils
LatLon_S *__MapUtils_getWeightCenterForNodes(std::vector<Node*> nodes)
{
	int count = nodes.size();	
	if (count==0) {
			return 0;
		}
		double longitude = 0;
		double latitude = 0;
		
		for (int i=0;i<count;i++) {
			Node *n=nodes[i];
			
				longitude += n->getLongitude();
				latitude += n->getLatitude();
			
		}
		
		return new LatLon_S(latitude/count, longitude/count);
}

LatLon_S *__MapUtils_getCenter(Entity *e){
		if(Entity::typeOf(e)== NODE){
			return ((Node*) e)->getLatLon();
		} else if(Entity::typeOf(e)== WAY){
			return __MapUtils_getWeightCenterForNodes(((Way*) e)->getNodes());
		} else if(Entity::typeOf(e)== RELATION){
			vector<LatLon_S*> list;
			
			vector<Entity*> v=((Relation*) e)->getMembers(null); 
			int len=v.size();
			for(int i=0;i<len;i++ ){
				Entity *fe =v[i];
				LatLon_S *c=null;
				// skip relations to avoid circular dependencies
				if(!(Entity::typeOf(e)== RELATION)){
					c = __MapUtils_getCenter(fe); //recursion
				}
				if(c != null){
					list.push_back(c);
				}
			}
			return __MapUtils_getWeightCenter(list);
		}
		return null;
	}

	LatLon_S *__MapUtils_getWeightCenter(vector<LatLon_S*> nodes){
		int size=nodes.size();
		if(size==0){
			return null;
		}
		double longitude = 0;
		double latitude = 0;
		for(int i=0;i<size;i++)
		{
			LatLon_S *n = nodes[i];
			longitude += n->getLongitude();
			latitude += n->getLatitude();
		}
		
		return new LatLon_S(latitude/size, longitude/size);
	}
	
