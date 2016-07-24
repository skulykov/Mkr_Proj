#ifndef MKR_MY_UTILS_H_INCLUDED
#define MKR_MY_UTILS_H_INCLUDED

#include <unordered_map>
#include <string>
#include <cstdarg>
#include <sstream>
#include <vector>


//#define toLowerCase(s) std::transform(s.begin(), s.end(), s.begin(), ::tolower)
#include <algorithm>
#define toUpperCase(s) std::transform(s.begin(), s.end(), s.begin(),	[](char c) { return (::toupper(c)); });
#define toLowerCase(s) std::transform(s.begin(), s.end(), s.begin(),	[](char c) { return (::tolower(c)); });



using namespace std;

string Int2Str(int v);
string Double2Str(double v);
string strToUpper(const char *s);
int Str2Int(std::string myString);
class stdH
{
public:
#if 0
	template <typename K, typename V>
	static std::vector<K> GetKeys(const std::map<K, V>& m) {
		std::vector<K> result;
		result.reserve(m.size()); // For efficiency

		for (typename std::map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			result.push_back(itr->first);

		return result;
	}

	template <typename K, typename V>
	static std::vector<V> GetValues(const std::map<K, V>& m) {
		std::vector<V> result;
		result.reserve(m.size()); // For efficiency

		for (typename std::map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			result.push_back(itr->second);

		return result;
	}
	template <typename K, typename V>
	static  bool containsKey(const map<K, V>& m, K key) //http://stackoverflow.com/questions/4599792/c-accessing-stdmap-keys-and-values
	{

		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			if (itr->first == key)
				return true;
		return false;

	}
	template <typename K, typename V>
	static  V *mapValByKey(const map<K, V>& m, K key)
	{

		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			if (itr->first == key)
				return (V*)&itr->second;
		return NULL;

	}
#endif
	template <typename VType, typename FType>
	static VType vFind(vector<VType> &src, FType func)
	{
		VType v = 0;
		for (auto itr = src.begin(), end = src.end(); itr != end; ++itr)
		{
			// call the function passed into FirstOrDefault and see if it matches
			if (func(*itr))
			{
				v = (*itr);
				break;
			}
		}

		return v;
	}

	template <typename VType, typename FType>
	static VType FirstOrDefault(vector<VType> &src, FType func)
	{
		VType v = 0;
		for (auto itr = src.begin(), end = src.end(); itr != end; ++itr)
		{
			// call the function passed into FirstOrDefault and see if it matches
			if (func(*itr))
			{
				v = (*itr);
				break;
			}
		}

		return v;
	}
	template <typename K, typename V>
	static  bool TryGetValue(unordered_map<K, V> &m, K key, V &defaultV)
	{

		for (auto itr = m.begin(); itr != m.end(); ++itr)
			if (itr->first == key)
			{
				defaultV=(V)&itr->second;
				 return true;
			}
		return false;

	}

	template <typename V>
	static void addAllV(vector<V> *dest, vector<V> *src)
	{
		int len1 = src->size();
		if (len1>0)
		{
			for (int j = 0; j<len1; j++)
			{
				V el = (*src)[j];
				dest->push_back(el);
			}
		}
	}
	template <typename V>
	static void addAll(vector <V>&dest, const vector <V>&src)
	{
		int len1 = src.size();
		if (len1>0)
		{
			for (int j = 0; j<len1; j++)
				dest.push_back(src[j]);
		}
	}
	template <typename V>
	static void addAllAt(int index, vector <V>&dest, const vector <V>&src)
	{
		int len1 = src.size();
		if (len1>0)
		{
			for (int j = 0; j<len1; j++)
				insertItemAtIndex(dest, src[j], index);//dest.push_back(src[j]);
		}
	}
	template <typename V>
	static bool containsItem(vector<V> &v, V x)
	{
		bool rv = false;
		if (v.size() == 0)
			return false;
		if (std::find(v.begin(), v.end(), x) != v.end()) {
			rv = true;// v contains x 
		}
		return rv;
	}
	template <typename V>
	static bool Contains(vector<V> &v, V x)
	{
		int len = v.size();
		if (len == 0)
			return false;
		for (int i = 0; i < len; i++)
		{
			if (x == v[i])
			{
				return true;
			}
		}
		return false;
	}
	template <typename V>
	static bool removeItemAtIndex(vector<V> &v, int index)
	{
		if (v.size() > index) {
			v.erase(v.begin() + index);  //nodes.remove(i);
		}
		return true;
	}
	template <typename V>
	static bool insertItemAtIndex(vector<V> &v, V item, int index)
	{
		typename vector<V>::iterator it1;//vector<V>::iterator it1; !!! //http://stackoverflow.com/questions/11275444/c-template-typename-iterator
		it1 = v.begin();
		it1 += index;
		v.insert(it1, item);
		return true;
	}
	template <typename V>
	static bool reverseV(vector<V> &v)
	{
		reverse(v.begin(), v.end());
		return true;
	}

	//====================string manipulations========================
	//http://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-string-starts-with-a-certain-string-and-convert-a-sub
	static bool contains(string &str, string &substr)
	{
		return str.find(substr) != string::npos;
	}
	static bool startsWith(string &str, string &substr)
	{
		bool rv = false;
		//strncmp(str, substr, strlen(substr));
		int len = substr.length();
		return strncmp(str.c_str(), substr.c_str(), len) == 0;
	}
	static bool endsWith(string &str, string &substr)  //http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c
	{
		if (str.length() >= substr.length()) {
			return (0 == str.compare(str.length() - substr.length(), substr.length(), substr));
		}
		else {
			return false;
		}

	}
	static bool equalsIgnoreCase(string s, string s1)
	{
		toLowerCase(s);
		toLowerCase(s1);
		return s.compare(s1) == 0;
	}
	static bool isEmpty(string s)
	{
		return s == "";
	}

};



//=============================================================================
std::string m_format(const char* fmt, ...);
//std::string mystr = m_format("%s %d %10.5f", "omg", 1, 10.5);

vector<string> split(const string &s, char delim);
std::string string_format(const std::string fmt_str, ...);

#endif