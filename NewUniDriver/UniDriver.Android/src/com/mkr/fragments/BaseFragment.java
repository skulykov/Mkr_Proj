package com.mkr.fragments;

import android.app.Fragment;
import android.app.LocalActivityManager;
import android.os.Bundle;

public class BaseFragment extends Fragment 
{
	public LocalActivityManager localManager;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		localManager = new LocalActivityManager (getActivity(), true);
		localManager.dispatchCreate (savedInstanceState);
	}
	@Override
	public void onResume()
	{
		super.onResume ();
		localManager.dispatchResume ();
	}
	

	@Override
	public void onPause()
	{
		super.onResume ();
		localManager.dispatchPause (getActivity().isFinishing());
	}
	
	@Override
	public void onStop()
	{
		super.onResume ();
		localManager.dispatchStop ();
	}
	@Override
	public void onDestroy()
	{
		super.onResume ();
		localManager.dispatchDestroy (getActivity().isFinishing());
	}
	
}
