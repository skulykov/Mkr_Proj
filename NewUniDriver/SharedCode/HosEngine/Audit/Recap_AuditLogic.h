private:
	std::vector<RecapRowAudit*> CalculateRecap(AuditRequest *ar, RULEID rule, std::vector<RuleExceptions> &ruleException);

	RecapRowAudit *ApplyRule(AuditRequest *ar, Timelog *lasttl, RULEID *&rid, std::vector<RuleExceptions> &ruleException);

	std::vector<RuleExceptions> GetRuleExceptions(int rule);

	RULEID *FindApplyingRule(Timelog *tl, std::vector<Cycle*> &cyclelist, std::vector<RuleExceptions> &ruleException);
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
	//	RecapRowAudit *Cycle(std::vector<Timelog*> &data, DateTime date, int days, int limit, int resettime, const std::string &cycle, bool isUSA);

	void TestCycle();

	RecapRowAudit *CAOilfield(std::vector<Timelog*> &source, DateTime date);
