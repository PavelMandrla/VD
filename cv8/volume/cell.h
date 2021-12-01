#ifndef CELL_H_
#define CELL_H_

#include <memory>
#include "vector3.h"
#include "ray.h"
#include "aabb.h"
#include "CellHit.h"
//#include "volume.h"

class Volume;

#define DATA_TYPE float
#define COMP_TYPE double

/*! \class Cell
\brief Implementation of a single hexahedral cell.

\author Tom� Fabi�n
\version 1.0
\date 2011-2020
*/
class Cell
{
public:
	Cell(const DATA_TYPE * rhos, const Vector3 & A, const Vector3 & B, CellIndices indicies, const Volume * volume);
	~Cell();
		
	float Gamma( const Vector3 & uvw ) const;
    //float Gamma_Cubic(const Vector3 & uvw) const;
    //float Gamma_BiCubic(const Vector3 & uvw) const;
    float Gamma_TriCubic(const Vector3 & uvw) const;
	Vector3 GradGamma( const Vector3 & uvw ) const;
	
	float Integrate( Ray & ray, const float t0, const float t1 ) const;		
	float Integrate( Ray & ray, CellHit & cellHit) const;
	float FindIsoSurface( Ray & ray, const float t0, const float t1, const float iso_value ) const;

	Vector3 u( const Vector3 & p ) const;

	DATA_TYPE rho_A() const;
	DATA_TYPE rho_B() const;
	DATA_TYPE rho_C() const;
	DATA_TYPE rho_D() const;

	DATA_TYPE rho_E() const;
	DATA_TYPE rho_F() const;
	DATA_TYPE rho_G() const;
	DATA_TYPE rho_H() const;

	Vector3 A() const;
	Vector3 G() const;

    void FrontToBack(Ray & ray, CellHit & cellHit, Vector3 &sample_color, float &sample_alpha) const ;
private:
	DATA_TYPE rhos_[8];	
	AABB bounds_;
	float getDifferenceInDir(const Vector3 & uvw, Vector3 dir, float d = 0.01f) const;
    Volume* volume;
    CellIndices indicies;

    float getPVal(int di, int dj, int dk) const;

};

#endif
