/* main.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 13, 2024
 * 
 */
#include <iostream>
#include <cmath>
#include "CoordinateConverter.h"
#include "NodeVector.h"
#include "BearingVector.h"
#include "LinerSegment.h"
#include "SurfaceSegment.h"
#include "AttributesManager.h"
#include "SocketServer.h"
#include "YamlConvertor.h"
#include "Draw.h"

void NodeVectorTest(AttributesManager& _attributesManager) {
    // 여러 개의 SphericalNodeVector 생성 및 NodeVector 초기화 (PI 단위 사용)
    SphericalNodeVector sphericalNode1(1, 10.0f, static_cast<float>(M_PI / 2), static_cast<float>(M_PI / 4));
    SphericalNodeVector sphericalNode2(2, 15.0f, static_cast<float>(M_PI / 4), static_cast<float>(M_PI / 2));
    SphericalNodeVector sphericalNode3(3, 20.0f, static_cast<float>(M_PI / 3), static_cast<float>(M_PI / 3));

    NodeVector node01(1, sphericalNode1);
    NodeVector node02(2, sphericalNode2);
    NodeVector node03(3, sphericalNode3);

    // AttributesManager를 사용하여 NodeVector 생성 및 저장
    _attributesManager.CreateNodeVector(node01);
    _attributesManager.CreateNodeVector(node02);
    _attributesManager.CreateNodeVector(node03);
}

void AttributesManagerTest() {
    AttributesManager _attributesManager;

    // NodeVectorTest 함수를 호출하여 NodeVector 생성 및 저장
    NodeVectorTest(_attributesManager);

    // 저장된 NodeVectors 가져오기
    const std::vector<NodeVector>& storedNodes = _attributesManager.getNodeVectors();
    if (!storedNodes.empty()) {
        for (const auto& node : storedNodes) {
            // 저장된 NodeVector의 Spherical 좌표 출력
            SphericalNodeVector storedSpherical = node.GetSphericalNodeVector();
            std::cout << "Stored Node (Spherical): Index = " << storedSpherical.i_n
                      << ", r = " << storedSpherical.r_i_n
                      << ", theta = " << storedSpherical.theta_i_n
                      << ", phi = " << storedSpherical.phi_i_n << std::endl;

            // Cartesian 좌표로 변환 후 출력
            NodeVector storedNode = node;
            storedNode.ConvertSphericalToCartesian();
            CartesianNodeVector storedCartesian = storedNode.GetCartesianNodeVector();
            std::cout << "Stored Node (Cartesian): Index = " << storedCartesian.i_n
                      << ", x = " << storedCartesian.x_i_n
                      << ", y = " << storedCartesian.y_i_n
                      << ", z = " << storedCartesian.z_i_n << std::endl;
        }
    } else {
        std::cout << "No NodeVectors stored in AttributesManager." << std::endl;
    }
}

int main() {
    AttributesManagerTest();
    std::cout << "Hello World!!!" << std::endl;
    return 0;
}