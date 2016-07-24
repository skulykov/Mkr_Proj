#ifndef HourCalculator_H
#define HourCalculator_H

#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "data/Definitions.h"

#include "data/HOSAgent.h"
#include "data/TimeUtil.h"
#include "data/Utils.h"

#include "audit/AuditLogic.h"

#include "my_time.h"
#include "my_utils.h"
namespace HosEngine
{
	typedef void(*AddDeferHour_CB)(DeferHourRow *dhr, std::string driverId);
	typedef DeferHourRow* (*FindDeferHourByDateEvent_CB)(DateTime dt, std::string driverId);
	typedef void(*UpdateRecapEvent_CB)(PRecapRow *rr, std::string driverId);

			class HoursDetail
			{
			public:
				std::string Name = "";
				double Available = std::numeric_limits<double>::max();
				double DayUsed = std::numeric_limits<double>::lowest();
				double ShiftUsed = std::numeric_limits<double>::lowest();

				HoursDetail(const std::string rule);
				~HoursDetail();
			};
			/// <summary>
			/// Calculate audit hours
			/// </summary>
//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in native C++:
//ORIGINAL LINE: [Description("AuditEngine is to calculate audit hours.")] public class HourCalculator
			class HourCalculator
			{
			public:
				
				AddDeferHour_CB  add_dhr_cb = nullptr;
				AddDeferHour_CB  del_dhr_cb = nullptr;
				FindDeferHourByDateEvent_CB FindDeferHourByDateEvent = nullptr;
				UpdateRecapEvent_CB UpdateRecapEvent = nullptr, AddRecapEvent=nullptr;


				int AvaliableOnDutyMinutes = std::numeric_limits<int>::max();
				std::string AvaliableOnDutyRule = "";
				std::string AvaliableOnDutyRuleDescription = "";
				bool EmergencyStart = false;
				int AvaliableDrivingMinutes = std::numeric_limits<int>::max();
				std::string AvaliableDrivingRule = "";
				std::string AvaliableDrivingRuleDescription = "";

				DateTime ShiftStart ;
				//For testing purpose, will be remove
				//static DateTime CurrentDatetime;
				//

				int maxDriving = 0;
				int maxOnduty = 0;
				int maxCycle = 0;
				int avaliableCycle = 0;

			private:
				void InitializeAlailValues();

				EmployeeRow *CurrentAuditDriver = nullptr;
				/*std::vector<TimeLogRow*>*/ PTimeLogRowList *CurrentAuditDriverTimeLogs;
				PRecapRowList* CurrentAuditDriverRecaps;
				PRuleSelectionHistoryRowList* CurrentAuditRules;

				
				std::vector<std::string> NorthenState;
				std::vector<std::string> NorthenStateUS;
				std::vector<std::string> Canadian;

				

			public:
				//AuditResultQuick *quickAuditResult=nullptr;  // do we use it???
				bool IsDoingAudit;
			private:
				bool IsUSA = false;

			public:
				int ShiftStartOdometer = 0;
				double CurrentLatitude = 0; //radius rule
				double CurrentLogitude = 0; //radius rule
				DateTime ReachAirMilesThresholdTime ; //radius rule
				int QualifyRadiusRule = -1; //radius rule
				/*static*/ std::unordered_map<int, int> defaultSateRuleWeight;
				
				const int &getMaxDriving() const;
				const int &getMaxOnduty() const;
				const int &getMaxCycle() const;
				const int &getAvaliableCycle() const;
				/// <summary>
				/// Calculate audit hours
				/// </summary>
				/// <param name="currentAuditDriverTimeLogs">Driver timelogs data</param>
				/// <param name="currentAuditDriverRecaps">Driver recap data</param>
				/// <param name="currentAuditRules">Driver rule selection history</param>
				/// <param name="currentAuditDriver">Driver info</param>
//C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in native C++:
//ORIGINAL LINE: [Description("Pass driver timelog, recap, rule selection, driver info and subscribe events.")] public void SetHourCalculator(List<TimeLogRow> currentAuditDriverTimeLogs, List<RecapRow> currentAuditDriverRecaps, List<RuleSelectionHistoryRow> currentAuditRules, EmployeeRow currentAuditDriver)
				void SetHourCalculator(PTimeLogRowList *currentAuditDriverTimeLogs, PRecapRowList* currentAuditDriverRecaps, PRuleSelectionHistoryRowList* currentAuditRules, EmployeeRow *currentAuditDriver);

				std::unordered_map<std::string, HoursDetail*> HoursBalance;
				std::unordered_map<std::string, HoursDetail*> ForcastBalance;

				HourCalculator();
				~HourCalculator();

				int GetDefaultStateRule(RULEID ruleid);
				AuditJurisdiction ConvertJurisdiction(std::string &state);

				
			private:
				AuditRequest *PrepareData(AuditJurisdiction region);
				TimeSpan ProcessForcastResult(AuditResult *result);


				TimeSpan ProcessResult(AuditResult *result);
				
			public:
				
				TimeSpan CalculateHours();

				void AddDeferHour(DeferHourRow *dhr, const std::string &driverId, std::unordered_map<DateTime*, DeferHourRow*> &defers);

				void DeleteDeferHour(DeferHourRow *dhr, const std::string &driverId, std::unordered_map<DateTime*, DeferHourRow*> &defers);

				DeferHourRow *FindDeferHourByDate(DateTime date, const std::string &driverId, std::unordered_map<DateTime*, DeferHourRow*> &defers);

				void UpdateRecap(PRecapRow *recapRow, const std::string &driverId);

				void AddRecap(PRecapRow *recapRow, const std::string &driverId);

			};

}
#endif
