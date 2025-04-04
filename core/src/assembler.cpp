#include "../../core/include/assembler.hpp"
#include <algorithm>
#include <cstdint>
#include <format>
#include <iterator>
#include <numeric>
#include <regex>
#include <sstream>

Assembler::Assembler() :
    line_number_(0) {
    opcode_map_ = {
        {"NOP", common::OpCode::NOP}, {"ADD", common::OpCode::ADD}, {"SUB", common::OpCode::SUB},
        {"MUL", common::OpCode::MUL}, {"DIV", common::OpCode::DIV}, {"MOD", common::OpCode::MOD},
        {"JMP", common::OpCode::JMP}, {"JNZ", common::OpCode::JNZ}, {"JGZ", common::OpCode::JGZ},
        {"CPSG", common::OpCode::CPS}, {"CPSE", common::OpCode::CPS}, {"SET", common::OpCode::SET},
        {"SAVE", common::OpCode::SAVE}, {"LOAD", common::OpCode::LOAD}, {"READ", common::OpCode::READ},
        {"WRITE", common::OpCode::WRITE}, {"PAGE", common::OpCode::PAGE}
    };

    type_modifier_map_ = {
        {"C", common::TypeModifier::C},
        {"W", common::TypeModifier::W},
        {"SW", common::TypeModifier::SW},
        {"R", common::TypeModifier::R}
    };

    arg_modifier_map_ = {
        {"&", common::ArgModifier::REF}, {">", common::ArgModifier::GZ}, {"=", common::ArgModifier::EQ}
    };
}

common::ByteCode Assembler::Compile(const std::string& source) {
    auto [byte_code, source_to_bytecode_map] = CompileInternal(source);

    return byte_code;
}

std::pair<common::ByteCode, common::SourceToBytecodeMap> Assembler::CompileWithDebugInfo(const std::string& source) {
    return CompileInternal(source);
}

std::pair<common::ByteCode, common::SourceToBytecodeMap> Assembler::CompileInternal(const std::string& source) {
    common::ByteCode byte_code{};
    common::SourceToBytecodeMap source_to_bytecode_map;

    common::DoubleByte current_bytecode = 0;

    // ReSharper disable once CppUseStructuredBinding
    auto [instructions, labels_addresses, errors] = ParseSource(source);

    if (!errors.empty()) {
        throw std::runtime_error(std::accumulate(errors.begin(), errors.end(), std::string(),
                                                 [](const std::string& a, const std::string& b) {
                                                     return a.empty() ? b : a + "\n" + b;
                                                 }));
    }

    // ReSharper disable once CppUseStructuredBinding
    for (auto& instruction : instructions) {
        if (instruction.using_label_name) {
            if (labels_addresses.contains(*instruction.using_label_name)) {
                instruction.argument = labels_addresses[*instruction.using_label_name];
            } else {
                throw Exception<std::runtime_error>(std::format("Label {} does not exist", *instruction.label_name),
                                                    instruction.line_number);
            }
        }

        common::Byte instruction_byte_code = common::InstructionByte(instruction.opcode,
                                                                     instruction.type_modifier,
                                                                     instruction.argument_modifier);

        byte_code.push_back(instruction_byte_code);

        for (auto& i : instruction.argument) {
            byte_code.push_back(i);
        }

        source_to_bytecode_map[instruction.line_number] = current_bytecode;
        current_bytecode++;
    }

    return std::make_pair(byte_code, source_to_bytecode_map);
}

std::vector<std::string> Assembler::TestSource(const std::string& source) {
    auto [instructions, labels_addresses, errors] = ParseSource(source);
    return errors;
}

std::tuple<std::vector<Instruction>, std::unordered_map<std::string, uint32_t>, std::vector<std::string>>
Assembler::ParseSource(
    const std::string& source) {
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, uint32_t> labels_addresses;
    std::vector<std::string> errors;

    line_number_ = 0;

    std::istringstream stream(source);
    std::string line = GetLine(stream);

    uint32_t address = 0;

    while (!line.empty()) {
        Instruction instruction;
        try {
            instruction = GetInstruction(line);
        } catch (const std::exception& e) {
            errors.emplace_back(e.what());
        }

        if (instruction.label_name) {
            labels_addresses[*instruction.label_name] = address;
        }

        instructions.push_back(instruction);

        line = GetLine(stream);
        address++;
    }

    instructions.shrink_to_fit();

    return std::make_tuple(instructions, labels_addresses, errors);
}

std::string Assembler::GetLine(std::istringstream& stream) {
    std::string line;

    while (std::getline(stream, line)) {
        line_number_++;
        line = RemoveComment(line);
        line = Trim(line);
        if (!line.empty())
            break;
    }

    return line;
}

template <typename T>
T Assembler::Exception(const std::string& message) {
    return Exception<T>(message, line_number_);
}

template <typename T>
T Assembler::Exception(const std::string& message, unsigned int line_number) {
    return T(std::format("Line {}: {}", line_number, message));
}

Instruction Assembler::GetInstruction(const std::string& line) {
    Instruction result;
    result.line_number = line_number_;

    std::istringstream stream(line);
    std::string token;

    if (stream >> token) {
        if (token.ends_with(":")) {
            result.label_name = token.substr(0, token.length() - 1);

            if (!IsValidLabelName(*result.label_name)) {
                throw Exception<std::runtime_error>(
                    std::format("Invalid label name: {}", *result.label_name));
            }
        } else {
            stream.seekg(-static_cast<int>(token.size()),
                         std::ios_base::cur); // Возвращаем токен обратно в поток
        }
    }

    if (stream >> token) {
        if (opcode_map_.contains(ToUpper(token))) {
            result.opcode = opcode_map_[ToUpper(token)];
        } else if (!result.label_name) {
            // При указании метки доступен синтаксис без команды, поэтому может не
            // быть команды. Однако если метка не указана и указана неизвестная
            // команда, то это ошибка
            throw Exception<std::runtime_error>(std::format("Unknown command: {}", token));
        } else {
            stream.seekg(-static_cast<int>(token.size()), std::ios_base::cur);
        }
    }

    // Чтение модификатора типа
    if (stream >> token) {
        if (type_modifier_map_.contains(ToUpper(token))) {
            result.type_modifier = type_modifier_map_[ToUpper(token)];
        } else {
            // TODO: Необходимо определять тип, если он явно не задан и указано число
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto& properties = common::OPCODE_PROPERTIES.at(result.opcode);
            if (properties.allowed_type_modifiers.contains(common::TypeModifier::SW)) {
                result.type_modifier = common::TypeModifier::SW;
            } else if (properties.allowed_type_modifiers.contains(common::TypeModifier::C)) {
                result.type_modifier = common::TypeModifier::C;
            } else {
                result.type_modifier = common::TypeModifier::C;
            }

            // Если токен не является модификатором аргумента, это может быть
            // модификатор типа или аргумент
            stream.seekg(-static_cast<int>(token.size()), std::ios_base::cur); // Возвращаем токен обратно в поток
        }
    }

    // Чтение модификатора аргумента
    if (stream >> token) {
        if (arg_modifier_map_.contains(ToUpper(token))) {
            if (!result.label_name) {
                result.argument_modifier = arg_modifier_map_[ToUpper(token)];
            } else {
                throw Exception<std::runtime_error>(
                    std::format("Arg modifier nedostupen dlya metok: {}", token));
            }
        } else {
            // Если токен не является модификатором аргумента, это может быть
            // модификатор типа или аргумент
            stream.seekg(-static_cast<int>(token.size()),
                         std::ios_base::cur); // Возвращаем токен обратно в поток
        }
    }

    /*
    // Проверка допустимости модификаторов для опкода
    const OpCodeProperties &props = opcodeProperties[result.opcode];
    if (props.allowedTypeModifiers.find(result.typeModifier) ==
    props.allowedTypeModifiers.end()) { std::string allowedModifiers = "Allowed
    type modifiers for " + std::to_string(static_cast<int>(result.opcode)) +
                                       ": ";
        for (const auto &mod: props.allowedTypeModifiers) {
            allowedModifiers += std::to_string(static_cast<int>(mod)) + " ";
        }
        throw std::runtime_error(std::format("Line {}: Invalid type modifier for
    instruction. {}", lineNumber, allowedModifiers));
    }
    if (props.allowedArgModifiers.find(result.argModifier) ==
    props.allowedArgModifiers.end()) { std::string allowedModifiers = "Allowed
    argument modifiers for " + std::to_string( static_cast<int>(result.opcode)) +
    ": "; for (const auto &mod: props.allowedArgModifiers) { allowedModifiers +=
    std::to_string(static_cast<int>(mod)) + " ";
        }
        throw std::runtime_error(std::format("Line {}: Invalid argument modifier
    for instruction. {}", lineNumber, allowedModifiers));
    }*/

    // Чтение аргумента
    if (stream >> token) {
        if (IsValidLabelName(token)) {
            result.using_label_name = token;
        } else {
            try {
                result.argument = ParseNumber(token);
            } catch ([[maybe_unused]] const std::exception& e) {
                throw Exception<std::invalid_argument>(
                    std::format("Invalid number format: {}", token));
            }
        }
    }

    if (stream >> token) {
        throw Exception<std::invalid_argument>("Invalid command format");
    }

    return result;
}

bool Assembler::IsArgumentModifier(const std::string& token) const {
    return arg_modifier_map_.contains(ToUpper(token));
}

bool Assembler::IsTypeModifier(const std::string& token) const {
    return type_modifier_map_.contains(ToUpper(token));
}

void Assembler::ValidateStringNumber(const std::string& str) {
    if (str.size() >= 2 && str.substr(0, 2) == "0b") {
        if (const std::regex binary_regex("^0b[01]+$"); !std::regex_match(str, binary_regex)) {
            throw Exception<std::invalid_argument>(
                "Invalid binary string: only '0' and '1' are allowed after '0b'");
        }
    }
    // Проверка на шестнадцатеричную строку (начинается с "0x")
    else if (str.size() >= 2 && str.substr(0, 2) == "0x") {
        if (const std::regex hex_regex("^0x[0-9A-Fa-f]+$"); !std::regex_match(str, hex_regex)) {
            throw Exception<std::invalid_argument>(
                "Invalid hex string: only digits (0-9, A-F) are allowed after '0x'");
        }
    }
    // Проверка на число с плавающей точкой (содержит ".")
    else if (str.find('.') != std::string::npos) {
        if (const std::regex float_regex("^-?[0-9]+\\.[0-9]+$"); !std::regex_match(str, float_regex)) {
            throw Exception<std::invalid_argument>(
                "Invalid float string: only digits (0-9), one '.', and optional '-' "
                "at the start are allowed");
        }
    }
    // Проверка на целое число (без ".")
    else {
        if (const std::regex int_regex("^-?[0-9]+$"); !std::regex_match(str, int_regex)) {
            throw Exception<std::invalid_argument>(
                "Invalid integer string: only digits (0-9) and optional '-' at the "
                "start are allowed");
        }
    }
}

std::string Assembler::Trim(const std::string& str) {
    const auto left = str.find_first_not_of(' ');
    const auto right = str.find_last_not_of(' ');

    if (left == std::string::npos) {
        return "";
    }

    std::string result = str.substr(left, right - left + 1);

    const std::string from = "  ";
    const std::string to = " ";
    size_t start_pos = 0;
    while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
        result.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return result;
}

bool Assembler::IsValidLabelName(const std::string& name) {
    if (name.empty()) {
        return false;
    }

    if (name[0] != '_' && !std::isalpha(name[0])) {
        return false;
    }

    if (std::ranges::all_of(
        name, [](const char c) {
            return c != '_' && !std::isalnum(c);
        })) {
        return false;
    }

    return true;
}

bool Assembler::IsNumberValidForType(const common::Bytes bytes, const common::TypeModifier type_modifier) {
    // TODO: необходимо переписать
    if (type_modifier == common::TypeModifier::C) {
        const auto value = static_cast<char>(bytes);
        return value >= -128 && value <= 127;
    }
    if (type_modifier == common::TypeModifier::W) {
        const auto value = static_cast<int32_t>(bytes);
        return value >= INT32_MIN && value <= INT32_MAX;
    }
    if (type_modifier == common::TypeModifier::SW) {
        const auto value = static_cast<uint32_t>(bytes);
        return value <= UINT32_MAX;
    }
    if (type_modifier == common::TypeModifier::R) {
        auto value = static_cast<float>(bytes);
        return true;
    }

    return false;
}

common::Bytes Assembler::ParseNumber(const std::string& str) {
    ValidateStringNumber(str);

    common::Bytes result;

    if (str.starts_with("0b")) {
        std::string binary_string = str.substr(2); // Убираем "0b"
        if (binary_string.size() > common::ARGUMENT_SIZE * 8) {
            throw Exception<std::invalid_argument>(
                std::format("Binary string {} is too long", str));
        }

        // Дополняем строку нулями слева до 32 бит (4 байта)
        binary_string.insert(binary_string.begin(),
                             common::ARGUMENT_SIZE * 8 - binary_string.size(), '0');

        const std::bitset<common::ARGUMENT_SIZE * 8> bits(binary_string);

        const uint32_t num = bits.to_ulong();

        for (size_t i = 0; i < common::ARGUMENT_SIZE; ++i) {
            result[i] = static_cast<uint8_t>(num >> (8 * i) & 0xFF);
        }
    } else if (str.starts_with("0x")) {
        // Обработка шестнадцатеричной строки
        std::string hex_string = str.substr(2); // Убираем "0x"
        if (hex_string.size() > common::ARGUMENT_SIZE * 2) {
            throw Exception<std::invalid_argument>(
                std::format("Hex string {} is too long", str));
        }

        // Дополняем строку нулями слева до 8 символов (4 байта)
        hex_string.insert(hex_string.begin(), common::ARGUMENT_SIZE * 2 - hex_string.size(),
                          '0');

        // Преобразуем шестнадцатеричную строку в число
        uint32_t num;
        std::stringstream ss;
        ss << std::hex << hex_string;
        ss >> num;

        for (size_t i = 0; i < common::ARGUMENT_SIZE; ++i) {
            result[i] = static_cast<uint8_t>(num >> (8 * i) & 0xFF);
        }
    } else if (str.find('.') != std::string::npos) {
        // Число с плавающей точкой
        result = std::stof(str);
    } else {
        // Целое число
        result = std::stoul(str);
    }

    return result;
}

std::string Assembler::RemoveComment(const std::string& line) {
    if (const size_t pos = line.find(';'); pos != std::string::npos) {
        return line.substr(0, pos);
    }

    return line;
}

std::string Assembler::ToUpper(std::string str) {
    std::ranges::transform(str, str.begin(), toupper);
    return str;
}
