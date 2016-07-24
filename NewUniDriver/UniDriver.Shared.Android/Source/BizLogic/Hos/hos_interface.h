#ifndef MKR_HOSINTEFACE_H_INCLUDED
#define MKR_HOSINTEFACE_H_INCLUDED

#include <iostream>
#include "EmployeeDB.h"
#include "TimeLogDB.h"
#include "Global.h"
#include "Dbg.h"

int process_driver_status(PTimeLogRow *ptlr,bool modify_ts=true );
PTimeLogRow *getLastDriverStatus();

PTimeLogRowList* getTLRList();
PTimeLogRowList* getAllTLRList4Date(int64_t date);
PRecapRowList* getRecapList();

PTimeLogRowList* getAllUnsentTLR();
string GetStringFromCurrentHosCycle();
int UpdateHosCycle(int cycle);
int CheckIfLoginIsNeeded();


#endif