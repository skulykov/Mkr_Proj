#ifndef _SK_TYPES_H
#define _SK_TYPES_H


#ifdef _WIN32
typedef _int64 int64_t;
#else
typedef long long int64_t;
#endif

typedef int Sk_Color;
#define Sk_Clr_Norm	1.0/255.0

#define SK_SCALAR_IS_FLOAT
#define Sk_Scalar float
#define Sk_ScalarHalf(a)         ((a) * 0.5f)


#ifdef SK_SCALAR_IS_FLOAT
#define Sk_float float
static inline float SkIntToScalar(signed int param) {
        return (float)param;
    }
    static inline float SkIntToScalar(unsigned int param) {
        return (float)param;
    }
    static inline float SkIntToScalar(signed long param) {
        return (float)param;
    }
    static inline float SkIntToScalar(unsigned long param) {
        return (float)param;
    }
#else
#define Sk_float double
#endif

#define _PI_ 3.1415926535897932384626433832795f

struct Sk_Point{
	
	Sk_Scalar fX,fY;
};


class Sk_Point3D{
public:
	Sk_Scalar fX,fY,fZ;
	Sk_Point3D operator - (const Sk_Point3D& p) const {
		Sk_Point3D r;
		r.fX = fX - p.fX;
		r.fY = fY - p.fY;
		r.fZ = fZ - p.fZ;
		return r;
	}
};

struct point {
	int x;
	int y;
};
typedef Sk_Point M_POINT;
struct point_rect {
	Sk_Point lu;
	Sk_Point rl;
};

struct color {
	int r,g,b,a;
};

//http://stackoverflow.com/questions/1972003/how-to-use-anonymous-structs-unions-in-c
typedef union { 
   
    struct { // human-friendly access 
        float r; 
        float g; 
        float b; 
        float a; 
    }; 
    float color[4]; 
} M_COLOR; 


typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;

#endif  //_SK_DRAW_H