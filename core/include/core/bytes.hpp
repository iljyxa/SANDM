#ifndef BYTES_HPP
#define BYTES_HPP

#include <array>
// ReSharper disable once CppUnusedIncludeDirective
#include <cstdint>
#include <cstring>
#include <format>

/**
 * @class Bytes
 *
 * @brief Класс для представления чисел в виде массив байтов.
 *
 */
template <size_t N>
class Bytes {
    std::array<uint8_t, N> data_;

public:
    Bytes() = default;

    /**
     * Конструктор, инициализирующий объект класса Bytes на основе переданного значения.
     *
     * @tparam T Тип значения, используемого для инициализации объекта Bytes.
     * @param value Значение, использующееся для инициализации объекта.
     */
    template <typename T>
    explicit Bytes(T value) {
        Store(value);
    }

    template <typename T>
    Bytes& operator=(T value) {
        Store(value);
        return *this;
    }

    uint8_t operator[](size_t index) const {
        return data_[index];
    }

    uint8_t& operator[](size_t index) {
        return data_[index];
    }

    [[nodiscard]] auto begin() const {
        return data_.begin();
    }

    [[nodiscard]] auto end() const {
        return data_.end();
    }

    /**
     * Оператор конвертации типа Bytes в тип T.
     *
     * @return Значение типа T
     */
    template <typename T>
    explicit operator T() const {
        T value = 0;
        std::memcpy(&value, data_.data(), std::min(sizeof(T), N));
        return value;
    }

    /**
     * Преобразует объект класса Bytes в строку, представляющую значением в шестнадцатеричном формате.
     *
     * @return Строка, содержащая шестнадцатеричное представление объекта Bytes.
     */
    std::string ToHexString() {
        return std::format("{:08x}", static_cast<unsigned int>(*this));
    }

    /**
     * Преобразует объект класса Bytes в строку, представляющую значение в двоичном формате.
     *
     * @return Строка, содержащая двоичное представление объекта Bytes.
     */
    std::string ToBinString() {
        return std::format("{:08b}", static_cast<unsigned int>(*this));
    }

private:
    /**
     * Сохраняет значение заданного типа в объект класса. При этом оставшиеся
     * байты заполняются нулями, если размер типа меньше, чем размер внутреннего массива.
     *
     * @tparam T Тип значения, передаваемого для сохранения.
     * @param value Значение, которое будет сохранено в объекте Bytes.
     */
    template <typename T>
    void Store(T value) {
        data_.fill(0);
        std::memcpy(data_.data(), &value, std::min(sizeof(T), N));
    }
};

#endif
