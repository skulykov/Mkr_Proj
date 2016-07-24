package com.mkr.chat;


import java.util.ArrayList;
import java.util.List;

import com.mkr.unidriver.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.TextView;

public class ChatArrayAdapter extends ArrayAdapter<ChatMessage> {

	
	private List<ChatMessage> messages = new ArrayList<ChatMessage>();
	
	Context ctx=null; 
	@Override
	public void add(ChatMessage object) {
		messages.add(object);
		super.add(object);
	}

	public ChatArrayAdapter(Context context, int textViewResourceId) {
		super(context, textViewResourceId);
		ctx=context;
	}

	public int getCount() {
		return this.messages.size();
	}

	public ChatMessage getItem(int index) {
		return this.messages.get(index);
	}
    public void _clear(int v)
    {
    	if(v==1) super.clear();
    	this.messages.clear();
      
    }
    /*
	public View getView(int position, View convertView, ViewGroup parent) {
		View row = convertView;
		
		if (row == null) {
			LayoutInflater inflater = (LayoutInflater) this.getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			row = inflater.inflate(R.layout.listitem_chat, parent, false);
		}

		LinearLayout wrapper = (LinearLayout) row.findViewById(R.id.wrapper);

		ChatMessage cm = getItem(position);

		
		TextView messageText = (TextView) row.findViewById(R.id.message_text_id);
		messageText.setText(cm.message);
		wrapper.setBackgroundResource(cm.left ? R.drawable.bubble_yellow : R.drawable.bubble_green);
		
		TextView tdir = (TextView) row.findViewById(R.id.message_dir_id);
		String text="";
		View view=(View)row.findViewById(R.id.zero_view_id);
		if(cm.left)
		{
			//wrapper.setGravity(Gravity.LEFT);  // outgoing message
			text=ctx.getString(R.string.str_to);
			view.setVisibility(View.GONE);
		}
		else
		{
			//wrapper.setGravity(Gravity.RIGHT);  // outgoing message
			text=ctx.getString(R.string.str_from);
			view.setVisibility(View.VISIBLE);
		}
		tdir.setText(text+cm.userDesc);
		return row;
	}
*/
    public View getView(int position, View convertView, ViewGroup parent) {
		View row = convertView;
		if (row == null) {
			LayoutInflater inflater = (LayoutInflater) this.getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			row = inflater.inflate(R.layout.listitem_chat, parent, false);
		}

		LinearLayout wrapper = (LinearLayout) row.findViewById(R.id.wrapper);

		ChatMessage cm = getItem(position);

		TextView fromText = (TextView) row.findViewById(R.id.message_dir_id);
		fromText.setText("From:"+cm.userDesc);
		
		TextView messageText = (TextView) row.findViewById(R.id.message_text_id);
		messageText.setText(cm.message);
		messageText.setBackgroundResource(cm.left ? R.drawable.bubble_yellow : R.drawable.bubble_green);
		wrapper.setGravity(cm.left ? Gravity.LEFT : Gravity.RIGHT);

		return row;
	}
	public Bitmap decodeToBitmap(byte[] decodedByte) {
		return BitmapFactory.decodeByteArray(decodedByte, 0, decodedByte.length);
	}

}