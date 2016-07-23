#ifndef _SK_PAINT_H
#define _SK_PAINT_H

#include <algorithm>

#include "Sk_Rect_Path.h"
#include "Sk_Atlas.h"

// 64bits wide, with a 16bit bias. Useful when accumulating lots of 16.16 so
// we don't overflow along the way
typedef /*int64_t*/ float Sk48Dot16;

//#ifdef SK_SCALAR_IS_FLOAT
//    static inline float Sk48Dot16ToScalar(Sk48Dot16 x) {
//        return (float) (x * 1.5258789e-5);   // x * (1 / 65536.0f)
//    }
//#else
//    static inline SkFixed Sk48Dot16ToScalar(Sk48Dot16 x) {
//        // just return the low 32bits
//        return static_cast<SkFixed>(x);
//    }
//#endif
#define _SkIntToScalar_
static void join_bounds_x(CBitmap_S *g, Sk_Rect* bounds, Sk48Dot16 dx) {
    Sk_Scalar sx = dx;//Sk48Dot16ToScalar(dx);
    bounds->join(_SkIntToScalar_(g->bl) + sx,
	             _SkIntToScalar_(g->bt),
                 _SkIntToScalar_(g->bl + g->bw) + sx,
                 _SkIntToScalar_(g->bt + g->bh));
}

static void join_bounds_y(CBitmap_S *g, Sk_Rect* bounds, Sk48Dot16 dy) {
    Sk_Scalar sy = dy;//Sk48Dot16ToScalar(dy);
    bounds->join(_SkIntToScalar_(g->bl),
                 _SkIntToScalar_(g->bt) + sy,
                 _SkIntToScalar_(g->bl + g->bw),
                 _SkIntToScalar_(g->bt + g->bh) + sy);
}
typedef void (*JoinBoundsProc)(CBitmap_S *, Sk_Rect*, Sk48Dot16);

class Sk_Paint
{
	short fStyle,fCapStyle; 
    Sk_Color    fColor;  // area color
	Sk_Color    bColor;  // boundaries color;
	Sk_Scalar fWidth;    // stroke width
	int textSize;

	bool isVerticalText() const {
        return false;//SkToBool(this->getFlags() & kVerticalText_Flag);
    }
public:

	/** Describes how to interpret the text parameters that are passed to paint
        methods like measureText() and getTextWidths().
    */
    enum TextEncoding {
        kUTF8_TextEncoding,     //!< the text parameters are UTF8
        kUTF16_TextEncoding,    //!< the text parameters are UTF16
        kUTF32_TextEncoding,    //!< the text parameters are UTF32
        kGlyphID_TextEncoding   //!< the text parameters are glyph indices
    };

    enum Style {
        kFill_Style,            //!< fill the geometry
        kStroke_Style,          //!< stroke the geometry
        kStrokeAndFill_Style,   //!< fill and stroke the geometry

        kStyleCount
    };
	 enum CapStyle {
        kButt_Cap=1,            //!< fill the geometry
        kRound_Cap,          //!< stroke the geometry
        kSquare_Cap
    };

    /** Return the paint's style, used for controlling how primitives'
        geometries are interpreted (except for drawBitmap, which always assumes
        kFill_Style).
        @return the paint's Style
    */
    Style getStyle() const { return (Style)fStyle; }
	void setStyle(Style style){fStyle = style;};
	
	Sk_Color getColor() const { return fColor; }
    void setColor(Sk_Color color){fColor=color;};

	Sk_Color getBColor() const { return bColor; }
    void setBColor(Sk_Color color){bColor=color;};

    void setStrokeWidth(Sk_Scalar width){fWidth = width;}
	Sk_Scalar getStrokeWidth() const { return fWidth; }

	 void setTextSize(int size){textSize = size;}
	int getTextSize() const { return textSize; }
	 
	void setStrokeCap(CapStyle style){fCapStyle = style;};
	CapStyle getStrokeCap(){return (CapStyle)fCapStyle;};

	 /** Return the width of the text. This will return the vertical measure
     *  if isVerticalText() is true, in which case the returned value should
     *  be treated has a height instead of a width.
     *
     *  @param text         The text to be measured
     *  @param length       Number of bytes of text to measure
     *  @param bounds       If not NULL, returns the bounds of the text,
     *                      relative to (0, 0).
     *  @param scale        If not 0, return width as if the canvas were scaled
     *                      by this value
     *  @return             The advance width of the text
     */
#if 0
	Sk_Scalar measureText(Sk_Atlas *a,const void* text, size_t length,
                         Sk_Rect* bounds, Sk_Scalar scale = 0) const
	{
		int xyIndex;
		JoinBoundsProc joinBoundsProc;
	 
		if (this->isVerticalText()) {
			xyIndex = 1;
			joinBoundsProc = join_bounds_y;
		} else {
			xyIndex = 0;
			joinBoundsProc = join_bounds_x;
		}

		int         n = 1;
		const char *p = (const char*)text ;

        Sk48Dot16 x = a->c[*p].ax; //advance(*g, xyIndex);
        p++;
		for (; n<length; n++) {
                struct CBitmap_S *b=&a->c[*p];
                joinBoundsProc(b, bounds, x);
                x += b->ax;
				p++;
        }

		return n;
	}
#else
	Sk_Scalar measureText(Sk_Atlas *a,const void* text, size_t length,
                         Sk_Rect* bounds, Sk_Scalar scale = 0) const
	{
		
		int         n = 1;
		const char *p = (const char*)text ;
		Sk_Rect r;
        int x = a->c[*p].ax; //advance(*g, xyIndex);
		int h= a->c[*p].bh;
        p++;
		for (; n<length; n++) {
                struct CBitmap_S *b=&a->c[*p];
                x += b->ax;
				h=max((float)h,b->bh);
				p++;
        }
		r.set(0,0,x,h);
		bounds->join(r);
		
		return n;
	}
#endif
    /** Return the width of the text. This will return the vertical measure
     *  if isVerticalText() is true, in which case the returned value should
     *  be treated has a height instead of a width.
     *
     *  @param text     Address of the text
     *  @param length   Number of bytes of text to measure
     *  @return         The advance width of the text
     */
   /* Sk_Scalar measureText(const void* text, size_t length) const {
        return this->measureText(text, length, NULL, 0);
    }*/


};

#endif  //_SK_PAINT_H