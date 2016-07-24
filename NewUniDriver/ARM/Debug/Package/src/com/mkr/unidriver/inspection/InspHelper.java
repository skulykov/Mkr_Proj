package com.mkr.unidriver.inspection;

import java.util.List;

import com.google.protobuf.InvalidProtocolBufferException;
import com.mkr.models.proto.CommPackets.PInspectionCategoryRow;
import com.mkr.models.proto.CommPackets.PInspectionCategoryRowList;
import com.mkr.models.proto.CommPackets.PInspectionItem;
import com.mkr.models.proto.CommPackets.PInspectionRow;
import com.mkr.models.proto.CommPackets.PInspectionRow.Builder;
import com.mkr.utils.GConst;

public class InspHelper {
/*
	public static boolean isContainDefect(PInspectionItem ii)
	{
		return (ii.getIsDefect() || ii.getComments() != "" || isContainMedia(ii));
	}
	public static boolean isContainMedia(PInspectionItem ii)
	{
		return ( ii.getMediaFilesList() != null && ii.getMediaFilesCount() > 0);
	}
	
	//------------CatInspectionRowElement----------
			public static boolean processItemDefectsInCats(PInspectionItem _item,String attID)
			{
				boolean rv = false;
				if (_item.getHasChildren() == 0) {
					if (InspHelper.isContainDefect(_item)) {
						if (_item.getAttID() == attID)
							rv = true;
						else
							rv = false;
					} else
						rv= false;
				}
				else if (_item.getHasChildren() > 0) {
					List<PInspectionItem> cldr=_item.getChildrenList();
					if (cldr != null) {
						boolean flag = false;
						for (PInspectionItem ii : cldr) {
							boolean rv1=processItemDefectsInCats (ii,attID);
							if (rv1) {
								flag = true;

							}
						}
						if (flag) {
							_item.toBuilder().setIsDefect(true);
							rv = true;
						}

					}

				}
				return rv;
			}

			static PInspectionItem getDefectInCats(List<PInspectionItem> arDef,int catID,PInspectionItem ii,String attID)
			{

				for (PInspectionItem item : arDef) 
				{
					if (item.getCID() == catID && item.getIID() == ii.getIID() && item.getAttID()==attID) {
						return item;
					}
				}
				return null;
			}
			public static boolean processItemDefectsInCats(PInspectionItem item,List<PInspectionItem> arDef,int catID,String attID)
			{
				boolean rv = false;
				if (item.getHasChildren() == 0) {
					PInspectionItem it=getDefectInCats(arDef,catID,item,attID);
					if (it != null) {
						item.toBuilder().setIsDefect(true);
						item.toBuilder().setComments(it.getComments());
						for(int i=0;i<it.getMediaFilesCount();i++)
						  item.toBuilder().setMediaFiles(i, it.getMediaFiles(i));
						
						item.toBuilder().setClrd(it.getClrd());
						rv= true;
					} else
						rv= false;
				}
				else if (item.getHasChildren() > 0) {
					List<PInspectionItem> cldr=item.getChildrenList();
					if (cldr != null) {
						boolean flag = false;
						boolean cleared = true;
						for (PInspectionItem ii : cldr) {
							boolean rv1=processItemDefectsInCats (ii, arDef,catID,attID);
							if (rv1) {
								flag = true;
								if (ii.getClrd() != "1")
									cleared = false;
							}
						}
						if (flag) {
							item.toBuilder().setIsDefect(true);
							if (cleared)
								item.toBuilder().setClrd("1");
							rv = true;
						}

					}

				}
				return rv;
			}

	
	
	//------------------SubCategoryRowElement------------------------------
			public static boolean processItemDefectsInSubCats(PInspectionItem _item)
			{
				boolean rv = false;
				if (_item.getHasChildren() == 0) {
					if (InspHelper.isContainDefect(_item)) {
						rv= true;
					} else
						rv= false;
				}
				else if (_item.getHasChildren() > 0) {
					List<PInspectionItem> cldr=_item.getChildrenList();
					if (cldr != null) {
						boolean flag = false;
						for (PInspectionItem ii :  cldr) {
							boolean rv1=processItemDefectsInSubCats (ii);
							if (rv1) {
								flag = true;

							}
						}
						if (flag) {
							_item.toBuilder().setIsDefect( true);
							rv = true;
						}

					}

				}
				return rv;
			}

			static PInspectionItem getDefectInSubCats(List<PInspectionItem> arDef,int catID,int IspItemID)
			{

				for (PInspectionItem item : arDef) 
				{
					if (item.getCID() == catID && item.getIID() == IspItemID) {
						return item;
					}
				}
				return null;
			}
			public static boolean processItemDefectsInSubCats(PInspectionItem item,List<PInspectionItem> arDef,int catID)
			{
				boolean rv = false;
				if (item.getHasChildren() == 0) {
					PInspectionItem it=getDefectInSubCats(arDef,catID,item.getIID());
					if (it != null) {
						item.toBuilder().setIsDefect(true);
						item.toBuilder().setComments(it.getComments());
						for(int i=0;i<it.getMediaFilesCount();i++)
						  item.toBuilder().setMediaFiles(i, it.getMediaFiles(i));
						
						item.toBuilder().setClrd(it.getClrd());
						rv= true;
					} else
						rv= false;
				}
				else if (item.getHasChildren() > 0) {
					List<PInspectionItem> cldr=item.getChildrenList();
					if (cldr != null) {
						boolean flag = false;
						boolean cleared = true;
						for (PInspectionItem ii : cldr) {
							boolean rv1=processItemDefectsInSubCats (ii, arDef,catID);
							if (rv1) {
								flag = true;
								if (ii.getClrd() != "1")
									cleared = false;
							}
						}
						if (flag) {
							item.toBuilder().setIsDefect(true);
							if (cleared)
								item.toBuilder().setClrd("1");
							rv = true;
						}

					}

				}
				return rv;
			}
*/

			//---------------------------------------------------------------------------------
			// make sure that we have at least one Defect in this report
			
			


	//http://stackoverflow.com/questions/29209153/making-a-small-change-to-a-java-protocol-buffers-object
	/*
	 * 
   
 
If you prefer, you can always keep your objects in Builder form long-term rather than immutable message form, 
and then you can keep a pointer to the particular sub-builder you wish to modify and modify it repeatedly. 
But watch out for the usual design issues with mutable objects. – Kenton Varda Mar 26 '15 at 8:50  		
	 */
			/*
	public static void AddDefect(PInspectionItem item) {
		
		InspCatFormActivity act=InspCatFormActivity.Get();
		Builder b=GConst.g_IR_Builder;
		if(act.IsNew()&&b!=null)
		{
			int found_ii = -1,index=0;
			for (PInspectionItem ii : b.getDefectsList()) {
				if (ii.getIID() == item.getIID()) {
					found_ii = index;
					break;
				}
				index++;
			}
			if (found_ii != -1)
				b.removeDefects(found_ii);//b.getDefectsList().remove (found_ii);
			
			//b.getDefectsList().add(item);
			b.addDefects(item);//b.setDefects(0,item);
		}
		
		
	}
	*/
			public static boolean isEditable()// make sure that we have at least one Defect in this report
			{
				InspCatFormActivity act=InspCatFormActivity.Get();
				return act.IsNew();
			}
			 /*public static PInspectionCategoryRowList CreateInspection(int InsRID,int is_new,int BoxID)
			 {
				 byte[] b=createInspection(InsRID,is_new,BoxID);
				 if(b!=null)
				 {
					 PInspectionCategoryRowList rv=null;
					 try {
						rv=PInspectionCategoryRowList.parseFrom(b);
					} catch (InvalidProtocolBufferException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					 return rv;
				 }
				 return null;
			 }*/
			 public static PInspectionItem GetDefect(int iID)
			 {
				 byte[] b=getDefect(iID);
				 if(b!=null)
				 {
					 PInspectionItem rv=null;
					 try {
						rv=PInspectionItem.parseFrom(b);
					} catch (InvalidProtocolBufferException e) {
						e.printStackTrace();
					}
					 return rv;
				 }
				 return null;
			 }
			 public static PInspectionCategoryRow GetCatRow(int position)
			 {
				 byte[] b=getCatRow(position);
				 if(b!=null)
				 {
					 PInspectionCategoryRow rv=null;
					 try {
						rv= PInspectionCategoryRow.parseFrom(b);
					} catch (InvalidProtocolBufferException e) {
						e.printStackTrace();
					}
					 return rv;
				 }
				 return null;
			 }
			
	public static native /*byte[]*/int createInspection(int irID,int is_new,int BoxID,int inspType);	
	public static native byte[] getDefect(int iID);
	public static native int addDefect(byte[] b);
	public static native boolean isContainsDefects();
	public static native boolean processItemDefectsInSubCats(int iID);
	
	//public static native int  getCatColor(int catID,int attID);
	public static native int   getIIColor(int iID);
	
	public static native byte[]  getCatRow(int position);
	
	public static native int   setAllChecked(int v);
	public static native int   getAllChecked(int odo);
	
}
