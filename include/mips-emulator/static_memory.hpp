#pragma once
#include "mips-emulator/memory.hpp"

namespace mips_emulator {
    template <uint32_t SIZE, typename MMIOHandler = NullMMIO>
    class StaticMemory
        : public Memory<StaticMemory<SIZE, MMIOHandler>, MMIOHandler> {
    public:
        static_assert(SIZE != 0, "SIZE of StaticMemory can't be zero");

        StaticMemory(std::shared_ptr<MMIOHandler> mmio_handler = nullptr)
            : Memory<StaticMemory<SIZE, MMIOHandler>, MMIOHandler>(
                  mmio_handler) {}

        uint8_t* get_memory() { return &memory[0]; }
        uint32_t get_size() const { return SIZE; }

    private:
        uint8_t memory[SIZE];
    };
} // namespace mips_emulator
