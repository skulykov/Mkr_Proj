
#include "my_utils.h"
#include "Dbg.h"

#include <string>
#include <iostream>
#include <fstream>

#include <google/protobuf/stubs/logging.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/arena_test_util.h>
#include <google/protobuf/map_lite_unittest.pb.h>
#include <google/protobuf/map_lite_test_util.h>
#include <google/protobuf/test_util_lite.h>
#include <google/protobuf/unittest_lite.pb.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/stubs/strutil.h>

#include "my_proto_comm_lite.pb.h"

using namespace std;
using namespace my_comm_proto;


string readBinaryFile(string fn)
{
	ifstream in(fn, ios::binary);
	in.seekg(0, ios::end);
	int iSize = in.tellg();
	in.seekg(0, ios::beg);

	std::vector<char> pBuff(iSize);

	if (iSize > 0)
		in.read(&pBuff[0], iSize);
	in.close();
	return string(pBuff.data(), iSize);

	
}


const char* fname_in = "/mnt/sdcard/unidriver/items_from_csharp.bin";
int main_proto_read()
{
	CommPacket commP;

	/*fstream input(fname_in, ios::in | ios::binary);
	if (!input) {
		m_printf("items_from_csharp.bin: File not found.  Creating a new file.");
	}
	else if (!commP.ParseFromIstream(&input)) {
		cerr << "Failed to parse address book." << endl;
		return -1;
	}*/
	string str1 = readBinaryFile(fname_in);
	int len1 = str1.length();
	if (len1 == 0)
		return 0;
	commP.ParseFromString(str1);
	int cid = commP.cid();
	string user = commP.user();
	m_printf("parse OK  cid:%d  User:%s", cid, user.c_str());

	string str = commP.payload();
	WorkOrderItems items;
	items.ParseFromString(str);

	int len = items.wo_items_size();
	for (int i = 0; i < len; i++)
	{
		WorkOrderItem it = items.wo_items(i);
		int id = it.id();
		string name = it.name();
		m_printf("WO  id: %d  Name: %s", id, name.c_str());
	}
}