/*
    open source routing machine
    Copyright (C) Dennis Luxen, 2010


 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#ifdef __APPLE__
#include <signal.h>
#endif

#include <climits>
#include <cstring>
#include <string>

using namespace std;

#define VERBOSE(x) x
#define VERBOSE2(x)
#ifdef STXXL_VERBOSE_LEVEL
#undef STXXL_VERBOSE_LEVEL
#endif
#define STXXL_VERBOSE_LEVEL -100

#define INFO(x) do {std::cout << "[info " << __FILE__ << ":" << __LINE__ << "] " << x << std::endl;} while(0);
#define ERR(x) do {std::cerr << "[error " << __FILE__ << ":" << __LINE__ << "] " << x << std::endl; exit(-1);} while(0);
#define WARN(x) do {std::cerr << "[warn " << __FILE__ << ":" << __LINE__ << "] " << x << std::endl;} while(0);
#ifdef NDEBUG
#define DEBUG(x)
#define GUARANTEE(x,y)
#else
#define DEBUG(x) do {std::cout << "[debug " << __FILE__ << ":" << __LINE__ << "] " << x << std::endl;} while(0);
#define GUARANTEE(x,y) do { {do{ if(false == (x)) { ERR(y) } } while(0);} } while(0);
#endif
//#define DELETE(x) do { if(NULL != x) { delete x; x = NULL; } }while(0);

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef HAVE_ROUNDF
#include <boost/math/tr1.hpp>
using namespace boost::math;
#endif

typedef unsigned int NodeID;
typedef unsigned int EdgeID;
typedef unsigned int EdgeWeight;

static const NodeID SPECIAL_NODEID = UINT_MAX;
static const EdgeID SPECIAL_EDGEID = UINT_MAX;

#ifndef _ONLYTYPEDEFS
#include <boost/thread.hpp>
#include <libxml/xmlreader.h>
#include "DataStructures/Util.h"
#endif //_ONLYTYPEDEFS

#include "DataStructures/HashTable.h"
#include "DataStructures/NodeCoords.h"
typedef NodeCoords<NodeID> NodeInfo;
#include "DataStructures/NodeInformationHelpDesk.h"
#include "DataStructures/BinaryHeap.h"
#include "Contractor/Contractor.h"
#include "Contractor/ContractionCleanup.h"
typedef ContractionCleanup::Edge::EdgeData EdgeData;
#include "DataStructures/DynamicGraph.h"

//Fix to make Eclipse 3.7 happy
#ifndef __TIMESTAMP__
#define __TIMESTAMP__ "unknown date"
#endif

#endif /* TYPEDEFS_H_ */
