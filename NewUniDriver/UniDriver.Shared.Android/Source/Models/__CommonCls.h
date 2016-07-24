#ifndef MKR_COMMONCLS_H_INCLUDED
#define MKR_COMMONCLS_H_INCLUDED
#include "JsonHelper.h"
#include "Definitions.h"
using namespace std;

using namespace HosEngine;

#if 0
enum HOSCYCLE
{
	Canadian_70hr_7day = 6,
	Canadian_120hr_14day = 7,
	Canadian_Oil_Field = 18,
	US_60hr_7day = 10,
	US_70hr_8day = 11,
	US_Oil_Field = 20,
	NONE = 19,

	//new rules for Kiewit  -> 
	US_Texas = 30,
	US_California = 31,  //intra state rules
	//US_NewYork=32,
	US_Florida_707 = 32,
	US_Florida_808 = 33,
	US_Alaskan_707 = 34,
	US_Alaskan_808 = 35

};

class ClsHosEvent :public BaseJsonCls<ClsHosEvent>
{
public:
	int BoxID = -1;
	int ev = -1;
	string LogTime = "";
	string DriverID = "";

	ClsHosEvent *deserialize(Json::Value val);
	Json::Value serialize(ClsHosEvent *obj);
	
};



class EmployeeRow : public BaseJsonCls<EmployeeRow>
{
public:
	string ID = "";
	string DriverName = "";
	string UserName = "";
	string Email = "";
	string Password = "";
	string License = "";
	string Country = "";

	string Signature = "";
	string Domain = "";                            // the same as ClientID
	string HomeAddress = "";
	string State = "";
	string OrgName = "", OrgAddr = "";
	string TimeZone = "";


	HOSCYCLE Cycle = HOSCYCLE::Canadian_70hr_7day;
	int HOSExcept = 0, ApplyDTS = 0;//applyDTS - apply Day Time Savings
	float TZ/*TimeZone*/ = 0;

		
	bool IsSupervisor = false;
	bool DLS = true;   // DayLightSavings

	bool haveSent = false;
	
	EmployeeRow *deserialize(Json::Value val);
	Json::Value serialize(EmployeeRow *obj);

};
#endif
class BoxIdVehicleRow:public BaseJsonCls<BoxIdVehicleRow>
{
public:
	int ID/*BoxId*/ = -1;
	uint64_t mTS = 0;   // current time in milliseconds since Epoch
	string desc/*Description*/ = "";
	string Img = "";//photo="";
	int weight = 0;
	int flag/*vehicleFlag*/ = 0;   // 0-vehicle, 1 - attachment
	string VL/*VehicleLicense*/ = "", VLP/*VehicleLicenseProvince*/ = "";

	BoxIdVehicleRow(int boxId, string description)
	{
		this->ID = boxId;
		this->desc = description;
	}
	void SetWeight(int v)
	{
		weight = v;
	}
	void SetVehicleFlag(int v)
	{
		flag = v;
	}
	int GetVehicleFlag()
	{
		return flag;
	}
	void SetLicense(string lic, string licProv)
	{
		VL = lic;
		VLP = licProv;
	}
	BoxIdVehicleRow(){
	}
	~BoxIdVehicleRow()
	{
	}
	BoxIdVehicleRow *deserialize(Json::Value val);
	Json::Value serialize(BoxIdVehicleRow *obj);
	
};
class Configuration :public BaseJsonCls<Configuration>
{
public :
	int inspHD = 0, inspHA = 0, imagesLimit = 0;
	int violationthreshold = 0;  // HOS violation threshold 
	int login = 0;  //  0- inspection screen goes first after login, 1 - HOS
	bool _scan_flag = false;
	string SearchType = ""; // SearchType is delimited by ','
	int ScreenLock = 0;   // if 1 then Lock the HOS screen ( do not allow to change driver status)
	int OdoInputFlag = 0;  //0 - do not allow manual Odometer modification
	int WeightThreshold = 0;  // threshold for vehicle weight 
	// if vehicle weight is below this weightthreashold ignore ECM data 

	~Configuration() {}

	Configuration *deserialize(Json::Value val);
	Json::Value serialize(Configuration *obj);
};
class ClsLoginMD:public BaseJsonCls<ClsLoginMD>
{
public :
	string user;
	string pass;
	int cid;
	bool res;

	int ResourceID=-1; // DriverID
	string DrName="";  // DriverName

	std::vector<BoxIdVehicleRow*> vehicles;
	EmployeeRow *driverProf=0;
	Configuration *config=0;
	ClsLoginMD()
	{
		reset();

	}

	ClsLoginMD(string user, string pass, int rid)
	{
		reset();
		this->user = user;
		this->pass = pass;
		this->ResourceID = rid;
	}
	~ClsLoginMD();
	ClsLoginMD *deserialize(Json::Value val);
	Json::Value serialize(ClsLoginMD *obj);
private:
	void reset()
	{
		driverProf = 0;
		config = 0;
		user = "DDD1";
		pass = "UUU2";

		cid = 234;
		res = true;
		
		//pois=null;
		ResourceID = 0;
		DrName = "";
		//driverProf = 0;
		//config = 0;
		//vehicles = 0;
	}
};




#endif //#ifndef MKR_COMMONCLS_H_INCLUDED