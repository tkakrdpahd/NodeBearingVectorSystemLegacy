/* LinerSegment.cpp
 * Implementation of the LinerSegment class
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 9, 2024
 */

#include "LinerSegment.h"

// Constructor
LinerSegment::LinerSegment(const NodeVectorWithBearing& n1, const NodeVectorWithBearing& n2, float lod, float alphaVal)
    : node_1(n1), node_2(n2), LevelOfDetail(lod), alpha(alphaVal), L_min(0.1f), L_max(10.0f) {
    SamplingBSpline();
}

// Calculate control points based on the equations
void LinerSegment::calculateControlPoints() {
    controlPoints.clear();

    // Equ(9): P0 = N1
    CartesianNodeVector N1 = node_1.node.GetCartesianNodeVector();
    Vector3 P0(N1.x_i_n, N1.y_i_n, N1.z_i_n);
    controlPoints.push_back(P0);

    // Equ(10): Pi = N1 + Ci, 1 ≤ i ≤ D1
    int D1 = node_1.bearings.size();
    for (int i = 0; i < D1; ++i) {
        Vector3 Ci;
        // Calculate Ci using Equ(4) and Equ(5)
        // For simplicity, assuming d_{s,i} = 1
        float bx, by, bz;
        node_1.bearings[i].calculateBearingVector(bx, by, bz);
        BearingVectorForce Fi = node_1.bearings[i].getForce();
        Vector3 Bi(bx, by, bz);
        Vector3 FiVec(Fi.f_x, Fi.f_y, Fi.f_z);
        Ci = Bi.cross(FiVec);

        Vector3 Pi = P0 + Ci;
        controlPoints.push_back(Pi);
    }

    // Equ(11): P_{D1+1} = α(N1 + C_{D1}) + (1 - α)(N2 - C_{D2})
    CartesianNodeVector N2 = node_2.node.GetCartesianNodeVector();
    Vector3 N2Vec(N2.x_i_n, N2.y_i_n, N2.z_i_n);

    // Last C from node 1
    Vector3 C_D1 = controlPoints.back() - P0;

    // Last C from node 2
    Vector3 Pn(N2.x_i_n, N2.y_i_n, N2.z_i_n);
    Vector3 C_D2;
    {
        // Calculate C_D2
        int D2 = node_2.bearings.size();
        float bx, by, bz;
        node_2.bearings[D2 - 1].calculateBearingVector(bx, by, bz);
        BearingVectorForce Fi = node_2.bearings[D2 - 1].getForce();
        Vector3 Bi(bx, by, bz);
        Vector3 FiVec(Fi.f_x, Fi.f_y, Fi.f_z);
        C_D2 = Bi.cross(FiVec);
    }

    Vector3 PD1plus1 = alpha * (P0 + C_D1) + (1 - alpha) * (Pn - C_D2);
    controlPoints.push_back(PD1plus1);

    // Equ(12): P_{D1+1+j} = N2 - C_{D2 - j + 1}, 1 ≤ j ≤ D2
    int D2 = node_2.bearings.size();
    for (int j = 0; j < D2; ++j) {
        Vector3 Ci;
        // Calculate Ci using Equ(4) and Equ(5)
        float bx, by, bz;
        node_2.bearings[D2 - j - 1].calculateBearingVector(bx, by, bz);
        BearingVectorForce Fi = node_2.bearings[D2 - j - 1].getForce();
        Vector3 Bi(bx, by, bz);
        Vector3 FiVec(Fi.f_x, Fi.f_y, Fi.f_z);
        Ci = Bi.cross(FiVec);

        Vector3 Pi = Pn - Ci;
        controlPoints.push_back(Pi);
    }

    // Equ(13): Pn = N2
    controlPoints.push_back(Pn);
}

// Calculate B-Spline curve based on control points
void LinerSegment::calculateBSpline() {
    sampledPoints.clear();
    int n = controlPoints.size() - 1;
    int sampleCount = static_cast<int>(LevelOfDetail);

    for (int i = 0; i <= sampleCount; ++i) {
        float t = static_cast<float>(i) / sampleCount;
        Vector3 Bt(0, 0, 0);
        for (int j = 0; j <= n; ++j) {
            int coeff = binomialCoefficient(n, j);
            float term = coeff * pow(1 - t, n - j) * pow(t, j);
            // Corrected scalar multiplication: term * controlPoints[j]
            Bt = Bt + term * controlPoints[j];
        }
        sampledPoints.push_back(Bt);
    }
}

// Compute binomial coefficient
int LinerSegment::binomialCoefficient(int n, int k) {
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;
    int res = 1;
    for (int i = 1; i <= k; ++i) {
        res *= n - (k - i);
        res /= i;
    }
    return res;
}

// Public function to sample B-Spline
void LinerSegment::SamplingBSpline() {
    calculateControlPoints();
    calculateBSpline();
}

// Placeholder for SamplingVertex (depends on specific implementation)
void LinerSegment::SamplingVertex() {
    // This function would generate polygon vertices based on sampledPoints
    // Implementation depends on how you want to use these vertices
}
