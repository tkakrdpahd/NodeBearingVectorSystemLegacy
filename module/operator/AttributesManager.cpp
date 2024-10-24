/* AttributesManager.cpp
 * Linked file: AttributesManager.h
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 13, 2024
 * 
 * Purpose: Implement the AttributesManager class
 */

#include "AttributesManager.h"

// 생성자
AttributesManager::AttributesManager() {
    // 초기화 코드 (필요 시)
}

// 소멸자
AttributesManager::~AttributesManager() {
    // 정리 코드 (필요 시)
}

// NodeVector 관련 함수 구현

// NodeVector 생성
NodeVector AttributesManager::CreateNodeVector(const NodeVector& node) {
    nodeVectors.push_back(node);
    return nodeVectors.back();
}

// NodeVector 수정
bool AttributesManager::EditNodeVector(int index, const NodeVector& newNode) {
    for(auto &node : nodeVectors) {
        if(node.GetSphericalNodeVector().i_n == index) {
            node = newNode;
            return true;
        }
    }
    return false; // 해당 인덱스를 찾지 못함
}

// NodeVector 삭제
bool AttributesManager::DeleteNodeVector(int index) {
    for(auto it = nodeVectors.begin(); it != nodeVectors.end(); ++it) {
        if(it->GetSphericalNodeVector().i_n == index) {
            nodeVectors.erase(it);
            return true;
        }
    }
    return false; // 해당 인덱스를 찾지 못함
}

// BearingVector 관련 함수 구현

// BearingVector 생성
BearingVector AttributesManager::CreateBearingVector(const BearingVector& bearing) {
    bearingVectors.push_back(bearing);
    return bearingVectors.back();
}

// BearingVector 수정
bool AttributesManager::EditBearingVector(int index, const BearingVector& newBearing) {
    for(auto &bearing : bearingVectors) {
        if(bearing.getNodeIndex() == index) { // 또는 다른 고유 식별자를 사용
            bearing = newBearing;
            return true;
        }
    }
    return false; // 해당 인덱스를 찾지 못함
}

// BearingVector 삭제
bool AttributesManager::DeleteBearingVector(int index) {
    for(auto it = bearingVectors.begin(); it != bearingVectors.end(); ++it) {
        if(it->getNodeIndex() == index) { // 또는 다른 고유 식별자를 사용
            bearingVectors.erase(it);
            return true;
        }
    }
    return false; // 해당 인덱스를 찾지 못함
}

// LinerSegment 관련 함수 구현

// LinerSegment 생성
LinerSegment AttributesManager::CreateLinerSegment(const LinerSegment& segment) {
    linerSegments.push_back(segment);
    return linerSegments.back();
}

// LinerSegment 수정
bool AttributesManager::EditLinerSegment(int index, const LinerSegment& newSegment) {
    // LinerSegment 클래스에 고유 식별자가 추가되어야 합니다.
    // 예: segment.getId() == index
    // 현재 예제에서는 인덱스로 접근합니다.
    if(index >= 0 && index < linerSegments.size()) {
        linerSegments[index] = newSegment;
        return true;
    }
    return false;
}

// LinerSegment 삭제
bool AttributesManager::DeleteLinerSegment(int index) {
    if(index >= 0 && index < linerSegments.size()) {
        linerSegments.erase(linerSegments.begin() + index);
        return true;
    }
    return false;
}

// 모든 Attributes 읽기
Attributes AttributesManager::ReadAllAttributes() const {
    Attributes attrs;
    attrs.nodeVectors = nodeVectors;
    attrs.bearingVectors = bearingVectors;
    attrs.linerSegments = linerSegments;
    return attrs;
}

// 모든 Attributes 삭제
void AttributesManager::DeleteAllAttributes() {
    nodeVectors.clear();
    bearingVectors.clear();
    linerSegments.clear();
}
