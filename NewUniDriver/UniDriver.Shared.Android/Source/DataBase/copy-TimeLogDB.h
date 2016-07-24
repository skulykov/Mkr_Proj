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
				void CreateTimeLogTable(Kompex::SQLiteStatement *pStmt);

				//LogStatus<>201  -> do not show deleted events
			public:
				std::vector<TimeLogRow*> GetAllTimeLogs(const std::string &DriverID);
				std::vector<TimeLogRow*> GetAllTimeLogs4Date(const std::string &DriverID, int64_t date);

			private:
				std::vector<TimeLogRow*> getTTList(const std::string &sql, int _status, bool _all);
				//======================================================

			public:
				std::vector<TimeLogRow*> GetAllUnSent();

				void SetHaveSentDeleteOlder();

		//==========================Request for Vehicle info after scan===================
				std::vector<TimeLogRow*> GetAllUnSent4Vehicle();

				void SetHaveSentDeleteOlder4Vehicle();
		//================================================================================

				bool UpdateSignedStatus(vector<TimeLogRow *> tlr_list);

			private:
				void SetParams(Kompex::SQLiteStatement *pStmt, TimeLogRow *timLogRow, bool sent = false);
				
			public:
				int SaveTimeLog(TimeLogRow *timLogRow);
				bool UpdateDriverStatus(TimeLogRow *tlr);

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
	void CreateRecapTable(Kompex::SQLiteStatement *pStmt);
public:
	void Recap_SetHaveSentDeleteOlder();
private:
	std::vector<RecapRow*> getAllRecap(const std::string &driverId, const std::string &sql);
public:
	std::vector<RecapRow*> GetAllRecapByDriver(const std::string &driverId);
	std::vector<RecapRow*> GetAllRecapByDriverForCurrentDay(const std::string &driverId);
private:
	void SetParams(Kompex::SQLiteStatement *pStmt, RecapRow *rr, const std::string &driverId);
public:
	void AddRecap(RecapRow *rr, const std::string &driverId);
private:
	void _doRecap(RecapRow *rr, const std::string &driverId, const std::string &sql);
public:
	void UpdateRecap(RecapRow *rr, const std::string &driverId);
#pragma endregion
#pragma region class DeferHoursRepository
private:
	void CreateDeferHoursTable(Kompex::SQLiteStatement *pStmt);
public:
	void DeleteDeferHour(DeferHourRow *dhr, const std::string &driverId);
	DeferHourRow *FindDeferHourByDate(DateTime date, const std::string &driverId);
	void DeferHours_SetHaveSentDeleteOlder();
	void AddDeferHour(DeferHourRow *dhr, const std::string &driverId);
#pragma endregion

#pragma region RuleSelectionHistory
private:
	void CreateHOSRulesTable(Kompex::SQLiteStatement *pStmt);
	void setParams(Kompex::SQLiteStatement *pStmt, RuleSelectionHistoryRow *rsr, const std::string &driverId, bool haveSent);

public:
	void AddRuleSelectionHistory(RuleSelectionHistoryRow *rsr, const std::string &driverId);
	// we use selectTime as Unique Identifier
	void UpdateRuleSelectionHistoryFromServer(std::vector<RuleSelectionHistoryRow*> &rlist, const std::string &driverId);

private:
	RuleSelectionHistoryRow *GetLastBeforeSelectTime(Kompex::SQLiteStatement *pStmt, const std::string &driverId, DateTime selectTime);
	RuleSelectionHistoryRow *readRuleRow(Kompex::SQLiteStatement *pStmt);


	bool _getRuleSelectionHistory(const std::string &sql, std::vector<RuleSelectionHistoryRow*> &rlist);

	//"DriverId ntext, ruleid int, selectTime datetime, country ntext, HaveSent BOOLEAN); ";
public:
	std::vector<RuleSelectionHistoryRow*> GetRuleSelectionHistory(const std::string &driverId);
	std::vector<RuleSelectionHistoryRow*> GetUnsentRuleSelectionHistory(const std::string &driverId);
	void RulesHos_SetHaveSentDeleteOlder(const std::string &drId);
	
#pragma endregion
#pragma region AlertDB
	private:
		void CreateAlertsTable(Kompex::SQLiteStatement *pStmt);
			public:
				void Alerts_SetHaveSentDeleteOlder();
			private:
				bool getAllAlerts(std::vector<HosAlertRow*> &alerts, Kompex::SQLiteStatement *p);
			public:
				std::vector<HosAlertRow*> GetAllAlertsByDriver(const std::string &driverId);
				bool isAlertExist(DateTime dt_shift, int al_type, const std::string &dr_rule, const std::string &onduty_rule, const std::string &driverId);
				void AddAlert(HosAlertRow *rr, const std::string &driverId);
#pragma endregion

};

#endif
