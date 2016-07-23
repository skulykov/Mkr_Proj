#include "files.h"



int delete_all_files(std::string dir)
{
	// These are data types defined in the "dirent" header
	DIR *theFolder = opendir(dir.c_str());
	struct dirent *next_file;
	char filepath[256];

	while ((next_file = readdir(theFolder)) != NULL)
	{
		// build the path for each file in the folder
		sprintf(filepath, "%s/%s", dir.c_str(), next_file->d_name);
		remove(filepath);
	}
	closedir(theFolder);
	return 0;
}

