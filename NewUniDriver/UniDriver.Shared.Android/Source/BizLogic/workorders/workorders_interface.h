#ifndef MKR_HOSINTEFACE_H_INCLUDED
#define MKR_HOSINTEFACE_H_INCLUDED

#include <iostream>
#include "EmployeeDB.h"
#include "TimeLogDB.h"
#include "Global.h"
#include "Dbg.h"


#include "my_proto_comm_lite.pb.h"


using namespace std;
using namespace my_comm_proto;


bool  getWoiList(std::vector<PWorkOrderItem*> &v);
void CleanUP_WOList(std::vector<PWorkOrderItem*> &v);

#endif