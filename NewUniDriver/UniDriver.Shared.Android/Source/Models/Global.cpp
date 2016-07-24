#include "Global.h"
#include "db_interface.h"



Global* Global::_instance = 0;
int Global::GetDID() 
{
	/*if (this->pL != 0)
		return pL->resourceid();
	else
		return -1;*/
	return did;
}
int Global::GetCID()
{
	return cid;
}
void Global::SetRegister(PEmployeeRow *pE)
{

}
void Global::SetLogin(ClsLoginMD *pL) 
{ 
	//this->pL = pL;
	this->did = pL->resourceid();
	this->DrName = pL->drname();
	this->cid = pL->cid();
	string DID = GetDriverID();
	//store login response to DB
	
	PBoxIdVehicleRowList *vList = pL->mutable_vehicles();
	if (vList != 0)
	{
		int len = vList->vitem_size();
		m_printf("SetLogin ..DriverName:%s  vehicles:%d", this->DrName.c_str(), len);

		if (len > 0)
		{   /*
			for (int i = 0; i < len; i++)
			{
				PBoxIdVehicleRow *vv = vList->mutable_vitem(i);
				m_printf("SetLogin ..Vehicle Desc:%s ", vv->desc().c_str());
			}*/
			EmployeeDB::Get()->SaveVehicleList2DB(vList, DID);
		}
	}//if (vList != 0)
	if(pL->has_driverprof())
	{
		PEmployeeRow emp = pL->driverprof();
		m_printf("SetLogin ..DriverProf Name:%s ", emp.drivername().c_str());

		EmployeeRow *_prof = new EmployeeRow();
		_prof->CopyFrom(emp);
		_prof->SetAutoLogin(true);
		_prof->set_domain(Int2Str(this->cid));
		UpdateCurrentProfile(_prof);
	}
}
void Global::CleanUp()
{
	did = 0;
	cid = 0;
	DrName = "";
}
string Global::GetDriverID()
{
#ifdef _TEST_
	return "1000";
#endif
	int v = GetDID();
	char buf[32] = { 0 };
	sprintf(buf, "%d", v);  //itoa is not supported
	return string(buf);
}
string Global::GetDriverName()
{
	/*if (this->pL != 0)
		return pL->drname();
	else
		return "";*/
	return DrName;
}

EmployeeRow *Global::GetCurrentProfile() 
{
	if (this->prof == 0)
	{
		std::string id = GetDriverID();
		EmployeeDB *db = EmployeeDB::Get();
		if (db != 0)
		{
			this->prof = db->GetEmployeeById(id);
		}
	}
	return this->prof;
}
void Global::SetCurrentProfile(EmployeeRow *_prof)
{
	if (this->prof != 0)
		delete this->prof;
	this->prof = _prof;
	if(_prof == NULL)
		CleanUp();
	else
	{
		this->cid = Str2Int(_prof->domain());
		this->did = Str2Int(_prof->id());
		this->DrName = _prof->drivername();
	}
}
//this call can come from two places 1. JniHos 2. From server
int Global::UpdateCurrentProfile(EmployeeRow *_prof)
{
	SetCurrentProfile(_prof);
	EmployeeDB *db = EmployeeDB::Get();
	if (db != 0)
	{
		db->SaveEmployee(this->prof);
	}
	return 1;
}
