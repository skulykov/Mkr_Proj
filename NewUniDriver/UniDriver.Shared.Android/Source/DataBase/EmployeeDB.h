#ifndef MKR_EMPLOYEEDB_H_INCLUDED
#define MKR_EMPLOYEEDB_H_INCLUDED
#include "BaseDB.h"
//#include "CommonCls.h"
#include "Definitions.h"
#include "TimeUtil.h"

using namespace HosEngine;

class EmployeeDB:public BaseDB
{
private:
	static EmployeeDB *_instance;
	int x;
	
	
	void createTables();
	void CreateEmployeeTable(SQLiteStatement *pStmt);
	void CreateVehiclesTable(SQLiteStatement *pStmt);
	void CreateTripInfoTable(SQLiteStatement *pStmt);
	void CreateGSettingsTable(SQLiteStatement *pStmt);
	void CreateConfigTable(SQLiteStatement *pStmt);
	void CreateCoworkersTable(SQLiteStatement *pStmt);
	void CreateEmailTable(SQLiteStatement *pStmt);
	void CreateVehOdoTable(SQLiteStatement *pStmt);

	EmployeeRow *get_employee(std::string sql);
	void SetParams(SQLiteStatement *pStmt, EmployeeRow *emp);

	//==========Vehicles=================================
	void setVehicleParams(SQLiteStatement *pStmt, PBoxIdVehicleRow  *vi, string EmployeeID, bool haveSent);
	bool saveVehicle2DB(SQLiteStatement *p,PBoxIdVehicleRow *vi, string &did, bool haveSent);
	PBoxIdVehicleRowList *getVehiclesFromDB(SQLiteStatement *p, string &sql);
	PBoxIdVehicleRow *readVehicleFromDB(SQLiteStatement *p);
public:
	EmployeeDB() : BaseDB("db_file_employee.db")
	{
		createTables();
		x = 1;
	}


	//No, destructors are called automatically in the reverse order of construction. (Base classes last). 
	//Do not call base class destructors.
	//!! you do not have to declare the destructor virtual in the derived class
	~EmployeeDB()
	{
		x = 0;
	}

	static EmployeeDB *Get(){
		if (_instance == 0)
			_instance = new EmployeeDB();
		return _instance; 
	}
	static void Clear() {
		if (_instance != 0)
		{
			delete _instance;
		}
		_instance=0;
	}

	EmployeeRow *GetEmployeeById(std::string id);
	void SaveEmployee(EmployeeRow *emp);
	void UpdateCycleById(string id, int cycle);
	EmployeeRow* GetEmployeeWithAutoLogin();
	void SetLogoutById(string id);


	//===========================Vehicles================================
	bool SaveVehicle2DB(PBoxIdVehicleRow *vi,string &did, bool haveSent = true);
	bool SaveVehicleList2DB(PBoxIdVehicleRowList *vList, string &did);
	PBoxIdVehicleRowList * GetVehicleListFromDB(string &did);
};

#endif