/* BearingVector.h
 * Linked file BearingVector.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 13, 2024
 * 
 * Purpose: Define functions for the BearingVector class
 * 
 * Equations
 * Equ(1): \vec{B_i}=\left(\begin{matrix}\sin{\phi_i}\cos{\theta_i}\\\sin{\phi_i}\sin{\theta_i}\cos{\phi_i}\\\end{matrix}\right)
 * Equ(2): \vec{B_{i,d}}=\left(B_{i,d,x},B_{i,d,y},B_{i,d,z}\right)
 * Equ(3): \vec{F_i}=F_{x_i}\thinsp\hat{x}+F_{y_i}\thinsp\hat{y}+F_{z_i}\thinsp\hat{z}
 */

#include "CoordinateConverter.h"
#include "NodeVector.h"