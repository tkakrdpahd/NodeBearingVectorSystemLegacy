/* main.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 13, 2024
 */

// 헤더 파일 포함
#include <iostream>
#include <cmath>
#include <fstream>
#include <thread>
#ifdef __APPLE__
#include <GLUT/glut.h> // MacOS 환경
#else
#include <GL/glut.h>   // 다른 환경
#endif
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h> // Unix/Linux 계열
#endif

// 나머지 헤더 파일 포함
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
Draw* draw = nullptr; // Draw 클래스 포인터 초기화

// 테스트 함수들
void NodeVectorTest(AttributesManager& _attributesManager) {
    // 여러 개의 SphericalNodeVector 생성 및 NodeVector 초기화 (PI 단위 사용)
    Vector3 sphericalCoords1(10.0f, static_cast<float>(M_PI / 2), static_cast<float>(M_PI / 4)); // r, theta, phi
    Vector3 sphericalCoords2(15.0f, static_cast<float>(M_PI / 4), static_cast<float>(M_PI / 2)); // r, theta, phi

    // SphericalNodeVector 생성자 호출: (int, Vector3)
    SphericalNodeVector sphericalNode1(1, sphericalCoords1);
    SphericalNodeVector sphericalNode2(2, sphericalCoords2);

    // NodeVector 생성자 호출 (SphericalNodeVector 사용)
    NodeVector node01(sphericalNode1);
    NodeVector node02(sphericalNode2);

    // AttributesManager를 사용하여 NodeVector 생성 및 저장
    _attributesManager.CreateNodeVector(node01);
    _attributesManager.CreateNodeVector(node02);
}

void BearingVectorTest(AttributesManager& _attributesManager) {
    // Bearing Vector 생성 및 초기화
    const auto& storedNodes = _attributesManager.getNodeVectors();
    if (storedNodes.empty()) {
        std::cerr << "No nodes available for creating BearingVectors." << std::endl;
        return;
    }

    // 첫 번째 노드를 사용하여 BearingVectors 생성
    NodeVector node = storedNodes[0];

    BearingVector bearingVector1(1, 1, node, static_cast<float>(M_PI / 4), static_cast<float>(M_PI / 6), 5.0f, 3.0f, 2.0f);
    BearingVector bearingVector1_1(1, 2, node, static_cast<float>(M_PI / -2), static_cast<float>(M_PI / 0.1), 8.0f, 1.0f, 5.0f); // Node 1에 대한 두 번째 벡터
    BearingVector bearingVector2(2, 1, node, static_cast<float>(M_PI / 3), static_cast<float>(M_PI / -4), 2.0f, 4.0f, 3.0f);

    // AttributesManager를 사용하여 BearingVector 생성 및 저장
    _attributesManager.CreateBearingVector(bearingVector1);
    _attributesManager.CreateBearingVector(bearingVector1_1);
    _attributesManager.CreateBearingVector(bearingVector2);
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
        linerSegment.SamplingBezierCurve(); // 함수 이름 변경됨
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

// 수정된 YamlConverterTest 함수
void YamlConverterTest(AttributesManager& _attributesManager) {
    YamlConverter yamlConverter;
    std::string yamlString = yamlConverter.ToString(_attributesManager);

    // 로그 디렉토리가 없는 경우 생성
    struct stat info;
    if (stat("../log", &info) != 0 || !(info.st_mode & S_IFDIR)) {
        // 디렉토리가 없으므로 생성
        #ifdef _WIN32
            int result = _mkdir("../log");
        #else
            int result = mkdir("../log", 0777);
        #endif
        if (result != 0) {
            std::cerr << "Error: Unable to create log directory." << std::endl;
            return;
        }
    }

    // YAML 문자열을 출력하거나 파일로 저장 (로그로 저장)
    std::ofstream logFile("../log/attributes_log.yaml");
    if (logFile.is_open()) {
        logFile << yamlString;
        logFile.close();
        std::cout << "YAML data saved to attributes_log.yaml" << std::endl;
    } else {
        std::cerr << "Error: Unable to open log file for writing." << std::endl;
    }
}

// Display 콜백 함수
void DisplayCallback() {
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
    gluLookAt(0.0, 0.0, 50.0,  // 카메라 위치
              0.0, 0.0, 0.0,   // 바라보는 지점
              -1.0, 1.0, 1.0);   // 상단을 위로 설정

    std::cout << "DisplayCallback called." << std::endl; // 디버깅용 로그 추가
    // `Draw` 객체가 전역으로 선언되어 있어야 함
    if (draw) {
        draw->DrawNodeVector();
        draw->DrawBearingVector();
        draw->DrawForce();
        draw->DrawSamplePoint();
    } else {
        std::cerr << "Draw object is not initialized." << std::endl;
    }

    // 그린 내용을 화면에 출력
    glutSwapBuffers();
}

// Socket 서버 테스트 함수로 AttributesManager의 데이터를 반환하게 함
void SocketServerTest(AttributesManager& attributesManager) {
    int serverPort = 8080;
    SocketServer server(serverPort, attributesManager);

    if (server.startServer()) {
        server.listenForClients();
    } else {
        std::cerr << "Failed to start the server." << std::endl;
    }
}

int main(int argc, char** argv) {
    // 전역 AttributesManager 사용
    // AttributesManager attributesManager; // 제거

    // 테스트 함수 호출 (데이터 추가)
    AttributesManagerTest(attributesManager);
    YamlConverterTest(attributesManager);

    // 서버를 실행하여 클라이언트 요청에 응답 (별도의 스레드)
    std::thread serverThread(SocketServerTest, std::ref(attributesManager));
    serverThread.detach(); // 스레드를 분리하여 메인 스레드와 독립적으로 실행

    // Draw 객체 생성 및 전역 변수에 할당
    draw = new Draw(attributesManager);

    // OpenGL 초기화
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Draw Test");

    // OpenGL 초기화 및 뷰포트 설정
    draw->InitializeOpenGL();

    // 콜백 함수 등록 (DisplayCallback 함수 등록)
    glutDisplayFunc(DisplayCallback);

    // OpenGL 메인 루프 시작 (블로킹 호출)
    glutMainLoop();

    // 프로그램 종료 시 메모리 해제
    delete draw;

    // 서버 스레드 종료 대기 (실제로는 OpenGL 루프가 종료되지 않으므로 필요 없음)
    // if (serverThread.joinable()) {
    //     serverThread.join();
    // }

    return 0;
}
