package com.mkr.unidriver;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.mkr.chat.ChatArrayAdapter;
import com.mkr.chat.ChatMessage;
import com.mkr.chat.ChatUser;
import com.mkr.chat.UsersArrayAdapter;
import com.mkr.service.MkrApp;
import com.mkr.service.MkrNLib;
import com.mkr.utils.Util;


import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.Spinner;


public class MessageActivity extends OnResultBaseActivity {
	protected static final int CONNECTION_FAILED = 0;
	protected static final int MESSAGE_ADD_RECEIVED = 1;
	
	protected static final int MESSAGE_SENT_SUCCESS = 2;
	protected static final int MESSAGE_SENT_FAILURE = 3;
	protected static final int NO_REGISTERD_USER =4;
	protected static final int USERS_UPDATED=5;
	
	private EditText messageTextEdit;
	private ProgressBar queryProgressBar;
	private ListView chatListView;
    private ChatArrayAdapter listAdapter;
    private UsersArrayAdapter usersAdapter;
	private Button sndBtn;
	Spinner users_spinner;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//mLog.w("WL","ON Create..MainActivity..");
		
				      
		setContentView(R.layout.my_chat_layout);
		sndBtn=(Button)findViewById(R.id.send_message_button);
		messageTextEdit = (EditText) findViewById(R.id.message_editText);
		queryProgressBar = (ProgressBar) findViewById(R.id.chatQuery_progressBar);
		chatListView = (ListView) findViewById(R.id.chat_listView);
		
		messageTextEdit.clearFocus();
		
		listAdapter = new ChatArrayAdapter(getApplicationContext(), R.layout.listitem_chat);
		chatListView.setAdapter(listAdapter);
		
		//just for test
		//addItems();
		//createBackgroundThread();
		
		
		 	users_spinner = (Spinner) findViewById(R.id.users_spinner_id);
		 	//http://stephenpengilley.blogspot.ca/2013/01/android-custom-spinner-tutorial.html#!/2013/01/android-custom-spinner-tutorial.html
		 	usersAdapter = new UsersArrayAdapter(this, R.layout.users_spinner_item,new ArrayList<ChatUser>());
	        users_spinner.setAdapter(usersAdapter);
		
	        
	        //do not have users -> hide it
	        //users_spinner.setVisibility(View.GONE);
		
	      //  MkrNLib.Get().initMQTT();   // init messaging   !!! IMPORTANT WE do it in MkrApp !!!
	        
	       
			SetTitle("Messenger");
			
	}
	//http://stackoverflow.com/questions/10550290/best-way-to-hide-keyboard-in-android
	
	void btnsSetToNormal()
	{
		queryProgressBar.setVisibility(View.GONE);
        sndBtn.setVisibility(View.VISIBLE);
        
       
        messageTextEdit.setText("");
        Util.hideSoftKeyboard(this);
	}
	 @Override
	    protected void onStop() {
	    
		 super.onStop();
	        
	    }

	    @Override
	    protected void onPause() {
	        super.onPause();
	        messageTextEdit.setText("");
	      
	    }

	    @Override
		public void onResume() {
	        super.onResume();
	        btnsSetToNormal();
            
	    }
	private void addItems() {
		/*
		listAdapter.add(new com.wl.chat.ChatMessage(true,"","", "Hello bubbles!\r\nWELCOME BIG MESSSAAAAAAAAGGGGGGEEEEEEEEEE"));

		for (int i = 0; i < 8; i++) {
			boolean left = Util.getRandomInteger(0, 1) == 0 ? true : false;
//			int word = Util.getRandomInteger(1, 10);
//			int start = Util.getRandomInteger(1, 40);
			String words = "helloMsg"+Integer.toString(i);//ipsum.getWords(word, start);

			listAdapter.add(new com.wl.chat.ChatMessage(left,"","", words));
		}*/
	}
	 public void onClickButtons(View v) {
	        switch (v.getId()) {
	            case R.id.send_message_button:
	               
	                // check message length
	            	String  message2Send = messageTextEdit.getText().toString();
	                if (message2Send.length() == 0) {
	                    return;
	                }
	                Log.d("MMM",message2Send);
	                queryProgressBar.setVisibility(View.VISIBLE);
	                sndBtn.setVisibility(View.GONE);
	                
	                String dest= getDestDid();
	              //  if(dest!="")
	                {
	                	MkrNLib.Get().sendMessage(dest,message2Send,0);
	                }
	                messageTextEdit.clearFocus(); // does not hide keyboard !!!!!!!!!!!!
	                
	                btnsSetToNormal();
	                break;
	        }
	    }
	 public void addNewUser(String did,String un)
	 {
	    final	String _did=did,_un=un; 
		 runOnUiThread(new Runnable() {
		        @Override
		        public void run() {
		        	 ChatUser cu = new ChatUser(_did,_un);
					 usersAdapter.addUser(cu);
		        }
		    });
	 }
	 void userUpdated()
	 {
		 usersAdapter.updateUsers();
		  if(usersAdapter.getCount()>0)
			  users_spinner.setVisibility(View.VISIBLE);
		  else
			  users_spinner.setVisibility(View.GONE);
	 }
	 String getDestDid()
	 {
		 ChatUser cu=usersAdapter.getSelected();
			if(cu!=null)
				return cu.did;
			else
				return "";
	 }
	 String getDestDesc()
	 {
		 ChatUser cu=usersAdapter.getSelected();
			if(cu!=null)
				return cu.userDesc;
			else
				return "";
	 }
	 public void MqttReceivedMsg(String from_did,String _msg) {
		    final String message = _msg,from=from_did;
		    
		    runOnUiThread(new Runnable() {
		        @Override
		        public void run() {
		        	btnsSetToNormal();
		        	String desc=usersAdapter.getUserDescByDid(from);
		        	ChatMessage cm=new ChatMessage(true,"111",from,desc,message);
		        	listAdapter.add(cm);
		        	//addNewUser("222",topic);
		        }
		    });
		}
	
		
	
	 
	
}
