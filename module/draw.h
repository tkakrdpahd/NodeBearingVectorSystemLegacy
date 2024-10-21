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

#ifdef __APPLE__
#include <GLUT/glut.h>   // MacOS 환경
#else
#include <GL/glut.h>     // 다른 환경 (Linux 등)
#endif

// Function to initialize OpenGL settings
void InitializeOpenGL();

// Function to set up the viewport and projection
void SetupViewport(int width, int height);

// Function to draw a point at the given Cartesian coordinates (x, y, z)
void DrawPoint(float x, float y, float z);

// Function to display points on the screen (OpenGL rendering loop)
void Display();

#endif // DRAW_H
