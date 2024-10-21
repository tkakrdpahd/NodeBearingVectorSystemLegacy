/* YamlConverter.h
 * Linked file YamlConverter.cpp
 * Security: Top Secret
 * Author: Minseok Doo
 * Date: Oct 20, 2024
 * 
 * Purpose of Class
 * Convert AttributeManager to .yaml
 * 
 * Equations
 * Convert AttributesManager to String
 * Convert string to .yaml
 */

#ifndef YAML_CONVERTER_H
#define YAML_CONVERTER_H

#include "AttributesManager.h"
#include <string>

class YamlConverter {
public:
    YamlConverter();
    ~YamlConverter();

    // AttributesManager 객체를 문자열로 변환하는 메서드
    std::string ToString(const AttributesManager &attributesManager);

    // AttributesManager 객체를 YAML 파일로 변환하는 메서드
    void ToYaml(const AttributesManager &attributesManager);
};

#endif // YAML_CONVERTER_H