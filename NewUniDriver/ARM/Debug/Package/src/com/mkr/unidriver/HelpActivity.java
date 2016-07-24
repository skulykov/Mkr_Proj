package com.mkr.unidriver;

import com.mkr.service.MkrApp;

import android.os.Bundle;
import android.view.View;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class HelpActivity extends OnResultBaseActivity{
	
	WebView web_view;
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.my_webview);
        web_view = (WebView) findViewById (R.id.help_webview);
        web_view.setWebViewClient(new HelloWebViewClient ());
        WebSettings ws=web_view.getSettings();
        ws.setJavaScriptEnabled(true);
        ws.setLoadWithOverviewMode(true);
        ws.setUseWideViewPort(true);
        
        web_view.loadUrl ("http://www.mikerel.com/mobile");
		SetMainTitles("UniDriver-Help",R.layout.custom_ab_help);
	}
	public void ABBackBtn(View v)
	{
	  finish();
	}
	class HelloWebViewClient extends WebViewClient
	{
		
	}
}
