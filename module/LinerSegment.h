/* LinerSegment.h
 * Linked file LinerSegment.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 9, 2024
 * 
 * Purpose: 
 * 1. Create B-Spline line
 * 2. Create Polygon Vertex based on LOD
 * 
 * Equations
 * Equ(1): R_i=R_z\left(\theta_i\right)R_y\left(\phi_i\right)
 * Equ(2): R_z\left(\theta_i\right)=\left(\begin{matrix}\cos{\theta_i}&\sin{\theta_i}&0\\\sin{\theta_i}&\cos{\theta_i}&0\\0&0&1\\\end{matrix}\right)
 * Equ(3): R_y\left(\phi_i\right)=\left(\begin{matrix}\cos{\phi_i}&0&\sin{\phi_i}\\0&1&0\\-\sin{\phi_i}&0&\cos{\phi_i}\\\end{matrix}\right)
 * Equ(4): \vec{V_{\mathrm{total},s}}=\sum_{i=1}^{D_s}d_{s,i}\cdot\left(\vec{B_i}\otimes\vec{F_i}\right)
 * Equ(5): \vec{P_{\mathrm{final},s}}=\vec{N_{\mathrm{Cartesian}}}+\vec{V_{\mathrm{total},s}}
 * Equ(6): L_{d,\mathrm{in}}=L_{\mathrm{min}}+\left(L_{\mathrm{max}}-L_{\mathrm{min}}\right)\cdot|\vec{B_{d,\mathrm{in}}}|
 * Equ(7): L_{d,\mathrm{out}}=L_{\mathrm{min}}+\left(L_{\mathrm{max}}-L_{\mathrm{min}}\right)\cdot|\vec{B_{d,\mathrm{out}}}|
 * Equ(8): \vec{B}\left(t\right)=\sum_{i=0}^{n}\binom{n}{i}\left(1-t\right)^{n-i}t^i\vec{P_i},\emsp0\le t\le1
 * Equ(9): \vec{P_0}=\vec{N_1}
 * Equ(10): \vec{P_i}=\vec{N_1}+\vec{C_{1,i}},\emsp1\le i\le D_1
 * Equ(11): \vec{P_{D_1+1}}=\alpha\left(\vec{N_1}+\vec{C_{1,D_1}}\right)+\left(1-\alpha\right)\left(\vec{N_2}-\vec{C_{2,D_2}}\right)
 * Equ(12): \vec{P_{D_1+1+j}}=\vec{N_2}-\vec{C_{2,D_2-j+1}},\emsp1\le j\le D_2
 * Equ(13): \vec{P_n}=\vec{N_2}
 * Equ(14): \kappa\left(t\right)=\frac{|\vec{B^{\prime\prime}}\left(t\right)\times\vec{B^\prime}\left(t\right)|}{|\vec{B^\prime}\left(t\right)|^3}
 */
#ifndef LINERSEGMENT_H
#define LINERSEGMENT_H

#include "NodeVector.h"
#include "BearingVector.h"
#include <vector>
#include <cmath>
#include <iostream> // 디버깅을 위한 헤더 추가

// Simple Vector3 struct for vector operations
struct Vector3 {
    float x, y, z;
    Vector3(float xVal = 0, float yVal = 0, float zVal = 0) : x(xVal), y(yVal), z(zVal) {}

    // Vector addition
    Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
    // Vector subtraction
    Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    // Scalar multiplication (Vector3 * float)
    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
    // Scalar division
    Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }
    // Dot product
    float dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }
    // Cross product
    Vector3 cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
    // Vector magnitude
    float magnitude() const { return sqrt(x * x + y * y + z * z); }

    // Friend function for scalar multiplication (float * Vector3)
    friend Vector3 operator*(float scalar, const Vector3& v) {
        return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
    }
};

struct NodeVectorWithBearing {
    NodeVector node;
    std::vector<BearingVector> bearings;
};

struct LinerSegmentData {
    int LinerBufferIndex;
    NodeVector NodeStart;
    NodeVector NodeEnd;
};

class LinerSegment {
private:
    float LevelOfDetail;
    NodeVectorWithBearing node_1;
    NodeVectorWithBearing node_2;
    std::vector<Vector3> controlPoints;
    std::vector<Vector3> sampledPoints;
    float alpha; // Blending factor for control points
    float L_min, L_max; // Min and Max lengths for Equ(6) and Equ(7)

    // Helper functions
    void calculateControlPoints();
    void calculateBezierCurve();
    int binomialCoefficient(int n, int k);

public:
    // Constructor
    LinerSegment(const NodeVectorWithBearing& n1, const NodeVectorWithBearing& n2, float lod, float alphaVal = 0.5f);

    LinerSegmentData LinerSegmentData;
    // Functions to generate the Bezier curve and sample vertices
    void SamplingBezierCurve();
    void SamplingVertex();
    void ReturnLinerSegmentData();

    // Getters
    const std::vector<Vector3>& getSampledPoints() const { return sampledPoints; }
    const std::vector<Vector3>& getControlPoints() const { return controlPoints; }

    // Setters
    void setLevelOfDetail(float lod) { LevelOfDetail = lod; }
};

#endif // LINERSEGMENT_H
