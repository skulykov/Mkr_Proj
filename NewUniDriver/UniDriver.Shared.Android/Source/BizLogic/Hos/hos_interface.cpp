#include "hos_interface.h"



LOGSTATUS get_from_index(int ind)
{
	LOGSTATUS sts = LOGSTATUS::OffDuty;
	switch (ind)
	{	
	case 1:
		sts = LOGSTATUS::Sleeping;
		break;
	case 2:
		sts = LOGSTATUS::Driving;
		break;
	case 3:
		sts = LOGSTATUS::OnDuty;
		break;
	}
	return sts;
}
int process_driver_status(PTimeLogRow *ptlr, bool modify_ts)
{
	int rv = 0;
	string driverId=Global::Get()->GetDriverID();
	LOGSTATUS status = get_from_index(ptlr->event());
	//TimeLogRow * tlr1 = 0;

	TimeLogRow *tlr = new TimeLogRow();
	tlr->set_event((int)status);
	int64_t t = DateTime::Now().getMillis();
	tlr->set_olt(t);
	if (modify_ts)
	{
		tlr->set_logtime(t);  //DateTime::UtcNow();
	}
	else {
		tlr->set_logtime(ptlr->logtime());
	}
	tlr->set_driverid(driverId);
	tlr->set_boxid(ptlr->boxid());
	tlr->set_type(ptlr->type());
	tlr->set_lat(ptlr->lat());
	tlr->set_lon(ptlr->lon());
	tlr->set_addr(ptlr->addr());
	tlr->set_comment(ptlr->comment());
	tlr->set_tlid(ptlr->tlid());
	try {
		rv=TimeLogDB::Get()->SaveTimeLog(tlr);
		//tlr1 = TimeLogDB::Get()->GetLast(driverId);
	}
	catch (Kompex::SQLiteException &e)
	{
		string str = e.GetString();
		LOGE(str.c_str());
		rv= -2;
	}
	catch (std::exception &ex)
	{
		string str = string(ex.what());
		if (!str.empty())
		{
		}
		rv= -3;
	}
	/*
	catch (...)
	{
		string str = "Except";//string(ex.what);
		if (!str.empty())
		{
		}
	}*/
	delete tlr;
	return rv >= 0 ? (int)status : rv;
}
PTimeLogRow *getLastDriverStatus()
{
	string driverId = Global::Get()->GetDriverID();
	return TimeLogDB::Get()->GetLast(driverId);
}

//http://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c
/*void ReplaceStringInPlace(std::string& subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}*/
string GetStringFromCurrentHosCycle()
{
	EmployeeRow *emp= Global::Get()->GetCurrentProfile();
	if (emp != 0)
	{
		int clc=emp->cycle();
		string rv= Cycle2Str((HOSCYCLE) clc);
		//ReplaceStringInPlace
		return rv;
	}
	return "";
}
int UpdateHosCycle(int cycle)
{
	string driverId = Global::Get()->GetDriverID();
	EmployeeDB::Get()->UpdateCycleById(driverId, cycle);
	EmployeeRow *emp = Global::Get()->GetCurrentProfile();
	if (emp != 0)
	{
		emp->set_cycle(cycle);
	}

	return 1;
}
int CheckIfLoginIsNeeded()
{
	LOGE("CheckIfLoginIsNeeded .. ");
	EmployeeRow* emp = EmployeeDB::Get()->GetEmployeeWithAutoLogin();
	if (emp != 0)
	{
		LOGE("CheckIfLoginIsNeeded .. %s flag=%d" ,emp->id().c_str(),emp->IsAutoLogin()?1:0);
		Global::Get()->SetCurrentProfile(emp);

		return 0;
	}
	return 1; // login is needed !!
}

PTimeLogRowList* getTLRList()
{
	string driverId =  Global::Get()->GetDriverID();
	TimeLogDB *db=TimeLogDB::Get();
//	db->DeleteLastRecord();
	PTimeLogRowList* rv=db->GetAllTimeLogs(driverId);
	return rv;
}
PTimeLogRowList* getAllTLRList4Date(int64_t date)
{
	string driverId = Global::Get()->GetDriverID();
	PTimeLogRowList* rv = TimeLogDB::Get()->GetAllTimeLogs4Date(driverId,date);
	return rv;
}
PTimeLogRowList* getAllUnsentTLR()
{
	PTimeLogRowList* rv= TimeLogDB::Get()->GetAllUnSent();
	return rv;
}


//-------------------------------------
PRecapRowList*  getRecapList()
{
	string did = Global::Get()->GetDriverID();
	TimeLogDB *db=TimeLogDB::Get();
	//db->CleanUpRecapTable();
	//std::vector<RecapRow*> rv;// TimeLogDB::Get()->GetAllTimeLogs(driverId);
	/*for (int i = 0; i < 5; i++)
	{
		RecapRow *r = new RecapRow();
		r->set_cycle("cycle-" + Int2Str(i));
		r->date = DateTime::Now();
		r->cycleTotal = 123 + i;
		r->cycleAvailable = 20 + i;
		r->today = 10 + i;
		rv.push_back(r);
	}*/
	return db->GetAllRecapByDriver(did);
}
#ifdef _TEST_
void printRecapList(PRecapRowList* list)
{
	if (list != 0)
	{
		if (list != 0)
		{
			int len = list->list_size();
			for (int i = 0; i < len; i++)
			{
				PRecapRow * pE = list->mutable_list(i);
				if (pE != 0)
				{
					int64_t ts = pE->date();
					string ts_str = DateTime::ToStringFromMillis(ts);
					
					printf("Recap today=%d cycle=%s  ts=%lld dt=%s\r\n", pE->today(), pE->cycle().c_str(), ts, ts_str.c_str());
				}
			}

			string pld = list->SerializeAsString();
			int size = pld.length();
		}
	}
}
void  TestRecapDB()
{
	string did = Global::Get()->GetDriverID();
	TimeLogDB *db = TimeLogDB::Get();
	//db->CleanUpRecapTable();
	std::vector<PRecapRow*> rv;// TimeLogDB::Get()->GetAllTimeLogs(driverId);
	PRecapRow *r = 0;
	/*for (int i = 0; i < 5; i++)
	{
	r = new RecapRow();
	r->set_cycle("cycle-" + Int2Str(i));
	r->set_date(DateTime::Now().getMillis()+i*5*1000);
	r->set_cycletotal(123 + i);
	r->set_cycleavailable(20 + i);
	r->set_today(10 + i);
	rv.push_back(r);
	db->AddRecap(r, did);
	}*/
	PRecapRowList* list= db->GetAllRecapByDriver(did);
	printRecapList(list);

	printf("\r\nUpdate..\r\n");
	r = (PRecapRow*)list->mutable_list(1);
	r->set_cycle("***KKK=1234***");
	db->UpdateRecap(r, did);
	PRecapRowList* list1 = db->GetAllRecapByDriver(did);
	printRecapList(list1);
}
#endif