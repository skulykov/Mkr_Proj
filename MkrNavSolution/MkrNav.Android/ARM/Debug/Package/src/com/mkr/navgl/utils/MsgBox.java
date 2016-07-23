package com.mkr.navgl.utils;



import com.mkr.navgl.nintf.MkrNavLib;
import com.mkr.navgl.nintf.R;

import android.app.Dialog;
import android.content.Context;
import android.os.Handler;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MsgBox {

	Context ctx;
	float _x,_y;
	public MsgBox(Context _ctx)
	{
		ctx=_ctx;
	}
	
	public void show_route_to(String aTitle,float x,float y)
	{
		_x=x;_y=y;
		final Dialog d = new Dialog(ctx,R.style.DialogSlideAnim); // with animation
			
		Window window = d.getWindow();
		window.setBackgroundDrawableResource(android.R.color.transparent);
		window.requestFeature(window.FEATURE_NO_TITLE);
		
		d.setContentView(R.layout.my_route_longtouch_dialog);
		TextView title=(TextView)d.findViewById(R.id.myDlgTitle);
		title.setText( aTitle );
		
		
		OnClickListener listener=new OnClickListener()
		{

			@Override
			public void onClick(View v) {
				int flag=0;//String str="";
				switch(v.getId())
				{
					case R.id.dlg_button_dest:
					flag=1;//str="Dest";// handle button A click;
					break;
					case R.id.dlg_button_route:
					flag=2;//str="Route";// handle button B click;
					break;
					case R.id.dlg_button_favor:
						flag=3;//str="Favor";// handle button B click;
						break;
					default:
					throw new RuntimeException("Unknow button ID");
				}
				//Toast.makeText(ctx, str, Toast.LENGTH_SHORT).show();
				
				if(flag!=0)
					MkrNavLib.routeOption(_x,_y,flag);
				 d.cancel();
                 d.dismiss();
			}
			
		};
		
		Button b1=(Button)d.findViewById(R.id.dlg_button_dest);
		b1.setOnClickListener(listener);
		Button b2=(Button)d.findViewById(R.id.dlg_button_route);
		b2.setOnClickListener(listener);
		Button b3=(Button)d.findViewById(R.id.dlg_button_favor);
		b3.setOnClickListener(listener);
				
		Button btn=(Button)d.findViewById(R.id.dlg_button_ok);
		btn.setOnClickListener(new OnClickListener() {
		    public void onClick(View v) {
		    	 d.cancel();
                 d.dismiss();
		    }
		});
		d.show();
		 
		

	}
}
