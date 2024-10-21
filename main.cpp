/* main.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 13, 2024
 * 
 */
#include <iostream>
#include <cmath>
#include <fstream>
// Header File
#include "CoordinateConverter.h"
#include "NodeVector.h"
#include "BearingVector.h"
#include "LinerSegment.h"
#include "SurfaceSegment.h"
#include "AttributesManager.h"
#include "SocketServer.h"
#include "YamlConverter.h"
#include "Draw.h"

// Test Functions

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

void BearingVectorTest(AttributesManager& _attributesManager) {
    // Bearing Vector 생성 및 초기화
    SphericalNodeVector sphericalNode(1, 10.0f, static_cast<float>(M_PI / 2), static_cast<float>(M_PI / 4));
    NodeVector node(1, sphericalNode);

    BearingVector bearingVector1(1, 1, node, static_cast<float>(M_PI / 4), static_cast<float>(M_PI / 6), 5.0f, 3.0f, 2.0f);
    BearingVector bearingVector2(2, 1, node, static_cast<float>(M_PI / 3), static_cast<float>(M_PI / 8), 2.0f, 4.0f, 3.0f);
    BearingVector bearingVector3(3, 1, node, static_cast<float>(M_PI / 6), static_cast<float>(M_PI / 5), 1.0f, 1.5f, 2.5f);

    // AttributesManager를 사용하여 BearingVector 생성 및 저장
    _attributesManager.CreateBearingVector(bearingVector1);
    _attributesManager.CreateBearingVector(bearingVector2);
    _attributesManager.CreateBearingVector(bearingVector3);
}

void LinerSegmentTest(AttributesManager& _attributesManager) {
    // NodeVectorWithBearing 초기화
    NodeVectorWithBearing node1;
    NodeVectorWithBearing node2;

    // AttributesManager에서 NodeVector와 해당 인덱스에 맞는 BearingVector 가져오기
    const auto& storedNodes = _attributesManager.getNodeVectors();
    const auto& storedBearings = _attributesManager.getBearingVectors();

    if (storedNodes.size() >= 2) {
        node1.node = storedNodes[0];
        node2.node = storedNodes[1];

        // Node 1과 Node 2에 해당하는 BearingVectors 추가
        for (const auto& bearing : storedBearings) {
            if (bearing.getNodeIndex() == node1.node.GetSphericalNodeVector().i_n) {
                node1.bearings.push_back(bearing);
            } else if (bearing.getNodeIndex() == node2.node.GetSphericalNodeVector().i_n) {
                node2.bearings.push_back(bearing);
            }
        }

        // LinerSegment 생성 및 AttributesManager에 추가
        LinerSegment linerSegment(node1, node2, 5.0f);

        // AttributesManager를 사용하여 LinerSegment 생성 및 저장
        _attributesManager.CreateLinerSegment(linerSegment);

        // LinerSegment의 샘플링된 점 출력
        linerSegment.SamplingBSpline();
        const auto& sampledPoints = linerSegment.getSampledPoints();
        if (!sampledPoints.empty()) {
            std::cout << "Sampled Points from LinerSegment:" << std::endl;
            for (const auto& point : sampledPoints) {
                std::cout << "(x: " << point.x << ", y: " << point.y << ", z: " << point.z << ")" << std::endl;
            }
        } else {
            std::cout << "No sampled points generated from LinerSegment." << std::endl;
        }
    } else {
        std::cout << "Not enough NodeVectors in AttributesManager to create a LinerSegment." << std::endl;
    }
}

void AttributesManagerTest(AttributesManager& _attributesManager) {
    // NodeVectorTest 함수를 호출하여 NodeVector 생성 및 저장
    NodeVectorTest(_attributesManager);

    // BearingVectorTest 함수를 호출하여 BearingVector 생성 및 저장
    BearingVectorTest(_attributesManager);

    // LinerSegmentTest 함수를 호출하여 LinerSegment 생성 및 저장
    LinerSegmentTest(_attributesManager);

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

void YamlConverterTest(AttributesManager& _attributesManager) {
    YamlConverter yamlConverter;
    std::string yamlString = yamlConverter.ToString(_attributesManager);

    // YAML 문자열을 출력하거나 파일로 저장 (로그로 저장)
    std::ofstream logFile("attributes_log.yaml");
    if (logFile.is_open()) {
        logFile << yamlString;
        logFile.close();
        std::cout << "YAML data saved to attributes_log.yaml" << std::endl;
    } else {
        std::cerr << "Error: Unable to open log file for writing." << std::endl;
    }
}

int main() {
    AttributesManager _attributesManager;

    // 테스트 함수 호출
    AttributesManagerTest(_attributesManager);
    YamlConverterTest(_attributesManager);

    std::cout << "Hello World!!!" << std::endl;
    return 0;
}
