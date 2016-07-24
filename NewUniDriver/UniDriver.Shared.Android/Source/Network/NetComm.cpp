#include "NetComm.h"
#include "../mqtt_client/ClsMqtt.h"

#include "my_time.h"
#include "../BizLogic/hos/hos_interface.h"
#include "InspectionDB.h"
#include <queue>          // std::queue

static std::queue<int64_t> myqueue;

int main_proto_read();
string genReq(int req)
{
	Global *g = Global::Get();
	int rid = g->GetDID();
	MCommPacket commP(req, rid);
	string pld = commP.SerializeAsString();
	return pld;
}
string genGBReq(int req)
{
	Global *g = Global::Get();
	int rid = g->GetDID();

	PMobileHB mhb;
	mhb.set_rid(rid);
	mhb.set_lat(43.555);
	mhb.set_lon(-79.3456);
	mhb.set_spd(104);
	mhb.set_cog(222);
	mhb.set_ts(DateTime::Now().getMillis());
	string payload = mhb.SerializeAsString();

	MCommPacket commP(req, rid, payload);
	string pld = commP.SerializeAsString();
	return pld;
}
int SendRequest(int req)
{
	string pld = "";
	ClsMqtt *mqtt = ClsMqtt::Get();
	if (!mqtt->isConnected())
		return -1;
	switch (req)
	{
	case (int)HttpRequestNumber::HBReq:
		pld = genGBReq(req);
		break;
	default:
		pld = genReq(req);
		break;
	}
	LOGI("Sending Request.. %d  \r\n ", req);

	mqtt->send2server((byte*)pld.c_str(), pld.length());
	return 1;
}



//===============================================================================
bool readBinaryFile(string fname, string &sout)
{
#if WIN32
	string path = "C:\\My\\MkrProj\\NewUniDriver\\sqlite_db\\";
#else
	string path = "/sdcard/unidriver/pictures/";
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
	sout = string(pBuff.data(), iSize);
	return true;
}
void _do_send_to_server(string &str_payload,int _req)
{
	int str_len = str_payload.length();
	if (str_len > 0)
	{
		Global *g = Global::Get();
		int rid = g->GetDID();
		MCommPacket commP(_req, rid, str_payload);
		string pld = commP.SerializeAsString();
		int mid = ClsMqtt::Get()->send2server((byte*)pld.c_str(), pld.length());
		int64_t req = _req;//HttpRequestNumber::SendInspectionReports;
		int64_t v = (int64_t)((req << 32) | mid);
		myqueue.push(v);
		LOGI("_do_send_to_server req=%d mid=%d  v=%lld \n",req, mid,v);
	}
}
int send_email(const void*payload, int payload_len)
{
	string str_payload((char*)payload, payload_len);
	_do_send_to_server(str_payload, HttpRequestNumber::Emails);
	return 0;
}
void process_insp_unsent()
{
	LOGI("process_insp_unsent... \n");
	PInspectionRowList* list = InspectionDB::Get()->GetAllUnSent();
	PInspectionReports *rpts = 0;
	int size = list->list_size();
	if (size>0)
	{
		rpts = new PInspectionReports();
		rpts->set_allocated_reports(list);
		for (int i = 0; i < size; i++)
		{
			PInspectionRow *ir = list->mutable_list(i);
			if (ir != 0 && ir->defects_size()>0)
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
							bool rv = readBinaryFile(media_file_name, str);
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
	
	if (rpts != 0)
	{
		string str_payload = rpts->SerializeAsString();
		LOGI("process_insp_unsent...sending %d items \n",size);
		_do_send_to_server(str_payload, HttpRequestNumber::SendInspectionReports);
	
		//delete list;
		delete rpts;
	}
	else
		delete list;
}

void process_tlr_unsent()
{
	//LOGI("process_insp_unsent...1 \n");
	PTimeLogRowList* list = getAllUnsentTLR();
	
	if (list != 0 && list->list_size())
	{
		string str_payload = list->SerializeAsString();
		_do_send_to_server(str_payload, HttpRequestNumber::SendTimeLogs);
	}
	if (list != 0)
		delete list;
}



static void *doSendUnsent(void* _arg)
{
	MqttThreadRequest_T *arg = (MqttThreadRequest_T *)_arg;
	int par = 0;
	if (arg != 0)
		par = arg->par1;

	switch (par)
	{
	case 0:
		process_insp_unsent();
		process_tlr_unsent();
		break;
		case 1:
		
			break;
		default:
			break;
	}
	

	if (arg != 0)
		delete arg;
	return 0;
}
void process_all_unsent_from_sqlite(int par1)
{
	// we send all unsent messages in sqlite db in one shot

	MqttThreadRequest_T *arg = new MqttThreadRequest_T();

	arg->par1 = par1;
	arg->par2 = 2;
	arg->payload = 0;
	arg->cb = 0;
	pthread_t thread = { 0 };
	if (pthread_create(&thread, NULL, doSendUnsent, 0) != 0)
	{
		LOGI("SQLiteQueue threadProc...failed \n");//LOGE("ERROR threadProc... \n");
		if (arg != 0)
		{
			delete arg;
		}
	}
}

//==========process publish_callback=================
void process_publish_callback(int mid)
{
	int req = -1;
	if (!myqueue.empty())
	{
		int64_t v = myqueue.front();
		LOGI("process_publish_callback...myqueue v=%lld \n", v);
		int r_mid = (int)v;
		if (r_mid == mid)
		{
			req =(int) (v >> 32);
			myqueue.pop();
			LOGI("process_publish_callback...Remove myqueue req=%d \n", req);
		}
	}
	if (req == -1)
		return;
	switch (req)
	{
	case HttpRequestNumber::SendInspectionReports:
		InspectionDB::Get()->SetHaveSentDeleteOlder();
		LOGI("SendInspectionReports process_publish_callback mid=%d... database updated \n",mid);
		break;
	case HttpRequestNumber::SendTimeLogs:
		TimeLogDB::Get()->SetHaveSentDeleteOlder();
		break;
	default:
		break;
	}
}