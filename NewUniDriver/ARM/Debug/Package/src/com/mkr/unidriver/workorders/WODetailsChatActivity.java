package com.mkr.unidriver.workorders;

import java.util.List;

import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.ListView;

import com.mkr.models.proto.CommPackets.PAMessage;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;

public class WODetailsChatActivity extends OnResultBaseActivity 
{
	EditText send_msg_v=null;
	WorkOrderChatAdapter adapter=null;
	ListView jobs_chat_lv=null;
	List<PAMessage> amsgs_list=null;
	 @Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.jobs_item_detail_chat);
	        send_msg_v=(EditText)findViewById (R.id.jobs_chat_msg_et);
			jobs_chat_lv = (ListView)findViewById (R.id.jobs_chat_lv);

			adapter = new WorkOrderChatAdapter (this, /*amsgs_list*/0);
			jobs_chat_lv.setAdapter(adapter);

			ActivateDispatcherChat();

			String title=(String) getResources().getText(R.string.str_disp_chat);
			SetMainTitles("Work order messages",R.layout.custom_ab_help);
	 }
	 public void ABBackBtn(View v)
		{
		  finish();
		}
	 public void Sendmsg(View v)
	 {
		 
	 }
	 void ActivateDispatcherChat()
		{
			/*int rid = Global.login_info.ResourceID;
			int aId = woi == null ? 0 : woi.ID;
			List<AMessage> list=WorkOrdersDB.Get().GetMessageList(aId,false);
			if (list != null && list.Count > 0) 
			{
				amsgs_list.AddRange(list);
				adapter.NotifyDataSetChanged ();
			}
			if (NetworkUtil.IsNetworkAvailable ()) {
				NetTcp.Get ().ReadAMessagesFromServer (aId, rid, add_comments_cb);
			} else {
				//add msgs from a local cache

			}*/
		}
}
