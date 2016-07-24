#include <iostream>
#include <exception>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(ANDROID)
#include <unistd.h>
#endif
#include <errno.h>    // errno, ENOENT, EEXIST
#include <fcntl.h>
#include <stdarg.h>
#include "../MkrHeader.h"  // int64 problem here !! moved it here after Kompex definitions
#include "Dbg.h"
short log_initialized = 0;
std::string log_path = "";
#if defined(ANDROID)
void createLogPath()
{
	char dir[128 * 2];
	memset(dir, 0, sizeof(dir));
	strcpy(dir, "/mnt/sdcard/unidriver/");
	int rv = -1;
	struct stat st = { 0 };
	if (stat(dir, &st) == -1) {
		rv = mkdir(dir, 0777);
	}
	//strcpy and strcat are used to copy and concatenate strings to an allocated char array.
	strcat(dir, "log/");
	st = { 0 };
	if (stat(dir, &st) == -1) {
		rv = mkdir(dir, 0777);
	}

	//strcat(dir, "/");

	strcat(dir, "mkr_log.log");
	log_path = std::string(dir);

	log_initialized=1;
}

void write_to_log_file(void *data, int length)
{
	int   fd;
	if (!(log_initialized == 1 && log_path.length() > 0))
	{
		createLogPath();
	}
	if (log_initialized == 1)
	{
		fd = open(log_path.c_str(), O_CREAT + O_WRONLY + O_APPEND, 0666);
		if (fd >= 0) {

			write(fd, data, length);
			close(fd);
		}
	}

}
#else
#include "stdio.h"
void WriteLogFile(const char* szString)
{
	

	FILE* pFile = fopen("logFile.txt", "a");
	fprintf(pFile, "%s\n", szString);
	fclose(pFile);



}
#endif

void sk_log_to_file(const char *format, ...)
{

	va_list ap;

	va_start(ap, format);
	char msg[1024];// = { 0 };
	vsprintf(msg, format, ap);  //The buffer should be large enough to contain the resulting string.

#if defined(ANDROID) 
								LOGI(msg);
	write_to_log_file((void*)msg, strlen(msg));
#else
	WriteLogFile(msg);
	printf("::%s", msg);
#endif
	va_end(ap);
}

