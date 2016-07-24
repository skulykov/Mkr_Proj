#include "TimeLogDB.h"
#include "Dbg.h"

TimeLogDB *TimeLogDB::_instance = nullptr;
#pragma region TimeLogDB
			

			TimeLogDB *TimeLogDB::Get()
			{
				if (_instance == nullptr)
				{
					_instance = new TimeLogDB();
				}
				return _instance;
			}
			void TimeLogDB::CreateTables()
			{
				SQLiteStatement *pStmt = CreateConnection();
				try
				{
					if (!IsTableExist(pStmt, "TIMELOG"))
					{
						CreateTimeLogTable(pStmt);
					}
					if (!IsTableExist(pStmt, "DeferHours"))
					{
						CreateDeferHoursTable(pStmt);
					}
					if (!IsTableExist(pStmt, "Recap"))
					{
						CreateRecapTable(pStmt);
					}
					
					if (!IsTableExist(pStmt, "RuleSelectionHistory"))
					{
						CreateHOSRulesTable(pStmt);
					}
					if (!IsTableExist(pStmt, "HOSAlerts"))
					{
						CreateAlertsTable(pStmt);
					}
				}
				catch (SQLiteException &e)
				{
					string str = e.GetString();
					LOGE(str.c_str());
				}

				CloseConn();
			}
			void TimeLogDB::CreateTimeLogTable(SQLiteStatement *pStmt)
			{
				// Create a db table to store timelog report
				auto sql = std::string("CREATE TABLE TIMELOG (Id INTEGER PRIMARY KEY AUTOINCREMENT, LogStatus INTEGER, LogTime datetime, ") + 
					std::string("Logbookstopid INTEGER, Odometer INTEGER, DriverId ntext, HaveSent BOOLEAN, IsFromServer BOOLEAN, Latitude, Longitude, BoxID INTEGER, EquipmentID ntext,OrigLogTime datetime,Editor integer,Comment ntext,Signed BOOLEAN,Type integer,Codriver ntext,TimeZone,DaylightSaving,Address ntext,arGeo integer,QualifyRadiusRule ntext ); ");

				CreateTableA(pStmt, sql);
			}

			PTimeLogRowList* TimeLogDB::GetAllTimeLogs(const std::string &DriverID)
			{
				//added Type<>3 means all which are not modified
				std::string sql = "SELECT * FROM TIMELOG ORDER BY LogTime";
				if (DriverID != "" && !DriverID.find("-1") != std::string::npos)
				{      // desc limit 14
					   sql = string_format("SELECT * FROM TIMELOG Where DriverID='%s' and Type<>3  ORDER BY LogTime",DriverID.c_str());
				}

				//var sql = "SELECT * FROM TIMELOG";
				return getTTList(sql,0,false);
			}

			PTimeLogRowList* TimeLogDB::getTTList(const std::string &sql, int _status, bool _all)
			{
				PTimeLogRowList* list=new PTimeLogRowList();
				SQLiteStatement *p = CreateConnection();
				p->Sql(sql);

				try
				{
				
					while (p->FetchRow())
					{
						int id = p->GetColumnInt(0);
						int status = p->GetColumnInt(1);
						int type = p->GetColumnInt(16);
						TimeLogRow *logTime = NULL;
						if (_all)
						{
							logTime = new TimeLogRow();//(TimeLogRow *) list->add_list();
						}
						else
						{
							if (type != (int)TimeLogType::Modified)
							{
								logTime = new TimeLogRow(); //(TimeLogRow *) list->add_list();
							}
						}
						if (logTime == NULL)
							continue;
						
						std::string dtStr = p->GetColumnString(2);
						DateTime _dt_ = DateTime::parse(dtStr);
						logTime->init(
								id,
								status, _dt_.getMillis(),
								p->GetColumnInt(3),
								p->GetColumnInt(4),
								p->GetColumnString(5),
								p->GetColumnBool(6),
								p->GetColumnBool(7),
								p->GetColumnDouble(8),
								p->GetColumnDouble(9),
								p->GetColumnInt(10),
								p->GetColumnString(11)
							);
							DateTime origTS = DateTime::parse(p->GetColumnString(12));
							int editor_flag = p->GetColumnInt(13);
							std::string comment = p->GetColumnString(14);
							logTime->updateEdit(origTS.getMillis(), editor_flag, comment);
							bool signed_Renamed = p->GetColumnBool(15);
							logTime->set_type(p->GetColumnInt(16));
							logTime->SetSigned(signed_Renamed);
							std::string s = "";
							//try{
							s = p->GetColumnString(17);

							logTime->set_codriver(s);
							logTime->set_tz(p->GetColumnDouble(18));
							logTime->set_dst( p->GetColumnDouble(19));
							logTime->set_addr(p->GetColumnString(20));
							logTime->set_argeo( p->GetColumnInt(21));
							s = p->GetColumnString(22);
							logTime->set_qualifyradiusrule(-1);
							if (s == "0")
							{
								logTime->set_qualifyradiusrule(0);
							}
							else if (s == "1")
							{
								logTime->set_qualifyradiusrule(1);
							}
							//}
							list->mutable_list()->AddAllocated(logTime);
							
					}

				}
				catch (std::exception &ex)
				{
					
				}
				// do not forget to clean-up
				p->FreeQuery();
				CloseConn();

				return list;
			}

			void TimeLogDB::SetParams(SQLiteStatement *pStmt, TimeLogRow *timLogRow, bool sent )
			{

				pStmt->BindInt(1, (int)timLogRow->Event());  //@LogStatus=@p1
				string str = timLogRow->LogTime().ToString();
															 //DateTime dt=TimeUtil.roundMinute (timLogRow.LogTime);     // round to minute!!!!!!!
				pStmt->BindString(2, str); //dt   //"@LogTime" = @p2
				pStmt->BindInt(3, timLogRow->logbookstopid());   //"@lbsid" =@p3
				pStmt->BindInt(4, timLogRow->odo());
				pStmt->BindString(5, timLogRow->driverid());   //"@DriverId" - @p5
				bool haveSent = sent == false ? timLogRow->GetHaveSend() : true;
				pStmt->BindBool(6, haveSent);
				pStmt->BindBool(7, timLogRow->GetisFromServer());
				pStmt->BindDouble(8, timLogRow->lat());
				pStmt->BindDouble(9, timLogRow->lon());
				pStmt->BindInt(10, timLogRow->boxid());
				pStmt->BindString(11, timLogRow->eqno());
				DateTime odts = TimeUtil::SkipMs(DateTime(timLogRow->olt()));
				pStmt->BindString(12, odts.ToString());
				pStmt->BindInt(13, timLogRow->editor());
				pStmt->BindString(14, timLogRow->comment());
				pStmt->BindBool(15, timLogRow->signed_());
				pStmt->BindInt(16, (int)timLogRow->type());
				//if (string.IsNullOrEmpty (timLogRow.CoDriver))
				//	timLogRow.CoDriver = "";
				pStmt->BindString(17, timLogRow->codriver());

				pStmt->BindDouble(18, timLogRow->tz());
				pStmt->BindDouble(19, timLogRow->dst());
				
				pStmt->BindString(20, timLogRow->addr());
				pStmt->BindInt(21, timLogRow->argeo());
				pStmt->BindString(22, Int2Str(timLogRow->qualifyradiusrule()));

			}


			int TimeLogDB::SaveTimeLog(TimeLogRow *timLogRow)
			{
				int rv = 1;
				SQLiteStatement *pStmt = CreateConnection();
			
				try {
					if (timLogRow->tlid() < 0)
					{

						std::string sql = "INSERT INTO TIMELOG (LogStatus, LogTime, Logbookstopid, Odometer, DriverId, HaveSent, IsFromServer, Latitude, Longitude, BoxID, EquipmentID,OrigLogTime,Editor,Comment,Signed,Type,Codriver,TimeZone,DaylightSaving,Address,arGeo,QualifyRadiusRule) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
						pStmt->Sql(sql);
						// bind all three values
						SetParams(pStmt, timLogRow);
						// execute the statement and reset the bindings
						pStmt->ExecuteAndFree();
						sql = " SELECT last_insert_rowid();";
						pStmt->Sql(sql);
						int id = -1;
						while (pStmt->FetchRow())
						{
							id = pStmt->GetColumnInt(0);
						}
						if (id > 0)
						{
							timLogRow->set_tlid(id);
						}
					}
					else
					{ //???
						string sql = "UPDATE TIMELOG SET LogStatus=?,LogTime=?,Logbookstopid=?,Odometer=?,DriverId=?,HaveSent=?,IsFromServer=?,Latitude=?, Longitude=?,BoxID=?,EquipmentID=?,OrigLogTime=?,Editor=?,Comment=?,Signed=?,Type=?,Codriver=?,TimeZone=?,DaylightSaving=?,Address=?,arGeo=?,QualifyRadiusRule=?  WHERE Id = ?";
						pStmt->Sql(sql);
						// bind all three values
						SetParams(pStmt, timLogRow);
						pStmt->BindInt(23, timLogRow->tlid());
						// execute the statement and reset the bindings
						pStmt->Execute();
						
					}
				}

				catch (std::exception &ex)
				{
					rv = -1;
				}
				pStmt->FreeQuery();
				CloseConn();
				return rv;
			}



			PTimeLogRowList* TimeLogDB::GetAllUnSent()
			{
				std::string sql = "SELECT * FROM TIMELOG WHERE HaveSent = 0 ORDER BY LogTime";
				//cmd.Parameters.AddWithValue ("@HaveSent", true);
				PTimeLogRowList* list = getTTList(sql,0,true);
				return list;
			}

			void TimeLogDB::SetHaveSentDeleteOlder()
			{
				SQLiteStatement *pStmt = CreateConnection();
				pStmt->BeginTransaction();
					pStmt->Transaction("DELETE FROM TIMELOG WHERE LogTime < datetime('now', '-30 day');");
					//pStmt->Transaction("DELETE FROM TIMELOG WHERE LogStatus=200");
					pStmt->Transaction("UPDATE TIMELOG SET HaveSent=1");
				pStmt->CommitTransaction();
			
				/*string	sql = "UPDATE TIMELOG SET HaveSent=?";
				pStmt->Sql(sql);
				pStmt->BindBool(1,true);
				pStmt->ExecuteAndFree();*/
				
				CloseConn();
			}

			PTimeLogRowList* TimeLogDB::GetAllUnSent4Vehicle()
			{
				std::string sql = "SELECT * FROM TIMELOG WHERE LogStatus=202 ORDER BY LogTime";
				return getTTList(sql,202,true);
			}
			
			bool TimeLogDB::UpdateSignedStatus(vector<TimeLogRow *> tlr_list)
			{
				bool rv = true;
				SQLiteStatement *pStmt = CreateConnection();
				int count = 0;
				try
				{
					for (TimeLogRow *tlr : tlr_list)
					{
					//cmd->Parameters->Clear();

						string sql = "UPDATE TIMELOG SET Signed = ?,HaveSent=0  where Id=?";
						pStmt->Sql(sql);
						pStmt->BindBool(1, tlr->signed_()); 
						pStmt->BindInt(2, tlr->tlid());
						pStmt->ExecuteAndFree();
					}
				}
				catch (std::exception &e1)
				{
					rv = false;
				}

				CloseConn();
				
				return rv;
			}


			bool TimeLogDB::UpdateDriverStatus(PTimeLogRow *tlr)
			{
				bool rv = true;
				SQLiteStatement *pStmt = CreateConnection();
				int count = 0;
				try
				{
					string sql = "UPDATE TIMELOG SET LogStatus = ?,Signed = ?,HaveSent=?  WHERE OrigLogTime = ?";
					pStmt->Sql(sql);
					pStmt->BindInt(1, (int)tlr->event());
					pStmt->BindBool(2, true);
					pStmt->BindBool(3, false);
					pStmt->BindString(4, TimeUtil::SkipMs(tlr->olt()).ToString());
					pStmt->ExecuteAndFree();
					
				}
				catch (std::exception &ex)
				{
					//Log->Debug("TLD", std::string("UpdateDriverStatus Exc:") + ex.what());
					rv = false;
				}

				CloseConn();
				//Log->Debug("TLD", std::string("UpdateDriverStatus count:") + std::to_string(count));
				return rv && count > 0;
			}

			void TimeLogDB::UpdateAddressInTimeLog(DateTime dt, const std::string &address)
			{
				int count = 0;
				SQLiteStatement *pStmt = CreateConnection();
				try
				{
						string sql = "UPDATE TIMELOG SET Address =?, arGeo =?  WHERE OrigLogTime =?";
						pStmt->Sql(sql);
						DateTime odts = TimeUtil::SkipMs(dt);
						pStmt->BindString(1, address);
						pStmt->BindInt(2, 1);
						pStmt->BindString(3, odts.ToString());
						pStmt->Execute();
						count += GetNuberOfRowsUpdated();
						pStmt->FreeQuery();
				}
				catch (std::exception &ex)
				{
					//Log->Debug("TLD", std::string("SaveTimeLog Exc:") + ex.what());
				}

				CloseConn();

			}
			//-------------------additional--------------------------
			TimeLogRow *TimeLogDB::getLast(string sql)
			{
				TimeLogRow *tlr = NULL;
				PTimeLogRowList* list = getTTList(sql, 0, true);
				if (list->list_size() > 0)
				{
					tlr = new TimeLogRow();
					PTimeLogRow *tlr_orig = (PTimeLogRow *)list->mutable_list(0);
					tlr->CopyFrom(*tlr_orig);

				}
				delete list;
				return tlr;
			}
			TimeLogRow *TimeLogDB::GetLast(string driverId/*, LOGSTATUS logStatus*/)
			{
				string sql = string_format(" SELECT * FROM TIMELOG WHERE DriverId = '%s'  ORDER BY LogTime DESC LIMIT 1; ", driverId.c_str());
				return getLast(sql);
			}
			PTimeLogRow*    TimeLogDB::GetLastBeforeDate(const std::string &did, int64_t ts)
			{
				if (did != "" && !did.find("-1") != std::string::npos)
				{
					string date = DateTime::DateStringFromMillis(ts);
					string sql = string_format("SELECT * FROM TIMELOG WHERE DriverId = '%s' AND strftime('%%Y-%%m-%%d', LogTime) < '%s' ORDER BY LogTime DESC LIMIT 1;", did.c_str(), date.c_str());
					return getLast(sql);
				}
				return 0;
			}
			PTimeLogRowList* TimeLogDB::GetAllTimeLogs4Date(const std::string &did,int64_t ts)
			{
				//added Type<>3 means all which are not modified
				std::string sql = "SELECT * FROM TIMELOG ORDER BY LogTime";
				if (did != "" && !did.find("-1") != std::string::npos)
				{
					int sts = 200;
					int type = (int)TimeLogType::Modified;
					string date = DateTime::DateStringFromMillis(ts);
					//sql = string_format("SELECT * FROM TIMELOG WHERE DriverId = '%s' AND strftime('%%Y-%%m-%%d', LogTime) = '%s' AND LogStatus < %d AND Type <> %d ORDER BY LogTime;", did.c_str(),date.c_str(),sts, type);
					sql = string_format("SELECT * FROM TIMELOG WHERE DriverId = '%s' AND strftime('%%Y-%%m-%%d', LogTime) = '%s' ORDER BY LogTime;", did.c_str(), date.c_str());
				}

				//var sql = "SELECT * FROM TIMELOG";
				return getTTList(sql, 0, false);
			}
			void TimeLogDB::DeleteLastRecord()
			{
				SQLiteStatement *p = CreateConnection();
				string sql = "DELETE FROM TIMELOG WHERE id = (SELECT MAX(id) FROM TIMELOG);";
				p->SqlStatement(sql);
				CloseConn();
			}
#pragma endregion
#pragma region RecapDB
			void TimeLogDB::CreateRecapTable(SQLiteStatement *pStmt)
			{
				// Create a db table to store recap
				string sql = std::string("CREATE TABLE Recap (Id INTEGER PRIMARY KEY AUTOINCREMENT, DriverId ntext, ") + 
					std::string("date datetime, today integer, ") + 
					std::string("cycleTotal integer, cycleAvailable integer,cycle ntext, HaveSent BOOLEAN); ");
				CreateTableA(pStmt, sql);
			}

			void TimeLogDB::Recap_SetHaveSentDeleteOlder()
			{
				SQLiteStatement *pStmt = CreateConnection();
				pStmt->SqlStatement( "DELETE FROM Recap WHERE date < datetime('now', '-31 day');");
				CloseConn();
			}

			PRecapRowList* TimeLogDB::getAllRecap(const std::string &driverId, const std::string &sql)
			{
				PRecapRowList* recaps=new PRecapRowList();
				if (driverId.empty())
				{
					return recaps;
				}
				try
				{
					SQLiteStatement *p = CreateConnection();
					p->Sql(sql);
								
							while (p->FetchRow())
							{
								PRecapRow *recap = new PRecapRow();
								string str= p->GetColumnString(2);
								recap->set_date(DateTime::parse(str).getMillis());
								recap->set_today(p->GetColumnInt(3));
								recap->set_cycletotal(p->GetColumnInt(4));
								recap->set_cycleavailable(p->GetColumnInt(5));
								recap->set_cycle(p->GetColumnString(6));
								recap->set_sent(p->GetColumnBool(7));
								recaps->mutable_list()->AddAllocated(recap);
							}
							p->FreeQuery();

				}
				catch (std::exception &ex)
				{
				}
				CloseConn();
				return recaps;
			}

			PRecapRowList* TimeLogDB::GetAllRecapByDriver(const std::string &driverId)
			{

				//string sql = string_format("Select * From Recap WHERE DriverId = '%s' order by date desc limit 14 ", driverId.c_str());
				string sql = string_format("Select * From Recap WHERE DriverId = '%s' order by date", driverId.c_str());
				return getAllRecap(driverId, sql);
			}

			PRecapRowList* TimeLogDB::GetAllRecapByDriverForCurrentDay(const std::string &driverId)
			{
				string sql = string_format("Select * From Recap WHERE DriverId = '%s' and date >= date('now','-1 day') order by date; ", driverId.c_str());
				return getAllRecap(driverId, sql);
			}

			void TimeLogDB::SetParams(SQLiteStatement *p, PRecapRow *rr, const std::string &driverId)
			{
				p->BindInt(1, rr->today());
				p->BindInt(2, rr->cycletotal());
				p->BindInt(3, rr->cycleavailable());
				p->BindString(4, rr->cycle());
				p->BindBool(5, false);
				p->BindString(6, DateTime::ToStringFromMillis(rr->date()));
				p->BindString(7, driverId);                  // driveId is the last !!!
			}

			void TimeLogDB::AddRecap(PRecapRow *rr, const std::string driverId)
			{
				std::string sql = std::string(" INSERT INTO Recap (today,cycleTotal,cycleAvailable,cycle,HaveSent,date,DriverId)  VALUES (?,?,?,?,?,?,?)");
				_doRecap(rr, driverId, sql);
			}

			void TimeLogDB::_doRecap(PRecapRow *rr, const std::string &driverId, const std::string &sql)
			{
				if (driverId.empty())
				{
					return;
				}
				try
				{
					SQLiteStatement *p = CreateConnection();
					p->Sql(sql);
					SetParams(p, rr, driverId);
					p->ExecuteAndFree();
				}
				catch (std::exception &ex)
				{
					//Log->Error("RRP", std::string("_doRecap Exc:") + ex.what());
				}
				CloseConn();
			}

			void TimeLogDB::UpdateRecap(PRecapRow *rr, const std::string driverId)
			{
				std::string sql = std::string(" Update Recap set today=?,cycleTotal=?,cycleAvailable=?,cycle=?,HaveSent=?  where date=? and DriverId=?");
				_doRecap(rr, driverId, sql);
			}
			void TimeLogDB::CleanUpRecapTable()
			{
				SQLiteStatement *p = CreateConnection();
				p->SqlStatement("delete from Recap");
				CloseConn();
			}
#pragma endregion

#pragma region class DeferHoursRepository
			void TimeLogDB::CreateDeferHoursTable(SQLiteStatement *p)
			{
				auto sql = "CREATE TABLE DeferHours (Id INTEGER PRIMARY KEY AUTOINCREMENT, DriverId ntext, day1 datetime, day2 datetime, deferMinutes integer, HaveSent BOOLEAN); ";
				CreateTableA(p, sql);
			}

			void TimeLogDB::DeleteDeferHour(DeferHourRow *dhr, const std::string &driverId)
			{
				if (driverId != "")
				{
					string sql = string_format("Delete From DeferHours WHERE DriverId = '%s' AND strftime('%Y-%m-%d %H:%M:%S', day1) = '%s' and  strftime('%Y-%m-%d %H:%M:%S', day2) = '%s'; ", driverId.c_str(), dhr->day1.to_string().c_str(), dhr->day2.to_string().c_str());
					SQLiteStatement *p = CreateConnection();
					p->SqlStatement(sql);
						CloseConn();
				}
			}

			DeferHourRow *TimeLogDB::FindDeferHourByDate(DateTime date, const std::string &driverId)
			{
				if (driverId.empty())
				{
					return nullptr;
				}
				DeferHourRow *deferhour = nullptr;
				SQLiteStatement *p = CreateConnection();
				try
				{
					string s = date.ToString();
					string st1 = "strftime('%Y-%m-%d %H:%M:%S', day1)";
					string st2 = "strftime('%Y-%m-%d %H:%M:%S', day2)";
					//string sql = string_format("Select * From DeferHours WHERE DriverId = '%s' AND (strftime('%Y-%m-%d %H:%M:%S', day1) = '%s' or  strftime('%Y-%m-%d %H:%M:%S', day2) = '%s') order by day2 desc limit 1; ", driverId.c_str(), str.c_str());
					/*string sql = "Select * From DeferHours WHERE DriverId = '" + driverId +"' AND (";
					sql = sql + st+", day1)="+s;*/
					string sql= string_format("Select * From DeferHours WHERE DriverId = '%s' AND (%s = '%s' or  %s = '%s') order by day2 desc limit 1; ", driverId.c_str(),st1.c_str(), s.c_str(),st2.c_str(),s.c_str());
					p->Sql(sql);

					while (p->FetchRow())
					{
						deferhour = new DeferHourRow();
						string str = p->GetColumnString(2);
						DateTime dt1 = DateTime::parse(str);

						string str1 = p->GetColumnString(3);
						DateTime dt2 = DateTime::parse(str);
						int v= p->GetColumnInt(4);

						deferhour->day1 = dt1;
						deferhour->day2 = dt2;
						deferhour->defers = TimeSpan(0, v, 0);
						deferhour->haveSend= p->GetColumnBool(5);
					}
					p->FreeQuery();
				}
				catch (std::exception &ex)
				{
					//LOGI(std::string("FindDeferHourByDate EXC: ") + ex.what());//Log->Debug("DHR", std::string("FindDeferHourByDate EXC: ") + ex.what());
				}
				CloseConn();
				return deferhour;
			}

			void TimeLogDB::DeferHours_SetHaveSentDeleteOlder()
			{
				SQLiteStatement *p = CreateConnection();
				p->SqlStatement("DELETE FROM DeferHours WHERE day1 < datetime('now', '-31 day');");
				CloseConn();
			}

			void TimeLogDB::AddDeferHour(DeferHourRow *dhr, const std::string &driverId)
			{
				if (driverId.empty())
				{
					return;
				}
				SQLiteStatement *p = CreateConnection();
				try
				{
					string sql = std::string(" INSERT INTO DeferHours (DriverId, day1, day2, deferMinutes,HaveSent)  VALUES (?,?,?,?,?);");
					p->Sql(sql);
						p->BindString(1, driverId);
						p->BindString(2, dhr->day1.ToString());
						p->BindString(3, dhr->day2.ToString());
						p->BindInt(4, int(dhr->defers.TotalMinutes()));
						p->BindBool(5, false);
						p->ExecuteAndFree();
				}
				catch (std::exception &ex)
				{
					//Log->Error("DHR", std::string("AddDeferHour Exc:") + ex.what());
				}
				CloseConn();

			}
#pragma endregion

#pragma region RuleSelectionHistory

			void TimeLogDB::CreateHOSRulesTable(SQLiteStatement *pStmt)
			{
				// Create a db table to store defer hour
				string sql = std::string("CREATE TABLE RuleSelectionHistory (Id INTEGER PRIMARY KEY AUTOINCREMENT, ") + 
					std::string("DriverId ntext, ruleid integer,ruleExc integer,  selectTime datetime, country ntext, HaveSent BOOLEAN); ");
				CreateTableA(pStmt, sql);

			}

			void TimeLogDB::setParams(SQLiteStatement *p, PRuleSelectionHistoryRow *rsr, const std::string &driverId, bool haveSent)
			{
				int cycle = int(rsr->ruleid());
				//Log.Debug ("RSH", "AddRSH DBPar:" + cycle.ToString());
				p->BindString(1, driverId);
				p->BindInt(2, cycle);
				p->BindInt(3, int(rsr->ruleexception()));
				p->BindString(4, DateTime::ToStringFromMillis(rsr->selecttime()));
				p->BindString(5, rsr->country());
				p->BindBool(6, haveSent);

			}

			void TimeLogDB::AddRuleSelectionHistory(PRuleSelectionHistoryRow *rsr, const std::string &driverId)
			{
				if (driverId.empty())
				{
					return;
				}
				int rv = 0;
				SQLiteStatement *p = CreateConnection();
				try
				{
					
						string sql = std::string(" INSERT INTO RuleSelectionHistory (DriverId, ruleid,ruleExc, selectTime, country, HaveSent) ") 
							+ std::string(" VALUES (?, ?,?,?,?,?)");
						p->Sql(sql);
						setParams(p, rsr, driverId,false);
						p->ExecuteAndFree();
				}
				catch (std::exception &ex)
				{
					//Log->Debug("RSH", std::string("AddRuleSelectionHistory Exc:") + ex.what());
				}
				CloseConn();
				//Log->Debug("RSH", std::string("AddRuleSelectionHistory Added:") + std::to_string(rv));
			}

			void TimeLogDB::UpdateRuleSelectionHistoryFromServer(std::vector<PRuleSelectionHistoryRow*> &rlist, const std::string &driverId)
			{
				if (driverId.empty())
				{
					return;
				}
				SQLiteStatement *p = CreateConnection();
				try
				{
						for (auto rule : rlist)
						{
							PRuleSelectionHistoryRow *rsh = GetLastBeforeSelectTime(p, driverId, rule->selecttime());
							if (rsh == nullptr || rsh->ruleid() != rule->ruleid() || rsh->ruleexception() != rule->ruleexception())
							{
								std::string sql = std::string(" INSERT INTO RuleSelectionHistory (DriverId, ruleid,ruleExc, selectTime, country, HaveSent) ")
									+ std::string(" VALUES (?,?,?,?,?,?);");
								p->Sql(sql);
								setParams(p, rule, driverId, true);
								p->ExecuteAndFree();
							}
						}
				}
				catch (std::exception &ex)
				{
					//Log->Debug("RSH", std::string("UpdateRuleSelectionHistoryFromServer Exc:") + ex.what());
				}
				CloseConn();
			}

			PRuleSelectionHistoryRow *TimeLogDB::GetLastBeforeSelectTime(SQLiteStatement *p, const std::string &driverId, DateTime selectTime)
			{
				PRuleSelectionHistoryRow *rsr = nullptr;
				string sql = string_format("Select * From RuleSelectionHistory WHERE DriverId = '%s' AND selectTime <= ? order by selectTime DESC LIMIT 1; ", driverId.c_str());
				p->Sql(sql);
				p->BindString(1, selectTime.ToString());
				p->Execute();
				while (p->FetchRow())
				{
						rsr = readRuleRow(p);
						break;
				}
				p->FreeQuery();
				return rsr;
			}

			PRuleSelectionHistoryRow *TimeLogDB::readRuleRow(SQLiteStatement *p)
			{
				PRuleSelectionHistoryRow *rsr = new PRuleSelectionHistoryRow();
				int cycle = p->GetColumnInt(2);
				rsr->set_ruleid( (int) (HOSCYCLE)(cycle));
				rsr->set_ruleexception((int) (RuleExceptions)(p->GetColumnInt(3)));
				rsr->set_selecttime( DateTime::parse(p->GetColumnString(4)).getMillis());
				rsr->set_country( p->GetColumnString(5));
				rsr->set_sent( p->GetColumnBool(6));
				return rsr;
			}

			bool TimeLogDB::_getRuleSelectionHistory(const std::string &sql, PRuleSelectionHistoryRowList* rlist)
			{
				bool rv = true;
				SQLiteStatement *p = CreateConnection();
				try
				{
					p->Sql(sql);
					while (p->FetchRow())
							{
								PRuleSelectionHistoryRow *rsr = readRuleRow(p);
								rlist->mutable_list()->AddAllocated(rsr);
					}
				}
				catch (std::exception &ex)
				{
					rv = false;//Log->Debug("RSH", std::string("_getRuleSelectionHistory Exc:") + ex.what());
				}
				CloseConn();
				return rv;
			}
			PRuleSelectionHistoryRow* TimeLogDB::GetLastRuleByDriverCountry(string driverId, string country)
			{
				PRuleSelectionHistoryRow *rsr = 0;
				SQLiteStatement *p = CreateConnection();
				string sql = string_format("Select * From RuleSelectionHistory WHERE DriverId = '%s' AND country = '%s' order by selectTime DESC LIMIT 1; ",  driverId.c_str(),country.c_str());
				try
				{
					p->Sql(sql);
					while (p->FetchRow())
					{
						rsr = readRuleRow(p);
						break;
					}
				}
				catch (std::exception &ex)
				{
					//Log->Debug("RSH", std::string("_getRuleSelectionHistory Exc:") + ex.what());
				}
				CloseConn();
				return rsr;
			}

			PRuleSelectionHistoryRowList* TimeLogDB::GetRuleSelectionHistory(const std::string &driverId)
			{
				PRuleSelectionHistoryRowList* rlist=new PRuleSelectionHistoryRowList();
				if (driverId.empty())
				{
					return rlist;
				}
				string sql = string_format("Select * From RuleSelectionHistory WHERE DriverId = '%s' order by selectTime", driverId.c_str());
				 _getRuleSelectionHistory(sql,rlist);
				 return rlist;
			}

			PRuleSelectionHistoryRowList* TimeLogDB::GetUnsentRuleSelectionHistory(const std::string &driverId)
			{
				PRuleSelectionHistoryRowList* rlist =new PRuleSelectionHistoryRowList();
				if (driverId.empty())
				{
					return rlist;
				}
				string sql = string_format("Select * From RuleSelectionHistory WHERE DriverId = '%s' and HaveSent=0 order by selectTime", driverId.c_str());
				 _getRuleSelectionHistory(sql, rlist);
				 return rlist;
			}

			void TimeLogDB::RulesHos_SetHaveSentDeleteOlder(const std::string &drId)
			{
				SQLiteStatement *p = CreateConnection();
				try
				{
						string sql = "DELETE FROM RuleSelectionHistory WHERE selectTime < datetime('now', '-30 day');";
						p->SqlStatement(sql);
						
						sql = "UPDATE RuleSelectionHistory SET HaveSent = ? WHERE DriverId = ?";
						
						p->BindBool(1, true);
						p->BindString(2, drId);
						p->ExecuteAndFree();
					
				}
				catch (std::exception &ex)
				{
					//Log->Debug("RSH", std::string("RulesHos_SetHaveSentDeleteOlder Exc:") + ex.what());
				}
				CloseConn();
			}

#pragma endregion

#pragma region AlertDB
			void TimeLogDB::CreateAlertsTable(SQLiteStatement *pStmt)
			{
				// Create a db table to store alerts
				auto sql = std::string("CREATE TABLE HOSAlerts (Id INTEGER PRIMARY KEY AUTOINCREMENT, DriverId ntext, ") + std::string("AlertDate datetime, DrivingAvail integer,OnDutyAvail integer, ") + std::string("AlertType integer, OnDutyRuleViolated ntext,DrivingRuleViolated ntext, HaveSent BOOLEAN); ");
				CreateTableA(pStmt, sql);
			}

			void TimeLogDB::Alerts_SetHaveSentDeleteOlder()
			{
				SQLiteStatement *p = CreateConnection();
				p->SqlStatement("DELETE FROM HOSAlerts WHERE AlertDate < datetime('now', '-31 day');");
				p->SqlStatement("UPDATE HOSAlerts SET HaveSent = 0 ");
				CloseConn();
			}

			bool TimeLogDB::getAllAlerts(std::vector<HosAlertRow*> &alerts,  SQLiteStatement *p)
			{
				bool rv = true;
				try
				{
							while (p->FetchRow())
							{
								HosAlertRow *al = new HosAlertRow();
								al->dt = DateTime::parse(p->GetColumnString(2));
								al->drivingAvailable = p->GetColumnInt(3);
								al->ondutyAvailable = p->GetColumnInt(4);
								al->alertType = (AlertTypes)(p->GetColumnInt(5));
								al->ondutyruleviolated = p->GetColumnString(6);
								al->drivingruleviolated = p->GetColumnString(7);
								al->haveSent = p->GetColumnBool(8);
								alerts.push_back(al);
							}
					
				}
				catch (std::exception &ex)
				{
					rv = false;
				}
				
				return rv;
			}

			std::vector<HosAlertRow*> TimeLogDB::GetAllAlertsByDriver(const std::string &driverId)
			{
				std::vector<HosAlertRow*> alerts;
				if (driverId.empty())
					return alerts;
				
				SQLiteStatement *p = CreateConnection();
				auto sql = string_format("Select * From HOSAlerts WHERE DriverId = '%s' and HaveSent = 0 order by AlertDate ", driverId.c_str());
				p->Execute();
				getAllAlerts(alerts,p);
				p->FreeQuery();
				return alerts;
			}

			bool TimeLogDB::isAlertExist(DateTime dt_shift, int al_type, const std::string &dr_rule, const std::string &onduty_rule, const std::string &driverId)
			{
				std::vector<HosAlertRow*> list;
				if (driverId.empty())
					return false;
				SQLiteStatement *p = CreateConnection();
				string sql = "Select * From HOSAlerts WHERE (DriverId=? and AlertType=? and DrivingRuleViolated=? and OnDutyRuleViolated=?) and (AlertDate>=? and AlertDate<=?) order by AlertDate ";
				p->Sql(sql);
				
				p->BindString(1, driverId);
				p->BindInt(2, al_type);
				p->BindString(3, dr_rule);
				p->BindString(4, onduty_rule);
				p->BindString(5, dt_shift.ToString());
				p->BindString(6, DateTime::UtcNow().ToString());
				p->Execute();
				bool rv = getAllAlerts(list,p);
				p->FreeQuery();
				if (rv&&list.size() > 0 && list.size() > 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			void TimeLogDB::AddAlert(HosAlertRow *rr, const std::string &driverId)
			{
				if (driverId != "")
				{
					SQLiteStatement *p = CreateConnection();
					string sql = std::string(" INSERT INTO HOSAlerts (DriverId, AlertDate, DrivingAvail, OnDutyAvail, AlertType, OnDutyRuleViolated,DrivingRuleViolated,HaveSent) ") 
						+ std::string(" VALUES (?, ?,?,?,?,?,?,?);");
					p->Sql(sql);
							p->BindString(1, driverId);
							p->BindString(2, rr->dt.ToString());

							p->BindInt(3, rr->drivingAvailable);
							p->BindInt(4, rr->ondutyAvailable);
							p->BindInt(5, rr->drivingAvailable);
							p->BindString(6, rr->ondutyruleviolated);
							p->BindString(7, rr->drivingruleviolated);
							p->BindBool(8, false);
							p->ExecuteAndFree();
							
							CloseConn();
				}
				
			}
#pragma endregion
