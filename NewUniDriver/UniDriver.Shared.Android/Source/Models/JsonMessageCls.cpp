#include "JsonMessageCls.h"

//=========================ClsLoginMD=====================
Json::Value JsonMessageCls::serialize(JsonMessageCls *_this)
{
	Json::Value val;
	if (_this != 0)
	{
		val["mT"]= _this->mT;
		val["dId"]= _this->dId;
		val["dN"]= _this->dN;
		val["msg"]= _this->msg;
		val["dt"]= _this->dt;

		
	}
	return val;
}

JsonMessageCls* JsonMessageCls::deserialize(Json::Value val)
{
	JsonMessageCls *_this = new JsonMessageCls();
	_this->mT = val["mT"].asInt();
	_this->dId = val["dId"].asString();
	_this->dN = val["dN"].asString();
	_this->msg = val["msg"].asString();
	_this->dt = val["dt"].asString();
	
	
	return _this;
}
JsonMessageCls::~JsonMessageCls()
{
	reset();
}
