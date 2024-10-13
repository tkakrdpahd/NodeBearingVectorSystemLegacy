/* LinerSegment.h
 * Linked file LinerSegment.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 7, 2024
 * 
 * Purpose:
 * Sampling the B-Spline line
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

#include "NodeVector.h"
#include "BearingVector.h"
#include <vector>
#include <cmath>