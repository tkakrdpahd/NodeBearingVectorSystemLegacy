/* CoordinateConverter.cpp
 * Linked file CoordinateConverter.h
 * Security: Confidential
 * Author: Minseok Doo
 * Date: Oct 7, 2024
 */

#include "CoordinateConverter.h"

/**
 * @brief SphericalVector를 CartesianVector로 변환합니다.
 * 
 * Equations:
 * Equ(1): x = r * sin(phi) * cos(theta)
 * Equ(2): y = r * sin(phi) * sin(theta)
 * Equ(3): z = r * cos(phi)
 * 
 * @param sv 변환할 SphericalVector 객체.
 * @return CartesianVector 변환된 CartesianVector 객체.
 */
CartesianVector CoordinateConverter::sphericalToCartesian(const SphericalVector& sv) {
    float x = sv.r * std::sin(sv.phi) * std::cos(sv.theta);
    float y = sv.r * std::sin(sv.phi) * std::sin(sv.theta);
    float z = sv.r * std::cos(sv.phi);
    return CartesianVector(x, y, z);
}

/**
 * @brief CartesianVector를 SphericalVector로 변환합니다.
 * 
 * Equations:
 * Equ(1): r = sqrt(x^2 + y^2 + z^2)
 * Equ(2): theta = atan2(y, x)
 * Equ(3): phi = acos(z / r)
 * 
 * @param cv 변환할 CartesianVector 객체.
 * @return SphericalVector 변환된 SphericalVector 객체.
 */
SphericalVector CoordinateConverter::cartesianToSpherical(const CartesianVector& cv) {
    float r = std::sqrt(cv.x * cv.x + cv.y * cv.y + cv.z * cv.z);
    float theta = (cv.x == 0.0f && cv.y == 0.0f) ? 0.0f : std::atan2(cv.y, cv.x);
    float phi = (r == 0.0f) ? 0.0f : std::acos(cv.z / r);
    return SphericalVector(r, theta, phi);
}
