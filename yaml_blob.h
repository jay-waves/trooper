#ifndef YAML_MUTATOR_H
#define YAML_MUTATOR_H

#include <fstream>
#include <vector>
#include "yaml-cpp/yaml.h"
#include "mutator.h" // 确保包含 mutator.cc 中的 Mutator 类定义

namespace trooper {
  class YamlMutator {
  public:
    YamlMutator(const std::string& input_file, const std::string& output_file);
    void MutateAndSave();

  private:
    std::string input_file_;
    std::string output_file_;
    Mutator mutator_;

    std::vector<uint8_t> YamlToVector(const YAML::Node& yamlData);
    YAML::Node VectorToYaml(const std::vector<uint8_t>& data);
    // Read
    // Write
  };
} // namespace trooper

#endif // YAML_MUTATOR_H
