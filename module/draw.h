/* Draw.h
 * Linked file: Draw.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 20, 2024
 * 
 * Purpose:
 * Draw Vector with OpenGL
 */

#ifndef DRAW_H
#define DRAW_H
// AttributesManager 포함
#include "AttributesManager.h"
#ifdef __APPLE__
// MacOS 환경
#include <GLUT/glut.h>
#else
// 다른 환경 (Linux 등)
#include <GL/glut.h>
#endif

class Draw {
    private:
        AttributesManager& attributesManager; // AttributesManager에 대한 참조
        // Function to draw a point at the given Cartesian coordinates (x, y, z)
        void DrawPoint(float x, float y, float z, float size = 5.0f);
        // Function to draw a line between two points
        void DrawLine(const Vector3& start, const Vector3& end, float lineWidth = 2.0f);
    public:
        // Constructor
        Draw(AttributesManager& manager);
        // Function to initialize OpenGL settings
        void InitializeOpenGL();
        // Function to set up the viewport and projection
        void SetupViewport(int width, int height);
        // UpdateCameraLocation by gluLookAt();
        void UpdateCameraLocation();
        // Drawing functions
        void DrawNodeVector();
        void DrawBearingVector();
        void DrawForce();
        void DrawSamplePoint();
        // Function to display points on the screen (OpenGL rendering loop)
        void Display();
};

#endif // DRAW_H
