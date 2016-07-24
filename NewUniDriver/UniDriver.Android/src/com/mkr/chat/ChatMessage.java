package com.mkr.chat;

import com.mkr.utils.Util;



public class ChatMessage {
	public boolean left;
	public String cid,did,userDesc,message;
    public String ts;
	public ChatMessage(boolean left, String cid,String did,String userDesc,String msg) {
		super();
		this.left = left;
		this.message = msg;
		this.cid=cid;
		this.did=did;
		this.userDesc=userDesc;
		ts=Util.getTimeStamp();
	}

}