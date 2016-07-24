package com.mkr.views;

import java.util.ArrayList;
import java.util.List;

import com.mkr.service.MkrNLib;
import com.mkr.unidriver.hos.HosActivity;
import com.mkr.unidriver.hos.RulesActivity;

import net.londatiga.android.ActionItem;
import net.londatiga.android.QuickAction;
import android.content.Context;
import android.view.View;

class HosCycle
{
   public String name;
   public int val;
}
public class HosCyclePopUp extends View
{
	RulesActivity act;
	QuickAction _quickAction;
	
	public HosCyclePopUp(RulesActivity _act)
	{
		 super(_act);
		act=_act;
		SetQuickAction();
	}
	ArrayList<HosCycle> process_hos_cycle_list(String str)
	{
		ArrayList<HosCycle> hlist=new ArrayList<HosCycle>();
		//http://stackoverflow.com/questions/3481828/how-to-split-a-string-in-java
		String[] sa = str.split ("\\|"); //there are 12 characters with special meanings: USE backslash  !!! or split("[.]")
		if(sa.length>0)
		{
			for(String s : sa)
			{
			   String[] sa1=s.split(",");
			   if(sa1.length>=2)
			   {
				   HosCycle clc=new HosCycle();
				   clc.name=sa1[0];
				   clc.val= Integer.parseInt(sa1[1]);
				   hlist.add(clc);
			   }
			}
		}
		return hlist;
	}
	void SetQuickAction()
	{
		_quickAction = new QuickAction(this.act, QuickAction.VERTICAL);
		
		String str= MkrNLib.Get().GetHosCycleList();
		if(str==null||str=="")
			return;
		ArrayList<HosCycle> hlist=process_hos_cycle_list(str);
		if(hlist.size()==0)
			return;
		for(HosCycle clc : hlist)
		{
			ActionItem ai=new ActionItem(clc.val,clc.name,null);
			_quickAction.addActionItem(ai);
		}
		
		 //Set listener for action item clicked
		_quickAction.setOnActionItemClickListener(new QuickAction.OnActionItemClickListener() {			
			@Override
			public void onItemClick(QuickAction source, int pos, int actionId) {				
				ActionItem ai = _quickAction.getActionItem(pos);
                act.RefreshCycleStatus(ai.getTitle());
			    MkrNLib.Get().updateHosCycle(actionId);
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
