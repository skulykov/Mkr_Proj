#ifndef MKR_DBINTEFACE_H_INCLUDED
#define MKR_DBINTEFACE_H_INCLUDED

#include <iostream>

#include "EmployeeDB.h"
#include "TimeLogDB.h"
#include "InspectionDB.h"

void clear_employee_db();
void save_employee(EmployeeRow *emp);
EmployeeRow *read_current_employee_profile();


void test_insp_db();

PInspectionRowList * read_inspections(int type, int flag);
PInspectionCategoryRowList* ReadAllCategories(int boxID, bool att_query);
PInspectionItemList* ReadInspectionItemsByCategoryId(int catID);


#endif