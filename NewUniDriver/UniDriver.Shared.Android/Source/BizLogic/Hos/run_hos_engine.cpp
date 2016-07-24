#include "run_hos_engine.h"
#include "hos_events.h"
#include "Global.h"


string GetHosRecapSummary();
void JniApi_PostReceivedMsgProto(int req, int par, void *payload, int payload_len);

static void __add_recap_row(PRecapRow *rr, string did)
{
	TimeLogDB *tldb = TimeLogDB::Get();
	tldb->AddRecap(rr, did);
}
static void __update_recap_row(PRecapRow *rr, string did)
{
	TimeLogDB *tldb = TimeLogDB::Get();
	tldb->UpdateRecap(rr, did);
}
static DeferHourRow* __find_defer_hour(DateTime date, string did)
{
	TimeLogDB *tldb = TimeLogDB::Get();
	return tldb->FindDeferHourByDate(date, did);
}
static void __add_defer_hour(DeferHourRow* dhr,string did)
{
	TimeLogDB *tldb = TimeLogDB::Get();
	tldb->AddDeferHour(dhr, did);
}
static void __del_defer_hour(DeferHourRow* dhr, string did)
{
	TimeLogDB *tldb = TimeLogDB::Get();
	tldb->DeleteDeferHour(dhr, did);
}

static int avail_driving_min=0, avail_onduty_min=0,max_onduty=0,max_driving=0, avail_cycle = 0, MaxCycle = 0;
static DateTime shiftstart;
static string ondutyruleviolated, drivingruleviolated;

void printTlrList(PTimeLogRowList* list)
{
#if !ANDROID
	if (list != 0)
	{
		int len = list->list_size();
		for (int i = 0; i < len; i++)
		{
			PTimeLogRow *tlr = list->mutable_list(i);
			std::cout << "list: " << DateTime::ToStringFromMillis(tlr->logtime()) << " ev:" << tlr->event() << std::endl;
		}
	}
#endif
}
HourCalculator *gHc = 0;
void run_hos_engine()
{
	Global *g = Global::Get();
	gHc = new HourCalculator();
	string did= g->GetDriverID();
	//did = "1000";
#if ANDROID
	//LOGI("run_hos_engine()...did=%s\n", did.c_str());
#endif
	EmployeeRow *emp = g->GetCurrentProfile(/*did*/);
	if (emp == 0)
	{
		//LOGI("run_hos_engine()...emp == 0\n");
	}
	TimeLogDB *tldb = TimeLogDB::Get();
	PTimeLogRowList *list = tldb->GetAllTimeLogs(did);

	if (list->list_size() > 0)
	{
#if !ANDROID
		printTlrList(list);
#endif
	}
	else
	{
		 // need to create at least one at a midnight  OffDuty
		addTlr(0, DateTime(2016, 4, 25, 0, 0, 0)); // add and read again
		list = tldb->GetAllTimeLogs(did);
	}
	//LOGI("run_hos_engine()_1\n");
	PRecapRowList *recap_list = tldb->GetAllRecapByDriver(did);
	 PRuleSelectionHistoryRowList *rules = tldb->GetRuleSelectionHistory(did);   // get rules from DB
	gHc->CurrentLatitude = 0;
	gHc->CurrentLogitude = 0;
	//LOGI("run_hos_engine()_2\n");

	string cntr = "CA";// HOSAgent.GetCurrentHosCycleCountry();
	RuleExceptions rexcp = (RuleExceptions)emp->hosexcept();
	int cur_emp_cycle = (int)(HOSCYCLE)emp->cycle();
	if (rules->list_size() == 0)
	{
		PRuleSelectionHistoryRow *rule = new PRuleSelectionHistoryRow();
		rule->set_ruleid(cur_emp_cycle);
		rule->set_ruleexception((int)rexcp);
		rule->set_selecttime(DateTime::UtcNow().getMillis());
		rule->set_country(cntr);
		//save to DB
		tldb->AddRuleSelectionHistory(rule, did);
		rules->mutable_list()->AddAllocated(rule);
		//IMPORTANT!! Do not delete rule here as it will be deleted when we do <delete rules;>
	}
	else {
		PRuleSelectionHistoryRow *rl1 = tldb->GetLastRuleByDriverCountry(did, cntr);
		if (rl1 == 0 || (rl1->ruleid() != cur_emp_cycle || rl1->ruleexception() !=(int) rexcp)) {
			PRuleSelectionHistoryRow *rule = new PRuleSelectionHistoryRow();
			rule->set_ruleid(cur_emp_cycle);
			rule->set_ruleexception((int)rexcp);
			rule->set_country(cntr);
			rule->set_selecttime(TimeUtil::CurrentTime().getMillis());
			tldb->AddRuleSelectionHistory(rule, did);
			rules->mutable_list()->AddAllocated(rule);
		}
		if (rl1 != 0)
			delete rl1;
	}
	//LOGI("run_hos_engine()_3\n");
	gHc->SetHourCalculator(list, recap_list, rules, emp);
	gHc->AddRecapEvent = __add_recap_row;
	gHc->UpdateRecapEvent = __update_recap_row;
	gHc->FindDeferHourByDateEvent = __find_defer_hour;
	gHc->add_dhr_cb = __add_defer_hour;
	gHc->del_dhr_cb= __del_defer_hour;
	//LOGI("run_hos_engine()_4\n");
	try {
		gHc->CalculateHours();

		avail_driving_min = gHc->AvaliableDrivingMinutes;
		avail_onduty_min = gHc->AvaliableOnDutyMinutes;
		max_driving = gHc->maxDriving;
		max_onduty = gHc->maxOnduty;
		avail_cycle = gHc->avaliableCycle;
		MaxCycle = gHc->maxCycle;
		
			
		shiftstart = (DateTime)gHc->ShiftStart;
		ondutyruleviolated = gHc->AvaliableDrivingRule;
		drivingruleviolated = gHc->AvaliableOnDutyRule;
#if !ANDROID
		std::cout << "shiftstart: " << shiftstart.ToString() << std::endl;
#endif
		
		{
			//Update Java side
			/*int64_t val = avail_driving_min;
			val = (int64_t)((val << 32) | avail_onduty_min);
			*/
			string str = GetHosRecapSummary();
#if ANDROID
			JniApi_PostReceivedMsgProto(1000, 0, (void*)str.c_str(), str.size());
#endif
		}
	}
	catch (std::exception &ex)
	{
#if !ANDROID
		std::cout << "dt1: " << std::string(ex.what()) << std::endl;
#else
		//LOGI("hc->CalculateHours...Exc:%s\n", string(ex.what()).c_str());
#endif
	}
	catch (...)
	{
		//LOGI("hc->CalculateHours....ExcALL:\n");
	}
	//LOGI("run_hos_engine()_5\n");
	delete list;
	delete recap_list;
	delete rules;
	//if (rule != 0)
	//	delete rule;   //IMPORTANT!! Do not delete rule here as it will be deleted when we do <delete rules;>

	if (gHc != 0)
		delete gHc;
}

string GetHosRecapSummary()
{
	//avail_driving_min=0, avail_onduty_min=0,max_onduty=0,max_driving=0, avail_cycle = 0, MaxCycle = 0;
	string str = Int2Str(avail_driving_min);
	string rv = str + "|";
	str = Int2Str(avail_onduty_min);
	rv = rv + str + "|";

	str = Int2Str(max_onduty);
	rv = rv + str + "|";

	str = Int2Str(max_driving);
	rv = rv + str + "|";

	str = Int2Str(avail_cycle);
	rv = rv + str + "|";

	str = Int2Str(MaxCycle);
	rv = rv + str +"|";

	return rv;
}

