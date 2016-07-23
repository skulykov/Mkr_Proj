
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <list>
#include <algorithm>
#include <set>

using namespace std;

#include "poly.h"
#include "glCtx.h"

#define TPPL_VERTEXTYPE_REGULAR 0
#define TPPL_VERTEXTYPE_START 1
#define TPPL_VERTEXTYPE_END 2
#define TPPL_VERTEXTYPE_SPLIT 3
#define TPPL_VERTEXTYPE_MERGE 4

TPPLPoly::TPPLPoly() { 
	hole = false;
	numpoints = 0;
	points = NULL;
}

TPPLPoly::~TPPLPoly() {
	if(points) delete [] points;
}

void TPPLPoly::Clear() {
	if(points) delete [] points;
	hole = false;
	numpoints = 0;
	points = NULL;
}

void TPPLPoly::Init(long numpoints) {
	Clear();
	this->numpoints = numpoints;
	points = new TPPLPoint[numpoints];
}

void TPPLPoly::Triangle(TPPLPoint &p1, TPPLPoint &p2, TPPLPoint &p3) {
	Init(3);
	points[0] = p1;
	points[1] = p2;
	points[2] = p3;
}

TPPLPoly::TPPLPoly(const TPPLPoly &src) {
	hole = src.hole;
	numpoints = src.numpoints;
	points = new TPPLPoint[numpoints];
	memcpy(points, src.points, numpoints*sizeof(TPPLPoint));
}

TPPLPoly& TPPLPoly::operator=(const TPPLPoly &src) {
	Clear();
	hole = src.hole;
	numpoints = src.numpoints;
	points = new TPPLPoint[numpoints];
	memcpy(points, src.points, numpoints*sizeof(TPPLPoint));
	return *this;
}

int TPPLPoly::GetOrientation() {
	long i1,i2;
	tppl_float area = 0;
	for(i1=0; i1<numpoints; i1++) {
		i2 = i1+1;
		if(i2 == numpoints) i2 = 0;
		area += points[i1].x * points[i2].y - points[i1].y * points[i2].x;
	}
	if(area>0) return TPPL_CCW;
	if(area<0) return TPPL_CW;
	return 0;
}

void TPPLPoly::SetOrientation(int orientation) {
	int polyorientation = GetOrientation();
	if(polyorientation&&(polyorientation!=orientation)) {
		Invert();
	}
}

void TPPLPoly::Invert() {
	long i;
	TPPLPoint *invpoints;

	invpoints = new TPPLPoint[numpoints];
	for(i=0;i<numpoints;i++) {
		invpoints[i] = points[numpoints-i-1];
	}

	delete [] points;
	points = invpoints;
}

TPPLPoint TPPLPartition::Normalize(const TPPLPoint &p) {
	TPPLPoint r;
	tppl_float n = sqrt(p.x*p.x + p.y*p.y);
	if(n!=0) {
		r = p/n;
	} else {
		r.x = 0;
		r.y = 0;
	}
	return r;
}

tppl_float TPPLPartition::Distance(const TPPLPoint &p1, const TPPLPoint &p2) {
	tppl_float dx,dy;
	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	return(sqrt(dx*dx + dy*dy));
}

//checks if two lines intersect
int TPPLPartition::Intersects(TPPLPoint &p11, TPPLPoint &p12, TPPLPoint &p21, TPPLPoint &p22) {
	if((p11.x == p21.x)&&(p11.y == p21.y)) return 0;
	if((p11.x == p22.x)&&(p11.y == p22.y)) return 0;
	if((p12.x == p21.x)&&(p12.y == p21.y)) return 0;
	if((p12.x == p22.x)&&(p12.y == p22.y)) return 0;

	TPPLPoint v1ort,v2ort,v;
	tppl_float dot11,dot12,dot21,dot22;

	v1ort.x = p12.y-p11.y;
	v1ort.y = p11.x-p12.x;

	v2ort.x = p22.y-p21.y;
	v2ort.y = p21.x-p22.x;

	v = p21-p11;
	dot21 = v.x*v1ort.x + v.y*v1ort.y;
	v = p22-p11;
	dot22 = v.x*v1ort.x + v.y*v1ort.y;

	v = p11-p21;
	dot11 = v.x*v2ort.x + v.y*v2ort.y;
	v = p12-p21;
	dot12 = v.x*v2ort.x + v.y*v2ort.y;

	if(dot11*dot12>0) return 0;
	if(dot21*dot22>0) return 0;

	return 1;
}

//removes holes from inpolys by merging them with non-holes
int TPPLPartition::RemoveHoles(list<TPPLPoly> *inpolys, list<TPPLPoly> *outpolys) {
	list<TPPLPoly> polys;
	list<TPPLPoly>::iterator holeiter,polyiter,iter,iter2;
	long i,i2,holepointindex,polypointindex;
	TPPLPoint holepoint,polypoint,bestpolypoint;
	TPPLPoint linep1,linep2;
	TPPLPoint v1,v2;
	TPPLPoly newpoly;
	bool hasholes;
	bool pointvisible;
	bool pointfound;
	
	//check for trivial case (no holes)
	hasholes = false;
	for(iter = inpolys->begin(); iter!=inpolys->end(); iter++) {
		if(iter->IsHole()) {
			hasholes = true;
			break;
		}
	}
	if(!hasholes) {
		for(iter = inpolys->begin(); iter!=inpolys->end(); iter++) {
			outpolys->push_back(*iter);
		}
		return 1;
	}

	polys = *inpolys;

	while(1) {
		//find the hole point with the largest x
		hasholes = false;
		for(iter = polys.begin(); iter!=polys.end(); iter++) {
			if(!iter->IsHole()) continue;

			if(!hasholes) {
				hasholes = true;
				holeiter = iter;
				holepointindex = 0;
			}

			for(i=0; i < iter->GetNumPoints(); i++) {
				if(iter->GetPoint(i).x > holeiter->GetPoint(holepointindex).x) {
					holeiter = iter;
					holepointindex = i;
				}
			}
		}
		if(!hasholes) break;
		holepoint = holeiter->GetPoint(holepointindex);
		
		pointfound = false;
		for(iter = polys.begin(); iter!=polys.end(); iter++) {
			if(iter->IsHole()) continue;
			for(i=0; i < iter->GetNumPoints(); i++) {
				if(iter->GetPoint(i).x <= holepoint.x) continue;
				if(!InCone(iter->GetPoint((i+iter->GetNumPoints()-1)%(iter->GetNumPoints())),
					iter->GetPoint(i),
					iter->GetPoint((i+1)%(iter->GetNumPoints())),
					holepoint)) 
					continue;
				polypoint = iter->GetPoint(i);
				if(pointfound) {
					v1 = Normalize(polypoint-holepoint);
					v2 = Normalize(bestpolypoint-holepoint);
					if(v2.x > v1.x) continue;				
				}
				pointvisible = true;
				for(iter2 = polys.begin(); iter2!=polys.end(); iter2++) {
					if(iter2->IsHole()) continue;
					for(i2=0; i2 < iter2->GetNumPoints(); i2++) {
						linep1 = iter2->GetPoint(i2);
						linep2 = iter2->GetPoint((i2+1)%(iter2->GetNumPoints()));
						if(Intersects(holepoint,polypoint,linep1,linep2)) {
							pointvisible = false;
							break;
						}
					}
					if(!pointvisible) break;
				}
				if(pointvisible) {
					pointfound = true;
					bestpolypoint = polypoint;
					polyiter = iter;
					polypointindex = i;
				}
			}
		}

		if(!pointfound) return 0;

		newpoly.Init(holeiter->GetNumPoints() + polyiter->GetNumPoints() + 2);
		i2 = 0;
		for(i=0;i<=polypointindex;i++) {
			newpoly[i2] = polyiter->GetPoint(i);
			i2++;
		}
		for(i=0;i<=holeiter->GetNumPoints();i++) {
			newpoly[i2] = holeiter->GetPoint((i+holepointindex)%holeiter->GetNumPoints());
			i2++;
		}
		for(i=polypointindex;i<polyiter->GetNumPoints();i++) {
			newpoly[i2] = polyiter->GetPoint(i);
			i2++;
		}
		
		polys.erase(holeiter);
		polys.erase(polyiter);
		polys.push_back(newpoly);
	}

	for(iter = polys.begin(); iter!=polys.end(); iter++) {
		outpolys->push_back(*iter);
	}
	
	return 1;
}

bool TPPLPartition::IsConvex(TPPLPoint& p1, TPPLPoint& p2, TPPLPoint& p3) {
	tppl_float tmp;
	tmp = (p3.y-p1.y)*(p2.x-p1.x)-(p3.x-p1.x)*(p2.y-p1.y);
	if(tmp>0) return 1;
	else return 0;
}

bool TPPLPartition::IsReflex(TPPLPoint& p1, TPPLPoint& p2, TPPLPoint& p3) {
	tppl_float tmp;
	tmp = (p3.y-p1.y)*(p2.x-p1.x)-(p3.x-p1.x)*(p2.y-p1.y);
	if(tmp<0) return 1;
	else return 0;
}

bool TPPLPartition::IsInside(TPPLPoint& p1, TPPLPoint& p2, TPPLPoint& p3, TPPLPoint &p) {
	if(IsConvex(p1,p,p2)) return false;
	if(IsConvex(p2,p,p3)) return false;
	if(IsConvex(p3,p,p1)) return false;
	return true;
}

bool TPPLPartition::InCone(TPPLPoint &p1, TPPLPoint &p2, TPPLPoint &p3, TPPLPoint &p) {
	bool convex;

	convex = IsConvex(p1,p2,p3);

	if(convex) {
		if(!IsConvex(p1,p2,p)) return false;
		if(!IsConvex(p2,p3,p)) return false;
		return true;
	} else {
		if(IsConvex(p1,p2,p)) return true;
		if(IsConvex(p2,p3,p)) return true;
		return false;
	}
}

bool TPPLPartition::InCone(PartitionVertex *v, TPPLPoint &p) {
	TPPLPoint p1,p2,p3;

	p1 = v->previous->p;
	p2 = v->p;
	p3 = v->next->p;

	return InCone(p1,p2,p3,p);
}

void TPPLPartition::UpdateVertexReflexity(PartitionVertex *v) {
	PartitionVertex *v1,*v3;
	v1 = v->previous;
	v3 = v->next;
	v->isConvex = !IsReflex(v1->p,v->p,v3->p);	
}

void TPPLPartition::UpdateVertex(PartitionVertex *v, PartitionVertex *vertices, int numvertices) {
	int i;
	PartitionVertex *v1,*v3;
	TPPLPoint vec1,vec3;

	v1 = v->previous;
	v3 = v->next;

	v->isConvex = IsConvex(v1->p,v->p,v3->p);

	vec1 = Normalize(v1->p - v->p);
	vec3 = Normalize(v3->p - v->p);
	v->angle = vec1.x*vec3.x + vec1.y*vec3.y;

	if(v->isConvex) {
		v->isEar = true;
		for(i=0;i<numvertices;i++) {
			if((vertices[i].p.x==v->p.x)&&(vertices[i].p.y==v->p.y)) continue;
			if((vertices[i].p.x==v1->p.x)&&(vertices[i].p.y==v1->p.y)) continue;
			if((vertices[i].p.x==v3->p.x)&&(vertices[i].p.y==v3->p.y)) continue;
			if(IsInside(v1->p,v->p,v3->p,vertices[i].p)) {
				v->isEar = false;
				break;
			}
		}
	} else {
		v->isEar = false;
	}
}



//--------------------------------------------------------------
int TPPLPartition::my_Triangulate_EC(TPPLPoly *poly,std::list<TPPLPoly> *triangles) {
	int numvertices;
	PartitionVertex *vertices;
	PartitionVertex *ear;
	TPPLPoly triangle;
	int i,j;
	bool earfound;

	if(poly->GetNumPoints() < 3) return 0;
	if(poly->GetNumPoints() == 3) {
		triangles->push_back(*poly);
		return 1;
	}

	numvertices = poly->GetNumPoints();

	vertices = new PartitionVertex[numvertices];

	for(i=0;i<numvertices;i++) {
		vertices[i].isActive = true;
		vertices[i].p = poly->GetPoint(i);
		if(i==(numvertices-1)) vertices[i].next=&(vertices[0]);
		else vertices[i].next=&(vertices[i+1]);
		if(i==0) vertices[i].previous = &(vertices[numvertices-1]);
		else vertices[i].previous = &(vertices[i-1]);
	}
	for(i=0;i<numvertices;i++) {
		UpdateVertex(&vertices[i],vertices,numvertices);
	}

	for(i=0;i<numvertices-3;i++) {
		earfound = false;
		//find the most extruded ear
		for(j=0;j<numvertices;j++) {
			if(!vertices[j].isActive) continue;
			if(!vertices[j].isEar) continue;
			if(!earfound) {
				earfound = true;
				ear = &(vertices[j]);
			} else {
				if(vertices[j].angle > ear->angle) {
					ear = &(vertices[j]);				
				}
			}
		}
		if(!earfound) {
			delete [] vertices;
			return 0;
		}

		triangle.Triangle(ear->previous->p,ear->p,ear->next->p);
		triangles->push_back(triangle);

		ear->isActive = false;
		ear->previous->next = ear->next;
		ear->next->previous = ear->previous;

		if(i==numvertices-4) break;

		UpdateVertex(ear->previous,vertices,numvertices);
		UpdateVertex(ear->next,vertices,numvertices);
	}
	for(i=0;i<numvertices;i++) {
		if(vertices[i].isActive) {
			triangle.Triangle(vertices[i].previous->p,vertices[i].p,vertices[i].next->p);
			triangles->push_back(triangle);
			break;
		}
	}

	delete [] vertices;

	return 1;
}

//--------------------------------------------------------------


#pragma region Geometry Intersection
//http://geomalgorithms.com/a01-_area.html#area2D_Polygon()


// Assume that classes are already given for the objects:
//    Point and Vector with
//        coordinates {float x, y, z;}
//        operators for:
//            == to test  equality
//            != to test  inequality
//            Point   = Point ± Vector
//            Vector =  Point - Point
//            Vector =  Scalar * Vector    (scalar product)
//            Vector =  Vector * Vector    (3D cross product)
//    Line and Ray and Segment with defining  points {Point P0, P1;}
//        (a Line is infinite, Rays and  Segments start at P0)
//        (a Ray extends beyond P1, but a  Segment ends at P1)
//    Plane with a point and a normal {Point V0; Vector  n;}
//===================================================================
 

#define SMALL_NUM   0.00000001 // anything that avoids division overflow
// dot product (3D) which allows vector operations in arguments
//#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y )
#define perp(u,v)  ((u).x * (v).y - (u).y * (v).x)  // perp product  (2D)
 


struct Segment 
{
	TPPLPoint P0,P1;

};
#define Vector TPPLPoint 
int inSegment( TPPLPoint P, Segment S);

// intersect2D_2Segments(): find the 2D intersection of 2 finite segments
//    Input:  two finite segments S1 and S2
//    Output: *I0 = intersect point (when it exists)
//            *I1 =  endpoint of intersect segment [I0,I1] (when it exists)
//    Return: 0=disjoint (no intersect)
//            1=intersect  in unique point I0
//            2=overlap  in segment from I0 to I1
int intersect2D_2Segments( Segment S1, Segment S2, TPPLPoint* I0, TPPLPoint* I1 )
{
    Vector    u = S1.P1 - S1.P0;
    Vector    v = S2.P1 - S2.P0;
    Vector    w = S1.P0 - S2.P0;
    float     D = perp(u,v);

    // test if  they are parallel (includes either being a point)
    if (fabs(D) < SMALL_NUM) {           // S1 and S2 are parallel
        if (perp(u,w) != 0 || perp(v,w) != 0)  {
            return 0;                    // they are NOT collinear
        }
        // they are collinear or degenerate
        // check if they are degenerate  points
        float du = dot(u,u);
        float dv = dot(v,v);
        if (du==0 && dv==0) {            // both segments are points
            if (S1.P0 !=  S2.P0)         // they are distinct  points
                 return 0;
            *I0 = S1.P0;                 // they are the same point
            return 1;
        }
        if (du==0) {                     // S1 is a single point
            if  (inSegment(S1.P0, S2) == 0)  // but is not in S2
                 return 0;
            *I0 = S1.P0;
            return 1;
        }
        if (dv==0) {                     // S2 a single point
            if  (inSegment(S2.P0, S1) == 0)  // but is not in S1
                 return 0;
            *I0 = S2.P0;
            return 1;
        }
        // they are collinear segments - get  overlap (or not)
        float t0, t1;                    // endpoints of S1 in eqn for S2
        Vector w2 = S1.P1 - S2.P0;
        if (v.x != 0) {
                 t0 = w.x / v.x;
                 t1 = w2.x / v.x;
        }
        else {
                 t0 = w.y / v.y;
                 t1 = w2.y / v.y;
        }
        if (t0 > t1) {                   // must have t0 smaller than t1
                 float t=t0; t0=t1; t1=t;    // swap if not
        }
        if (t0 > 1 || t1 < 0) {
            return 0;      // NO overlap
        }
        t0 = t0<0? 0 : t0;               // clip to min 0
        t1 = t1>1? 1 : t1;               // clip to max 1
        if (t0 == t1) {                  // intersect is a point
            // svk ?? *I0 = S2.P0 +  t0 * v;
            return 1;
        }

        // they overlap in a valid subsegment
        /* svk *I0 = S2.P0 + t0 * v;
        *I1 = S2.P0 + t1 * v;*/
        return 2;
    }

    // the segments are skew and may intersect in a point
    // get the intersect parameter for S1
    float     sI = perp(v,w) / D;
    if (sI < 0 || sI > 1)                // no intersect with S1
        return 0;

    // get the intersect parameter for S2
    float     tI = perp(u,w) / D;
    if (tI < 0 || tI > 1)                // no intersect with S2
        return 0;

    //svk *I0 = S1.P0 + sI * u;                // compute S1 intersect point
    return 1;
}
//===================================================================
 


// inSegment(): determine if a point is inside a segment
//    Input:  a point P, and a collinear segment S
//    Return: 1 = P is inside S
//            0 = P is  not inside S
int inSegment( TPPLPoint P, Segment S)
{
    if (S.P0.x != S.P1.x) {    // S is not  vertical
        if (S.P0.x <= P.x && P.x <= S.P1.x)
            return 1;
        if (S.P0.x >= P.x && P.x >= S.P1.x)
            return 1;
    }
    else {    // S is vertical, so test y  coordinate
        if (S.P0.y <= P.y && P.y <= S.P1.y)
            return 1;
        if (S.P0.y >= P.y && P.y >= S.P1.y)
            return 1;
    }
    return 0;
}

bool isIntersect(Sk_Path *path1,Sk_Path *path2)
{
	Segment S1, S2;
	S1.P0.x=path1->arr[0].fX;
	S1.P0.y=path1->arr[0].fY;
	S1.P1.x=path1->arr[path1->len-1].fX;
	S1.P1.y=path1->arr[path1->len-1].fY;
	
	S2.P0.x=path2->arr[0].fX;
	S2.P0.y=path2->arr[0].fY;
	S2.P1.x=path2->arr[path2->len-1].fX;
	S2.P1.y=path2->arr[path2->len-1].fY;
	

	TPPLPoint I0,I1;

	int rv=intersect2D_2Segments( S1,  S2, &I0, &I1 );
	if(rv==0)
		return false;
	else
		return true;
}
//===================================================================


#pragma endregion
