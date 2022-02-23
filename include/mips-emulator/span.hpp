#pragma once
#include <cstdint>
#include <cstddef>

namespace mips_emulator {
    template <typename T>
    class Span {
    public:
        Span(T* data, std::size_t size) : data(data), size(size) {}

        T* get_data() noexcept { data; }
        const T* get_data() const noexcept { data; }

        std::size_t get_size() const noexcept { return size; }

        T& operator[](std::size_t i) { return data[i]; }
        const T& operator[](std::size_t i) const { return data[i]; }

    private:
        T* data;
        std::size_t size;
    };
} // namespace mips_emulator
