#include "employee_row.h"

namespace HosEngine
{
	EmployeeRow::EmployeeRow()
	{
	}
	void EmployeeRow::init(const std::string &id, const std::string &driverName, const std::string &username, const std::string &pw, const std::string &license, bool autoLogin, const std::string &homeAddress, bool isSupervisor, const std::string &state)
	{
		this->set_id(id);
		this->set_drivername(driverName);
		this->set_username(username);
		this->set_password(pw);
		this->set_license(license);
		//this->set_autologin(autoLogin);
		this->set_homeaddress(homeAddress);
		this->set_issupervisor(isSupervisor);
		this->set_state(state);
	}
	EmployeeRow::EmployeeRow(const std::string &id, const std::string &driverName, const std::string &username, const std::string &pw, const std::string &license, bool autoLogin, const std::string &homeAddress, bool isSupervisor, const std::string &state)
	{
		this->init(id, driverName, username, pw, license, autoLogin, homeAddress, isSupervisor, state);
	}

	EmployeeRow::EmployeeRow(const std::string &id, const std::string &driverName, const std::string &username, const std::string &pw, const std::string &license, bool autoLogin, const std::string &homeAddress, bool isSupervisor, const std::string &signature, const std::string &domain, HOSCYCLE cycle, const std::string &state)
	{
		this->init(id, driverName, username, pw, license, autoLogin, homeAddress, isSupervisor, state);

		this->set_signature(signature);
		this->set_domain(domain);
		this->set_cycle((int)cycle);
		
	}

	void EmployeeRow::SetDomain(const std::string &s)
	{
		this->set_domain( s);
	}

	void EmployeeRow::SetSignature(const std::string &v)
	{
		this->set_signature( v);
	}

	void EmployeeRow::SetOrg(const std::string &name, const std::string &addr)
	{
		this->set_orgaddr(addr);
		this->set_orgname(name);
	}

	void EmployeeRow::SetCycle(HOSCYCLE v)
	{
		this->set_cycle ((int)v);
	}

	void EmployeeRow::SetHOSExcept(int v)
	{
		this->set_hosexcept(v);
	}

	int EmployeeRow::GetHOSExcept()
	{
		return this->hosexcept();
	}

	void EmployeeRow::SetTimeZone(float v)
	{
		this->set_tz(v);
	}

	float EmployeeRow::GetTimeZone()
	{
		return this->tz();
	}

	void EmployeeRow::SetApplyDTS(int v)
	{
		this->set_applydts(v);
	}

	int EmployeeRow::GetApplyDTS()
	{
		return this->applydts();
	}

	void EmployeeRow::SetHaveSent(bool v)
	{
		this->haveSend = v;
	}
	void EmployeeRow::SetAutoLogin(bool v)
	{
		this->auto_login = v;
	}
	std::string EmployeeRow::ToString()
	{
		return this->drivername();
	}
}
