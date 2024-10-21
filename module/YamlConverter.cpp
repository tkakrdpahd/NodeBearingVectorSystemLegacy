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
        out << YAML::Key << "index" << YAML::Value << bearing.getNodeIndex();
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
