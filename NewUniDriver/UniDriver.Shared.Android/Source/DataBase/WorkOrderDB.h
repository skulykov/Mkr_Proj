#ifndef MKR_WORKORDERDB_H_INCLUDED
#define MKR_WORKORDERDB_H_INCLUDED
#include "BaseDB.h"
//#include "CommonCls.h"
#include "Definitions.h"
#include "TimeUtil.h"

using namespace HosEngine;

class WorkOrderDB:public BaseDB
{
private:
	static WorkOrderDB *_instance;
	int x;
	
	
	void createTables();
	void CreateWOTable(SQLiteStatement *pStmt);
	void CreateWOMsgsTable(SQLiteStatement *pStmt);
	

	PWorkOrderItem * readAct(SQLiteStatement *p);
	PWOIStatus* readActStatus(SQLiteStatement *p);
	PWorkOrderList *getWorkOrders(string sql);
	void SetParams(SQLiteStatement *pStmt, PWorkOrderItem  *wi,bool sent);
	void saveWOI(SQLiteStatement *pStmt, PWorkOrderItem  *wi);
public:
	WorkOrderDB() : BaseDB("db_workorders.db")
	{
		createTables();
		x = 1;
	}


	//No, destructors are called automatically in the reverse order of construction. (Base classes last). 
	//Do not call base class destructors.
	//!! you do not have to declare the destructor virtual in the derived class
	~WorkOrderDB()
	{
		x = 0;
	}

	static WorkOrderDB *Get(){
		if (_instance == 0)
			_instance = new WorkOrderDB();
		return _instance; 
	}
	static void Clear() {
		if (_instance != 0)
		{
			delete _instance;
		}
		_instance=0;
	}

	PWorkOrderList *GetAllWorkOrders();
	PWorkOrderItem *GetWorkOrderByActivityId(int Id);
	PWorkOrderList *GetWorkOrdersByResourceId(int rId);
	PWOIStatusList *GetAllUnSentWorkOrdersWithStatusChange();
	void SetHaveSent4WorkOrder();
	void SaveWorkOrders(PWorkOrderList *alist);
	void UpdateWorkOrderStatus(PWorkOrderItem *woi/*,bool insert*/);
};

#endif