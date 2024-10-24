/* NodeVector.h
 * Linked file NodeVector.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 7, 2024
 * 
 * Purpose of Class
 * Declare Node Vector
 * 
 * Equations
 * Equ(1): \vec{N_i}=\left(r_i,\theta_i,\phi_i\right)
 */

#ifndef NODEVECTOR_H
#define NODEVECTOR_H

#include "Vector3.h"
#include "CoordinateConverter.h"

// SphericalNodeVector 구조체
struct SphericalNodeVector {
    int i_n; // index of node vector
    Vector3 sphericalCoords; // x: r, y: theta, z: phi

    // 기본 생성자
    SphericalNodeVector(int index = 0, float r = 0.0f, float theta = 0.0f, float phi = 0.0f)
        : i_n(index), sphericalCoords(r, theta, phi) {}

    // 추가 생성자: (int, Vector3)
    SphericalNodeVector(int index, const Vector3& coords)
        : i_n(index), sphericalCoords(coords) {}
};

// Cartesian Node Vector 구조체
struct CartesianNodeVector {
    int i_n; // index of node vector
    Vector3 cartesianCoords; // x: x, y: y, z: z

    // 생성자
    CartesianNodeVector(int index = 0, float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : i_n(index), cartesianCoords(x, y, z) {}
};

// NodeVector 클래스 선언
class NodeVector {
private:
    SphericalNodeVector sphericalNode;   // Spherical 형태의 Node Vector
    CartesianNodeVector cartesianNode;   // Cartesian 형태의 Node Vector

public:
    // 기본 생성자
    NodeVector();

    // Spherical Node Vector를 사용한 생성자
    NodeVector(const SphericalNodeVector& snv);

    // Cartesian Node Vector를 사용한 생성자
    NodeVector(const CartesianNodeVector& cnv);

    // Spherical Node Vector를 반환하는 함수
    SphericalNodeVector GetSphericalNodeVector() const;

    // Cartesian Node Vector를 반환하는 함수
    CartesianNodeVector GetCartesianNodeVector() const;

    // Spherical 좌표를 Cartesian 좌표로 변환하는 함수
    void ConvertSphericalToCartesian();

    // Cartesian 좌표를 Spherical 좌표로 변환하는 함수
    void ConvertCartesianToSpherical();
};

#endif // NODEVECTOR_H
