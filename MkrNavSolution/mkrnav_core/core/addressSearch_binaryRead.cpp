
////---------------------Affress Search--------------------------
////https://developers.google.com/protocol-buffers/docs/encoding
//
//typedef struct{
//	string building;
//	string street;
//	string city;
//	string province;
//	string postalcode;
//}FindAddress_T;
//typedef struct{
//	string name;
////	__int64 id;
//	int x,y;
//}Found_Entity_T;
//typedef struct
//{
//	Found_Entity_T city;
//	Found_Entity_T street;
//	Found_Entity_T building;
//}Found_Address_T;
static vector<Found_Address_T*> found_addresses;

bool readCityIndex(CodedInputStream* input,int length,FindAddress_T *a,Found_Entity_T *e)
{
	uint32_t tag;
	bool found=false;
	uint32_t tile_x,tile_y;  //  tile of 31 zoom use get31LongitudeX(int tileX) to get location
	while ((tag = input->ReadTag()) != 0) {
		int fNum=WireFormatLite::GetTagFieldNumber(tag);
		switch (fNum) 
		{
		case CityIndex::kCityTypeFieldNumber:
				uint32_t type;
				DO_((WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &type)));
				if(type!=0)
				{
				}
			break;
		case CityIndex::kNameFieldNumber:
			{
				std::string s;
				WireFormatLite::ReadString(input, &s);
				toLowerCase(s);
				//list.push_back(s);
				//if(equalsIgnoreCase(s,a->city)
				if(s!=""&&s==a->city)
				{
					found=true;
					e->name=s;
				}
			}
			break;
		case CityIndex::kXFieldNumber:
			DO_((WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &tile_x)));
			if(found)
				e->x=tile_x;
			break;
		case CityIndex::kYFieldNumber:
			DO_((WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &tile_y)));
			if(found)
				e->y=tile_y;
			break;
			default: 
			{
				if (WireFormatLite::GetTagWireType(tag) == WireFormatLite::WIRETYPE_END_GROUP) {
					return false;
				}
				if (!skipUnknownFields(input, tag)) {
					return false;
				}
				break;
			}
		}
		
	}
	return found;
}
bool readBuildingIndex(CodedInputStream* input,int length,FindAddress_T *a,vector<Found_Entity_T> *v)
{
	uint32_t tag;
	//bool found=false;
	//int interpolation;
	//int x,y;//,x2,y2;
	//string name="";
	Found_Entity_T b;
	while ((tag = input->ReadTag()) != 0) {
		int fNum=WireFormatLite::GetTagFieldNumber(tag);
		switch (fNum) 
		{
		case BuildingIndex::kNameFieldNumber:
			{	
				WireFormatLite::ReadString(input, &b.name);
				//list.push_back(s);
				/*if(s!=""&&s==a->building)
				{
					found=true;
					faddr->building.name=s;
				}*/
			}
			break;
		//case BuildingIndex::kName2FieldNumber:
		//	{
		//		std::string s;
		//		WireFormatLite::ReadString(input, &s);
		//		//list.push_back(s);
		//		if(s!=""&&s==a->building)
		//		{
		//			found=true;
		//		}
		//	}
		//	break;
		/*case BuildingIndex::kInterpolationFieldNumber:
			{
				DO_((WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_SINT32>(input, &interpolation)));
				if(found)
					faddr->building.x=x;
			}
			break;*/
		case BuildingIndex::kXFieldNumber:
			DO_((WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_SINT32>(input, &b.x)));
			/*if(found)
					faddr->building.x=x;*/
			break;
		case BuildingIndex::kYFieldNumber:
			DO_((WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_SINT32>(input, &b.y)));
			/*if(found)
				faddr->building.x=x;*/
			break;
		/*case BuildingIndex::kX2FieldNumber:
			DO_((WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_SINT32>(input, &x2)));
			break;
		case BuildingIndex::kY2FieldNumber:
			DO_((WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_SINT32>(input, &y2)));
			break;*/

		default: 
			{
				if (WireFormatLite::GetTagWireType(tag) == WireFormatLite::WIRETYPE_END_GROUP) {
					return false;
				}
				if (!skipUnknownFields(input, tag)) {
					return false;
				}
				break;
			}
		}
		
	}
	v->push_back(b);
	return true;
}

bool readStreetIndex(CodedInputStream* input,int length,FindAddress_T *a,Found_Address_T *faddr)
{
	uint32_t tag;
	bool found_street=false;
	
	int x,y;
	vector<Found_Entity_T>  buildings;
	while ((tag = input->ReadTag()) != 0) {
		int fNum=WireFormatLite::GetTagFieldNumber(tag);
		switch (fNum) 
		{
		case StreetIndex::kNameFieldNumber:
			{
				std::string s;
				WireFormatLite::ReadString(input, &s);
				toLowerCase(s);
				
				if(s!=""&&s.find(a->street)!=-1)
				{
					found_street=true;
					faddr->street.name=s;
				}
			}
			break;
		case StreetIndex::kXFieldNumber:
			DO_((WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_SINT32>(input, &x)));
			if(found_street)
				faddr->street.x=x;
			break;
		case StreetIndex::kYFieldNumber:
			DO_((WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_SINT32>(input, &y)));
			if(found_street)
				faddr->street.y=y;
			break;
		case StreetIndex::kBuildingsFieldNumber:
			{
				if(found_street)
				{
					int length;
					WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_INT32>(input, &length);
					int oldLimit = input->PushLimit(length);
					readBuildingIndex(input,length,a,&buildings); // insert all buildings numbers
					input->PopLimit(oldLimit);
					break;
				}
				//default and skip
			}
			
			default: 
			{
				if (WireFormatLite::GetTagWireType(tag) == WireFormatLite::WIRETYPE_END_GROUP) {
					return false;
				}
				if (!skipUnknownFields(input, tag)) {
					return false;
				}
				break;
			}
		}
		
	}
	
	if(found_street&&buildings.size()>0)
	{   tag=buildings.size();
		int n=0;
			if(a->building!="")
				n=atoi(a->building.c_str());
		int *buf=new int[tag];  
		for(int i=0;i<tag;i++)
		{
			Found_Entity_T e =buildings[i];
			int n1=atoi(e.name.c_str());
			buf[i]=n-n1;
		}
		n=0x7fffffff;
		int index=0;
		for(int i=0;i<tag;i++)
		{
			if(n>abs(buf[i]))
			{
				n=buf[i];
				index=i;
			}
		}
		Found_Entity_T e =buildings[index];
		faddr->building.name=e.name;
		faddr->building.x=e.x;
		faddr->building.y=e.y;

		delete[] buf;
	}
	return found_street;
}


bool readCityBlockIndex(CodedInputStream* input,int length,FindAddress_T *a,Found_Address_T *faddr)
{
	uint32_t tag;
	bool found=false;
	while ((tag = input->ReadTag()) != 0) {
		int fNum=WireFormatLite::GetTagFieldNumber(tag);
		switch (fNum) 
		{
		case CityBlockIndex::kShiftToCityIndexFieldNumber:
				uint32_t x;
				readInt(input, &x);  // fixed32 !!!!!
				//DO_((WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &x)));
				break;
		
		case CityBlockIndex::kStreetsFieldNumber:  //StreetIndex streets = 12;
				{
					int length;
					WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_INT32>(input, &length);
					int oldLimit = input->PushLimit(length);
					found=readStreetIndex(input,length,a,faddr);
					input->PopLimit(oldLimit);
				}
			break;
			default: 
			{
				if (WireFormatLite::GetTagWireType(tag) == WireFormatLite::WIRETYPE_END_GROUP) {
					return false;
				}
				if (!skipUnknownFields(input, tag)) {
					return false;
				}
				break;
			}
		}
		
	}
	return found;
}

bool readCitiesIndex(CodedInputStream* input,int length,FindAddress_T *a,Found_Address_T *faddr)
{
	uint32_t tag;
	uint32_t type;
	int city_index=0;
	int index_found=-1;
	int block_index=0;
	while ((tag = input->ReadTag()) != 0) {
		int fNum=WireFormatLite::GetTagFieldNumber(tag);
		switch (fNum) 
		{
		case FlcNavAddressIndex_CitiesIndex::kTypeFieldNumber:
			{
				
				//readInt(input, &type);
				DO_((WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &type)));
				if(type!=0)
				{
				}
			}
			break;
			// the following two fields are not mixed
			// the list of the cities and the blocks are synchronized by the order (so even empty block will be written)
		case FlcNavAddressIndex_CitiesIndex::kCitiesFieldNumber:  //repeated CityIndex cities = 5;
			{
				int length;
				WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_INT32>(input, &length);
				int oldLimit = input->PushLimit(length);
				
				bool found=readCityIndex(input,length,a,&faddr->city);
				if(found&&index_found==-1)
				{
					//found_addresses.push_back(*a);
					index_found=city_index;
				}
				input->PopLimit(oldLimit);
				city_index++;
			}
			break;
		case FlcNavAddressIndex_CitiesIndex::kBlocksFieldNumber: //repeated CityBlockIndex blocks = 7;
			{
				if(index_found!=-1&&index_found==block_index)
				{
					int length;
					WireFormatLite::ReadPrimitive<int32_t, WireFormatLite::TYPE_INT32>(input, &length);
					int oldLimit = input->PushLimit(length);
					readCityBlockIndex(input,length,a,faddr);
					
					input->PopLimit(oldLimit);
					block_index++;
					break;
				}
				//if city is not found we go strait to default and skip this field !!!!!!
				block_index++;
			}
		default: 
			{
				if (WireFormatLite::GetTagWireType(tag) == WireFormatLite::WIRETYPE_END_GROUP) {
					return false;
				}
				if (!skipUnknownFields(input, tag)) {
					return false;
				}
				break;
			}
		}
	}
}
bool readAddrInd(CodedInputStream* input, uint32_t len,FindAddress_T *a) {
	uint32_t tag;
	while ((tag = input->ReadTag()) != 0) {
		int fNum=WireFormatLite::GetTagFieldNumber(tag);
		switch (fNum) {
		case FlcNavAddressIndex::kNameFieldNumber: 
		{                 //1
			std::string s;
			WireFormatLite::ReadString(input, &s);
			//province name!!!!!!!
			if(s!="")
			{
			}
			break;
		}
		//case FlcNavAddressIndex::kNameEnFieldNumber:
		//	{                
		//		std::string s;
		//		WireFormatLite::ReadString(input, &s);
		//		//list.push_back(s);
		//		if(s!="")
		//		{
		//		}
		//		break;
		//	}
		//	break;
		case FlcNavAddressIndex::kCitiesFieldNumber:   // repeated CitiesIndex cities = 6;
			{
				uint32_t length;
				readInt(input, &length);
				int filePointer = input->getTotalBytesRead();
				int oldLimit = input->PushLimit(length);
				Found_Address_T *faddr=new Found_Address_T();
				readCitiesIndex(input,length,a,faddr);
				if(faddr->city.name!="")
				{
					found_addresses.push_back(faddr);
					/*double lat=MapUtils::get31LatitudeY(faddr.city.y);
					double lon=MapUtils::get31LongitudeX(faddr.city.x);
					if(lat!=lon)
					{
						int sx = MapUtils::get31TileNumberX(-79.12345);
						int sy = MapUtils::get31TileNumberY(43.6789);
						lat=MapUtils::get31LatitudeY(sy);
						lon=MapUtils::get31LongitudeX(sx);
						if(lat!=lon)
						{
						}

					}*/
					/*
					uint32_t cx=faddr->city.x;
					uint32_t cy=faddr->city.y;
					//streetBuilder.setX((sx - cx) >> 7);
					//streetBuilder.setY((sy - cy) >> 7);
					int sx = faddr->street.x*128+cx;
					int sy=faddr->street.y*128+cy;
					int bx=faddr->building.x*128+sx;
					int by=faddr->building.y*128+sy;
					double lat=MapUtils::get31LatitudeY(by);
					double lon=MapUtils::get31LongitudeX(bx);
					if(lat!=lon)
					{
					}
					*/

				}
				input->PopLimit(oldLimit);
				input->Seek(filePointer + length);
			}
			break;
		case FlcNavAddressIndex::kNameIndexFieldNumber: //optional FlcNavAddressNameIndexData nameIndex = 7; 
			break;
		case FlcNavAddressIndex::kBoundariesFieldNumber:
			 //uint32_t length;
			// uint32_t left,right,top,bottom;
			// WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &left);
			// WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &right);
			// WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &top);
			// WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &bottom);
			//		
			//break;  //skip it for now
		default: {
			if (WireFormatLite::GetTagWireType(tag) == WireFormatLite::WIRETYPE_END_GROUP) {
				return false;
			}
			if (!skipUnknownFields(input, tag)) {
				return false;
			}
			break;
		}
		}
	}
	return true;
}
bool initAddressStructure(CodedInputStream* input, BinaryMapFile* file,FindAddress_T *addr ) {
	
	uint32_t tag;
	uint32_t versionConfirm = -2;

	
	while ((tag = input->ReadTag()) != 0) {
		switch (WireFormatLite::GetTagFieldNumber(tag)) {
		// required uint32_t version = 1;
		case FlcNavStructure::kVersionFieldNumber: {
			DO_((WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &file->version)));
			break;
		}
		case FlcNavStructure::kDateCreatedFieldNumber: {
			DO_((WireFormatLite::ReadPrimitive<uint64_t, WireFormatLite::TYPE_UINT64>(input, &file->dateCreated)));
			break;
		}
		case FlcNavStructure::kAddressIndexFieldNumber: {
			//MapIndex mapIndex;
			uint32_t length;
			readInt(input, &length);
			int filePointer = input->getTotalBytesRead();//mapIndex.filePointer = input->getTotalBytesRead();
			int oldLimit = input->PushLimit(length);
		    readAddrInd(input,length,addr);//	readMapIndex(input, &mapIndex);
			input->PopLimit(oldLimit);
			input->Seek(filePointer + length);
			//file->mapIndexes.push_back(mapIndex);
			//file->indexes.push_back(&file->mapIndexes.back());
			break;
		}
		
		case FlcNavStructure::kVersionConfirmFieldNumber: {
			DO_((WireFormatLite::ReadPrimitive<uint32_t, WireFormatLite::TYPE_UINT32>(input, &versionConfirm)));
			break;
		}
		default: {
			if (WireFormatLite::GetTagWireType(tag) == WireFormatLite::WIRETYPE_END_GROUP) {
				return true;
			}
			if (!skipUnknownFields(input, tag)) {
				return false;
			}
			break;
		}
		}
	}
	if (file->version != versionConfirm) {
		LOGE( "Corrupted file. It should be ended as it starts with version");
		return false;
	}
	if (file->version != MAP_VERSION) {
		LOGE( "Version of the file is not supported.");
		return false;
	}
	return true;
}
//------------------------------------------------------------

void clearFoundAddresses()
{
	if(found_addresses.size()>0)
	{
		
		found_addresses.clear();
	}
}

vector<Found_Address_T*> & getAddressSearchResult()
{
	LOGI("getAddressSearchResult :%d ... ",found_addresses.size());
	return found_addresses;
}
