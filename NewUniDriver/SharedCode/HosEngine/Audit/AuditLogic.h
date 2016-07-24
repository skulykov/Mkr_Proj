#ifndef MKR_AUDITLOGIC_H_INCLUDED
#define MKR_AUDITLOGIC_H_INCLUDED

#define _USE_MATH_DEFINES
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>

#include "Definitions.h"  // it contains #include "Definitions.h"
#include "Utils.h"
#include "HOSAgent.h"
#include "TimeUtil.h"


namespace HosEngine
{
	class HourCalculator;
	class OffdutySession
	{
	public:
		DateTime start = DateTime::Now().Date();// Today;
		DateTime stop = DateTime::Now().Date();// Today;
		TimeSpan ts() { return (stop - start); }
	
		DutyStatus prev;
		int StopAction = 0;
		DutyStatus current;
				
		std::string ToString();
		//OffdutySession *Clone();
	};

	class AuditLogic
	{
	private:
		   HourCalculator *hc;
			public:
				static int TRIPINSPECTION;
				static int PERSONALUSE;
				static int FUELFORM;
				static int REFUEL;
				static int OILFIELDWAIT;
				static int ONTRACK;
				static int OffDuty;
				static int OnDuty;
				static int Driving;
				static int Sleeping;
				static int STANDBYOFFTIME;

				static int ManualStartOff;
				static int ManualStartSleep;
				static int ManualStartDriving;
				static int ManualStartOnduty;

				static int ManualEndOff;
				static int ManualEndSleep;
				static int ManualEndDriving;
				static int ManualEndOnduty;

				static int EmergencyUseStart;
				static int EmergencyUseEnd;
				static int PersonalUseEnd;

				static int ThirtyMinutesOffDutyStart;

				static int OffDutyMinutes;

				static bool Stop168hrsReset;
				static int Airmiles150OndutyThreshold;

			private:
				static std::vector<RULEID> CanadianRuleset;
				static std::vector<RULEID> USRuleset;
			public:
				static DateTime EndOffduty;
				EmployeeRow *CurrentAuditDriver = 0;
				bool IsFromTimer = false;
				/*std::vector<TimeLogRow*>*/ PTimeLogRowList * CurrentAuditDriverTimeLogs;
				bool IsUSA = false;
				static bool NeedtoCalculateRecap;
				DateTime  ShiftStart;//Nullable<DateTime> ShiftStart = Nullable<DateTime>();
				int ShiftStartOdometer = 0;
				int maxdriving = 0;

				int maxonduty = 0;
			private:
				
				double homeLatitude = 0; //radius rule
				double homeLogitude = 0; //radius rule

			public:
				const int &getMaxDriving() const;
				const int &getMaxOnduty() const;


			private:
				std::unordered_map<DateTime*, DeferHourRow*> CurrentDefers;  // svk -> DateTime* is a key and must be a pointer !!!

			public:
				AuditLogic(HourCalculator *p);
				
				AuditResult *Audit(AuditRequest *ar, EmployeeRow *&currentAuditDriver, bool isUSA, PTimeLogRowList *currentAuditDriverTimeLogs, double currentLatitude, double currentLogitude, DateTime &reachAirMilesThresholdTime, int qualifyRadiusRule);
			private:
				std::vector<AuditTime*> Audit(std::vector<Timelog*> &data, std::vector<OffdutySession*> &offdutysessions, DateTime date, RULEID rule, std::vector<Cycle*> &cycles, std::vector<RuleExceptions> &ruleExceptions, double currentLatitude, double currentLogitude, DateTime &reachAirMilesThresholdTime, int qualifyRadiusRule);
				int CheckRadiusRule(std::vector<AuditTime*> &result, std::vector<RuleExceptions> &ruleExceptions, double currentLatitude, double currentLogitude, DateTime &reachAirMilesThresholdTime, RULEID ruleId, std::vector<Timelog*> &data, int maxDriving, bool &airmiles150, int maxonduty);
			public:
				double DistanceToCalculateAirMiles(double lat1, double lon1, double lat2, double lon2);
			private:
				DateTime FindShiftStart(std::vector<Timelog*> &data, int resetLimit, DateTime end);
				TimeSpan CalculateHours(std::vector<Timelog*> &data, std::vector<DutyStatus> &requiredStatus, DateTime start, DateTime end);
				AuditTime *Audit_CA_CycleSwitch(std::vector<OffdutySession*> &offduties, std::vector<Timelog*> &data, DateTime date, Cycle *newcycle, Cycle *oldcycle);
			public:
				std::string Audit_CycleSwitch(std::vector<Timelog*> &data, RULEID oldCycle, RULEID newCycle);
			private:
				std::string CheckRestHours(std::vector<Timelog*> &data, int limit);


         //==================USRule_AuditLogic.h======================
		//#include "USRule_AuditLogic.h"
		private:
			std::vector<AuditTime*> AuditUS(std::vector<Timelog*> &data, int drivinglimit, int ondutylimit, RULEID rule);
			int CheckOnDutyHoursLimitCount(std::vector<Timelog*> &data, int ondutylimit, RULEID rule, DateTime startTime, int days);
			DateTime USFindShiftStartBySleeper(std::vector<Timelog*> &data, int sleeplimit, TimeSpan &lastslpduration, std::vector<OffdutySession*> &sleeps);
			AuditTime *Audit_US_Shift_Driving(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit, std::vector<OffdutySession*> &sleeps);

			AuditTime *Audit_US_Shift_OnDuty(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit, std::vector<OffdutySession*> &sleeps);
			//=========================Added from AuditCA_Rules========================================
			AuditTime *Audit_Recap(std::vector<RecapRowAudit*> &recaps, std::vector<OffdutySession*> &offduties, RULEID rule, DateTime date, std::vector<Timelog*> &data);
			AuditTime *Audit_CA_70Hour_1(std::vector<Timelog*> &data, DateTime date, int limit);
			AuditTime *Audit_CA_70Hour_2(std::vector<Timelog*> &data, DateTime date, int limit);
			void Audit_DeferredRules(std::vector<Timelog*> &data, std::vector<OffdutySession*> &offduties, std::vector<AuditTime*> &result, DateTime dayStart);
			DeferHourRow *FindExistingDeferHour(DateTime date);


			//===========================CARule_AuditLogic.h======================================================================================================
	
	private:
		std::vector<OffdutySession*> MergeOffduty(std::vector<Timelog*> &data, DateTime end);
		std::vector<OffdutySession*> MergeOffduty(std::vector<Timelog*> &data, DateTime end, bool mergeSleepOff, DateTime endDT);
		//Find workshift start
		DateTime CA_FindShiftStartByOffduty(std::vector<Timelog*> &data);
		DateTime US_FindShiftStartByOffduty(std::vector<Timelog*> &data);
		DateTime FindShiftStartBySleeper(std::vector<Timelog*> &data, int sleeplimit, TimeSpan &lastslpduration);
		TimeSpan Calculate_TotalOff_InDate(std::vector<OffdutySession*> &todaylist, DateTime searchDate, DateTime dayend, OffdutySession *consectiveoff);
		std::vector<OffdutySession*> Get_Offduty_Session_TodayList(std::vector<OffdutySession*> &sessions, DateTime date);
		std::vector<AuditTime*> Audit_CA_ShiftRule(std::vector<Timelog*> &data, std::vector<OffdutySession*> &offduties, DateTime &shiftStartDt);
		AuditTime *Audit_CA_Shift_DrivingRule(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit);
		AuditTime *Audit_CA_Shift_OnDutyRule(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit);
		AuditTime *Audit_CA_Shift_16HourRule(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit, TimeSpan lastslp);
		std::vector<AuditTime*> Audit_CA_DayRule(std::vector<Timelog*> &data, double drivinglimit, double ondutylimit, DateTime dayStart);
		DeferHourRow *CheckPrevDayDeferCondition(std::vector<OffdutySession*> &offduties, std::vector<Timelog*> &data, double offdutylimit, Cycle *cycle, DateTime dayStart);
		TimeSpan FindOffDutyToEnd();

		AuditTime *Audit_CA_Offduty_DayRule(std::vector<OffdutySession*> &offduties, std::vector<Timelog*> &data, double offdutylimit, Cycle *cycle, DateTime dayStart);

		TimeSpan GetTotalOffdutyInDate(std::vector<OffdutySession*> &offduties, DateTime start, DateTime end, DateTime &timemark, double &corehours, DateTime &corehoursStop);
		TimeSpan GetTotalDrivingTimeInDayRange(std::vector<Timelog*> &data, DateTime start, DateTime end);

		bool CheckDeferDay1New(std::vector<Timelog*> &data, DateTime date, Cycle *cycle);


		bool CheckDeferDay1(std::vector<OffdutySession*> &offduties, std::vector<Timelog*> &data, DateTime date, Cycle *cycle, double dfhours, bool isToday);

		std::vector<AuditTime*> Audit_CA_North(std::vector<Timelog*> &data, int drivinglimit, int ondutylimit, int elapslimit);
		std::vector<OffdutySession*> Audit_CA_MergeSleep(std::vector<Timelog*> &data, DateTime end);



		//=========================================Recap_AuditLogic======================================
		private:
			std::vector<RecapRowAudit*> CalculateRecap(AuditRequest *ar, RULEID &rule, std::vector<RuleExceptions> &ruleException);

			RecapRowAudit *ApplyRule(AuditRequest *ar, Timelog *lasttl, RULEID &rid, std::vector<RuleExceptions> &ruleException);

			std::vector<RuleExceptions> GetRuleExceptions(int rule);

			RULEID FindApplyingRule(Timelog *tl, std::vector<Cycle*> &cyclelist, std::vector<RuleExceptions> &ruleException);
			void Calculate(Timelog *lasttl, DateTime tl, DateTime today, RecapRowAudit *&rr, TimeSpan &offtime, int resettime, DateTime &offDutyStart, bool isUSA, std::vector<Timelog*> &data, std::vector<DateTime> &previousLogs);

			void CalculateExt(Timelog *lasttl, DateTime tl, DateTime today, TimeSpan &offtime, int resettime, DateTime &offDutyStart, bool isUSA, std::vector<Timelog*> &data, std::vector<DateTime> &previousLogs);

			//Check if current cycle start is 168 hours after previous cycle start
			bool CheckPreviousCycleStart(std::vector<Timelog*> &data, DateTime &offDutyStart, DateTime offDutyEnd, int resettime, std::vector<DateTime> &previousLogs);

			RecapRowAudit *US607(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);
			RecapRowAudit *US708(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);
			RecapRowAudit *US607North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);
			RecapRowAudit *US708North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);

			RecapRowAudit *USOilfield(std::vector<Timelog*> &data, DateTime date);

			RecapRowAudit *USTexas(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);

			RecapRowAudit *USCalifornia(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);

			RecapRowAudit *USFlorida707(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);

			RecapRowAudit *USFlorida808(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);




			RecapRowAudit *CA707(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);
			RecapRowAudit *CA12014(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);
			RecapRowAudit *CA707North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);
			RecapRowAudit *CA12014North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException);
			RecapRowAudit *Cycle_Func(std::vector<Timelog*> &data, DateTime date, int days, int limit, int resettime, const std::string &cycle, bool isUSA);

			void TestCycle();

			RecapRowAudit *CAOilfield(std::vector<Timelog*> &source, DateTime date);


	};  //class AuditLogic

// =========== Helper functions==================
	bool IsUSRule(RULEID rule);
	bool IsCARule(RULEID rule);

	std::string Rule2Str(int rule);
	std::string RuleT2Str(int ruletype);

	Timelog *GetLastOnDutyTlr(std::vector<Timelog*> &data, DateTime &startTime);
	Timelog *GetLastOffDutyOrSleepTlr(std::vector<Timelog*> &data, DateTime &startTime, DateTime &endTime);
}

#endif

