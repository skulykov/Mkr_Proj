// FCNavGLTest.cpp : Defines the entry point for the application.
//


#include "GLSimulator.h"

void test_fcnav() ;

const char *test__argv__[]={
	"-fcnav",
	"-renderingOutputFile=C:\\my\\OSM_Maps\\obf\\1.png", 
	"-zoom=15",
	"-lt=8,4",  //8*256=2132  4*256 = 1128
   // "-lbox=-79.49917316436767,43.85117915386791",
    "-lbox=-79.51239109039306,43.854954695691774",
	"-renderingInputFile=C:\\my\\OSM_Maps\\obf\\ontario.obf",
	"-renderingInputFile=C:\\my\\OSM_Maps\\obf\\basemap.obf",
  //"-renderingStyleFile=C:\\my\\FCNavigation\\DataExtractionOSM\\src\\net\\flcnav\\render\\default.render.xml",
	"-renderingStyleFile=C:\\my\\OSM_Maps\\obf\\render.xml",
    "-imagesBasePathFile=C:\\my\\FCNavigation\\FCNav\\res\\drawable-mdpi"
};
int svk_main(int argc, char **argv);


//void test_fcnav() {
//	int test_argc=9;
//	svk_main(test_argc, (char **)test__argv__);
////	_CrtDumpMemoryLeaks();
//	
//}



