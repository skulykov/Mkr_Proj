#include "Global.h"
#include "test_inspection.h"
#include "InspHelper.h"

class MCommPacket :public CommPacket
{
public:
	MCommPacket() {}
	MCommPacket(int req, int rid, string payload)
	{
		//Global *g = Global::Get();
		int cid = 3604;// g->GetCID();
		this->set_req(req);

		this->set_rid(rid);
		this->set_cid(cid);
		this->set_user("*");
		this->set_payload(payload);
	}
};


PInspectionItem * create_fakeDefect()
{
	PInspectionItem *pII = new PInspectionItem();
	pII->set_iid(1);
	pII->set_cid(1);
	pII->set_comments("My Comments");
	pII->set_clrd("Clrd11");
	pII->set_clrdid("ClrdID22");
	pII->set_clrdname("Clrd Name");

	pII->set_attid("AttID=9999");
	std::string s = "my1.jpg,my2.jpg,my3.jpg";
	if (!s.empty() && s.find(",") != string::npos)
	{
		std::vector<std::string> a = split(s, ',');
		if (a.size() > 0)
		{
			//pII->media_files = std::vector<std::string>();
			for (int i = 0; i < a.size(); i++)
			{
				if (!(a[i]).empty())
				{
					pII->add_media_files(a[i]);
				}
			}
		}
	}
	/*if (pII->comments() != "" || (pII->media_files_size() > 0))
	{
		ir->mutable_defects()->AddAllocated(pII);
	}*/
	return pII;
}


//void Test_SaveInspection(InspectionDB *db)
//{
//	int vid = 1111;
//	PInspectionRow *ir = new PInspectionRow();
//	ir->set_insrid(-1);
//	ir->set_instime(DateTime::UtcNow().getMillis());
//	ir->set_cb_cats("1,2,3,4,5,");
//	ir->set_cb_a_cats("11,12,13,14,15");
//	ir->set_boxid(vid);
//	ir->set_drid("1024");
//	ir->set_drname("SergeyKulykov");
//	ir->set_attid("-100");
//
//
//	PInspectionItemList* ll = db->GetInspectionItemsByCategoryId(1);
//	if (ll != 0 && ll->list_size() > 0)
//	{
//		for (int i = 0; i < ll->list_size(); i++)
//		{
//			PInspectionItem *pII = ll->mutable_list(i);
//			addDefect(pII, ir);
//		}
//	}
//
//	
//	db->SaveInspectionReport(ir);
//}
void test_existed_insp(int IspRID)
{
	InspHelper *h = InspHelper::Get();
	PInspectionCategoryRowList* ir_list = h->CreateInspection(IspRID, 0, -1,0);
	
	if (ir_list != 0)
	{
		PInspectionCategoryRow * icr=h->GetCatRow(0);
		if (icr != 0)
		{
			PInspectionItemList* ii_list = h->ReadInspectionItemsByCategoryIdFromCache(icr->cid());
			if (ii_list != 0)
			{
				PInspectionItem * pii1 = ii_list->mutable_list(0);

				int iID = pii1->iid();
				PInspectionItem * p = h->GetDefect(iID);
				if (p != 0)
				{
					printf("test_existed_insp Defect cid=%d id=%d desc=%s media_files_size=%d\r\n", p->cid(), p->iid(), p->comments().c_str(), p->media_files_size());
				}
			}
		}
	}

	bool def = h->isContainsDefects();
	if (def)
	{
		h->printAllDefects();
	}
}


//====================Test inspection Helper====================

PInspectionCategoryRowList* create_New_Insp_with_def(InspHelper *h)
{
	PInspectionCategoryRowList* ir_list = h->CreateInspection(-1, 1, -1,0);                     // new inspection

	PInspectionItem *pDef = create_fakeDefect();
	h->addDefect(pDef);
	return ir_list;
}

void show_insp_def(PInspectionCategoryRowList* ir_list)
{
	
	InspHelper *h = InspHelper::Get();
	if (ir_list != 0)
	{
		PInspectionCategoryRow * icr = h->GetCatRow(0);
		if (icr != 0)
		{
			PInspectionItemList* ii_list = h->ReadInspectionItemsByCategoryIdFromCache(icr->cid());
			if (ii_list != 0)
			{
				PInspectionItem * pii1 = ii_list->mutable_list(0);

				int iID = pii1->iid();
				PInspectionItem * p = h->GetDefect(iID);
				if (p != 0)
				{
					
						printf("show_insp_def Defect cid=%d id=%d desc=%s media_files_size=%d\r\n", p->cid(), p->iid(), p->comments().c_str(), p->media_files_size());
					
				}
			}
		}
	}

	bool def = h->isContainsDefects();
	if (def)
	{
		h->printAllDefects();
	}
}

void test_Insp_helper()
{
	/*
	InspHelper *h = InspHelper::Get();
	PInspectionCategoryRowList* ir_list = create_New_Insp_with_def(h);
	show_insp_def(ir_list);
	
	int rv=h->getAllCheckedAndSaveInspection(123456);   // save inspection
	if (rv == 1)
	{

	}
	*/
	//
	test_existed_insp(1);
}
void write_binary_file(string fname, string str)
{
	ofstream file1(fname, ios::out | ios::binary);
	if (file1.good()) {
		file1.write(str.c_str(), str.size());
		file1.close();
	}
	else {
		//std::cout << "file error write" << endl;
	}
}
#if 0
//Network\NetComm.cpp(62)
bool readBinaryFile(string fname,string &sout)
{
#if WIN32
	string path = "C:\\My\\MkrProj\\NewUniDriver\\sqlite_db\\";
#else
	string path="/sdcard/unidriver/pictures/"
#endif
	fname = path + fname;
	ifstream in(fname, ios::binary);
	in.seekg(0, ios::end);
	int iSize = in.tellg();
	in.seekg(0, ios::beg);

	std::vector<char> pBuff(iSize);
	if (iSize > 0)
		in.read(&pBuff[0], iSize);
	in.close();
	sout= string(pBuff.data(), iSize);
	return true;
}
#else
bool readBinaryFile(string fname, string &sout);
#endif
void process_all_unsent()
{
	PInspectionRowList* list = InspectionDB::Get()->GetAllUnSent();
	if (list != 0 && list->list_size() > 0)
	{
		InspectionDB::Get()->SetHaveSentDeleteOlder();
	}
}
void process_all_unsent_1()
{
	PInspectionRowList* list = InspectionDB::Get()->GetAllUnSent();
	PInspectionReports *rpts = new PInspectionReports();
	if (list != 0)
	{
		rpts->set_allocated_reports(list);
		int size = list->list_size();
		for (int i = 0; i < size; i++)
		{
			PInspectionRow *ir = list->mutable_list(i);
			if (ir != 0&&ir->defects_size()>0)
			{
				int dsize = ir->defects_size();
				for (int j = 0; j < dsize; j++)
				{
					PInspectionItem *pii = ir->mutable_defects(j);
					if (pii != 0 && pii->media_files_size()>0)
					{
						int mfsize = pii->media_files_size();
						for (int k = 0; k < mfsize; k++)
						{
							string media_file_name = pii->media_files(k);
							string str = "";
							bool rv=readBinaryFile(media_file_name, str);
							if (rv&&str.length()>0)
							{
								PInspectionReportMediaFile *imf = new PInspectionReportMediaFile();
								imf->set_fname(media_file_name);
								imf->set_payload(str);
								rpts->mutable_mfiles()->AddAllocated(imf);
							}
						}
					}
				}
			}
		}
		
	}//if (list != 0)

	string str_payload = rpts->SerializeAsString();
	int str_len = str_payload.length();
	if (str_len > 0)
	{
		MCommPacket commP(100, 1000, str_payload);
		string pld = commP.SerializeAsString();


		 //save to file
		string fname_insp = "C:\\My\\MkrProj\\NewUniDriver\\sqlite_db\\insp_reports_from_cpp.bin";
		write_binary_file(fname_insp, pld);

	}
	//delete list;
	delete rpts;
}

void test_insp_db()
{
	int vid = 1111;
	InspectionDB *db = InspectionDB::Get();
	if (db != 0)
	{
		try {
			PInspectionCategoryRowList *cL = InspHelper::Get()->ReadAllCategoriesFromDB(-1, false);
			if (cL != 0)
			{
				string pld = cL->SerializeAsString();
				int size = pld.length();

				delete cL;
			}
		}
		catch (std::exception &ex)
		{
			string msg = std::string("READ ALL Categories Exc:") + ex.what();// +std::string(" Stk:") + ex.StackTrace;
			printf("Exc:%s",msg.c_str()) ;
		}
		//db->SaveEmployee(emp);
		CategoryVehicleAssignment *list1=db->GetVehicleCategories(vid);
		if (list1 != 0)
		{
		}
		else
		{

		}

//		SaveInspection(db);

		
		PInspectionRowList* list = db->GetAllUnSent();//   GetAllInspReports(10023, 0, 0);           //db->GetAllUnSent();
		PInspectionRowList* list2 = db->GetAllInspReports(10023,1, 0);

		try {
			string pld = list2->SerializeAsString();
			int size = pld.length();
		}
		catch (std::exception &ex)
		{
			string msg = std::string("READ ALL Categories Exc:") + ex.what();// +std::string(" Stk:") + ex.StackTrace;
			printf("Exc:%s", msg.c_str());
		}
		if (list != 0&& list2 != 0)
		{
			int len = list->list_size();
			if (len == 0) return;
			for (int i = 0; i < len; i++)
			{
				PInspectionRow *ir = list->mutable_list(i);
				printf("\r\nIR type:%d did:%s boxid:%d date:%s\r\n",ir->instype(), ir->drid().c_str(),ir->boxid(),DateTime::ToStringFromMillis(ir->instime()).c_str());
				int len1 = ir->defects_size();
				if (len1 > 0)
				{
					printf("defects len=%d\r\n", len1);
					for (int i = 0; i < len1; i++)
					{
						PInspectionItem *ii = ir->mutable_defects(i);
						if (ii != 0)
						{
							int id = ii->iid();
							string aId = ii->attid();
							printf("comments=%s\r\n", ii->comments().c_str());
						}
					}
				}
			}
		}
	}
}

/*
/*
std::string id = Global::Get()->GetDriverID();
string drName = Global::Get()->GetDriverName();
PInspectionRowList *list = new PInspectionRowList();
PInspectionRow *r = new PInspectionRow();
r->set_insrid(1122);
r->set_instime(DateTime::Now().getMillis());
r->set_signed_(true);
r->set_drid(id);
r->set_drname(drName);
r->set_instype(type);
r->set_cb_cats("**");
r->set_boxid(1234);
list->mutable_list()->AddAllocated(r);

return list;
*/





