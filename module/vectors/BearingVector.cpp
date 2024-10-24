/* BearingVector.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 7, 2024
 * 
 * Purpose: Define functions for the BearingVector class
 */

#include "BearingVector.h"
#include "CoordinateConverter.h"
#include <cmath>

// Constructor implementation
BearingVector::BearingVector(int index, int depth, const NodeVector& node, float phi_i, float theta_i, float f_x, float f_y, float f_z)
{
    // Initialize spherical bearing vector
    sphericalBearing.i = index;
    sphericalBearing.d = depth;
    sphericalBearing.node = node;
    sphericalBearing.angularAcceleration.phi_i = phi_i;
    sphericalBearing.angularAcceleration.theta_i = theta_i;
    sphericalBearing.force.Force = Vector3(f_x, f_y, f_z);

    // Initialize Cartesian bearing vector by converting spherical to Cartesian
    SphericalVector sv(1.0f, theta_i, phi_i); // Assuming unit vector
    CartesianVector cv = CoordinateConverter::sphericalToCartesian(sv);
    cartesianBearing = CartesianBearingVector(index, depth, cv.x, cv.y, cv.z, f_x, f_y, f_z);
}

// Function to calculate the Cartesian components of the Bearing Vector (unit vector) based on the Node Vector
void BearingVector::calculateBearingVector(float& x, float& y, float& z) const {
    // Assuming bearing vector is a unit vector in the direction specified by spherical angles
    float r = 1.0f; // Unit vector
    float theta = sphericalBearing.angularAcceleration.theta_i;
    float phi = sphericalBearing.angularAcceleration.phi_i;

    x = r * sin(phi) * cos(theta);
    y = r * sin(phi) * sin(theta);
    z = r * cos(phi);
}

// Function to convert the spherical bearing vector to a Cartesian bearing vector
CartesianBearingVector BearingVector::convertToCartesianBearingVector() const {
    // Use CoordinateConverter to convert spherical to Cartesian
    SphericalVector sv(1.0f, sphericalBearing.angularAcceleration.theta_i, sphericalBearing.angularAcceleration.phi_i);
    CartesianVector cv = CoordinateConverter::sphericalToCartesian(sv);

    // Initialize and return CartesianBearingVector
    return CartesianBearingVector(
        sphericalBearing.i, 
        sphericalBearing.d, 
        cv.x, 
        cv.y, 
        cv.z, 
        sphericalBearing.force.Force.x, 
        sphericalBearing.force.Force.y, 
        sphericalBearing.force.Force.z
    );
}

// Function to convert the Cartesian bearing vector back to spherical coordinates (using node vector)
SphericalBearingVectorStruct BearingVector::convertToSphericalBearingVector(const CartesianBearingVector& cartesian, const NodeVector& node) const {
    // Use CoordinateConverter to convert Cartesian to spherical
    SphericalVector sv = CoordinateConverter::cartesianToSpherical(
        CartesianVector(cartesian.cartesianCoords.x, cartesian.cartesianCoords.y, cartesian.cartesianCoords.z)
    );

    // Initialize and return SphericalBearingVectorStruct
    SphericalBearingVectorStruct sbv;
    sbv.i = cartesian.i;
    sbv.d = cartesian.d;
    sbv.node = node;
    sbv.angularAcceleration.phi_i = sv.phi;
    sbv.angularAcceleration.theta_i = sv.theta;
    sbv.force.Force = cartesian.force.Force;

    return sbv;
}

// Function to get the force vector components
BearingVectorForce BearingVector::getForce() const {
    return sphericalBearing.force;
}

// Getters for the spherical angles (φ, θ)
float BearingVector::getPhi() const {
    return sphericalBearing.angularAcceleration.phi_i;
}

float BearingVector::getTheta() const {
    return sphericalBearing.angularAcceleration.theta_i;
}

// Get node index and bearing vector depth
int BearingVector::getNodeIndex() const {
    return sphericalBearing.i;
}

int BearingVector::getDepth() const {
    return sphericalBearing.d;
}
