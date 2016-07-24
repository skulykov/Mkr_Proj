

			private:
				std::vector<AuditTime*> AuditUS(std::vector<Timelog*> &data, int drivinglimit, int ondutylimit, RULEID *rule);
				int CheckOnDutyHoursLimitCount(std::vector<Timelog*> &data, int ondutylimit, RULEID *rule, DateTime startTime, int days);
				DateTime USFindShiftStartBySleeper(std::vector<Timelog*> &data, int sleeplimit, TimeSpan &lastslpduration, std::vector<OffdutySession*> &sleeps);
				AuditTime *Audit_US_Shift_Driving(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit, std::vector<OffdutySession*> &sleeps);

				AuditTime *Audit_US_Shift_OnDuty(std::vector<Timelog*> &data, DateTime end, DateTime shiftstart, int limit, std::vector<OffdutySession*> &sleeps);
		//=========================Added from AuditCA_Rules========================================
				AuditTime *Audit_Recap(std::vector<RecapRowAudit*> &recaps, std::vector<OffdutySession*> &offduties, RULEID *rule, DateTime date, std::vector<Timelog*> &data);
				AuditTime *Audit_CA_70Hour_1(std::vector<Timelog*> &data, DateTime date, int limit);
				AuditTime *Audit_CA_70Hour_2(std::vector<Timelog*> &data, DateTime date, int limit);
				void Audit_DeferredRules(std::vector<Timelog*> &data, std::vector<OffdutySession*> &offduties, std::vector<AuditTime*> &result, DateTime dayStart);
				DeferHourRow *FindExistingDeferHour(DateTime date);


