#include "binaryRead.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
//#include "google/protobuf/wire_format_lite.cc"
#include "proto/flcnav_odb.pb.h"
//#include "flcnav_log.h"
#include "MapUtils.h"

#include <sstream>
#include <string>



#if LEAK_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
//#endif
#endif


using namespace std;
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
using google::protobuf::io::CodedInputStream;
using google::protobuf::io::FileInputStream;
using google::protobuf::internal::WireFormatLite;
//using namespace google::protobuf::internal;


//extern std::map< std::string, BinaryMapFile* > openFiles;
std::map< std::string, BinaryMapFile* > _openFiles;

inline bool _readInt(CodedInputStream* input, uint32_t* sz ){
	uint8_t buf[4];
	if (!input->ReadRaw(buf, 4)) {
		return false;
	}
	*sz = ((buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3] << 0));
	return true;
}

bool _skipFixed32(CodedInputStream* input) {
	uint32_t sz;
	if (!_readInt(input, &sz)) {
		return false;
	}
	return input->Skip(sz);
}

bool _skipUnknownFields(CodedInputStream* input, int tag) {
	if (WireFormatLite::GetTagWireType(tag) == WireFormatLite::WIRETYPE_FIXED32_LENGTH_DELIMITED) {
		if (!_skipFixed32(input)) {
			return false;
		}
	} else if (!WireFormatLite::SkipField(input, tag)) {
		return false;
	}
	return true;
}

void clearFoundAddresses();
bool initAddressStructure(CodedInputStream* input, BinaryMapFile* file,FindAddress_T *addr ) ;
vector<Found_Address_T*> & getAddressSearchResult();

LatLon_T getLocation(Found_Address_T *faddr)
{
	LatLon_T ll;
	uint32_t cx=faddr->city.x;
	uint32_t cy=faddr->city.y;
					//streetBuilder.setX((sx - cx) >> 7);
					//streetBuilder.setY((sy - cy) >> 7);
	int sx = faddr->street.x*128+cx;
	int sy=faddr->street.y*128+cy;
	int bx=faddr->building.x*128+sx;
	int by=faddr->building.y*128+sy;
	ll.latitude=MapUtils::get31LatitudeY(by);
	ll.longitude=MapUtils::get31LongitudeX(bx);
	return ll;				
}
#if 1
vector<string> initBinaryMapFile4Address(std::string inputName,FindAddress_T *addr) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	vector<string> res;
	std::map<std::string, BinaryMapFile*>::iterator iterator;
	if ((iterator = _openFiles.find(inputName)) != _openFiles.end()) {
		delete iterator->second;
		_openFiles.erase(iterator);
	}
	
	

#if defined(_WIN32)
	int fileDescriptor = open(inputName.c_str(), O_RDONLY | O_BINARY);
	int routeDescriptor = open(inputName.c_str(), O_RDONLY | O_BINARY);
#else
	int fileDescriptor = open(inputName.c_str(), O_RDONLY);
	int routeDescriptor = open(inputName.c_str(), O_RDONLY);
#endif
	if (fileDescriptor < 0 || routeDescriptor < 0 || routeDescriptor == fileDescriptor) {
		LOGE( "File could not be open to read from C : %s", inputName.c_str());
		return res;
	}
	BinaryMapFile* mapFile = new BinaryMapFile();
	mapFile->fd = fileDescriptor;
	mapFile->routefd = routeDescriptor;
	FileInputStream input(fileDescriptor);
	input.SetCloseOnDelete(false);
	CodedInputStream cis(&input);
	cis.SetTotalBytesLimit(INT_MAX, INT_MAX);

	if (!initAddressStructure(&cis, mapFile,addr)) {
		LOGE( "File not initialised : %s", inputName.c_str());
		delete mapFile;
		return res;
	}
	delete mapFile;
	vector<Found_Address_T*> & addresses = getAddressSearchResult();
	if(addresses.size()>0)
	{
		for(int i=0;i<addresses.size();i++)
		{
			Found_Address_T *a=addresses[i];
			LatLon_T ll=getLocation(a);

			std::stringstream msg;
			msg << a->city.name <<"," << a->street.name << ","  << a->building.name << "," << ll.latitude << "," <<ll.longitude <<"|" ;
			LOGI("addr found : msg =%s",msg.str().c_str());
			res.push_back(msg.str()); 

			delete a;  // we do not need it anymore!!!!!!!!!!!!!!!!!
		}

	}
	else
	{
		LOGI("addr NOT found : ... ");
	}

	/*mapFile->inputName = inputName;
	_openFiles.insert(std::pair<std::string, BinaryMapFile*>(inputName, mapFile));
	return mapFile;*/

	
	clearFoundAddresses();
	return res;

}
#else
BinaryMapFile* initBinaryMapFile4Address(std::string inputName) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	BinaryMapFile* mapFile=0;
	std::map<std::string, BinaryMapFile*>::iterator iterator;
	if ((iterator = openFiles.find(inputName)) != openFiles.end()) {
		//delete iterator->second;  // delete pointer to BinaryMapFile
		//openFiles.erase(iterator); // delete record
		mapFile=iterator->second;
		if(mapFile!=0)
		{
			//int fileDescriptor=mapFile->fd;
			/*FileInputStream input(fileDescriptor);
			input.SetCloseOnDelete(false);
			CodedInputStream cis(&input);
			cis.SetTotalBytesLimit(INT_MAX, INT_MAX);*/
			CodedInputStream *cis=(CodedInputStream*)mapFile->cis;
			if (!initAddressStructure(cis, mapFile)) {
				LOGE( "File not initialised : %s", inputName.c_str());
				//delete mapFile;
				return NULL;
			}

		}
	}
	else
	{
		#if defined(_WIN32)
			int fileDescriptor = open(inputName.c_str(), O_RDONLY | O_BINARY);
			int routeDescriptor = open(inputName.c_str(), O_RDONLY | O_BINARY);
		#else
			int fileDescriptor = open(inputName.c_str(), O_RDONLY);
			int routeDescriptor = open(inputName.c_str(), O_RDONLY);
		#endif
			if (fileDescriptor < 0 || routeDescriptor < 0 || routeDescriptor == fileDescriptor) {
				LOGE( "File could not be open to read from C : %s", inputName.c_str());
				return NULL;
			}
			mapFile = new BinaryMapFile();
			mapFile->fd = fileDescriptor;
			mapFile->routefd = routeDescriptor;
			FileInputStream input(fileDescriptor);
			input.SetCloseOnDelete(false);
			CodedInputStream cis(&input);
			cis.SetTotalBytesLimit(INT_MAX, INT_MAX);

			if (!initAddressStructure(&cis, mapFile)) {
				LOGE( "File not initialised : %s", inputName.c_str());
				delete mapFile;
				return NULL;
			}
			mapFile->inputName = inputName;
			openFiles.insert(std::pair<std::string, BinaryMapFile*>(inputName, mapFile));
	}
	return mapFile;
}
#endif