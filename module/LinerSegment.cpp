/* LinerSegment.cpp
 * Implementation of the LinerSegment class
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 9, 2024
 * 
 * Equations
 * Equ(1): R_i=R_z\left(\theta_i\right)R_y\left(\phi_i\right)
 * Equ(2): R_z\left(\theta_i\right)=\left(\begin{matrix}\cos{\theta_i}&\sin{\theta_i}&0\\\sin{\theta_i}&\cos{\theta_i}&0\\0&0&1\\\end{matrix}\right)
 * Equ(3): R_y\left(\phi_i\right)=\left(\begin{matrix}\cos{\phi_i}&0&\sin{\phi_i}\\0&1&0\\-\sin{\phi_i}&0&\cos{\phi_i}\\\end{matrix}\right)
 * Equ(4): \vec{V_{\mathrm{total},s}}=\sum_{i=1}^{D_s}d_{s,i}\cdot\left(\vec{B_i}\otimes\vec{F_i}\right)
 * Equ(5): \vec{P_{\mathrm{final},s}}=\vec{N_{\mathrm{Cartesian}}}+\vec{V_{\mathrm{total},s}}
 * Equ(6): L_{d,\mathrm{in}}=L_{\mathrm{min}}+\left(L_{\mathrm{max}}-L_{\mathrm{min}}\right)\cdot|\vec{B_{d,\mathrm{in}}}|
 * Equ(7): L_{d,\mathrm{out}}=L_{\mathrm{min}}+\left(L_{\mathrm{max}}-L_{\mathrm{min}}\right)\cdot|\vec{B_{d,\mathrm{out}}}|
 * Equ(8): \vec{B}\left(t\right)=\sum_{i=0}^{n}\binom{n}{i}\left(1-t\right)^{n-i}t^i\vec{P_i},\emsp0\le t\le1
 * Equ(9): \vec{P_0}=\vec{N_1}
 * Equ(10): \vec{P_i}=\vec{N_1}+\vec{C_{1,i}},\emsp1\le i\le D_1
 * Equ(11): \vec{P_{D_1+1}}=\alpha\left(\vec{N_1}+\vec{C_{1,D_1}}\right)+\left(1-\alpha\right)\left(\vec{N_2}-\vec{C_{2,D_2}}\right)
 * Equ(12): \vec{P_{D_1+1+j}}=\vec{N_2}-\vec{C_{2,D_2-j+1}},\emsp1\le j\le D_2
 * Equ(13): \vec{P_n}=\vec{N_2}
 * Equ(14): \kappa\left(t\right)=\frac{|\vec{B^{\prime\prime}}\left(t\right)\times\vec{B^\prime}\left(t\right)|}{|\vec{B^\prime}\left(t\right)|^3}
 */

#include "LinerSegment.h"

// 성분별 곱셈 함수 추가
Vector3 hadamardProduct(const Vector3& a, const Vector3& b) {
    return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

// Constructor
LinerSegment::LinerSegment(const NodeVectorWithBearing& n1, const NodeVectorWithBearing& n2, float lod, float alphaVal)
    : node_1(n1), node_2(n2), LevelOfDetail(lod), alpha(alphaVal), L_min(0.1f), L_max(10.0f) {
    SamplingBezierCurve();
}

// Calculate control points based on the equations
void LinerSegment::calculateControlPoints() {
    controlPoints.clear();

    // Equ(9): P0 = N1
    CartesianNodeVector N1 = node_1.node.GetCartesianNodeVector();
    Vector3 P0(N1.cartesianCoords.x, N1.cartesianCoords.y, N1.cartesianCoords.z);
    controlPoints.push_back(P0);

    // Equ(10): Pi = N1 + Ci, 1 ≤ i ≤ D1
    int D1 = node_1.bearings.size();
    std::vector<Vector3> C_list_1; // 노드 1의 Ci 리스트

    for (int i = 0; i < D1; ++i) {
        Vector3 Ci;
        // Calculate Ci using Equ(4) and Equ(5)
        float bx, by, bz;
        node_1.bearings[i].calculateBearingVector(bx, by, bz);
        BearingVectorForce Fi = node_1.bearings[i].getForce();
        Vector3 Bi(bx, by, bz);
        Vector3 FiVec(Fi.Force.x, Fi.Force.y, Fi.Force.z); // 수정됨

        // Equ(4): Vi = Bi ⊗ Fi (성분별 곱셈)
        Vector3 Vi = hadamardProduct(Bi, FiVec); // Vi = Bi ⊗ Fi

        // Equ(5): Ci = Vi (d_{s,i}는 1로 가정)
        Ci = Vi;
        C_list_1.push_back(Ci);

        Vector3 Pi = P0 + Ci;
        controlPoints.push_back(Pi);
    }

    // Equ(13): Pn = N2
    CartesianNodeVector N2 = node_2.node.GetCartesianNodeVector();
    Vector3 Pn(N2.cartesianCoords.x, N2.cartesianCoords.y, N2.cartesianCoords.z); // 수정됨

    // Equ(11): P_{D1+1} = α(N1 + C_{D1}) + (1 - α)(N2 - C_{1})
    Vector3 C_D1 = C_list_1.back(); // 노드 1의 마지막 Ci
    Vector3 C_1_D2; // 노드 2의 첫 번째 Ci

    // 노드 2의 첫 번째 Ci 계산
    if (node_2.bearings.size() > 0) { // 안전성 추가
        // Calculate Ci using Equ(4) and Equ(5)
        float bx, by, bz;
        node_2.bearings[0].calculateBearingVector(bx, by, bz);
        BearingVectorForce Fi = node_2.bearings[0].getForce();
        Vector3 Bi(bx, by, bz);
        Vector3 FiVec(Fi.Force.x, Fi.Force.y, Fi.Force.z); // 수정됨
        C_1_D2 = hadamardProduct(Bi, FiVec); // Vi = Bi ⊗ Fi
    } else {
        C_1_D2 = Vector3(0.0f, 0.0f, 0.0f); // 기본값 설정
    }

    Vector3 PD1plus1 = alpha * (P0 + C_D1) + (1.0f - alpha) * (Pn - C_1_D2);
    controlPoints.push_back(PD1plus1);

    // Equ(12): P_{D1+1+j} = N2 - C_{j}, 1 ≤ j ≤ D2
    int D2 = node_2.bearings.size();
    for (int j = 0; j < D2; ++j) {
        Vector3 Ci;
        // Calculate Ci using Equ(4) and Equ(5)
        float bx, by, bz;
        node_2.bearings[j].calculateBearingVector(bx, by, bz);
        BearingVectorForce Fi = node_2.bearings[j].getForce();
        Vector3 Bi(bx, by, bz);
        Vector3 FiVec(Fi.Force.x, Fi.Force.y, Fi.Force.z); // 수정됨
        Vector3 Vi = hadamardProduct(Bi, FiVec); // Vi = Bi ⊗ Fi
        Ci = Vi; // d_{s,i}는 1로 가정

        Vector3 Pi = Pn - Ci;
        controlPoints.push_back(Pi);
    }

    // Equ(13): Pn = N2
    controlPoints.push_back(Pn);

    // 컨트롤 포인트 출력 (디버깅 목적)
    for (size_t i = 0; i < controlPoints.size(); ++i) {
        Vector3 cp = controlPoints[i];
        std::cout << "Control Point " << i << ": (" << cp.x << ", " << cp.y << ", " << cp.z << ")" << std::endl;
    }
}

// Calculate Bezier curve based on control points
void LinerSegment::calculateBezierCurve() {
    sampledPoints.clear();
    int n = controlPoints.size() - 1;
    int sampleCount = static_cast<int>(LevelOfDetail);

    for (int i = 0; i <= sampleCount; ++i) {
        float t = static_cast<float>(i) / sampleCount;
        Vector3 Bt(0, 0, 0);
        for (int j = 0; j <= n; ++j) {
            int coeff = binomialCoefficient(n, j);
            float term = coeff * pow(1 - t, n - j) * pow(t, j);
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

// Public function to sample Bezier curve
void LinerSegment::SamplingBezierCurve() {
    calculateControlPoints();
    calculateBezierCurve();
}

// Placeholder for SamplingVertex (depends on specific implementation)
void LinerSegment::SamplingVertex() {
    // This function would generate polygon vertices based on sampledPoints
    // Implementation depends on how you want to use these vertices
}

// Return LinerSegmentData (Implementation)
LinerSegmentData LinerSegment::ReturnLinerSegmentData() const {
    LinerSegmentData data;
    data.LinerBufferIndex = 0; // 필요한 경우 적절한 값으로 설정
    data.NodeStart = node_1.node;
    data.NodeEnd = node_2.node;
    data.LevelOfDetail = LevelOfDetail;
    data.alpha = alpha;
    return data;
}
