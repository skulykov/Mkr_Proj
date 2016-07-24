

			const DateTime &AuditLogic::OffdutySession::getstart() const
			{
				return _start;
			}

			void AuditLogic::OffdutySession::setstart(const DateTime &value)
			{
				_start = value;
				_ts = _stop - _start;
			}

			const DateTime &AuditLogic::OffdutySession::getstop() const
			{
				return _stop;
			}

			void AuditLogic::OffdutySession::setstop(const DateTime &value)
			{
				_stop = value;
				_ts = _stop - _start;
			}

			const TimeSpan &AuditLogic::OffdutySession::getts() const
			{
				return _ts;
			}

			std::string AuditLogic::OffdutySession::ToString()
			{
				return std::string::Format("start={0}, stop={1}, span={2}, next duty={3}", getstart(), getstop(), getts(), prev);
			}

			OffdutySession *AuditLogic::OffdutySession::Clone()
			{
				OffdutySession *os = new OffdutySession();
				os->setstart(this->_start);
				os->setstop(this->_stop);
				os->prev = this->prev;
				os->StopAction = this->StopAction;
				os->current = this->current;
				return os;
			}

			std::vector<OffdutySession*> AuditLogic::MergeOffduty(std::vector<Timelog*> &data, DateTime end)
			{
				return MergeOffduty(data, end, true, nullptr);
			}

			std::vector<OffdutySession*> AuditLogic::MergeOffduty(std::vector<Timelog*> &data, DateTime end, bool mergeSleepOff, Nullable<DateTime> endDT)
			{

				int start = 0; // FindStart(data, date);
				std::vector<OffdutySession*> sessions;
				for (int i = start; i < data.size(); i++)
				{
					if (endDT.HasValue && data[i]->Logtime > endDT)
					{
						break;
					}
					if ((data[i]->duty == DutyStatus::Sleeping) || (data[i]->duty == DutyStatus::Offduty))
					{
						OffdutySession *os = new OffdutySession();
						os->setstart(data[i]->Logtime);
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
							os->setstop(data[i + 1]->Logtime);
						}
						else
						{
							os->setstop(end);
						}
						sessions.push_back(os);
						if (os->getstop() > end)
						{
							os->setstop(end);
							break;
						}
					}
				}
				for (int i = sessions.size() - 1; i >= 1; i--)
				{
					if ((sessions[i]->getstart() == sessions[i - 1]->getstop()) && ((sessions[i]->prev == DutyStatus::Offduty) || sessions[i]->prev == DutyStatus::Sleeping) && (sessions[i]->StopAction == sessions[i - 1]->StopAction) && ((sessions[i]->current == sessions[i - 1]->current) || mergeSleepOff))
					{
						sessions[i - 1]->setstop(sessions[i]->getstop());
						sessions.RemoveAt(i);
					}
				}
				for (int i = sessions.size() - 1; i >= 0; i--)
				{
					//If less than 30 minutes, then it is not off duty
					if (sessions[i]->getts().TotalMinutes < AuditLogic::OffDutyMinutes)
					{
						sessions.RemoveAt(i);
					}
				}
				return sessions;

			}

			DateTime AuditLogic::CA_FindShiftStartByOffduty(std::vector<Timelog*> &data)
			{
				std::vector<OffdutySession*> offdutys = MergeOffduty(data, TimeUtil::CurrentTime);
				DateTime shiftStart = DateTime::MinValue;
				DateTime satisfingEndTime = DateTime::MinValue;
				TimeSpan extra = TimeSpan(0);
				if (data.empty())
				{
					return TimeUtil::CurrentTime;
				}
				DeferHourRow *dhr = FindExistingDeferHour(TimeUtil::CurrentTime::Date);
				for (int i = 0; i < offdutys.size(); i++)
				{
					double limit = 8;
					if ((dhr != nullptr) && (offdutys[i]->getstart()->Date == dhr->day2.Date))
					{
						limit += dhr->defers.TotalHours;
					}
					if ((offdutys[i]->getts().TotalHours >= limit) && (offdutys[i]->StopAction != AuditLogic::STANDBYOFFTIME))
					{
						if (offdutys[i]->getstart()->Date == offdutys[i]->getstop().Date)
						{
							shiftStart = offdutys[i]->getstop();
							satisfingEndTime = offdutys[i]->getstart().AddHours(8);
							continue;
						}
						else //now 8 hour consecutive is cross two days check for double dipping
						{
							if (satisfingEndTime.Date < offdutys[i]->getstart().Date)
							{
								satisfingEndTime = offdutys[i]->getstart().Date;
							}
							TimeSpan offtime = TimeSpan(0);
							for (int j = 0; j < offdutys.size(); j++)
							{
								if ((offdutys[j]->getstop() >= satisfingEndTime) && (offdutys[j]->getstop()->Date == offdutys[i]->getstart().Date))
								{
									if (offdutys[j]->getstart() > satisfingEndTime)
									{
										offtime += offdutys[j]->getts();
									}
									else
									{
										offtime += offdutys[j]->getstop() - satisfingEndTime;
									}
								}
							}
							DateTime start = offdutys[i]->getstart();
							if (offtime.TotalHours < 2)
							{
								start = offdutys[i]->getstart().AddHours(2 - offtime.TotalHours);
							}
							TimeSpan ts = offdutys[i]->getstop() - start;
							if (ts.TotalHours >= limit)
							{
								shiftStart = offdutys[i]->getstop();
								satisfingEndTime = start.AddHours(8);
							}
						}
					}

				}
				return shiftStart;
			}

			DateTime AuditLogic::US_FindShiftStartByOffduty(std::vector<Timelog*> &data)
			{
				std::vector<OffdutySession*> offdutys = MergeOffduty(data, TimeUtil::CurrentTime);
				DateTime shiftStart = DateTime::MinValue;
				DateTime satisfingEndTime = DateTime::MinValue;
				TimeSpan extra = TimeSpan(0);
				if (data.empty())
				{
					return TimeUtil::CurrentTime;
				}
				for (int i = 0; i < offdutys.size(); i++)
				{
					double limit = 8;
					if (offdutys[i]->getts().TotalHours >= limit)
					{
						if (offdutys[i]->getstart()->Date == offdutys[i]->getstop().Date)
						{
							shiftStart = offdutys[i]->getstop();
							satisfingEndTime = offdutys[i]->getstart().AddHours(8);
							continue;
						}
					}
				}
				return shiftStart;
			}

			DateTime AuditLogic::FindShiftStartBySleeper(std::vector<Timelog*> &data, int sleeplimit, TimeSpan &lastslpduration)
			{
				std::vector<OffdutySession*> sleeps = Audit_CA_MergeSleep(data, TimeUtil::CurrentTime);
				for (int i = sleeps.size() - 1; i > 0; i--)
				{
					if ((sleeps[i]->getts().TotalHours < 2) || (sleeps[i - 1]->getts().TotalHours < 2))
					{
						continue;
					}
					if ((sleeps[i]->getts().TotalHours + sleeps[i - 1]->getts().TotalHours) >= sleeplimit)
					{
						lastslpduration = sleeps[i]->getts();
						return sleeps[i - 1]->getstop();
					}
				}
				lastslpduration = TimeSpan(0);
				return DateTime::MinValue;
			}

			TimeSpan AuditLogic::Calculate_TotalOff_InDate(std::vector<OffdutySession*> &todaylist, DateTime searchDate, DateTime dayend, OffdutySession *consectiveoff)
			{
				TimeSpan dayoff = TimeSpan(0);
				for (int i = 0; i < todaylist.size(); i++)
				{
					if (todaylist[i]->getstart() < searchDate)
					{
						dayoff += (todaylist[i]->getstop() - searchDate);
					}
					if ((todaylist[i]->getstart() >= searchDate) && (todaylist[i]->getstop() <= dayend))
					{
						dayoff += todaylist[i]->getts();
					}
					if ((todaylist[i]->getstart() < dayend) && (todaylist[i]->getstop() > dayend))
					{
						dayoff += (dayend - todaylist[i]->getstart());
					}
				}
				return dayoff;
			}

			std::vector<OffdutySession*> AuditLogic::Get_Offduty_Session_TodayList(std::vector<OffdutySession*> &sessions, DateTime date)
			{
				std::vector<OffdutySession*> todaylist;
				for (auto os : sessions)
				{
					if (os->getstop().Date == date)
					{
						todaylist.push_back(os);
					}
					else if (os->getstart().Date == date)
					{
						todaylist.push_back(os);
					}
				}
				return todaylist;
			}

			std::vector<AuditTime*> AuditLogic::Audit_CA_ShiftRule(std::vector<Timelog*> &data, std::vector<OffdutySession*> &offduties, Nullable<DateTime> &shiftStartDt)
			{
				DateTime end = TimeUtil::CurrentTime;
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
				AuditTime *at = new AuditTime();
				at->rule = RULEID::CA_Driving;
				at->DispName = "Driving";
				at->ViolationDescription = "Driving time limit for work shift exceeded";
				at->ruletype = RuleType::Shift;
				at->available = TimeSpan(limit, 0, 0);
				std::vector<DutyStatus*> required;
				required.push_back(DutyStatus::Driving);
				TimeSpan driving = CalculateHours(data, required, shiftstart, end);
				at->available = TimeSpan(limit, 0, 0) - driving;
				at->RuleDescription = std::string::Format("{0}{1}{2}", "Your are about to exceed driving time limit for work shift in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				at->used = driving;
				return at;
			}

			AuditTime *AuditLogic::Audit_CA_Shift_OnDutyRule(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit)
			{
				AuditTime *at = new AuditTime();
				at->DispName = "On Duty";
				at->ViolationDescription = "On duty time limit for work shift exceeded";
				at->rule = RULEID::CA_OnDuty;
				at->ruletype = RuleType::Shift;
				at->available = TimeSpan(limit, 0, 0);
				std::vector<DutyStatus*> required;
				required.push_back(DutyStatus::Driving);
				required.push_back(DutyStatus::Onduty);
				TimeSpan onduty = CalculateHours(data, required, shiftstart, end); // new TimeSpan(0);
				at->available = TimeSpan(limit,0,0) - onduty;
				at->ondutyavailable = at->available;
				at->RuleDescription = std::string::Format("{0}{1}{2}", "Your are about to exceed on duty time limit for work shift in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				;
				at->used = onduty;
				return at;

			}

			AuditTime *AuditLogic::Audit_CA_Shift_16HourRule(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit, TimeSpan lastslp)
			{
				Timelog *lasttl = nullptr;
				Timelog *tl = nullptr;
				AuditTime *at = new AuditTime();
				at->rule = RULEID::CA_16Hour;
				at->ruletype = RuleType::Shift;
				at->used = (end - shiftstart) - lastslp;
				at->available = TimeSpan(limit, 0, 0) - at->used;
				at->DispName = "Elapsed";
				at->ViolationDescription = "Elapsed work shift hours exceeded";
				at->RuleDescription = std::string::Format("{0}{1}{2}", "Your are about to exceed elapsed work shift hours in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				return at;
			}

			std::vector<AuditTime*> AuditLogic::Audit_CA_DayRule(std::vector<Timelog*> &data, double drivinglimit, double ondutylimit, DateTime dayStart)
			{
				std::vector<AuditTime*> violations;
				Timelog *lasttl = nullptr;
				//DateTime start = TimeUtil.CurrentTime.Date;

				DateTime start = dayStart;
				DateTime end = TimeUtil::CurrentTime; // date.Date.AddDays(1);

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
						lasttl = lasttl->Clone();
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
				AuditTime *at = new AuditTime();
				at->rule = RULEID::CA_Driving;
				at->DispName = "Driving";
				at->ViolationDescription = "Daily driving time limit exceeded";
				at->ruletype = RuleType::Day;
				at->available = TimeSpan(static_cast<int>(drivinglimit),0,0) - driving;
				at->RuleDescription = std::string::Format("{0}{1}{2}", "Your are about to exceed daily driving time limit in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				at->used = driving;
				violations.push_back(at);
				at = new AuditTime();
				at->DispName = "On Duty";
				at->ViolationDescription = "Daily on duty time limit exceeded";
				at->available = TimeSpan(static_cast<int>(ondutylimit),0,0) - onduty;
				at->rule = RULEID::CA_OnDuty;
				at->ondutyavailable = at->available;
				at->ruletype = RuleType::Day;
				at->used = onduty;
				at->RuleDescription = std::string::Format("{0}{1}{2}", "Your are about to exceed daily on duty time limit in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				violations.push_back(at);
				return violations;
			}

			DeferHourRow *AuditLogic::CheckPrevDayDeferCondition(std::vector<OffdutySession*> &offduties, std::vector<Timelog*> &data, double offdutylimit, Cycle *cycle, DateTime dayStart)
			{
				DateTime prevDay = dayStart.AddHours(-24);
				DateTime end = prevDay.AddDays(1);

				DeferHourRow *dhr = FindExistingDeferHour(prevDay);
				if (dhr != nullptr && dhr->day2->Date == prevDay.Date)
				{
					return nullptr;
				}
				//if (dhr != null) return null; //it must be day2 of prev days
				DateTime timemark = TimeUtil::CurrentTime;
				double corehours = 0;
				DateTime corehoursStop = DateTime::MinValue;
				TimeSpan offduty = GetTotalOffdutyInDate(offduties, prevDay, end, timemark, corehours, corehoursStop);
				if (corehours < 8)
				{
					if (dhr != nullptr)
					{
						HourCalculator::DeleteDeferHour(dhr, CurrentAuditDriver::ID, CurrentDefers);
					}
					return nullptr;
				}
				double delta = offdutylimit - offduty.TotalHours;
				if ((delta > 0) && (delta <= 2))
				{
					//if (CheckDeferDay1New(data, prevDay, cycle))
					if (CheckDeferDay1(offduties, data, prevDay, cycle, delta, false))
					{
						if (dhr == nullptr)
						{
							dhr = new DeferHourRow();
							dhr->day1 = corehoursStop;
							dhr->day2 = dhr->day1.AddDays(1);
							dhr->defers = TimeSpan(static_cast<int>(delta), static_cast<int>((delta - static_cast<int>(delta)) * 60), 0);
							HourCalculator::AddDeferHour(dhr, CurrentAuditDriver::ID, CurrentDefers);
						}
						else
						{
							dhr->defers = TimeSpan(static_cast<int>(delta), static_cast<int>((delta - static_cast<int>(delta)) * 60), 0);
						}
					}
					else
					{
						HourCalculator::DeleteDeferHour(dhr, CurrentAuditDriver::ID, CurrentDefers);
						return nullptr;
					}
				}
				else
				{
					if (dhr != nullptr)
					{
						HourCalculator::DeleteDeferHour(dhr, CurrentAuditDriver::ID, CurrentDefers);
					}
					return nullptr;
				}
				return dhr;
			}

			TimeSpan AuditLogic::FindOffDutyToEnd()
			{
				TimeSpan totalHr = TimeSpan(0);
				DateTime offDateTime = DateTime::MinValue;
				if (CurrentAuditDriverTimeLogs[CurrentAuditDriverTimeLogs->Count - 1]->Event == LOGSTATUS::OffDuty || CurrentAuditDriverTimeLogs[CurrentAuditDriverTimeLogs->Count - 1]->Event == LOGSTATUS::Sleeping)
				{
					for (int index = CurrentAuditDriverTimeLogs->Count - 1; index >= 0; index--)
					{
						if (CurrentAuditDriverTimeLogs[index]->Event == LOGSTATUS::OffDuty || CurrentAuditDriverTimeLogs[index]->Event == LOGSTATUS::Sleeping)

						{
							offDateTime = CurrentAuditDriverTimeLogs[index].LogTime;
						}
						else
						{
							break;
						}
					}
				}

				if (offDateTime != DateTime::MinValue && (TimeUtil::CurrentTime - offDateTime).TotalMinutes <= 15)
				{
					totalHr = TimeUtil::CurrentTime::Date::AddHours(24) - offDateTime;
				}
				return totalHr;
			}

			AuditTime *AuditLogic::Audit_CA_Offduty_DayRule(std::vector<OffdutySession*> &offduties, std::vector<Timelog*> &data, double offdutylimit, Cycle *cycle, DateTime dayStart)
			{
				AuditTime *at = new AuditTime();
				at->rule = RULEID::CA_OffDuty;
				at->DispName = "Off duty";
				at->ViolationDescription = "Off-duty time is less than mandatory hours";
				at->ruletype = RuleType::Day;

				DateTime date = dayStart.Date;
				DateTime timemark = date;
				DateTime end = TimeUtil::CurrentTime;
				double corehours = 0;
				DateTime corehoursStop = DateTime::MinValue;
				TimeSpan offduty = GetTotalOffdutyInDate(offduties, date, end, timemark, corehours, corehoursStop);
				at->used = offduty;

				DeferHourRow *dhr_today = FindExistingDeferHour(dayStart.Date);
				DeferHourRow *dhr = CheckPrevDayDeferCondition(offduties, data, offdutylimit, cycle, dayStart);
				if (dhr != nullptr && dhr->day2->Date == date)
				{
					if (dhr_today != nullptr && dhr_today->day1->Date == date)
					{
						HourCalculator::DeleteDeferHour(dhr_today, CurrentAuditDriver::ID, CurrentDefers);
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
					if (dhr->day2->Date == dayStart.Date)
					{
						double corehoursPre = 0;
						DateTime corehoursStopPre = DateTime::MinValue;
						TimeSpan prevday = GetTotalOffdutyInDate(offduties, date.AddHours(-24), date, timemark, corehoursPre, corehoursStopPre);
						delta = 10 - (prevday.TotalHours);
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
				if (offduty.TotalHours >= offdutylimit && corehours >= (8 + delta))
				{
					at->available = TimeSpan(13, 0, 0);
					at->notNeedCal = true;
				}
				else
				{
					TimeSpan left = dayStart.AddHours(24) - TimeUtil::CurrentTime;
					TimeSpan offLeft = FindOffDutyToEnd();
					if (offLeft.TotalSeconds != 0)
					{
						left = offLeft;
					}
					double limiteLeft = offduty.TotalHours - (delta + 8);
					//core hours passed already
					if (corehours >= delta + 8)
					{
						if ((left.TotalHours > (2 - limiteLeft)) || (left.TotalHours == (2 - limiteLeft) && (CurrentAuditDriverTimeLogs[CurrentAuditDriverTimeLogs->Count - 1]->Event == LOGSTATUS::OffDuty || CurrentAuditDriverTimeLogs[CurrentAuditDriverTimeLogs->Count - 1]->Event == LOGSTATUS::Sleeping)))
						{
							double available = left.TotalHours - (2 - limiteLeft);
							int available_h = static_cast<int>(floor(available));
							at->available = TimeSpan(available_h, static_cast<int>((available - available_h) * 60), 0);
						}
						else
						{
							at->available = TimeSpan(0);
							//11.16
							if ((CurrentAuditDriverTimeLogs[CurrentAuditDriverTimeLogs->Count - 1]->Event == LOGSTATUS::OnDuty || CurrentAuditDriverTimeLogs[CurrentAuditDriverTimeLogs->Count - 1]->Event == LOGSTATUS::Driving) || left.TotalMinutes < 30)
							{
								dfhours = (2 - limiteLeft);
							}
							else
							{
								dfhours = (2 - limiteLeft - left.TotalHours);
							}
						}
					}
					else
					{
						//Get last off duty hours
						TimeSpan lastoffdutyHour = TimeSpan(0);
						if (offduties.size() > 0 && offduties.size() > 0 && offduties[offduties.size() - 1]->getstop() == EndOffduty)
						{
							lastoffdutyHour = offduties[offduties.size() - 1]->getts() + (dayStart.AddHours(24) - offduties[offduties.size() - 1]->getstop());
						}
						else
						{
							lastoffdutyHour = (dayStart.AddHours(24) - TimeUtil::CurrentTime);
						}
						if (dhr != nullptr && dhr->day2->Date == date) //Defer need core hours
						{
							if (lastoffdutyHour.TotalHours >= delta + 8)
							{
								double offdutytoend = (offduty + (dayStart.AddHours(24) - TimeUtil::CurrentTime)).TotalHours;
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
							double offdutytoend = (offduty + (dayStart.AddHours(24) - TimeUtil::CurrentTime)).TotalHours;
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
					if (dhr->day1->Date == dayStart.Date)
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
								HourCalculator::DeleteDeferHour(dhr, CurrentAuditDriver::ID, CurrentDefers);
							}
						}
						else
						{
							HourCalculator::DeleteDeferHour(dhr, CurrentAuditDriver::ID, CurrentDefers);
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
							dhr = new DeferHourRow();
							dhr->day1 = corehoursStop;
							dhr->day2 = dhr->day1.AddDays(1);
							int dfhours_h = static_cast<int>(floor(dfhours));
							//11.16
							dhr->defers = TimeSpan(dfhours_h, static_cast<int>((dfhours - dfhours_h) * 60), 0);
							HourCalculator::AddDeferHour(dhr, CurrentAuditDriver::ID, CurrentDefers);
						}
					}
				}
				if (at->available->TotalHours == 13)
				{
					at->ondutyavailable = TimeSpan(14, 0, 0);
				}
				else
				{
					at->ondutyavailable = at->available;
				}
				at->RuleDescription = std::string::Format("{0}{1}{2}", "You are about to violate minimum mandatory off-duty time requirement in ", std::to_string(Math::Round(at->ondutyavailable.TotalMinutes, 2)), " minute(s)");
				return at;
			}

			TimeSpan AuditLogic::GetTotalOffdutyInDate(std::vector<OffdutySession*> &offduties, DateTime start, DateTime end, DateTime &timemark, double &corehours, DateTime &corehoursStop)
			{
				corehours = 0;
				TimeSpan offduty = TimeSpan(0);
				for (auto os : offduties)
				{
					if (os->getstop() <= start)
					{
						continue;
					}
					if (os->getstart() >= end)
					{
						break;
					}
					if (os->getstop() <= end)
					{
						if (os->getstart() < start)
						{
							offduty = os->getts() - (start - os->getstart());
							timemark = os->getstop();
						}
						else
						{
							offduty += os->getts();
							timemark = os->getstart();
						}
					}
					if (os->getstop() > end)
					{
						if (os->getstart() <= start)
						{
							offduty = TimeSpan(24, 0, 0);
							timemark = start;
						}
						else
						{
							timemark = os->getstart();
							offduty += end - os->getstart();
						}
					}

					//devin 2012-08-17
					if (os->getstart() < start && os->getstop() > start && os->getstop() <= end)
					{
						double todayOffHours = os->getts().TotalHours - (start - os->getstart()).TotalHours;

						if (todayOffHours >= 8 && corehours < todayOffHours)
						{
							corehours = todayOffHours;
							corehoursStop = os->getstop();
						}
					}

					//devin 11.16
					if (os->getstart() < start && os->getstop() > end)
					{
						corehours = (TimeSpan(24, 0, 0)).TotalHours;
						corehoursStop = os->getstop();
					}
					if (os->getstart() >= start && os->getstop() <= end && os->getts().TotalHours >= 8 && corehours < os->getts().TotalHours)
					{
						corehours = os->getts().TotalHours;
						corehoursStop = os->getstop();
					}
					if (os->getstart() >= start && os->getstop() > end)
					{
						double todayOffHours = (end - os->getstart()).TotalHours;

						if (todayOffHours >= 8 && corehours < todayOffHours)
						{
							corehours = todayOffHours;
							corehoursStop = os->getstop();
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
				for (int i = 0;i < data.size();i++)
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
						if (os->getstart() >= driverLastTime && os->getstart() <= dayEnd)
						{
							if (os->getstop() >= dayEnd)
							{
								off_duty = off_duty + dayEnd.Subtract(os->getstart()).TotalHours;
							}
							else
							{
								off_duty = off_duty + os->getts().TotalHours;
							}
						}
						else
						{
							break;
						}
					}


					if ((CurrentAuditDriverTimeLogs[CurrentAuditDriverTimeLogs->Count - 1]->Event == LOGSTATUS::OffDuty || CurrentAuditDriverTimeLogs[CurrentAuditDriverTimeLogs->Count - 1]->Event == LOGSTATUS::Sleeping))
					{
						off_duty = off_duty + (dayEnd - CurrentAuditDriverTimeLogs[CurrentAuditDriverTimeLogs->Count - 1].LogTime).TotalHours;
					}


					if ((dayEnd.Subtract(driverLastTime).TotalHours - off_duty) > dfhours)
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

				DateTime shiftstart = FindShiftStart(data, 8, nullptr);
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
					ts = TimeUtil::CurrentTime - lasttl->Logtime;
					if ((lasttl->duty == DutyStatus::Onduty) || (lasttl->duty == DutyStatus::Driving))
					{
						if (lasttl->duty == DutyStatus::Driving)
						{
							driving += ts;
						}
						onduty += ts;
					}
				}
				AuditTime *at = new AuditTime();
				at->ruletype = RuleType::Shift;
				at->DispName = "Elapsed";
				at->ViolationDescription = "Elapsed work shift hours exceeded";
				at->rule = RULEID::CA_16Hour;
				at->used = TimeUtil::CurrentTime - shiftstart - lastslp;
				at->available = TimeSpan(elapslimit, 0, 0) - at->used;
				at->RuleDescription = std::string::Format("{0}{1}{2}", "Your are about to exceed elapsed work shift hours in ", std::to_string(Math::Round(at->available.TotalMinutes,2)), " minute(s)");
				violations.push_back(at);
				at = new AuditTime();
				at->ruletype = RuleType::Shift;
				at->rule = RULEID::CA_Driving;
				at->DispName = "Driving";
				at->ViolationDescription = "Driving time limit for work shift exceeded";
				at->used = driving;
				at->available = TimeSpan(drivinglimit,0,0) - driving;
				at->RuleDescription = std::string::Format("{0}{1}{2}", "Your are about to exceed driving time limit for work shift in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				violations.push_back(at);
				at = new AuditTime();
				at->ruletype = RuleType::Shift;
				at->rule = RULEID::CA_OnDuty;
				at->DispName = "On Duty";
				at->ViolationDescription = "On duty time limit for work shift exceeded";
				at->used = onduty;
				at->available = TimeSpan(ondutylimit, 0,0) - onduty;
				at->RuleDescription = std::string::Format("{0}{1}{2}", "Your are about to exceed on duty time limit for work shift in ", std::to_string(Math::Round(at->available.TotalMinutes, 2)), " minute(s)");
				at->ondutyavailable = at->available;
				violations.push_back(at);

				return violations;
			}

			std::vector<OffdutySession*> AuditLogic::Audit_CA_MergeSleep(std::vector<Timelog*> &data, DateTime end)
			{
				std::vector<OffdutySession*> sessions;
				for (int i = 0; i < data.size();i++)
				{
					if (data[i]->duty == DutyStatus::Sleeping)
					{
						OffdutySession *os = new OffdutySession();
						os->setstart(data[i]->Logtime);
						os->prev = DutyStatus::Offduty;
						if (i > 0)
						{
							os->prev = data[i - 1]->duty;
						}
						if (i < (data.size() - 1))
						{
							os->setstop(data[i + 1]->Logtime);
						}
						else
						{
							os->setstop(end);
						}
						sessions.push_back(os);
						if (os->getstop() > end)
						{
							os->setstop(end);
							break;
						}
					}
				}
				for (int i = sessions.size() - 1; i >= 1; i--)
				{
					if ((sessions[i]->getstart() == sessions[i - 1]->getstop()) && (sessions[i]->prev == DutyStatus::Sleeping))
					{
						sessions[i - 1]->setstop(sessions[i]->getstop());
						sessions.RemoveAt(i);
					}
				}
				if ((sessions.size() > 0) && (sessions[sessions.size() - 1]->getstop() > end))
				{
					sessions.pop_back();
				}
				return sessions;
			}
