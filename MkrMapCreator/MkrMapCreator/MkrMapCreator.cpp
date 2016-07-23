// MkrMapCreator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common\files.h"



#include <iostream>
#include <exception>


int main_process_pbf(int argc, char** argv);
void generateIndexes(const char *fileName);
//void linked_map_test();
void test_1();
int main(int argc, char** argv)
{
	/*delete_all_files("C:\\my\\m_maps\\mOut");
	main_process_pbf(argc, argv);*/

	

	try {
		//test_1();//linked_map_test();
		generateIndexes("toronto");
	}
	catch (const std::exception &e)
	{
		const char *desc = e.what();
		printf("std exception...%s", desc);
	}
	catch (...)
	{
		printf("all exception...");
		//std::exception_ptr p = std::current_exception();
		//std::clog << (p ? p.__cxa_exception_type()->name() : "null") << std::endl;

	}
	
	getchar(); // Wait for keypress on exit so we can read console output
    return 0;
}


