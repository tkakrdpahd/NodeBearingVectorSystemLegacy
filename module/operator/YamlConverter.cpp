/* YamlConverter.cpp
 * Implementation of the YamlConverter class
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 20, 2024
 */

#include "YamlConverter.h"
#include "AttributesManager.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>

YamlConverter::YamlConverter() {}

YamlConverter::~YamlConverter() {}

std::string YamlConverter::ToString(const AttributesManager &attributesManager) {
    YAML::Emitter out;
    Attributes attributes = attributesManager.ReadAllAttributes();

    out << YAML::BeginMap;

    // Node Vectors
    out << YAML::Key << "NodeVectors";
    out << YAML::BeginSeq;
    for (const auto& node : attributes.nodeVectors) {
        auto spherical = node.GetSphericalNodeVector();
        auto cartesian = node.GetCartesianNodeVector();

        out << YAML::BeginMap;
        // Include both spherical and cartesian representations
        out << YAML::Key << "index" << YAML::Value << spherical.i_n;
        out << YAML::Key << "spherical" << YAML::BeginMap;
        out << YAML::Key << "r" << YAML::Value << spherical.r_i_n;
        out << YAML::Key << "theta" << YAML::Value << spherical.theta_i_n;
        out << YAML::Key << "phi" << YAML::Value << spherical.phi_i_n;
        out << YAML::EndMap;

        out << YAML::Key << "cartesian" << YAML::BeginMap;
        out << YAML::Key << "x" << YAML::Value << cartesian.x_i_n;
        out << YAML::Key << "y" << YAML::Value << cartesian.y_i_n;
        out << YAML::Key << "z" << YAML::Value << cartesian.z_i_n;
        out << YAML::EndMap;

        out << YAML::EndMap;
    }
    out << YAML::EndSeq;

    // Bearing Vectors
    out << YAML::Key << "BearingVectors";
    out << YAML::BeginSeq;
    for (const auto& bearing : attributes.bearingVectors) {
        out << YAML::BeginMap;
        out << YAML::Key << "nodeIndex" << YAML::Value << bearing.getNodeIndex();
        out << YAML::Key << "depth" << YAML::Value << bearing.getDepth();

        out << YAML::Key << "angles" << YAML::BeginMap;
        out << YAML::Key << "phi" << YAML::Value << bearing.getPhi();
        out << YAML::Key << "theta" << YAML::Value << bearing.getTheta();
        out << YAML::EndMap;

        auto force = bearing.getForce();
        out << YAML::Key << "force" << YAML::BeginMap;
        out << YAML::Key << "f_x" << YAML::Value << force.f_x;
        out << YAML::Key << "f_y" << YAML::Value << force.f_y;
        out << YAML::Key << "f_z" << YAML::Value << force.f_z;
        out << YAML::EndMap;

        out << YAML::EndMap;
    }
    out << YAML::EndSeq;

    // Liner Segments
    out << YAML::Key << "LinerSegments";
    out << YAML::BeginSeq;
    for (const auto& segment : attributes.linerSegments) {
        out << YAML::BeginMap;

        // LinerSegmentData를 사용하여 모든 멤버 변수 출력
        LinerSegmentData segmentData = segment.ReturnLinerSegmentData();

        // LinerBufferIndex 출력
        out << YAML::Key << "LinerBufferIndex" << YAML::Value << segmentData.LinerBufferIndex;

        // NodeStart 정보 출력
        out << YAML::Key << "NodeStart";
        out << YAML::BeginMap;
        auto sphericalStart = segmentData.NodeStart.GetSphericalNodeVector();
        auto cartesianStart = segmentData.NodeStart.GetCartesianNodeVector();
        out << YAML::Key << "index" << YAML::Value << sphericalStart.i_n;
        out << YAML::Key << "spherical" << YAML::BeginMap;
        out << YAML::Key << "r" << YAML::Value << sphericalStart.r_i_n;
        out << YAML::Key << "theta" << YAML::Value << sphericalStart.theta_i_n;
        out << YAML::Key << "phi" << YAML::Value << sphericalStart.phi_i_n;
        out << YAML::EndMap;
        out << YAML::Key << "cartesian" << YAML::BeginMap;
        out << YAML::Key << "x" << YAML::Value << cartesianStart.x_i_n;
        out << YAML::Key << "y" << YAML::Value << cartesianStart.y_i_n;
        out << YAML::Key << "z" << YAML::Value << cartesianStart.z_i_n;
        out << YAML::EndMap;
        out << YAML::EndMap;

        // NodeEnd 정보 출력
        out << YAML::Key << "NodeEnd";
        out << YAML::BeginMap;
        auto sphericalEnd = segmentData.NodeEnd.GetSphericalNodeVector();
        auto cartesianEnd = segmentData.NodeEnd.GetCartesianNodeVector();
        out << YAML::Key << "index" << YAML::Value << sphericalEnd.i_n;
        out << YAML::Key << "spherical" << YAML::BeginMap;
        out << YAML::Key << "r" << YAML::Value << sphericalEnd.r_i_n;
        out << YAML::Key << "theta" << YAML::Value << sphericalEnd.theta_i_n;
        out << YAML::Key << "phi" << YAML::Value << sphericalEnd.phi_i_n;
        out << YAML::EndMap;
        out << YAML::Key << "cartesian" << YAML::BeginMap;
        out << YAML::Key << "x" << YAML::Value << cartesianEnd.x_i_n;
        out << YAML::Key << "y" << YAML::Value << cartesianEnd.y_i_n;
        out << YAML::Key << "z" << YAML::Value << cartesianEnd.z_i_n;
        out << YAML::EndMap;
        out << YAML::EndMap;

        // LevelOfDetail과 alpha 출력
        out << YAML::Key << "LevelOfDetail" << YAML::Value << segmentData.LevelOfDetail;
        out << YAML::Key << "alpha" << YAML::Value << segmentData.alpha;

        // Control Points 출력
        const auto& controlPoints = segment.getControlPoints();
        out << YAML::Key << "controlPoints" << YAML::BeginSeq;
        for (const auto& point : controlPoints) {
            out << YAML::BeginMap;
            out << YAML::Key << "x" << YAML::Value << point.x;
            out << YAML::Key << "y" << YAML::Value << point.y;
            out << YAML::Key << "z" << YAML::Value << point.z;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        // Sampled Points 출력
        const auto& sampledPoints = segment.getSampledPoints();
        out << YAML::Key << "sampledPoints" << YAML::BeginSeq;
        for (const auto& point : sampledPoints) {
            out << YAML::BeginMap;
            out << YAML::Key << "x" << YAML::Value << point.x;
            out << YAML::Key << "y" << YAML::Value << point.y;
            out << YAML::Key << "z" << YAML::Value << point.z;
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::EndMap;
    }
    out << YAML::EndSeq;

    out << YAML::EndMap;

    return out.c_str();
}

void YamlConverter::ToYaml(const AttributesManager &attributesManager) {
    std::string yamlString = ToString(attributesManager);
    std::ofstream fout("attributes.yaml");
    if (fout.is_open()) {
        fout << yamlString;
        fout.close();
    } else {
        std::cerr << "Error: Unable to open file for writing YAML." << std::endl;
    }
}