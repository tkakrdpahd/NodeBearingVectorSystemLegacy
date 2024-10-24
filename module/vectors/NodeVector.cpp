/* NodeVector.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 7, 2024
 * 
 * Purpose of Class
 * Declare Node Vector
 */

#include "NodeVector.h"

// 기본 생성자
NodeVector::NodeVector()
    : sphericalNode(), cartesianNode() {}

// Spherical Node Vector를 사용한 생성자
NodeVector::NodeVector(int index, const SphericalNodeVector& snv)
    : sphericalNode(snv) {
    ConvertSphericalToCartesian();  // 생성 시 Cartesian 좌표로 변환
}

// Cartesian Node Vector를 사용한 생성자
NodeVector::NodeVector(int index, const CartesianNodeVector& cnv)
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
    CartesianVector cartVec = CoordinateConverter::sphericalToCartesian(
        SphericalVector(sphericalNode.r_i_n, sphericalNode.theta_i_n, sphericalNode.phi_i_n));
    
    cartesianNode = CartesianNodeVector(sphericalNode.i_n, cartVec.x, cartVec.y, cartVec.z);
}

// Cartesian 좌표를 Spherical 좌표로 변환하는 함수
void NodeVector::ConvertCartesianToSpherical() {
    SphericalVector sphVec = CoordinateConverter::cartesianToSpherical(
        CartesianVector(cartesianNode.x_i_n, cartesianNode.y_i_n, cartesianNode.z_i_n));
    
    sphericalNode = SphericalNodeVector(cartesianNode.i_n, sphVec.r, sphVec.theta, sphVec.phi);
}
