#ifndef MKR_HOSEVENTS_H_INCLUDED
#define MKR_HOSEVENTS_H_INCLUDED

#include <iostream>
#include "EmployeeDB.h"
#include "TimeLogDB.h"
#include "Global.h"
#include "Dbg.h"


TimeLogRow * HosEvents_CreateAtMidNight(int64_t date,bool save_to_db=true,int offset=0);
void addTlr(int status_index, DateTime dt);

void RestructureHosEvents();

#endif