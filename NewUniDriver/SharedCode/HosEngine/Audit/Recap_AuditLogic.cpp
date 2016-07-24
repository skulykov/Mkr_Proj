
			std::vector<RecapRowAudit*> AuditLogic::CalculateRecap(AuditRequest *ar, RULEID *&rule, std::vector<RuleExceptions*> &ruleException)
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

			RecapRowAudit *AuditLogic::ApplyRule(AuditRequest *ar, Timelog *lasttl, RULEID *&rid, std::vector<RuleExceptions*> &ruleException)
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
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_NewJersey_707.ToString(), true);
					}
					if (rid == RULEID::US_Georgia_707)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_Georgia_707.ToString(), true);
					}
					if (rid == RULEID::US_NewJersey_808)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 80, 34, HOSCYCLE::US_NewJersey_808.ToString(), true);
					}
					if (rid == RULEID::US_Georgia_808)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 80, 34, HOSCYCLE::US_Georgia_808.ToString(), true);
					}
					if (rid == RULEID::US_Alabama_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Alabama_607.ToString(), true);
					}
					if (rid == RULEID::US_Alabama_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Alabama_708.ToString(), true);
					}
					if (rid == RULEID::US_Arizona_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Arizona_607.ToString(), true);
					}
					if (rid == RULEID::US_Arizona_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Arizona_708.ToString(), true);
					}
					if (rid == RULEID::US_Connecticut_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Connecticut_607.ToString(), true);
					}
					if (rid == RULEID::US_Connecticut_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Connecticut_708.ToString(), true);
					}
					if (rid == RULEID::US_Delaware_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Delaware_607.ToString(), true);
					}
					if (rid == RULEID::US_Delaware_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Delaware_708.ToString(), true);
					}
					if (rid == RULEID::US_Idaho_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Idaho_607.ToString(), true);
					}
					if (rid == RULEID::US_Idaho_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Idaho_708.ToString(), true);
					}
					if (rid == RULEID::US_Indiana_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Indiana_607.ToString(), true);
					}
					if (rid == RULEID::US_Indiana_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Indiana_708.ToString(), true);
					}
					if (rid == RULEID::US_Iowa_707)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_Iowa_707.ToString(), true);
					}
					if (rid == RULEID::US_Iowa_808)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 80, 34, HOSCYCLE::US_Iowa_808.ToString(), true);
					}
					if (rid == RULEID::US_Kansas_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Kansas_607.ToString(), true);
					}
					if (rid == RULEID::US_Kansas_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Kansas_708.ToString(), true);
					}
					if (rid == RULEID::US_Louisiana_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Louisiana_607.ToString(), true);
					}
					if (rid == RULEID::US_Louisiana_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Louisiana_708.ToString(), true);
					}
					if (rid == RULEID::US_Maryland_707)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_Maryland_707.ToString(), true);
					}
					if (rid == RULEID::US_Maryland_808)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 80, 34, HOSCYCLE::US_Maryland_808.ToString(), true);
					}
					if (rid == RULEID::US_Michigan_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Michigan_607.ToString(), true);
					}
					if (rid == RULEID::US_Michigan_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Michigan_708.ToString(), true);
					}
					if (rid == RULEID::US_Mississippi_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Mississippi_607.ToString(), true);
					}
					if (rid == RULEID::US_Michigan_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Mississippi_708.ToString(), true);
					}
					if (rid == RULEID::US_Missouri_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Missouri_607.ToString(), true);
					}
					if (rid == RULEID::US_Missouri_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Missouri_708.ToString(), true);
					}
					if (rid == RULEID::US_Montana_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Montana_607.ToString(), true);
					}
					if (rid == RULEID::US_Montana_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Montana_708.ToString(), true);
					}
					if (rid == RULEID::US_Nebraska_707)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_Nebraska_707.ToString(), true);
					}
					if (rid == RULEID::US_Nebraska_808)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 80, 34, HOSCYCLE::US_Nebraska_808.ToString(), true);
					}
					if (rid == RULEID::US_Nevada_707)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_Nevada_707.ToString(), true);
					}
					if (rid == RULEID::US_NewMexico_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_NewMexico_607.ToString(), true);
					}
					if (rid == RULEID::US_NewMexico_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_NewMexico_708.ToString(), true);
					}
					if (rid == RULEID::US_NorthCarolina_707)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_NorthCarolina_707.ToString(), true);
					}
					if (rid == RULEID::US_NorthCarolina_808)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 80, 34, HOSCYCLE::US_NorthCarolina_808.ToString(), true);
					}
					if (rid == RULEID::US_NorthDakota_707)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_NorthDakota_707.ToString(), true);
					}
					if (rid == RULEID::US_Ohio_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Ohio_607.ToString(), true);
					}
					if (rid == RULEID::US_Ohio_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Ohio_708.ToString(), true);
					}
					if (rid == RULEID::US_Oklahoma_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Oklahoma_607.ToString(), true);
					}
					if (rid == RULEID::US_Oklahoma_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Oklahoma_708.ToString(), true);
					}
					if (rid == RULEID::US_Oregon_707)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_Oregon_707.ToString(), true);
					}
					if (rid == RULEID::US_Oregon_808)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 80, 34, HOSCYCLE::US_Oregon_808.ToString(), true);
					}
					if (rid == RULEID::US_Pennsylvania_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Pennsylvania_607.ToString(), true);
					}
					if (rid == RULEID::US_Pennsylvania_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Pennsylvania_708.ToString(), true);
					}
					if (rid == RULEID::US_South_Dakota_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_South_Dakota_607.ToString(), true);
					}
					if (rid == RULEID::US_South_Dakota_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_South_Dakota_708.ToString(), true);
					}
					if (rid == RULEID::US_South_Dakota_707)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_South_Dakota_707.ToString(), true);
					}
					if (rid == RULEID::US_South_Dakota_808)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 80, 34, HOSCYCLE::US_South_Dakota_808.ToString(), true);
					}
					if (rid == RULEID::US_Tennessee_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Tennessee_607.ToString(), true);
					}
					if (rid == RULEID::US_Tennessee_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Tennessee_708.ToString(), true);
					}
					if (rid == RULEID::US_Vermont_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Vermont_607.ToString(), true);
					}
					if (rid == RULEID::US_Vermont_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Vermont_708.ToString(), true);
					}
					if (rid == RULEID::US_Virginia_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Virginia_607.ToString(), true);
					}
					if (rid == RULEID::US_Virginia_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Virginia_708.ToString(), true);
					}
					if (rid == RULEID::US_Wisconsin_707)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 70, 34, HOSCYCLE::US_Wisconsin_707.ToString(), true);
					}
					if (rid == RULEID::US_Wisconsin_808)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 80, 34, HOSCYCLE::US_Wisconsin_808.ToString(), true);
					}
					if (rid == RULEID::US_Wyoming_607)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 7, 60, 34, HOSCYCLE::US_Wyoming_607.ToString(), true);
					}
					if (rid == RULEID::US_Wyoming_708)
					{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
						return Cycle(ar->timelog, lasttl->Logtime, 8, 70, 34, HOSCYCLE::US_Wyoming_708.ToString(), true);
					}
				}
				return nullptr;
			}

			std::vector<RuleExceptions*> AuditLogic::GetRuleExceptions(int rule)
			{
				std::vector<RuleExceptions*> ruleExceptions;
				for (int bit = 0; bit < 32; ++bit)
				{
					if (((rule >> bit) & 1) == 1)
					{
						if (bit == 0)
						{
							ruleExceptions.push_back(static_cast<RuleExceptions*>(bit + 1));
						}
						else
						{
							ruleExceptions.push_back(static_cast<RuleExceptions*>(2 << (bit - 1)));
						}
					}
				}

				return ruleExceptions;
			}

			RULEID *AuditLogic::FindApplyingRule(Timelog *tl, std::vector<Cycle*> &cyclelist, std::vector<RuleExceptions*> &ruleException)
			{
				for (int i = cyclelist.size() - 1;i >= 0;i--)
				{
					if (((tl->Logtime >= cyclelist[i]->selectTime.Date) && (i > 0)) || (i == 0))
					{
						ruleException = GetRuleExceptions(static_cast<int>(cyclelist[i]->ruleException));
						return cyclelist[i]->rule;
						/* comment out because always use current cycle selection on 2014-12-10
						if (((tl.Location == AuditJurisdiction.US)||(tl.Location==AuditJurisdiction.US_North)) && (USRuleset.Contains(cyclelist[i].rule)))
						{
						    ruleException = cyclelist[i].ruleException;
							return cyclelist[i].rule;
						}
						if (((tl.Location == AuditJurisdiction.Canada)||(tl.Location==AuditJurisdiction.CA_North)) && (CanadianRuleset.Contains(cyclelist[i].rule)))
						{
						    ruleException = cyclelist[i].ruleException;
							return cyclelist[i].rule;
						}
						 */
					}
				}
				return RULEID::CA_Service_707;
			}

			void AuditLogic::Calculate(Timelog *lasttl, DateTime tl, DateTime today, RecapRowAudit *&rr, TimeSpan &offtime, int resettime, DateTime &offDutyStart, bool isUSA, std::vector<Timelog*> &data, std::vector<DateTime> &previousLogs)
			{

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
							if ((offDutyStart.Date.AddHours(1) >= offDutyStart && tl >= offDutyStart.Date.AddDays(1).AddHours(5)) || (offDutyStart.Date.AddDays(1).AddHours(1) >= offDutyStart && tl >= offDutyStart.Date.AddDays(2).AddHours(5)))
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
						if ((offDutyStart.Date.AddHours(1) >= offDutyStart && tl >= offDutyStart.Date.AddDays(1).AddHours(5)) || (offDutyStart.Date.AddDays(1).AddHours(1) >= offDutyStart && tl >= offDutyStart.Date.AddDays(2).AddHours(5)))
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
						if (!((lastCycleTime.Date.AddHours(1) >= lastCycleTime && offDutyEnd >= lastCycleTime.Date.AddDays(1).AddHours(5) && offDutyEnd.Subtract(lastCycleTime).TotalHours() >= resettime) || (offDutyEnd >= lastCycleTime.Date.AddDays(2).AddHours(5) && offDutyEnd.Subtract(lastCycleTime).TotalHours() >= resettime)))
						{
							return false;
						}
						else
						{
							offDutyStart = lastCycleTime; // new change 2014-09-16.  Because offduty start is after 168 hours from last reset start
						}
					}
				}
				//TimeSpan offtimeCycle = new TimeSpan(0);
				//DateTime stop = DateTime.MinValue;
				//DateTime lastCycleTime = DateTime.MinValue;
				//for (int index = data.Count - 1; index >= 1; index--)
				//{
				//    if (data[index].Logtime >= offDutyStart) continue;
				//    Timelog tr = data[index];
				//    Timelog trPre = data[index - 1];
				//    if ((trPre.duty == DutyStatus.Offduty) || (trPre.duty == DutyStatus.Sleeping))
				//    {
				//        if (stop == DateTime.MinValue) stop = tr.Logtime;
				//        TimeSpan tsCycle = tr.Logtime - trPre.Logtime;
				//        offtimeCycle += tsCycle;
				//        if (offtimeCycle.TotalHours() >= resettime)
				//        {
				//            if (
				//                  (trPre.Logtime.Date().AddHours(1) >= trPre.Logtime && stop >= trPre.Logtime.Date().AddDays(1).AddHours(5)) ||
				//                   stop >= trPre.Logtime.Date().AddDays(2).AddHours(5)
				//               )
				//                lastCycleTime = trPre.Logtime;
				//        }
				//    }
				//    else
				//    {
				//        if (lastCycleTime != DateTime.MinValue) break;
				//        offtimeCycle = new TimeSpan(0);
				//        stop = DateTime.MinValue;
				//    }
				//}
				//if (lastCycleTime != DateTime.MinValue)
				//{
				//    lastCycleTime = lastCycleTime.AddHours(168);
				//    if (lastCycleTime > offDutyStart)
				//    {
				//        if (!(
				//                 (lastCycleTime.Date.AddHours(1) >= lastCycleTime &&
				//                  offDutyEnd >= lastCycleTime.Date.AddDays(1).AddHours(5) && 
				//                  offDutyEnd.Subtract(lastCycleTime).TotalHours() >= resettime) ||
				//                 (offDutyEnd >= lastCycleTime.Date.AddDays(2).AddHours(5))
				//             ))
				//           return false;
				//    }
				//}
				return true;
			}

			RecapRowAudit *AuditLogic::US607(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_Oilfield_waiting_time) != ruleException.end())
				{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					return Cycle(data, date, 8, 70, 24, HOSCYCLE::US_70hr_8day.ToString(), false);
				}
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || ruleException.Contains(RuleExceptions::USA_Transportation_construction_Materialsandequipment))
				{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					return Cycle(data, date, 7, 60, 24, HOSCYCLE::US_60hr_7day.ToString(), false);
				}
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 7, 60, 34, HOSCYCLE::US_60hr_7day.ToString(), true);

			}

			RecapRowAudit *AuditLogic::US708(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || ruleException.Contains(RuleExceptions::USA_Transportation_construction_Materialsandequipment) || ruleException.Contains(RuleExceptions::USA_Oilfield_waiting_time))
				{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					return Cycle(data, date, 8, 70, 24, HOSCYCLE::US_70hr_8day.ToString(), false);
				}
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 8, 70, 34, HOSCYCLE::US_70hr_8day.ToString(), true);
			}

			RecapRowAudit *AuditLogic::US607North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_Oilfield_waiting_time) != ruleException.end())
				{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					return Cycle(data, date, 8, 80, 24, HOSCYCLE::US_Alaskan_808.ToString(), false);
				}
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || ruleException.Contains(RuleExceptions::USA_Transportation_construction_Materialsandequipment))
				{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					return Cycle(data, date, 7, 70, 24, HOSCYCLE::US_Alaskan_707.ToString(), false);
				}
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 7, 70, 34, HOSCYCLE::US_Alaskan_707.ToString(), true);
			}

			RecapRowAudit *AuditLogic::US708North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || ruleException.Contains(RuleExceptions::USA_Transportation_construction_Materialsandequipment) || ruleException.Contains(RuleExceptions::USA_Oilfield_waiting_time))
				{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					return Cycle(data, date, 8, 80, 24, HOSCYCLE::US_Alaskan_808.ToString(), false);
				}
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 8, 80, 34, HOSCYCLE::US_Alaskan_808.ToString(), true);
			}

			RecapRowAudit *AuditLogic::USOilfield(std::vector<Timelog*> &data, DateTime date)
			{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 8, 70, 24, HOSCYCLE::US_Oil_Field.ToString(), false);
			}

			RecapRowAudit *AuditLogic::USTexas(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || ruleException.Contains(RuleExceptions::USA_Transportation_construction_Materialsandequipment) || ruleException.Contains(RuleExceptions::USA_Oilfield_waiting_time))
				{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					return Cycle(data, date, 7, 70, 24, HOSCYCLE::US_Texas.ToString(), false);
				}
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 7, 70, 34, HOSCYCLE::US_Texas.ToString(), true);
			}

			RecapRowAudit *AuditLogic::USCalifornia(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || ruleException.Contains(RuleExceptions::USA_Transportation_construction_Materialsandequipment) || ruleException.Contains(RuleExceptions::USA_Oilfield_waiting_time))
				{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					return Cycle(data, date, 8, 80, 24, HOSCYCLE::US_California.ToString(), false);
				}
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 8, 80, 34, HOSCYCLE::US_California.ToString(), true);
			}

			RecapRowAudit *AuditLogic::USFlorida707(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || ruleException.Contains(RuleExceptions::USA_Transportation_construction_Materialsandequipment) || ruleException.Contains(RuleExceptions::USA_Oilfield_waiting_time))
				{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					return Cycle(data, date, 7, 70, 24, HOSCYCLE::US_Florida_707.ToString(), false);
				}
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 7, 70, 34, HOSCYCLE::US_Florida_707.ToString(), true);
			}

			RecapRowAudit *AuditLogic::USFlorida808(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
				if (std::find(ruleException.begin(), ruleException.end(), RuleExceptions::USA_24_hour_cycle_reset) != ruleException.end() || ruleException.Contains(RuleExceptions::USA_Transportation_construction_Materialsandequipment) || ruleException.Contains(RuleExceptions::USA_Oilfield_waiting_time))
				{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
					return Cycle(data, date, 8, 80, 24, HOSCYCLE::US_Florida_808.ToString(), false);
				}
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 8, 80, 34, HOSCYCLE::US_Florida_808.ToString(), true);
			}

			RecapRowAudit *AuditLogic::CA707(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 7, 70, 36, HOSCYCLE::Canadian_70hr_7day.ToString(), false);
			}

			RecapRowAudit *AuditLogic::CA12014(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 14, 120, 72, HOSCYCLE::Canadian_120hr_14day.ToString(), false);
			}

			RecapRowAudit *AuditLogic::CA707North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 7, 80, 36, HOSCYCLE::Canadian_70hr_7day.ToString(), false);
			}

			RecapRowAudit *AuditLogic::CA12014North(std::vector<Timelog*> &data, DateTime date, std::vector<RuleExceptions*> &ruleException)
			{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				return Cycle(data, date, 14, 120, 72, HOSCYCLE::Canadian_120hr_14day.ToString(), false);
			}

			RecapRowAudit *AuditLogic::Cycle(std::vector<Timelog*> &data, DateTime date, int days, int limit, int resettime, const std::string &cycle, bool isUSA)
			{
				//Calculate recap by cycle
				DateTime stop = date.Date.AddDays(1);
				DateTime start = stop.Date.AddDays(-1*days);
				RecapRowAudit *rr = new RecapRowAudit();
				rr->Cycle = cycle;
				rr->Day = date.Date;
				Timelog *lasttl = nullptr;
				TimeSpan offtime = TimeSpan(0);
				Timelog *lasttl_1 = nullptr;
				DateTime offDutyStart = DateTime::MinValue;
				std::vector<DateTime> previousLogs;
				for (auto tl : data)
				{
					if (tl->Logtime < start)
					{
						if (isUSA && !Stop168hrsReset) //for new regulation 2013.07.01
						{
							if (lasttl_1 == nullptr)
							{
								lasttl_1 = tl;
								continue;
							}
							CalculateExt(lasttl_1, tl->Logtime, date.Date, offtime, resettime, offDutyStart, isUSA, data, previousLogs);
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
								CalculateExt(lasttl_1, tl->Logtime, date.Date, offtime, resettime, offDutyStart, isUSA, data, previousLogs);
							}
						}
						continue;
					}
					Calculate(lasttl, tl->Logtime, date.Date, rr, offtime, resettime, offDutyStart, isUSA, data, previousLogs);
					lasttl = tl;
				}
				Calculate(lasttl, TimeUtil::CurrentTime() < stop ? TimeUtil::CurrentTime() : stop, date.Date, rr, offtime, resettime, offDutyStart, isUSA, data, previousLogs);
				TimeSpan tslimit = TimeSpan(limit, 0, 0);
				rr->CycleAvail = tslimit.Subtract(rr->CycleTotal);
				return rr;
			}

			void AuditLogic::TestCycle()
			{
				std::vector<Timelog*> data;
				Random *rdn = new Random();
				for (int i = 40; i >= 0; i--) //40 days
				{
					DateTime date = DateTime::Today.AddDays(-1*i);
					int hour = 0;
					int remain = 24;
					for (int j = 0; j < 3; j++) //each day has three data point
					{
						Timelog *tl = new Timelog();
						tl->duty = static_cast<DutyStatus*>(rdn->Next(4) + 101);
						tl->Location = AuditJurisdiction::Canada;
						tl->Logtime = date.AddHours(hour + rdn->Next(remain));
						data.push_back(tl);
						if (tl->Logtime->Hour == 24)
						{
							break;
						}
						hour = tl->Logtime.Hour;
						remain -= hour;
						if (remain <= 0)
						{
							break;
						}
					}
				}
				std::sort(data.begin(), data.end());
				DateTime d = DateTime::Today;
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
				DateTime stop = date.Date.AddDays(1);
				if (date.Date == TimeUtil::CurrentTime().Date())
				{
					stop = TimeUtil::CurrentTime();
				}
				DateTime start = stop.Date.AddDays(-1 * 24);
				RecapRowAudit *rr = new RecapRowAudit();
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				rr->Cycle = HOSCYCLE::Canadian_Oil_Field.ToString();
				rr->Day = date.Date;
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
					Timelog *tt = data[0]->Clone();
					tt->Logtime = tt->Logtime.Date().AddDays(-1);
					data.Insert(0, tt);
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
						lasttl = tl->Clone();
						lasttl->Logtime = stop;
						if (date.Date == TimeUtil::CurrentTime().Date())
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
						if ((offtime.TotalHours() >= 24) && (offend <= stop) && (offend > date.Date))
						{
							if (offend == stop)
							{
								rr->DayUsed = TimeSpan::Zero;
							}
							else
							{
								rr->DayUsed = offend - date.Date;
							}
						}
					}
					while (offtime.TotalHours() >= 24)
					{
						offtime -= TimeSpan(24, 0, 0);
						count++;
					}
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
				rr->CycleTotal = stop - tl->Logtime - offtime - TimeSpan(72,0,0);
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

