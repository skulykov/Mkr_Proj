

			std::vector<AuditTime*> AuditLogic::AuditUS(std::vector<Timelog*> &data, int drivinglimit, int ondutylimit, RULEID *rule)
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

				DateTime dt1 = FindShiftStart(data, resetHours, nullptr);
				TimeSpan lastslpduration = TimeSpan(0);
				std::vector<OffdutySession*> sleeps = MergeOffduty(data, TimeUtil::CurrentTime, false, nullptr);
				DateTime dt2 = USFindShiftStartBySleeper(data, 8, lastslpduration,sleeps);
				if (dt1 < dt2)
				{
					dt1 = dt2;
				}

				//if it is coworker hours, don't need to set
				//if (!AuditLogic.IsCoworker)
				ShiftStart = dt1;

				result.push_back(Audit_US_Shift_Driving(data, TimeUtil::CurrentTime, dt1, drivinglimit, sleeps));
				result.push_back(Audit_US_Shift_OnDuty(data, TimeUtil::CurrentTime, dt1, ondutylimit, sleeps));
				return result;
			}

			int AuditLogic::CheckOnDutyHoursLimitCount(std::vector<Timelog*> &data, int ondutylimit, RULEID *rule, DateTime startTime, int days)
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
				Timelog *lastOnDutyTlr = (from o in data where ((o->duty == DutyStatus::Driving || o->duty == DutyStatus::Onduty) && o::Logtime < startTime) orderby o::Logtime descending select o)->FirstOrDefault<Timelog*>();
				if (lastOnDutyTlr != nullptr) // find first off duty or sleep
				{
					lastOnDutyTlr = (from o in data where ((o->duty == DutyStatus::Offduty || o->duty == DutyStatus::Sleeping) && o::Logtime < startTime && o::Logtime > lastOnDutyTlr.Logtime) orderby o::Logtime select o)->FirstOrDefault<Timelog*>();
				}
				int index = 0;
				while (lastOnDutyTlr != nullptr && lastOnDutyTlr->Logtime.Date >= TimeUtil::DateTime_Now().Date::AddDays(-1 * days) && index <= 30) // need to change now;
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
					if (auditTime != nullptr && auditTime->used.TotalHours >= ondutylimit)
					{
						count = count + 1;
					}
					startTime = dt1;
					lastOnDutyTlr = (from o in data where ((o->duty == DutyStatus::Driving || o->duty == DutyStatus::Onduty) && o::Logtime < startTime) orderby o::Logtime descending select o)->FirstOrDefault<Timelog*>();
					if (lastOnDutyTlr != nullptr) // find first off duty or sleep
					{
						lastOnDutyTlr = (from o in data where ((o->duty == DutyStatus::Offduty || o->duty == DutyStatus::Sleeping) && o::Logtime < startTime && o::Logtime > lastOnDutyTlr.Logtime) orderby o::Logtime select o)->FirstOrDefault<Timelog*>();
					}
					index = index + 1;
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
					if ((sleeps[i]->current == DutyStatus::Offduty) && (sleeps[i]->ts.TotalHours >= 2))
					{
						if ((sleeps[i - 1]->current == DutyStatus::Sleeping) && (sleeps[i - 1]->ts.TotalHours >= 8))
						{
							lastslpduration = sleeps[i]->ts;
							return sleeps[i - 1]->stop;
						}
					}

					if ((sleeps[i]->current == DutyStatus::Sleeping) && (sleeps[i]->ts.TotalHours >= 2))
					{
						if ((sleeps[i - 1]->current == DutyStatus::Offduty) && (sleeps[i - 1]->ts.TotalHours >= 2) && (sleeps[i]->ts.TotalHours >= 8))
						{
							lastslpduration = sleeps[i]->ts;
							return sleeps[i - 1]->stop;
						}
						if ((sleeps[i - 1]->current == DutyStatus::Sleeping) && sleeps[i - 1]->ts.TotalHours >= 2) //new 2014.09.16 must >= 2 hours
						{
							if (((sleeps[i]->ts.TotalHours + sleeps[i - 1]->ts.TotalHours) >= 10) && (sleeps[i]->ts.TotalHours >= 8 || sleeps[i - 1]->ts.TotalHours >= 8))
							{
								lastslpduration = sleeps[i]->ts;
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
				AuditTime *at = new AuditTime();
				at->rule = RULEID::US_Driving;
				at->DispName = "Driving";
				at->ViolationDescription = "Driving time limit for work shift exceeded";
				at->ruletype = RuleType::Shift;
				at->available = TimeSpan(limit, 0, 0);
				std::vector<DutyStatus*> required;
				required.push_back(DutyStatus::Driving);
				TimeSpan driving = CalculateHours(data, required, shiftstart, end);
				at->available = TimeSpan(limit, 0, 0) - driving;
				at->RuleDescription = std::string::Format("{0}{1}{2}", "You are about to exceed driving time limit for work shift in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
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
						if (sleeps[index]->ts.TotalMinutes >= 30)
						{
							lastThirtyMinutes = sleeps[index]->stop;
							break;
						}
					}
					//if (TimeUtil.CurrentTime.Subtract(sleeps[sleeps.Count - 1].stop).TotalHours >= 8)
					//Fixed on Nov 4, 2013 for misunderstanding
					//30 minutes is not for 8 consecutive hours on duty, is for 8 hours on duty 
					//which not has 30 minutes break inside 8 hours,
					AuditGlobalInstance::LastThirtyMinuteOffDuty[CurrentAuditDriver::ID] = lastThirtyMinutes;
					if (TimeUtil::CurrentTime::Subtract(lastThirtyMinutes)->TotalHours >= 8)
					{
						if ((lastThirtyMinutes.AddHours(8) - TimeUtil::CurrentTime) < at->available)
						{
							at->rule = RULEID::Thirty_Minutes_Break;


							at->available = lastThirtyMinutes.AddHours(8) - TimeUtil::CurrentTime;
							at->ViolationDescription = "Minimum 30-minute off-duty break required.";
							at->RuleDescription = std::string::Format("{0}{1}{2}", "You are about to violate minimum 30-minute off-duty break requirement in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
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
						if (sleeps[index]->start >= shiftstart && sleeps[index]->stop <= end && sleeps[index]->current == DutyStatus::Sleeping && (sleeps[index]->ts.TotalHours >= 8 || sleeps[index]->StopAction == AuditLogic::OILFIELDWAIT))
						{
							sleeptotal = sleeptotal + sleeps[index]->ts;
						}
					}
				}

				AuditTime *at = new AuditTime();
				at->DispName = "On Duty";
				at->ViolationDescription = "On duty time limit for work shift exceeded";
				at->rule = RULEID::US_OnDuty;
				at->ruletype = RuleType::Shift;
				at->available = TimeSpan(limit, 0, 0);
				//List<DutyStatus> required = new List<DutyStatus>();
				//required.Add(DutyStatus.Driving);
				//required.Add(DutyStatus.Onduty);
				TimeSpan onduty = end - shiftstart; // CalculateHours(data, required, shiftstart, end);// new TimeSpan(0);
				at->available = TimeSpan(limit, 0, 0) + sleeptotal - onduty;
				at->ondutyavailable = at->available;
				at->used = onduty;
				at->RuleDescription = std::string::Format("{0}{1}{2}", "You are about to exceed on duty time limit for work shift in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				//at.ondutyavailable = at.available;
				return at;

			}

			AuditTime *AuditLogic::Audit_Recap(std::vector<RecapRowAudit*> &recaps, std::vector<OffdutySession*> &offduties, RULEID *rule, DateTime date, std::vector<Timelog*> &data)
			{
				AuditTime *at = new AuditTime();
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
					at->RuleDescription = std::string::Format("{0}{1}{2}", "Your are about to exceed driving time limit for cycle in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
					//at.DispName = rr.Cycle;
					return at;
				}
				return nullptr;
			}

			AuditTime *AuditLogic::Audit_CA_70Hour_1(std::vector<Timelog*> &data, DateTime date, int limit)
			{
				//all driver must take 24 hour off within the last 14 days
				DateTime start = data[0]->Logtime;
				DateTime end = TimeUtil::CurrentTime;
				DateTime lasttl = end;

				TimeSpan offduty = TimeSpan(0);
				for (int i = data.size() - 1;i >= 0;i--)
				{
					Timelog *tl = data[i];
					if ((tl->duty == DutyStatus::Offduty) || (tl->duty == DutyStatus::Sleeping))
					{
						offduty += lasttl - tl->Logtime;
						if (offduty.TotalHours >= 24)
						{
							start = tl->Logtime.AddHours(offduty.TotalHours);
							break;
						}
					}
					else
					{
						offduty = TimeSpan(0);
					}
					lasttl = tl->Logtime;
				}
				AuditTime *at = new AuditTime();
				at->available = TimeSpan(limit, 0, 0);

				at->ruletype = RuleType::Day;
				at->rule = RULEID::CA_70Hour;
				at->DispName = "24 Hour off";
				at->ViolationDescription = "24 Hour off duty was not taken within the last 14 days";
				at->used = end - start;
				at->available = TimeSpan(limit, 0, 0) - (end - start);
				at->RuleDescription = std::string::Format("{0}{1}{2}", "You are about to violate 24 hour off-duty requirement within the last 14 days in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				return at;

			}

			AuditTime *AuditLogic::Audit_CA_70Hour_2(std::vector<Timelog*> &data, DateTime date, int limit)
			{
				//in cycle 2, driver must take 24 hour off before reach 70 hour of driving time
				AuditTime *at = new AuditTime();
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

				DateTime lasttl = TimeUtil::CurrentTime;
				TimeSpan offduty = TimeSpan(0);
				TimeSpan onduty = TimeSpan(0);
				for (int i = data.size() - 1; i >= 0; i--)
				{
					Timelog *tl = data[i];
					TimeSpan ts = lasttl - tl->Logtime;
					if ((tl->duty == DutyStatus::Offduty) || (tl->duty == DutyStatus::Sleeping))
					{
						offduty += ts;
						if (offduty.TotalHours >= 24)
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
				at->RuleDescription = std::string::Format("{0}{1}{2}", "You are about to violate 24 hours off-duty requirement before 80 hours for CA Cycle 2 in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
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
				if (dhr->day2->Date == dayStart.Date) //now checking if existing dhr cause any problem or not
				{
					TimeSpan ts = GetTotalDrivingTimeInDayRange(data, dayStart.AddHours(-24), dayStart.AddHours(24));
					if (ts.TotalHours > 26)
					{
						avail = TimeSpan::Zero;
						return;
					}
					DateTime remark = TimeUtil::CurrentTime;
					double corehours = 0;
					DateTime corehoursStop = DateTime::MinValue;
					ts = GetTotalOffdutyInDate(offduties, dayStart.AddHours(-24), dayStart.AddHours(24), remark, corehours, corehoursStop);
					//ts = GetTotalOffdutyInDate(offduties, dhr.day1.Date, dhr.day2.Date.AddDays(1), ref remark);
					if (ts.TotalHours < 20)
					{
						TimeSpan required = TimeSpan(20, 0, 0) - ts;
						//TimeSpan left = TimeUtil.CurrentTime.Date.AddDays(1) - TimeUtil.CurrentTime;
						TimeSpan left = dayStart.AddHours(24) - TimeUtil::CurrentTime;

						if (left <= required)
						{
							avail = TimeSpan::Zero;
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
					if (dhr->day1->Date == dayStart.Date) //by devin
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
				if (CurrentAuditDriverTimeLogs->Count == 0)
				{
					return nullptr;
				}
				return HourCalculator::FindDeferHourByDate(date, CurrentAuditDriver::ID, CurrentDefers);
			}
