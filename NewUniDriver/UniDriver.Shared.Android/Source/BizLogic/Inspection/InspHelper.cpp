#include "insphelper.h"
#include "db_interface.h"

InspHelper* InspHelper::_instance = 0;
InspHelper *InspHelper::Get()
{
	if(_instance==0)
		_instance= new InspHelper();
	return _instance;
}
void InspHelper::processCategories()
{
	if (this->gIR == 0||this->cL==0) return;
	vector<string>  catIds = split(gIR->cb_cats(),',');
	int CategoryID_Index = 0;
	int size = cL->list_size();
	for (int i = 0; i < size;i++) 
	{
		PInspectionCategoryRow  *icr = cL->mutable_list(i);
		if (catIds.size() > 0) 
		{
				string s = catIds[CategoryID_Index];
				if (s.empty())
					continue;
				string str = "+";
				if (stdH::contains(s,str))
					icr->set_ischecked(2/* CheckType.ScannedChecked*/);
				else
					icr->set_ischecked(1/* CheckType.ManualChecked*/);
		}
		CategoryID_Index++;
	}
}
PInspectionCategoryRowList* InspHelper::CreateInspection(int InsRID, int is_new, int BoxID,int inspType)
{
	//LOGE("CreateInspection..start..");
	if (this->gIR != 0)
		delete this->gIR;
	if (this->cL != 0)
		delete cL;
	//LOGE("CreateInspection..deleted..");
	this->is_new = is_new;
	cL = ReadAllCategoriesFromDB(BoxID, false);
	if (is_new)
	{
		this->gIR = new PInspectionRow();
		this->gIR->set_instype(inspType);
		this->BoxID = BoxID;
	}
	else if(InsRID>0)
	{
		//read inspection report
		InspectionDB *db = InspectionDB::Get();
		if (db != 0)
		{
			this->gIR = db-> GetInspReport(InsRID);
			processCategories();
		}
	}
	

	return cL;
}
PInspectionItemList*  InspHelper::ReadInspectionItemsByCategoryIdFromCache(int catID)
{
	if (this->cL != 0)
	{
		int size=cL->list_size();
		for (int i = 0; i < size; i++)
		{
			PInspectionCategoryRow *icr = cL->mutable_list(i);
			if (icr&&icr->cid() == catID)
			{
				PInspectionItemList *iiL1 = icr->mutable_items(); //icr->items();
				return iiL1;
			}
		}
	}
	return NULL;
}
PInspectionRowList *InspHelper::ReadAllInspReportsFromDB(int BoxID,int type, int flag)
{

	PInspectionRowList *list = 0;
	InspectionDB *db = InspectionDB::Get();
	if (db != 0)
	{

		//try {
		//LOGI("read_inspections_1 ");
		list = db->GetAllInspReports(BoxID,type, flag);
		//LOGI("read_inspections_2 ");
		//}
		/*
		catch (std::exception &ex)
		{
		string msg = ex.what();// +std::string(" Stk:") + ex.StackTrace;
		LOGE("read_inspections Exc:%s", msg.c_str());

		}
		catch (...)
		{
		string what = "read_inspections Unidentified ex..";// string(ex.what());
		if (what.empty())
		{

		}
		LOGE(what.c_str());
		}*/
	}
	return list;
}
PInspectionCategoryRowList* InspHelper::ReadAllCategoriesFromDB(int boxID, bool att_query)
{
	InspectionDB *db = InspectionDB::Get();
	if (db != 0)
	{
		return db->GetAllCategories(boxID, att_query);
	}
	return 0;
}
PInspectionItemList* InspHelper::ReadInspectionItemsByCategoryIdFromDB(int catID)
{
	InspectionDB *db = InspectionDB::Get();
	if (db != 0)
	{
		return db->GetInspectionItemsByCategoryId(catID);
	}
	return 0;
}


void InspHelper::addDefect(PInspectionItem *pII)
{
	if (gIR != 0)
	{
		PInspectionItem *p = 0;
		int size = gIR->defects_size();
		//1. Check to see if we have this inspection item as a defect
		int index = -1;
		for (int i = 0; i < size; i++)
		{
			p = gIR->mutable_defects(i);
			if (p->iid() == pII->iid())
			{
				index = i;
				break;
			}
		}
		if (index != -1)
		{
			gIR->mutable_defects()->DeleteSubrange(index, 1); // delete it
		}
		p = new PInspectionItem();
		p->CopyFrom(*pII);
		gIR->mutable_defects()->AddAllocated(p);
		//LOGE("addDefect..iID=%d..", p->iid());
	}
}
bool InspHelper::isContainsDefects()// make sure that we have at least one Defect in this report
{
	if (gIR != 0)
	{
		return gIR->defects_size()>0;
	}
	return false;
}
bool InspHelper::processItemDefectsInSubCats(int iID)
{
	if (gIR != 0)
	{
		return gIR->defects_size()>0;
	}
	return false;
}
PInspectionItem *InspHelper::GetDefect(int iID)
{
	PInspectionItem *p = 0;
	if (gIR != 0)
	{
		PInspectionItem *p = 0;
		int size = gIR->defects_size();
		//1. Check to see if we have this inspection item as a defect
		int index = -1;
		for (int i = 0; i < size; i++)
		{
			p = gIR->mutable_defects(i);
			if (p->iid() == iID)
			{
				return p;
			}
		}
	}
	return p;
}
void InspHelper::printAllDefects()
{
	PInspectionItem *p = 0;
	if (gIR != 0)
	{
		PInspectionItem *p = 0;
		int size = gIR->defects_size();
		//1. Check to see if we have this inspection item as a defect
		int index = -1;
		for (int i = 0; i < size; i++)
		{
			p = gIR->mutable_defects(i);
			printf("Defect cid=%d id=%d desc=%s media_files_size=%d\r\n", p->cid(), p->iid(), p->comments().c_str(), p->media_files_size());
		}
	}
	
}
PInspectionCategoryRow *InspHelper::GetCatRow(int pos)
{
	PInspectionCategoryRow *p = 0;
	if (gIR != 0&&cL!=0)
	{
		PInspectionCategoryRow *p = 0;
		int size = cL->list_size();
		if (pos < size)
		{
			p = cL->mutable_list(pos);
			int cl = getCatColor(p->cid(), 0);
			p->set_color(cl);
			return p;
		}
		
	}
	return p;
}
int InspHelper::getCatColor(int cID, int attID)
{
	if (gIR != 0)
	{
		//LOGE("getIIColor..iID=%d..", iID);
		int size = gIR->defects_size();
		for (int i = 0; i < size; i++)
		{
			PInspectionItem *p = gIR->mutable_defects(i);
			if (p->cid() == cID)
				return 0xffff0000;
		}
	}
	return 0xff000000; // black
}
int InspHelper::getIIColor(int iID)
{
	if (gIR != 0)
	{
		//LOGE("getIIColor..iID=%d..", iID);
		int size=gIR->defects_size();
		for (int i = 0; i < size; i++)
		{
			PInspectionItem *p = gIR->mutable_defects(i);
			
			if (p->iid() == iID)
			{
				int rv = 1;
				if (p->media_files_size() > 0)
					rv = 3;
				return rv;// 0xffff0000;
			}
		}
	}
	return 0;// 0xff000000; // black
}
int InspHelper::setAllChecked(int flag)
{
	PInspectionCategoryRow *p = 0;
	if (cL != 0)
	{
		PInspectionCategoryRow *p = 0;
		int size = cL->list_size();
		for (int i = 0; i < size;i++)
		{
			p = cL->mutable_list(i);
			p->set_ischecked(flag);
			
		}
		return 1;
	}
	return 0;
}
int InspHelper::getAllCheckedAndSaveInspection(int odo)
{
	int rv = 1;
	PInspectionCategoryRow *p = 0;
	if (cL != 0)
	{
		PInspectionCategoryRow *p = 0;
		int size = cL->list_size();
		for (int i = 0; i < size; i++)
		{
			p = cL->mutable_list(i);
			if (p->ischecked() == 0)
			{
				rv = 0;   //comment out -> just for test
				break;
			}

			//find out how this category was checked ??
			string s = gIR->cb_cats();// +"";
			string _cid = Int2Str(p->cid());
			string s1 = "";
			if (p->ischecked() == 1/*CheckType.ManualChecked*/)
			{
				//Global.globalInspectionReport.cb_cats.Length == 0 ? icr.cID.ToString() : ("," + icr.cID.ToString());
				s1 = s.length() == 0 ? _cid : "," + _cid;
			}
			else if (p->ischecked() == 2/*CheckType.ScannedChecked*/)
			{
				// Global.globalInspectionReport.cb_cats.Length == 0 ? "+" + icr.cID.ToString() : (",+" + icr.cID.ToString());
				s1 = s.length() == 0 ?"+"+ _cid : ",+" + _cid;
			}
			else {
				//should not be here!!!
			}
			gIR->set_cb_cats(s + s1);

		}
		if (rv == 1)
		{
			rv = SaveInspection(odo);
		}
		return rv;
	}
	return 0;
}
int InspHelper::SaveInspection(int odo)
{
	int rv = 0;
	InspectionDB *db = InspectionDB::Get();
	if (gIR!=0&&db != 0&&is_new==1)
	{
		try {
			PInspectionRow *ir = gIR;
			ir->set_insrid(-1);
			ir->set_instime(DateTime::Now().getMillis());

			ir->set_cb_a_cats("");
			ir->set_boxid(this->BoxID);
			std::string did = Global::Get()->GetDriverID();
			std::string drname = Global::Get()->GetDriverName();
			ir->set_drid(did);
			ir->set_drname(drname);
			ir->set_attid("-100");
			ir->set_odo(odo);
			db->SaveInspectionReport(gIR);
			rv = 1;
			LOGI("Saved IR Did:%s boxid=%d ",did.c_str(), this->BoxID);
		}
		catch (std::exception &ex)
		{
			string msg =  ex.what();// +std::string(" Stk:") + ex.StackTrace;
			LOGE("SaveInspection Exc:%s", msg.c_str());
			rv = 0;
		}
		
	}
	return rv;
}
