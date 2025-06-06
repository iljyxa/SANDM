#include "../include/core/assembler.hpp"

Assembler::Assembler() :
    line_number_(0) {
    for (const auto& [opcode, properties] : snm::OPCODE_PROPERTIES) {
        opcode_map_.insert({properties.name, opcode});
    }

    type_modifier_map_ = {
        {"C", snm::TypeModifier::C},
        {"W", snm::TypeModifier::W},
        {"SW", snm::TypeModifier::SW},
        {"R", snm::TypeModifier::R}
    };

    arg_modifier_map_ = {
        {"&", snm::ArgModifier::REF},
        {"&&", snm::ArgModifier::REF_REF}
    };
}

snm::ByteCode Assembler::Compile(const std::string& source) {
    return CompileInternal(source).first;
}

std::pair<snm::ByteCode, snm::SourceToBytecodeMap> Assembler::CompileWithDebugInfo(const std::string& source) {
    return CompileInternal(source);
}

std::pair<snm::ByteCode, snm::SourceToBytecodeMap> Assembler::CompileInternal(const std::string& source) {
    auto [instructions, labels, errors] = ParseSource(source);
    if (!errors.empty()) {
        throw std::runtime_error(std::accumulate(errors.begin(), errors.end(), std::string(),
                                                 [](const std::string& a, const std::string& b) {
                                                     return a.empty() ? b : a + "\n" + b;
                                                 }));
    }

    snm::ByteCode byte_code;
    snm::SourceToBytecodeMap map;
    snm::Address current_addr = 0;

    for (auto& instr : instructions) {
        if (instr.using_label_name) {
            auto label = ToUpper(*instr.using_label_name);
            if (!labels.contains(label)) {
                throw Exception<std::runtime_error>(std::format("Label {} does not exist", *instr.using_label_name),
                                                    instr.line_number);
            }
            instr.argument = labels[label];
        }

        byte_code.push_back(snm::InstructionByte(instr.opcode, instr.type_modifier, instr.argument_modifier));
        for (auto& byte : instr.argument ? *instr.argument : snm::Bytes(0)) byte_code.push_back(byte);

        map[instr.line_number] = current_addr++;
    }

    return {byte_code, map};
}

std::vector<std::string> Assembler::TestSource(const std::string& source) {
    auto [_, __, errors] = ParseSource(source);
    return errors;
}

std::tuple<std::vector<Instruction>, std::unordered_map<std::string, snm::Address>, std::vector<std::string>>
Assembler::ParseSource(const std::string& source) {
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, snm::Address> labels;
    std::vector<std::string> errors;
    line_number_ = 0;
    snm::Address address = 0;

    std::istringstream stream(source);
    for (std::string line; !(line = GetLine(stream)).empty();) {
        if (address == std::numeric_limits<snm::Address>::max()) {
            errors.emplace_back("Too many instructions: address overflow");
            break;
        }

        try {
            Instruction instr = GetInstruction(line);
            if (instr.label_name) {
                auto name = ToUpper(*instr.label_name);
                if (labels.contains(name)) {
                    errors.push_back(std::format("Line {}: Label {} already exists", instr.line_number,
                                                 *instr.label_name));
                } else {
                    labels[name] = address;
                }
            }
            instructions.push_back(std::move(instr));
        } catch (const std::exception& e) {
            errors.emplace_back(e.what());
        }

        ++address;
    }

    return {instructions, labels, errors};
}

std::string Assembler::GetLine(std::istringstream& stream) {
    std::string line;
    while (std::getline(stream, line)) {
        ++line_number_;
        line = Trim(RemoveComment(line));
        if (!line.empty()) break;
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
    Instruction instr;
    instr.line_number = line_number_;

    std::istringstream stream(line);
    std::string token;

    // Разбор метки
    if (stream >> token && token.ends_with(':')) {
        instr.label_name = token.substr(0, token.size() - 1);
        if (!IsValidLabelName(*instr.label_name)) {
            throw Exception<std::invalid_argument>(std::format("Invalid label name: {}", *instr.label_name));
        }
    } else {
        stream.seekg(-static_cast<int>(token.size()), std::ios_base::cur);
    }

    // Опкод
    if (stream >> token && opcode_map_.contains(ToUpper(token))) {
        instr.opcode = opcode_map_[ToUpper(token)];
    } else {
        stream.seekg(-static_cast<int>(token.size()), std::ios_base::cur);
    }

    // Модификаторы типа и аргумента
    ParseModifiers(stream, instr);

    // Аргумент
    ParseArgument(stream, instr);

    // Ошибка при наличии лишних токенов
    if (stream >> token) {
        throw Exception<std::runtime_error>(std::format("Invalid instruction: {}", line));
    }

    if (snm::OPCODE_PROPERTIES.at(instr.opcode).is_argument_required
        && !instr.argument
        && !instr.using_label_name) {
        throw Exception<std::runtime_error>(std::format("Argument is required for instruction: {}", line));
    }

    return instr;
}

void Assembler::ParseModifiers(std::istringstream& stream, Instruction& instr) {
    std::string token;

    if (stream >> token) {
        const auto upper = ToUpper(token);
        if (type_modifier_map_.contains(upper)) {
            const auto mod = type_modifier_map_[upper];
            if (!snm::OPCODE_PROPERTIES.at(instr.opcode).allowed_type_modifiers.contains(mod)) {
                throw Exception<std::domain_error>(std::format("Modifier {} cannot be used", token));
            }
            instr.type_modifier = mod;
        } else {
            // Используем тип по умолчанию
            const auto& props = snm::OPCODE_PROPERTIES.at(instr.opcode);
            instr.type_modifier = props.allowed_type_modifiers.contains(snm::TypeModifier::SW)
                ? snm::TypeModifier::SW
                : snm::TypeModifier::W;
            stream.seekg(-static_cast<int>(token.size()), std::ios_base::cur);
        }
    }

    if (stream >> token) {
        const auto upper = ToUpper(token);
        if (arg_modifier_map_.contains(upper)) {
            const auto mod = arg_modifier_map_[upper];
            if (!snm::OPCODE_PROPERTIES.at(instr.opcode).allowed_arg_modifiers.contains(mod)) {
                throw Exception<std::domain_error>(std::format("Modifier {} cannot be used", token));
            }
            instr.argument_modifier = mod;
        } else {
            stream.seekg(-static_cast<int>(token.size()), std::ios_base::cur);
        }
    }
}

void Assembler::ParseArgument(std::istringstream& stream, Instruction& instr) {
    if (!snm::OPCODE_PROPERTIES.at(instr.opcode).is_argument_available) return;

    std::string token;
    if (!(stream >> token)) return;

    // Обработка символа
    if (token == "'" && stream.peek() != EOF) {
        char next;
        token.clear();
        token += "'";
        stream.get(next);
        token += next;
        if (stream.get(next)) token += next;
    }

    if (IsValidLabelName(token)) {
        instr.using_label_name = token;
    } else if (IsValidChar(token)) {
        instr.argument = static_cast<int>(token[1]);
    } else {
        try {
            instr.argument = ParseNumber(token, instr.type_modifier);
        } catch (...) {
            stream.seekg(-static_cast<int>(token.size()), std::ios_base::cur);
        }
    }
}

bool Assembler::IsArgumentModifier(std::string& token) const {
    return arg_modifier_map_.contains(ToUpper(token));
}

bool Assembler::IsTypeModifier(std::string& token) const {
    return type_modifier_map_.contains(ToUpper(token));
}

void Assembler::ValidateStringNumber(const std::string& str) {
    if (str.size() >= 2 && str.substr(0, 2) == "0b") {
        if (const std::regex binary_regex("^0b[01]+$"); !std::regex_match(str, binary_regex)) {
            throw Exception<std::invalid_argument>(
                std::format("Invalid binary string {}: only '0' and '1' are allowed after '0b'", str));
        }
    }
    // Проверка на шестнадцатеричную строку (начинается с "0x")
    else if (str.size() >= 2 && str.substr(0, 2) == "0x") {
        if (const std::regex hex_regex("^0x[0-9A-Fa-f]+$"); !std::regex_match(str, hex_regex)) {
            throw Exception<std::invalid_argument>(
                std::format("Invalid hex string {}: only digits (0-9, A-F) are allowed after '0x'", str));
        }
    }
    // Проверка на число с плавающей точкой (содержит ".")
    else if (str.find('.') != std::string::npos) {
        if (const std::regex float_regex("^-?[0-9]+\\.[0-9]+$"); !std::regex_match(str, float_regex)) {
            throw Exception<std::invalid_argument>(
                std::format("Invalid float string {}: only digits (0-9), one '.', and optional '-' "
                    "at the start are allowed", str));
        }
    }
    // Проверка на целое число (без ".")
    else {
        if (const std::regex int_regex("^-?[0-9]+$"); !std::regex_match(str, int_regex)) {
            throw Exception<std::invalid_argument>(
                std::format("Invalid integer string {}: only digits (0-9) and optional '-' at the "
                "start are allowed", str));
        }
    }
}

std::string Assembler::Trim(const std::string& str) {

    const auto left = str.find_first_not_of(" \t");
    const auto right = str.find_last_not_of(" \t");

    if (left == std::string::npos) {
        return "";
    }

    std::string result = str.substr(left, right - left + 1);

    const std::string whitespace = " \t";
    bool in_whitespace = false;

    for (size_t i = 0; i < result.length(); ) {
        if (whitespace.find(result[i]) != std::string::npos) {
            if (in_whitespace) {
                result.erase(i, 1);
            } else {
                result[i] = ' ';
                in_whitespace = true;
                ++i;
            }
        } else {
            in_whitespace = false;
            ++i;
        }
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

    if (std::ranges::any_of(
        name, [](const char c) {
            return c != '_' && !std::isalnum(c);
        })) {
        return false;
    }

    return true;
}

bool Assembler::IsValidChar(const std::string& token) {
    if (token.size() != 3
        || token[0] != '\'' || token[2] != '\'') {

        return false;
    }

    return true;
}

bool Assembler::IsNumberValidForType(const snm::Bytes bytes, const snm::TypeModifier type_modifier) {
    switch (type_modifier) {
    case snm::TypeModifier::C:
    {
        const auto value = static_cast<uint64_t>(bytes);
        return value >= std::numeric_limits<snm::Byte>::min() && value <= std::numeric_limits<snm::Byte>::max();
    }
    case snm::TypeModifier::W:
    {
        const auto value = static_cast<uint64_t>(bytes);
        return value >= std::numeric_limits<snm::Word>::min() && value <= std::numeric_limits<snm::Word>::max();
    }
    case snm::TypeModifier::SW:
    {
        const auto value = static_cast<int64_t>(bytes);
        return value >= std::numeric_limits<snm::SignedWord>::min() && value <= std::numeric_limits<
            snm::SignedWord>::max();
    }
    case snm::TypeModifier::R: {
        const auto value = static_cast<double>(bytes);
        return value >= std::numeric_limits<snm::Real>::min() && value <= std::numeric_limits<snm::Real>::max();
    }
    default:
        return false;
    }
}

snm::Bytes Assembler::ParseNumber(const std::string& str, const snm::TypeModifier& type_modifier) {
    ValidateStringNumber(str);

    snm::Bytes result;

    if (str.starts_with("0b")) {
        std::string binary_string = str.substr(2); // Убираем "0b"
        if (binary_string.size() > snm::ARGUMENT_SIZE * 8) {
            throw Exception<std::out_of_range>(
                std::format("Binary string {} is too long (max {} bits)", str, std::to_string(snm::ARGUMENT_SIZE * 8)));
        }

        // Дополняем строку нулями слева
        binary_string.insert(binary_string.begin(), snm::ARGUMENT_SIZE * 8 - binary_string.size(), '0');
        const std::bitset<snm::ARGUMENT_SIZE * 8> bits(binary_string);
        const uint32_t num = bits.to_ulong();

        for (size_t i = 0; i < snm::ARGUMENT_SIZE; ++i) {
            result[i] = static_cast<uint8_t>(num >> (8 * i) & 0xFF);
        }
    } else if (str.starts_with("0x")) {
        // Обработка шестнадцатеричной строки
        std::string hex_string = str.substr(2); // Убираем "0x"
        if (hex_string.size() > snm::ARGUMENT_SIZE * 2) {
            throw Exception<std::out_of_range>(
                std::format("Hex string {} is too long (max {} nibbles)", str, std::to_string(snm::ARGUMENT_SIZE * 2)));
        }

        // Дополняем строку нулями слева
        hex_string.insert(hex_string.begin(), snm::ARGUMENT_SIZE * 2 - hex_string.size(),
                          '0');

        // Преобразуем шестнадцатеричную строку в число
        uint32_t num;
        std::stringstream ss;
        ss << std::hex << hex_string;
        ss >> num;

        for (size_t i = 0; i < snm::ARGUMENT_SIZE; ++i) {
            result[i] = static_cast<uint8_t>(num >> (8 * i) & 0xFF);
        }
    } else if (type_modifier == snm::TypeModifier::R) {
        // Число с плавающей запятой
        result = std::stof(str);
    } else {
        // Целое число. В данном случае не имеет значения, какой тип аргумента.
        result = std::stoul(str);
    }

    return result;
}

std::string Assembler::RemoveComment(const std::string& line) {
    if (const size_t pos = line.find("//"); pos != std::string::npos) {
        return line.substr(0, pos);
    }

    return line;
}

std::string Assembler::ToUpper(std::string& str) {
    std::ranges::transform(str, str.begin(), toupper);
    return str;
}

