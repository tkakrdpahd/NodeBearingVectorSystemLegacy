/* NodeVector.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 7, 2024
 * 
 * Purpose of Class
 * Declare Node Vector
 */

#include "NodeVector.h"
#include "CoordinateConverter.h" // CoordinateConverter 포함
#include <cmath> // 수학 함수 사용을 위해 추가

// 기본 생성자
NodeVector::NodeVector()
    : sphericalNode(), cartesianNode() {}

// Spherical Node Vector를 사용한 생성자
NodeVector::NodeVector(const SphericalNodeVector& snv)
    : sphericalNode(snv) {
    ConvertSphericalToCartesian();  // 생성 시 Cartesian 좌표로 변환
}

// Cartesian Node Vector를 사용한 생성자
NodeVector::NodeVector(const CartesianNodeVector& cnv)
    : cartesianNode(cnv) {
    ConvertCartesianToSpherical();  // 생성 시 Spherical 좌표로 변환
}

// Spherical Node Vector를 반환하는 함수
SphericalNodeVector NodeVector::GetSphericalNodeVector() const {
    return sphericalNode;
}

// Cartesian Node Vector를 반환하는 함수
CartesianNodeVector NodeVector::GetCartesianNodeVector() const {
    return cartesianNode;
}

// Spherical 좌표를 Cartesian 좌표로 변환하는 함수
void NodeVector::ConvertSphericalToCartesian() {
    // Vector3에서 r, theta, phi 추출
    float r = sphericalNode.sphericalCoords.x;
    float theta = sphericalNode.sphericalCoords.y; // 경도 (radians)
    float phi = sphericalNode.sphericalCoords.z;   // 위도 (radians)

    // SphericalVector 생성
    SphericalVector sv(r, theta, phi);

    // CoordinateConverter를 사용하여 변환
    CartesianVector cv = CoordinateConverter::sphericalToCartesian(sv);

    // 변환된 CartesianVector를 Vector3로 매핑
    cartesianNode.cartesianCoords = Vector3(cv.x, cv.y, cv.z);

    // 인덱스 동기화
    cartesianNode.i_n = sphericalNode.i_n;
}

// Cartesian 좌표를 Spherical 좌표로 변환하는 함수
void NodeVector::ConvertCartesianToSpherical() {
    // Vector3에서 x, y, z 추출
    float x = cartesianNode.cartesianCoords.x;
    float y = cartesianNode.cartesianCoords.y;
    float z = cartesianNode.cartesianCoords.z;

    // CartesianVector 생성
    CartesianVector cv(x, y, z);

    // CoordinateConverter를 사용하여 변환
    SphericalVector sv = CoordinateConverter::cartesianToSpherical(cv);

    // 변환된 SphericalVector를 Vector3로 매핑
    sphericalNode.sphericalCoords = Vector3(sv.r, sv.theta, sv.phi);

    // 인덱스 동기화
    sphericalNode.i_n = cartesianNode.i_n;
}
