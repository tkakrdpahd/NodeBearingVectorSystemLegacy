/* Vector3.h
 * Linked file Vector3.cpp
 * Security: Confidential
 * Author: Minseok Doo
 * Date: Oct 24, 2024
 * 
 */

#include <vector>

#ifndef VECTOR3_H
#define VECTOR3_H

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

#endif // VECTOR3_H
