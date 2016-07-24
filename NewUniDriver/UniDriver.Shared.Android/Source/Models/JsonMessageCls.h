#ifndef MKR_JSONMESSAGECLS_H_INCLUDED
#define MKR_JSONMESSAGECLS_H_INCLUDED
#include "JsonHelper.h"
#include "Definitions.h"
using namespace std;

using namespace HosEngine;

class JsonMessageCls:public BaseJsonCls<JsonMessageCls>
{
public :
	string msg;
	string dN,dId;
	int mT;
	string dt;  //[{"mT":1, "dId" : "admin_3604", "dN" : "Dispatcher", "msg" : "1", "dt" : "2016-05-05T14:51:55.092Z"}]

	
	JsonMessageCls() { reset(); }

	JsonMessageCls(int mT,string did, string name,string msg )
	{
		reset();
		this->mT = mT;
		this->dId = did;
		this->dN = name;
		this->msg = msg;
	}
	~JsonMessageCls();
	JsonMessageCls *deserialize(Json::Value val);
	Json::Value serialize(JsonMessageCls *obj);
private:
	void reset()
	{
		msg="";
		dN="", dId="";
		mT=0;
		dt="";
	}
};




#endif //#ifndef MKR_COMMONCLS_H_INCLUDED