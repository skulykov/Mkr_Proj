#include "../HourCalculator.h"//#include "AuditLogic.h"
#include "my_utils.h"

#include <math.h>  // Math::Round ->round

#include <random> 
#include <iostream> 
namespace HosEngine
{
	/*  qualifyRadiusRule.GetValueOrDefault() == true Nullable  (-1-not defined, 0 - false, 1 - true)
	*/

	void Clean_OffDutys(std::vector<OffdutySession*> offdutys)
	{
		for (OffdutySession *p : offdutys)
		{
			if (p != 0)
				delete p;
		}
	}

	//=============Helper functions===========================
	class clsRandom   //http://stackoverflow.com/questions/19665818/best-way-to-generate-random-numbers-using-c11-random-library
	{
		std::random_device rd;
		/*std::mt19937 mt(rd());
		std::uniform_real_distribution<double> dist(1.0, 10.0);*/
		std::mt19937 mt;
		std::uniform_real_distribution<double> dist;
	public:
		clsRandom()
		{
			mt= std::mt19937(rd());
			dist= std::uniform_real_distribution<double>(1.0, 100.0);
		}
		int Next(int v)
		{
			int rv = (int)dist(mt);
			return rv;
		}
	};
#pragma region Helper functions	
	string Rule2Str(int rule)
	{
		
		return "Rule:"+ Int2Str(rule);
	}
	string RuleT2Str(int ruletype)
	{
		if(ruletype==(int)RuleType::Day)
			return "Day";
		else if (ruletype == (int)RuleType::Shift)
			return "Shift";
		else
			return "";

	}

	bool IsUSRule(RULEID rule)
	{
		int v = (int)rule;
		if (v != 19 && v != 6 && v != 7 && v != 18)
			return true;
		else
			return false;
	}
	bool IsCARule(RULEID rule)
	{
		int v = (int)rule;
		if (v!=19&&(v == 6 || v == 7 || v == 18))
			return true;
		else
			return false;
	}
	/*struct {
		char *pStr;
		int val;
	}*/HOSCycleStruct	_cycles_[] = {
		{"Canadian_70hr_7day" , 6},
		{"Canadian_120hr_14day" , 7},

		{"US_60hr_7day" , 10},
		{"US_70hr_8day" , 11},
	{"Canadian_Oil_Field" , 18},

	{"NONE" , 19},

		{"US_Oil_Field" , 20},
		//MKR_NEW rules for Kiewit  -> 
		{"US_Texas" , 30},
		{"US_California" , 31}, //intra state rules
							//{"US_NewYork,32,
		{"US_Florida_707" , 32},
		{"US_Florida_808" , 33},
		{"US_Alaskan_707" , 34},
		{"US_Alaskan_808" , 35},

		{"CA_AlbertaRule" , 36},
		{"US_NewJersey_707" , 38},
		{"US_NewJersey_808" , 39},
		{"US_Georgia_707" , 40},
		{"US_Georgia_808" , 41},
		{"US_Alabama_607" , 42},
		{"US_Alabama_708" , 43},
		{"US_Arizona_607" , 44},
		{"US_Arizona_708" , 45},
		{"US_Connecticut_607" , 46},
		{"US_Connecticut_708" , 47},
		{"US_Delaware_607" , 48},
		{"US_Delaware_708" , 49},
		{"US_Idaho_607" , 50},
		{"US_Idaho_708" , 51},
		{"US_Indiana_607" , 52},
		{"US_Indiana_708" , 53},
		
		{"US_Iowa_707" , 54},
		{"US_Iowa_808" , 55},
		{"US_Kansas_607" , 56},
		{"US_Kansas_708" , 57},
		{"US_Louisiana_607" , 58},
		{"US_Louisiana_708" , 59},
		{"US_Maryland_707" , 60},
		{"US_Maryland_808" , 61},
		{"US_Michigan_607" , 62},
		{"US_Michigan_708" , 63},
		{"US_Mississippi_607" , 64},
		{"US_Mississippi_708" , 65},
		{"US_Missouri_607" , 66},
		{"US_Missouri_708" , 67},
		{"US_Montana_607" , 68},
		{"US_Montana_708" , 69},
		{"US_Nebraska_707" , 70},
		{"US_Nebraska_808" , 71},
		{"US_Nevada_707" , 72},
		{"US_NewMexico_607" , 73},
		{"US_NewMexico_708" , 74},
		{"US_NorthCarolina_707" , 75},
		{"US_NorthCarolina_808" , 76},
		{"US_NorthDakota_707" , 77},
		{"US_Ohio_607" , 78},
		{"US_Ohio_708" , 79},
		{"US_Oklahoma_607" , 80},
		{"US_Oklahoma_708" , 81},
		{"US_Oregon_707" , 82},
		{"US_Oregon_808" , 83},
		{"US_Pennsylvania_607" , 84},
		{"US_Pennsylvania_708" , 85},
		{"US_South_Dakota_707" , 86},
		{"US_South_Dakota_808" , 87},
		{"US_South_Dakota_607" , 88},
		{"US_South_Dakota_708" , 89},
		{"US_Tennessee_607" , 90},
		{"US_Tennessee_708" , 91},
		{"US_Vermont_607" , 92},
		{"US_Vermont_708" , 93},
		{"US_Virginia_607" , 94},
		{"US_Virginia_708" , 95},
		{"US_Wisconsin_707" , 96},
		{"US_Wisconsin_808" , 97},
		{"US_Wyoming_607" , 98},
		{"US_Wyoming_708" , 99}
	};

	std::string Cycle2Str(HOSCYCLE cycle)
	{
		std::string rv = "";
		/*switch ((int)cycle)
		{
			case 6:
				rv = "Canadian_70hr_7day";
				break;
		}*/
		for (HOSCycleStruct c : _cycles_)
		{
			if (c.val == (int)cycle)
			{
				rv = string(c.pStr);
				break;
			}
		}
		return rv;
	}
	std::string GetHosCycleList()
	{
		std::string rv = "";
		for (HOSCycleStruct c : _cycles_)
		{
			std::string rv1 = string(c.pStr) + "," + Int2Str(c.val)+"|";
			rv.append(rv1);
			
		}

		return rv;
	}
	struct greater
	{
		template<class T>
		bool operator()(T /*const*/ &a, T /*const*/ &b) /*const*/ 
		{ 
			return a > b; 
		}
	};
	struct greaterT
	{
		
		bool operator()(Timelog* /*const*/ &a, Timelog* /*const*/ &b) /*const*/
		{
			return (*a) > (*b);
		}
	};

	Timelog *GetLastOnDutyTlr(vector<Timelog*> &data,DateTime &startTime)
	{ 
		// sort FIRST ->  orderby o::Logtime descending   !!!!!!  -> the most most recent datetime goes FIRST!!
		std::sort(data.begin(), data.end(), greaterT());
		//Timelog *lastOnDutyTlr = (from o in data where((o->duty == DutyStatus::Driving || o->duty == DutyStatus::Onduty) && o::Logtime < startTime) orderby o::Logtime descending select o)->FirstOrDefault<Timelog*>();
		for (auto itr = data.begin(), end = data.end(); itr != end; ++itr)
		{
			
			Timelog * o = (*itr);
			if (o != 0)
			{
				if ((o->duty == DutyStatus::Driving || o->duty == DutyStatus::Onduty) && o->Logtime < startTime)
					return o;
			}
			
		}
		return 0;
	}
	Timelog *GetLastOffDutyOrSleepTlr(vector<Timelog*> &data, DateTime &startTime, DateTime &endTime)
	{
		// sort FIRST ->  orderby o::Logtime   !!!!!!
		//lastOnDutyTlr = (from o in data where((o->duty == DutyStatus::Offduty || o->duty == DutyStatus::Sleeping) && o::Logtime < startTime && o::Logtime > lastOnDutyTlr.Logtime) orderby o::Logtime select o)->FirstOrDefault<Timelog*>();
		for (auto itr = data.begin(), end = data.end(); itr != end; ++itr)
		{

			Timelog * o = (*itr);
			if (o != 0)
			{
				if ((o->duty == DutyStatus::Offduty || o->duty == DutyStatus::Sleeping) && o->Logtime < startTime && o->Logtime > endTime)
					return o;
			}

		}
		return 0;
	}
#pragma endregion
	//========================================================
#pragma region Audit Logic Base

int AuditLogic::TRIPINSPECTION = 7;
int AuditLogic::PERSONALUSE = 20;
int AuditLogic::FUELFORM = 1999;
int AuditLogic::REFUEL = 3;
int AuditLogic::OILFIELDWAIT = 31;
int AuditLogic::ONTRACK = 30;
int AuditLogic::OffDuty = 19;
int AuditLogic::OnDuty = 29;
int AuditLogic::Driving = 17;
int AuditLogic::Sleeping = 18;
int AuditLogic::STANDBYOFFTIME = 2000;
int AuditLogic::ManualStartOff = 2001;
int AuditLogic::ManualStartSleep = 2002;
int AuditLogic::ManualStartDriving = 2003;
int AuditLogic::ManualStartOnduty = 2004;
int AuditLogic::ManualEndOff = 2005;
int AuditLogic::ManualEndSleep = 2006;
int AuditLogic::ManualEndDriving = 2007;
int AuditLogic::ManualEndOnduty = 2008;
int AuditLogic::EmergencyUseStart = 2009;
int AuditLogic::EmergencyUseEnd = 2010;
int AuditLogic::PersonalUseEnd = 2011;
int AuditLogic::ThirtyMinutesOffDutyStart = 2012;
int AuditLogic::OffDutyMinutes = 30;
bool AuditLogic::Stop168hrsReset = true;
int AuditLogic::Airmiles150OndutyThreshold = 16;
std::vector<RULEID> AuditLogic::CanadianRuleset;
std::vector<RULEID> AuditLogic::USRuleset;
DateTime AuditLogic::EndOffduty = DateTime::MinValue;
bool AuditLogic::NeedtoCalculateRecap = true;

			const int &AuditLogic::getMaxDriving() const
			{
				return maxdriving;
			}

			const int &AuditLogic::getMaxOnduty() const
			{
				return maxonduty;
			}
			AuditLogic::AuditLogic(HourCalculator *p)
			{
				this->hc = p;
			}
			AuditResult *AuditLogic::Audit(AuditRequest *ar, EmployeeRow *&currentAuditDriver, bool isUSA, PTimeLogRowList *currentAuditDriverTimeLogs, double currentLatitude, double currentLogitude, DateTime &reachAirMilesThresholdTime, int qualifyRadiusRule)
			{
				CurrentAuditDriver = currentAuditDriver;
				CurrentAuditDriverTimeLogs = currentAuditDriverTimeLogs;
				//CurrentDefers = std::unordered_map<DateTime*, DeferHourRow*>(); // svk it's not a pointer !!!

				IsUSA = isUSA;
//C# TO C++ CONVERTER TODO TASK: There is no built-in support for multithreading in native C++:
				//lock (CanadianRuleset)
				{
					if (CanadianRuleset.empty())
					{
						CanadianRuleset.push_back(RULEID::CA_16Hour);
						CanadianRuleset.push_back(RULEID::CA_70Hour);
						CanadianRuleset.push_back(RULEID::CA_ChangeCycle);
						CanadianRuleset.push_back(RULEID::CA_Combine);
						CanadianRuleset.push_back(RULEID::CA_Driving);
						CanadianRuleset.push_back(RULEID::CA_OffDuty);
						CanadianRuleset.push_back(RULEID::CA_OilField);
						CanadianRuleset.push_back(RULEID::CA_OnDuty);
						CanadianRuleset.push_back(RULEID::CA_REST);
						CanadianRuleset.push_back(RULEID::CA_Service);
						CanadianRuleset.push_back(RULEID::CA_Service_12014);
						CanadianRuleset.push_back(RULEID::CA_Service_607);
						CanadianRuleset.push_back(RULEID::CA_Service_707);
						CanadianRuleset.push_back(RULEID::CA_Service_708);
					}
					if (USRuleset.empty())
					{
						USRuleset.push_back(RULEID::US_ChangeCycle);
						USRuleset.push_back(RULEID::US_Driving);
						USRuleset.push_back(RULEID::US_OnDuty);
						USRuleset.push_back(RULEID::US_Service_607);
						USRuleset.push_back(RULEID::US_Service_708);
						USRuleset.push_back(RULEID::US_OilField);
						USRuleset.push_back(RULEID::US_Alaskan_707);
						USRuleset.push_back(RULEID::US_Alaskan_808);
						USRuleset.push_back(RULEID::US_California);
						USRuleset.push_back(RULEID::US_Florida_707);
						USRuleset.push_back(RULEID::US_Florida_808);
						USRuleset.push_back(RULEID::US_Texas);
						USRuleset.push_back(RULEID::US_NewJersey_707);
						USRuleset.push_back(RULEID::US_NewJersey_808);
						USRuleset.push_back(RULEID::US_Georgia_707);
						USRuleset.push_back(RULEID::US_Georgia_808);
						USRuleset.push_back(RULEID::US_Alabama_607);
						USRuleset.push_back(RULEID::US_Alabama_708);
						USRuleset.push_back(RULEID::US_Arizona_607);
						USRuleset.push_back(RULEID::US_Arizona_708);
						USRuleset.push_back(RULEID::US_Connecticut_607);
						USRuleset.push_back(RULEID::US_Connecticut_708);
						USRuleset.push_back(RULEID::US_Delaware_607);
						USRuleset.push_back(RULEID::US_Delaware_708);
						USRuleset.push_back(RULEID::US_Idaho_607);
						USRuleset.push_back(RULEID::US_Idaho_708);
						USRuleset.push_back(RULEID::US_Indiana_607);
						USRuleset.push_back(RULEID::US_Indiana_708);
						USRuleset.push_back(RULEID::US_Iowa_707);
						USRuleset.push_back(RULEID::US_Iowa_808);
						USRuleset.push_back(RULEID::US_Kansas_607);
						USRuleset.push_back(RULEID::US_Kansas_708);
						USRuleset.push_back(RULEID::US_Louisiana_607);
						USRuleset.push_back(RULEID::US_Louisiana_708);
						USRuleset.push_back(RULEID::US_Maryland_707);
						USRuleset.push_back(RULEID::US_Maryland_808);
						USRuleset.push_back(RULEID::US_Michigan_607);
						USRuleset.push_back(RULEID::US_Michigan_708);
						USRuleset.push_back(RULEID::US_Mississippi_607);
						USRuleset.push_back(RULEID::US_Mississippi_708);
						USRuleset.push_back(RULEID::US_Missouri_607);
						USRuleset.push_back(RULEID::US_Missouri_708);
						USRuleset.push_back(RULEID::US_Montana_607);
						USRuleset.push_back(RULEID::US_Montana_708);
						USRuleset.push_back(RULEID::US_Nebraska_707);
						USRuleset.push_back(RULEID::US_Nebraska_808);
						USRuleset.push_back(RULEID::US_Nevada_707);
						USRuleset.push_back(RULEID::US_NewMexico_607);
						USRuleset.push_back(RULEID::US_NewMexico_708);
						USRuleset.push_back(RULEID::US_NorthCarolina_707);
						USRuleset.push_back(RULEID::US_NorthCarolina_808);
						USRuleset.push_back(RULEID::US_NorthDakota_707);
						USRuleset.push_back(RULEID::US_Ohio_607);
						USRuleset.push_back(RULEID::US_Ohio_708);
						USRuleset.push_back(RULEID::US_Oklahoma_607);
						USRuleset.push_back(RULEID::US_Oklahoma_708);
						USRuleset.push_back(RULEID::US_Oregon_707);
						USRuleset.push_back(RULEID::US_Oregon_808);
						USRuleset.push_back(RULEID::US_Pennsylvania_607);
						USRuleset.push_back(RULEID::US_Pennsylvania_708);
						USRuleset.push_back(RULEID::US_South_Dakota_607);
						USRuleset.push_back(RULEID::US_South_Dakota_708);
						USRuleset.push_back(RULEID::US_South_Dakota_707);
						USRuleset.push_back(RULEID::US_South_Dakota_808);
						USRuleset.push_back(RULEID::US_Tennessee_607);
						USRuleset.push_back(RULEID::US_Tennessee_708);
						USRuleset.push_back(RULEID::US_Vermont_607);
						USRuleset.push_back(RULEID::US_Vermont_708);
						USRuleset.push_back(RULEID::US_Virginia_607);
						USRuleset.push_back(RULEID::US_Virginia_708);
						USRuleset.push_back(RULEID::US_Wisconsin_707);
						USRuleset.push_back(RULEID::US_Wisconsin_808);
						USRuleset.push_back(RULEID::US_Wyoming_607);
						USRuleset.push_back(RULEID::US_Wyoming_708);
					}
				}
				AuditResult *result = MKR_NEW AuditResult();
				RULEID rule = RULEID::NONE;
				if (ar->timelog.size() > 0)
				{
					Timelog *tl = MKR_NEW Timelog();
					tl->Location = ar->timelog[0]->Location;
					tl->duty = DutyStatus::Offduty;
					//tl.Logtime = ar.timelog[0].Logtime.Date.AddDays(-1);
					tl->Logtime = ar->timelog[0]->Logtime.Date();
					tl->odometer = ar->timelog[0]->odometer;
					tl->stop = ar->timelog[0]->stop;
					stdH::insertItemAtIndex(ar->timelog, tl, 0);//ar->timelog->Insert(0, tl);
				}
				DateTime date_now = TimeUtil::DateTime_Now().Date();
				int size = ar->timelog.size();
				Timelog *ctl = ar->timelog[size - 1];
				if (ctl->Logtime.Date().CompareTo(date_now)!=0)
				{
					Timelog *tl1 = MKR_NEW Timelog();
					tl1->Location = ctl->Location;
					tl1->duty = ctl->duty;
					tl1->Logtime = TimeUtil::CurrentTime();
					tl1->odometer = ctl->odometer;
					tl1->stop = ctl->stop;
					ar->timelog.push_back(tl1);//ar->timelog->Add(tl1);

				}
				//Calculate recap //get recap.CycleAvail for reset cycle
				std::vector<RuleExceptions> ruleException;
				result->recap = CalculateRecap(ar, rule, ruleException); // createrecaptest();
				// ref rule is current rule
				//Get all offdufy
				EndOffduty = TimeUtil::CurrentTime();
				std::vector<OffdutySession*> offdutysessions = MergeOffduty(ar->timelog, TimeUtil::CurrentTime());

				//Nothing to do
				AuditTime *vl = Audit_Recap(result->recap, offdutysessions, rule, TimeUtil::CurrentTime(), ar->timelog);
				if (vl != nullptr)
				{
					result->AuditTimes.push_back(vl);
				}
				//cycle list my cycle history
				std::vector<AuditTime*> aav = Audit(ar->timelog, offdutysessions, TimeUtil::CurrentTime(), rule, ar->CycleList, ruleException, currentLatitude, currentLogitude, reachAirMilesThresholdTime, qualifyRadiusRule);
				stdH::addAll(result->AuditTimes, aav);//result->AuditTimes->AddRange(aav);
				Clean_OffDutys(offdutysessions);
				return result;
			}

			std::vector<AuditTime*> AuditLogic::Audit(std::vector<Timelog*> &data, std::vector<OffdutySession*> &offdutysessions, DateTime date, RULEID rule, std::vector<Cycle*> &cycles, std::vector<RuleExceptions> &ruleExceptions, double currentLatitude, double currentLogitude, DateTime &reachAirMilesThresholdTime, int qualifyRadiusRule)
			{
				std::vector<AuditTime*> result;
				if (data.empty())
				{
					return result;
				}
				Timelog *tl = data[data.size() - 1];
				 /* comment out because always use current cycle selection on 2014-12-10 
				if ((tl.Location == AuditJurisdiction.Canada) || (tl.Location == AuditJurisdiction.CA_North))
				*/

//C# TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to this .NET String method:
				if (/*rule->ToString()->StartsWith("CA"*/IsCARule(rule))
				{
					//if (tl.Location == AuditJurisdiction.Canada)
					if (tl->Latitude < 60)
					{
						maxdriving = 13;
						maxonduty = 14;
						//Audit Canada
						//Find shift start and dayused for check 16 elapse, 14 on duty and 13 hours driving
						//By Devin
						DateTime dt = TimeUtil::CurrentTime();
						DateTime dayStart = dt.Date();
						std::vector<AuditTime*> atshift = Audit_CA_ShiftRule(data, offdutysessions, ShiftStart);

						//if it is coworker hours, don't need to set
						//if (!AuditLogic.IsCoworker)
						//	BsmDevice.BsmHome.Program.ShiftStart = shiftStart;

						result.insert(result.end(), atshift.begin(), atshift.end());

						Cycle *cyc = nullptr;

						for (int index = cycles.size() - 1; index >= 0; index--)
						{
							Cycle *c = cycles[index];
							if (c->rule == RULEID::US_Service_607 || c->rule == RULEID::US_Service_708 || c->rule == RULEID::US_OilField)
							{
								continue;
							}
							cyc = c;
							break;
						}

						//Commented by Devin
						//foreach (Cycle c in cycles)
						//{
						//    if (c.rule == RULEID.US_Service_607 || c.rule == RULEID.US_Service_708 || c.rule == RULEID.US_OilField)
						//    {
						//        continue;
						//    }
						//    cyc=c;
						//    break;  
						//}
						//Check off duty 10 hours
						//11.16
						result.push_back(Audit_CA_Offduty_DayRule(offdutysessions, data, 10, cyc, dayStart));

						stdH::addAll(result, Audit_CA_DayRule(data, 13, 14, dayStart));//stdH::addAll(result,Audit_CA_DayRule(data, 13, 14, dayStart));

						if (rule == RULEID::CA_Service_12014)
						{
							result.push_back(Audit_CA_70Hour_2(data, date, 70));
						}

						//By devin changed
						//if ((rule != RULEID.CA_OilField)&&(rule!=RULEID.CA_Service_12014))
						if ((rule != RULEID::CA_OilField))
						{
							result.push_back(Audit_CA_70Hour_1(data, date, 14 * 24));
						}
						if (rule != RULEID::CA_OilField)
						{
							//Audit_DeferredRules(data, offdutysessions, result);
							Audit_DeferredRules(data, offdutysessions, result, dayStart);
						}
					}
					//if (tl.Location == AuditJurisdiction.CA_North)
					if (tl->Latitude >= 60)
					{
						maxdriving = 15;
						maxonduty = 18;
						stdH::addAll(result,Audit_CA_North(data, 15, 18, 20));
						if (rule == RULEID::CA_Service_12014)
						{
							result.push_back(Audit_CA_70Hour_2(data, date, 80));
						}
						//by devin
						//if ((rule != RULEID.CA_OilField) && (rule != RULEID.CA_Service_12014))
						if ((rule != RULEID::CA_OilField))
						{
							result.push_back(Audit_CA_70Hour_1(data, date, 14 * 24));
						}

					}
					//if (cycles.Count < 2) return result;
					//vl = Audit_CA_CycleSwitch(offdutysessions, data, date, cycles[cycles.Count - 1], cycles[cycles.Count - 2]);
					//if (vl != null) result.Add(vl);
					//return result;
				}

//C# TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to this .NET String method:
				if (/*rule->ToString()->StartsWith("US")*/IsUSRule(rule))
				{
					switch (rule)
					{
						case RULEID::US_Texas:
						{
								maxdriving = 12;
								maxonduty = 15;
								stdH::addAll(result,AuditUS(data, 12, 15, rule));
								break;
						}
						case RULEID::US_California:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						case RULEID::US_Florida_808:
						case RULEID::US_Florida_707:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}

						case RULEID::US_Alaskan_707:
						case RULEID::US_Alaskan_808:
						{
								maxdriving = 15;
								maxonduty = 20;
								stdH::addAll(result,AuditUS(data, 15, 20, rule));
								break;
						}
						case RULEID::US_NewJersey_707:
						case RULEID::US_NewJersey_808:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						case RULEID::US_Georgia_707:
						case RULEID::US_Georgia_808:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						case RULEID::US_Connecticut_607:
						case RULEID::US_Connecticut_708:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						case RULEID::US_Iowa_707:
						case RULEID::US_Iowa_808:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						case RULEID::US_Nebraska_707:
						case RULEID::US_Nebraska_808:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						case RULEID::US_Nevada_707:
						{
								maxdriving = 12;
								maxonduty = 15;
								stdH::addAll(result,AuditUS(data, 12, 15, rule));
								break;
						}
						case RULEID::US_NorthCarolina_707:
						case RULEID::US_NorthCarolina_808:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						case RULEID::US_NorthDakota_707:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						case RULEID::US_Oregon_707:
						case RULEID::US_Oregon_808:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						case RULEID::US_South_Dakota_707:
						case RULEID::US_South_Dakota_808:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						case RULEID::US_Wisconsin_707:
						case RULEID::US_Wisconsin_808:
						{
								maxdriving = 12;
								maxonduty = 16;
								stdH::addAll(result,AuditUS(data, 12, 16, rule));
								break;
						}
						default:
						{
								maxdriving = 11;
								maxonduty = 14;
								stdH::addAll(result,AuditUS(data, 11, 14, rule));
								break;
						}
					}
				}
				if (tl->Latitude >= 60)
				{
					maxdriving = 15;
					maxonduty = 20;
					stdH::addAll(result,AuditUS(data, 15, 20, rule));
				}
				 bool airmiles150 = false;

//C# TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to this .NET String method:
				if (/*rule->ToString()->StartsWith("US")*/IsUSRule(rule)) //only USA has radius rule
				{
					qualifyRadiusRule = CheckRadiusRule(result, ruleExceptions, currentLatitude, currentLogitude, reachAirMilesThresholdTime, rule, data, maxdriving, airmiles150, maxonduty);
				}
				for (auto at : result)
				{
					if (at->rule == RULEID::Thirty_Minutes_Break && /*qualifyRadiusRule.GetValueOrDefault() == true*/qualifyRadiusRule>=0) // if it is qulified for radius rule then do not need to take 30 minutes break
					{
						at->available = TimeSpan(maxdriving, 0, 0) - at->used;
					}
					if (at->rule == RULEID::US_OnDuty && qualifyRadiusRule>=0 && airmiles150 && maxonduty < Airmiles150OndutyThreshold && at->used.TotalHours() > maxonduty && at->available.TotalMinutes() <= 0)
					{
						at->available = at->available.Add(TimeSpan(Airmiles150OndutyThreshold - maxonduty, 0, 0));
						at->ondutyavailable = at->available;
					}
					at->maxdriving = maxdriving;
					if (at->available.TotalHours() > maxdriving)
					{
						at->shiftOndty = at->available.TotalMinutes() - maxdriving * 60;
						at->available = TimeSpan(maxdriving, 0, 0);
					}
					else
					{
						at->shiftOndty = 0;
					}
				}
				return result;
			}

			/*Nullable<bool>*/ int  AuditLogic::CheckRadiusRule(std::vector<AuditTime*> &result, std::vector<RuleExceptions> &ruleExceptions, double currentLatitude, double currentLogitude, DateTime &reachAirMilesThresholdTime, RULEID ruleId, std::vector<Timelog*> &data, int maxDriving, bool &airmiles150, int maxonduty)
			{
				bool airmiles100 = false;
				double onduty = 0;
				double avaliableDriving = 0;
				for (auto rule : ruleExceptions)
				{
					if (rule == RuleExceptions::USA_100_air_mile_radius)
					{
						airmiles100 = true;
						break;
					}
					if (rule == RuleExceptions::USA_150_air_mile_radius)
					{
						airmiles150 = true;
						break;
					}
				}
				if (airmiles100 || airmiles150)
				{
					double airMiles = 0;
					if (homeLatitude != 0 && homeLogitude != 0 && currentLatitude != 0 && currentLogitude != 0)
					{
						airMiles = DistanceToCalculateAirMiles(homeLatitude, homeLogitude, currentLatitude, currentLogitude); //Chekck air miles
					}
					AuditTime *ondutyAuditTime = stdH::FirstOrDefault(result,[&] (AuditTime *s)
					{
						return (s->rule == RULEID::US_OnDuty && s->ruletype == RuleType::Shift);
					});
					if (ondutyAuditTime != nullptr)
					{
						onduty = ondutyAuditTime->used.TotalHours();
					}
					Timelog *tlrtmp = stdH::FirstOrDefault(data,  [&] ( Timelog *s)
					{
						return (s->QualifyRadiusRule == false && s->Logtime > ShiftStart && s->duty == DutyStatus::Driving);
					});
					if (tlrtmp != nullptr)
					{
						return false;
					}
					AuditTime *onDrivingTime = stdH::FirstOrDefault(result,[&] (AuditTime *s)
					{
						return (s->rule == RULEID::US_Driving && s->ruletype == RuleType::Shift) || (s->rule == RULEID::Thirty_Minutes_Break && s->ruletype == RuleType::Shift);
					});
					if (onDrivingTime != nullptr)
					{
						avaliableDriving = maxdriving - onDrivingTime->used.TotalHours();
					}
					if (airmiles100)
					{
						//========fake it!!!
						if (airMiles > 100)
						{
							reachAirMilesThresholdTime = TimeUtil::DateTime_Now(); //Please change to dirver's time zone
						}
						if ((reachAirMilesThresholdTime.HasValue() && reachAirMilesThresholdTime >= ShiftStart) || onduty > 12 || avaliableDriving < 0)
						{
							return false;
						}
					}
					if (airmiles150)
					{
						int count = 0;
						if (maxonduty < Airmiles150OndutyThreshold)
						{
							count = CheckOnDutyHoursLimitCount(data, maxonduty, ruleId, ShiftStart, 7);
						}
						if (airMiles > 150)
						{
							reachAirMilesThresholdTime = TimeUtil::DateTime_Now(); //Please change to dirver's time zone
						}
						if ((reachAirMilesThresholdTime.HasValue() && reachAirMilesThresholdTime >= ShiftStart) || avaliableDriving < 0)
						{
							return false;
						}
						if (maxonduty < Airmiles150OndutyThreshold)
						{
							if ((onduty > maxonduty && count > 1) || onduty > Airmiles150OndutyThreshold)
							{
								return 0;
							}
						}
					}
					return 1;
				}
				return -1;
			}

			double AuditLogic::DistanceToCalculateAirMiles(double lat1, double lon1, double lat2, double lon2)
			{
				double rlat1 = M_PI * lat1 / 180;
				double rlat2 = M_PI * lat2 / 180;
				double theta = lon1 - lon2;
				double rtheta = M_PI * theta / 180;
				double dist = sin(rlat1) * sin(rlat2) + cos(rlat1) * cos(rlat2) * cos(rtheta);
				dist = acos(dist);
				dist = dist * 180 / M_PI;
				dist = dist * 60 * 1.1515;
				return /*Math::Round*/round(dist * 0.8684 /*, 2*/);
			}

			DateTime AuditLogic::FindShiftStart(std::vector<Timelog*> &data, int resetLimit, DateTime end)
			{
				TimeSpan offduty = TimeSpan(0);
				if (data.empty())
				{
					return TimeUtil::CurrentTime();
				}
				if (!end.HasValue())
				{
				Timelog *ntl = MKR_NEW Timelog();
				ntl->Location = data[data.size() - 1]->Location;
				ntl->Logtime = TimeUtil::CurrentTime();
				ntl->duty = data[data.size() - 1]->duty;
				data.push_back(ntl);
				}
				Timelog *lasttl = nullptr;
				DateTime lastoffdutyend = DateTime::MinValue;
				for (int i = data.size() - 1; i >= 0; i--)
				{
					Timelog *tl = data[i];
					 if (end.HasValue() && tl->Logtime > end)
					 {
						continue;
					 }
					if (lasttl == nullptr)
					{
						lasttl = tl;
						continue;
					}
					if ((tl->duty == DutyStatus::Driving) || (tl->duty == DutyStatus::Onduty))
					{
						if (offduty.TotalHours() >= resetLimit)
						{
							if (lastoffdutyend == DateTime::MinValue)
							{
								if (end.HasValue())
								{
									return end;
								}
								return TimeUtil::CurrentTime();
							}
							return lastoffdutyend;
						}
						offduty = TimeSpan(0);
						lastoffdutyend = tl->Logtime;
					}
					else
					{
						offduty += lasttl->Logtime - tl->Logtime;
					}
					lasttl = tl;
				}
				if (end.HasValue())
				{
					return lastoffdutyend == DateTime::MinValue ? end : lastoffdutyend;
				}
				else
				{
					return lastoffdutyend == DateTime::MinValue ? TimeUtil::CurrentTime() : lastoffdutyend;
				}
			}

			TimeSpan AuditLogic::CalculateHours(std::vector<Timelog*> &data, std::vector<DutyStatus> &requiredStatus, DateTime start, DateTime end)
			{
				TimeSpan duration = TimeSpan(0);
				Timelog *lasttl = nullptr;
				Timelog *tl = nullptr;
				for (int i = 0; i < data.size(); i++)
				{
					tl = data[i];
					if (tl->Logtime < start)
					{
						continue;
					}
					if (tl->Logtime >= end)
					{
						break;
					}
					if (ShiftStartOdometer == 0 && tl->odometer > 0)
					{
						ShiftStartOdometer = tl->odometer;
					}
					if (tl->Latitude != 0 && tl->Longitude != 0 && homeLatitude == 0 && homeLogitude == 0) //radius rule
					{
						homeLatitude = tl->Latitude;
						homeLogitude = tl->Longitude;
					}
					if (lasttl == nullptr)
					{
						lasttl = tl;
						continue;
					}
					if (std::find(requiredStatus.begin(), requiredStatus.end(), lasttl->duty) != requiredStatus.end())
					{
						duration += tl->Logtime - lasttl->Logtime;
					}
					lasttl = tl;
				}
				if (tl == nullptr)
				{
					return duration;
				}
				if (lasttl == nullptr)
				{
					return duration;
				}
				if (std::find(requiredStatus.begin(), requiredStatus.end(), lasttl->duty) != requiredStatus.end())
				{
					duration += end - lasttl->Logtime;
				}
				return duration;
			}

			AuditTime *AuditLogic::Audit_CA_CycleSwitch(std::vector<OffdutySession*> &offduties, std::vector<Timelog*> &data, DateTime date, Cycle *newcycle, Cycle *oldcycle)
			{
				if (newcycle->selectTime.Date() != date)
				{
					return nullptr;
				}
				if (newcycle->rule == RULEID::CA_OilField)
				{
					return nullptr;
				}
				int limit = 72;
				if ((oldcycle->rule == RULEID::CA_Service_707) && (newcycle->rule == RULEID::CA_Service_12014))
				{
					limit = 36;
				}

				OffdutySession *consectiveoff = nullptr;
				for (int i = offduties.size() - 1; i >= 0; i--)
				{
					if (offduties[i]->ts().TotalHours() >= limit)
					{
						consectiveoff = offduties[i];
						break;
					}
				}
				if ((consectiveoff == nullptr) || (consectiveoff->stop < date))
				{
					AuditTime *vl = MKR_NEW AuditTime();
					vl->rule = newcycle->rule;
					//vl.violation = newcycle.selectTime;
					return vl;
				}
				return nullptr;
			}

			std::string AuditLogic::Audit_CycleSwitch(std::vector<Timelog*> &data, RULEID oldCycle, RULEID newCycle)
			{
				//todo need to translate to multiple language
				std::string ret = "You are required to have three 24 hours off in the previous 24 days to switch to oil field rule.";
				//if (BsmDevice.BsmHome.Program.Language != "E")
				//	ret = "Trois jours de 24 heures de congés dans un cycle de 24 jours pour passer à la règle \"champ pétrolier\"";
				if (oldCycle == RULEID::CA_Service_707) //cycle 1
				{
					if (newCycle == RULEID::CA_Service_12014)
					{
						return CheckRestHours(data, 36);
					}
					else if (newCycle == RULEID::CA_OilField)
					{
						RecapRowAudit *rr = CAOilfield(data, TimeUtil::CurrentTime());
						if (rr->CycleAvail.TotalHours() > 0)
						{
							return "";
						}
						return ret;
					}
				}
				else if (oldCycle == RULEID::CA_Service_12014) //cycle 2
				{
					if (newCycle == RULEID::CA_OilField)
					{
						//By Devin 
						RecapRowAudit *rr = CAOilfield(data, TimeUtil::CurrentTime());
						if (rr->CycleAvail.TotalHours() > 0)
						{
							return "";
						}
						return ret;
					}
					else if (newCycle == RULEID::CA_Service_707)
					{
						return CheckRestHours(data,72);
					}
				}
				else if (oldCycle == RULEID::CA_OilField)
				{
					if (newCycle == RULEID::CA_Service_707)
					{
						return CheckRestHours(data,72);
					}
					else if (newCycle == RULEID::CA_Service_12014)
					{
						return CheckRestHours(data,72);
					}
				}
				else if ((oldCycle == RULEID::US_Service_607) || (oldCycle == RULEID::US_Service_708))
				{
					if (oldCycle != newCycle)
					{
						return CheckRestHours(data, 34);
					}
				}
				else if (oldCycle == RULEID::US_OilField)
				{
					if (oldCycle != newCycle)
					{
						return CheckRestHours(data, 24);
					}
				}
				return "";
			}

			std::string AuditLogic::CheckRestHours(std::vector<Timelog*> &data, int limit)
			{
				float totaloff = 0;
				bool haveworked = false;
				for (int i = data.size() - 1; i >= 0; i--)
				{
					if (data[i]->duty != DutyStatus::Offduty)
					{
						haveworked = true;
						break;
					}
				}
				if (!haveworked)
				{
					return "";
				}
				if (data.size() <= 1)
				{
					return "";
				}
				DateTime lasttime = TimeUtil::CurrentTime();
				for (int i = data.size() - 1; i >= 0; i--)
				{
					Timelog *tlr = data[i];
					if (tlr->duty != DutyStatus::Offduty)
					{
						break;
					}
					TimeSpan ts = lasttime - tlr->Logtime;
					totaloff += (float)(ts.TotalHours());
					lasttime = tlr->Logtime;
					if (totaloff >= limit)
					{
						return "";
					}
				}
			
					return std::string("You are required to be off duty for at least ") + Int2Str(limit) + std::string(" hours to change cycle. "); //You have rested for " + totaloff.ToString("N2", MKR_NEW CultureInfo("en-US")) + " hours.";
				

			}
#pragma endregion
	//====================================================================================
#pragma region USRule_AuditLogic.cpp


			std::vector<AuditTime*> AuditLogic::AuditUS(std::vector<Timelog*> &data, int drivinglimit, int ondutylimit, RULEID rule)
			{
				std::vector<AuditTime*> result;
				int resetHours = 10;
				switch (rule)
				{
				case RULEID::US_Texas:
				{
					resetHours = 8;
					break;
				}
				case RULEID::US_Connecticut_607:
				case RULEID::US_Connecticut_708:
				{
					resetHours = 8;
					break;
				}
				case RULEID::US_Nebraska_707:
				case RULEID::US_Nebraska_808:
				{
					resetHours = 8;
					break;
				}
				case RULEID::US_NorthCarolina_707:
				case RULEID::US_NorthCarolina_808:
				{
					resetHours = 8;
					break;
				}
				case RULEID::US_South_Dakota_707:
				case RULEID::US_South_Dakota_808:
				{
					resetHours = 8;
					break;
				}
				}

				DateTime dt1 = FindShiftStart(data, resetHours, /*nullptr*/DateTime());
				TimeSpan lastslpduration = TimeSpan(0);
				std::vector<OffdutySession*> sleeps = MergeOffduty(data, TimeUtil::CurrentTime(), false,/* nullptr*/DateTime());
				DateTime dt2 = USFindShiftStartBySleeper(data, 8, lastslpduration, sleeps);
				if (dt1 < dt2)
				{
					dt1 = dt2;
				}

				//if it is coworker hours, don't need to set
				//if (!AuditLogic.IsCoworker)
				ShiftStart = dt1;

				result.push_back(Audit_US_Shift_Driving(data, TimeUtil::CurrentTime(), dt1, drivinglimit, sleeps));
				result.push_back(Audit_US_Shift_OnDuty(data, TimeUtil::CurrentTime(), dt1, ondutylimit, sleeps));
				Clean_OffDutys(sleeps);
				return result;
			}

			int AuditLogic::CheckOnDutyHoursLimitCount(std::vector<Timelog*> &data, int ondutylimit, RULEID rule, DateTime startTime, int days)
			{
				int count = 0;
				int resetHours = 10;
				switch (rule)
				{
				case RULEID::US_Texas:
				{
					resetHours = 8;
					break;
				}
				case RULEID::US_Connecticut_607:
				case RULEID::US_Connecticut_708:
				{
					resetHours = 8;
					break;
				}
				case RULEID::US_Nebraska_707:
				case RULEID::US_Nebraska_808:
				{
					resetHours = 8;
					break;
				}
				case RULEID::US_NorthCarolina_707:
				case RULEID::US_NorthCarolina_808:
				{
					resetHours = 8;
					break;
				}
				case RULEID::US_South_Dakota_707:
				case RULEID::US_South_Dakota_808:
				{
					resetHours = 8;
					break;
				}
				}
				if (data.empty() || data.empty())
				{
					return 0;
				}
				//Timelog *lastOnDutyTlr = (from o in data where((o->duty == DutyStatus::Driving || o->duty == DutyStatus::Onduty) && o::Logtime < startTime) orderby o::Logtime descending select o)->FirstOrDefault<Timelog*>();
				Timelog *lastOnDutyTlr = GetLastOnDutyTlr(data, startTime);
				if (lastOnDutyTlr != nullptr) // find first off duty or sleep
				{
					lastOnDutyTlr = GetLastOffDutyOrSleepTlr(data, startTime, lastOnDutyTlr->Logtime);//(from o in data where((o->duty == DutyStatus::Offduty || o->duty == DutyStatus::Sleeping) && o::Logtime < startTime && o::Logtime > lastOnDutyTlr.Logtime) orderby o::Logtime select o)->FirstOrDefault<Timelog*>();
				}
				int index = 0;
				while (lastOnDutyTlr != nullptr && lastOnDutyTlr->Logtime.Date() >= TimeUtil::DateTime_Now().Date().AddDays(-1 * days) && index <= 30) // need to change now;
				{
					startTime = lastOnDutyTlr->Logtime;
					DateTime dt1 = FindShiftStart(data, resetHours, startTime);
					if (dt1 >= ShiftStart)
					{
						break;
					}
					TimeSpan lastslpduration = TimeSpan(0);
					std::vector<OffdutySession*> sleeps = MergeOffduty(data, startTime, false, startTime);
					DateTime dt2 = USFindShiftStartBySleeper(data, 8, lastslpduration, sleeps);
					if (dt1 < dt2)
					{
						dt1 = dt2;
					}
					AuditTime *auditTime = Audit_US_Shift_OnDuty(data, startTime, dt1, ondutylimit, sleeps);
					if (auditTime != nullptr && auditTime->used.TotalHours() >= ondutylimit)
					{
						count = count + 1;
					}
					startTime = dt1;
					lastOnDutyTlr = GetLastOnDutyTlr(data, startTime);//(from o in data where((o->duty == DutyStatus::Driving || o->duty == DutyStatus::Onduty) && o::Logtime < startTime) orderby o::Logtime descending select o)->FirstOrDefault<Timelog*>();
					if (lastOnDutyTlr != nullptr) // find first off duty or sleep
					{
						lastOnDutyTlr = GetLastOffDutyOrSleepTlr(data, startTime, lastOnDutyTlr->Logtime);//(from o in data where((o->duty == DutyStatus::Offduty || o->duty == DutyStatus::Sleeping) && o::Logtime < startTime && o::Logtime > lastOnDutyTlr.Logtime) orderby o::Logtime select o)->FirstOrDefault<Timelog*>();
					}
					index = index + 1;

					Clean_OffDutys(sleeps);
				}
				return count;
			}

			DateTime AuditLogic::USFindShiftStartBySleeper(std::vector<Timelog*> &data, int sleeplimit, TimeSpan &lastslpduration, std::vector<OffdutySession*> &sleeps)
			{
				//List<OffdutySession> sleeps = MergeOffduty(data, localtime, false);
				//Drivers using a sleeper berth must take at least 8 hours in the sleeper berth, and may split the sleeper berth time into two periods provided neither is less than 2 hours.
				lastslpduration = TimeSpan(8, 0, 0);
				for (int i = sleeps.size() - 1; i > 0; i--)
				{
					if ((sleeps[i]->current == DutyStatus::Offduty) && (sleeps[i]->ts().TotalHours() >= 2))
					{
						if ((sleeps[i - 1]->current == DutyStatus::Sleeping) && (sleeps[i - 1]->ts().TotalHours() >= 8))
						{
							lastslpduration = sleeps[i]->ts();
							return sleeps[i - 1]->stop;
						}
					}

					if ((sleeps[i]->current == DutyStatus::Sleeping) && (sleeps[i]->ts().TotalHours() >= 2))
					{
						if ((sleeps[i - 1]->current == DutyStatus::Offduty) && (sleeps[i - 1]->ts().TotalHours() >= 2) && (sleeps[i]->ts().TotalHours() >= 8))
						{
							lastslpduration = sleeps[i]->ts();
							return sleeps[i - 1]->stop;
						}
						if ((sleeps[i - 1]->current == DutyStatus::Sleeping) && sleeps[i - 1]->ts().TotalHours() >= 2) //MKR_NEW 2014.09.16 must >= 2 hours
						{
							if (((sleeps[i]->ts().TotalHours() + sleeps[i - 1]->ts().TotalHours()) >= 10) && (sleeps[i]->ts().TotalHours() >= 8 || sleeps[i - 1]->ts().TotalHours() >= 8))
							{
								lastslpduration = sleeps[i]->ts();
								return sleeps[i - 1]->stop;
							}
						}
					}
				}
				lastslpduration = TimeSpan(0);
				return DateTime::MinValue;
			}

			AuditTime *AuditLogic::Audit_US_Shift_Driving(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit, std::vector<OffdutySession*> &sleeps)
			{
				AuditTime *at = MKR_NEW AuditTime();
				at->rule = RULEID::US_Driving;
				at->DispName = "Driving";
				at->ViolationDescription = "Driving time limit for work shift exceeded";
				at->ruletype = RuleType::Shift;
				at->available = TimeSpan(limit, 0, 0);
				std::vector<DutyStatus> required;
				required.push_back(DutyStatus::Driving);
				TimeSpan driving = CalculateHours(data, required, shiftstart, end);
				at->available = TimeSpan(limit, 0, 0) - driving;
				at->RuleDescription = "";//TODO string_format("{0}{1}{2}", "You are about to exceed driving time limit for work shift in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				at->used = driving;

				//2013.06.09 30 minutes after 8 consecutive hours on duty
				//if (at.available.TotalMinutes > 0 && sleeps != null && sleeps.Count > 0)
				if (sleeps.size() > 0 && sleeps.size() > 0)
				{
					DateTime lastThirtyMinutes = shiftstart;
					for (int index = sleeps.size() - 1; index >= 0; index--)
					{
						if (sleeps[index]->stop < shiftstart)
						{
							break;
						}
						if (sleeps[index]->ts().TotalMinutes() >= 30)
						{
							lastThirtyMinutes = sleeps[index]->stop;
							break;
						}
					}
					//if (TimeUtil.CurrentTime.Subtract(sleeps[sleeps.Count - 1].stop).TotalHours >= 8)
					//Fixed on Nov 4, 2013 for misunderstanding
					//30 minutes is not for 8 consecutive hours on duty, is for 8 hours on duty 
					//which not has 30 minutes break inside 8 hours,
					AuditGlobalInstance::LastThirtyMinuteOffDuty[CurrentAuditDriver->id()] = lastThirtyMinutes;
					DateTime dt1 = TimeUtil::CurrentTime();
					if (dt1.Subtract(lastThirtyMinutes).TotalHours() >= 8)
					{
						DateTime dt = lastThirtyMinutes.AddHours(8);
						TimeSpan ts = dt - dt1;
						if ((ts) < at->available)
						{
							at->rule = RULEID::Thirty_Minutes_Break;
							at->available = ts;
							at->ViolationDescription = "Minimum 30-minute off-duty break required.";
							at->RuleDescription = "";//TODO string_format("{0}{1}{2}", "You are about to violate minimum 30-minute off-duty break requirement in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
						}
					}
				}
				return at;
			}

			AuditTime *AuditLogic::Audit_US_Shift_OnDuty(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit, std::vector<OffdutySession*> &sleeps)
			{
				//calculate 8 consecutive sleep hours
				TimeSpan sleeptotal = TimeSpan(0);
				if (sleeps.size() > 0 && sleeps.size() > 0)
				{
					for (int index = sleeps.size() - 1; index >= 0; index--)
					{
						if (sleeps[index]->start >= shiftstart && sleeps[index]->stop <= end && sleeps[index]->current == DutyStatus::Sleeping && (sleeps[index]->ts().TotalHours() >= 8 || sleeps[index]->StopAction == AuditLogic::OILFIELDWAIT))
						{
							sleeptotal = sleeptotal + sleeps[index]->ts();
						}
					}
				}

				AuditTime *at = MKR_NEW AuditTime();
				at->DispName = "On Duty";
				at->ViolationDescription = "On duty time limit for work shift exceeded";
				at->rule = RULEID::US_OnDuty;
				at->ruletype = RuleType::Shift;
				at->available = TimeSpan(limit, 0, 0);
				//List<DutyStatus> required = MKR_NEW List<DutyStatus>();
				//required.Add(DutyStatus.Driving);
				//required.Add(DutyStatus.Onduty);
				TimeSpan onduty = end - shiftstart; // CalculateHours(data, required, shiftstart, end);// MKR_NEW TimeSpan(0);
				at->available = TimeSpan(limit, 0, 0) + sleeptotal - onduty;
				at->ondutyavailable = at->available;
				at->used = onduty;
				at->RuleDescription = "";//TODO string_format("{0}{1}{2}", "You are about to exceed on duty time limit for work shift in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				//at.ondutyavailable = at.available;
				return at;

			}

			AuditTime *AuditLogic::Audit_Recap(std::vector<RecapRowAudit*> &recaps, std::vector<OffdutySession*> &offduties, RULEID rule, DateTime date, std::vector<Timelog*> &data)
			{
				AuditTime *at = MKR_NEW AuditTime();
				at->ruletype = RuleType::Day;
				at->rule = rule;
				at->ViolationDescription = "Driving time limit exceeded for cycle";
				if (rule == RULEID::CA_Service_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "CA Cycle 1";
				}
				else if (rule == RULEID::CA_Service_12014)
				{
					at->available = TimeSpan(120, 0, 0);
					at->DispName = "CA Cycle 2";
				}
				else if (rule == RULEID::CA_OilField)
				{
					at->available = TimeSpan(504, 0, 0);
					at->DispName = "CA Oil Field";
				}
				else if (rule == RULEID::US_Service_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "US 60/7";
				}
				else if (rule == RULEID::US_Service_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "US 70/8";
				}
				else if (rule == RULEID::US_OilField)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "US Oil Field";
				}
				else if (rule == RULEID::US_California)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "California Rule";
				}
				else if (rule == RULEID::US_Florida_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Florida 70/7  Rule";
				}
				else if (rule == RULEID::US_Florida_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "Florida 80/8  Rule";
				}
				else if (rule == RULEID::US_Alaskan_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Alaskan 70/7  Rule";
				}
				else if (rule == RULEID::US_Alaskan_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "Alaskan 80/8  Rule";
				}
				else if (rule == RULEID::CA_AlbertaRule)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Alberta Rule";
				}
				else if (rule == RULEID::US_NewJersey_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "New Jersey Rule";
				}
				else if (rule == RULEID::US_NewJersey_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "New Jersey Rule";
				}
				else if (rule == RULEID::US_Georgia_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Georgia Rule";
				}
				else if (rule == RULEID::US_Georgia_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "Georgia Rule";
				}
				else if (rule == RULEID::US_Arizona_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Arizona Rule";
				}
				else if (rule == RULEID::US_Arizona_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Arizona Rule";
				}
				else if (rule == RULEID::US_Alabama_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Alabama Rule";
				}
				else if (rule == RULEID::US_Alabama_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Alabama Rule";
				}
				else if (rule == RULEID::US_Connecticut_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Connecticut Rule";
				}
				else if (rule == RULEID::US_Connecticut_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Connecticut Rule";
				}
				else if (rule == RULEID::US_Delaware_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Delaware Rule";
				}
				else if (rule == RULEID::US_Delaware_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Delaware Rule";
				}
				else if (rule == RULEID::US_Idaho_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Idaho Rule";
				}
				else if (rule == RULEID::US_Idaho_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Idaho Rule";
				}
				else if (rule == RULEID::US_Indiana_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Indiana Rule";
				}
				else if (rule == RULEID::US_Indiana_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Indiana Rule";
				}
				else if (rule == RULEID::US_Iowa_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Iowa Rule";
				}
				else if (rule == RULEID::US_Iowa_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "Iowa Rule";
				}
				else if (rule == RULEID::US_Kansas_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Kansas Rule";
				}
				else if (rule == RULEID::US_Kansas_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Kansas Rule";
				}
				else if (rule == RULEID::US_Louisiana_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Louisiana Rule";
				}
				else if (rule == RULEID::US_Louisiana_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Louisiana Rule";
				}
				else if (rule == RULEID::US_Maryland_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Maryland Rule";
				}
				else if (rule == RULEID::US_Maryland_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "Maryland Rule";
				}
				else if (rule == RULEID::US_Michigan_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Michigan Rule";
				}
				else if (rule == RULEID::US_Michigan_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Michigan Rule";
				}
				else if (rule == RULEID::US_Mississippi_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Mississippi Rule";
				}
				else if (rule == RULEID::US_Mississippi_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Mississippi Rule";
				}
				else if (rule == RULEID::US_Missouri_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Missouri Rule";
				}
				else if (rule == RULEID::US_Missouri_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Missouri Rule";
				}
				else if (rule == RULEID::US_Montana_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Montana Rule";
				}
				else if (rule == RULEID::US_Montana_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Montana Rule";
				}
				else if (rule == RULEID::US_Nebraska_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Nebraska Rule";
				}
				else if (rule == RULEID::US_Nebraska_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "Nebraska Rule";
				}
				else if (rule == RULEID::US_Nevada_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Nevada Rule";
				}
				else if (rule == RULEID::US_NewMexico_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "New Mexico Rule";
				}
				else if (rule == RULEID::US_NewMexico_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "New Mexico Rule";
				}
				else if (rule == RULEID::US_NorthCarolina_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "North Carolina Rule";
				}
				else if (rule == RULEID::US_NorthCarolina_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "North Carolina Rule";
				}
				else if (rule == RULEID::US_NorthDakota_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "North Dakota Rule";
				}
				else if (rule == RULEID::US_Ohio_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Ohio Rule";
				}
				else if (rule == RULEID::US_Ohio_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Ohio Rule";
				}
				else if (rule == RULEID::US_Oklahoma_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Oklahoma Rule";
				}
				else if (rule == RULEID::US_Oklahoma_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Oklahoma Rule";
				}
				else if (rule == RULEID::US_Oregon_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Oregon Rule";
				}
				else if (rule == RULEID::US_Oregon_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "Oregon Rule";
				}
				else if (rule == RULEID::US_Pennsylvania_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Pennsylvania Rule";
				}
				else if (rule == RULEID::US_Pennsylvania_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Pennsylvania Rule";
				}
				else if (rule == RULEID::US_South_Dakota_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "South Dakota Rule";
				}
				else if (rule == RULEID::US_South_Dakota_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "South Dakota Rule";
				}
				else if (rule == RULEID::US_South_Dakota_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "South Dakota Rulee";
				}
				else if (rule == RULEID::US_South_Dakota_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "South Dakota Rule";
				}
				else if (rule == RULEID::US_Tennessee_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Tennessee Rulee";
				}
				else if (rule == RULEID::US_Tennessee_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Tennessee Rule";
				}
				else if (rule == RULEID::US_Vermont_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Vermont Rulee";
				}
				else if (rule == RULEID::US_Vermont_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Vermont Rule";
				}
				else if (rule == RULEID::US_Virginia_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Virginia Rulee";
				}
				else if (rule == RULEID::US_Virginia_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Virginia Rule";
				}
				else if (rule == RULEID::US_Wisconsin_707)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Wisconsin Rule";
				}
				else if (rule == RULEID::US_Wisconsin_808)
				{
					at->available = TimeSpan(80, 0, 0);
					at->DispName = "Wisconsin Rule";
				}
				else if (rule == RULEID::US_Wyoming_607)
				{
					at->available = TimeSpan(60, 0, 0);
					at->DispName = "Wyoming Rule";
				}
				else if (rule == RULEID::US_Wyoming_708)
				{
					at->available = TimeSpan(70, 0, 0);
					at->DispName = "Wyoming Rule";
				}
				if (recaps.empty())
				{
					return at;
				}
				RecapRowAudit *rr = recaps[recaps.size() - 1];
				if (rr == nullptr)
				{
					return nullptr;
				}
				DateTime end = date.AddDays(1);
				//if (rr.CycleAvail.TotalHours < 0)
				{
					at->available = rr->CycleAvail;
					at->used = rr->DayUsed;
					at->RuleDescription = m_format("Your are about to exceed driving time limit for cycle in %10.5f %s", round(at->available.TotalMinutes()), " minute(s)");
					//at.DispName = rr.Cycle;
					return at;
				}
				return nullptr;
			}

			AuditTime *AuditLogic::Audit_CA_70Hour_1(std::vector<Timelog*> &data, DateTime date, int limit)
			{
				//all driver must take 24 hour off within the last 14 days
				DateTime start = data[0]->Logtime;
				DateTime end = TimeUtil::CurrentTime();
				DateTime lasttl = end;

				TimeSpan offduty = TimeSpan(0);
				for (int i = data.size() - 1; i >= 0; i--)
				{
					Timelog *tl = data[i];
					if ((tl->duty == DutyStatus::Offduty) || (tl->duty == DutyStatus::Sleeping))
					{
						offduty += lasttl - tl->Logtime;
						if (offduty.TotalHours() >= 24)
						{
							start = tl->Logtime.AddHours(offduty.TotalHours());
							break;
						}
					}
					else
					{
						offduty = TimeSpan(0);
					}
					lasttl = tl->Logtime;
				}
				AuditTime *at = MKR_NEW AuditTime();
				at->available = TimeSpan(limit, 0, 0);

				at->ruletype = RuleType::Day;
				at->rule = RULEID::CA_70Hour;
				at->DispName = string("24 Hour off");
				at->ViolationDescription = string("24 Hour off duty was not taken within the last 14 days");
				at->used = end - start;
				at->available = TimeSpan(limit, 0, 0) - (end - start);
				at->RuleDescription = m_format("You are about to violate 24 hour off-duty requirement within the last 14 days in %10.5f %s", round(at->available.TotalMinutes()), " minute(s)");
				return at;

			}

			AuditTime *AuditLogic::Audit_CA_70Hour_2(std::vector<Timelog*> &data, DateTime date, int limit)
			{
				//in cycle 2, driver must take 24 hour off before reach 70 hour of driving time
				AuditTime *at = MKR_NEW AuditTime();
				at->DispName = "CA2 70/80";
				at->ViolationDescription = "24 hours off duty was not taken before 80 hours for CA Cycle 2";
				at->available = TimeSpan(limit, 0, 0);
				at->rule = RULEID::CA_70Hour;
				at->ruletype = RuleType::Day;
				at->used = TimeSpan(0);
				if (data.empty())
				{
					return at;
				}

				DateTime lasttl = TimeUtil::CurrentTime();
				TimeSpan offduty = TimeSpan(0);
				TimeSpan onduty = TimeSpan(0);
				for (int i = data.size() - 1; i >= 0; i--)
				{
					Timelog *tl = data[i];
					TimeSpan ts = lasttl - tl->Logtime;
					if ((tl->duty == DutyStatus::Offduty) || (tl->duty == DutyStatus::Sleeping))
					{
						offduty += ts;
						if (offduty.TotalHours() >= 24)
						{
							break;
						}
					}
					else
					{
						offduty = TimeSpan(0);
						onduty += ts;
					}
					lasttl = tl->Logtime;
				}

				at->available = TimeSpan(limit, 0, 0) - onduty;
				at->RuleDescription = m_format("You are about to violate 24 hours off-duty requirement before 80 hours for CA Cycle 2 in %10.5f %s", round(at->available.TotalMinutes()), " minute(s)");
				at->used = onduty;
				return at;
			}

			void AuditLogic::Audit_DeferredRules(std::vector<Timelog*> &data, std::vector<OffdutySession*> &offduties, std::vector<AuditTime*> &result, DateTime dayStart)
			{
				DeferHourRow *dhr = FindExistingDeferHour(dayStart);
				if (dhr == nullptr)
				{
					return;
				}
				TimeSpan avail = dhr->defers;
				if (dhr->day2.Date() == dayStart.Date()) //now checking if existing dhr cause any problem or not
				{
					TimeSpan ts = GetTotalDrivingTimeInDayRange(data, dayStart.AddHours(-24), dayStart.AddHours(24));
					if (ts.TotalHours() > 26)
					{
						avail = TimeSpan(0);
						return;
					}
					DateTime remark = TimeUtil::CurrentTime();
					double corehours = 0;
					DateTime corehoursStop = DateTime::MinValue;
					ts = GetTotalOffdutyInDate(offduties, dayStart.AddHours(-24), dayStart.AddHours(24), remark, corehours, corehoursStop);
					//ts = GetTotalOffdutyInDate(offduties, dhr.day1.Date, dhr.day2.Date.AddDays(1), ref remark);
					if (ts.TotalHours() < 20)
					{
						TimeSpan required = TimeSpan(20, 0, 0) - ts;
						DateTime dt1 = dayStart.AddHours(24), dt2 = TimeUtil::CurrentTime();
						TimeSpan left = dt1 - dt2;

						if (left <= required)
						{
							avail = TimeSpan(0);
							return;
						}
					}
				}
				for (auto at : result)
				{
					if (at->ruletype == RuleType::Shift)
					{
						continue;
					}
					if (dhr->day1.Date() == dayStart.Date()) //by devin
					{
						if ((at->rule == RULEID::CA_OnDuty) || (at->rule == RULEID::CA_Driving) || (at->rule == RULEID::CA_OffDuty))
						{
							at->available += avail;
						}
					}

				}
			}

			DeferHourRow *AuditLogic::FindExistingDeferHour(DateTime date)
			{
				if (CurrentAuditDriverTimeLogs->list_size() == 0)
				{
					return nullptr;
				}
				return this->hc->FindDeferHourByDate(date, CurrentAuditDriver->id(), CurrentDefers);
			}

#pragma endregion
#pragma region CARule_AuditLogic_Cpp


			
			std::string OffdutySession::ToString()
			{
				return string_format("start=%s, stop=%s, span=%s, next duty=%d", start.ToString().c_str(), stop.ToString().c_str(), ts().ToString().c_str(), (int)prev);
			}
			/*
			OffdutySession *OffdutySession::Clone()
			{
				OffdutySession *os = MKR_NEW OffdutySession();
				os->start=(this->start);
				os->stop=(this->stop);
				os->prev = this->prev;
				os->StopAction = this->StopAction;
				os->current = this->current;
				return os;
			}
			*/
			std::vector<OffdutySession*> AuditLogic::MergeOffduty(std::vector<Timelog*> &data, DateTime end)
			{
				return MergeOffduty(data, end, true, DateTime());
			}

			std::vector<OffdutySession*> AuditLogic::MergeOffduty(std::vector<Timelog*> &data, DateTime end, bool mergeSleepOff, DateTime endDT)
			{

				int start = 0; // FindStart(data, date);
				std::vector<OffdutySession*> sessions;
				for (int i = start; i < data.size(); i++)
				{
					if (endDT.HasValue() && data[i]->Logtime > endDT)
					{
						break;
					}
					if ((data[i]->duty == DutyStatus::Sleeping) || (data[i]->duty == DutyStatus::Offduty))
					{
						OffdutySession *os = MKR_NEW OffdutySession();
						os->start=(data[i]->Logtime);
						os->prev = DutyStatus::Offduty;
						os->StopAction = data[i]->stop;
						os->current = data[i]->duty;
						if (os->StopAction == AuditLogic::OILFIELDWAIT)
						{
							os->current = DutyStatus::Sleeping;
						}
						//2014.09.17 if oilfield waiting then extend 14 hours dring windows
						if (os->StopAction != AuditLogic::STANDBYOFFTIME && os->StopAction != AuditLogic::OILFIELDWAIT)
						{
							os->StopAction = 0;
						}

						if (i > 0)
						{
							os->prev = data[i - 1]->duty;
						}
						if (i < (data.size() - 1))
						{
							os->stop=(data[i + 1]->Logtime);
						}
						else
						{
							os->stop=(end);
						}
						sessions.push_back(os);
						if (os->stop > end)
						{
							os->stop=(end);
							break;
						}
					}
				}
				for (int i = sessions.size() - 1; i >= 1; i--)
				{
					if ((sessions[i]->start == sessions[i - 1]->stop) && ((sessions[i]->prev == DutyStatus::Offduty) || sessions[i]->prev == DutyStatus::Sleeping) && (sessions[i]->StopAction == sessions[i - 1]->StopAction) && ((sessions[i]->current == sessions[i - 1]->current) || mergeSleepOff))
					{
						sessions[i - 1]->stop=(sessions[i]->stop);
						OffdutySession *p = sessions[i];
						if (p != 0) delete p;
						stdH::removeItemAtIndex(sessions, i);//sessions.RemoveAt(i);
					}
				}
				for (int i = sessions.size() - 1; i >= 0; i--)
				{
					//If less than 30 minutes, then it is not off duty
					if (sessions[i]->ts().TotalMinutes() < AuditLogic::OffDutyMinutes)
					{
						OffdutySession *p = sessions[i];
						if (p != 0) delete p;
						stdH::removeItemAtIndex(sessions, i);
					}
				}
				return sessions;

			}

			DateTime AuditLogic::CA_FindShiftStartByOffduty(std::vector<Timelog*> &data)
			{
				DateTime curt = TimeUtil::CurrentTime();
				std::vector<OffdutySession*> offdutys = MergeOffduty(data, curt);
				DateTime shiftStart = DateTime::MinValue;
				DateTime satisfingEndTime = DateTime::MinValue;
				TimeSpan extra = TimeSpan(0);
				if (data.empty())
				{
					return TimeUtil::CurrentTime();
				}
				DeferHourRow *dhr = FindExistingDeferHour(curt.Date());
				for (int i = 0; i < offdutys.size(); i++)
				{
					double limit = 8;
					if ((dhr != nullptr) && (offdutys[i]->start.Date() == dhr->day2.Date()))
					{
						limit += dhr->defers.TotalHours();
					}
					if ((offdutys[i]->ts().TotalHours() >= limit) && (offdutys[i]->StopAction != AuditLogic::STANDBYOFFTIME))
					{
						if (offdutys[i]->start.Date() == offdutys[i]->stop.Date())
						{
							shiftStart = offdutys[i]->stop;
							satisfingEndTime = offdutys[i]->start.AddHours(8);
							continue;
						}
						else //now 8 hour consecutive is cross two days check for double dipping
						{
							if (satisfingEndTime.Date() < offdutys[i]->start.Date())
							{
								satisfingEndTime = offdutys[i]->start.Date();
							}
							TimeSpan offtime = TimeSpan(0);
							for (int j = 0; j < offdutys.size(); j++)
							{
								if ((offdutys[j]->stop >= satisfingEndTime) && (offdutys[j]->stop.Date() == offdutys[i]->start.Date()))
								{
									if (offdutys[j]->start > satisfingEndTime)
									{
										offtime += offdutys[j]->ts();
									}
									else
									{
										offtime += offdutys[j]->stop - satisfingEndTime;
									}
								}
							}
							DateTime start = offdutys[i]->start;
							if (offtime.TotalHours() < 2)
							{
								start = offdutys[i]->start.AddHours(2 - offtime.TotalHours());
							}
							TimeSpan ts = offdutys[i]->stop - start;
							if (ts.TotalHours() >= limit)
							{
								shiftStart = offdutys[i]->stop;
								satisfingEndTime = start.AddHours(8);
							}
						}
					}

				}
				Clean_OffDutys(offdutys);
				return shiftStart;
			}

			DateTime AuditLogic::US_FindShiftStartByOffduty(std::vector<Timelog*> &data)
			{
				std::vector<OffdutySession*> offdutys = MergeOffduty(data, TimeUtil::CurrentTime());
				DateTime shiftStart = DateTime::MinValue;
				DateTime satisfingEndTime = DateTime::MinValue;
				TimeSpan extra = TimeSpan(0);
				if (data.empty())
				{
					return TimeUtil::CurrentTime();
				}
				for (int i = 0; i < offdutys.size(); i++)
				{
					double limit = 8;
					if (offdutys[i]->ts().TotalHours() >= limit)
					{
						if (offdutys[i]->start.Date() == offdutys[i]->stop.Date())
						{
							shiftStart = offdutys[i]->stop;
							satisfingEndTime = offdutys[i]->start.AddHours(8);
							continue;
						}
					}
				}
				Clean_OffDutys(offdutys);
				return shiftStart;
			}

			DateTime AuditLogic::FindShiftStartBySleeper(std::vector<Timelog*> &data, int sleeplimit, TimeSpan &lastslpduration)
			{
				std::vector<OffdutySession*> sleeps = Audit_CA_MergeSleep(data, TimeUtil::CurrentTime());
				for (int i = sleeps.size() - 1; i > 0; i--)
				{
					if ((sleeps[i]->ts().TotalHours() < 2) || (sleeps[i - 1]->ts().TotalHours() < 2))
					{
						continue;
					}
					if ((sleeps[i]->ts().TotalHours() + sleeps[i - 1]->ts().TotalHours()) >= sleeplimit)
					{
						lastslpduration = sleeps[i]->ts();

						DateTime dt = sleeps[i - 1]->stop;
						Clean_OffDutys(sleeps);
						return dt;
					}
				}
				lastslpduration = TimeSpan(0);
				Clean_OffDutys(sleeps);
				return DateTime::MinValue;
			}

			TimeSpan AuditLogic::Calculate_TotalOff_InDate(std::vector<OffdutySession*> &todaylist, DateTime searchDate, DateTime dayend, OffdutySession *consectiveoff)
			{
				TimeSpan dayoff = TimeSpan(0);
				for (int i = 0; i < todaylist.size(); i++)
				{
					if (todaylist[i]->start < searchDate)
					{
						dayoff += (todaylist[i]->stop - searchDate);
					}
					if ((todaylist[i]->start >= searchDate) && (todaylist[i]->stop <= dayend))
					{
						dayoff += todaylist[i]->ts();
					}
					if ((todaylist[i]->start < dayend) && (todaylist[i]->stop > dayend))
					{
						dayoff += (dayend - todaylist[i]->start);
					}
				}
				return dayoff;
			}

			std::vector<OffdutySession*> AuditLogic::Get_Offduty_Session_TodayList(std::vector<OffdutySession*> &sessions, DateTime date)
			{
				std::vector<OffdutySession*> todaylist;
				for (auto os : sessions)
				{
					if (os->stop.Date() == date)
					{
						todaylist.push_back(os);
					}
					else if (os->start.Date() == date)
					{
						todaylist.push_back(os);
					}
				}
				return todaylist;
			}

			std::vector<AuditTime*> AuditLogic::Audit_CA_ShiftRule(std::vector<Timelog*> &data, std::vector<OffdutySession*> &offduties, DateTime &shiftStartDt)
			{
				DateTime end = TimeUtil::CurrentTime();
				std::vector<AuditTime*> audits;
				std::vector<OffdutySession*> sleeps;
				TimeSpan lastslp = TimeSpan(0);
				//Find work shift start
				DateTime shiftstart = CA_FindShiftStartByOffduty(data);
				//Find work shift start for sleep
				DateTime shiftstart2 = FindShiftStartBySleeper(data, 10, lastslp);
				if (shiftstart < shiftstart2)
				{
					shiftstart = shiftstart2;
				}
				if (shiftstart == DateTime::MinValue)
				{
					shiftstart = end;
					for (int i = 0; i < data.size(); i++)
					{
						if ((data[i]->duty != DutyStatus::Offduty) || (data[i]->duty != DutyStatus::Sleeping))
						{
							shiftstart = data[i]->Logtime;
							break;
						}
					}
				}
				shiftStartDt = shiftstart;
				audits.push_back(Audit_CA_Shift_16HourRule(data, end, shiftstart, 16, lastslp));
				audits.push_back(Audit_CA_Shift_DrivingRule(data, end, shiftstart, 13));
				audits.push_back(Audit_CA_Shift_OnDutyRule(data, end, shiftstart, 14));

				return audits;
			}

			AuditTime *AuditLogic::Audit_CA_Shift_DrivingRule(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit)
			{
				AuditTime *at = MKR_NEW AuditTime();
				at->rule = RULEID::CA_Driving;
				at->DispName = "Driving";
				at->ViolationDescription = "Driving time limit for work shift exceeded";
				at->ruletype = RuleType::Shift;
				at->available = TimeSpan(limit, 0, 0);
				std::vector<DutyStatus> required;
				required.push_back(DutyStatus::Driving);
				TimeSpan driving = CalculateHours(data, required, shiftstart, end);
				at->available = TimeSpan(limit, 0, 0) - driving;
				at->RuleDescription = string_format("Your are about to exceed driving time limit for work shift in ", Double2Str(round(at->available.TotalMinutes())).c_str(), " minute(s)");
				at->used = driving;
				return at;
			}

			AuditTime *AuditLogic::Audit_CA_Shift_OnDutyRule(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit)
			{
				AuditTime *at = MKR_NEW AuditTime();
				at->DispName = "On Duty";
				at->ViolationDescription = "On duty time limit for work shift exceeded";
				at->rule = RULEID::CA_OnDuty;
				at->ruletype = RuleType::Shift;
				at->available = TimeSpan(limit, 0, 0);
				std::vector<DutyStatus> required;
				required.push_back(DutyStatus::Driving);
				required.push_back(DutyStatus::Onduty);
				TimeSpan onduty = CalculateHours(data, required, shiftstart, end); // MKR_NEW TimeSpan(0);
				at->available = TimeSpan(limit, 0, 0) - onduty;
				at->ondutyavailable = at->available;
				at->RuleDescription = string_format("Your are about to exceed on duty time limit for work shift in %s %s", Double2Str(round(at->available.TotalMinutes())).c_str(), " minute(s)");
				;
				at->used = onduty;
				return at;

			}

			AuditTime *AuditLogic::Audit_CA_Shift_16HourRule(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit, TimeSpan lastslp)
			{
				Timelog *lasttl = nullptr;
				Timelog *tl = nullptr;
				AuditTime *at = MKR_NEW AuditTime();
				at->rule = RULEID::CA_16Hour;
				at->ruletype = RuleType::Shift;
				at->used = (end - shiftstart) - lastslp;
				at->available = TimeSpan(limit, 0, 0) - at->used;
				at->DispName = "Elapsed";
				at->ViolationDescription = "Elapsed work shift hours exceeded";
				at->RuleDescription = string_format("Your are about to exceed elapsed work shift hours in %s %s", Double2Str(round(at->available.TotalMinutes())).c_str(), " minute(s)");
				return at;
			}

			std::vector<AuditTime*> AuditLogic::Audit_CA_DayRule(std::vector<Timelog*> &data, double drivinglimit, double ondutylimit, DateTime dayStart)
			{
				std::vector<AuditTime*> violations;
				Timelog *lasttl = nullptr;
				//DateTime start = TimeUtil.CurrentTime.Date;

				DateTime start = dayStart;
				DateTime end = TimeUtil::CurrentTime(); // date.Date.AddDays(1);

				TimeSpan onduty = TimeSpan(0);
				TimeSpan driving = TimeSpan(0);
				for (auto tl : data)
				{
					if (lasttl == nullptr)
					{
						lasttl = tl;
						continue;
					}
					if (tl->Logtime < start)
					{
						lasttl = tl;
						continue;
					}
					if ((tl->Logtime >= start) && (lasttl->Logtime < start))
					{
						
						lasttl = lasttl->Clone();   // memory leak !!!
						lasttl->Logtime = start;
					}

					TimeSpan ts = tl->Logtime - lasttl->Logtime;
					if ((lasttl->duty == DutyStatus::Onduty) || (lasttl->duty == DutyStatus::Driving))
					{
						if (lasttl->duty == DutyStatus::Driving)
						{
							driving += ts;
						}
						onduty += ts;
					}
					if (lasttl != 0&&lasttl->isClone())
					{
						delete lasttl;  // to prevent memory leak
						//lasttl = 0;
					}
					lasttl = tl;
				}

				TimeSpan ts2 = end - lasttl->Logtime;
				if ((lasttl->duty == DutyStatus::Onduty) || (lasttl->duty == DutyStatus::Driving))
				{
					if (lasttl->duty == DutyStatus::Driving)
					{
						driving += ts2;
					}
					onduty += ts2;
				}
				AuditTime *at = MKR_NEW AuditTime();
				at->rule = RULEID::CA_Driving;
				at->DispName = "Driving";
				at->ViolationDescription = "Daily driving time limit exceeded";
				at->ruletype = RuleType::Day;
				at->available = TimeSpan(static_cast<int>(drivinglimit), 0, 0) - driving;
				at->RuleDescription = string_format("Your are about to exceed daily driving time limit in %s %s", Double2Str(round(at->available.TotalMinutes())).c_str(), " minute(s)");
				at->used = driving;
				violations.push_back(at);
				at = MKR_NEW AuditTime();
				at->DispName = "On Duty";
				at->ViolationDescription = "Daily on duty time limit exceeded";
				at->available = TimeSpan(static_cast<int>(ondutylimit), 0, 0) - onduty;
				at->rule = RULEID::CA_OnDuty;
				at->ondutyavailable = at->available;
				at->ruletype = RuleType::Day;
				at->used = onduty;
				at->RuleDescription = string_format("Your are about to exceed daily on duty time limit in %s %s", Double2Str(round(at->available.TotalMinutes())).c_str(), " minute(s)");
				violations.push_back(at);
				return violations;
			}

			DeferHourRow *AuditLogic::CheckPrevDayDeferCondition(std::vector<OffdutySession*> &offduties, std::vector<Timelog*> &data, double offdutylimit, Cycle *cycle, DateTime dayStart)
			{
				DateTime prevDay = dayStart.AddHours(-24);
				DateTime end = prevDay.AddDays(1);

				DeferHourRow *dhr = FindExistingDeferHour(prevDay);
				if (dhr != nullptr && dhr->day2.Date() == prevDay.Date())
				{
					return nullptr;
				}
				//if (dhr != null) return null; //it must be day2 of prev days
				DateTime timemark = TimeUtil::CurrentTime();
				double corehours = 0;
				DateTime corehoursStop = DateTime::MinValue;
				TimeSpan offduty = GetTotalOffdutyInDate(offduties, prevDay, end, timemark, corehours, corehoursStop);
				if (corehours < 8)
				{
					if (dhr != nullptr)
					{
						this->hc->DeleteDeferHour(dhr, CurrentAuditDriver->id(), CurrentDefers);
					}
					return nullptr;
				}
				double delta = offdutylimit - offduty.TotalHours();
				if ((delta > 0) && (delta <= 2))
				{
					//if (CheckDeferDay1New(data, prevDay, cycle))
					if (CheckDeferDay1(offduties, data, prevDay, cycle, delta, false))
					{
						if (dhr == nullptr)
						{
							dhr = MKR_NEW DeferHourRow();
							dhr->day1 = corehoursStop;
							dhr->day2 = dhr->day1.AddDays(1);
							dhr->defers = TimeSpan(int(delta), int((delta - int(delta)) * 60), 0);
							this->hc->AddDeferHour(dhr, CurrentAuditDriver->id(), CurrentDefers);
						}
						else
						{
							dhr->defers = TimeSpan(static_cast<int>(delta), static_cast<int>((delta - static_cast<int>(delta)) * 60), 0);
						}
					}
					else
					{
						this->hc->DeleteDeferHour(dhr, CurrentAuditDriver->id(), CurrentDefers);
						return nullptr;
					}
				}
				else
				{
					if (dhr != nullptr)
					{
						this->hc->DeleteDeferHour(dhr, CurrentAuditDriver->id(), CurrentDefers);
					}
					return nullptr;
				}
				return dhr;
			}

			TimeSpan AuditLogic::FindOffDutyToEnd()
			{
				TimeSpan totalHr = TimeSpan(0);
				DateTime offDateTime = DateTime::MinValue;
				int size = CurrentAuditDriverTimeLogs->list_size();
				TimeLogRow *tlr =(TimeLogRow *) CurrentAuditDriverTimeLogs->mutable_list(size - 1);
				if (tlr!=NULL && (tlr->Event() == LOGSTATUS::OffDuty || tlr->Event() == LOGSTATUS::Sleeping))
				{
					for (int index = size - 1; index >= 0; index--)
					{
						tlr = (TimeLogRow *)CurrentAuditDriverTimeLogs->mutable_list(index);
						if (tlr->Event() == LOGSTATUS::OffDuty || tlr->Event() == LOGSTATUS::Sleeping)

						{
							offDateTime = tlr->LogTime();
						}
						else
						{
							break;
						}
					}
				}

				if (offDateTime != DateTime::MinValue && (TimeUtil::CurrentTime() - offDateTime).TotalMinutes() <= 15)
				{
					totalHr = TimeUtil::CurrentTime().Date().AddHours(24) - offDateTime;
				}
				return totalHr;
			}

			AuditTime *AuditLogic::Audit_CA_Offduty_DayRule(std::vector<OffdutySession*> &offduties, std::vector<Timelog*> &data, double offdutylimit, Cycle *cycle, DateTime dayStart)
			{
				AuditTime *at = MKR_NEW AuditTime();
				at->rule = RULEID::CA_OffDuty;
				at->DispName = "Off duty";
				at->ViolationDescription = "Off-duty time is less than mandatory hours";
				at->ruletype = RuleType::Day;

				DateTime date = dayStart.Date();
				DateTime timemark = date;
				DateTime end = TimeUtil::CurrentTime();
				double corehours = 0;
				DateTime corehoursStop = DateTime::MinValue;
				TimeSpan offduty = GetTotalOffdutyInDate(offduties, date, end, timemark, corehours, corehoursStop);
				at->used = offduty;

				DeferHourRow *dhr_today = FindExistingDeferHour(dayStart.Date());
				DeferHourRow *dhr = CheckPrevDayDeferCondition(offduties, data, offdutylimit, cycle, dayStart);
				if (dhr != nullptr && dhr->day2.Date() == date)
				{
					if (dhr_today != nullptr && dhr_today->day1.Date() == date)
					{
						this->hc->DeleteDeferHour(dhr_today, CurrentAuditDriver->id(), CurrentDefers);
					}
				}
				else
				{
					if (dhr == nullptr)
					{
						dhr = dhr_today;
					}
				}
				//}
				double delta = 0;
				if (dhr != nullptr)
				{
					if (dhr->day2.Date() == dayStart.Date())
					{
						double corehoursPre = 0;
						DateTime corehoursStopPre = DateTime::MinValue;
						TimeSpan prevday = GetTotalOffdutyInDate(offduties, date.AddHours(-24), date, timemark, corehoursPre, corehoursStopPre);
						delta = 10 - (prevday.TotalHours());
						if (corehoursPre < 8 || delta <= 0 || delta > 2)
						{
							dhr->defers = TimeSpan(0);
						}
						if (corehoursPre >= 8 && delta > 0 && delta <= 2)
						{
							int delta_h = static_cast<int>(floor(delta));
							dhr->defers = TimeSpan(delta_h, static_cast<int>((delta - delta_h) * 60), 0);
							offdutylimit += delta;
						}
					}
				}
				double dfhours = 0;
				if (offduty.TotalHours() >= offdutylimit && corehours >= (8 + delta))
				{
					at->available = TimeSpan(13, 0, 0);
					at->notNeedCal = true;
				}
				else
				{
					TimeSpan left = dayStart.AddHours(24) - TimeUtil::CurrentTime();
					TimeSpan offLeft = FindOffDutyToEnd();
					if (offLeft.TotalSeconds() != 0)
					{
						left = offLeft;
					}
					double limiteLeft = offduty.TotalHours() - (delta + 8);
					//core hours passed already
					int _size = CurrentAuditDriverTimeLogs->list_size();
					TimeLogRow *tlr = (TimeLogRow *)CurrentAuditDriverTimeLogs->mutable_list(_size - 1);
					if (corehours >= delta + 8)
					{
						if ((left.TotalHours() > (2 - limiteLeft)) || (left.TotalHours() == (2 - limiteLeft) && (tlr->Event() == LOGSTATUS::OffDuty || tlr->Event() == LOGSTATUS::Sleeping)))
						{
							double available = left.TotalHours() - (2 - limiteLeft);
							int available_h = static_cast<int>(floor(available));
							at->available = TimeSpan(available_h, static_cast<int>((available - available_h) * 60), 0);
						}
						else
						{
							at->available = TimeSpan(0);
							//11.16
							if ((tlr->Event() == LOGSTATUS::OnDuty || tlr->Event() == LOGSTATUS::Driving) || left.TotalMinutes() < 30)
							{
								dfhours = (2 - limiteLeft);
							}
							else
							{
								dfhours = (2 - limiteLeft - left.TotalHours());
							}
						}
					}
					else
					{
						//Get last off duty hours
						TimeSpan lastoffdutyHour = TimeSpan(0);
						if (offduties.size() > 0 && offduties.size() > 0 && offduties[offduties.size() - 1]->stop == EndOffduty)
						{
							lastoffdutyHour = offduties[offduties.size() - 1]->ts() + (dayStart.AddHours(24) - offduties[offduties.size() - 1]->stop);
						}
						else
						{
							lastoffdutyHour = (dayStart.AddHours(24) - TimeUtil::CurrentTime());
						}
						if (dhr != nullptr && dhr->day2.Date() == date) //Defer need core hours
						{
							if (lastoffdutyHour.TotalHours() >= delta + 8)
							{
								double offdutytoend = (offduty + (dayStart.AddHours(24) - TimeUtil::CurrentTime())).TotalHours();
								if (offdutytoend >= offdutylimit)
								{
									//double available = lastoffdutyHour.TotalHours - (delta + 8);
									double available = offdutytoend - offdutylimit;
									int available_h = static_cast<int>(floor(available));
									at->available = TimeSpan(available_h, static_cast<int>((available - available_h) * 60), 0);
								}
								else
								{
									at->available = TimeSpan(0);
								}
							}
							else
							{
								//No core hours  
								at->available = TimeSpan(0);
							}
						}
						else
						{
							double offdutytoend = (offduty + (dayStart.AddHours(24) - TimeUtil::CurrentTime())).TotalHours();
							if (offdutytoend >= offdutylimit)
							{
								//double available = lastoffdutyHour.TotalHours - (delta + 8);
								double available = offdutytoend - offdutylimit;
								int available_h = static_cast<int>(floor(available));
								at->available = TimeSpan(available_h, static_cast<int>((available - available_h) * 60), 0);
							}
							else
							{
								at->available = TimeSpan(0);
							}
						}
					}
				}

				if (dhr != nullptr)
				{
					if (dhr->day1.Date() == dayStart.Date())
					{
						if (dfhours > 0)
						{
							if (CheckDeferDay1(offduties, data, dayStart, cycle, dfhours, true))
							{
								int dfhours_h = static_cast<int>(floor(dfhours));
								dhr->defers = TimeSpan(dfhours_h, static_cast<int>((dfhours - dfhours_h) * 60), 0);
							}
							else
							{
								this->hc->DeleteDeferHour(dhr, CurrentAuditDriver->id(), CurrentDefers);
							}
						}
						else
						{
							this->hc->DeleteDeferHour(dhr, CurrentAuditDriver->id(), CurrentDefers);
						}
					}
				}
				else
				{
					if (dfhours > 0)
					{
						//11.16
						if (CheckDeferDay1(offduties, data, dayStart, cycle, dfhours, true))
						{
							dhr = MKR_NEW DeferHourRow();
							dhr->day1 = corehoursStop;
							dhr->day2 = dhr->day1.AddDays(1);
							int dfhours_h = static_cast<int>(floor(dfhours));
							//11.16
							dhr->defers = TimeSpan(dfhours_h, static_cast<int>((dfhours - dfhours_h) * 60), 0);
							this->hc->AddDeferHour(dhr, CurrentAuditDriver->id(), CurrentDefers);
						}
					}
				}
				if (at->available.TotalHours() == 13)
				{
					at->ondutyavailable = TimeSpan(14, 0, 0);
				}
				else
				{
					at->ondutyavailable = at->available;
				}
				at->RuleDescription = string_format("You are about to violate minimum mandatory off-duty time requirement in &s  %s", Double2Str(round(at->ondutyavailable.TotalMinutes())).c_str(), " minute(s)");
				return at;
			}

			TimeSpan AuditLogic::GetTotalOffdutyInDate(std::vector<OffdutySession*> &offduties, DateTime start, DateTime end, DateTime &timemark, double &corehours, DateTime &corehoursStop)
			{
				corehours = 0;
				TimeSpan offduty = TimeSpan(0);
				for (OffdutySession *os : offduties)
				{
					if (os->stop <= start)
					{
						continue;
					}
					if (os->start >= end)
					{
						break;
					}
					if (os->stop <= end)
					{
						if (os->start < start)
						{
							offduty = os->ts() - (start - os->start);
							timemark = os->stop;
						}
						else
						{
							//offduty += os->ts();
							TimeSpan ts = os->ts();
							offduty += ts;

							timemark = os->start;
						}
					}
					if (os->stop > end)
					{
						if (os->start <= start)
						{
							offduty = TimeSpan(24, 0, 0);
							timemark = start;
						}
						else
						{
							timemark = os->start;
							offduty += end - os->start;
						}
					}

					//devin 2012-08-17
					if (os->start < start && os->stop > start && os->stop <= end)
					{
						double todayOffHours = os->ts().TotalHours() - (start - os->start).TotalHours();

						if (todayOffHours >= 8 && corehours < todayOffHours)
						{
							corehours = todayOffHours;
							corehoursStop = os->stop;
						}
					}

					//devin 11.16
					if (os->start < start && os->stop > end)
					{
						corehours = (TimeSpan(24, 0, 0)).TotalHours();
						corehoursStop = os->stop;
					}
					if (os->start >= start && os->stop <= end && os->ts().TotalHours() >= 8 && corehours < os->ts().TotalHours())
					{
						corehours = os->ts().TotalHours();
						corehoursStop = os->stop;
					}
					if (os->start >= start && os->stop > end)
					{
						double todayOffHours = (end - os->start).TotalHours();

						if (todayOffHours >= 8 && corehours < todayOffHours)
						{
							corehours = todayOffHours;
							corehoursStop = os->stop;
						}
					}
				}
				return offduty;
			}

			TimeSpan AuditLogic::GetTotalDrivingTimeInDayRange(std::vector<Timelog*> &data, DateTime start, DateTime end)
			{
				Timelog *tl = nullptr;
				Timelog *lasttl = nullptr;
				TimeSpan driving = TimeSpan(0);
				for (int i = 0; i < data.size(); i++)
				{
					tl = data[i];
					if (tl->Logtime < start)
					{
						lasttl = tl;
						continue;
					}
					if (tl->Logtime > end)
					{
						break;
					}
					TimeSpan ts = tl->Logtime - lasttl->Logtime;
					if (lasttl->duty == DutyStatus::Driving)
					{
						if (lasttl->Logtime < start)
						{
							ts = tl->Logtime - start;
						}
						driving += ts;
					}
					lasttl = tl;
				}
				if (tl == nullptr)
				{
					return driving;
				}
				if ((tl->Logtime > end) && (lasttl->duty == DutyStatus::Driving))
				{
					driving += end - lasttl->Logtime;
				}
				return driving;
			}

			bool AuditLogic::CheckDeferDay1New(std::vector<Timelog*> &data, DateTime date, Cycle *cycle)
			{
				bool found = true;
				if (data.empty())
				{
					return false;
				}
				for (int i = data.size() - 1; i >= 0; i--)
				{
					Timelog *tl = data[i];
					if (tl->Logtime > date.AddHours(24))
					{
						continue;
					}
					if (tl->Location != AuditJurisdiction::Canada)
					{
						return false;
					}
					break;
				}
				//Devin cycle
				if (cycle == nullptr)
				{
					return false;
				}
				if ((cycle->rule == RULEID::CA_Service_12014) || (cycle->rule == RULEID::CA_Service_707))
				{
					return found;
				}
				return false;
			}

			bool AuditLogic::CheckDeferDay1(std::vector<OffdutySession*> &offduties, std::vector<Timelog*> &data, DateTime date, Cycle *cycle, double dfhours, bool isToday)
			{
				bool found = true;
				if (data.empty())
				{
					return false;
				}
				//for (int i = 0; i < data.Count; i++)
				DateTime driverLastTime = DateTime::MinValue;
				DateTime dayEnd = date.AddHours(24);
				for (int i = data.size() - 1; i >= 0; i--)
				{
					Timelog *tl = data[i];
					if (tl->Logtime > date.AddHours(24))
					{
						continue;
					}
					if (tl->Location != AuditJurisdiction::Canada)
					{
						return false;
					}
					break;
				}
				if (isToday)
				{
					for (int i = data.size() - 1; i >= 0; i--)
					{
						Timelog *tl = data[i];
						if (tl->Logtime > date.AddHours(24))
						{
							continue;
						}
						if (tl->Logtime < date)
						{
							break;
						}
						if (tl->duty == DutyStatus::Driving || tl->duty == DutyStatus::Onduty)
						{
							if (i == data.size() - 1)
							{
								driverLastTime = date.AddHours(24);
							}
							else
							{
								if (tl->Logtime != data[i + 1]->Logtime)
								{
									driverLastTime = data[i + 1]->Logtime;
								}
							}
							if (driverLastTime != DateTime::MinValue)
							{
								break;
							}
						}
					}
					if (driverLastTime == DateTime::MinValue)
					{
						return false;
					}
					double off_duty = 0;
					for (int i = offduties.size() - 1; i >= 0; i--)
					{
						OffdutySession *os = offduties[i];
						if (os->start >= driverLastTime && os->start <= dayEnd)
						{
							if (os->stop >= dayEnd)
							{
								off_duty = off_duty + dayEnd.Subtract(os->start).TotalHours();
							}
							else
							{
								off_duty = off_duty + os->ts().TotalHours();
							}
						}
						else
						{
							break;
						}
					}

					int _size = CurrentAuditDriverTimeLogs->list_size();
					TimeLogRow *tlr = (TimeLogRow *)CurrentAuditDriverTimeLogs->mutable_list(_size - 1);
					if ((tlr->Event() == LOGSTATUS::OffDuty || tlr->Event() == LOGSTATUS::Sleeping))
					{
						off_duty = off_duty + (dayEnd - tlr->LogTime()).TotalHours();
					}


					if ((dayEnd.Subtract(driverLastTime).TotalHours() - off_duty) > dfhours)
					{
						return false;
					}

				}

				if (cycle == nullptr)
				{
					return false;
				}
				if ((cycle->rule == RULEID::CA_Service_12014) || (cycle->rule == RULEID::CA_Service_707))
				{
					return found;
				}
				return false;
			}

			std::vector<AuditTime*> AuditLogic::Audit_CA_North(std::vector<Timelog*> &data, int drivinglimit, int ondutylimit, int elapslimit)
			{
				std::vector<AuditTime*> violations;

				TimeSpan driving = TimeSpan(0);
				TimeSpan onduty = TimeSpan(0);
				TimeSpan elaps = TimeSpan(0);
				TimeSpan offduty = TimeSpan(0);
				TimeSpan lastslp = TimeSpan(0);
				DateTime dt;
				DateTime shiftstart = FindShiftStart(data, 8, dt);
				DateTime shiftSleep = FindShiftStartBySleeper(data, 8, lastslp);
				if (shiftstart < shiftSleep)
				{
					shiftstart = shiftSleep;
				}

				TimeSpan ts;
				Timelog *lasttl = nullptr;
				for (auto tl : data)
				{
					if (tl->Logtime < shiftstart)
					{
						continue;
					}
					if (ShiftStartOdometer == 0 && tl->odometer > 0)
					{
						ShiftStartOdometer = tl->odometer;
					}
					if (tl->Latitude != 0 && tl->Longitude != 0 && homeLatitude == 0 && homeLogitude == 0) //radius rule
					{
						homeLatitude = tl->Latitude;
						homeLogitude = tl->Longitude;
					}
					if (lasttl == nullptr)
					{
						lasttl = tl;
						continue;
					}
					ts = tl->Logtime - lasttl->Logtime;
					if ((lasttl->duty == DutyStatus::Onduty) || (lasttl->duty == DutyStatus::Driving))
					{
						if (lasttl->duty == DutyStatus::Driving)
						{
							driving += ts;
						}
						onduty += ts;
					}
					elaps += ts;
					lasttl = tl;
				}

				if (lasttl != nullptr)
				{
					ts = TimeUtil::CurrentTime() - lasttl->Logtime;
					if ((lasttl->duty == DutyStatus::Onduty) || (lasttl->duty == DutyStatus::Driving))
					{
						if (lasttl->duty == DutyStatus::Driving)
						{
							driving += ts;
						}
						onduty += ts;
					}
				}
				AuditTime *at = MKR_NEW AuditTime();
				at->ruletype = RuleType::Shift;
				at->DispName = "Elapsed";
				at->ViolationDescription = "Elapsed work shift hours exceeded";
				at->rule = RULEID::CA_16Hour;
				at->used = TimeUtil::CurrentTime() - shiftstart - lastslp;
				at->available = TimeSpan(elapslimit, 0, 0) - at->used;
				at->RuleDescription = string_format("Your are about to exceed elapsed work shift hours in %s %s", Double2Str(round(at->available.TotalMinutes())).c_str(), " minute(s)");
				violations.push_back(at);
				at = MKR_NEW AuditTime();
				at->ruletype = RuleType::Shift;
				at->rule = RULEID::CA_Driving;
				at->DispName = "Driving";
				at->ViolationDescription = "Driving time limit for work shift exceeded";
				at->used = driving;
				at->available = TimeSpan(drivinglimit, 0, 0) - driving;
				at->RuleDescription = string_format("Your are about to exceed driving time limit for work shift in %s %s", Double2Str(round(at->available.TotalMinutes())).c_str(), " minute(s)");
				violations.push_back(at);
				at = MKR_NEW AuditTime();
				at->ruletype = RuleType::Shift;
				at->rule = RULEID::CA_OnDuty;
				at->DispName = "On Duty";
				at->ViolationDescription = "On duty time limit for work shift exceeded";
				at->used = onduty;
				at->available = TimeSpan(ondutylimit, 0, 0) - onduty;
				at->RuleDescription = string_format("Your are about to exceed on duty time limit for work shift in %s %s", Double2Str(round(at->available.TotalMinutes())).c_str(), " minute(s)");
				at->ondutyavailable = at->available;
				violations.push_back(at);

				return violations;
			}

			std::vector<OffdutySession*> AuditLogic::Audit_CA_MergeSleep(std::vector<Timelog*> &data, DateTime end)
			{
				std::vector<OffdutySession*> sessions;
				for (int i = 0; i < data.size(); i++)
				{
					if (data[i]->duty == DutyStatus::Sleeping)
					{
						OffdutySession *os = MKR_NEW OffdutySession();
						os->start=(data[i]->Logtime);
						os->prev = DutyStatus::Offduty;
						if (i > 0)
						{
							os->prev = data[i - 1]->duty;
						}
						if (i < (data.size() - 1))
						{
							os->stop=(data[i + 1]->Logtime);
						}
						else
						{
							os->stop=(end);
						}
						sessions.push_back(os);
						if (os->stop > end)
						{
							os->stop=(end);
							break;
						}
					}
				}
				for (int i = sessions.size() - 1; i >= 1; i--)
				{
					if ((sessions[i]->start == sessions[i - 1]->stop) && (sessions[i]->prev == DutyStatus::Sleeping))
					{
						sessions[i - 1]->stop=(sessions[i]->stop);
						OffdutySession *p = sessions[i];
						if (p != 0) delete p;
						stdH::removeItemAtIndex(sessions, i);
					}
				}
				if ((sessions.size() > 0) && (sessions[sessions.size() - 1]->stop > end))
				{
					sessions.pop_back();
				}
				return sessions;
			}

#pragma endregion
#pragma region Recap_AuditLogic_Cpp

			std::vector<RecapRowAudit*> AuditLogic::CalculateRecap(AuditRequest *ar, RULEID &rule, std::vector<RuleExceptions> &ruleException)
			{
				std::vector<RecapRowAudit*> result;
				RecapRowAudit *rr = nullptr;
				Timelog *lasttl = nullptr;

				if (!AuditLogic::NeedtoCalculateRecap && IsFromTimer && ar->timelog.size() > 0)
				{
					//only do today if there is nothing changed for duty status
					lasttl = ar->timelog[ar->timelog.size() - 1];
				}
				else
				{
					for (Timelog *tl : ar->timelog)
					{
						if (lasttl == nullptr)
						{
							lasttl = tl;
							continue;
						}
						if (lasttl->Logtime.Date() != tl->Logtime.Date())
						{
							rr = ApplyRule(ar, lasttl, rule, ruleException);
							result.push_back(rr);
						}
						lasttl = tl;
					}
				}
				rr = ApplyRule(ar, lasttl, rule, ruleException);
				result.push_back(rr);
				AuditLogic::NeedtoCalculateRecap = false;
				return result;
			}

			RecapRowAudit *AuditLogic::ApplyRule(AuditRequest *ar, Timelog *lasttl, RULEID &rid, std::vector<RuleExceptions> &ruleException)
			{
				rid = FindApplyingRule(lasttl, ar->CycleList, ruleException);
				//get recap.CycleAvail
				//Log.Debug ("DEV","===rid:"+rid.ToString());
				if (rid == RULEID::US_Service_607)
				{
					if (lasttl->Latitude < 60)
					{
						return US607(ar->timelog, lasttl->Logtime, ruleException);
					}
					if (lasttl->Latitude >= 60)
					{
						return US607North(ar->timelog, lasttl->Logtime, ruleException);
					}
				}
				else if (rid == RULEID::US_Service_708)
				{
					if (lasttl->Latitude < 60)
					{
						return US708(ar->timelog, lasttl->Logtime, ruleException);
					}
					if (lasttl->Latitude >= 60)
					{
						return US708North(ar->timelog, lasttl->Logtime, ruleException);
					}
				}
				else if (rid == RULEID::CA_Service_707)
				{
					if (lasttl->Latitude < 60)
					{
						return CA707(ar->timelog, lasttl->Logtime, ruleException);
					}
					if (lasttl->Latitude >= 60)
					{
						return CA707North(ar->timelog, lasttl->Logtime, ruleException);
					}
				}
				else if (rid == RULEID::CA_Service_12014)
				{
					if (lasttl->Latitude < 60)
					{
						return CA12014(ar->timelog, lasttl->Logtime, ruleException);
					}
					if (lasttl->Latitude >= 60)
					{
						return CA12014North(ar->timelog, lasttl->Logtime, ruleException);
					}
				}
				else //oilfield rule
				{
					if (rid == RULEID::CA_OilField)
					{
						return CAOilfield(ar->timelog, lasttl->Logtime);
					}
					if (rid == RULEID::US_OilField)
					{
						return USOilfield(ar->timelog, lasttl->Logtime);
					}
					if (rid == RULEID::US_Texas)
					{
						return USTexas(ar->timelog, lasttl->Logtime, ruleException);
					}
					if (rid == RULEID::US_California)
					{
						return USCalifornia(ar->timelog, lasttl->Logtime, ruleException);
					}
					if (rid == RULEID::US_Florida_707)
					{
						return USFlorida707(ar->timelog, lasttl->Logtime, ruleException);
					}
					if (rid == RULEID::US_Florida_808)
					{
						return USFlorida808(ar->timelog, lasttl->Logtime, ruleException);
					}
					if (rid == RULEID::US_Alaskan_707)
					{
						return US607North(ar->timelog, lasttl->Logtime, ruleException);
					}
					if (rid == RULEID::US_Alaskan_808)
					{
						return US708North(ar->timelog, lasttl->Logtime, ruleException);
					}
					if (rid == RULEID::US_NewJersey_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_NewJersey_707), true);
					}
					if (rid == RULEID::US_Georgia_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_Georgia_707), true);
					}
					if (rid == RULEID::US_NewJersey_808)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 80, 34, Cycle2Str(HOSCYCLE::US_NewJersey_808), true);
					}
					if (rid == RULEID::US_Georgia_808)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 80, 34, Cycle2Str(HOSCYCLE::US_Georgia_808), true);
					}
					if (rid == RULEID::US_Alabama_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Alabama_607), true);
					}
					if (rid == RULEID::US_Alabama_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Alabama_708), true);
					}
					if (rid == RULEID::US_Arizona_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Arizona_607), true);
					}
					if (rid == RULEID::US_Arizona_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Arizona_708), true);
					}
					if (rid == RULEID::US_Connecticut_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Connecticut_607), true);
					}
					if (rid == RULEID::US_Connecticut_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Connecticut_708), true);
					}
					if (rid == RULEID::US_Delaware_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Delaware_607), true);
					}
					if (rid == RULEID::US_Delaware_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Delaware_708), true);
					}
					if (rid == RULEID::US_Idaho_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Idaho_607), true);
					}
					if (rid == RULEID::US_Idaho_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Idaho_708), true);
					}
					if (rid == RULEID::US_Indiana_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Indiana_607), true);
					}
					if (rid == RULEID::US_Indiana_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Indiana_708), true);
					}
					if (rid == RULEID::US_Iowa_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_Iowa_707), true);
					}
					if (rid == RULEID::US_Iowa_808)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 80, 34, Cycle2Str(HOSCYCLE::US_Iowa_808), true);
					}
					if (rid == RULEID::US_Kansas_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Kansas_607), true);
					}
					if (rid == RULEID::US_Kansas_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Kansas_708), true);
					}
					if (rid == RULEID::US_Louisiana_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Louisiana_607), true);
					}
					if (rid == RULEID::US_Louisiana_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Louisiana_708), true);
					}
					if (rid == RULEID::US_Maryland_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_Maryland_707), true);
					}
					if (rid == RULEID::US_Maryland_808)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 80, 34, Cycle2Str(HOSCYCLE::US_Maryland_808), true);
					}
					if (rid == RULEID::US_Michigan_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Michigan_607), true);
					}
					if (rid == RULEID::US_Michigan_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Michigan_708), true);
					}
					if (rid == RULEID::US_Mississippi_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Mississippi_607), true);
					}
					if (rid == RULEID::US_Michigan_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Mississippi_708), true);
					}
					if (rid == RULEID::US_Missouri_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Missouri_607), true);
					}
					if (rid == RULEID::US_Missouri_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Missouri_708), true);
					}
					if (rid == RULEID::US_Montana_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Montana_607), true);
					}
					if (rid == RULEID::US_Montana_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Montana_708), true);
					}
					if (rid == RULEID::US_Nebraska_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_Nebraska_707), true);
					}
					if (rid == RULEID::US_Nebraska_808)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 80, 34, Cycle2Str(HOSCYCLE::US_Nebraska_808), true);
					}
					if (rid == RULEID::US_Nevada_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_Nevada_707), true);
					}
					if (rid == RULEID::US_NewMexico_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_NewMexico_607), true);
					}
					if (rid == RULEID::US_NewMexico_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_NewMexico_708), true);
					}
					if (rid == RULEID::US_NorthCarolina_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_NorthCarolina_707), true);
					}
					if (rid == RULEID::US_NorthCarolina_808)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 80, 34, Cycle2Str(HOSCYCLE::US_NorthCarolina_808), true);
					}
					if (rid == RULEID::US_NorthDakota_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_NorthDakota_707), true);
					}
					if (rid == RULEID::US_Ohio_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Ohio_607), true);
					}
					if (rid == RULEID::US_Ohio_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Ohio_708), true);
					}
					if (rid == RULEID::US_Oklahoma_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Oklahoma_607), true);
					}
					if (rid == RULEID::US_Oklahoma_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Oklahoma_708), true);
					}
					if (rid == RULEID::US_Oregon_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_Oregon_707), true);
					}
					if (rid == RULEID::US_Oregon_808)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 80, 34, Cycle2Str(HOSCYCLE::US_Oregon_808), true);
					}
					if (rid == RULEID::US_Pennsylvania_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Pennsylvania_607), true);
					}
					if (rid == RULEID::US_Pennsylvania_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Pennsylvania_708), true);
					}
					if (rid == RULEID::US_South_Dakota_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_South_Dakota_607), true);
					}
					if (rid == RULEID::US_South_Dakota_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_South_Dakota_708), true);
					}
					if (rid == RULEID::US_South_Dakota_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_South_Dakota_707), true);
					}
					if (rid == RULEID::US_South_Dakota_808)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 80, 34, Cycle2Str(HOSCYCLE::US_South_Dakota_808), true);
					}
					if (rid == RULEID::US_Tennessee_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Tennessee_607), true);
					}
					if (rid == RULEID::US_Tennessee_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Tennessee_708), true);
					}
					if (rid == RULEID::US_Vermont_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Vermont_607), true);
					}
					if (rid == RULEID::US_Vermont_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Vermont_708), true);
					}
					if (rid == RULEID::US_Virginia_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Virginia_607), true);
					}
					if (rid == RULEID::US_Virginia_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Virginia_708), true);
					}
					if (rid == RULEID::US_Wisconsin_707)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 70, 34, Cycle2Str(HOSCYCLE::US_Wisconsin_707), true);
					}
					if (rid == RULEID::US_Wisconsin_808)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 80, 34, Cycle2Str(HOSCYCLE::US_Wisconsin_808), true);
					}
					if (rid == RULEID::US_Wyoming_607)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 7, 60, 34, Cycle2Str(HOSCYCLE::US_Wyoming_607), true);
					}
					if (rid == RULEID::US_Wyoming_708)
					{
						
						return Cycle_Func(ar->timelog, lasttl->Logtime, 8, 70, 34, Cycle2Str(HOSCYCLE::US_Wyoming_708), true);
					}
				}
				return nullptr;
			}

			std::vector<RuleExceptions> AuditLogic::GetRuleExceptions(int rule)
			{
				std::vector<RuleExceptions> ruleExceptions;
				for (int bit = 0; bit < 32; ++bit)
				{
					if (((rule >> bit) & 1) == 1)
					{
						if (bit == 0)
						{
							ruleExceptions.push_back(static_cast<RuleExceptions>(bit + 1));
						}
						else
						{
							ruleExceptions.push_back(static_cast<RuleExceptions>(2 << (bit - 1)));
						}
					}
				}

				return ruleExceptions;
			}

			RULEID AuditLogic::FindApplyingRule(Timelog *tl, std::vector<Cycle*> &cyclelist, std::vector<RuleExceptions> &ruleException)
			{
				for (int i = cyclelist.size() - 1; i >= 0; i--)
				{
					if (((tl->Logtime >= cyclelist[i]->selectTime.Date()) && (i > 0)) || (i == 0))
					{
						ruleException = GetRuleExceptions(static_cast<int>(cyclelist[i]->ruleException));
						return cyclelist[i]->rule;
						
					}
				}
				return RULEID::CA_Service_707;
			}

			void AuditLogic::Calculate(Timelog *lasttl, DateTime tl, DateTime today, RecapRowAudit *&rr, TimeSpan &offtime, int resettime, DateTime &offDutyStart, bool isUSA, std::vector<Timelog*> &data, std::vector<DateTime> &previousLogs)
			{
				//svk fatal if lasttl=null
				if (lasttl == 0)
					return;

				TimeSpan ts = tl - lasttl->Logtime;
				if ((lasttl->duty == DutyStatus::Driving) || (lasttl->duty == DutyStatus::Onduty))
				{
					rr->CycleTotal += ts;
					if (lasttl->Logtime >= today)
					{
						rr->DayUsed += ts;
					}
					if (lasttl->Logtime < today && tl >= today)
					{
						rr->DayUsed += tl - today;
					}
					offtime = TimeSpan(0);
					offDutyStart = DateTime::MinValue;
				}
				else
				{
					if (offDutyStart == DateTime::MinValue)
					{
						offDutyStart = lasttl->Logtime;
					}
					offtime += ts;
					if (offtime.TotalHours() >= resettime)
					{
						if (!isUSA || Stop168hrsReset)
						{
							rr->CycleTotal = TimeSpan(0);
						}
						else
						{
							//34 hours restart has been removed from regulation
							if ((offDutyStart.Date().AddHours(1) >= offDutyStart && tl >= offDutyStart.Date().AddDays(1).AddHours(5)) || (offDutyStart.Date().AddDays(1).AddHours(1) >= offDutyStart && tl >= offDutyStart.Date().AddDays(2).AddHours(5)))
							{
								if (CheckPreviousCycleStart(data, offDutyStart, tl, resettime, previousLogs))
								{
									rr->CycleTotal = TimeSpan(0);
									previousLogs.push_back(offDutyStart);
								}
							}
						}
					}
				}
			}

			void AuditLogic::CalculateExt(Timelog *lasttl, DateTime tl, DateTime today, TimeSpan &offtime, int resettime, DateTime &offDutyStart, bool isUSA, std::vector<Timelog*> &data, std::vector<DateTime> &previousLogs)
			{
				TimeSpan ts = tl - lasttl->Logtime;
				if ((lasttl->duty == DutyStatus::Driving) || (lasttl->duty == DutyStatus::Onduty))
				{
					offtime = TimeSpan(0);
					offDutyStart = DateTime::MinValue;
				}
				else
				{
					if (offDutyStart == DateTime::MinValue)
					{
						offDutyStart = lasttl->Logtime;
					}
					offtime += ts;
					if (offtime.TotalHours() >= resettime)
					{
						if ((offDutyStart.Date().AddHours(1) >= offDutyStart && tl >= offDutyStart.Date().AddDays(1).AddHours(5)) || (offDutyStart.Date().AddDays(1).AddHours(1) >= offDutyStart && tl >= offDutyStart.Date().AddDays(2).AddHours(5)))
						{
							if (CheckPreviousCycleStart(data, offDutyStart, tl, resettime, previousLogs))
							{
								previousLogs.push_back(offDutyStart);
							}
						}
					}
				}
			}

			bool AuditLogic::CheckPreviousCycleStart(std::vector<Timelog*> &data, DateTime &offDutyStart, DateTime offDutyEnd, int resettime, std::vector<DateTime> &previousLogs)
			{
				if (previousLogs.size() > 0)
				{
					DateTime lastCycleTime = previousLogs[previousLogs.size() - 1].AddHours(168);
					if (lastCycleTime > offDutyStart)
					{
						if (!((lastCycleTime.Date().AddHours(1) >= lastCycleTime && offDutyEnd >= lastCycleTime.Date().AddDays(1).AddHours(5) && offDutyEnd.Subtract(lastCycleTime).TotalHours() >= resettime) || (offDutyEnd >= lastCycleTime.Date().AddDays(2).AddHours(5) && offDutyEnd.Subtract(lastCycleTime).TotalHours() >= resettime)))
						{
							return false;
						}
						else
						{
							offDutyStart = lastCycleTime; // MKR_NEW change 2014-09-16.  Because offduty start is after 168 hours from last reset start
						}
					}
				}
			
				return true;
			}

			RecapRowAudit *AuditLogic::US607(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_Oilfield_waiting_time) != ruleException.end())
				{
					
					return Cycle_Func(data, date, 8, 70, 24,Cycle2Str( HOSCYCLE::US_70hr_8day), false);
				}
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || stdH::Contains(ruleException,RuleExceptions::USA_Transportation_construction_Materialsandequipment))
				{
					
					return Cycle_Func(data, date, 7, 60, 24, Cycle2Str(HOSCYCLE::US_60hr_7day), false);
				}
				
				return Cycle_Func(data, date, 7, 60, 34, Cycle2Str(HOSCYCLE::US_60hr_7day), true);

			}

			RecapRowAudit *AuditLogic::US708(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || stdH::Contains(ruleException,RuleExceptions::USA_Transportation_construction_Materialsandequipment) || stdH::Contains(ruleException,RuleExceptions::USA_Oilfield_waiting_time))
				{
					
					return Cycle_Func(data, date, 8, 70, 24, Cycle2Str(HOSCYCLE::US_70hr_8day), false);
				}
				
				return Cycle_Func(data, date, 8, 70, 34, Cycle2Str(HOSCYCLE::US_70hr_8day), true);
			}

			RecapRowAudit *AuditLogic::US607North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_Oilfield_waiting_time) != ruleException.end())
				{
					
					return Cycle_Func(data, date, 8, 80, 24, Cycle2Str(HOSCYCLE::US_Alaskan_808), false);
				}
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || stdH::Contains(ruleException,RuleExceptions::USA_Transportation_construction_Materialsandequipment))
				{
					
					return Cycle_Func(data, date, 7, 70, 24, Cycle2Str(HOSCYCLE::US_Alaskan_707), false);
				}
				
				return Cycle_Func(data, date, 7, 70, 34, Cycle2Str(HOSCYCLE::US_Alaskan_707), true);
			}

			RecapRowAudit *AuditLogic::US708North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || stdH::Contains(ruleException,RuleExceptions::USA_Transportation_construction_Materialsandequipment) || stdH::Contains(ruleException,RuleExceptions::USA_Oilfield_waiting_time))
				{
					
					return Cycle_Func(data, date, 8, 80, 24, Cycle2Str(HOSCYCLE::US_Alaskan_808), false);
				}
				
				return Cycle_Func(data, date, 8, 80, 34, Cycle2Str(HOSCYCLE::US_Alaskan_808), true);
			}

			RecapRowAudit *AuditLogic::USOilfield(std::vector<Timelog*> &data, DateTime date)
			{
				
				return Cycle_Func(data, date, 8, 70, 24, Cycle2Str(HOSCYCLE::US_Oil_Field), false);
			}

			RecapRowAudit *AuditLogic::USTexas(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || stdH::Contains(ruleException,RuleExceptions::USA_Transportation_construction_Materialsandequipment) || stdH::Contains(ruleException,RuleExceptions::USA_Oilfield_waiting_time))
				{
					
					return Cycle_Func(data, date, 7, 70, 24, Cycle2Str(HOSCYCLE::US_Texas), false);
				}
				
				return Cycle_Func(data, date, 7, 70, 34, Cycle2Str(HOSCYCLE::US_Texas), true);
			}

			RecapRowAudit *AuditLogic::USCalifornia(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || stdH::Contains(ruleException,RuleExceptions::USA_Transportation_construction_Materialsandequipment) || stdH::Contains(ruleException,RuleExceptions::USA_Oilfield_waiting_time))
				{
					
					return Cycle_Func(data, date, 8, 80, 24, Cycle2Str(HOSCYCLE::US_California), false);
				}
				
				return Cycle_Func(data, date, 8, 80, 34, Cycle2Str(HOSCYCLE::US_California), true);
			}

			RecapRowAudit *AuditLogic::USFlorida707(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || stdH::Contains(ruleException,RuleExceptions::USA_Transportation_construction_Materialsandequipment) || stdH::Contains(ruleException,RuleExceptions::USA_Oilfield_waiting_time))
				{
					
					return Cycle_Func(data, date, 7, 70, 24, Cycle2Str(HOSCYCLE::US_Florida_707), false);
				}
				
				return Cycle_Func(data, date, 7, 70, 34, Cycle2Str(HOSCYCLE::US_Florida_707), true);
			}

			RecapRowAudit *AuditLogic::USFlorida808(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || stdH::Contains(ruleException,RuleExceptions::USA_Transportation_construction_Materialsandequipment) || stdH::Contains(ruleException,RuleExceptions::USA_Oilfield_waiting_time))
				{
					
					return Cycle_Func(data, date, 8, 80, 24, Cycle2Str(HOSCYCLE::US_Florida_808), false);
				}
				
				return Cycle_Func(data, date, 8, 80, 34, Cycle2Str(HOSCYCLE::US_Florida_808), true);
			}

			RecapRowAudit *AuditLogic::CA707(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				
				return Cycle_Func(data, date, 7, 70, 36, Cycle2Str( HOSCYCLE::Canadian_70hr_7day), false);
			}

			RecapRowAudit *AuditLogic::CA12014(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				
				return Cycle_Func(data, date, 14, 120, 72, Cycle2Str(HOSCYCLE::Canadian_120hr_14day), false);
			}

			RecapRowAudit *AuditLogic::CA707North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				
				return Cycle_Func(data, date, 7, 80, 36, Cycle2Str(HOSCYCLE::Canadian_70hr_7day), false);
			}

			RecapRowAudit *AuditLogic::CA12014North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions> &ruleException)
			{
				
				return Cycle_Func(data, date, 14, 120, 72, Cycle2Str(HOSCYCLE::Canadian_120hr_14day), false);
			}

			RecapRowAudit *AuditLogic::Cycle_Func(std::vector<Timelog*> &data, DateTime date, int days, int limit, int resettime, const std::string &cycle, bool isUSA)
			{
				//Calculate recap by cycle
				DateTime stop = date.Date().AddDays(1);
				DateTime start = stop.Date().AddDays(-1 * days);
				RecapRowAudit *rr = MKR_NEW RecapRowAudit();
				rr->Cycle_Renamed = cycle;
				rr->Day = date.Date();
				Timelog *lasttl = nullptr;
				TimeSpan offtime = TimeSpan(0);
				Timelog *lasttl_1 = nullptr;
				DateTime offDutyStart = DateTime::MinValue;
				std::vector<DateTime> previousLogs;
				for (auto tl : data)
				{
					if (tl->Logtime < start)
					{
						if (isUSA && !Stop168hrsReset) //for MKR_NEW regulation 2013.07.01
						{
							if (lasttl_1 == nullptr)
							{
								lasttl_1 = tl;
								continue;
							}
							CalculateExt(lasttl_1, tl->Logtime, date.Date(), offtime, resettime, offDutyStart, isUSA, data, previousLogs);
							lasttl_1 = tl;
						}
						continue;
					}
					if (tl->Logtime >= stop)
					{
						break;
					}
					if (lasttl == nullptr)
					{
						lasttl = tl;
						if (isUSA && !Stop168hrsReset)
						{
							if (lasttl_1 != nullptr)
							{
								CalculateExt(lasttl_1, tl->Logtime, date.Date(), offtime, resettime, offDutyStart, isUSA, data, previousLogs);
							}
						}
						continue;
					}
					Calculate(lasttl, tl->Logtime, date.Date(), rr, offtime, resettime, offDutyStart, isUSA, data, previousLogs);
					lasttl = tl;
				}
				Calculate(lasttl, TimeUtil::CurrentTime() < stop ? TimeUtil::CurrentTime() : stop, date.Date(), rr, offtime, resettime, offDutyStart, isUSA, data, previousLogs);
				TimeSpan tslimit = TimeSpan(limit, 0, 0);
				rr->CycleAvail = tslimit-rr->CycleTotal;//tslimit.Subtract(rr->CycleTotal);
				return rr;
			}

			void AuditLogic::TestCycle()
			{
				std::vector<Timelog*> data;
				clsRandom *rdn = MKR_NEW clsRandom();
				for (int i = 40; i >= 0; i--) //40 days
				{
					DateTime date = DateTime::Now().Date().AddDays(-1 * i);//DateTime::Today.AddDays(-1 * i);
					int hour = 0;
					int remain = 24;
					for (int j = 0; j < 3; j++) //each day has three data point
					{
						Timelog *tl = MKR_NEW Timelog();
						tl->duty =( DutyStatus )(rdn->Next(4) + 101);
						tl->Location = AuditJurisdiction::Canada;
						tl->Logtime = date.AddHours(hour + rdn->Next(remain));
						data.push_back(tl);
						if (tl->Logtime.Hour() == 24)
						{
							break;
						}
						hour = tl->Logtime.Hour();
						remain -= hour;
						if (remain <= 0)
						{
							break;
						}
					}
				}
				std::sort(data.begin(), data.end());
				DateTime d = DateTime::Now().Date();//DateTime::Today;
				//CAOilfield(data, d);
				//CA707(data, d,);
				//CA707North(data, d);
				//US607(data, d);
				//US607North(data, d);
				//US708(data, d);
				//US708North(data, d);
			}

			RecapRowAudit *AuditLogic::CAOilfield(std::vector<Timelog*> &source, DateTime date)
			{
				std::vector<Timelog*> data;
				data.insert(data.end(), source.begin(), source.end());
				DateTime stop = date.Date().AddDays(1);
				if (date.Date() == TimeUtil::CurrentTime().Date())
				{
					stop = TimeUtil::CurrentTime();
				}
				DateTime start = stop.Date().AddDays(-1 * 24);
				RecapRowAudit *rr = MKR_NEW RecapRowAudit();
				
				rr->Cycle_Renamed = Cycle2Str(HOSCYCLE::Canadian_Oil_Field);//
				rr->Day = date.Date();
				rr->CycleAvail = TimeSpan(21, 0, 0, 0);
				Timelog *lasttl = nullptr;
				TimeSpan offtime = TimeSpan(0);
				int count = 0;
				rr->DayUsed = TimeSpan::MinValue;
				Timelog *tl = nullptr;
				int k = -1;
				for (int i = 0; i < data.size(); i++)
				{
					if (((data[i]->duty == DutyStatus::Driving) || (data[i]->duty == DutyStatus::Onduty)) && (data[i]->Logtime < stop))
					{
						k = i;
						break;
					}
				}
				if (k == -1)
				{
					//No onduty return 0;
					rr->DayUsed = TimeSpan::Zero;
					return rr;
				}
				//Add more data
				while (data[0]->Logtime.Date().AddDays(6) >= stop)
				{
					Timelog *prev = data[0];
					Timelog *tt = prev->Clone();                  // memory leak
					tt->Logtime = tt->Logtime.Date().AddDays(-1);
					stdH::insertItemAtIndex(data, tt, 0);//data.Insert(0, tt);
					delete prev;
				}
				DateTime offstart = stop;
				for (int i = data.size() - 1; i >= 0; i--)
				{
					tl = data[i];
					if (tl->Logtime > stop)
					{
						continue;
					}
					if (lasttl == nullptr)
					{
						lasttl = tl->Clone();    // memory leak
						lasttl->Logtime = stop;
						if (date.Date() == TimeUtil::CurrentTime().Date())
						{
							lasttl->Logtime = TimeUtil::CurrentTime();
						}
						continue;
					}
					if ((tl->duty == DutyStatus::Driving) || (tl->duty == DutyStatus::Onduty))
					{
						offtime = TimeSpan(0);
					}
					if ((tl->duty == DutyStatus::Offduty) || (tl->duty == DutyStatus::Sleeping))
					{
						offstart = tl->Logtime;
						offtime += lasttl->Logtime - tl->Logtime;
						DateTime offend = offstart.AddHours(offtime.TotalHours());
						if ((offtime.TotalHours() >= 24) && (offend <= stop) && (offend > date.Date()))
						{
							if (offend == stop)
							{
								rr->DayUsed = TimeSpan::Zero;
							}
							else
							{
								rr->DayUsed = offend - date.Date();
							}
						}
					}
					while (offtime.TotalHours() >= 24)
					{
						TimeSpan m_ts = TimeSpan(24, 0, 0);
						offtime = offtime- m_ts;
						count++;
					}
					if (lasttl != 0&&lasttl->isClone())
						delete lasttl;//to prevent memory leak
					lasttl = tl;
					if (count >= 3)
					{
						break;
					}
				}
				if (tl == nullptr)
				{
					return rr;
				}
				TimeSpan m_ts = TimeSpan(72, 0, 0);
				rr->CycleTotal = (stop - tl->Logtime) - (offtime - m_ts);
				rr->CycleAvail = TimeSpan(21, 0, 0, 0) - rr->CycleTotal;
				if ((rr->DayUsed != TimeSpan::Zero) && (rr->DayUsed != TimeSpan::MaxValue) && (rr->DayUsed != TimeSpan::MinValue))
				{
					DateTime d = rr->Day.AddHours(rr->DayUsed.TotalHours());
					for (int i = data.size() - 1; i >= 0; i--)
					{
						tl = data[i];
						if (tl->Logtime >= d)
						{
							continue;
						}
						if ((tl->duty == DutyStatus::Driving) || (tl->duty == DutyStatus::Onduty))
						{
							TimeSpan ts = tl->Logtime - tl->Logtime.Date();
							if (ts > rr->DayUsed)
							{
								rr->DayUsed = TimeSpan::MinValue;
							}
							else
							{
								rr->DayUsed = TimeSpan::MaxValue;
							}
							break;
						}
					}
					if ((rr->DayUsed != TimeSpan::Zero) && (rr->DayUsed != TimeSpan::MaxValue) && (rr->DayUsed != TimeSpan::MinValue))
					{
						rr->DayUsed = TimeSpan::MaxValue;
					}
				}
				return rr;
			}


#pragma endregion
}
