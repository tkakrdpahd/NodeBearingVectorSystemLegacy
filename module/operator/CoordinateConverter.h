/* CoordinateConverter.h
 * Linked file CoordinateConverter.cpp
 * Security: Confidential
 * Author: Minseok Doo
 * Date: Oct 7, 2024
 * 
 * Purpose of Class
 * SphericalVector <-> CartesianVector 변환
 * 
 * Equations
 * Equ(1): x = r * sin(phi) * cos(theta)
 * Equ(2): y = r * sin(phi) * sin(theta)
 * Equ(3): z = r * cos(phi)
 */

#ifndef COORDINATECONVERTER_H
#define COORDINATECONVERTER_H

#include <cmath>

/**
 * @brief Structure representing a Spherical Vector.
 */
struct SphericalVector {
    float r;      ///< Radius
    float theta;  ///< Azimuthal angle in radians
    float phi;    ///< Polar angle in radians

    /**
     * @brief Constructor for SphericalVector.
     * 
     * @param radius Radius value.
     * @param azimuth Azimuthal angle in radians.
     * @param polar Polar angle in radians.
     */
    SphericalVector(float radius = 0.0f, float azimuth = 0.0f, float polar = 0.0f)
        : r(radius), theta(azimuth), phi(polar) {}
};

/**
 * @brief Structure representing a Cartesian Vector.
 */
struct CartesianVector {
    float x;  ///< X-component
    float y;  ///< Y-component
    float z;  ///< Z-component

    /**
     * @brief Constructor for CartesianVector.
     * 
     * @param x_val X-component value.
     * @param y_val Y-component value.
     * @param z_val Z-component value.
     */
    CartesianVector(float x_val = 0.0f, float y_val = 0.0f, float z_val = 0.0f)
        : x(x_val), y(y_val), z(z_val) {}
};

/**
 * @brief CoordinateConverter 클래스.
 *        SphericalVector와 CartesianVector 간의 변환을 제공하는 유틸리티 클래스.
 */
class CoordinateConverter {
public:
    /**
     * @brief SphericalVector를 CartesianVector로 변환합니다.
     * 
     * @param sv 변환할 SphericalVector 객체.
     * @return CartesianVector 변환된 CartesianVector 객체.
     */
    static CartesianVector sphericalToCartesian(const SphericalVector& sv);

    /**
     * @brief CartesianVector를 SphericalVector로 변환합니다.
     * 
     * @param cv 변환할 CartesianVector 객체.
     * @return SphericalVector 변환된 SphericalVector 객체.
     */
    static SphericalVector cartesianToSpherical(const CartesianVector& cv);
};

#endif // COORDINATECONVERTER_H
