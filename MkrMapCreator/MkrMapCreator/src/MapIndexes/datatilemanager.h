#ifndef DataTileManager_H
#define DataTileManager_H


#include "../common/oCls.h"
#include <sqlite3.h>


using namespace std;



#pragma region class DataTileManager
template <class T>
class DataTileManager
{
	int zoom;
	
	// * map for objects stores as 'xTile_yTile' -> vector<T>
	map<string, vector<T>*> objects;//map<string, vector<T>> objects = new HashMap<string, vector<T>>();

	////this is a helper to calculate objects.keySet()
	

	void putObjects(int tx, int ty, vector<T> *r){
		/*if(objects.containsKey(evTile(tx, ty))){
			r.addAll(objects.get(evTile(tx, ty)));
		} */
		string s=evTile(tx, ty);
		if(stdH::containsKey<string,vector<T>*>(objects,s))
		{
			vector<T> *v=objects[s];
			stdH::addAllV<T>(r,v);
		}
	}
	string evTile(int tileX, int tileY){
		//return tileX +"_"+tileY;
		char buf[32];
		memset(buf,9,32);
		sprintf(buf,"%d_%d",tileX,tileY);
		string s=string(buf);
		
		return s;
	}

public:
	
	
	
	
	DataTileManager(){
		zoom = 15;
	}
	
	DataTileManager(int z){
		zoom = z;
	}
	
	int getZoom() {
		return zoom;
	}
	
	void setZoom(int zoom) {
		
		this->zoom = zoom;
	}
	
	bool isEmpty(){
		
		/*for(string s : objects.keySet()){
			if(!objects.get(s).isEmpty()){
				return false;
			}
		}*/
		vector<string> v=stdH::GetKeys<string,vector<T>*>(objects);//objects_keySet();
		int len=v.size();
		for(int i=0;i<len;i++)
		{
			string s=v[i];
			vector<T> *vv=objects[s];
			if(vv->size()>0)
				return false;
		}
		return true;
	}
	
	int getObjectsCount(){
		int x = 0;
		/*for(string s : objects.keySet()){
			x += objects.get(s).size();
		}*/
		vector<string> v=stdH::GetKeys<string,vector<T>*>(objects);
		int len=v.size();
		for(int i=0;i<len;i++)
		{
			string s=v[i];
			vector<t> *vv=objects[s];
			x+=vv->size();
		}
		return x;
	}
	
	
	
	vector<T> getAllObjects(){
		vector<T> l ;
		vector<string> keys=stdH::GetKeys<string,vector<T>*>(objects);
		for(int i=0;i<keys.size();i++){
			string s=keys[i];
			stdH::addAll(l,objects[s]);
		}
		return l;
	}
	
	vector<T> getObjects(double latitudeUp, double longitudeUp, double latitudeDown, double longitudeDown) {
		int tileXUp = (int) MapUtils::getTileNumberX(zoom, longitudeUp);
		int tileYUp = (int) MapUtils::getTileNumberY(zoom, latitudeUp);
		int tileXDown = (int) MapUtils::getTileNumberX(zoom, longitudeDown) + 1;
		int tileYDown = (int) MapUtils::getTileNumberY(zoom, latitudeDown) + 1;
		vector<T> result ;//= new ArrayList<T>();
		for (int i = tileXUp; i <= tileXDown; i++) {
			for (int j = tileYUp; j <= tileYDown; j++) {
				putObjects(i, j, result);
			}
		}
		return result;
	}
	
	/**
	 * @depth of the neighbor tile to visit
	 * returns not exactly sorted list, 
	 * however the first objects are from closer tile than last
	 */
	vector<T> *getClosestObjects(double latitude, double longitude, int defaultStep){
		vector<T> *l = null;
		if(isEmpty()){
			return l;//Collections.emptyList();
		}
		int dp = 0;
		
		while (l==null||l->size()==0) {
			l = getClosestObjects(latitude, longitude, dp, dp + defaultStep);
			dp += defaultStep;
		}
		return l;
	}
	
	vector<T> *getClosestObjects(double latitude, double longitude){
		return getClosestObjects(latitude, longitude, 3);
	}
		
	vector<T> *getClosestObjects(double latitude, double longitude, int startDepth, int depth){
		int tileX = (int) MapUtils::getTileNumberX(zoom, longitude);
		int tileY = (int) MapUtils::getTileNumberY(zoom, latitude);
		vector<T> *result = new vector<T>();  // must be a pointer here !!!!!!!!!!!!!!!!!
		
		if(startDepth <= 0){
			putObjects(tileX, tileY, result);
			startDepth = 1;
		}
		
		// that's very difficult way visiting node : 
		// similar to visit by spiral
		// however the simplest way could be to visit row by row & after sort tiles by distance (that's less efficient) 
		
		// go through circle
		for (int i = startDepth; i <= depth; i++) {

			// goes 
			for (int j = 0; j <= i; j++) {
				// left & right
				int dx = j == 0 ? 0 : -1;
				for (; dx < 1 || (j < i && dx == 1); dx += 2) {
					// north
					putObjects(tileX + dx * j, tileY + i, result);
					// east
					putObjects(tileX + i, tileY - dx * j, result);
					// south
					putObjects(tileX - dx * j, tileY - i, result);
					// west
					putObjects(tileX - i, tileY + dx * j, result);
				}
			}
		}
		return result;
	}
	
	
	
	
	string evaluateTile(double latitude, double longitude){
		int tileX = (int) MapUtils::getTileNumberX(zoom, longitude);
		int tileY = (int) MapUtils::getTileNumberY(zoom, latitude);
		return evTile(tileX, tileY);
	}
	
	void unregisterObject(double latitude, double longitude, T object){
		string tile = evaluateTile(latitude, longitude);
		if(objects.containsKey(tile)){
			objects.get(tile).remove(object);
		}
		
	}
	
	string registerObject(double latitude, double longitude, T object){
		string tile = evaluateTile(latitude, longitude);
		if(/*containsKey*/objects.count(tile)==0){
			objects.insert(pair<string, vector<T>*>(tile,new vector<T>()/*new ArrayList<T>()*/));//MUST be a pointer
		}
		//objects.get(tile).add(object);
		vector<T> *v=objects[tile];
		v->push_back(object);
		return tile;
	}

	
	void clear(){
		objects.clear();
	}
	

};
#pragma endregion

#endif  //#ifndef DBACCESSOR_H

