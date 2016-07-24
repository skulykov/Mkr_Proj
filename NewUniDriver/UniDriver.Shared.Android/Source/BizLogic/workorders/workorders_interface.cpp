#include "workorders_interface.h"

bool  getWoiList(std::vector<PWorkOrderItem*> &v)
{
	
	for (int i = 0; i < 10; i++)
	{
		PWorkOrderItem *wo = new PWorkOrderItem();
		wo->set_id(i + 1);
		wo->set_name("My Name");
		wo->set_createdts(1456075016058+(i+2)*4000);
		v.push_back(wo);
	}

	return 1;
}

void CleanUP_WOList(std::vector<PWorkOrderItem*> &v)
{
	for (PWorkOrderItem *wo : v)
	{
		if (wo)
			delete wo;
	}
	v.clear();
}