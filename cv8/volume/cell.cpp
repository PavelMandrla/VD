#include "stdafx.h"

#include "cell.h"
#include "volume.h"
#include "utils.h"
#include "iostream"

using namespace std;


Cell::Cell( const DATA_TYPE * rhos, const Vector3 & A, const Vector3 & B, CellIndices indicies, const Volume * volume) {
	memcpy( rhos_, rhos, sizeof( rhos_ ) );

	bounds_ = AABB( A, B );
    this->indicies = indicies;
    this->volume = (Volume*) volume;
}

Cell::~Cell() {}

#pragma region cubic interpolation

float s(int i, int j, int k) {
    return 0;
}

float cubicInterpolate (float p[4], float x) {
    return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
}

float bicubicInterpolate (float p[4][4], float x, float y) {
    float arr[4];
    arr[0] = cubicInterpolate(p[0], y);
    arr[1] = cubicInterpolate(p[1], y);
    arr[2] = cubicInterpolate(p[2], y);
    arr[3] = cubicInterpolate(p[3], y);
    return cubicInterpolate(arr, x);
}

float tricubicInterpolate(float p[4][4][4], float x, float y, float z) {
    float arr[4];
    arr[0] = bicubicInterpolate(p[0], y, z);
    arr[1] = bicubicInterpolate(p[1], y, z);
    arr[2] = bicubicInterpolate(p[2], y, z);
    arr[3] = bicubicInterpolate(p[3], y, z);
    return cubicInterpolate(arr, x);
}

#pragma endregion



float Cell::getPVal(int di, int dj, int dk) const {
    Cell cell = this->volume->cell(this->indicies.i + di, this->indicies.j + dj, this->indicies.k + dk);
    return cell.rho_A();
}

float Cell::Gamma_TriCubic(const Vector3 &uvw) const {
    try {
        float arr[4][4][4];
        for (int x = -1; x <= 2; x++) {
            for (int y = -1; y <= 2; y++) {
                for (int z = -1; z <= 2; z++) {
                    arr[x + 1][y + 1][z + 1] = getPVal(x, y, z);
                }
            }
        }

        return tricubicInterpolate(arr, uvw.x, uvw.y, uvw.z);
    } catch (std::invalid_argument &e) {
        return Gamma(uvw);
    }
}


float Cell::Gamma( const Vector3 & uvw ) const {

	//return ( rho_A() + rho_B() + rho_C() + rho_D() + rho_E() + rho_F() + rho_G() + rho_H() ) / 8.0f;

	const float & u_t = uvw.x;
	const float & v_t = uvw.y;
	const float & w_t = uvw.z;

	// trilinear interpolation
	const COMP_TYPE alpha_AB = rho_A() * ( 1 - u_t ) + rho_B() * u_t;
	const COMP_TYPE alpha_DC = rho_D() * ( 1 - u_t ) + rho_C() * u_t;
	const COMP_TYPE alpha_EF = rho_E() * ( 1 - u_t ) + rho_F() * u_t;
	const COMP_TYPE alpha_HG = rho_H() * ( 1 - u_t ) + rho_G() * u_t;

	const COMP_TYPE beta_0 = alpha_AB * ( 1 - v_t ) + alpha_DC * v_t;
	const COMP_TYPE beta_1 = alpha_EF * ( 1 - v_t ) + alpha_HG * v_t;

	return static_cast< DATA_TYPE >( beta_0 * ( 1 - w_t ) + beta_1 * w_t );

}

int getXYMultiplier(int x, int y, int z) {
    int mask[3][3] = {{1,2,1}, {2,4,2}, {1,2,1}};
    switch (z) {
        case 0:
            return 0;
        case -1:
            return mask[x+1][y+1];
        case 1:
            return -mask[x+1][y+1];
    }
}

float Cell::getDifferenceInDir(const Vector3 & uvw, Vector3 dir, float d) const {
    float sum = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dz = -1; dz <= 1; dz++) {
                Vector3 pos = uvw + Vector3(dx*d, dy*d, dz*d);
                if (dir.x == 1) {
                    sum += this->Gamma(this->u(pos)) * getXYMultiplier(dy, dz, dx);
                } else if (dir.y ==1) {
                    sum += this->Gamma(this->u(pos)) * getXYMultiplier(dx, dz, dy);
                } else {
                    sum += this->Gamma(this->u(pos)) * getXYMultiplier(dx, dy, dz);
                }
            }
        }
    }
    return sum;
}

Vector3 Cell::GradGamma( const Vector3 & uvw ) const {
  // compute the gradient of the scalar field here (use finite central differences or Sobel�Feldman operator)
  //cout << "fuck" <<endl;
	return Vector3(
                this->getDifferenceInDir(uvw, Vector3(1, 0, 0)),
                this->getDifferenceInDir(uvw, Vector3(0, 1, 0)),
                this->getDifferenceInDir(uvw, Vector3(0, 0, 1))
            );
}

#define SAMPLE_COUNT 8

float Cell::Integrate( Ray & ray, const float t0, const float t1 ) const {
    float sum = 0.0f;
    float dif = (t1 - t0) / SAMPLE_COUNT;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        float t = t0 + i * dif;

        float val = this->Gamma(ray.eval(t)) * dif;
        //float val = this->Gamma_sigmoid(this->u(ray.eval(t))) * dif;
        //if (val > 0.03f) {
            sum += val;
        //}
    }
    return sum;
}

float Cell::Integrate(Ray &ray, CellHit & cellHit) const {
    //auto cell = this->volume->cell(cellHit.indices.i, cellHit.indices.j, cellHit.indices.k);
    float sum = 0.0f;
    float dif = (cellHit.t_1 - cellHit.t_0) / SAMPLE_COUNT;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        float t = cellHit.t_0 + i * dif;

        //float val = this->Gamma_sigmoid(this->u(ray.eval(t))) * dif;
        sum += this->Gamma(this->u(ray.eval(t))) * dif;
        /*
        float sample = this->Gamma(this->u(ray.eval(t)));
        float px[4] = {0.1f, 0.25f, 0.55f, 0.7f};
        float py[2] = {0.001f, 0.1f};
        float t_sample = transfer_function(sample, px, py);
        sum += val * dif;
        */

    }
    return sum;
}


float Cell::FindIsoSurface( Ray & ray, const float t0, const float t1, const float iso_value ) const {
	// find the parametric distance of the iso surface of the certain iso value along the given segment of the ray
    float dif = (t1 - t0) / SAMPLE_COUNT;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        float t = t0 + i * dif;
        if (iso_value < this->Gamma(this->u(ray.eval(t)))) {
            return t;
        }
        return -1;
    }

	return -1.0f;
}

#pragma region useless bordel

Vector3 Cell::u( const Vector3 & p ) const
{
	Vector3 uvw = ( p - A() ) / ( G() - A() ); // gives the reference coordinates of the world space point p inside this cell (uvw is in the range <0,1>^3)

	return uvw;
}

Vector3 Cell::A() const
{
	return bounds_.lower_bound();
}

Vector3 Cell::G() const
{
	return bounds_.upper_bound();
}

float Cell::rho_A() const
{
	return rhos_[0];
}

float Cell::rho_B() const
{
	return rhos_[1];
}

float Cell::rho_C() const
{
	return rhos_[2];
}

float Cell::rho_D() const
{
	return rhos_[3];
}

float Cell::rho_E() const
{
	return rhos_[4];
}

float Cell::rho_F() const
{
	return rhos_[5];
}

float Cell::rho_G() const
{
	return rhos_[6];
}

float Cell::rho_H() const
{
	return rhos_[7];
}

#pragma endregion

void Cell::FrontToBack(Ray &ray, CellHit & cellHit, Vector3 &sample_color, float &sample_alpha) const {
    const Vector3 light(300.0f, 800.0f, 800.0f);
    float px_alpha[4] = {0.3f, 0.8f, 0.9f, 1.3f};
    float py_alpha[2] = {0.0f, 0.6f};

    float dif = (cellHit.t_1 - cellHit.t_0) / SAMPLE_COUNT;
    for (int i = 0; i < SAMPLE_COUNT; i++) {

        float t = cellHit.t_0 + float(i) * dif;
        auto p = ray.eval(t);
        auto uvw = this->u(p);
        float sample = this->Gamma_TriCubic(uvw);

        float alpha_in = sample_alpha;
        Vector3 C_in = sample_color;
        float alpha = transfer_function(sample, px_alpha, py_alpha);

        sample_alpha = alpha_in + (1-alpha_in) * alpha;

        auto n = this->GradGamma(p);
        Vector3 L = light - p;
        L.Normalize();
        auto LN = L.DotProduct(n);
        Vector3 C {LN, LN, LN};

        sample_color = C_in + (1 - alpha_in) * alpha * C * transfer_function_color(sample);

    }
}







