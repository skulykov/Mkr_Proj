package com.mkr.utils;

import java.util.List;

import com.mkr.models.JsonMessageCls;
import com.mkr.models.proto.CommPackets.PInspectionItem;
import com.mkr.models.proto.CommPackets.PInspectionItemList;
import com.mkr.models.proto.CommPackets.PInspectionRow;
import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.models.proto.CommPackets.PWorkOrderItem;
import com.mkr.models.proto.CommPackets.PInspectionRowList.Builder;

public class GConst 
{

	public static final int G_RESULT_OK = 0;
	public static final int  ActivityCanceled=1;
	public static final int REQUEST_DEFECT_ADD=3;
	public static int REQUEST_VEH_BOX_ID=5;
	public static int  REQUEST_REGISTER_ADD=6;
	public static int REQUEST_InspectionReport_ID=7;
	public static int REQUEST_HELP_ID=15;
	public static int INSP_FORM_ACTIVITY_RESULT_ID=27;
	public static int REQUEST_ATTACHMENT=28;
	public static final int  REQUEST_TAKE_PICTURE=29;
	
	public static final int REQUEST_HOS_EVENT_EDIT=1112;
	public static final int REQUEST_SIGNATURE_ADD=1113;
	public static final int REQUEST_HOS_DAILY_LOGS=1111;
	
	public static final int REQUEST_PreTripIR=1114;
	public static final int REQUEST_PostTripIR=1115;
	public static final int REQUEST_HOS_RULES=1116;
	public static final int REQUEST_LOGOUT=1117;
	public static final int REQUEST_WORK_ORDER_CHAT=1118;
	public static final int REQUEST_WORK_ORDER_DETAIL=1119;
	
     
	public static final String EXTRA_HOS_EVENT_EDIT_ID = "hos_event_edit_id";
	public static final String EXTRA_HOS_GRAPH_DATE="hos_ev_edit";
	
	public static final String EXTRA_REG_USER="req_user";
	public static final String EXTRA_REG_PASS="req_pass";
	
	public static final String EXTRA_REG_INSPECTION="req_inspec";
	
	public static final String INSP_FORM_NEW="insp_form_new";
	public static final String INSP_FORM_ID="insp_form_id";
	public static final String INSP_FORM_TYPE="insp_f_type";
	public static final String IS_ATTACHMENT="is_attach";
	public static final String INSP_PARENT_DESC="ins_par_desc";
	public static final String EXTRA_DEFECT_ABBR="def_abbr";
	public static final String EXTRA_COMMENT="def_coments";
	public static final String EXTRA_MEDIA_FILES="def_media_f";
	public static final String  EXTRA_HAS_DEFECT="has_def";
	public static final String  EXTRA_IS_READONLY="def_is_ro";
	public static final String  EXTRA_LOGOUT="ext_logout";
	
	
	//Global vars
	public static String CurrentDriverName="";
	public static int BoxID=10023;
	
	public static  PTimeLogRow selected_tlr=null;
	//inspection vars used to send between activities
	public static PInspectionRow  g_IR=null;  // inspection report to view
//	public static PInspectionRow.Builder g_IR_Builder=null;//
	
	public static PInspectionItemList insp_items_array=null;
	public static PInspectionItem currentInspectionItem=null;
	
	
	public static  PWorkOrderItem wo_item=null;
	
	public static List<JsonMessageCls> gMsgUsers=null;
}
