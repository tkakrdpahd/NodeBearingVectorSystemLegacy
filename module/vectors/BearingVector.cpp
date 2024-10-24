/* BearingVector.cpp
 * Implementation of BearingVector class
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 7, 2024
 * 
 * Purpose: Define functions for the BearingVector class
 */

#include "BearingVector.h"
#include <cmath>

// Constructor
BearingVector::BearingVector(int index, int depth, const NodeVector& node, float phi_i, float theta_i, float f_x, float f_y, float f_z) {
    sphericalBearing.i = index;
    sphericalBearing.d = depth;
    sphericalBearing.node = node;
    sphericalBearing.angularAcceleration.phi_i = phi_i;
    sphericalBearing.angularAcceleration.theta_i = theta_i;
    sphericalBearing.force.f_x = f_x;
    sphericalBearing.force.f_y = f_y;
    sphericalBearing.force.f_z = f_z;
}

// Function to calculate the Cartesian components of the Bearing Vector (unit vector) based on the Node Vector
void BearingVector::calculateBearingVector(float& x, float& y, float& z) const {
    float phi = sphericalBearing.angularAcceleration.phi_i;
    float theta = sphericalBearing.angularAcceleration.theta_i;

    // Calculate unit vector direction (magnitude = 1) in Cartesian coordinates
    float unit_x = sin(phi) * cos(theta);
    float unit_y = sin(phi) * sin(theta);
    float unit_z = cos(phi);

    // Get the Cartesian coordinates of the node (Node is required here)
    CartesianNodeVector nodeCartesian = sphericalBearing.node.GetCartesianNodeVector();

    // Translate to the node's Cartesian position by adding the unit vector to the node's coordinates
    x = nodeCartesian.x_i_n + unit_x;
    y = nodeCartesian.y_i_n + unit_y;
    z = nodeCartesian.z_i_n + unit_z;
}

// Function to convert spherical bearing vector to Cartesian bearing vector
CartesianBearingVector BearingVector::convertToCartesianBearingVector() const {
    float x, y, z;
    calculateBearingVector(x, y, z);

    // Return the CartesianBearingVector with the force and converted position
    return CartesianBearingVector(sphericalBearing.i, sphericalBearing.d, x, y, z, sphericalBearing.force.f_x, sphericalBearing.force.f_y, sphericalBearing.force.f_z);
}

// Function to convert Cartesian bearing vector back to spherical coordinates (requires node position)
SphericalBearingVectorStruct BearingVector::convertToSphericalBearingVector(const CartesianBearingVector& cartesian, const NodeVector& node) const {
    SphericalBearingVectorStruct spherical;

    // Get the Cartesian coordinates of the node
    CartesianNodeVector nodeCartesian = node.GetCartesianNodeVector();

    // Calculate relative position of the bearing vector (subtract node position)
    float rel_x = cartesian.x - nodeCartesian.x_i_n;
    float rel_y = cartesian.y - nodeCartesian.y_i_n;
    float rel_z = cartesian.z - nodeCartesian.z_i_n;

    // Calculate spherical angles from relative Cartesian coordinates
    float r = sqrt(rel_x * rel_x + rel_y * rel_y + rel_z * rel_z);
    spherical.angularAcceleration.phi_i = acos(rel_z / r);  // phi
    spherical.angularAcceleration.theta_i = atan2(rel_y, rel_x);  // theta

    spherical.i = cartesian.i;
    spherical.d = cartesian.d;
    spherical.force = cartesian.force;  // Force remains the same in both systems

    return spherical;
}

// Function to get the force vector components
BearingVectorForce BearingVector::getForce() const {
    return sphericalBearing.force;
}

// Getters for the spherical angles
float BearingVector::getPhi() const {
    return sphericalBearing.angularAcceleration.phi_i;
}

float BearingVector::getTheta() const {
    return sphericalBearing.angularAcceleration.theta_i;
}

// Get node index
int BearingVector::getNodeIndex() const {
    return sphericalBearing.i;
}

// Get bearing vector depth
int BearingVector::getDepth() const {
    return sphericalBearing.d;
}
