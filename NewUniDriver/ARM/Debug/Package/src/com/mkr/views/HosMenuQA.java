package com.mkr.views;

import com.mkr.unidriver.R;
import com.mkr.unidriver.hos.HosActivity;

import net.londatiga.android.ActionItem;
import net.londatiga.android.QuickAction;
import android.content.Context;
import android.view.View;


public class HosMenuQA extends View
{
	private static final int ID_RECAP    = 1;
	private static final int ID_DAILY_LOGS   = 2;
	private static final int ID_RULES = 3;
	private static final int ID_COWORKER   = 4;
	private static final int ID_COWORKER_LIST  = 5;	
	private static final int ID_EMAIL    = 6;
	
	
	Context ctx;
	QuickAction _quickAction;
	public HosMenuQA(Context context)
	{
		 super(context);
		ctx=context;
		SetQuickAction();
	}
	void SetQuickAction()
	{
		
				ActionItem[] ai=new ActionItem[6];
				ai[0] = new ActionItem((int)ID_RECAP,(String) ctx.getResources().getText(R.string.str_recap), ctx.getResources().getDrawable(R.drawable.hos));
				ai[1] = new ActionItem((int)ID_DAILY_LOGS,(String) ctx.getResources().getText(R.string.str_daily_logs), ctx.getResources().getDrawable(android.R.drawable.ic_dialog_map));
				ai[2] = new ActionItem((int)ID_RULES,(String) ctx.getResources().getText(R.string.str_hos_rules), ctx.getResources().getDrawable(android.R.drawable.ic_menu_preferences));
				ai[3] = new ActionItem((int)ID_COWORKER,(String) ctx.getResources().getText(R.string.str_coworker), ctx.getResources().getDrawable(android.R.drawable.ic_menu_directions));
				ai[4] = new ActionItem((int)ID_COWORKER_LIST,(String) ctx.getResources().getText(R.string.str_coworker_list), ctx.getResources().getDrawable(android.R.drawable.ic_menu_agenda));
				ai[5] = new ActionItem((int)ID_EMAIL,(String) ctx.getResources().getText(R.string.str_send_email), ctx.getResources().getDrawable(android.R.drawable.ic_dialog_email));

	            _quickAction = new QuickAction(this.ctx, QuickAction.VERTICAL);
	            for(int i=0;i<6;i++)
					_quickAction.addActionItem(ai[i]);
	           
	            //Set listener for action item clicked
	    		_quickAction.setOnActionItemClickListener(new QuickAction.OnActionItemClickListener() {			
	    			@Override
	    			public void onItemClick(QuickAction source, int pos, int actionId) {				
	    				ActionItem actionItem = _quickAction.getActionItem(pos);
	                     
	    				int ind=actionId-1;	
	    				HosActivity act=(HosActivity)ctx;
	    				switch(ind)
	    				{
	    				case 1:
	    					act.StartHosDailyLogsActivity();
	    					break;
	    				case 2:
	    					act.StartHosRulesActivity ();
	    					break;
	    				case 3:
	    					act.StartCoWorkerLoginActivity ();
	    					break;
	    				case 4:
	    					act.StartCoWorkerLogoutActivity ();
	    					break;
	    				case 5:
	    					act.StartEmailActivity ();
	    					break;
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
