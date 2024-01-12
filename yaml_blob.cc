#include "mutator.h"

Mutator::YamlMutator(const std::string& input_file, const std::string& output_file)
    : input_file_(input_file), output_file_(output_file) {}

void YamlMutator::MutateAndSave() {
    // 1. 读取 YAML 文件
    std::ifstream file(input_file_);
    YAML::Node yamlData = YAML::Load(file);

    // 2. 将 YAML 数据转换为 vector<uint8_t>
    std::vector<uint8_t> data = YamlToVector(yamlData);

    // 3. 使用 Mutator 变异数据
    std::vector<uint8_t> mutatedData = mutator_.Mutate(data);

    // 4. 将变异后的数据转换回 YAML
    YAML::Node mutatedYaml = VectorToYaml(mutatedData);

    // 5. 输出变异后的 YAML 文件
    std::ofstream outFile(output_file_);
    outFile << mutatedYaml;
}

std::vector<uint8_t> YamlMutator::YamlToVector(const YAML::Node& yamlData) {
    // 实现转换逻辑
    // ...
}

YAML::Node YamlMutator::VectorToYaml(const std::vector<uint8_t>& data) {
    // 实现转换逻辑
    // ...
}
