#ifndef MKR_TIMELOGDB_H_INCLUDED
#define MKR_TIMELOGDB_H_INCLUDED

#include <string>
#include <vector>
#include <stdexcept>

#include "BaseDB.h"
#include "Definitions.h"
#include "TimeUtil.h"

#include "my_utils.h"

using namespace HosEngine;
using namespace Kompex;
class TimeLogDB:public BaseDB
{
			private:
				static TimeLogDB *_instance;
				
				
				void CreateTables();
				
			public:
				TimeLogDB() : BaseDB("file_timelog.db")
				{
					CreateTables();
				}
				static TimeLogDB *Get();

			private:
				void CreateTimeLogTable(SQLiteStatement *pStmt);

				//LogStatus<>201  -> do not show deleted events
			public:
				PTimeLogRowList* GetAllTimeLogs(const std::string &DriverID);
				PTimeLogRowList* GetAllTimeLogs4Date(const std::string &DriverID, int64_t date);
				PTimeLogRow*    GetLastBeforeDate(const std::string &DriverID, int64_t date);
				void DeleteLastRecord();
			private:
				PTimeLogRowList* getTTList(const std::string &sql, int _status, bool _all);
				//======================================================

			public:
				PTimeLogRowList* GetAllUnSent();

				void SetHaveSentDeleteOlder();

		//==========================Request for Vehicle info after scan===================
				PTimeLogRowList* GetAllUnSent4Vehicle();

				void SetHaveSentDeleteOlder4Vehicle();
		//================================================================================

				bool UpdateSignedStatus(vector<TimeLogRow *> tlr_list);

			private:
				void SetParams(SQLiteStatement *pStmt, TimeLogRow *timLogRow, bool sent = false);
				TimeLogRow *getLast(string sql);
			public:
				int SaveTimeLog(TimeLogRow *timLogRow);
				bool UpdateDriverStatus(PTimeLogRow *tlr);

				void UpdateAddressInTimeLog(DateTime dt, const std::string &address);
				#if defined(NOT_ONLY_ONE_LOG)

				void SaveTimeLogUpdateFromServer(TimeLogRow *timLogRow);
				#else

				void SaveTimeLogsUpdateFromServer(std::vector<TimeLogRow*> &list);
				#endif
				void DeleteAll();

				void DeleteRow(int id);


				TimeLogRow *GetLast(string driverId/*, LOGSTATUS logStatus*/);

				TimeLogRow *GetLastRecord();
				//-------------------additional--------------------------
				#if defined(NOT_USE)
				TimeLogRow *GetLast(const std::string &driverId, LOGSTATUS *logStatus);
				TimeLogRow *GetLastBeforeDate(const std::string &driverId, DateTime before);

				std::vector<TimeLogRow*> GetAllForDate(DateTime date, const std::string &driverId = "");
				#endif

#pragma region RecapDB
private:
	void CreateRecapTable(SQLiteStatement *pStmt);
public:
	void Recap_SetHaveSentDeleteOlder();
private:
	PRecapRowList* getAllRecap(const std::string &driverId, const std::string &sql);
public:
	PRecapRowList* GetAllRecapByDriver(const std::string &driverId);
	PRecapRowList* GetAllRecapByDriverForCurrentDay(const std::string &driverId);
private:
	void SetParams(SQLiteStatement *pStmt, PRecapRow *rr, const std::string &driverId);
public:
	void AddRecap(PRecapRow *rr, const std::string driverId);
	void CleanUpRecapTable();
private:
	void _doRecap(PRecapRow *rr, const std::string &driverId, const std::string &sql);
public:
	void UpdateRecap(PRecapRow *rr, const std::string driverId);
#pragma endregion
#pragma region class DeferHoursRepository
private:
	void CreateDeferHoursTable(SQLiteStatement *pStmt);
public:
	void DeleteDeferHour(DeferHourRow *dhr, const std::string &driverId);
	DeferHourRow *FindDeferHourByDate(DateTime date, const std::string &driverId);
	void DeferHours_SetHaveSentDeleteOlder();
	void AddDeferHour(DeferHourRow *dhr, const std::string &driverId);
#pragma endregion

#pragma region RuleSelectionHistory
private:
	void CreateHOSRulesTable(SQLiteStatement *pStmt);
	void setParams(SQLiteStatement *pStmt, PRuleSelectionHistoryRow *rsr, const std::string &driverId, bool haveSent);

public:
	void AddRuleSelectionHistory(PRuleSelectionHistoryRow *rsr, const std::string &driverId);
	// we use selectTime as Unique Identifier
	void UpdateRuleSelectionHistoryFromServer(std::vector<PRuleSelectionHistoryRow*> &rlist, const std::string &driverId);

private:
	PRuleSelectionHistoryRow *GetLastBeforeSelectTime(SQLiteStatement *pStmt, const std::string &driverId, DateTime selectTime);
	PRuleSelectionHistoryRow *readRuleRow(SQLiteStatement *pStmt);


	bool _getRuleSelectionHistory(const std::string &sql, PRuleSelectionHistoryRowList* rlist);

	//"DriverId ntext, ruleid int, selectTime datetime, country ntext, HaveSent BOOLEAN); ";
public:
	PRuleSelectionHistoryRowList* GetRuleSelectionHistory(const std::string &driverId);
	PRuleSelectionHistoryRowList* GetUnsentRuleSelectionHistory(const std::string &driverId);
	void RulesHos_SetHaveSentDeleteOlder(const std::string &drId);
	
	PRuleSelectionHistoryRow* GetLastRuleByDriverCountry(string driverId, string country);
#pragma endregion
#pragma region AlertDB
	private:
		void CreateAlertsTable(SQLiteStatement *pStmt);
			public:
				void Alerts_SetHaveSentDeleteOlder();
			private:
				bool getAllAlerts(std::vector<HosAlertRow*> &alerts, SQLiteStatement *p);
			public:
				std::vector<HosAlertRow*> GetAllAlertsByDriver(const std::string &driverId);
				bool isAlertExist(DateTime dt_shift, int al_type, const std::string &dr_rule, const std::string &onduty_rule, const std::string &driverId);
				void AddAlert(HosAlertRow *rr, const std::string &driverId);
#pragma endregion

};

#endif
