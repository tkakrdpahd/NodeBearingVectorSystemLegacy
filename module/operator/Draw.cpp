/* Draw.cpp
 * Linked file Draw.h
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 20, 2024
 */
#include "Draw.h"
#ifdef __APPLE__
#include <GLUT/glut.h>   // MacOS 환경
#else
#include <GL/glut.h>     // 다른 환경 (Linux 등)
#endif

// Constructor
Draw::Draw(AttributesManager& manager) : attributesManager(manager) {}

// OpenGL 초기화
void Draw::InitializeOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // 배경 색상을 검은색으로 설정
    glEnable(GL_DEPTH_TEST);               // 깊이 테스트 활성화
}

// 뷰포트 설정
void Draw::SetupViewport(int width, int height) {
    glViewport(0, 0, width, height);       // 뷰포트 설정
    glMatrixMode(GL_PROJECTION);           // 프로젝션 모드로 전환
    glLoadIdentity();                      // 투영 행렬 초기화

    // 원근 투영 설정
    gluPerspective(45.0, static_cast<float>(width) / static_cast<float>(height), 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);            // 다시 모델뷰 모드로 전환
}

// 점 그리기 함수
void Draw::DrawPoint(float x, float y, float z, float size) {
    glPointSize(size);  // 점의 크기 설정
    glBegin(GL_POINTS); // 점 그리기 시작
        glVertex3f(x, y, z); // 지정된 좌표에 점을 그림
    glEnd(); // 점 그리기 종료
}

// 선 그리기 함수
void Draw::DrawLine(const Vector3& start, const Vector3& end, float lineWidth) {
    glLineWidth(lineWidth);
    glBegin(GL_LINES);
        glVertex3f(start.x, start.y, start.z);
        glVertex3f(end.x, end.y, end.z);
    glEnd();
}

// 노드 벡터 그리기
void Draw::DrawNodeVector() {
    const auto& nodeVectors = attributesManager.getNodeVectors();
    if (nodeVectors.empty()) return;

    for (const auto& node : nodeVectors) {
        glColor3f(0.0f, 1.0f, 0.0f);  // 녹색
        CartesianNodeVector cartNode = node.GetCartesianNodeVector();
        // 수정: x_i_n, y_i_n, z_i_n 대신 cartesianCoords.x, cartesianCoords.y, cartesianCoords.z 사용
        DrawPoint(cartNode.cartesianCoords.x, cartNode.cartesianCoords.y, cartNode.cartesianCoords.z, 10.0f);
    }
}

// 베어링 벡터 그리기
void Draw::DrawBearingVector() {
    const auto& bearingVectors = attributesManager.getBearingVectors();
    if (bearingVectors.empty()) return;

    for (const auto& bearing : bearingVectors) {
        CartesianBearingVector cartBearing = bearing.convertToCartesianBearingVector();

        // 베어링 벡터 위치 그리기 (청록색 점)
        glColor3f(0.0f, 1.0f, 1.0f);  // 청록색
        // 수정: cartBearing.x, cartBearing.y, cartBearing.z 대신 cartBearing.cartesianCoords.x, y, z 사용
        DrawPoint(cartBearing.cartesianCoords.x, cartBearing.cartesianCoords.y, cartBearing.cartesianCoords.z, 8.0f);

        // 노드와 베어링 벡터 사이의 선 그리기
        int nodeIndex = bearing.getNodeIndex() - 1; // 인덱스 조정
        const auto& nodeVectors = attributesManager.getNodeVectors();
        if (nodeIndex >= 0 && nodeIndex < nodeVectors.size()) {
            CartesianNodeVector cartNode = nodeVectors[nodeIndex].GetCartesianNodeVector();
            // 수정: x_i_n, y_i_n, z_i_n 대신 cartesianCoords.x, cartesianCoords.y, cartesianCoords.z 사용
            Vector3 nodePos(cartNode.cartesianCoords.x, cartNode.cartesianCoords.y, cartNode.cartesianCoords.z);
            Vector3 bearingPos(cartBearing.cartesianCoords.x, cartBearing.cartesianCoords.y, cartBearing.cartesianCoords.z);

            glColor3f(0.0f, 1.0f, 0.0f); // 녹색
            DrawLine(nodePos, bearingPos, 2.0f);
        }
    }
}

// 힘 벡터 그리기
void Draw::DrawForce() {
    const auto& bearingVectors = attributesManager.getBearingVectors();
    if (bearingVectors.empty()) return;

    for (const auto& bearing : bearingVectors) {
        CartesianBearingVector cartBearing = bearing.convertToCartesianBearingVector();
        Vector3 bearingPos(cartBearing.cartesianCoords.x, cartBearing.cartesianCoords.y, cartBearing.cartesianCoords.z);
        BearingVectorForce force = bearing.getForce();

        // 힘 벡터 (빨간색)
        // 수정: f_x, f_y, f_z 대신 Force.x, Force.y, Force.z 사용
        Vector3 forceVec(force.Force.x, force.Force.y, force.Force.z);
        Vector3 forceEnd = bearingPos + forceVec;

        glColor3f(1.0f, 0.0f, 0.0f);  // 빨간색
        DrawLine(bearingPos, forceEnd, 2.0f);
    }
}

// 샘플링된 포인트 그리기
void Draw::DrawSamplePoint() {
    const auto& linerSegments = attributesManager.getLinerSegments();
    if (linerSegments.empty()) return;

    glColor3f(0.5f, 0.5f, 0.5f);  // 회색
    for (const auto& segment : linerSegments) {
        const std::vector<Vector3>& sampledPoints = segment.getSampledPoints();
        for (const auto& point : sampledPoints) {
            DrawPoint(point.x, point.y, point.z, 5.0f);
        }
    }
}

// Display 함수 (OpenGL 렌더링 루프)
void Draw::Display() {
    // OpenGL 그리기 설정
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 화면과 깊이 버퍼 지우기

    // 모델뷰 행렬 초기화
    glLoadIdentity();

    // 카메라 설정
    gluLookAt(0.0, 0.0, 10.0,  // 카메라 위치 (원래 0,0,0은 바라보는 지점)
              0.0, 0.0, 0.0,    // 바라보는 지점
              0.0, 1.0, 0.0);   // 상단을 위로 설정

    // 각 그리기 함수 호출
    DrawNodeVector();
    DrawBearingVector();
    DrawForce();
    DrawSamplePoint();

    // 그린 내용을 화면에 출력
    glutSwapBuffers();
}
