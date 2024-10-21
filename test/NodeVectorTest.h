// NodeVectorTest.h
#include "NodeVector.h"

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