/* Draw.cpp
 * Linked file Draw.h
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 20, 2024
 */

#ifdef __APPLE__
#include <GLUT/glut.h>   // MacOS 환경
#else
#include <GL/glut.h>     // 다른 환경 (Linux 등)
#endif
#include "Draw.h"

// OpenGL 초기화 및 뷰포트 설정
void InitializeOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // 배경 색상을 검은색으로 설정
    glEnable(GL_DEPTH_TEST);               // 깊이 테스트 활성화
}

void SetupViewport(int width, int height) {
    glViewport(0, 0, width, height);       // 뷰포트 설정
    glMatrixMode(GL_PROJECTION);           // 프로젝션 모드로 전환
    glLoadIdentity();                      // 투영 행렬 초기화

    // 원근 투영 설정
    gluPerspective(45.0, (float)width / (float)height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);            // 다시 모델뷰 모드로 전환
}

void DrawPoint(float x, float y, float z) {
    glPointSize(2.5);  // 점의 크기 설정
    glBegin(GL_POINTS); // 점 그리기 시작
        glVertex3f(x, y, z); // 지정된 좌표에 점을 그림
    glEnd(); // 점 그리기 종료
}
