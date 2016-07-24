package com.mkr.unidriver.hos;

import com.mkr.models.MyLocation;
import com.mkr.models.RecapSummary;
import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.models.proto.CommPackets.PTimeLogRow.Builder;
import com.mkr.service.LocationService;
import com.mkr.service.MkrApp;
import com.mkr.service.MkrNLib;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;

public class HosHelper {

	public static String GetEventStr(int ev)
	{
	   String rv="Off Duty";
	   if(ev==102)
		   rv="Sleeper Birth";
	   else if(ev==103)
		   rv="Driving";
	   else if (ev==104)
		   rv="On Duty";
	   return rv;
	}
	public static int CreateDriverStatus(int ind,int tlid,String comments,int type,int edit_flag,long ts)
	{
		
		Builder b=PTimeLogRow.newBuilder();
		LocationService lsrv=MkrApp.GetLocationService();
		if(lsrv!=null)
		{
		   b.setAddr(lsrv.getCurrentAddress());
		   MyLocation mloc=lsrv.GetMLoc();
		   b.setLat(mloc.Lat);
		   b.setLon(mloc.Lon);
		}
		b.setBoxID(GConst.BoxID);
		b.setEvent(ind);             // must be changed to Event in Jni
		
		b.setLogbookstopid(0);
		b.setComment(comments);
		
		//REMEMber!! All required fields MUST be INitialized here  
		b.setLogTime(ts);
		b.setTLID(tlid);
		b.setSigned(false);
		b.setType(type);         // 0 - manual, 1 - auto, 2 -modified
		
		b.setDriverId("");
		b.setOLT(0);
		
		byte[] ba = b.build().toByteArray();
	
		int rv=MkrNLib.Get().setDriverSts(ba,edit_flag);
		return rv;
	}
	
	public static RecapSummary parseRecapSummary(String str)
	{
		
		RecapSummary rr=new RecapSummary ();
		String[] sa = str.split ("\\|"); //there are 12 characters with special meanings: USE backslash  !!! or split("[.]")
		if(sa.length>0)
		{
			int ind=0,x=0;
			for(String s : sa)
			{
			   if(!s.isEmpty())
			   {
				   switch(ind)
				   {
				   case 0:
					   x=Util.str2int(s);
					   if(x<0)x=0;
					   rr.avail_driving_min=x;
					   break;
				   case 1:
					   x=Util.str2int(s);
					   if(x<0)x=0;
					   rr.avail_onduty_min=x;
					   break;
				   case 2:
					   rr.max_onduty=Util.str2int(s);
					   break;
				   case 3:
					   rr.max_driving=Util.str2int(s);
					   break;
				   case 4:
					   x=Util.str2int(s);
					   if(x<0)x=0;
					   rr.avail_cycle=x;
					   break;
				   case 5:
					   rr.MaxCycle=Util.str2int(s);
					   break;
				   }
			   }
			   ind++;
			}
		}
		return rr;
	}
	
	
}
