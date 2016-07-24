#include "hos_events.h"
#include "hos_interface.h"

#include <pthread.h>
//#include <unistd.h>

TimeLogRow * HosEvents_CreateAtMidNight(int64_t date_ms, bool save_to_db, int offset)
{
	int rv = 0;
	string driverId = Global::Get()->GetDriverID();
	LOGSTATUS status = LOGSTATUS::OffDuty;

	TimeLogDB *db = TimeLogDB::Get();
	uint64_t ts_mid_night = DateTime::getMidNightMillis(date_ms);
	PTimeLogRow * ptlr = db->GetLastBeforeDate(driverId, /*(int64_t)(date_ms-24*3600*1000)*/ts_mid_night);
	if (ptlr != 0)
	{
		status = (LOGSTATUS)ptlr->event();
		delete ptlr;
	}

	TimeLogRow *tlr = new TimeLogRow();
	tlr->set_event((int)status);
	tlr->set_logtime(/*DateTime::getMidNightMillis(date_ms)*/ts_mid_night + offset);
	tlr->set_driverid(driverId);
	tlr->set_boxid(-1);
	tlr->set_type((int)TimeLogType::Auto);  //TimeLogType::Auto=1

	/*tlr->set_tlid(51);

	tlr->set_lat(43.555);
	tlr->set_lon(-79.888);*/
	try {
		if (save_to_db)
		{
			rv = db->SaveTimeLog(tlr);
			if (rv == 1)
			{
				/*TimeLogRow * tlr1 = TimeLogDB::Get()->GetLast(driverId);
				if (tlr1 != NULL)
				{
					float lat = tlr1->lat();
					float lon = tlr1->lon();
					if (lat == lon)
					{

					}
				}*/
			}
		}
	}
	catch (Kompex::SQLiteException &e)
	{
		string str = e.GetString();
		LOGE(str.c_str());
		rv = -2;
	}
	catch (std::exception &ex)
	{
		string str = string(ex.what());
		if (!str.empty())
		{
		}
		rv = -3;
	}
	/*
	catch (...)
	{
		string str = "Except";//string(ex.what);
		if (!str.empty())
		{
		}
	}*/

	return tlr;
}

void addTlr(int status_index, DateTime dt)
{
	PTimeLogRow *ptlr = new PTimeLogRow();
	ptlr->set_event(status_index);
	int64_t dd = dt.getMillis();
	ptlr->set_logtime(dd);
	process_driver_status(ptlr, false);
	delete ptlr;
}

//===================Restructure Hos Events ======================
bool isMidNight(PTimeLogRow *tlr)
{
	
	int64_t t = tlr->logtime();
	if ((t % 24 * 3600 * 1000) == 0)
		return true;
	return false;
}


pthread_t restructure_thread = { 0 };
static int _do_restructure(PTimeLogRowList* list,string &did,int64_t t)
{
	if (list != 0&& list->list_size())
	{
		int size = list->list_size();
		
			if (size >= 1)
			{
				PTimeLogRow *tlr = list->mutable_list(0);
				PTimeLogRow *prev_tlr = TimeLogDB::Get()->GetLastBeforeDate(did, t);
				if (tlr != 0 && prev_tlr != 0)
				{
					if ((tlr->type() == 1 || isMidNight(tlr)) && tlr->event() != prev_tlr->event())
					{
						tlr->set_event(prev_tlr->event());
						TimeLogDB::Get()->UpdateDriverStatus(tlr);
						//LOGI("RestructureHosEvents UpdateDriverStatus... \n");
					}
				}
			}
		
	}
	else {
		HosEvents_CreateAtMidNight(t, true, 0);
	}
	return 0;
}
static void *do_restructure(void *context)
{
	//LOGI("RestructureHosEvents Enter threadProc... \n");
	string driverId = Global::Get()->GetDriverID();
	int64_t now = DateTime::Now().getMillis();
	for (int j = 14; j >=0; j--)
	{
		int64_t t = now - 24 * j * 3600 * 1000;
		PTimeLogRowList* list = getAllTLRList4Date(t); //getTLRList();
		_do_restructure(list, driverId, t);
	}
	//LOGI("RestructureHosEvents Enter threadProc...DONE \n");
	return 0;
}
void RestructureHosEvents()
{
	//do_restructure(0);
	
	if (pthread_create(&restructure_thread, NULL, do_restructure, 0) != 0)
	{
		LOGI("ERROR RestructureHosEvents threadProc... \n");
		
	}

}