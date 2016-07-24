#ifndef MKR_INSPECTIONINTEFACE_H_INCLUDED
#define MKR_INSPECTIONINTEFACE_H_INCLUDED

#include <iostream>
#include "EmployeeDB.h"
#include "TimeLogDB.h"
#include "InspectionDB.h"
#include "Global.h"
#include "Dbg.h"

class InspHelper
{
	static InspHelper *_instance;
	PInspectionRow *gIR;
	PInspectionCategoryRowList* cL;
	int is_new;
	int BoxID;

	int getCatColor(int cID, int attID);
	void processCategories();
	int SaveInspection(int odo);
public:
	InspHelper() { gIR = 0; cL = 0; }
	static InspHelper *Get();
	PInspectionCategoryRowList* CreateInspection(int irID,int is_new,int BoxID,int inspType);
	void addDefect(PInspectionItem *pII);

	bool isContainsDefects();
	bool processItemDefectsInSubCats(int iID);
	
	PInspectionItemList*  ReadInspectionItemsByCategoryIdFromCache(int catID);
	PInspectionItem *GetDefect(int iID);
	int getIIColor(int iID);
	PInspectionCategoryRow *GetCatRow(int cID);
	int setAllChecked(int flag);
	int getAllCheckedAndSaveInspection(int odo);
	PInspectionRowList *ReadAllInspReportsFromDB(int BoxID, int type, int flag);
	PInspectionCategoryRowList* ReadAllCategoriesFromDB(int boxID, bool att_query);
	PInspectionItemList* ReadInspectionItemsByCategoryIdFromDB(int catID);

	void printAllDefects();  // just for test
};
#endif