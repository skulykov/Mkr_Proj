#ifndef UTILS_H
#define UTILS_H

#include <unordered_map>
#include <list>
#include "cxxext/linked_map.hpp"

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <sstream>


using namespace std;
using namespace cxxext;

class Entity;
class Node;
class Way;
class Relation;


#define toLowerCase(s) transform(s.begin(), s.end(), s.begin(), ::tolower)
#define toUpperCase(s) transform(s.begin(), s.end(), s.begin(), ::toupper)


//http://stackoverflow.com/questions/3450860/check-if-a-stdvector-contains-a-certain-object
/*
Checking if v contains the element x:
if(find(v.begin(), v.end(), x) != v.end()) {
    // v contains x 
} else {
    // v does not contain x 
}
Checking if v contains elements (is non-empty):
if(!v.empty()){
    // v is non-empty 
} else {
    // v is empty 
}
*/
class stdH
{
public:

	//for ************ linked_map ********************************
	template <typename K, typename V>
	static vector<K> LMGetKeys(linked_map<K, V>& m) {
		vector<K> result;
	
		//for (typename linked_map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
		//	result.push_back(itr->first);
		for (auto pair : m) {
			result.push_back(pair.first);
		}
		return result;
	}
	template <typename K, typename V>
	static vector<V> LMGetValues(linked_map<K, V>& m) {
		vector<V> result;
		result.reserve(m.size()); // For efficiency

		for (typename linked_map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			result.push_back(itr->second);

		return result;
	}
	template <typename K, typename V>
	static  bool LMmapValByKey(linked_map<K, V>& m, K key, V&res)
	{

		for (typename linked_map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			if (itr->first == key)
			{
				res = itr->second;
				return true;
			}
		return false;

	}

	//for ************ unordered_map ********************************
	template <typename K, typename V>
	static vector<K> UMGetKeys(const unordered_map<K, V>& m) {
		vector<K> result;
		
		for (typename unordered_map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			result.push_back(itr->first);

		return result;
	}
	template <typename K, typename V>
	static  bool UMmapValByKey(const unordered_map<K, V>& m, K key, V&res)
	{

		for (typename unordered_map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			if (itr->first == key)
			{
				res = itr->second;
				return true;
			}
		return false;

	}
	//---------------------------------------------------------------
	template <typename K, typename V>
	static vector<K> GetKeys(const map<K, V>& m) {
		vector<K> result;
		result.reserve(m.size()); // For efficiency

		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			 result.push_back(itr->first);

		return result;
	}

	template <typename K, typename V>
	static vector<V> GetValues(const map<K, V>& m) {
		vector<V> result;
		result.reserve(m.size()); // For efficiency

		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			 result.push_back(itr->second);

		return result;
	}
	template <typename K, typename V>
	static  bool containsKey(const map<K, V>& m, K key) //http://stackoverflow.com/questions/4599792/c-accessing-stdmap-keys-and-values
	{
		
		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
         if(itr->first == key)
			 return true;
		return false;

	}
	/*template <typename K, typename V>
	static  V *mapValByKey(const map<K, V>& m, K key) 
	{
		
		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
         if(itr->first == key)
		 		 return (V*)&itr->second;
		return NULL;

	}*/
	template <typename K, typename V>
	static  bool mapValByKey(const map<K, V>& m, K key, V&res)
	{

		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			if (itr->first == key)
			{
				res = itr->second;
				return true;
			}
		return false;

	}
	template <typename V>
	static void addAllV(vector<V> *dest,vector<V> *src)
	{
		if (src == 0)
			return;
				int len1=src->size();
				if(len1>0)
				{
					for(int j=0;j<len1;j++)
					{
						V el=(*src)[j];
						dest->push_back(el);
					}
				}
	}
	template <typename V>
	static void addAll(vector <V>&dest,const vector <V>&src)
	{
				int len1=src.size();
				if(len1>0)
				{
					for(int j=0;j<len1;j++)
						dest.push_back(src[j]);
				}
	}

	template <typename V>
	static void addAll2LV(list <V> *dest, vector <V> *src)
	{
		if (src == 0)
			return;
		int len1 = src->size();
		if (len1>0)
		{
			for (int j = 0; j<len1; j++)
				dest->push_back((*src)[j]);
		}
	}

	//remove all elements from A that are contained in B 
	template <typename V>
	static void removeAll(vector <V>&a, const vector <V>&b)
	{
		a.erase(remove_if(begin(a), end(a),
			[&](V x) {return find(begin(b), end(b), x) != end(b); }), end(a));
	}

	template <typename V>
	static void addAll(int index,vector <V>&dest,const vector <V>&src)
	{
				int len1=src.size();
				if(len1>0)
				{
					for(int j=0;j<len1;j++)
						insertItemAtIndex(dest,src[j],index + j);//added (+ j)  !!!!!!!!!!!!!!
				}
	}
	template <typename V>
	static bool containsItem(vector<V> &v,V x)
	{
		bool rv=false;
		if(v.size()==0)
			return false;
		if(find(v.begin(), v.end(), x) != v.end()) {
			rv=true;// v contains x 
		} 
		return rv;
	}
	template <typename V>
	static bool removeItem(vector<V> &v, V &item)
	{
		//v.erase(remove(v.begin(), v.end(), item), v.end());
		auto it = find(v.begin(), v.end(), item);
		if (it != v.end())
		{
			v.erase(it);
			return true;
		}
		return false;
	}
	template <typename V>
	static bool removeItemAtIndex(vector<V> &v,int index )
	{
		if(v.size() > index){
			v.erase (v.begin()+index);  //nodes.remove(i);
		}
		return true;
	}
	template <typename V>
	static bool insertItemAtIndex(vector<V> &v,V item,int index )
	{
		vector<V>::iterator it1;
		it1 = v.begin();
		it1+=index;
		v.insert(it1, item);
		return true;
	}
	template <typename V>
	static bool reverseV(vector<V> &v)
	{
		reverse(v.begin(),v.end());
		return true;
	}

//====================string manipulations========================
	static bool containsV(vector<string> &str, string &substr)
	{
		for (string s : str)
		{
			bool rv= s.find(substr) != string::npos;
			if (rv)
				return true;
		}
		return false;
	}
	//http://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-string-starts-with-a-certain-string-and-convert-a-sub
	static bool contains(string &str, string &substr)
	{
		return str.find(substr)!=string::npos;
	}
	static bool startsWith(string &str, string &substr)
	{
		bool rv=false;
		//strncmp(str, substr, strlen(substr));
		int len=substr.length();
		return strncmp(str.c_str(), substr.c_str(), len)==0;
	}
	static bool endsWith(string &str, string &substr)  //http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c
	{
		 if (str.length() >= substr.length()) {
			return (0 == str.compare (str.length() - substr.length(), substr.length(), substr));
		} else {
			return false;
		}

	}
	static bool equalsIgnoreCase(string s,string s1)
	{
		toLowerCase(s);
		toLowerCase(s1);
		return s.compare(s1)==0;
	}
	static bool isEmpty(string s)
	{
		return s=="";
	}
	
	//--------------------Trim----------------------
	// trim from start
	static string &ltrim(string &s) {
		s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
		return s;
	}

	// trim from end
	static string &rtrim(string &s) {
		s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	static string &trim(string &s) {
		return ltrim(rtrim(s));
	}
	//---------------------Split-----------------------
	static vector<string> &split(const string &s, char delim, vector<string> &elems) {
		stringstream ss(s);
		string item;
		while (getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}


	static vector<string> split(const string &s, char delim) {
		vector<string> elems;
		split(s, delim, elems);
		return elems;
	}


	static bool  isLetterOrDigit(int v)
	{
		return isalpha(v)>0;
	}
};


LatLon_S *__MapUtils_getWeightCenter(vector<LatLon_S*> nodes);
LatLon_S *__MapUtils_getCenter(Entity *e);
LatLon_S *__MapUtils_getWeightCenterForNodes(vector<Node*> nodes);

#endif  //#ifndef DBACCESSOR_H

