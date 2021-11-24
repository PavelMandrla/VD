#ifndef VOLUME_H_
#define VOLUME_H_

#include "vector3.h"
#include "matrix4x4.h"
#include "aabb.h"
#include "ray.h"
#include "intersection.h"
#include "cell.h"
#include "camera.h"
#include "CellHit.h"

float Random( const float range_min = 0.0f, const float range_max = 1.0f );

#define CACHE_LINE_SIZE 64

/*! \class Volume
\brief T��da pro p��mou vizualizaci skal�rn�ch objemov�ch dat.

\author Tom� Fabi�n
\version 1.0
\date 2015-2020
*/
class Volume
{
public:
	Volume( const int width, const int height, const int n, const Vector3 & cell_size );
	~Volume();

	void Load( std::string & file_name_mask, const int first_slice_index, const int last_slice_index );
	
	void Generate();		

	Cell cell( const CellIndices & indices ) const;
	Cell cell( const int i, const int j, const int k ) const;

	CellIndices cell_indices( const Vector3 & p ) const; // vr�t� indexy bu�ky, ve kter� le�� bod p
	
	void Traverse( Ray & ray, std::vector<CellHit> & traversed_cells );

	void Raycast( Camera & camera, const int samples = 1 );
	
protected:
	int offset( const int i, const int j, const int k ) const;	

private:
	int width_; // horizont�ln� rozli�en� zdrojov�ho sn�mku [px]; osa y
	int height_; // vertik�ln� rozli�en� zdrojov�ho sn�mku [px]; osa -z
	int n_; // po�et �ez� [-]; osa x

	int width_step_; // po�et re�ln�ch hodnot na jednom ��dku datov� matice [-]
	
	Vector3 cell_size_; // rozm�ry bu�ky [m]
	Vector3 half_volume_size_; // rozm�ry p�lky cel�ho objemu [m]

	DATA_TYPE * data_; // datov� matice s hustotami o rozm�rech (width_step_, height_, n_) a v tomto po�ad� jsou tak� ulo�eny jednotliv� hodnoty

	AABB bounds_;

	static const int kElement_size = sizeof( DATA_TYPE );
};

#endif
