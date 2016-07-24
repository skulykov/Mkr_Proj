package com.mkr.chat;

import java.util.ArrayList;
import java.util.List;

import com.mkr.unidriver.R;


import android.app.Activity;
import android.content.Context;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.TextView;

public class UsersArrayAdapter extends ArrayAdapter<ChatUser>{

	
	//private List<ChatUser> users = new ArrayList<ChatUser>();
	//private LinearLayout wrapper;
	private int selectedPos=-1;
	private Activity context;
	public UsersArrayAdapter(Activity context, int textViewResourceId,ArrayList<ChatUser> list) {
		super(context, textViewResourceId,list);
		this.context=context; 
	}

	@Override
	public void add(ChatUser object) {
		//users.add(object);
		super.add(object);
	}
	
	public void addUser(ChatUser cu) {
		int len=super.getCount();
		if(len>0)
		{
			boolean found=false;
			for(int i=0;i<len;i++)
			{
				ChatUser cu1=getItem(i);
				//if(cu.did==cu1.did)  //== tests for reference equality (whether they are the same object).
				if(cu.did.equalsIgnoreCase(cu1.did))
				{
				  found=true;
				  break;
				}
			}
			if(!found)
				super.add(cu);
		}
		else
		{
			super.add(cu);
			selectedPos=0;
		}
	}
	public String getUserDescByDid(String did) {
		int len=super.getCount();
		String rv="unknown";
		if(len>0)
		{
			
			for(int i=0;i<len;i++)
			{
				ChatUser cu1=getItem(i);
				//if(cu.did==cu1.did)  //== tests for reference equality (whether they are the same object).
				if(did.equalsIgnoreCase(cu1.did))
				{
				  rv=cu1.userDesc;
				  break;
				}
			}
			
		}
		return rv;
	}
	public void updateUsers()
	{
		super.clear();
		/*
		for(int i=0;i<users.size();i++)
		{
			this.add(getItem(i));
		}*/
		super.notifyDataSetChanged();
	}
	public int getCount() {
		return super.getCount();
	}

	public ChatUser getItem(int index) {
		return super.getItem(index);
	}
	public List<ChatUser> getUsers()
	{
		return null;//super.getAll();
	}
	public ChatUser getSelected() {
		if(selectedPos!=-1)
			return getItem(selectedPos);
		else
			return null;
	}
	public void _clear(int v)
	{
		if(v==1) super.clear();
	//	users.clear();
	}
	public void preprocess()
	{
		/*for(ChatUser c : users)
		{
			c.processed=false;
		}*/
	}
	public boolean postprocess() // find all users which need to be removed ( logout devices)
	{
		boolean rv=false;
		/*boolean done=true;
		while(done)
		{
			done=false;
			int len=users.size();
			ChatUser c=null;
			for(int i=0;i<len;i++)
			{
				c=this.users.get(i);
				if(!c.processed)
				{
				    rv=true;
				    done=true;
					break;
				}
			}
			if(done)
				this.users.remove(c);
		}*/
		return rv;
	}
	
	View m_getView(int position, View convertView, ViewGroup parent, int rr,int rr1,int rr2) {
		View row = convertView;
		if (row == null) {
			LayoutInflater inflater = context.getLayoutInflater();//(LayoutInflater) this.getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			row = inflater.inflate(rr, parent, false);
				
		}
        selectedPos=position;
		ChatUser user = getItem(position);

		TextView tv = (TextView) row.findViewById(rr1);
		tv.setText(user.ts);
		
		TextView tv1 = (TextView) row.findViewById(rr2);
		tv1.setText(user.userDesc);

		return row;
	}
	public View getView(int position, View convertView, ViewGroup parent) {
		
		return m_getView(position, convertView, parent,R.layout.users_spinner_item,R.id.deviceid,R.id.description);
	}

	@Override
	public View getDropDownView(int position, View convertView,ViewGroup parent) 
	{
	  return m_getView(position,convertView,parent,R.layout.user_spinner_item_dd,R.id.user_id_dd,R.id.user_desc_dd);
	}
	
}
