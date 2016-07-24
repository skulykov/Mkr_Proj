package com.mkr.chat;

import com.mkr.utils.DateUtil;
import com.mkr.utils.Util;



public class ChatUser {
	public String userDesc;
	public String did;
    public String ts;
    public boolean processed;
    
    public ChatUser(String did,String un)
    {
    	this.userDesc=un;
    	this.did=did;
    	this.ts=DateUtil.getTS(null);
    	processed=true;
    }

}
