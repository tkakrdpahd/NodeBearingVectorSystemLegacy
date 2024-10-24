#ifndef BEARINGVECTOR_H
#define BEARINGVECTOR_H

#include "Vector3.h"
#include "CoordinateConverter.h"
#include "NodeVector.h"
#include <cmath>

/**
 * @brief Structure to represent the force vector in BearingVector.
 */
struct BearingVectorForce { 
    Vector3 Force;
    // float f_x, f_y, f_z; // Removed as we are using Vector3
};

/**
 * @brief Structure to represent angular acceleration in BearingVector.
 */
struct BearingVectorAngularAcceleration {
    float phi_i;    ///< Polar angle (φ)
    float theta_i;  ///< Azimuthal angle (θ)
};

/**
 * @brief Structure to hold full spherical bearing vector data.
 */
struct SphericalBearingVectorStruct {
    int i; // Node vector index
    int d; // Bearing vector depth (vector depth)
    NodeVector node;  // Node vector
    BearingVectorAngularAcceleration angularAcceleration;  // Angular acceleration (φ, θ)
    BearingVectorForce force;  // Force vector (Fx, Fy, Fz)
};

/**
 * @brief Structure to hold Cartesian bearing vector data.
 */
struct CartesianBearingVector {
    int i; // Node vector index
    int d; // Bearing vector depth (vector depth)
    Vector3 cartesianCoords; // Cartesian coordinates (x, y, z)
    BearingVectorForce force; // Force vector in Cartesian coordinates

    /**
     * @brief Constructor to initialize the Cartesian Bearing Vector.
     * 
     * @param index Node vector index.
     * @param depth Bearing vector depth.
     * @param x_val X-component of the Cartesian vector.
     * @param y_val Y-component of the Cartesian vector.
     * @param z_val Z-component of the Cartesian vector.
     * @param f_x X-component of the force vector.
     * @param f_y Y-component of the force vector.
     * @param f_z Z-component of the force vector.
     */
    CartesianBearingVector(int index = 0, int depth = 0, float x_val = 0.0f, float y_val = 0.0f, float z_val = 0.0f, float f_x = 0.0f, float f_y = 0.0f, float f_z = 0.0f)
        : i(index), d(depth), cartesianCoords(x_val, y_val, z_val) {
        force.Force = Vector3(f_x, f_y, f_z);
    }
};

/**
 * @brief BearingVector 클래스.
 *        Spherical 및 Cartesian 형태의 Bearing Vector를 관리.
 */
class BearingVector {
private:
    SphericalBearingVectorStruct sphericalBearing;
    CartesianBearingVector cartesianBearing;

public:
    /**
     * @brief Constructor for BearingVector.
     * 
     * @param index Node vector index.
     * @param depth Bearing vector depth.
     * @param node NodeVector instance.
     * @param phi_i Polar angle (φ).
     * @param theta_i Azimuthal angle (θ).
     * @param f_x X-component of the force vector.
     * @param f_y Y-component of the force vector.
     * @param f_z Z-component of the force vector.
     */
    BearingVector(int index, int depth, const NodeVector& node, float phi_i, float theta_i, float f_x, float f_y, float f_z);

    /**
     * @brief Function to calculate the Cartesian components of the Bearing Vector (unit vector) based on the Node Vector.
     * 
     * @param x Reference to store the x-component.
     * @param y Reference to store the y-component.
     * @param z Reference to store the z-component.
     */
    void calculateBearingVector(float& x, float& y, float& z) const;

    /**
     * @brief Function to convert the spherical bearing vector to a Cartesian bearing vector.
     * 
     * @return CartesianBearingVector Converted Cartesian bearing vector.
     */
    CartesianBearingVector convertToCartesianBearingVector() const;

    /**
     * @brief Function to convert the Cartesian bearing vector back to spherical coordinates (using node vector).
     * 
     * @param cartesian Cartesian bearing vector to convert.
     * @param node NodeVector instance for reference.
     * @return SphericalBearingVectorStruct Converted spherical bearing vector.
     */
    SphericalBearingVectorStruct convertToSphericalBearingVector(const CartesianBearingVector& cartesian, const NodeVector& node) const;

    /**
     * @brief Function to get the force vector components.
     * 
     * @return BearingVectorForce Force vector.
     */
    BearingVectorForce getForce() const;

    /**
     * @brief Getter for the polar angle (φ).
     * 
     * @return float Polar angle.
     */
    float getPhi() const;

    /**
     * @brief Getter for the azimuthal angle (θ).
     * 
     * @return float Azimuthal angle.
     */
    float getTheta() const;

    /**
     * @brief Getter for the node index.
     * 
     * @return int Node index.
     */
    int getNodeIndex() const;

    /**
     * @brief Getter for the bearing vector depth.
     * 
     * @return int Bearing vector depth.
     */
    int getDepth() const;
};

#endif // BEARINGVECTOR_H
