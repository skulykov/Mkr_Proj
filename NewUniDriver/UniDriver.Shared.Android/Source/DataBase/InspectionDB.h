#ifndef MKR_INSPECTIONDB_H_INCLUDED
#define MKR_INSPECTIONDB_H_INCLUDED

#include <string>
#include <vector>
#include <stdexcept>

#include "BaseDB.h"
#include "Definitions.h"
#include "TimeUtil.h"

#include "my_utils.h"

using namespace HosEngine;

struct CategoryVehicleAssignment
{
	string groupIDs = "";
	/*int64_t*/string updateTS = "";
};

class InspectionDB : public BaseDB
{
private:
	static InspectionDB *_instance;
	void CreateTables();
	void CreateCategoryTable(SQLiteStatement *pStmt);
	void CreateCategoryVehicleAssignmentTable(SQLiteStatement *p);
	void SetCatParams(SQLiteStatement *pStmt, PInspectionCategoryRow *icr);
	void SetIIParams(SQLiteStatement *pStmt, PInspectionItem *ii);
	PInspectionItem * read_pitem(SQLiteStatement *p, PInspectionItem *pII, int index=0);
	PInspectionRow *readInspRow(SQLiteStatement *p);
	void PopulateDeffectList(/*PInspectionRow *ir*/PInspectionRowList* list, SQLiteStatement *p);
	string adjustFiles(PInspectionItem *ii);
	void SetIRParams(SQLiteStatement *p, PInspectionRow *ir, bool haveSent, bool flag=false);
	void SetIRDParams(SQLiteStatement *p, PInspectionItem *ii, int insRID, string mfiles);
	void InsertDefects(PInspectionRow *ir, SQLiteStatement *p);
	void PopulateDeffectListInIR(PInspectionRow *ir, SQLiteStatement *p);
	PInspectionRowList* GetAllInspReports(string sql);
public:
	InspectionDB() : BaseDB("inspections.db")
	{
		CreateTables();
	}
	static InspectionDB *Get();
	PInspectionCategoryRowList* GetAllCategories(int boxID, bool att_query = false);
	void SaveCategory(PInspectionCategoryRowList* list, bool delete_flag, std::vector<int> &grIDs);
	void SaveCategory2VehicleAssignment(int boxId, const std::string &groupIDs);
	CategoryVehicleAssignment *GetVehicleCategories(int boxID);

	//-----------------Inspection Items------------------------
	void CreateInspectionItemTable(SQLiteStatement *pStmt);
	PInspectionItemList* GetInspectionItemsByCategoryId(int categoryId);
	PInspectionItemList* GetInspectionItemsByCategoryId(int categoryId, SQLiteStatement *p);
	PInspectionItemList* GetInspectionItemChildren(int inspItemId);
	
	PInspectionItem *GetInspectionItemById(int id);
	void SaveInspectionItem(PInspectionItem *inspectionItem);

	//-----------------------------------------InspectionReportDB-------------------------------------------------
	void CreateInspectionReportTable(SQLiteStatement *p);
	PInspectionRowList* GetAllUnSent();
	void SetHaveSentDeleteOlder();

	void SaveInspectionReport(PInspectionRow *ir);
	PInspectionRow* GetInspReport(int InsRID);
	PInspectionRowList* GetAllInspReports(int BoxID, int type,int flag);
};

#endif