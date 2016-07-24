#ifndef MKR_JSONHELPER_H_INCLUDED
#define MKR_JSONHELPER_H_INCLUDED

#include "MkrHeader.h"

using namespace std;

template <class A_Type>
class BaseJsonCls
{
public:
	string serializeList(vector<A_Type*> &arr)
	{

		Json::Value Root;
		if (arr.size() > 0)
		{
			int size = arr.size();
			for (int i = 0; i<size; i++)
			{
				Root.append(this->serialize(arr[i]));
			}
		}
		Json::StyledWriter writer;
		string output = writer.write(Root);
		return output;
	}

	virtual Json::Value serialize(A_Type *obj) = 0;   //

	string toJsonString(A_Type *obj)
	{
		Json::Value val = serialize(obj);
		Json::StyledWriter writer;
		string output = writer.write(val);
		return output;
	}

	int deserializeList(string &strJson, vector<A_Type*> &arr)
	{
		int rv = -1;
		Json::Value root;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse(strJson, root);
		if (parsingSuccessful)
		{
			rv = deserializeList(root,arr);
		}
		return rv;
	}
	int deserializeList(Json::Value &root, vector <A_Type*> &arr)
	{
		
		for (Json::ValueIterator itr = root.begin(); itr != root.end(); itr++) {
			// Print depth. 

			Json::Value obj = *itr;
			if (obj != 0)
			{
				A_Type *ev = deserialize(obj);
				arr.push_back(ev);
			}
		}
		return 0;
	}
	virtual A_Type *deserialize(Json::Value val) = 0;

	A_Type *toJsonObj(string strJson)
	{
		Json::Value root;
		Json::Reader reader;
		A_Type *ev = 0;
		bool parsingSuccessful = reader.parse(strJson, root);
		if (parsingSuccessful)
		{
			ev = deserialize(root);
		}

		return ev;
	}
};

#endif
