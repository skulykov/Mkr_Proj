package com.mkr.navgl.service;
import android.content.Intent;
public interface IMkRlTtsManager 
{
	public void InitTtsMngNativeLayer();
	public void Submit( final String aText, final String aFullPath, final long aCbContext );
	public void Prepare();
	public void onDataCheckResult( int resultCode, Intent data );
	public void onDataInstallResult( int resultCode, Intent data );
	public void onInit( int status );
}
