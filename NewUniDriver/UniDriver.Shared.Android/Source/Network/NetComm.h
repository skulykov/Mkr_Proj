#ifndef MKR_NETCOMM_H_INCLUDED
#define MKR_NETCOMM_H_INCLUDED

#include "Global.h"
#include "network/TcpRequest.h"
#include "CommonUtils/Dbg.h"

#include "my_proto_comm_lite.pb.h"

using namespace std;
using namespace my_comm_proto;

class MCommPacket :public CommPacket
{
	void init(int req, int rid)
	{
		Global *g = Global::Get();
		int cid = g->GetCID();
		this->set_req(req);

		this->set_rid(rid);
		this->set_cid(cid);
		this->set_user("*");
		this->set_payload("");
	}
public:
	MCommPacket() {}
	MCommPacket(int req, int rid) { init(req, rid); }
	MCommPacket(int req,int rid,string &payload)
	{
		init(req, rid);
		this->set_payload(payload);
	}
};

	
	int SendRequest(int req= HttpRequestNumber::HBReq);
	void process_all_unsent_from_sqlite(int par1);
	int send_email(const void*payload, int payload_len);

#endif