/* main.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 13, 2024
 * 
 */
#include <iostream>
#include <cmath>
#include <fstream>
#include <GLUT/glut.h> // Added for OpenGL
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

// 전역 변수 선언
AttributesManager attributesManager;

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
    BearingVector bearingVector1_1(1, 2, node, static_cast<float>(M_PI / 2), static_cast<float>(M_PI / 9), 8.0f, 1.0f, 5.0f); // Node 1에 대한 두 번째 벡터
    BearingVector bearingVector2(2, 1, node, static_cast<float>(M_PI / 3), static_cast<float>(M_PI / 8), 2.0f, 4.0f, 3.0f);
    BearingVector bearingVector3(3, 1, node, static_cast<float>(M_PI / 6), static_cast<float>(M_PI / 5), 1.0f, 1.5f, 2.5f);

    // AttributesManager를 사용하여 BearingVector 생성 및 저장
    _attributesManager.CreateBearingVector(bearingVector1);
    _attributesManager.CreateBearingVector(bearingVector1_1); // 추가된 BearingVector를 다시 활성화합니다.
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
                std::cout << "Bearing vector added to Node 1, Node Index: " << node1.node.GetSphericalNodeVector().i_n << std::endl;
            } else if (bearing.getNodeIndex() == node2.node.GetSphericalNodeVector().i_n) {
                node2.bearings.push_back(bearing);
                std::cout << "Bearing vector added to Node 2, Node Index: " << node2.node.GetSphericalNodeVector().i_n << std::endl;
            }
        }

        // Bearing Vector가 잘 추가되었는지 확인
        std::cout << "Node 1 bearing vectors count: " << node1.bearings.size() << std::endl;
        std::cout << "Node 2 bearing vectors count: " << node2.bearings.size() << std::endl;

        // LinerSegment 생성 및 AttributesManager에 추가
        LinerSegment linerSegment(node1, node2, 50);

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
}

void YamlConverterTest(AttributesManager& _attributesManager) {
    YamlConverter yamlConverter;
    std::string yamlString = yamlConverter.ToString(_attributesManager);

    // YAML 문자열을 출력하거나 파일로 저장 (로그로 저장)
    std::ofstream logFile("log/attributes_log.yaml");
    if (logFile.is_open()) {
        logFile << yamlString;
        logFile.close();
        std::cout << "YAML data saved to attributes_log.yaml" << std::endl;
    } else {
        std::cerr << "Error: Unable to open log file for writing." << std::endl;
    }
}

void DrawTest() {
    // OpenGL 그리기 설정
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 화면과 깊이 버퍼 지우기

    // 윈도우의 너비와 높이 가져오기
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    // 뷰포트 설정
    glViewport(0, 0, width, height);

    // 투영 행렬 설정
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<float>(width) / static_cast<float>(height), 1.0, 1000.0);

    // 모델뷰 행렬로 전환
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 카메라 설정
    gluLookAt(0.0, 0.0, 15.0,  // 카메라 위치
              5.0, 12.0, 5.0,    // 바라보는 지점
              0.0, 1.0, 0.0);   // 상단을 위로 설정

    // AttributesManager를 통해 객체들 가져오기
    const auto& nodeVectors = attributesManager.getNodeVectors();
    const auto& bearingVectors = attributesManager.getBearingVectors();
    const auto& linerSegments = attributesManager.getLinerSegments();

    if (nodeVectors.empty() || bearingVectors.empty() || linerSegments.empty()) {
        std::cerr << "Insufficient objects for rendering!" << std::endl;
        return;
    }

    // **노드 벡터 그리기 (녹색)**
    glColor3f(0.0f, 1.0f, 0.0f);  // 녹색
    glPointSize(10.0f); // 점 크기 증가
    glBegin(GL_POINTS);
    for (const auto& node : nodeVectors) {
        CartesianNodeVector cartNode = node.GetCartesianNodeVector();
        Vector3 nodePos(cartNode.x_i_n, cartNode.y_i_n, cartNode.z_i_n);
        glVertex3f(nodePos.x, nodePos.y, nodePos.z);
    }
    glEnd();

    // **베어링 벡터 그리기**
    glColor3f(0.0f, 1.0f, 1.0f);  // 청록색
    glPointSize(8.0f); // 점 크기 설정
    glBegin(GL_POINTS);
    for (const auto& bearing : bearingVectors) {
        CartesianBearingVector cartBearing = bearing.convertToCartesianBearingVector();
        Vector3 bearingPos(cartBearing.x, cartBearing.y, cartBearing.z);
        glVertex3f(bearingPos.x, bearingPos.y, bearingPos.z);
    }
    glEnd();

    // **노드와 베어링 벡터 사이의 선 그리기 (녹색)**
    glColor3f(0.0f, 1.0f, 0.0f); // 녹색
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (const auto& bearing : bearingVectors) {
        int nodeIndex = bearing.getNodeIndex() - 1; // 인덱스 조정 (0부터 시작)
        if (nodeIndex >= 0 && nodeIndex < nodeVectors.size()) {
            CartesianNodeVector cartNode = nodeVectors[nodeIndex].GetCartesianNodeVector();
            Vector3 nodePos(cartNode.x_i_n, cartNode.y_i_n, cartNode.z_i_n);

            CartesianBearingVector cartBearing = bearing.convertToCartesianBearingVector();
            Vector3 bearingPos(cartBearing.x, cartBearing.y, cartBearing.z);

            glVertex3f(nodePos.x, nodePos.y, nodePos.z);
            glVertex3f(bearingPos.x, bearingPos.y, bearingPos.z);
        }
    }
    glEnd();

    // **힘 벡터 그리기 (빨간색)**
    glColor3f(1.0f, 0.0f, 0.0f);  // 빨간색
    glBegin(GL_LINES);
    for (const auto& bearing : bearingVectors) {
        CartesianBearingVector cartBearing = bearing.convertToCartesianBearingVector();
        Vector3 bearingPos(cartBearing.x, cartBearing.y, cartBearing.z);

        BearingVectorForce force = bearing.getForce();
        Vector3 forceVec(force.f_x, force.f_y, force.f_z);
        Vector3 forceEnd = bearingPos + forceVec;

        glVertex3f(bearingPos.x, bearingPos.y, bearingPos.z);
        glVertex3f(forceEnd.x, forceEnd.y, forceEnd.z);
    }
    glEnd();

    // **LinerSegment를 사용하여 샘플링된 포인트 그리기 (회색)**
    glColor3f(0.5f, 0.5f, 0.5f);  // 회색
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (const auto& segment : linerSegments) {
        const std::vector<Vector3>& sampledPoints = segment.getSampledPoints();
        for (const auto& point : sampledPoints) {
            glVertex3f(point.x, point.y, point.z);
        }
    }
    glEnd();

    // 그린 내용을 화면에 출력
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    AttributesManager _attributesManager;

    // 테스트 함수 호출
    AttributesManagerTest(_attributesManager);
    YamlConverterTest(_attributesManager);

    // 전역 AttributesManager 업데이트
    attributesManager = _attributesManager;

    // OpenGL 초기화
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Draw Test");

    // 배경 색상을 설정
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    // 콜백 함수 등록 (DrawTest 함수 등록)
    glutDisplayFunc(DrawTest);

    // OpenGL 메인 루프 시작
    glutMainLoop();

    return 0;
}
