/* AttributesManager.h
 * Linked file: AttributesManager.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 13, 2024
 * 
 * Purpose: Manage Vector Attributes
 */

#ifndef ATTRIBUTESMANAGER_H
#define ATTRIBUTESMANAGER_H

#include "NodeVector.h"
#include "BearingVector.h"
#include "LinerSegment.h"
#include <vector>

struct Attributes {
    std::vector<NodeVector> nodeVectors;
    std::vector<BearingVector> bearingVectors;
    std::vector<LinerSegment> linerSegments;
};

class AttributesManager {
private:
    std::vector<NodeVector> nodeVectors;
    std::vector<BearingVector> bearingVectors;
    std::vector<LinerSegment> linerSegments;

public:
    AttributesManager();
    ~AttributesManager();

    // NodeVector 관련 함수
    NodeVector CreateNodeVector(const NodeVector& node);
    bool EditNodeVector(int index, const NodeVector& newNode);
    bool DeleteNodeVector(int index);

    // BearingVector 관련 함수
    BearingVector CreateBearingVector(const BearingVector& bearing);
    bool EditBearingVector(int index, const BearingVector& newBearing);
    bool DeleteBearingVector(int index);

    // LinerSegment 관련 함수
    LinerSegment CreateLinerSegment(const LinerSegment& segment);
    bool EditLinerSegment(int index, const LinerSegment& newSegment);
    bool DeleteLinerSegment(int index);

    // Attributes 관련 함수
    Attributes ReadAllAttributes() const;
    void DeleteAllAttributes();

    // 접근자 함수 추가
    const std::vector<NodeVector>& getNodeVectors() const { return nodeVectors; }
    const std::vector<BearingVector>& getBearingVectors() const { return bearingVectors; }
    const std::vector<LinerSegment>& getLinerSegments() const { return linerSegments; }
};

#endif // ATTRIBUTESMANAGER_H
