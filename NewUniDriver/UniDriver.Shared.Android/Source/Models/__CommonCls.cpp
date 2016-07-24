#include "CommonCls.h"

//=========================ClsHosEvent=====================
#if 0
ClsHosEvent * ClsHosEvent::deserialize(Json::Value val)
{
	ClsHosEvent *ev = new ClsHosEvent();
	//PrintJSONValue(val);
	ev->BoxID = val["BoxID"].asInt();
	ev->ev = val["Event"].asInt();
	ev->DriverID = val["DriverId"].asString();
	ev->LogTime = val["LogTime"].asString();
	return ev;
}
Json::Value ClsHosEvent::serialize(ClsHosEvent *obj)
{
	Json::Value val;
	if (obj != 0)
	{

		val["BoxID"] = obj->BoxID;
		val["Event"] = obj->ev;
		val["DriverId"] = obj->DriverID;
		val["LogTime"] = obj->LogTime;
	}
	return val;
}
#endif
//=========================ClsLoginMD=====================
Json::Value ClsLoginMD::serialize(ClsLoginMD *_this)
{
	Json::Value val;
	if (_this != 0)
	{
		val["user"]= _this->user;
		val["pass"]= _this->pass;
		val["DrName"]= _this->DrName;
		val["cid"]= _this->cid;
		val["ResourceID"]= _this->ResourceID;

		
	}
	return val;
}

ClsLoginMD* ClsLoginMD::deserialize(Json::Value val)
{
	ClsLoginMD *_this = new ClsLoginMD();
	_this->cid = val["cid"].asInt();
	_this->user = val["user"].asString();
	_this->pass = val["pass"].asString();
	_this->DrName = val["DrName"].asString();
	_this->ResourceID = val["ResourceID"].asInt();
	_this->res = val["res"].asBool();
	
	//var varc = jsonVar[Identifier("driverProf")];
	//this->driverProf.FromJsonVarvar(varc);
	
	Json::Value vehicles = val["vehicles"]; // array
	BoxIdVehicleRow box;
	_this->vehicles=box.deserializeList(vehicles);

	Json::Value drProf = val["driverProf"];
	EmployeeRow emp;
	_this->driverProf = 0;// emp.deserialize(drProf);

	Json::Value conf = val["config"];
	Configuration cf;
	_this->config = cf.deserialize(conf);

	return _this;
}
ClsLoginMD::~ClsLoginMD()
{
	if (driverProf != 0)
		delete driverProf;
	if (config != 0)
		delete config;
	if (vehicles.size() > 0)
	{
		vector<BoxIdVehicleRow*>::iterator it;
		for (it = vehicles.begin(); it != vehicles.end(); ++it)
			delete (*it);
		vehicles.clear();
	}
}
//==================================BoxIdVehicleRow===============
BoxIdVehicleRow *BoxIdVehicleRow::deserialize(Json::Value val)
{
	BoxIdVehicleRow *_this = new BoxIdVehicleRow();
	_this->ID = val["ID"].asInt();
	_this->desc = val["desc"].asString();
	_this->Img = val["Img"].asString();
	_this->weight = val["weight"].asInt();
	_this->flag = val["flag"].asInt();
	_this->VL = val["VL"].asString();
	_this->VLP = val["VLP"].asString();
	return _this;
}
Json::Value BoxIdVehicleRow::serialize(BoxIdVehicleRow *obj)
{
	Json::Value val;
	if (obj != 0)
	{
		val["ID"] = obj->ID;
		val["desc"] = obj->desc;
		val["Img"] = obj->Img;
		val["weight"] = obj->weight;
		val["flag"] = obj->flag;
		val["VL"] = obj->VL;
		val["VLP"] = obj->VLP;
	}
	return val;
}
//=======================================================
#if 0
EmployeeRow *EmployeeRow::deserialize(Json::Value val)
{
	EmployeeRow *_this = new EmployeeRow();
	_this->ID = val["ID"].asString();
	_this->DriverName = val["DriverName"].asString();
	_this->UserName = val["UserName"].asString();
	_this->Email = val["Email"].asString();
	_this->Password = val["Password"].asString();
	_this->License = val["License"].asString();
	_this->Signature = val["Signature"].asString();

	_this->Country = val["Country"].asString();
	_this->Domain = val["Domain"].asString();
	_this->HomeAddress = val["HomeAddress"].asString();
	_this->State = val["State"].asString();
	_this->OrgName = val["OrgName"].asString();
	_this->OrgAddr = val["OrgAddr"].asString();
	_this->TimeZone = val["TimeZone"].asString();

	_this->Cycle =(HOSCYCLE) val["Cycle"].asInt();
	_this->HOSExcept = val["HOSExcept"].asInt();
	_this->ApplyDTS = val["ApplyDTS"].asInt();
	_this->TZ = val["TZ"].asFloat();
	_this->IsSupervisor = val["IsSupervisor"].asBool();
	_this->DLS = val["DLS"].asBool();

	return _this;
}
Json::Value EmployeeRow::serialize(EmployeeRow *obj)
{
	Json::Value val;
	if (obj != 0)
	{
		val["ID"] = obj->ID;
		val["DriverName"] = obj->DriverName;
		val["UserName"] = obj->UserName;
		val["Email"] = obj->Email;
		val["Password"] = obj->Password;
		val["License"] = obj->License;
		val["Signature"] = obj->Signature;

		val["Country"] = obj->Country;
		val["Domain"] = obj->Domain;
		val["HomeAddress"] = obj->HomeAddress;
		val["State"] = obj->State;
		val["OrgName"] = obj->OrgName;
		val["OrgAddr"] = obj->OrgAddr;

		val["TimeZone"] = obj->TimeZone;
		val["Cycle"] = (int)obj->Cycle;
		val["HOSExcept"] = obj->HOSExcept;
		val["ApplyDTS"] = obj->ApplyDTS;
		val["TZ"] = obj->TZ;
		val["IsSupervisor"] = obj->IsSupervisor;
		val["DLS"] = obj->DLS;
	}
	return val;
}
#endif
//========================================================
Configuration *Configuration::deserialize(Json::Value val)
{
	Configuration *_this = new Configuration();
	_this->ScreenLock = val["ScreenLock"].asInt();
	_this->SearchType = val["SearchType"].asString();
	_this->OdoInputFlag = val["OdoInputFlag"].asInt();
	_this->WeightThreshold = val["WeightThreshold"].asInt();
	return _this;
}
Json::Value Configuration::serialize(Configuration *obj)
{
	Json::Value val;
	if (obj != 0)
	{
		val["SearchType"] = obj->SearchType;
		val["ScreenLock"] = obj->ScreenLock;
		val["OdoInputFlag"] = obj->OdoInputFlag;
		val["WeightThreshold"] = obj->WeightThreshold;
		
	}
	return val;
}
