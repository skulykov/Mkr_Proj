package com.mkr.service;

import java.io.UnsupportedEncodingException;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;


import com.mkr.models.JsonMessageCls;
import com.mkr.models.proto.CommPackets.PEmployeeRow;
import com.mkr.models.proto.CommPackets.PInspectionCategoryRow;
import com.mkr.models.proto.CommPackets.PInspectionCategoryRowList;
import com.mkr.models.proto.CommPackets.PInspectionItemList;
import com.mkr.models.proto.CommPackets.PInspectionRowList;
import com.mkr.models.proto.CommPackets.PRecapRowList;
import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.models.proto.CommPackets.PTimeLogRowList;
import com.mkr.models.proto.CommPackets.PWorkOrderItem;
import com.mkr.models.proto.CommPackets.PWorkOrderList;
import com.mkr.unidriver.LoginActivity;
import com.mkr.unidriver.MessageActivity;
import com.mkr.unidriver.RegisterActivity;
import com.mkr.unidriver.hos.HosActivity;
import com.mkr.unidriver.workorders.WorkOrdersActivity;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;

import android.app.Activity;
import android.util.Log;

public final class MkrNLib 
{
	static MkrNLib _instance=null;
	boolean initialized=false;
	
    public static MkrNLib Get()
    {
    	if(_instance==null)
    		_instance=new MkrNLib();
    	return _instance;
    }
	public void initLibs() {
    	if(!initialized)
    	{
    	try
    	 {
    		 Log.d("LLL","Loading native Libs stlport_shared...");
    		 System.loadLibrary ("UniDriver_Shared_Android");
    		 initialized=true;
    	 }
    	 catch (Throwable e) {Log.d("LLL","EXCEPTION Loading native libs...");}
    	}
     }
     
	 public void loginResp(int v) {
		 Activity a=MkrApp.GetCurrentOnTopActivity();
			if(v==2||v==-2)
			{
				LoginActivity act = (LoginActivity)a;
				if(act!=null)
				{
					act.login_resp(v);
				}
			}else if(v==1||v==-1)
			{
				RegisterActivity act = (RegisterActivity)a;
				if(act!=null)
				{
					act.register_resp(v);
				}
			}
	}
  
	 public List<PTimeLogRow> GetTLRList(long date1,long date2,int flag)
	 {
		 List<PTimeLogRow> list=null;
		 byte[] ba = getTLRList(date1,date2,flag);
		 
		 if(ba!=null&&ba.length>0)
		 {
			 PTimeLogRowList ll=null;
			try {
				ll = PTimeLogRowList.parseFrom(ba);
			} catch (InvalidProtocolBufferException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			 if(ll!=null)
				 list=ll.getListList();
			 
		 }
		 return list;
	 }
	 public PTimeLogRow GetLastDriverStatus()
	 {
		 byte[] b=getLastDriverStatus();
		 if(b!=null)
		 {
			 PTimeLogRow tlr=null;
			 try {
				tlr=PTimeLogRow.parseFrom(b);
			} catch (InvalidProtocolBufferException e) {
			}
			 return tlr;
		 }
		 return null;
	 }
	 
	 public PEmployeeRow GetEmployee()
	 {
		 byte[] b=getEmployee();
		 if(b!=null)
		 {
			 PEmployeeRow emp=null;
			 try {
				emp=PEmployeeRow.parseFrom(b);
			} catch (InvalidProtocolBufferException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			 return emp;
		 }
		 return null;
	 }
	 public PInspectionRowList GetInspections(int boxID,int type,int flag)
	 {
		 byte[] b=getInspections( boxID,type,flag);
		 if(b!=null)
		 {
			 PInspectionRowList rv=null;
			 try {
				rv=PInspectionRowList.parseFrom(b);
			} catch (InvalidProtocolBufferException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			 return rv;
		 }
		 return null;
	 }
	
	 public PInspectionItemList GetInspectionItemsByCategoryId(int catID)
	 {
		 byte[] b=getInspectionItemsByCategoryId(catID);
		 if(b!=null)
		 {
			 PInspectionItemList rv=null;
			 try {
				rv=PInspectionItemList.parseFrom(b);
			} catch (InvalidProtocolBufferException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			 return rv;
		 }
		 return null;
	 }
	 
	 //--------------------------------------------------------
	 public String GetStringFromCurrentHosCycle() {
		 byte[] bytes= getStringFromCurrentHosCycle();
		 if(bytes!=null)
		 {
			String decoded="";
			try {
				decoded = new String(bytes, "UTF-8");
			} catch (UnsupportedEncodingException e) {}
			return decoded;
		 }
			return "";
		}
	 public String GetHosCycleList() {
		 byte[] bytes= getHosCycleList();
		 if(bytes!=null)
		 {
			String decoded="";
			try {
				decoded = new String(bytes, "UTF-8");
			} catch (UnsupportedEncodingException e) {}
			return decoded;
		 }
			return "";
		}
	 public String GetHosRecapSummary() {
		 byte[] bytes= getHosRecapSummary();
		 if(bytes!=null)
		 {
			String decoded="";
			try {
				decoded = new String(bytes, "UTF-8");
			} catch (UnsupportedEncodingException e) {}
			return decoded;
		 }
			return "";
		}
	 
	 public PRecapRowList GetRecapRowList() {
		 byte[] b= getRecapList();
		 if(b!=null)
		 {
			 PRecapRowList rr = null;
			try {
				rr=PRecapRowList.parseFrom(b);
			} catch (InvalidProtocolBufferException e) {}
			return rr;
		 }
			return null;
		}
	 
	 //===================WorkOrders=============================
	 
	 public PWorkOrderList GetWOList()
	 {
		 byte[] b= getWOList();
		 if(b!=null)
		 {
			 PWorkOrderList rv=null;
			 try {
				rv=PWorkOrderList.parseFrom(b);
			} catch (InvalidProtocolBufferException e) {WriteExc2Log(e);}
			 return rv;
		 }
		 return null;
	 }
	 
	 
     public native void initOnStart();
     public native int loginReq(String user,String pass, int rid );
     public native int registerReq(byte[] b ,int id);
 
     public native byte[] getProtoMessage(int msg_id);
     //======================messaging==========================
     //this is called from background thread !!!
    /* public void ReceiveMsg(String topic,String msg)
     {
        Log.d("MMM","Received message...");
		MessageActivity act = (MessageActivity)MkrApp.GetCurrentOnTopActivity();
			if(act!=null)
			{
				act.MqttReceivedMsg(topic,msg) ;
			}
     }*/
     public void ReceiveMsgCB(int req,int param,byte[] msg)
     {
        Log.d("MMM","Received message..."+req);
        Activity cur_act = MkrApp.GetCurrentOnTopActivity();
        switch(req)
        {
	        case GetActivities:
	        {
	        	
	        	 Activity act = MkrApp.GetCurrentOnTopActivity();
	             if(act!=null && act instanceof WorkOrdersActivity)
	             {
	            	 WorkOrdersActivity wo_act = (WorkOrdersActivity)MkrApp.GetCurrentOnTopActivity();
	            	 wo_act.Response4WOs(param);
	     			Log.d("MMM","Received GetActivities: "+param);
	             }
	        }
        	break;
	        case 1000:     // update from HosEngine HourCulculator
	        	 
	             if(cur_act!=null && cur_act instanceof HosActivity)
	             {
	            	 HosActivity _act = (HosActivity)cur_act;
	            	 try {
						_act.UpdateFromHosEngine(new String(msg, "UTF-8"));
					} catch (UnsupportedEncodingException e) {
						e.printStackTrace();
					}
	     			Log.d("MMM","UpdateFromHosEngine(): "+param);
	             }
	        	break;
	        case 1001:
	        {
	        
	          List<JsonMessageCls> msgs=_getMsgs_(msg);
	          if(msgs.size()>0)
	          {
	        	  if(GConst.gMsgUsers==null)
	        		  GConst.gMsgUsers=new ArrayList<JsonMessageCls>(); 
	        	 
	        	  for(JsonMessageCls m : msgs)
	        	  {
	        		  GConst.gMsgUsers.add(m);
	        	    if(cur_act!=null && cur_act instanceof MessageActivity)
	       			{
	        	    	MessageActivity act1 = (MessageActivity)cur_act;
	       				act1.addNewUser(m.dId,m.dN) ;
	       			}
	        		 
	        	  }
	          }
	        }
	        	break;
	        case 1002:
	        {
	        	 List<JsonMessageCls> msgs=_getMsgs_(msg);
	        	 if(msgs.size()>0)
		         {
	        		 
		        	  for(JsonMessageCls m : msgs)
		        	  {
		        		 // GConst.gMsgUsers.add(m);
		        		if(cur_act!=null && cur_act instanceof MessageActivity)
		       			{
		        			  MessageActivity act1 = (MessageActivity)cur_act;
		        			  act1.MqttReceivedMsg(m.dId,m.msg) ;
		       			}
		        		 
		        	  }
		         }
	        }
	        	break;
        }
       
     }
     List<JsonMessageCls> _getMsgs_(byte[] b)
     {
    		String json="";
	          try {
					json=new String(b, "UTF-8");
				} catch (UnsupportedEncodingException e) {}
	           // List<JsonMessageCls> msgs=GSON.
	          Gson gson=new Gson();
	          Type listType = new TypeToken<ArrayList<JsonMessageCls>>(){}.getType();
	          List<JsonMessageCls> msgs=gson.fromJson(json, listType);
	          return msgs;
     }
     /*
     void AddNewUser(String did,String un)
     {
    	 Log.d("MMM","Add new user...");
 		MessageActivity act = (MessageActivity)MkrApp.GetCurrentOnTopActivity();
 			if(act!=null)
 			{
 				act.addNewUser(did,un) ;
 			}
     }
     */
     
     public void WriteExc2Log(Exception ex)
     {
    	 String stackTrace = Log.getStackTraceString(ex); 
    	 write2Log(stackTrace);
     }
     
     //=====================================================================================
     public final int GetActivities=50;
     //=====================================================================================
     public native int write2Log(String msg );
     
     public native void initMQTT();  // init messaging
        
     public native int sendMessage(String topic,String message, int rid );
     
     public native int sendRequest(int req);
     //-----------------------------TimeLogs--------------------------------------
     public native int setDriverSts(byte[] ba ,int edit_flag);
     public native byte[]  getLastDriverStatus();
		// public native ArrayList<TimeLogRow> getTLRList();
     public native byte[] getTLRList(long date1,long date2,int flag);
     public native byte[] getRecapList(); 
     
     public native int saveEmployeeRow(byte[] b);
     public native byte[] getEmployee();
     public native byte[]  getStringFromCurrentHosCycle();
     public native byte[]   getHosCycleList();
     public native byte[]  getHosRecapSummary();
     
     
     public native byte[] getInspections(int boxID,int type,int flag);  // type=0 - pretrip, 1- posttrip / flag 0-recent, 1 -old
    // public native byte[] getCategories(int bid);
     public native byte[] getInspectionItemsByCategoryId(int cId);
     //--------------------------WorkOrders----------------------------
     public native byte[] getWOList();
	 public native void updateHosCycle(int actionId);
	 public native  int checkIfLoginIsNeeded() ;
	public native int processLogout();
	public native int sendEmail(byte[] bytes, int length) ;
	
     
}
