#include "HourCalculator.h"



namespace HosEngine
{

	template <typename FType>
	static PRecapRow* vFind(PRecapRowList* src, FType func)
	{
		PRecapRow* v = NULL;
		int size = src->list_size();
		for (int i = 0; i < size;i++)
		{
			PRecapRow* vv = (PRecapRow*)src->mutable_list(i);
			// call the function passed into FirstOrDefault and see if it matches
			if (func(vv))
			{
				v = vv;
				break;
			}
		}

		return v;
	}

	

			HoursDetail::HoursDetail(const std::string rule)
			{
				Name = rule;
			}
			HoursDetail::~HoursDetail()
			{
				Name = "";
			}

//DateTime HourCalculator::CurrentDatetime = TimeUtil::DateTime_Now();

			void HourCalculator::InitializeAlailValues()
			{
				AvaliableOnDutyMinutes = std::numeric_limits<int>::max();
				AvaliableOnDutyRule = "";
				EmergencyStart = false;
				AvaliableDrivingMinutes = std::numeric_limits<int>::max();
				AvaliableDrivingRule = "";

				DateTime ShiftStart ;
			}

			const int &HourCalculator::getMaxDriving() const
			{
				return maxDriving;
			}

			const int &HourCalculator::getMaxOnduty() const
			{
				return maxOnduty;
			}

			const int &HourCalculator::getMaxCycle() const
			{
				return maxCycle;
			}

			const int &HourCalculator::getAvaliableCycle() const
			{
				return avaliableCycle;
			}

			void HourCalculator::SetHourCalculator(PTimeLogRowList *currentAuditDriverTimeLogs, PRecapRowList* currentAuditDriverRecaps, PRuleSelectionHistoryRowList* currentAuditRules, EmployeeRow *currentAuditDriver)
			

			{
				CurrentAuditDriverTimeLogs = currentAuditDriverTimeLogs;// .empty() ? std::vector<TimeLogRow*>() : currentAuditDriverTimeLogs;
				CurrentAuditDriverRecaps = currentAuditDriverRecaps;// .empty() ? std::vector<RecapRow*>() : currentAuditDriverRecaps;
				CurrentAuditRules = currentAuditRules;// .empty() ? std::vector<RuleSelectionHistoryRow*>() : currentAuditRules;
				CurrentAuditDriver = currentAuditDriver == nullptr ? MKR_NEW EmployeeRow() : currentAuditDriver;
				;

				IsUSA = Utils::IsUSA(currentAuditDriver);

				InitializeAlailValues();
			}

			HourCalculator::HourCalculator()
			{

				if (NorthenState.empty())
				{
					NorthenState = std::vector<std::string>();
					NorthenState.push_back("YT");
					NorthenState.push_back("NU");
					NorthenState.push_back("NT");
				}
				if (Canadian.empty())
				{
					Canadian = std::vector<std::string>();
					Canadian.push_back("AB");
					Canadian.push_back("BC");
					Canadian.push_back("SK");
					Canadian.push_back("MB");
					Canadian.push_back("ON");
					Canadian.push_back("QC");
					Canadian.push_back("NB");
					Canadian.push_back("NL");
					Canadian.push_back("NS");
					Canadian.push_back("PE");
					
					Canadian.push_back(strToUpper("Alberta"));
					Canadian.push_back(strToUpper("British Columbia"));
					Canadian.push_back(strToUpper("Saskatchewen"));
					Canadian.push_back(strToUpper("Manitoba"));
					Canadian.push_back(strToUpper("Ontario"));
					Canadian.push_back(strToUpper("Quebec"));
					Canadian.push_back(strToUpper("New Brunswick"));
					Canadian.push_back(strToUpper("Nova Scotia"));
				}
				if (NorthenStateUS.empty())
				{
					NorthenStateUS = std::vector<std::string>();
					NorthenStateUS.push_back("AK");
				}
			}

			HourCalculator::~HourCalculator()
			{
				Canadian.clear();
				defaultSateRuleWeight.clear();
				for (auto mv : HoursBalance)
				{
					HoursDetail *hd = (HoursDetail *)mv.second;
					if (hd != 0)
						delete hd;
				}
				HoursBalance.clear();

				for (auto mv : ForcastBalance)
				{
					HoursDetail *hd = (HoursDetail *)mv.second;
					if (hd != 0)
						delete hd;
				}
				ForcastBalance.clear();
				
				/*if (quickAuditResult != 0)
					delete quickAuditResult;*/

				NorthenState.clear();
				NorthenStateUS.clear();
			}
			int HourCalculator::GetDefaultStateRule(RULEID ruleid)
			{
				if (defaultSateRuleWeight.empty())
				{
					defaultSateRuleWeight = std::unordered_map<int, int>();
					defaultSateRuleWeight.insert(std::make_pair(0, 10001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Alabama_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Alabama_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Alaskan_707), 14000));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Alaskan_808), 14000));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Arizona_607), 18001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Arizona_708), 18001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Connecticut_607), 18000));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Connecticut_708), 18000));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Delaware_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Delaware_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Florida_707), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Florida_808), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Idaho_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Idaho_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Indiana_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Indiana_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Iowa_707), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Iowa_808), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Kansas_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Kansas_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Louisiana_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Louisiana_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Michigan_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Michigan_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Mississippi_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Mississippi_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Missouri_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Missouri_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Montana_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Montana_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Nebraska_707), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Nebraska_808), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Nevada_707), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_NewMexico_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_NewMexico_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_NorthCarolina_707), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_NorthCarolina_808), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_NorthDakota_707), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Ohio_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Ohio_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Oklahoma_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Oklahoma_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Pennsylvania_607), 17001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Pennsylvania_708), 17001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_South_Dakota_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_South_Dakota_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_South_Dakota_707), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_South_Dakota_808), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Tennessee_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Tennessee_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Texas), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Vermont_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Vermont_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Virginia_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Virginia_708), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Wisconsin_707), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Wisconsin_808), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Wyoming_607), 26001));
					defaultSateRuleWeight.insert(std::make_pair(static_cast<int>(RULEID::US_Wyoming_708), 26001));
				}
				int weight = 0;
				if (stdH::TryGetValue(this->defaultSateRuleWeight,(int)ruleid, weight))
				{
					return weight;
				}
				return defaultSateRuleWeight[0];
			}

			AuditJurisdiction HourCalculator::ConvertJurisdiction(std::string &state)
			{
				state = strToUpper(state.c_str());
				if (std::find(NorthenState.begin(), NorthenState.end(), state) != NorthenState.end())
				{
					return AuditJurisdiction::CA_North;
				}
				if (std::find(Canadian.begin(), Canadian.end(), state) != Canadian.end())
				{
					return AuditJurisdiction::Canada;
				}
				if (std::find(NorthenStateUS.begin(), NorthenStateUS.end(), state) != NorthenStateUS.end())
				{
					return AuditJurisdiction::US_North;
				}
				return AuditJurisdiction::US;
			}

			AuditRequest *HourCalculator::PrepareData(AuditJurisdiction region)
			{
				AuditRequest *request = MKR_NEW AuditRequest();   // requires CleanUp
				//request->CycleList= std::vector<Cycle*>();
				int size = CurrentAuditRules->list_size();
				for (int i = 0; i < size;i++)
				{
					PRuleSelectionHistoryRow *r =(PRuleSelectionHistoryRow *) CurrentAuditRules->mutable_list(i);
					Cycle *c = MKR_NEW Cycle();
					c->rule = (RULEID)r->ruleid();
					c->selectTime = r->selecttime();
					c->ruleException = (RuleExceptions)(r->ruleexception());
					request->CycleList.push_back(c);
				}

				AuditJurisdiction defaultAuditJu = AuditJurisdiction::UnKnown;
				if (size >= 0)
				{
					RULEID defaultRuleId = RULEID::NONE;
					PRuleSelectionHistoryRow *rr = (PRuleSelectionHistoryRow *)CurrentAuditRules->mutable_list(size - 1);
					defaultRuleId = (RULEID)rr->ruleid();
					if (IsUSRule(defaultRuleId))
					{
						if (defaultRuleId == RULEID::US_Alaskan_707 || defaultRuleId == RULEID::US_Alaskan_808)
						{
							defaultAuditJu = AuditJurisdiction::US_North;
						}
						else
						{
							defaultAuditJu = AuditJurisdiction::US;
						}
					}
					else
					{
						defaultAuditJu = AuditJurisdiction::Canada;
					}
				}

				//request->timelog = std::vector<Timelog*>();
				bool hasEmergency = false;
				int size1 = CurrentAuditDriverTimeLogs->list_size();
				for (int i = 0; i < size1;i++)
				{
					TimeLogRow *tlr =(TimeLogRow *) CurrentAuditDriverTimeLogs->mutable_list(i);
					if (tlr->logbookstopid() == AuditLogic::EmergencyUseStart)
					{
						hasEmergency = true;
					}
					if (tlr->logbookstopid() == AuditLogic::EmergencyUseEnd)
					{
						hasEmergency = false;
					}
					Timelog *t = MKR_NEW Timelog();
					t->duty = (DutyStatus)(tlr->Event());

					//If there is not state specify, then use the default cycle selection. if there is no default cycle selection, then use home terminal  
					if (tlr->state().empty() && defaultAuditJu != AuditJurisdiction::UnKnown)
					{
						t->Location = defaultAuditJu;
					}
					else
					{
						bool flag = tlr->state().empty();
						string str = flag ? Utils::GetDriverDefaultState(CurrentAuditDriver) : tlr->state();
						t->Location = region == AuditJurisdiction::UnKnown ? ConvertJurisdiction(str) : region;
					}
					t->Longitude = tlr->lon();
					t->Latitude = tlr->lat();
					t->Logtime = tlr->LogTime();
					t->odometer = tlr->odo();
					t->stop = tlr->logbookstopid();
					t->QualifyRadiusRule = tlr->qualifyradiusrule();
					request->timelog.push_back(t);
				}
				//if (GlobalInstance.IsFirstCheckEmer)
				//{
					EmergencyStart = hasEmergency; // !!!!!--------!!!!!!!!!!!
					//GlobalInstance.IsFirstCheckEmer = false;
				//}
				return request;
			}

			TimeSpan HourCalculator::ProcessForcastResult(AuditResult *result)
			{
				ForcastBalance.clear();
				TimeSpan avail = TimeSpan(0);
				RecapRowAudit *recap = result->recap[result->recap.size() - 1];
				if (recap != nullptr)
				{
					HoursDetail *hd = MKR_NEW HoursDetail(recap->Cycle_Renamed);
					//hd.DayUsed = recap.CycleTotal.TotalHours;
					//hd.Available = recap.CycleAvail.TotalHours;
					hd->DayUsed = recap->CycleTotal.TotalMinutes();
					hd->Available = recap->CycleAvail.TotalMinutes();
					ForcastBalance[hd->Name] = hd;
					if (recap->CycleAvail > avail)
					{
						avail = recap->CycleAvail;
					}
				}
				for (AuditTime *at : result->AuditTimes)
				{
					if (avail > at->available)
					{
						avail = at->available;
					}
					HoursDetail *hd = MKR_NEW HoursDetail(at->DispName);
					if (ForcastBalance.find(hd->Name) != ForcastBalance.end())
					{
						hd = ForcastBalance[hd->Name];
					}
					//if (hd.Available > at.available.TotalHours)
					if (hd->Available > at->available.TotalMinutes())
					{
						//hd.Available = at.available.TotalHours;
						hd->Available = at->available.TotalMinutes();
					}
					if (at->ruletype == RuleType::Day)
					{
						//hd.DayUsed = at.used.TotalHours;
						hd->DayUsed = at->used.TotalMinutes();
					}
					else
					{
						//hd.ShiftUsed = at.used.TotalHours;
						hd->ShiftUsed = at->used.TotalMinutes();
					}
					ForcastBalance[hd->Name] = hd;
				}
				return avail;
			}

			TimeSpan HourCalculator::ProcessResult(AuditResult *result)
			{
				RecapRowAudit *todayCap = nullptr;

				for (RecapRowAudit *rr : result->recap)
				{
					if (rr == nullptr)
					{
						continue;
					}
					if (rr->Day == TimeUtil::DateTime_Now().Date())
					{
						todayCap = rr;
					}
					/* adopted it!!!
					RecapRow *r = stdH::vFind(CurrentAuditDriverRecaps,[&] (RecapRow *x)
					{
						return x->date == rr->Day;
					});*/
					PRecapRow *r = vFind(CurrentAuditDriverRecaps, [&](PRecapRow *x)
					{
						return DateTime(x->date()).Date() == rr->Day;    // not sure !!!
					});

					int used = 0;
					TimeSpan ts_min_val;
					if (rr->DayUsed == ts_min_val)
					{
						used = 0;
					}
					else if (rr->DayUsed == ts_min_val)
					{
						used = 0;
					}
					else
					{
						used = static_cast<int>(rr->DayUsed.TotalMinutes());
					}

					//in case of Math.Round 
					double cycleTotal = rr->CycleTotal.TotalMinutes();
					double cycleAvail = rr->CycleAvail.TotalMinutes();
					cycleAvail = int(cycleTotal + cycleAvail - cycleTotal);

					if (rr->Day == TimeUtil::DateTime_Now().Date())
					{
						maxCycle = int(cycleTotal + cycleAvail);
						avaliableCycle = int(cycleAvail);
					}

					if (r == NULL)
					{
						PRecapRow r1;//r = MKR_NEW RecapRow();
						r1.set_date(rr->Day.getMillis());
						r1.set_today(used);
						r1.set_cycletotal(int(cycleTotal));
						r1.set_cycleavailable( int(cycleAvail));

						r1.set_cycle( rr->Cycle_Renamed);
						AddRecap(&r1, CurrentAuditDriver->id());
					}
					else
					{
						if ((r->today() != used && r->cycletotal() != int(cycleTotal) && r->cycleavailable() != int(cycleAvail)) || r->cycle() != rr->Cycle_Renamed)
						{
							r->set_today(used);
						}
						r->set_cycletotal(int(cycleTotal));
						r->set_cycleavailable( int(cycleAvail));
						r->set_cycle(rr->Cycle_Renamed);
						UpdateRecap(r, CurrentAuditDriver->id());
					}
				}

				//AuditCommon.AuditJurisdiction states =
				//	ConvertJurisdiction(ARManager.State.Find(ARManager.TimeLog[ARManager.TimeLog.Count - 1].Location.State).State);
				TimeSpan avail = TimeSpan::MaxValue;
				//HoursBalance = std::unordered_map<std::string, HoursDetail*>();

				{
				//if ((states == AuditCommon.AuditJurisdiction.Canada) || (states == AuditCommon.AuditJurisdiction.CA_North))
					for (AuditTime *at : result->AuditTimes)
					{
						if ((at->rule == RULEID::CA_OnDuty || at->rule == RULEID::US_OnDuty || at->rule == RULEID::CA_OffDuty) && AvaliableOnDutyMinutes > at->ondutyavailable.TotalMinutes())
						{
							AvaliableOnDutyMinutes = (int)at->ondutyavailable.TotalMinutes();
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
							AvaliableOnDutyRule = Int2Str(int(at->rule)) + std::string("|") + Int2Str(int(at->ruletype)) + std::string("|") + RuleT2Str((int)at->ruletype) + std::string(" ") + Rule2Str((int)at->rule);
							if (AvaliableOnDutyMinutes < 0)
							{
							   AvaliableOnDutyRuleDescription = at->ViolationDescription;
							}
							else
							{
								AvaliableOnDutyRuleDescription = at->RuleDescription;
							}
						}
						if (avail > at->available)
						{
							avail = at->available;
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
							AvaliableDrivingRule = Rule2Str(int(at->rule)) + std::string("|") + RuleT2Str(int(at->ruletype)) + std::string("|") + RuleT2Str((int) at->ruletype) + std::string(" ") + Rule2Str((int)at->rule);
							;
							if (avail.TotalSeconds() < 0)
							{
								AvaliableDrivingRuleDescription = at->ViolationDescription;
							}
							else
							{
								AvaliableDrivingRuleDescription = at->RuleDescription;
							}
						}
						string name = at->DispName;
						HoursDetail *hd = MKR_NEW HoursDetail(name);
						if (HoursBalance.find(hd->Name) != HoursBalance.end())
						{
							name = hd->Name;
							delete hd;                     // do not leak the memory
							hd = HoursBalance[name];
						}
						//if (hd.Available > at.available.TotalHours)
						//    hd.Available = at.available.TotalHours;
						if (hd->Available > at->available.TotalMinutes())
						{
							hd->Available = at->available.TotalMinutes();
						}

						if (at->ruletype == RuleType::Day)
						{
							//hd.DayUsed = at.used.TotalHours;
							hd->DayUsed = at->used.TotalMinutes();
						}
						else
						{
							//hd.ShiftUsed = at.used.TotalHours;
							hd->ShiftUsed = at->used.TotalMinutes();
						}
						HoursBalance[hd->Name] = hd;
					}
				}
				if (EmergencyStart)
				{
					avail = TimeSpan(maxDriving,0,0);
					//if (IsUSA) avail = MKR_NEW TimeSpan(11, 0, 0);
					//else avail = MKR_NEW TimeSpan(13, 0, 0);
				}

				AvaliableDrivingMinutes = (int)(avail.TotalMinutes());
				

				//if (quickAuditResult == nullptr)
				//{
				//	quickAuditResult = MKR_NEW AuditResultQuick();
				//}

				//quickAuditResult->AuditRecap = todayCap;
				//quickAuditResult->AuditTime_Renamed = TimeUtil::DateTime_Now();
				//			
				//quickAuditResult->AuditTimes = result->AuditTimes;//result->AuditTimes;                   //???

				return avail;
			}

			TimeSpan HourCalculator::CalculateHours()
			{

				IsDoingAudit = true;
				if (CurrentAuditDriverTimeLogs->list_size()==0)
				{
					return TimeSpan(13, 0, 0);
				}
				AuditRequest *request = PrepareData(AuditJurisdiction::UnKnown);
				AuditLogic *auditLogic = MKR_NEW AuditLogic(this);
				AuditResult *result = auditLogic->Audit(request, CurrentAuditDriver, IsUSA, CurrentAuditDriverTimeLogs, CurrentLatitude, CurrentLogitude, ReachAirMilesThresholdTime, QualifyRadiusRule);
				ShiftStart = auditLogic->ShiftStart;
				ShiftStartOdometer = auditLogic->ShiftStartOdometer;
				maxDriving = auditLogic->getMaxDriving();
				maxOnduty = auditLogic->getMaxOnduty();
				IsDoingAudit = false;
				TimeSpan ts= ProcessResult(result);
				//clean up resources
				if (result != 0)
					delete result;
				delete auditLogic;
				delete request;
				return ts;
			}

			void HourCalculator::AddDeferHour(DeferHourRow *dhr, const std::string &driverId, std::unordered_map<DateTime*, DeferHourRow*> &defers)
			{
				defers.clear();
				if (this->add_dhr_cb != nullptr)
				{
					this->add_dhr_cb(dhr, driverId);
				  
				}
			}

			void HourCalculator::DeleteDeferHour(DeferHourRow *dhr, const std::string &driverId, std::unordered_map<DateTime*, DeferHourRow*> &defers)
			{
				defers.clear();
				if (this->del_dhr_cb != nullptr)
				{
					this->del_dhr_cb(dhr, driverId);

				}
			}

			DeferHourRow *HourCalculator::FindDeferHourByDate(DateTime date, const std::string &driverId, std::unordered_map<DateTime*, DeferHourRow*> &defers)
			{
				if (defers.size() > 0 && defers.find(&date) != defers.end())
				{
					return defers[&date];
				}
				if (FindDeferHourByDateEvent != nullptr)
				{
					
					DeferHourRow *dhr=FindDeferHourByDateEvent(date, driverId);
					if(dhr!=0)
						defers.insert(std::make_pair(&date, dhr));
					return dhr;
				}
				else
				{
					return nullptr;
				}
			}

			void HourCalculator::UpdateRecap(PRecapRow *recapRow, const std::string &driverId)
			{
				if (UpdateRecapEvent != nullptr)
				{
					UpdateRecapEvent(recapRow, driverId);
				}
			}

			void HourCalculator::AddRecap(PRecapRow *recapRow, const std::string &driverId)
			{
				if (AddRecapEvent != nullptr)
				{
					AddRecapEvent(recapRow, driverId);
				}
			}

}
