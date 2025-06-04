#ifndef BYTES_HPP
#define BYTES_HPP

#include <array>
#include <cstdint>
#include <cstring>
#include <format>

template <size_t N>
class Bytes {
    std::array<uint8_t, N> data_;

public:
    Bytes() = default;

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

    template <typename T>
    explicit operator T() const {
        T value = 0;
        std::memcpy(&value, data_.data(), std::min(sizeof(T), N));
        return value;
    }

    std::string ToHexString() {
        return std::format("{:08x}", static_cast<unsigned int>(*this));
    }

    std::string ToBinString() {
        return std::format("{:08b}", static_cast<unsigned int>(*this));
    }

private:
    template <typename T>
    void Store(T value) {
        data_.fill(0);
        std::memcpy(data_.data(), &value, std::min(sizeof(T), N));
    }
};

#endif
