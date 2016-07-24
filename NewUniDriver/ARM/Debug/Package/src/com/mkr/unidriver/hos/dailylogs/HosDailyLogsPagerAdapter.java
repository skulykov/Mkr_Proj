package com.mkr.unidriver.hos.dailylogs;


import java.util.HashMap;
import java.util.Map;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentStatePagerAdapter;
import android.view.ViewGroup;

public class HosDailyLogsPagerAdapter extends FragmentStatePagerAdapter
{
	private final int PageCount = 14;  // two weeks
	Map<Integer, PagerFragment> mPageReferenceMap = null;
	public HosDailyLogsPagerAdapter(FragmentManager fm) {
		super(fm);
		mPageReferenceMap = new HashMap<Integer, PagerFragment>();
	}
	@Override
	public Fragment getItem(int position) {
			// return new PagerFragment(position);
		PagerFragment myFragment = new PagerFragment(position);
	    mPageReferenceMap.put(position, myFragment);
	    return myFragment;

	}
	@Override
	public void destroyItem (ViewGroup container, int position, Object object) {
	    super.destroyItem(container, position, object);
	    mPageReferenceMap.remove(position);
	}
	@Override
	public int getCount() {
		 return PageCount;
	}

	public PagerFragment getFragment(int key) {
	    return (PagerFragment) mPageReferenceMap.get(key);
	}
}
