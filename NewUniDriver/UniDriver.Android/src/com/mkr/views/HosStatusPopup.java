package com.mkr.views;

import com.mkr.unidriver.R;
import com.mkr.unidriver.hos.HosActivity;

import net.londatiga.android.ActionItem;
import net.londatiga.android.QuickAction;
import android.content.Context;
import android.view.View;
import android.widget.Toast;


public class HosStatusPopup extends View
{
	
	private static final int ID_OFFDUTY    = 1;
	private static final int ID_SLEEPING   = 2;
	private static final int ID_DRIVING = 3;
	private static final int ID_ONDUTY   = 4;
	private static final int ID_EMERGENCY  = 5;	
	private static final int ID_PERSONAL    = 6;
	
	Context ctx;
	QuickAction _quickAction;
	public HosStatusPopup(Context context)
	{
		 super(context);
		ctx=context;
		SetQuickAction();
	}
	
	void SetQuickAction()
	{
		
				ActionItem[] ai=new ActionItem[6];
				ai[0] = new ActionItem((int)ID_OFFDUTY,(String) ctx.getResources().getText(R.string.str_off_d), ctx.getResources().getDrawable(R.drawable.hos_off_d));
				ai[1] = new ActionItem((int)ID_SLEEPING,(String) ctx.getResources().getText(R.string.str_sleeping), ctx.getResources().getDrawable(R.drawable.hos_sleep));
				ai[2] = new ActionItem((int)ID_DRIVING,(String) ctx.getResources().getText(R.string.str_driving), ctx.getResources().getDrawable(R.drawable.hos_driving));
				ai[3] = new ActionItem((int)ID_ONDUTY,(String) ctx.getResources().getText(R.string.str_on_d), ctx.getResources().getDrawable(R.drawable.hos_on_d));
				ai[4] = new ActionItem((int)ID_EMERGENCY,(String) ctx.getResources().getText(R.string.str_emerg_dri), ctx.getResources().getDrawable(R.drawable.hos_emerg));
				ai[5] = new ActionItem((int)ID_PERSONAL,(String) ctx.getResources().getText(R.string.str_pers_dri), ctx.getResources().getDrawable(R.drawable.hos_person));

	           // prevItem.IsSticky = true;
	           // nextItem.IsSticky = true;
			   //	searchItem.IsSticky=true;

			   int selId=101;//HOSAgent.getSelectedDriverStatusIndexRow();
				if(selId>=0&&selId<6)
					ai[selId].setSelected(true);

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
	    				//HOSAgent.setSelectedDriverStatusByIndexRow(ind);
	    				if(act!=null)
	    					act.selectDriverStatus(ind);
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