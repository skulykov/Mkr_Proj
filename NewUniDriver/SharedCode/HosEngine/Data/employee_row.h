#ifndef employee_row_H
#define employee_row_H

#include <string>
#include "my_time.h"
#include "SharedObjects.h"

#include "my_proto_comm_lite.pb.h"

using namespace std;
using namespace my_comm_proto;

namespace HosEngine
{
	class EmployeeRow:public PEmployeeRow
	{
	public:
		//std::string ID = "";
		//std::string DriverName = "";
		//std::string UserName = "";
		//std::string Password = "";
		//std::string License = "";
		//std::string Country = "";
		//bool AutoLogin = false;
		//std::string Signature = "";
		//std::string Domain = "";
		//HOSCYCLE Cycle = HOSCYCLE::Canadian_70hr_7day;

		//int HOSExcept = 0, ApplyDTS = 0; //applyDTS - apply Day Time Savings
		//float TZ = 0; //TimeZone
		//			  //internal LocationInfoRow Home = new LocationInfoRow();
		//			  // DataNode Members
	public:
		bool haveSend = true;
		bool isFromServer = false;
		bool auto_login = false;
		
		/*std::string HomeAddress = "";
		bool IsSupervisor = false;
		std::string State = "";
		std::string OrgName = "", OrgAddr = "";
		std::string Email = "";*/

		EmployeeRow();
		void init(const std::string &id, const std::string &driverName, const std::string &username, const std::string &pw, const std::string &license, bool autoLogin, const std::string &homeAddress, bool isSupervisor, const std::string &state = "");
		EmployeeRow(const std::string &id, const std::string &driverName, const std::string &username, const std::string &pw, const std::string &license, bool autoLogin, const std::string &homeAddress, bool isSupervisor, const std::string &state = "");

		EmployeeRow(const std::string &id, const std::string &driverName, const std::string &username, const std::string &pw, const std::string &license, bool autoLogin, const std::string &homeAddress, bool isSupervisor, const std::string &signature, const std::string &domain, HOSCYCLE cycle, const std::string &state = "") ;
		void SetDomain(const std::string &s);
		void SetSignature(const std::string &v);
		void SetOrg(const std::string &name, const std::string &addr);
		void SetCycle(HOSCYCLE v);
		void SetHOSExcept(int v);
		int GetHOSExcept();

		void SetTimeZone(float v);
		float GetTimeZone();

		void SetApplyDTS(int v);
		int GetApplyDTS();

		void SetHaveSent(bool v);
		void SetAutoLogin(bool v);
		bool IsAutoLogin() { return auto_login; }
		std::string ToString();

		//virtual int GetHashCode() override;
	};

}

#endif