#include "InspectionDB.h"
#include "Dbg.h"

InspectionDB *InspectionDB::_instance = nullptr;

InspectionDB *InspectionDB::Get()
{
	if (_instance == nullptr)
	{
		_instance = new InspectionDB();
	}
	return _instance;
}
void InspectionDB::CreateTables()
{
	SQLiteStatement *conn = CreateConnection();
	try
	{
		if (!IsTableExist(conn, "CATEGORY"))
		{
			CreateCategoryTable(conn);
		}
		if (!IsTableExist(conn, "CATEGORY_VEHICLE"))
		{
			CreateCategoryVehicleAssignmentTable(conn);
		}
		
		if (!IsTableExist(conn, "INSPECTION_ITEM"))
		{
			CreateInspectionItemTable(conn);
		}
		
		if (!IsTableExist(conn, "INSPECTION_REPORT"))
		{
			CreateInspectionReportTable(conn);
		}
	}
	catch (SQLiteException &e)
	{
		string str = e.GetString();
		LOGE(str.c_str());
	}

	CloseConn();
}
void InspectionDB::CreateCategoryTable(SQLiteStatement *pStmt)
{
	// Create a db table to store categories
	string sql = "CREATE TABLE CATEGORY (CategoryId INTEGER, Description ntext,attID ntext,GroupID integer,BarCodeID integer);";

	pStmt->SqlStatement(sql);

	// Insert all default categories
	auto commands = std::vector<std::string>{ "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (1, 'Air Brake System','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (2, 'Cab','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (3, 'Cargo Securement','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (4, 'Coupling Devices','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (5, 'Dangerous Goods/Hazardous Materials','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (6, 'Driver controls','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (7, 'Driver Seat','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (8, 'Electric Brake System','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (9, 'Emergency Equipment and Safety Devices','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (10, 'Exhaust System','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (11, 'Frame and Cargo Body','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (12, 'Fuel System','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (13, 'General','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (14, 'Glass and Mirrors','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (15, 'Heater/Defroster','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (16, 'Horn','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (17, 'Hydraulic Brake System','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (18, 'Lamps and Reflectors','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (19, 'Steering','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (20, 'Suspension System','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (21, 'Tires','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (22, 'Wheels, Hubs and Fasteners','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (23, 'Windshield Wiper/Washer','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (24, 'Refrigerated Trailer','',0,-1)", "INSERT INTO CATEGORY (CategoryId, Description,attID,GroupID,BarCodeID) VALUES (25, 'Trailer brake connections','',0,-1)" };

	for (string command : commands)
	{
		pStmt->SqlStatement(command);
	}

}

PInspectionCategoryRowList* InspectionDB::GetAllCategories(int boxID, bool att_query )
{
	PInspectionCategoryRowList *list=new PInspectionCategoryRowList();
	CategoryVehicleAssignment *vv = new CategoryVehicleAssignment();

	try
	{
		SQLiteStatement *p = CreateConnection();
	//1.
		string sql = string_format("SELECT GroupIDs, updateTS FROM CATEGORY_VEHICLE where BoxId=%d", boxID);
				p->Sql(sql);
				while (p->FetchRow())
				{
					vv->groupIDs = p->GetColumnString(0);
					vv->updateTS = p->GetColumnString(1);
					
					break;
				}
			std::vector<int> grIDS;
			if (vv->groupIDs != "")
			{
				//C# TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to this .NET String method:
				std::vector<std::string> s1 = split(vv->groupIDs,',');
				if (s1.size() > 0 && s1.size() > 0)
				{
					for (int i = 0; i < s1.size(); i++)
					{
						if (!(s1[i]).empty())
						{
							int x = Str2Int(s1[i]);
							if (x > 0)
							{
								grIDS.push_back(x);
							}
						}
					}
				}
			}
			else
			{
				if (!att_query)
				{
					grIDS.push_back(0); // when there is no attachment (BoxID for attachment) info
				}
				// DO NOT get default categories !!!
			}
			p->FreeQuery();

			for (int grID : grIDS)
			{
				sql = string_format("SELECT CategoryId, Description,attID,BarCodeID FROM CATEGORY where GroupID=%d", grID);
				p->Sql(sql);
				
					
					while (p->FetchRow())
					{
						short catID = p->GetColumnInt(0);
						std::string desc = p->GetColumnString(1);
						std::string barcodeLoc = p->GetColumnString(2);
						int barcodeid = p->GetColumnInt(3);
						PInspectionCategoryRow *icr = new PInspectionCategoryRow();
						icr->set_cid(catID);
						icr->set_desc(desc);
						icr->set_groupid(grID);
						icr->set_barcodeid(barcodeid);
						icr->set_bcloc(barcodeLoc);
						//, desc, grID, barcodeid, barcodeLoc
						icr->set_color(0);                        // color is a required field !!!!!
						list->mutable_list()->AddAllocated(icr);
					}
				p->FreeQuery();
				
			}
			for (int i = 0; i < list->list_size();i++)
			{
				PInspectionCategoryRow *icr = list->mutable_list(i);
				int cid = icr->cid();
				PInspectionItemList* iiL = GetInspectionItemsByCategoryId(cid, p);
				//PInspectionItemList iiL1=icr->items();
				if (iiL->list_size() > 0) {

					for (int i = 0; i < iiL->list_size();i++) {
						PInspectionItem *ii = iiL->mutable_list(i);
						if (ii->parentid() == 0)
							icr->mutable_items()->mutable_list()->AddAllocated(ii);

					}
					
					/*if (this.items.Count > 0) {
						int levels = 0;
						createAllLevels(this.items, all_list, ref levels);
					}*/
				}
			}
	}//try
	catch (std::exception &ex)
	{
		//Log->Error("CDB", std::string("READ ALL Categories Exc:") + ex.what() + std::string(" Stk:") + ex.StackTrace);
	}
	CloseConn();
	return list;
}

////, cat->Category->Replace("'", "''"), cat->BarCodeID, cat->CategoryID, cat->GroupID, cat->BarCodeLocation
void InspectionDB::SetCatParams(SQLiteStatement *pStmt, PInspectionCategoryRow *icr)
{
	std::string str = icr->desc();
	pStmt->BindString(1, str);					
	pStmt->BindInt(2, icr->barcodeid());
	pStmt->BindString(3, icr->bcloc());
	pStmt->BindInt(4, icr->cid());				
	pStmt->BindInt(5, icr->groupid());        
	       
	
}
void InspectionDB::SaveCategory(PInspectionCategoryRowList* list, bool delete_flag, std::vector<int> &grIDs)
{
	int tmp = -1;
	//just for test
	std::string s = "";
	for (auto v : grIDs)
	{
		s = s + Int2Str(v) + std::string(",");
	}
	try
	{
		SQLiteStatement *p = CreateConnection();
			if (delete_flag)
			{
				if (grIDs.size() > 0)
				{
					for (auto groupId : grIDs)
					{
						string sql = string_format("DELETE FROM CATEGORY WHERE GroupID=%d", groupId);
						p->SqlStatement(sql);
					}
				}
			}
			tmp = 0;
			int size = list->list_size();
			for (int i = 0; i < size;i++)
			{
				PInspectionCategoryRow *icr=list->mutable_list(i);
				string sql ="UPDATE CATEGORY SET Description=?,BarCodeID=?, attID=? WHERE CategoryId = ? and GroupID=? ";
				p->Sql(sql);
				SetCatParams(p, icr);
				p->Execute();
				int tmp = GetNuberOfRowsUpdated();
				p->FreeQuery();
				if (tmp == 0)
				{
					sql = "INSERT INTO CATEGORY (Description,BarCodeID,attID,CategoryId, GroupID) VALUES (?, ?,?,?,?)";
					p->Sql(sql);
					SetCatParams(p, icr);

					p->Execute();
					tmp = GetNuberOfRowsUpdated();
					p->FreeQuery();
				}
				
			}
		
	}
	catch (std::exception &ex)
	{
		
	}
	CloseConn();
}
void InspectionDB::CreateCategoryVehicleAssignmentTable(SQLiteStatement *p)
{
	auto sql = "CREATE TABLE CATEGORY_VEHICLE (BoxId INTEGER , GroupIDs ntext,  updateTS datetime); ";
	CreateTableA(p, sql);
}
void InspectionDB::SaveCategory2VehicleAssignment(int boxId, const std::string &groupIDs)
{
	
	int tmp = -1;
	try
	{
		SQLiteStatement *p = CreateConnection();
				
		string sql = "UPDATE CATEGORY_VEHICLE SET GroupIDs = ?,updateTS=? WHERE BoxId = ?";
		p->Sql(sql);
			tmp = 0;
			p->BindString(1, groupIDs);
			p->BindString(2, DateTime::UtcNow().ToString());
			p->BindInt(3,boxId );
			p->Execute();
			tmp = GetNuberOfRowsUpdated();
			p->FreeQuery();
			if (tmp == 0)
			{
				// Do an insert
				sql="INSERT INTO CATEGORY_VEHICLE (GroupIDs,updateTS,BoxId) VALUES (?,?,?)";
				p->Sql(sql);
				p->BindString(1, groupIDs);
				p->BindString(2, DateTime::UtcNow().ToString());
				p->BindInt(3, boxId);
				p->Execute();
				tmp = GetNuberOfRowsUpdated();
				p->FreeQuery();

			}
	}
	catch (std::exception &ex)
	{}
	CloseConn();
}
CategoryVehicleAssignment *InspectionDB::GetVehicleCategories(int boxID)
{
	CategoryVehicleAssignment *vv = new CategoryVehicleAssignment();
	auto sql = string_format("SELECT GroupIDs, updateTS FROM CATEGORY_VEHICLE where BoxId=%d", boxID);
	
	try
	{
		SQLiteStatement *p = CreateConnection();
		 p->Sql(sql);
				while (p->FetchRow())
				{
					vv->groupIDs = p->GetColumnString(0);
					vv->updateTS = p->GetColumnString(1);
					break;
				}
		p->FreeQuery();
	}
	catch (std::exception &ex)
	{
	}
	CloseConn();
	
	return vv;
}
//--------------------------------Inspection Items-----------------------------------------------

void InspectionDB::CreateInspectionItemTable(SQLiteStatement *pStmt)
{
	// Create a db table to store inspection items
	string sql = "CREATE TABLE INSPECTION_ITEM (InspectionItemId INTEGER, CategoryID INTEGER, Defect ntext, DefectAbbr ntext, DefectLevel INTEGER,parentId INTEGER,hasChildren integer,attID ntext);";
	pStmt->SqlStatement(sql);

	// Insert all default categories
	auto commands = std::vector<std::string>{ "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (1, 1, 'Audible air leak', 'Audible air leak', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (2, 1, 'Slow air pressure build-up rate', 'Slow build-up rate', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (3, 1, 'Pushrod stroke of any brake exceeds the adjustment limit', 'Excess brake stroke', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (4, 1, 'Air loss rate exceeds prescribed limit', 'Excessive air loss', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (5, 1, 'Inoperative towing vehicle (tractor) protection system', 'Protection valve test failed', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (6, 1, 'Low air warning system fails or system is activated', 'Pressure warning failure', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (7, 1, 'Inoperative service, parking or emergency brake', 'Any brake failure', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (8, 2, 'Occupant compartment door fails to open', 'Door malfunction', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (9, 2, 'Any door fails to close securely', 'Latch malfunction', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (10, 3, 'Insecure of improper load covering', 'Ineffective load cover', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (11, 3, 'Insecure cargo', 'Insecure Cargo', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (12, 3, 'Absence, failure, malfunction or deterioration of required cargo securement device or load covering', 'Ineffective securement device', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (13, 4, 'Coupler or mounting has loose or missing fastener', 'Defective fastener', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (14, 4, 'Coupler is insecure of movement exceeds prescribed limit', 'Excessive free-play', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (15, 4, 'Coupling or locking mechanism is damaged or fails to lock', 'Lock failure/damaged', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (16, 4, 'Defective, incorrect or missing safety chain/cable', 'Ineffective chain/cable', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (17, 5, 'Dangerous goods/Hazardous Materials requirement not met', 'DG deficiency', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (18, 6, 'Accelerator pedal, clutch, gauges, audible and visual indicators or instruments fail to function properly', 'Instrument/Control malfunction', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (19, 7, 'Seat is damaged or fails to remain in set position', 'Damage/Adj. failure', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (20, 7, 'Seatbelt or tether belt is insecure, missing or malfunctions', 'Seatbelt/Tether defective', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (21, 8, 'Loose or insecure wiring or electrical connection', 'Deficient wiring or connections', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (22, 8, 'Inoperative breakaway device', 'Inoperative breakaway device', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (23, 8, 'Inoperative break', 'Inoperative break', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (24, 9, 'Emergency equipment is missing, damaged or defective', 'Deficent safty equipment', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (25, 10, 'Exhaust leak', 'Exhaust leak', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (26, 10, 'Leak that causes exhaust gas to enter the occupant compartment', 'Exhaust enters cabin', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (27, 11, 'Damaged frame or cargo body', 'Damaged frame/cargo body', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (28, 11, 'Visibly shifted, cracked, collapsing or sagging frame members', 'Failed frame member', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (29, 12, 'Missing fuel tank cap', 'Missing tank cap', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (30, 12, 'Insecure fuel tank', 'Insecure fuel tank', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (31, 12, 'Dripping fuel leak', 'Dripping fuel leak', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (32, 13, 'Serious damage or deterioration that is noticeable and may affect the safe operation of vehicle', 'Serious vehicle cond.', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (33, 14, 'Required mirror or window glass fails to provide the required view to the driver as a result of being cracked, broken, damaged, missing or maladjusted', 'Defective view', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (34, 14, 'Required mirror or glass has broken or damaged attachments onto vehicle body', 'Broken mount', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (35, 15, 'Control or system failure', 'Heater/defroster malfunction', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (36, 15, 'Defroster fails to provide unobstructed view through the windshield', 'Ineffective defroster', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (37, 16, 'Vehicle has no operative horn', 'Inoperative horn', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (38, 17, 'Brake fluid level is below indicated minimum level', 'Low fluid', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (39, 17, 'Parking brake is inoperative', 'Inoperative parking brake', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (40, 17, 'Brake boost or power assist is not operative', 'Inoperative power assist', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (41, 17, 'Brake fluid leak', 'Brake fluid leak', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (42, 17, 'Brake pedal fade or insufficient brake pedal reserve', 'Brake pedal fades', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (43, 17, 'Activated (other than ABS) warning device', 'Active brake warning', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (44, 17, 'Brake fluid reservoir is less than a quarter full', 'Missing brake fluid', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (45, 18, 'Required lamp does not function as intended', 'Lamp malfunction', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (46, 18, 'Required reflector is missing or partially missing', 'Missing reflector', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (47, 18, 'Failure of both low-beam headlamps', 'Headlamps failed', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (48, 18, 'Failure of both rearmost tail lamps', 'Tail lamps failed', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (49, 18, 'Failure of a rearmost turn-indicator lamp', 'Turn signal failed', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (50, 18, 'Failure of both rearmost brake lamps', 'Brake lamps failed', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (51, 19, 'Steering wheel lash (free-play) is greater than normal', 'Steering steering lash', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (52, 19, 'Steering wheel is insecure, or does not respond normally', 'Steering wheel malfunction', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (53, 19, 'Steering wheel lash (free-play) exceeds required limit', 'Steering lash exceeded', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (54, 20, 'Air leak in air suspension system', 'Air suspension leak', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (55, 20, 'Broken spring leaf', 'Broken spring', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (56, 20, 'Suspension fastener is loose, missing or broken', 'Defective anchor point', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (57, 20, 'Damaged, deflated air bag', 'Defective air bag', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (58, 20, 'Cracked or broken main spring leaf or more than one broken spring leaf', 'Spring failure', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (59, 20, 'Part of spring leaf or suspension is missing, shifted out of place or in ccontact with another vehicle component', 'Defective suspension mount', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (60, 20, 'Loose U-bolt', 'Loose U-bolt', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (61, 21, 'Damaged tread or sidewall of tire', 'Any tire damage', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (62, 21, 'Tire leaking', 'Tire leaking', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (63, 21, 'Flat tire', 'Flat tire', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (64, 21, 'Tire tread depth is less than wear limit', 'Visible tread wear bar', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (65, 21, 'Tire is in contact with another tire or any vehicle component other than mud-flap', 'Insufficient tire clearance', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (66, 21, 'Tire is marked ''Not for highway use''', 'Improper tire', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (67, 21, 'Tire has exposed cords in the tread or outer side wall area', 'Exposed cords', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (68, 22, 'Hub oil below minimum level (when fitted with sight glass)', 'Low hub oil', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (69, 22, 'Leaking wheel seal', 'Leaking wheel seal', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (70, 22, 'Wheel has loose, missing or ineffective fastener', 'Ineffective fastener', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (71, 22, 'Damaged, cracked or broken wheel, rim or attaching part', 'Defective rim/mount', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (72, 22, 'Evidence of imminent wheel, hub or bearing failure', 'Probable bearing failure', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (73, 23, 'Control or system malfunction', 'Wiper not operation', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (74, 23, 'Wiper blade damaged, missing or fails to adequately clear driver''s field of vision', 'Wiper ineffective', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (75, 23, 'Wiper or washer fails to adequately clear driver''s field of vision in area swept by driver''s side wiper', 'Fail to clear window', 1,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (76, 24, 'Next service due date or mileage has exceed limit or will by end of current trip', 'Trailer service overdue', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (77, 24, 'Next service due date or engine hours have exceed limit or will by end of current trip', 'Reefer service overdue', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (78, 24, 'Fluid levels not within specified limits. Damaged or worn belts, hoses or wiring. Engine will not start or operate smoothly', 'Reefer engine operation', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (79, 24, 'Not able to operate temperature control or read temperature setting', 'Temperature control', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (80, 24, 'Not able to achieve or maintain set operating temperature', 'Operating temperature', 0,0,0,'')", "INSERT INTO INSPECTION_ITEM (InspectionItemId, CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,attID) VALUES (81, 25, 'General', 'General', 0,0,0,'')" };

	for (auto command : commands)
	{
		pStmt->SqlStatement(command);
	}

}

PInspectionItem * InspectionDB::read_pitem(SQLiteStatement *p, PInspectionItem *pII,int index)
{
	//PInspectionItem *pII = new PInspectionItem();
	pII->set_iid(p->GetColumnInt(index++));  //0
	pII->set_cid(p->GetColumnInt(index++));
	string str = p->GetColumnString(index++);
	pII->set_defect(str);
	str = p->GetColumnString(index++);    //3
	pII->set_defabbr(str);
	pII->set_deflevel(p->GetColumnInt(index++));
	pII->set_parentid(p->GetColumnInt(index++));   // 5
	pII->set_haschildren(p->GetColumnInt(index++));
	pII->set_attid(p->GetColumnString(index++));
	return pII;
}
PInspectionItem *InspectionDB::GetInspectionItemById(int id)
{
	PInspectionItem *rv = 0;
	auto sql = string_format("SELECT * FROM INSPECTION_ITEM WHERE InspectionItemId = %d", id);
	SQLiteStatement *p = CreateConnection();
	
	p->Sql(sql);
				if (p->FetchRow())
				{
					rv= read_pitem(p, new PInspectionItem());
				}
	p->FreeQuery();
	CloseConn();
	return rv;
}
PInspectionItemList* InspectionDB::GetInspectionItemsByCategoryId(int categoryId, SQLiteStatement *p)
{
	string sql = string_format("SELECT * FROM INSPECTION_ITEM WHERE CategoryID = %d", categoryId);
	PInspectionItemList* list = new PInspectionItemList();
	p->Sql(sql);
	while (p->FetchRow())
	{
		PInspectionItem *pII = new PInspectionItem();
		list->mutable_list()->AddAllocated(read_pitem(p, pII));
	}
	p->FreeQuery();
	return list;
}
PInspectionItemList* InspectionDB::GetInspectionItemsByCategoryId(int categoryId)
{
	SQLiteStatement *p = CreateConnection();
	
	PInspectionItemList* list = this->GetInspectionItemsByCategoryId(categoryId, p);
	CloseConn();
	return list;
}
PInspectionItemList* InspectionDB::GetInspectionItemChildren(int inspItemId)
{
	string sql = string_format("SELECT * FROM INSPECTION_ITEM WHERE parentId= %d", inspItemId);
	PInspectionItemList* list = new PInspectionItemList();
	SQLiteStatement *p = CreateConnection();
	
	p->Sql(sql);
	while (p->FetchRow())
	{
		PInspectionItem *pII = new PInspectionItem();
		list->mutable_list()->AddAllocated(read_pitem(p, pII));
	}
	p->FreeQuery();
	CloseConn();
	return list;
}
void InspectionDB::SetIIParams(SQLiteStatement *pStmt, PInspectionItem *ii)
{
	pStmt->BindInt(1, ii->cid());
	pStmt->BindString(2, ii->defect());
	pStmt->BindString(3, ii->defabbr());
	pStmt->BindInt(4, ii->deflevel());
	pStmt->BindInt(5, ii->parentid()); // parentid
	pStmt->BindInt(6, ii->haschildren());
	pStmt->BindInt(7, ii->iid());
}
void InspectionDB::SaveInspectionItem(PInspectionItem *ii)
{
	SQLiteStatement *p = CreateConnection();
	
	string sql = "UPDATE INSPECTION_ITEM SET CategoryID=?,Defect=?,DefectAbbr=?,DefectLevel=?,parentId=?,hasChildren=? WHERE InspectionItemId=?";
	p->Sql(sql);
	SetIIParams(p, ii);
	p->Execute();
	int tmp = GetNuberOfRowsUpdated();
	p->FreeQuery();
	if (tmp == 0)
	{
		// Do an insert
		sql = "INSERT INTO INSPECTION_ITEM (CategoryID, Defect, DefectAbbr, DefectLevel,parentId,hasChildren,InspectionItemId) VALUES (?,?,?,?,?,?,? );";
		p->Sql(sql);
		SetIIParams(p, ii);
		p->Execute();
		
		p->FreeQuery();

	}
		
	CloseConn();
}
//-----------------------------------------InspectionReportDB-------------------------------------------------
void InspectionDB::CreateInspectionReportTable(SQLiteStatement *p)
{
	string sql = "CREATE TABLE INSPECTION_REPORT (Id INTEGER PRIMARY KEY AUTOINCREMENT, InspectionTime datetime, ModifiedDate datetime, EquipmentID ntext, Odometer INTEGER, DriverId ntext, DriverName ntext, InspectionType INTEGER, HaveSent BOOLEAN, IsFromServer BOOLEAN, cb_cats ntext, Latitude, Longitude, BoxID INTEGER,Signed integer,attID ntext); ";
	CreateTableA(p, sql);

	sql= "CREATE TABLE INSPECTION_REPORT_DEFFECTS(InspectionReportId INTEGER, InspectionItemId INTEGER, Comments ntext, Clr ntext, ClrDriverID ntext, ClrDriverName ntext, attID ntext, MFILES ntext)";
	CreateTableA(p, sql);
}
PInspectionRow *InspectionDB::readInspRow(SQLiteStatement *p)
{
	PInspectionRow *ir = new PInspectionRow();
	//p->GetColumnInt(0), p->GetDateTime(1), p->GetDateTime(2), p->GetColumnString(3), p->GetColumnInt(4), p->GetColumnString(5), p->GetColumnString(6), p->GetColumnInt(7), p->GetBoolean(8), p->GetBoolean(9), p->GetColumnString(10), p->GetDouble(11), p->GetDouble(12), p->GetColumnInt(13)
	
	ir->set_insrid(p->GetColumnInt(0));
	
	string str = p->GetColumnString(1);
	DateTime dt = DateTime::parse(str);
	int64_t tt = dt.getMillis();
	ir->set_instime(tt);  // 1,2
	ir->set_eqid(p->GetColumnString(3));
	ir->set_odo(p->GetColumnInt(4));
	ir->set_drid(p->GetColumnString(5));
	ir->set_drname(p->GetColumnString(6));

	ir->set_attid( p->GetColumnString(15) );
	ir->set_signed_( p->GetColumnInt(14) == 1 ? true : false );

	ir->set_boxid(p->GetColumnInt(13));   //13
	ir->set_instype(p->GetColumnInt(7)); //7
	
	str = p->GetColumnString(10);  // cb_cats + cb_a_cats

	std::vector<std::string> s1 = split(str,'|');
	ir->set_cb_cats("");
	ir->set_cb_a_cats("");
	if (s1.size() ==2)
	{
		ir->set_cb_cats(s1[0]);
		ir->set_cb_a_cats(s1[1]);
	}
	else if (s1.size() == 1) {
		ir->set_cb_cats(s1[0]);
		
	}
	
	//PopulateDeffectList(insRow);
	return ir;
}
void InspectionDB::PopulateDeffectListInIR(PInspectionRow *ir, SQLiteStatement *p)
{
	string sql = string_format(" SELECT * FROM INSPECTION_REPORT_DEFFECTS R left join INSPECTION_ITEM T ON R.InspectionItemId=T.InspectionItemId WHERE InspectionReportId = %d", ir->insrid());
	p->Sql(sql);
	//int count = sqlite3_data_count(p);
	while (p->FetchRow())
	{

		try
		{
			//InspectionReportId INTEGER, InspectionItemId INTEGER, Comments ntext, Clr ntext, ClrDriverID ntext, ClrDriverName ntext, attID ntext, MFILES ntext
			PInspectionItem *pII = new PInspectionItem();
			pII->set_comments(p->GetColumnString(2));
			pII->set_clrd(p->GetColumnString(3));
			string str = p->GetColumnString(4);
			pII->set_clrdid(str);
			str = p->GetColumnString(5);
			pII->set_clrdname(str);
			str = p->GetColumnString(6);
			pII->set_attid(str);

			//PInspectionItem *pII = read_pitem(p);


			std::string s = p->GetColumnString(7); //MFILES
			if (!s.empty() && s.find(",") != string::npos)
			{
				//C# TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to this .NET String method:
				std::vector<std::string> a = split(s, ',');
				if (a.size() > 0)
				{
					//pII->media_files = std::vector<std::string>();
					for (int i = 0; i < a.size(); i++)
					{
						if (!(a[i]).empty())
						{
							pII->add_media_files(a[i]);
						}
					}
				}
			}
			read_pitem(p, pII, 8);
			if (pII->comments() != "" || (pII->media_files_size() > 0))
			{
				ir->mutable_defects()->AddAllocated(pII);
			}


		}
		catch (std::exception &ex)
		{
		}
	}
	p->FreeQuery();

}
void InspectionDB::PopulateDeffectList(PInspectionRowList* list, SQLiteStatement *p)
{
	int len = list->list_size();
	if (len == 0) return;
	for (int i = 0; i < len; i++)
	{
		PInspectionRow *ir = list->mutable_list(i);
		PopulateDeffectListInIR(ir, p);
	}//for (int i = 0; i < len; i++)
		
}

void InspectionDB::SetIRParams(SQLiteStatement *p, PInspectionRow *ir,bool haveSent,bool flag)
{
//InspectionTime, ModifiedDate, EquipmentID, Odometer, DriverId, DriverName, InspectionType, HaveSent, IsFromServer, cb_cats,Latitude, Longitude, BoxID,Signed,attID
	string str= DateTime::ToStringFromMillis(ir->instime());
	p->BindString(1,str);
	p->BindString(2, str); //TimeStamp
	p->BindString(3, ""/*ir->equipid()*/);
	p->BindInt(4, ir->odo());
	p->BindString(5, ir->drid());
	p->BindString(6, ir->drname());
	p->BindInt(7, ir->instype());
	p->BindBool(8, haveSent);
	p->BindBool(9, true);
	
	std::string checkedCats = ir->cb_cats() + std::string("|") + ir->cb_a_cats();
	p->BindString(10, checkedCats);

	p->BindDouble(11, ir->lat());
	p->BindDouble(12, ir->lon());
	p->BindInt(13, ir->boxid());
	p->BindInt(14, ir->signed_());
	p->BindString(15, ir->attid());

	if(flag)
		p->BindInt(16, ir->insrid());

}
void InspectionDB::SetIRDParams(SQLiteStatement *pStmt, PInspectionItem *ii,int insRID,string mfiles)
{
	//inspectionRow->InsRowID, ii->iID, ii->Comments, ii->Clrd, ii->ClrdId, ii->ClrdName, ii->attID, mfiles
	pStmt->BindInt(1, insRID);
	pStmt->BindInt(2, ii->iid());
	pStmt->BindString(3, ii->comments());
	pStmt->BindString(4, ii->clrd());
	pStmt->BindString(5, ii->clrdid()); 
	pStmt->BindString(6, ii->clrdname());
	pStmt->BindString(7, ii->attid());
	pStmt->BindString(8, mfiles);
}
void InspectionDB::InsertDefects(PInspectionRow *ir, SQLiteStatement *p)
{
	int len = ir->defects_size();
	if (len > 0)
	{
		for (int i = 0; i < len; i++)
		{
			PInspectionItem *ii = ir->mutable_defects(i);
			std::string mfiles = adjustFiles(ii);
			//ii->Comments = ii->Comments->Replace("'", "''"); // escape '
			string sql = "INSERT INTO INSPECTION_REPORT_DEFFECTS (InspectionReportId,InspectionItemId,Comments,Clr,ClrDriverId,ClrDriverName,attID,MFILES) VALUES (?,?,?,?,?,?,?,?)";
			p->Sql(sql);
			SetIRDParams(p, ii, ir->insrid(), mfiles);
			p->ExecuteAndFree();
		}
	}
}
void InspectionDB::SaveInspectionReport(PInspectionRow *ir)
{
	
	try
	{
		SQLiteStatement *p = CreateConnection();
		
			if (ir->insrid() < 0)
			{
				// Do an insert
				string sql = "INSERT INTO INSPECTION_REPORT (InspectionTime, ModifiedDate, EquipmentID, Odometer, DriverId, DriverName, InspectionType, HaveSent, IsFromServer, cb_cats,Latitude, Longitude, BoxID,Signed,attID) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
				p->Sql(sql);
				// bind all three values
				SetIRParams(p, ir,false);
				// execute the statement and reset the bindings
				p->ExecuteAndFree();
				sql = "SELECT last_insert_rowid();";
				p->Sql(sql);

				int id = -1;
				while (p->FetchRow())
				{
					id = p->GetColumnInt(0);
				}
				if (id > 0)
				{
					ir->set_insrid(id);
				}
				p->FreeQuery();

				InsertDefects(ir, p);
			}
			else
			{
				// Do an update
				string sql = "UPDATE INSPECTION_REPORT SET InspectionTime=?,ModifiedDate=?,EquipmentID=?,Odometer=?,DriverId=?,DriverName=?,InspectionType=?,HaveSent=?,IsFromServer=?,cb_cats=?,Latitude=?, Longitude=?, BoxID=?,Signed=?,attID=? WHERE Id=?";
				p->Sql(sql);
				SetIRParams(p, ir,false,true);
				p->ExecuteAndFree();
				
				// First delete all the deffects for this report and re-add them for updating procedure
				sql = string_format("DELETE FROM INSPECTION_REPORT_DEFFECTS WHERE InspectionReportId = %d ", ir->insrid());
				p->SqlStatement(sql);

				InsertDefects(ir, p);
			}
		
	}
	catch (SQLiteException &e)
	{
		string str = e.GetString();
		LOGE(str.c_str());
		
	}
	catch (std::exception &ex)
	{
		string str = string(ex.what());
		if (!str.empty())
		{
		}
	}
	CloseConn();

}

std::string InspectionDB::adjustFiles(PInspectionItem *ii)
{
	std::string files = "";
	if (ii->media_files_size() > 0)
	{
		for (std::string s : ii->media_files())
		{
			files = files + s + std::string(","); // list of files -> comma delimited
		}
	}
	return files;
}

//---------------------------------------------------------------
PInspectionRowList* InspectionDB::GetAllInspReports(string sql)
{
	PInspectionRowList* list = new PInspectionRowList();
	SQLiteStatement *p = CreateConnection();
	
	p->Sql(sql);
	while (p->FetchRow())
	{
		PInspectionRow *insRow = readInspRow(p);
		list->mutable_list()->AddAllocated(insRow);
	}
	p->FreeQuery();
	if (list != 0)
		PopulateDeffectList(list, p);

	CloseConn();
	return list;
}
PInspectionRowList* InspectionDB::GetAllUnSent()
{
	string sql = " SELECT * FROM INSPECTION_REPORT WHERE HaveSent = 0 ORDER BY InspectionTime; ";
	//string sql = " SELECT * FROM INSPECTION_REPORT ";
	//string sql = " SELECT * FROM INSPECTION_REPORT where attid not like '%-%'";
	return GetAllInspReports(sql);
}
PInspectionRowList* InspectionDB::GetAllInspReports(int BoxID, int type, int flag)
{//// type=0 - pretrip, 1- posttrip / flag 0-recent, 1 -old

	string sql = string_format("SELECT * FROM INSPECTION_REPORT WHERE BoxID=%d and InspectionType=%d ", BoxID, type);

	if(flag==0)
		sql =sql+ " and InspectionTime >= datetime('now', '-1 day') ";
	else if(flag==1)
		sql =sql+ " and InspectionTime < datetime('now', '-1 day') ";
	sql = sql + " ORDER BY InspectionTime; ";
	return GetAllInspReports(sql);
}
PInspectionRow* InspectionDB::GetInspReport(int InsRID)
{
	PInspectionRow *insRow = 0;
	string sql = string_format(" SELECT * FROM INSPECTION_REPORT WHERE Id = %d  ",InsRID);
	SQLiteStatement *p = CreateConnection();
	p->Sql(sql);
	while (p->FetchRow())
	{
		insRow = readInspRow(p);
		break;;
	}
	p->FreeQuery();

	if(insRow!=0)
	 PopulateDeffectListInIR(insRow, p);

	CloseConn();
	return insRow;
}

void InspectionDB::SetHaveSentDeleteOlder()
{
	SQLiteStatement *p = CreateConnection();
	
	string sql= "DELETE FROM INSPECTION_REPORT_DEFFECTS WHERE InspectionReportId IN (SELECT Id FROM INSPECTION_REPORT WHERE InspectionTime < datetime('now', '-30 day'));";
	p->SqlStatement(sql);
	sql="DELETE FROM INSPECTION_REPORT WHERE InspectionTime < datetime('now', '-30 day');";
	p->SqlStatement(sql);
	sql= "UPDATE INSPECTION_REPORT SET HaveSent = 1; ";
	p->SqlStatement(sql);
	CloseConn();
}