#include "HOSAgent.h"

#include "TimeUtil.h"
#include "SharedObjects.h"
#include "Definitions.h"

namespace HosEngine
{


bool HOSAgent::_30in_break = false;
DateTime HOSAgent::_30in_dt = DateTime::MinValue;

			void HOSAgent::set30MinBreak(bool v)
			{
				_30in_break = v;
				if (v)
				{
					_30in_dt = TimeUtil::DateTime_Now().AddMinutes(30);
				}
			}

			bool HOSAgent::is30MinBreak()
			{
				return _30in_break;
			}

			TimeSpan HOSAgent::get30MinRemaining()
			{
				DateTime dt1 = _30in_dt;
				DateTime dt2= TimeUtil::DateTime_Now();
				TimeSpan ts = dt1 - dt2;
				return ts;
			}

bool HOSAgent::_personal_driving = false;
float HOSAgent::_personal_use_odometer = 0;
DateTime HOSAgent::_personal_driving_dt;
DateTime HOSAgent::_emegency_driving_dt;

			void HOSAgent::set_personal_driving(bool v, float odo )
			{
				_personal_driving = v;
				if (v)
				{
					_personal_driving_dt = TimeUtil::DateTime_Now(); //.AddMinutes (30);
				}
				_personal_use_odometer = odo;
			}

			bool HOSAgent::is_personal_driving()
			{
				return _personal_driving;
			}

			TimeSpan HOSAgent::get_personal_driving_dtRemaining()
			{
				
				DateTime dt1 = _personal_driving_dt;
				DateTime dt2 = TimeUtil::DateTime_Now();
				TimeSpan ts = dt1 - dt2;
				return ts;
			}

bool HOSAgent::_emergency_driving = false;

			void HOSAgent::set_emergency_driving(bool v)
			{
				_emergency_driving = v;
				if (v)
				{
					_emegency_driving_dt = TimeUtil::DateTime_Now(); //.AddMinutes (30);
				}
			}

			bool HOSAgent::is_emergency_driving()
			{
				return _emergency_driving;
			}

			TimeSpan HOSAgent::get_emergency_driving_dtRemaining()
			{
				DateTime dt1 = _emegency_driving_dt;
				DateTime dt2 = TimeUtil::DateTime_Now();
				TimeSpan ts = dt1 - dt2;
				
				return ts;
			}

int HOSAgent::SelectedDriverStatus =(int) DutyStatus::Offduty;

			void HOSAgent::setSelectedDriverStatus(int status)
			{
				SelectedDriverStatus = status;
			}

			int HOSAgent::getSelectedDriverStatus()
			{
				return SelectedDriverStatus;
			}

			int HOSAgent::getSelectedDriverStatusIndexRow()
			{
				int ind = SelectedDriverStatus - static_cast<int>(DutyStatus::Offduty);
				return ind;
			}

			int HOSAgent::GetDriverStatusIndex(TimeLogRow *tlr)
			{
				int ind = static_cast<int>(tlr->Event()) - static_cast<int>(DutyStatus::Offduty);
				return ind;
			}

			void HOSAgent::setSelectedDriverStatusByIndexRow(int indexRow)
			{
				SelectedDriverStatus = indexRow + static_cast<int>(DutyStatus::Offduty);
			}

			int HOSAgent::getDriverStatusByIndexRow(int indexRow)
			{
				return (indexRow + static_cast<int>(DutyStatus::Offduty));
			}

			int HOSAgent::getIndexRowByDriverStatus(int status)
			{
				return (status - static_cast<int>(DutyStatus::Offduty));
			}

int HOSAgent::selectedIndexRow = 0;

			int HOSAgent::getJobsStatusSelectedIndexRow()
			{
				return selectedIndexRow;
			}

			void HOSAgent::setJobsStatusSelectedIndexRow(int ind)
			{
				selectedIndexRow = ind;
			}

			bool HOSAgent::isDutyStatusTransitioningFromOFFtoON(int currentDutyStatus, int requestedDutyStatus)
			{
				return ((currentDutyStatus == static_cast<int>(DutyStatus::Offduty) || currentDutyStatus == static_cast<int>(DutyStatus::Sleeping)) && (requestedDutyStatus == static_cast<int>(DutyStatus::Onduty) || requestedDutyStatus == static_cast<int>(DutyStatus::Driving)));
			}

			bool HOSAgent::isDutyStatusTransitioningFromONtoOFF(int currentDutyStatus, int requestedDutyStatus)
			{
				return ((requestedDutyStatus == static_cast<int>(DutyStatus::Offduty) || requestedDutyStatus == static_cast<int>(DutyStatus::Sleeping)) && (currentDutyStatus == static_cast<int>(DutyStatus::Onduty) || currentDutyStatus == static_cast<int>(DutyStatus::Driving)));
			}

			DateTime HOSAgent::getUtcDate()
			{
				return TimeUtil::DateTime_Now(); //DateTime.UtcNow;
			}

			long long HOSAgent::getEpochTime(DateTime dt)
			{
				//DateTime nd = DateTime(2010, 1, 1); //DateTime (1970, 0, 0);
				return 0;// static_cast<long long>((dt - nd).TotalMilliseconds);
			}

			long long HOSAgent::getEpochAtMidnight(DateTime currentDate)
			{
				//DateTime dt = DateTime(currentDate.Year,currentDate.Month,currentDate.Day);
				return 0;// getEpochTime(dt);
			}

			std::string HOSAgent::getDuration(DateTime transitionTimestamp, DateTime previousTransitionTimestamp)
			{
				//return getDuration(transitionTimestamp.getTime(), previousTransitionTimestamp.getTime(), true); 
				TimeSpan duration = transitionTimestamp - previousTransitionTimestamp;
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return "";// duration.ToString(R"(hh\:mm)");

			}

			int HOSAgent::GetMinutesFromMidNight(DateTime dt)
			{
				DateTime dt0 = dt.Date();//DateTime(dt.Year,dt.Month,dt.Day);
				TimeSpan ts = dt - dt0;
				return ts.TotalMinutes();
			}


			//===========================================================================================
			//===========================================================================================
#if 0 //TODO
			HOSCYCLE HOSAgent::GetCycleByIndex(int index)
			{
				//HOSCYCLE c = HOSCYCLE.Canadian_120hr_14day;
				HOSCYCLE cycle = static_cast<HOSCYCLE>((Enum::GetValues(HOSCYCLE::typeid))->GetValue(index));
				return cycle;
			}

			int HOSAgent::GetCycleIndexByCycle(HOSCYCLE cycle)
			{
				Type *type = cycle.GetType();
				//Type type1 = typeof(HOSCYCLE);
				Array *ar = Enum::GetValues(type);
				int index = Array::IndexOf(ar, cycle);
				return index;
			}

			int HOSAgent::NumOfCycles()
			{
				return Enum::GetNames(HOSCYCLE::typeid)->Length;
			}

			std::string HOSAgent::GetHosCycleStrByIndex(int index)
			{
				HOSCYCLE c = GetCycleByIndex(index);
				//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return c.ToString();
			}

			HOSCYCLE HOSAgent::selectedHosCycle = HOSCYCLE::NONE;

			std::string HOSAgent::GetSelectedHosCycleStr()
			{
				//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return selectedHosCycle.ToString();
			}

			int HOSAgent::GetSelectedCycleIndex()
			{
				return GetCycleIndexByCycle(selectedHosCycle);
			}

			void HOSAgent::SetSelectedCycleByIndex(int radioElementIndex)
			{
				selectedHosCycle = GetCycleByIndex(radioElementIndex);
			}

			HOSCYCLE HOSAgent::GetHosCycleByIndex(int ind)
			{
				return GetCycleByIndex(ind);
			}

			void HOSAgent::SetSelectedHosCycle(HOSCYCLE cycle)
			{
				selectedHosCycle = cycle;

			}

			HOSCYCLE HOSAgent::GetCurrentHosCycle()
			{
				return static_cast<HOSCYCLE>(selectedHosCycle);
			}

			std::string HOSAgent::GetCurrentHosCycleCountry()
			{
				std::string rv = "";
				//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				std::string str = GetCurrentHosCycle().ToString();

				if (str.find("CA_") != string::npos || str.find("Canadian") != string::npos)
				{
					rv = "CA";
				}
				else
				{
					if (str.find("US_") != string::npos)
					{
						rv = "US";
					}
				}
				return rv;
			}
#endif		

}
