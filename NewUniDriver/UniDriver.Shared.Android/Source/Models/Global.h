#ifndef MKR_GLOBAL_H_
#define MKR_GLOBAL_H_
#include <stdio.h>
#include <stdlib.h>

#include "CommonUtils/Dbg.h"
#include "Definitions.h"

using namespace HosEngine;
class Global
{
	static Global *_instance;
	Global() {}
	//ClsLoginMD *pL=0;
	int did = 0;
	int cid = 0;
	string DrName = "";
	EmployeeRow *prof=0;
public:

	static Global *Get()
	{
		if (_instance == 0)
			_instance = new Global();
		return _instance;
	}
	void SetLogin(ClsLoginMD *pL);
	void SetRegister(PEmployeeRow *pE);
	string GetDriverID();
	int GetDID();
	int GetCID();
	string GetDriverName();
	void CleanUp();

	EmployeeRow *GetCurrentProfile();
	void SetCurrentProfile(EmployeeRow *prof);
	int UpdateCurrentProfile(EmployeeRow *);
};

#endif
