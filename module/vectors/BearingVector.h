/* BearingVector.h
 * Linked file BearingVector.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 7, 2024
 * 
 * Purpose: Define functions for the BearingVector class
 * 
 * Equations
 * Equ(1): \vec{B_i}=\left(\begin{matrix}\sin{\phi_i}\cos{\theta_i}\\\sin{\phi_i}\sin{\theta_i}\cos{\phi_i}\\\end{matrix}\right)
 * Equ(2): \vec{B_{i,d}}=\left(B_{i,d,x},B_{i,d,y},B_{i,d,z}\right)
 * Equ(3): \vec{F_i}=F_{x_i}\thinsp\hat{x}+F_{y_i}\thinsp\hat{y}+F_{z_i}\thinsp\hat{z}
 */

#ifndef BEARINGVECTOR_H
#define BEARINGVECTOR_H

#include "CoordinateConverter.h"
#include "NodeVector.h"
#include <cmath>

struct BearingVectorForce { 
    float f_x, f_y, f_z; 
};

struct BearingVectorAngularAcceleration {
    float phi_i, theta_i; // phi_i: Polar angle (φ), theta_i: Azimuthal angle (θ);
};

// Struct to hold full spherical bearing vector data
struct SphericalBearingVectorStruct {
    int i; // Node vector index
    int d; // Bearing vector depth (vector depth)
    NodeVector node;  // Node vector
    BearingVectorAngularAcceleration angularAcceleration;  // Angular acceleration (φ, θ)
    BearingVectorForce force;  // Force vector (Fx, Fy, Fz)
};

// Struct to hold Cartesian bearing vector data
struct CartesianBearingVector {
    int i; // Node vector index
    int d; // Bearing vector depth (vector depth)
    float x, y, z; // Cartesian
    BearingVectorForce force; // Force vector in Cartesian coordinates

    // Constructor to initialize the Cartesian Bearing Vector
    CartesianBearingVector(int index = 0, int depth = 0, float x_val = 0.0f, float y_val = 0.0f, float z_val = 0.0f, float f_x = 0.0f, float f_y = 0.0f, float f_z = 0.0f)
        : i(index), d(depth), x(x_val), y(y_val), z(z_val) {
        force.f_x = f_x;
        force.f_y = f_y;
        force.f_z = f_z;
    }
};

class BearingVector {
private:
    SphericalBearingVectorStruct sphericalBearing;
    CartesianBearingVector cartesianBearing;
public:
    // Constructor
    BearingVector(int index, int depth, const NodeVector& node, float phi_i, float theta_i, float f_x, float f_y, float f_z);

    // Function to calculate the Cartesian components of the Bearing Vector (unit vector) based on the Node Vector
    void calculateBearingVector(float& x, float& y, float& z) const;

    // Function to convert the spherical bearing vector to a Cartesian bearing vector
    CartesianBearingVector convertToCartesianBearingVector() const;

    // Function to convert the Cartesian bearing vector back to spherical coordinates (using node vector)
    SphericalBearingVectorStruct convertToSphericalBearingVector(const CartesianBearingVector& cartesian, const NodeVector& node) const;

    // Function to get the force vector components
    BearingVectorForce getForce() const;

    // Getters for the spherical angles (φ, θ)
    float getPhi() const;
    float getTheta() const;

    // Get node index and bearing vector depth
    int getNodeIndex() const;
    int getDepth() const;
};

#endif // BEARINGVECTOR_H
