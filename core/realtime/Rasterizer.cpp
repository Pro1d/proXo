#include "../math/type.h"
#include "Buffer.h"
#include <algorithm>
#include <cmath>

using std::min;
using std::max;

/** A : UP, B : LEFT, C : RIGHT, By=Cy
 *	remplit les pixels dont le centre est contenu dans le triangle
 **/
void triangleSup(Buffer & buffer, const vec3 A, const vec3 B, const vec3 C, const vec3 colorA, const vec3 colorB, const vec3 colorC) {
	/// suffix -> s : start (côté gauche/haut), e : end (côté droit/bas)
	/// preffix -> v : vecteur, d : delta (variation), l : ligne (concerne la ligne actuelle)

	/// ys in[i-0.5; i-0.5[ -> ys' = i -> ys'=ys'+0.5
	/// ye in[i-0.5; i-0.5[ -> ye' = i -> ye'=ye'-0.5
	/// => ys in triangle ABC, ye in triangle ABC

	/// coord y de bord sup du pixel dont le centre est strictement après A
	//real ysmin = round(A[1]);
	/// coord y du centre (du pixel) [strictement après/avant] A/B (=> [ys-1 <=] A[1] < ys) (si A est au centre d'un pixel, le pixel n'est pas dans le triangle)
	const real ys = max((real)round(A[1]), (real) 0.0) + (real) 0.5;
	const integer ye = min((integer)round(B[1]), (integer) buffer.height);
	/// coord y du vecteur A->B (=A->C)
	const real vy = B[1]-A[1];
	/// A et BC sont sur la même ligne horizontale -> le triangle est plat, il ne contient aucun point
	if(vy <= 0) return;
	const real vy_inv = 1 / vy;

	/// var en x du bord gauche et droite par rapport à y
	const real dxs = (B[0]-A[0]) * vy_inv, dxe = (C[0]-A[0]) * vy_inv;
	/// var en z du bord gauche et droite par rapport à y
	const real dzs = (B[2]-A[2]) * vy_inv, dze = (C[2]-A[2]) * vy_inv;
	/// var de la couleur par rapport à y
	const real drs = (colorB[0]-colorA[0]) * vy_inv, dre = (colorC[0]-colorA[0]) * vy_inv;
	const real dgs = (colorB[1]-colorA[1]) * vy_inv, dge = (colorC[1]-colorA[1]) * vy_inv;
	const real dbs = (colorB[2]-colorA[2]) * vy_inv, dbe = (colorC[2]-colorA[2]) * vy_inv;

    /// différence de y entre le sommet du triangle et le premier pixel de la première ligne
    const real vy1 = (ys-A[1]);
	/// coord en x du centre du premier pixel de la première ligne
	real xs = A[0] + dxs*vy1, xe = A[0] + dxe*vy1;
	/// coord en z du centre du premier pixel de la première  ligne
	real zs = A[2] + dzs*vy1, ze = A[2] + dze*vy1;
	/// couleur au centre du premier pixel de la première  ligne
	real rs = colorA[0] + drs*vy1, re = colorA[0] + dre*vy1;
	real gs = colorA[1] + dgs*vy1, ge = colorA[1] + dge*vy1;
	real bs = colorA[2] + dbs*vy1, be = colorA[2] + dbe*vy1;

	for(integer y = (integer) ys; y <= ye; y++)
	{
		/// coord en x du centre du pixel [strictement après/avant] le côté gauche/droit
		const real lxs = max((real)round(xs), (real) 0.0) + (real) 0.5;
		const integer  lxe = min((positive)round(xe), buffer.width);
		const real lvx = xe - xs;
		const real lvx_inv = 1 / lvx;
		/// var en z par rapport à x
		const real ldz = (ze - zs) * lvx_inv;
		/// var de la couleur par rapport à x
		const real ldr = (re - rs) * lvx_inv;
		const real ldg = (ge - gs) * lvx_inv;
		const real ldb = (be - bs) * lvx_inv;

		/// Différence entre le bord de gauche et le premier pixel de la ligne
		const real vx1 = (lxs-xs);
		/// coord z du point associé au centre du premier pixel de la première  ligne
		real lz = zs + ldz*vx1;
		/// couleur du point associé au centre du premier pixel de la première  ligne
		real lr = rs + ldr*vx1;
		real lg = gs + ldg*vx1;
		real lb = bs + ldb*vx1;

		// Dessine une ligne
		for(integer lx = (integer) lxs; lx < lxe; lx++) {
			real * pts = buffer.getPtr(lx, y);
			// depth test
			if(lz < pts[BUF_Z_OFFSET]) {
				pts[BUF_Z_OFFSET] = lz;
				pts[BUF_R_OFFSET] = lr;
				pts[BUF_G_OFFSET] = lg;
				pts[BUF_B_OFFSET] = lb;
			}

			lz += ldz;
			lr += ldr;
			lg += ldg;
			lb += ldb;
		}

		xs += dxs; xe += dxe;
		zs += dzs; ze += dze;
		rs += drs; re += dre;
		gs += dgs; ge += dge;
		bs += dbs; be += dbe;
	}
}
/** A : DOWN, B : LEFT, C : RIGHT, By=Cy
 *	remplit les pixels dont le centre^+ est contenu dans le triangle
 **/
void triangleInf(Buffer & buffer, const vec3 A, const vec3 B, const vec3 C, const vec3 colorA, const vec3 colorB, const vec3 colorC) {
	/// suffix -> s : start (côté gauche/haut), e : end (côté droit/bas)
	/// preffix -> v : vecteur, d : delta (variation), l : ligne (concerne la ligne actuelle)

	/// ys in[i-0.5; i-0.5[ -> ys' = i -> ys'=ys'+0.5
	/// ye in[i-0.5; i-0.5[ -> ye' = i -> ye'=ye'-0.5
	/// => ys in triangle ABC, ye in triangle ABC

	/// coord y de bord sup du pixel dont le centre est strictement après A
	//real ysmin = round(A[1]);
	/// coord y du centre (du pixel) [strictement après/avant] A/B (=> [ys-1 <=] A[1] < ys) (si A est au centre d'un pixel, le pixel n'est pas dans le triangle)
	const real ys = max((real)round(B[1]), (real) 0.0) + (real) 0.5;
	const integer  ye = min((integer)round(A[1]), (integer) buffer.height);
	/// coord y du vecteur B->A.y (=C->Ay)
	const real vy = A[1]-B[1];
	/// A et BC sont sur la même ligne horizontale -> le triangle est plat, il ne contient aucun point
	if(vy <= 0) return;
	const real vy_inv = 1 / vy;

	/// var en x du bord gauche et droite par rapport à y
	const real dxs = (A[0]-B[0]) * vy_inv, dxe = (A[0]-C[0]) * vy_inv;
	/// var en z du bord gauche et droite par rapport à y
	const real dzs = (A[2]-B[2]) * vy_inv, dze = (A[2]-C[2]) * vy_inv;
	/// var de la couleur par rapport à y
	const real drs = (colorA[0]-colorB[0]) * vy_inv, dre = (colorA[0]-colorC[0]) * vy_inv;
	const real dgs = (colorA[1]-colorB[1]) * vy_inv, dge = (colorA[1]-colorC[1]) * vy_inv;
	const real dbs = (colorA[2]-colorB[2]) * vy_inv, dbe = (colorA[2]-colorC[2]) * vy_inv;

    /// différence de y entre le haut du triangle et le premier pixel de la première ligne
    const real vy1 = (ys-B[1]);
	/// coord en x du centre du premier pixel de la première ligne
	real xs = B[0] + dxs*vy1, xe = C[0] + dxe*vy1;
	/// coord en z du centre du premier pixel de la première  ligne
	real zs = B[2] + dzs*vy1, ze = C[2] + dze*vy1;
	/// couleur au centre du premier pixel de la première  ligne
	real rs = colorB[0] + drs*vy1, re = colorC[0] + dre*vy1;
	real gs = colorB[1] + dgs*vy1, ge = colorC[1] + dge*vy1;
	real bs = colorB[2] + dbs*vy1, be = colorC[2] + dbe*vy1;

	for(integer y = (integer) ys; y <= ye; y++)
	{
		/// coord en x du centre du pixel [strictement après/avant] le côté gauche/droit
		const real lxs = max((real)round(xs), (real)0.0) + (real) 0.5;
		const integer  lxe = min((positive)round(xe), buffer.width);
		const real lvx = xe - xs;
		const real lvx_inv = 1 / lvx;
		/// var en z par rapport à x
		const real ldz = (ze - zs) * lvx_inv;
		/// var de la couleur par rapport à x
		const real ldr = (re - rs) * lvx_inv;
		const real ldg = (ge - gs) * lvx_inv;
		const real ldb = (be - bs) * lvx_inv;

		/// Différence entre le bord de gauche et le premier pixel de la ligne
		const real vx1 = (lxs-xs);
		/// coord z du point associé au centre du premier pixel de la première  ligne
		real lz = zs + ldz*vx1;
		/// couleur du point associé au centre du premier pixel de la première  ligne
		real lr = rs + ldr*vx1;
        real lg = gs + ldg*vx1;
		real lb = bs + ldb*vx1;

		// Dessine une ligne
		for(integer lx = (integer) lxs; lx < lxe; lx++) {
			real * pts = buffer.getPtr(lx, y);
			// depth test
			if(lz < pts[BUF_Z_OFFSET]) {
				pts[BUF_Z_OFFSET] = lz;
				pts[BUF_R_OFFSET] = lr;
				pts[BUF_G_OFFSET] = lg;
				pts[BUF_B_OFFSET] = lb;
			}

			lz += ldz;
			lr += ldr;
			lg += ldg;
			lb += ldb;
		}

		xs += dxs; xe += dxe;
		zs += dzs; ze += dze;
		rs += drs; re += dre;
		gs += dgs; ge += dge;
		bs += dbs; be += dbe;
	}
}

void triangleSortedPoints(Buffer & buffer, const vec3 A, const vec3 B, const vec3 C, const vec3 colorA, const vec3 colorB, const vec3 colorC) {
	// cas A[1] == B[1] == C[1] => ne dessine rien
	/// côté inférieur horizontal
	if(B[1] == C[1]) {
		if(C[0] < B[0])
            triangleSup(buffer, A, C, B, colorA, colorC, colorB);
        else
            triangleSup(buffer, A, B, C, colorA, colorB, colorC);
	}
	/// côté supérieur horizontal
	else if(A[1] == B[1]) {
		if(A[0] < B[0])
            triangleInf(buffer, C, A, B, colorC, colorA, colorB);
		else
            triangleInf(buffer, C, B, A, colorC, colorB, colorA);
	}
	/// côté supérieur horizontal
	else if(A[1] == C[1]) {
		if(A[0] < C[0])
            triangleInf(buffer, B, A, C, colorB, colorA, colorC);
        else
            triangleInf(buffer, B, C, A, colorB, colorC, colorA);
	}
	/// pas de côté horizontal
	else {
		if(C[1] < B[1]) {
            /// On calcule I, le point de AC de même y que B <=> AI.y = k * AC.y = AB.y
            real k = (C[1]-A[1]) / (B[1]-A[1]); /// rmq : 0 < k < 1
            real I[3] = {(B[0]-A[0]) * k + A[0], C[1], (B[2]-A[2]) * k + A[2]};
            real colorI[VEC3_SCALARS_COUNT] = {
                (colorB[0]-colorA[0]) * k + colorA[0],
                (colorB[1]-colorA[1]) * k + colorA[1],
                (colorB[2]-colorA[2]) * k + colorA[2]
            };

            /// I est à gauche de C
            if(I[0] < C[0]) {
                /// on dessine le triangle supérieur AIC
                triangleSup(buffer, A, I, C, colorA, colorI, colorC);
                /// on dessine le triangle inférieur BCI
                triangleInf(buffer, B, I, C, colorB, colorI, colorC);
            }
            /// C est à gauche de A
            else {
                /// on dessine le triangle supérieur ACI
                triangleSup(buffer, A, C, I, colorA, colorC, colorI);
                /// on dessine le triangle inférieur BCI
                triangleInf(buffer, B, C, I, colorB, colorC, colorI);
            }
		}
		else {
            /// On calcule I, le point de AC de même y que B <=> AI.y = k * AC.y = AB.y
            real k = (B[1]-A[1]) / (C[1]-A[1]); /// rmq : 0 < k < 1
            real I[3] = {(C[0]-A[0]) * k + A[0], B[1], (C[2]-A[2]) * k + A[2]};
            real colorI[VEC3_SCALARS_COUNT] = {
                (colorC[0]-colorA[0]) * k + colorA[0],
                (colorC[1]-colorA[1]) * k + colorA[1],
                (colorC[2]-colorA[2]) * k + colorA[2]
            };

            /// I est à gauche de B
            if(I[0] < B[0]) {
                /// on dessine le triangle supérieur AIB
                triangleSup(buffer, A, I, B, colorA, colorI, colorB);
                /// on dessine le triangle inférieur CBI
                triangleInf(buffer, C, I, B, colorC, colorI, colorB);
            }
            /// B est à gauche de A
            else {
                /// on dessine le triangle supérieur ABI
                triangleSup(buffer, A, B, I, colorA, colorB, colorI);
                /// on dessine le triangle inférieur CBI
                triangleInf(buffer, C, B, I, colorC, colorB, colorI);
            }
		}
	}
}
/**
the coordinates of vertices are (A.x,A.y), (B.x,B.y), (C.x,C.y) we assume that A.y<=B.y<=C.y (you should sort them first)
vertex A has color (A[0],A[1],A[2]), B (B[0],B[1],B[2]), C (C[0],C[1],C[2]), where X[0] is color's red component, X[1] is color's green component and X[2] is color's blue component
dx1,dx2,dx3 are deltas used in interpolation of x-coordinate
dr1,dr2,dr3, dg1,dg2,dg3, db1,db2,db3 are deltas used in interpolation of color's components
putpixel(P) plots a pixel with coordinates (P.x,P.y) and color (P[0],P[1],P[2])
S=A means that S.x=A.x; S.y=A.y; S[0]=A[0]; S[1]=A[1]; S[2]=A[2];*/
void triangle(Buffer & buffer, const vec3 A, const vec3 B, const vec3 C, const vec3 colorA, const vec3 colorB, const vec3 colorC) {

    if(A[1] <= B[1]) {
        if(B[1] <= C[1]) {
            triangleSortedPoints(buffer, A, B, C, colorA, colorB, colorC);
        } else if(C[1] <= A[1]) {
            triangleSortedPoints(buffer, C, A, B, colorC, colorA, colorB);
        } else {
            triangleSortedPoints(buffer, A, C, B, colorA, colorC, colorB);
        }
    } else {
        if(A[1] <= C[1]) {
            triangleSortedPoints(buffer, B, A, C, colorB, colorA, colorC);
        } else if(C[1] <= B[1]) {
            triangleSortedPoints(buffer, C, B, A, colorC, colorB, colorA);
        } else {
            triangleSortedPoints(buffer, B, C, A, colorB, colorC, colorA);
        }
    }
}


