package com.mkr.unidriver.workorders;

import java.util.ArrayList;

import net.londatiga.android.ActionItem;
import net.londatiga.android.QuickAction;

import com.mkr.models.proto.CommPackets.PWorkOrderItem.Builder;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.R;
import com.mkr.unidriver.hos.RulesActivity;
import com.mkr.utils.GConst;


import android.app.Activity;
import android.view.View;



public class JobStatusPopUp extends View 
{
	WorkOrderDetailsActivity wo_act;
	QuickAction _quickAction;
	
	public static int 	Sts_Open=0,
		Sts_Dispatched=1,
		Sts_Accepted=2,
		Sts_Started=3,
		Sts_Completed=4,
		Sts_Rejected=5;
		
	public static int[] Sts_Names_Ids={
		R.string.str_job_disp,
		R.string.str_job_accept,
		R.string.str_job_start,
		R.string.str_job_done,
		R.string.str_job_rej
	};
	
	public JobStatusPopUp(WorkOrderDetailsActivity _act)
	{
		 super(_act);
		wo_act=_act;
		SetQuickAction();
	}
	void SetQuickAction()
	{
		_quickAction = new QuickAction(this.wo_act, QuickAction.VERTICAL);
		ActionItem[] ai=new ActionItem[5];
		ai[0] = new ActionItem(Sts_Dispatched,(String) wo_act.getResources().getText(R.string.str_job_disp), wo_act.getResources().getDrawable(R.drawable.wo_sts_dispatched_24));
		ai[1] = new ActionItem(Sts_Accepted,(String) wo_act.getResources().getText(R.string.str_job_accept), wo_act.getResources().getDrawable(R.drawable.wo_sts_accept_24));
		ai[2] = new ActionItem(Sts_Started,(String) wo_act.getResources().getText(R.string.str_job_start), wo_act.getResources().getDrawable(R.drawable.wo_sts_start_24));
		ai[3] = new ActionItem(Sts_Completed,(String) wo_act.getResources().getText(R.string.str_job_done), wo_act.getResources().getDrawable(R.drawable.wo_sts_complete_24));
		ai[4] = new ActionItem(Sts_Rejected,(String) wo_act.getResources().getText(R.string.str_job_rej), wo_act.getResources().getDrawable(R.drawable.wo_sts_reject_24));

		for(int i=0;i<5;i++)
			_quickAction.addActionItem(ai[i]);
		
		 //Set listener for action item clicked
		_quickAction.setOnActionItemClickListener(new QuickAction.OnActionItemClickListener() {			
			@Override
			public void onItemClick(QuickAction source, int pos, int actionId) {				
				//ActionItem ai = _quickAction.getActionItem(pos);
				int sts=actionId;  // work order status
				if(sts!=GConst.wo_item.getStatus())
				{
	                wo_act.RefreshJobStatus(pos);
	                Builder bldr=GConst.wo_item.toBuilder();   //=ind;
	                bldr.setStatus(pos+1);
	                GConst.wo_item=bldr.build();
					//WorkOrdersDB.Get().UpdateWorkOrderStatus (woi);
				    //MkrNLib.Get().updateHosCycle(actionId);
				}
			}
		});
		
		//set listnener for on dismiss event, this listener will be called only if QuickAction dialog was dismissed
		//by clicking the area outside the dialog.
		_quickAction.setOnDismissListener(new QuickAction.OnDismissListener() {			
			@Override
			public void onDismiss() {
				//Toast.makeText(ctx.getApplicationContext(), "Dismissed", Toast.LENGTH_SHORT).show();
			}
		});
		
	}
	
	 public void Show(View v)
     {
			if(_quickAction==null)
					SetQuickAction ();
				if(_quickAction!=null)
				{
					//_quickAction.Show((View)sender, QuickActionAnimationStyle.GrowFromCenter);
					_quickAction.show(v);
					_quickAction.setAnimStyle(QuickAction.ANIM_REFLECT);
				}
     }
}
