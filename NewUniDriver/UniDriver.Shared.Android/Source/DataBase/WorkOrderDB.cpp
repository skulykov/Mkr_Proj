#include "my_utils.h"//#include "CommonUtils/my_utils.h"
#include "WorkOrderDB.h"

WorkOrderDB *WorkOrderDB::_instance = 0;  // initialize static var member

void WorkOrderDB::createTables()
{
	SQLiteStatement *pStmt = CreateConnection();
	 
	if (!IsTableExist(pStmt, "WORK_ORDERS"))
	{
		CreateWOTable(pStmt);
	}
	if (!IsTableExist(pStmt, "WORK_ORDERS_MSGS"))
		CreateWOMsgsTable(pStmt);
	
	CloseConn();
}

void WorkOrderDB::CreateWOTable(SQLiteStatement *pStmt)
{
	string sql = "CREATE TABLE WORK_ORDERS (ActivityID INTEGER, StartTime ntext, EndTime ntext,LocationName ntext,Caller ntext,Phone ntext,Reference ntext, " +
		string("Name ntext, Street ntext, City ntext, Country ntext, PostalCode ntext,Province ntext, Instructions ntext,ActInstrs ntext, Latitude real, Longitude real, ResourceID INTEGER,ActStatus integer,serviceGroup integer,HaveSent BOOLEAN)");
	CreateTableA(pStmt, sql);
}
void WorkOrderDB::CreateWOMsgsTable(SQLiteStatement *pStmt)
{
	string sql = "CREATE TABLE WORK_ORDERS_MSGS (ResID INTEGER,ActivityID INTEGER, Msg ntext,insertTime datetime,type integer,HaveSent BOOLEAN)";
	CreateTableA(pStmt, sql);
}

//*************************************************************************************************************************
PWorkOrderItem * WorkOrderDB::readAct(SQLiteStatement *p)
{
	PWorkOrderItem *act = new PWorkOrderItem();
	act->set_id( p->GetColumnInt(0) );
	act->set_starttime(DateTime::parse(p->GetColumnString(1)).getMillis());
	act->set_endtime(DateTime::parse(p->GetColumnString(2)).getMillis());
	act->set_locationname(p->GetColumnString(3));
	act->set_caller(p->GetColumnString(4));
	act->set_phone ( p->GetColumnString(5) );
	act->set_reference ( p->GetColumnString(6) );
	act->set_name ( p->GetColumnString(7) );
	act->set_street ( p->GetColumnString(8) );
	act->set_city ( p->GetColumnString(9) );
	act->set_country ( p->GetColumnString(10) );
	act->set_postalcode ( p->GetColumnString(11) );
	act->set_province ( p->GetColumnString(12) );
	act->set_instructions ( p->GetColumnString(13) );
	act->set_actinstrs ( p->GetColumnString(14) );
	act->set_lat ( p->GetColumnDouble(15));
	act->set_lon ( p->GetColumnDouble(16) );
	act->set_resourceid ( p->GetColumnInt(17) );
	act->set_status ( p->GetColumnInt(18));
	act->set_servicegroup ( p->GetColumnInt(19) );
	
	//act->set_havesent(p->GetColumnBool(20));

	return act;
}
PWOIStatus* WorkOrderDB::readActStatus(SQLiteStatement *p)
{
	PWOIStatus *sts = new PWOIStatus();
	sts->set_id ( p->GetColumnInt(0));
	sts->set_status ( p->GetColumnInt(1));
	return sts;
}

PWorkOrderList *WorkOrderDB::getWorkOrders(string sql)
{
	PWorkOrderList *list = new PWorkOrderList();
	SQLiteStatement *p = CreateConnection();
	p->Sql(sql);
	while (p->FetchRow())
	{
		list->mutable_wo_items()->AddAllocated(readAct(p));
	}
	// do not forget to clean-up
	p->FreeQuery();
	CloseConn();
	return list;
}
PWorkOrderList *WorkOrderDB::GetAllWorkOrders()
{
	string sql = " SELECT * FROM WORK_ORDERS ";//ORDER BY StartTime DESC; ";
	return getWorkOrders(sql);
}
PWorkOrderItem *WorkOrderDB::GetWorkOrderByActivityId(int Id)
{
	string sql = string_format(" SELECT * FROM WORK_ORDERS WHERE Id = %d ", Id);
	PWorkOrderList *list = getWorkOrders(sql);
	return list!=0&&list->wo_items_size()>0?list->mutable_wo_items(0):0;
}
PWorkOrderList *WorkOrderDB::GetWorkOrdersByResourceId(int rId)
{
	string sql = string_format(" SELECT * FROM WORK_ORDERS WHERE ResourceId = %d ", rId);
	return getWorkOrders(sql);
}
PWOIStatusList *WorkOrderDB::GetAllUnSentWorkOrdersWithStatusChange()
{
	PWOIStatusList *list = new PWOIStatusList();
	SQLiteStatement *p = CreateConnection();
	p->Sql("SELECT ActivityID,ActStatus FROM WORK_ORDERS WHERE HaveSent = 0");
	while (p->FetchRow())
	{
		list->mutable_wosts_items()->AddAllocated(readActStatus(p));
	}
	// do not forget to clean-up
	p->FreeQuery();
	CloseConn();
	return list;
}
void WorkOrderDB::SetHaveSent4WorkOrder()
{
	SQLiteStatement *p = CreateConnection();
	p->SqlStatement("UPDATE WORK_ORDERS SET HaveSent = @HaveSent; ");
	CloseConn();
}
//*************************************************************************************************************************
void WorkOrderDB::SetParams(SQLiteStatement *p, PWorkOrderItem  *wi,bool sent)
{
	p->BindString(1, DateTime::ToStringFromMillis(wi->starttime()));
	p->BindString(2, DateTime::ToStringFromMillis(wi->endtime()));
	p->BindString(3,wi->locationname());
	p->BindString(4, wi->caller());
	p->BindString(5, wi->phone());
	p->BindString(6, wi->reference());
	p->BindString(7, wi->name());
	p->BindString(8, wi->street());
	p->BindString(9, wi->city());
	p->BindString(10, wi->country());
	p->BindString(11, wi->postalcode());
	p->BindString(12, wi->province());
	p->BindString(13, wi->instructions());
	p->BindString(14, wi->actinstrs());
	p->BindDouble(15, wi->lat());
	p->BindDouble(16, wi->lon());
	p->BindInt(17, wi->resourceid());
	p->BindInt(18, wi->status());
	p->BindInt(19, wi->servicegroup());
	p->BindBool(20, sent);

	
	p->BindInt64(21, wi->id()); // must be a last one

}
void WorkOrderDB::saveWOI(SQLiteStatement *p, PWorkOrderItem  *wi)
{
	string sql = "UPDATE WORK_ORDERS SET StartTime=?,EndTime=?,LocationName=?,Caller=?,Phone=?,Reference=?," +
		string("Name=?, Street=?,City=?,Country=?, PostalCode=?,Province=?,Instructions=?,ActInstrs=?, Latitude=?,Longitude=?,ResourceID=?,ActStatus=?,serviceGroup=?,HaveSent=?  WHERE ActivityID = ?");
	p->Sql(sql);
	SetParams(p, wi,true);
	p->Execute();
	int tmp = GetNuberOfRowsUpdated();
	p->FreeQuery();
	if (tmp == 0)
	{
		sql = " INSERT INTO WORK_ORDERS (StartTime,EndTime,LocationName,Caller,Phone,Reference,Name,Street,City,Country,PostalCode,Province,Instructions,ActInstrs, Latitude, Longitude, ResourceID,ActStatus,serviceGroup,HaveSent,ActivityID)" +
			string(" VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
		p->Sql(sql);
		SetParams(p, wi,false);
		p->ExecuteAndFree();
	}
}
void WorkOrderDB::SaveWorkOrders(PWorkOrderList *alist)
{
	SQLiteStatement *p = CreateConnection();
	int len = alist->wo_items_size();
	if (len > 0)
	{
		for (int i = 0; i < len; i++)
		{
			PWorkOrderItem *pwi = alist->mutable_wo_items(i);
			if(pwi!=0)saveWOI(p, pwi);
		}
	}
	CloseConn();
}
void WorkOrderDB::UpdateWorkOrderStatus(PWorkOrderItem *woi/*,bool insert*/)
{
	SQLiteStatement *p = CreateConnection();
	string sql = string_format("UPDATE WORK_ORDERS SET ActStatus=%d,HaveSent=0  WHERE ActivityID=%d",woi->status(),woi->id());
	p->SqlStatement(sql);
	CloseConn();
}