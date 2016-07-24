package com.mkr.fragments;

import com.mkr.unidriver.MessageActivity;
import com.mkr.unidriver.inspection.InspectionReportActivity;

import android.app.Activity;
import android.app.Fragment;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;

public class InspectionFragment extends BaseFragment {

	@Override
	  public View onCreateView(LayoutInflater inflater, ViewGroup container,Bundle savedInstanceState) {
	   // View view = inflater.inflate(R.layout.fragment_rssitem_detail, container, false);
		Activity act=getActivity(); 
		Intent intent = new Intent(act, InspectionReportActivity.class);
		Window window = localManager.startActivity ("IRActivity", intent);
		 View currentView = window.getDecorView();
       currentView.setVisibility(View.VISIBLE);
       currentView.setFocusableInTouchMode(true);
       ((ViewGroup)currentView).setDescendantFocusability(ViewGroup.FOCUS_AFTER_DESCENDANTS/*DescendantFocusability.AfterDescendants*/);
	    return currentView;
	  }
}
