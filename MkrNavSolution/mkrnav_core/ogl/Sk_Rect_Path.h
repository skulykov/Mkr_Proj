#ifndef _SK_RECT_H
#define _SK_RECT_H

#include "Sk_Types.h"



struct Sk_Rect{
	//int left,right,top,bottom;
	/*bool intersects(Sk_Rect r)
	{
		 return
			(left < r.right
		  && right > r.left
		  && top < r.bottom
		  && bottom > r.top);
	}*/


	Sk_Scalar fLeft, fTop, fRight, fBottom;

	bool isEmpty() const { return fLeft >= fRight || fTop >= fBottom; }

	Sk_Scalar    left() const { return fLeft; }
    Sk_Scalar    top() const { return fTop; }
    Sk_Scalar    right() const { return fRight; }
    Sk_Scalar    bottom() const { return fBottom; }
    Sk_Scalar    width() const { return fRight - fLeft; }
    Sk_Scalar    height() const { return fBottom - fTop; }
    Sk_Scalar    centerX() const { return Sk_ScalarHalf(fLeft + fRight); }
    Sk_Scalar    centerY() const { return Sk_ScalarHalf(fTop + fBottom); }
   


	/**
     *  Return true if rectangles a and b are not empty and intersect.
     */
    static bool Intersects(const Sk_Rect& a, const Sk_Rect& b) {
        return  !a.isEmpty() && !b.isEmpty() &&
                a.fLeft < b.fRight && b.fLeft < a.fRight &&
                a.fTop < b.fBottom && b.fTop < a.fBottom;
    }
	static Sk_Rect  MakeLTRB(Sk_Scalar l, Sk_Scalar t, Sk_Scalar r, Sk_Scalar b) {
        Sk_Rect rect;
        rect.set(l, t, r, b);
        return rect;
    }
	static Sk_Rect  MakeXYWH(Sk_Scalar x, Sk_Scalar y, Sk_Scalar w, Sk_Scalar h) {
        Sk_Rect r;
        r.set(x, y, x + w, y + h);
        return r;
    }
	void set(Sk_Scalar left, Sk_Scalar top, Sk_Scalar right, Sk_Scalar bottom) {
        fLeft   = left;
        fTop    = top;
        fRight  = right;
        fBottom = bottom;
    }


	/** Offset set the rectangle by adding dx to its left and right,
        and adding dy to its top and bottom.
    */
    void offset(Sk_Scalar dx, Sk_Scalar dy) {
        fLeft   += dx;
        fTop    += dy;
        fRight  += dx;
        fBottom += dy;
    }

    void offset(const Sk_Point& delta) {
        this->offset(delta.fX, delta.fY);
    }

	 /** Inset the rectangle by (dx,dy). If dx is positive, then the sides are
        moved inwards, making the rectangle narrower. If dx is negative, then
        the sides are moved outwards, making the rectangle wider. The same holds
         true for dy and the top and bottom.
    */
    void inset(Sk_Scalar dx, Sk_Scalar dy)  {
        fLeft   += dx;
        fTop    += dy;
        fRight  -= dx;
        fBottom -= dy;
    }

	/**
     *  Returns true if (p.fX,p.fY) is inside the rectangle, and the rectangle
     *  is not empty.
     *
     *  Contains treats the left and top differently from the right and bottom.
     *  The left and top coordinates of the rectangle are themselves considered
     *  to be inside, while the right and bottom are not. Thus for the rectangle
     *  {0, 0, 5, 10}, (0,0) is contained, but (0,10), (5,0) and (5,10) are not.
     */
    bool contains(const Sk_Point& p) const {
        return !this->isEmpty() &&
               fLeft <= p.fX && p.fX < fRight && fTop <= p.fY && p.fY < fBottom;
    }
	 /**
     *  Returns true if (x,y) is inside the rectangle, and the rectangle
     *  is not empty.
     *
     *  Contains treats the left and top differently from the right and bottom.
     *  The left and top coordinates of the rectangle are themselves considered
     *  to be inside, while the right and bottom are not. Thus for the rectangle
     *  {0, 0, 5, 10}, (0,0) is contained, but (0,10), (5,0) and (5,10) are not.
     */
    bool contains(Sk_Scalar x, Sk_Scalar y) const {
        return  !this->isEmpty() &&
                fLeft <= x && x < fRight && fTop <= y && y < fBottom;
    }

    /**
     *  Return true if this rectangle contains r, and if both rectangles are
     *  not empty.
     */
    bool contains(const Sk_Rect& r) const {
        return  !r.isEmpty() && !this->isEmpty() &&
                fLeft <= r.fLeft && fTop <= r.fTop &&
                fRight >= r.fRight && fBottom >= r.fBottom;
    }

	/**
     *  Update this rectangle to enclose itself and the specified rectangle.
     *  If this rectangle is empty, just set it to the specified rectangle.
     *  If the specified rectangle is empty, do nothing.
     */
    void join(Sk_Scalar left, Sk_Scalar top, Sk_Scalar right, Sk_Scalar bottom)
	{
		 // do nothing if the params are empty
		if (left >= right || top >= bottom) {
			return;
		}

		// if we are empty, just assign
		if (fLeft >= fRight || fTop >= fBottom) {
			this->set(left, top, right, bottom);
		} else {
			if (left < fLeft) fLeft = left;
			if (top < fTop) fTop = top;
			if (right > fRight) fRight = right;
			if (bottom > fBottom) fBottom = bottom;
		}
	}

    /** Update this rectangle to enclose itself and the specified rectangle.
        If this rectangle is empty, just set it to the specified rectangle. If the specified
        rectangle is empty, do nothing.
    */
    void join(const Sk_Rect& r) {
        this->join(r.fLeft, r.fTop, r.fRight, r.fBottom);
    }

};

struct Sk_Path
{
	int ref;
	int len;
	Sk_Point3D *arr;//Sk_Point *arr;
	
	Sk_Path(int size)
	{
		this->len=size;
		this->arr=new Sk_Point3D[size];
		this->ref=0;
	}
	~Sk_Path()
	{
		if(this->arr!=NULL)
			delete[] this->arr;
		this->arr = 0;
	}

	int countPoints(){return len;}
    int getPoints(Sk_Point3D *points,int _len)
	{
		if(_len>this->len)
			return -1;
		memcpy(points,this->arr,_len*sizeof(Sk_Point3D));
		return len;
	}
	
	void set(int ind,Sk_Scalar x,Sk_Scalar y,Sk_Scalar z=0)
	{
		this->arr[ind].fX=x;
		this->arr[ind].fY=y;
		this->arr[ind].fZ=z;
	}
};

#endif  //_SK_RECT_H