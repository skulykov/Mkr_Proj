#include "Dbg.h"
#include "my_utils.h"//#include "CommonUtils/my_utils.h"
#include "EmployeeDB.h"

EmployeeDB *EmployeeDB::_instance = 0;  // initialize static var member

void EmployeeDB::createTables()
{
	SQLiteStatement *pStmt = CreateConnection();
	 
	if (!IsTableExist(pStmt, "EMPLOYEE"))
	{
		CreateEmployeeTable(pStmt);
	}
	if (!IsTableExist(pStmt, "VEHICLES"))
		CreateVehiclesTable(pStmt);
	if (!IsTableExist(pStmt, "TRIPINFO"))
		CreateTripInfoTable(pStmt);

	if (!IsTableExist(pStmt, "GSettings"))
		CreateGSettingsTable(pStmt);
	if (!IsTableExist(pStmt, "CONFIGURATION"))
		CreateConfigTable(pStmt);
	if (!IsTableExist(pStmt, "COWORKERSLOGIN"))
		CreateCoworkersTable(pStmt);
	if (!IsTableExist(pStmt, "EmailTbl"))
		CreateEmailTable(pStmt);

	if (!IsTableExist(pStmt, "VehOdoTbl"))
		CreateVehOdoTable(pStmt);
	CloseConn();
}

void EmployeeDB::CreateEmployeeTable(SQLiteStatement *pStmt)
{
	std::string sql = "CREATE TABLE EMPLOYEE (Id ntext, DriverName ntext,Username ntext, Password ntext,License ntext, AutoLogin BOOLEAN, HomeAddress ntext, IsSupervisor BOOLEAN,Signature ntext,HaveSent BOOLEAN,Cycle integer,HOSExcept integer,Domain ntext,OrgName ntext,OrgAddr ntext,TimeZone,ApplyDTS integer,Email ntext)";
	CreateTableA(pStmt, sql);
}

void EmployeeDB::CreateTripInfoTable(SQLiteStatement *pStmt)
{
	//public class TripInfoRow
	std::string sql = "CREATE TABLE TRIPINFO (ID INTEGER PRIMARY KEY AUTOINCREMENT,EmployeeID ntext, LogTime datetime,JobRef ntext, Permit ntext,VLicense ntext,VLicenseProvince ntext,TrailerLicense ntext,Coworker ntext,VehicleDesc ntext,BoxID integer,OrigLogTime datetime,IsDeleted boolean,HaveSent boolean);";
	CreateTableA(pStmt, sql);
}
void EmployeeDB::CreateGSettingsTable(SQLiteStatement *pStmt)
{
	std::string sql = "CREATE TABLE GSettings (Id INTEGER PRIMARY KEY ,Net3G BOOLEAN,BTEnable BOOLEAN ,LocEnable BOOLEAN,IPAddr ntext,OdoU INTEGER)";

	CreateTableA(pStmt, sql);
}
void EmployeeDB::CreateConfigTable(SQLiteStatement *pStmt)
{
	std::string sql = "CREATE TABLE CONFIGURATION (EmployeeID ntext primary key, LoginFlag integer,ScanFlag boolean, InspHistDays INTEGER ,InspHistAmount INTEGER, ImagesLimit integer,ViolationTH integer,SearchType ntext,ScreenLock integer,OdoInput integer,WeightTH integer)";
	CreateTableA(pStmt, sql);
}
void EmployeeDB::CreateCoworkersTable(SQLiteStatement *pStmt)
{
	std::string sql = "CREATE TABLE COWORKERSLOGIN (EmployeeID ntext,DriverID ntext, DriverName ntext,LoginTime datetime,Status integer)";
	CreateTableA(pStmt, sql);
}
void EmployeeDB::CreateEmailTable(SQLiteStatement *pStmt)
{
	std::string sql = "CREATE TABLE EmailTbl (EmployeeID ntext,e_addr ntext, e_from ntext,e_to ntext,e_types ntext,HaveSent boolean)";
	CreateTableA(pStmt, sql);
}
void EmployeeDB::CreateVehOdoTable(SQLiteStatement *pStmt)
{

	std::string sql = "CREATE TABLE VehOdoTbl (BoxID integer, EmployeeID ntext,InsertTS datetime, OdoV,Type integer);";
	CreateTableA(pStmt, sql);
}


//*************************************************************************************************************************
//*************************************************************************************************************************
EmployeeRow *EmployeeDB::get_employee(std::string sql)
{
	EmployeeRow *emp = 0;
	SQLiteStatement *pStmt = CreateConnection();
	 pStmt->Sql(sql);
	//Id ntext, DriverName ntext,
	//Username ntext, Password ntext,
	//License ntext, AutoLogin BOOLEAN, 
	//HomeAddress ntext, IsSupervisor BOOLEAN,
	//Signature ntext,HaveSent BOOLEAN,
	//Cycle integer,HOSExcept integer,
	//Domain ntext,OrgName ntext,OrgAddr ntext,
	//TimeZone,ApplyDTS integer,Email ntext
	while (pStmt->FetchRow())
	{
		emp = new EmployeeRow();
		emp->set_id(pStmt->GetColumnString(0));
		emp->set_drivername( pStmt->GetColumnString(1));
		emp->set_username(pStmt->GetColumnString(2));
		emp->set_password(pStmt->GetColumnString(3));
		emp->set_license(pStmt->GetColumnString(4));
		emp->SetAutoLogin(pStmt->GetColumnBool(5));
		emp->set_homeaddress(pStmt->GetColumnString(6));
		emp->set_issupervisor(pStmt->GetColumnBool(7));
		emp->set_signature(pStmt->GetColumnString(8));
		emp->haveSend = pStmt->GetColumnBool(9);

		emp->set_cycle((int) (HOSCYCLE)pStmt->GetColumnInt(10));
		emp->set_hosexcept(pStmt->GetColumnInt(11));

		emp->set_domain(pStmt->GetColumnString(12));
		emp->set_orgname(pStmt->GetColumnString(13));
		emp->set_orgaddr(pStmt->GetColumnString(14));
		emp->set_tz( (float)pStmt->GetColumnDouble(15) );

		emp->set_applydts(pStmt->GetColumnInt(16));
		emp->set_email ( pStmt->GetColumnString(17));

		break; // it will not allow to get more then 1 Employee
	}

	// do not forget to clean-up
	pStmt->FreeQuery();
	CloseConn();

	return emp;
}
EmployeeRow *EmployeeDB::GetEmployeeById(std::string id)
{
	std::string sql = m_format("SELECT * FROM EMPLOYEE WHERE Id = '%s' ", id.c_str()); 
	return get_employee(sql);
}

void EmployeeDB::SetParams(SQLiteStatement *pStmt, EmployeeRow *emp)
{
	std::string str = emp->drivername();
	pStmt->BindString(1, str);					// DriverName=@p1
	pStmt->BindString(2, emp->password());			// Password=@p2
	pStmt->BindString(3, emp->username());				// Username=@p3
	pStmt->BindString(4, emp->license());        //License=@p4
	pStmt->BindBool(5, emp->auto_login);        //AutoLogin=@p5
	pStmt->BindString(6, emp->homeaddress());        //p6
	pStmt->BindBool(7, emp->issupervisor());                      //p7
	pStmt->BindString(8, emp->signature());        //p6
	pStmt->BindBool(9, emp->haveSend);                      //p9
	pStmt->BindInt(10, (int)emp->cycle());                      //p10
	//HOSExcept=@p11,Domain=@p12,OrgName=@p13,OrgAddr=@p14,TimeZone=@p15,ApplyDTS=@p16,Email=@p17 WHERE Id = @p18 

	pStmt->BindInt(11, emp->hosexcept());                      //p11
	pStmt->BindString(12, emp->domain());        //p12
	pStmt->BindString(13, emp->orgname());        //p13
	pStmt->BindString(14, emp->orgaddr());        //p14
	pStmt->BindDouble(15, emp->tz());        //p15
	pStmt->BindInt(16, emp->applydts());                      //p16
	pStmt->BindString(17, emp->email());        //p17
	pStmt->BindString(18, emp->id());        //p18
}
void EmployeeDB::SaveEmployee(EmployeeRow *emp)
{
	//combine HomeAddress and State
	std::string _HomeAddress = emp->homeaddress() + "|" + emp->state();  // !!!!
	SQLiteStatement *pStmt = CreateConnection();
	std::string sql ="UPDATE EMPLOYEE SET DriverName=@p1,Password=@p2,Username=@p3,License=@p4,AutoLogin=@p5,HomeAddress=@p6,IsSupervisor=@p7,Signature=@p8,HaveSent=@p9,Cycle=@p10,HOSExcept=@p11,Domain=@p12,OrgName=@p13,OrgAddr=@p14,TimeZone=@p15,ApplyDTS=@p16,Email=@p17 WHERE Id = @p18 ";
	pStmt->Sql(sql);
	// bind all three values
	SetParams(pStmt, emp);
	// execute the statement and reset the bindings
	pStmt->Execute();
	//pStmt->Reset();
	//get the number of rows updated
	int tmp= GetNuberOfRowsUpdated();
	// do not forget to clean-up
	pStmt->FreeQuery();

	if (tmp == 0)
	{
		sql = " INSERT INTO EMPLOYEE (DriverName,Password,Username, License, AutoLogin, HomeAddress, IsSupervisor,Signature,HaveSent,Cycle,HOSExcept,Domain,OrgName,OrgAddr,TimeZone,ApplyDTS,Email, Id)  VALUES ( @p1,@p2, @p3,@p4,  @p5, @p6, @p7,@p8,@p9,@p10,@p11,@p12,@p13,@p14,@p15,@p16,@p17,@p18); ";
		pStmt->Sql(sql);
		SetParams(pStmt, emp);
		
		pStmt->Execute();
		tmp = GetNuberOfRowsUpdated();
		pStmt->FreeQuery();
	}

	CloseConn();
}
void EmployeeDB::UpdateCycleById(string id, int cycle)
{
	SQLiteStatement *p = CreateConnection();
	string sql = string_format("UPDATE EMPLOYEE SET HaveSent=0,Cycle=%d WHERE Id = '%s' ",cycle,id.c_str());
	p->SqlStatement(sql);
	CloseConn();
}
void EmployeeDB::SetLogoutById(string id)
{
	SQLiteStatement *p = CreateConnection();
	string sql = string_format("UPDATE EMPLOYEE SET AutoLogin=0 WHERE Id = '%s' ", id.c_str());
	p->SqlStatement(sql);
	CloseConn();
}
EmployeeRow* EmployeeDB::GetEmployeeWithAutoLogin()
{
	return get_employee(" SELECT * FROM EMPLOYEE WHERE AutoLogin = 1; ");
}


//========================Vehicles=========================================
void EmployeeDB::CreateVehiclesTable(SQLiteStatement *pStmt)
{
	//ID - vehicle ID
	std::string sql = "CREATE TABLE VEHICLES (ID INTEGER,DID ntext,CId INTEGER,Desc ntext,InsertTS integer,vehFlag integer,VLicense ntext,VLicenseProvince ntext,Weight integer,Img ntext,maker ntext,model ntext,color ntext,year integer,sent boolean);";
	CreateTableA(pStmt, sql);
}
void EmployeeDB::setVehicleParams(SQLiteStatement *pStmt,  PBoxIdVehicleRow  *vi, string EmployeeID, bool haveSent)
{
	pStmt->BindString(1, vi->desc());  // Desc
	pStmt->BindInt64(2, vi->mts());     //InsertTS
	pStmt->BindInt(3, vi->flag());     //vehFlag
	pStmt->BindString(4, vi->vl()); //VL
	pStmt->BindString(5, vi->vlp()); //VLP
	pStmt->BindInt(6, vi->weight());     //weight
	pStmt->BindString(7, vi->img()); //Img

	pStmt->BindString(8, vi->maker()); //maker
	pStmt->BindString(9, vi->model()); //model
	pStmt->BindString(10, vi->color()); //color
	pStmt->BindInt(11, vi->year()); //year

	pStmt->BindBool(12, haveSent); //sent

	pStmt->BindInt(13, vi->id());  // ID
	pStmt->BindString(14, EmployeeID); //DID driverID (EmployeeID)
	pStmt->BindInt(15, vi->cid());  // ID
}
bool EmployeeDB::saveVehicle2DB(SQLiteStatement *p,PBoxIdVehicleRow *vi, string &did, bool haveSent)
{
	std::string sql = "UPDATE VEHICLES SET Desc=?,InsertTS=?,vehFlag=?,VLicense=?,VLicenseProvince=?,Weight=?,Img=?,maker=?,model=?,color=?,year=?,sent=? WHERE ID = ? and DID=? and CID=? ";
	p->Sql(sql);
	setVehicleParams(p, vi, did, haveSent);
	p->Execute();
	int tmp = GetNuberOfRowsUpdated();
	p->FreeQuery();
	if (tmp == 0)
	{
		sql = "INSERT INTO VEHICLES (Desc,InsertTS,vehFlag,VLicense,VLicenseProvince,Weight,Img,maker,model,color,year,sent,ID,DID,CID) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
		p->Sql(sql);
		setVehicleParams(p, vi, did, haveSent);

		p->Execute();
		tmp = GetNuberOfRowsUpdated();
		p->FreeQuery();
	}
	return true;
}


bool EmployeeDB::SaveVehicle2DB(PBoxIdVehicleRow *vi,string &did, bool haveSent)
{
	
	SQLiteStatement *p = CreateConnection();
	saveVehicle2DB(p, vi, did, haveSent);
	CloseConn();
	return true;
}
bool EmployeeDB::SaveVehicleList2DB(PBoxIdVehicleRowList *vList, string &did)
{
	int len = vList->vitem_size();
	if (len > 0)
	{
		SQLiteStatement *p = CreateConnection();
		for (int i = 0; i < len; i++)
		{
			PBoxIdVehicleRow *vv = vList->mutable_vitem(i);
			m_printf("Save to DB ..Vehicle Desc:%s ", vv->desc().c_str());
			saveVehicle2DB(p, vv, did, false);  // sent=false since it comes from a server
		}
		CloseConn();
	}
	return true;
}
//ID INTEGER,DID ntext,CId INTEGER,Desc ntext,InsertTS integer,vehFlag integer,VLicense ntext,VLicenseProvince ntext,Weight integer,
//Img ntext,maker ntext,model ntext,color ntext,year ntext,sent boolean
PBoxIdVehicleRow *EmployeeDB::readVehicleFromDB(SQLiteStatement *p)
{
	PBoxIdVehicleRow *v = new PBoxIdVehicleRow();
	v->set_id(p->GetColumnInt(0));
	//v->set_employeeid(p->GetColumnString(1));  //DID
	v->set_cid(p->GetColumnInt(2));
	v->set_desc(p->GetColumnString(3));
	v->set_mts(p->GetColumnInt64(4));
	v->set_flag(p->GetColumnInt(5));

	v->set_vl(p->GetColumnString(6));
	v->set_vlp(p->GetColumnString(7));
	v->set_weight(p->GetColumnInt(8));

	v->set_img(p->GetColumnString(9));
	v->set_maker(p->GetColumnString(10));
	v->set_model(p->GetColumnString(11));
	v->set_color(p->GetColumnString(12));
	v->set_year (p->GetColumnInt(13));

	return v;
}

PBoxIdVehicleRowList * EmployeeDB::getVehiclesFromDB(SQLiteStatement * p, string & sql)
{
	PBoxIdVehicleRowList *list = new PBoxIdVehicleRowList();
	p->Sql(sql);

	while (p->FetchRow())
	{
		PBoxIdVehicleRow *v=readVehicleFromDB(p);
		list->mutable_vitem()->AddAllocated(v);
	}
	return list;
}
PBoxIdVehicleRowList * EmployeeDB::GetVehicleListFromDB(string &did)
{
	SQLiteStatement *p = CreateConnection();
	string sql = m_format("SELECT * FROM VEHICLES WHERE DID =  '%s'",did.c_str());
	PBoxIdVehicleRowList *list = getVehiclesFromDB(p,sql);
	CloseConn();
	return list;
}