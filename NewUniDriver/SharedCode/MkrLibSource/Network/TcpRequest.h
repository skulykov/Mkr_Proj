#ifndef MKR_TCPREQUEST_H_INCLUDED
#define MKR_TCPREQUEST_H_INCLUDED


//#include "juce_Socket.h"
#include "Convert.h"
#include "Dbg.h"

#include "my_proto_comm_lite.pb.h"

using namespace std;
using namespace my_comm_proto;


typedef unsigned char  byte;
typedef unsigned int  uint;

class TcpRequestObj;
typedef void(*request_cb_t)(void *o);

using namespace std;

enum HttpRequestNumber
{
	Login = 1,
	RegisterMD = 2,
	HBReq = 10,

	SendTLR = 20,
	SendTimeLogs = 21,
	SendEmpRow = 23,

	StoreTokenToServer = 40,

	Emails = 41,
	SearchRequest = 42,

	SendHosAlert = 43,
	SendHosRecap = 44,

	GetActivities = 50,
	UpdateActivites = 51,
	SendMsgFromMobile = 53,
	ReadMsgFromServer = 54,
	MarkMsgAsSent = 55,

	Request4Categories = 80,
	Send4VeicleInfo = 81,
	SendTripInfo = 82,
	SendAttRequest = 83,
	SendAttRequest4CatIds = 84,
	Request4HosTimeLogs = 85,
	Request4HosInspReports = 86,
	SendInspectionReports = 100   // from WebStreamRequest
};


#if 0

//class TcpRequestHeader
//{
//private:
//	int size;
//	byte *buffer;
//	int Write7BitEncodedInt(int value, byte *buf, int index) {
//		// Write out an int 7 bits at a time. The high bit of the byte,
//		// when on, tells reader to continue reading more bytes.
//		uint v = (uint)value; // support negative numbers
//		while (v >= 0x80) {
//			buf[index++] = (byte)(v | 0x80);//Write((byte)(v | 0x80));
//			v >>= 7;
//		}
//		buf[index++] = (byte)v;//Write((byte)v);
//		return index;
//	}
//
//	int Read7BitEncodedInt() {
//		// Read out an int 7 bits at a time. The high bit
//		// of the byte when on means to continue reading more bytes.
//		int count = 0;
//		int shift = 0;
//		byte b;
//		do {
//			//b = ReadByte();
//			count |= (b & 0x7F) << shift;
//			shift += 7;
//		} while ((b & 0x80) != 0);
//		return count;
//	}
//
//public:
//	byte req;//Identifier=0;
//	int cid;
//	int rid;
//	string user;
//	string json;
//
//	TcpRequestHeader(HttpRequestNumber _req, string _json)
//	{
//		req = (byte)_req;
//		size = 0;
//		buffer = 0;
//
//		populateUser();
//		this->json = _json;
//	}
//	void populateUser()
//	{
//		rid = 1032;
//		cid = 3666;
//		user = "**";
//	}
//	byte *GetBuffer() { return buffer; }
//	int Size() { return size; }
//	~TcpRequestHeader()
//	{
//		if (buffer != 0)
//			delete buffer;
//		user.clear();
//		json.clear();
//	}
//	void serialize()
//	{
//		int len = 9;     // req+cid+rid
//		len += user.length();
//		len += 4;   // for string length ( might be 1-4 bytes)
//		len += json.length();
//		len += 4; //// for string length ( might be 1-4 bytes)
//		buffer = new byte[len + 4]; // the first 4 bytes for total length
//
//		byte *pBuf = buffer + 4;
//
//		int index = 0;
//		pBuf[index++] = req;
//		Convert::Int32ToBytes(cid, pBuf, index);
//		index += 4;
//		Convert::Int32ToBytes(rid, pBuf, index);
//		index += 4;
//
//		/*The C# BinaryWriter/Reader behavior uses, if I recall correctly, the 8th bit to signify where the last byte of the count is.
//		BinaryWriter.Write7BitEncodedInt Method
//		The integer of the value parameter is written out seven bits at a time, starting with the seven least-significant bits. The high bit of a byte indicates whether there are more bytes to be written after this one.
//		If value will fit in seven bits, it takes only one byte of space. If value will not fit in seven bits, the high bit is set on the first byte and written out. value is then shifted by seven bits and the next byte is written. This process is repeated until the entire integer has been written.
//
//		*/
//		len = user.length();
//		//Convert::Int32ToBytes(len, pBuf, index);
//		//index += 4;
//		index = Write7BitEncodedInt(len, pBuf, index);
//		const char*  pUser = user.c_str();
//		memcpy(&pBuf[index], (const char*)pUser, len);
//		index += len;
//
//
//		len = json.length();
//		//Convert::Int32ToBytes(len, pBuf, index);
//		//index += 4;
//		index = Write7BitEncodedInt(len, pBuf, index);
//
//		const char*  pJson = json.c_str();
//		memcpy(&pBuf[index], (const char*)pJson, len);
//		index += len;
//
//		size = index;
//
//		Convert::Int32ToBytes(size, buffer, 0);  // put size at the beginning
//		size += 4;
//	}
//};
//
//
//class TcpRequestObj
//{
//private:
//	request_cb_t cb;
//	string respStr;
//public:
//	void *parObj;
//	TcpRequestHeader *hdr;
//	TcpRequestObj(request_cb_t cb, HttpRequestNumber req, string json)
//	{
//		this->cb = cb;
//		respStr = "";
//		hdr = new TcpRequestHeader(req, json);
//		parObj = 0;
//	}
//	~TcpRequestObj()
//	{
//		if (hdr != 0)
//			delete hdr;
//		respStr.clear();
//	}
//	void SetResponse(byte *b, int len)
//	{
//		respStr = string((char*)b, len);
//	}
//	string GetResponse() { return respStr; }
//	void ExecuteCB()
//	{
//		if (cb != 0)
//			cb(this);
//	}
//};
#else
class TcpRequestObj
{
private:
	request_cb_t cb;
	string request;
	string response;
public:
	void *parObj;
	
	TcpRequestObj(request_cb_t cb, HttpRequestNumber req, string payload)
	{
		this->cb = cb;
		this->response = "";
		this->parObj = 0;

		CommPacket commP;
		commP.set_req(req);
		commP.set_rid(1024);
		commP.set_cid(3501);
		commP.set_user("SergeyK");
		commP.set_payload(payload);
		this->request= commP.SerializeAsString();
	}
	~TcpRequestObj()
	{
		request.clear();
		response.clear();
	}
	void SetResponse(byte *b, int len)
	{
		response = string((char*)b, len);
	}
	string &GetRequest() { return request; }
	string &GetResponse() { return response; }
	void ExecuteCB()
	{
		if (cb != 0)
			cb(this);
	}
};
#endif



#endif
