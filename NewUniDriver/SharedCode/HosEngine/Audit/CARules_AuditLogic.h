#pragma once

#include <string>
#include <vector>
#include <cmath>

using namespace SentinelMobile;
using namespace SentinelMobile::Shared::Auditing;
using namespace SentinelMobile::AuditEngine::Data;
using namespace HOSAuditEngine;
namespace SentinelMobile
{
	namespace Shared
	{
		namespace Auditing
		{
			class AuditLogic
			{
			private:
				class OffdutySession
				{
				private:
					DateTime _start = DateTime::Today;
					DateTime _stop = DateTime::Today;
					TimeSpan _ts = TimeSpan(0);
				public:
					DutyStatus *prev;
					int StopAction = 0;
					DutyStatus *current;
					const DateTime &getstart() const;
					void setstart(const DateTime &value);
					const DateTime &getstop() const;
					void setstop(const DateTime &value);
					const TimeSpan &getts() const;
					virtual std::string ToString() override;
					OffdutySession *Clone();
				};
			private:
				std::vector<OffdutySession*> MergeOffduty(std::vector<Timelog*> &data, DateTime end);
				std::vector<OffdutySession*> MergeOffduty(std::vector<Timelog*> &data, DateTime end, bool mergeSleepOff, Nullable<DateTime> endDT);
				//Find workshift start
				DateTime CA_FindShiftStartByOffduty(std::vector<Timelog*> &data);
				DateTime US_FindShiftStartByOffduty(std::vector<Timelog*> &data);
				DateTime FindShiftStartBySleeper(std::vector<Timelog*> &data, int sleeplimit, TimeSpan &lastslpduration);
				TimeSpan Calculate_TotalOff_InDate(std::vector<OffdutySession*> &todaylist, DateTime searchDate, DateTime dayend, OffdutySession *consectiveoff);
				std::vector<OffdutySession*> Get_Offduty_Session_TodayList(std::vector<OffdutySession*> &sessions, DateTime date);
				std::vector<AuditTime*> Audit_CA_ShiftRule(std::vector<Timelog*> &data, std::vector<OffdutySession*> &offduties, Nullable<DateTime> &shiftStartDt);
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
			};
		}
	}
}
