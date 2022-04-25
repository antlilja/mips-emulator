#pragma once
#include "mips-emulator/memory.hpp"

#include <vector>

namespace mips_emulator {

    template <typename MMIOHandler = NullMMIO>
    class RuntimeStaticMemory
        : public Memory<RuntimeStaticMemory<MMIOHandler>, MMIOHandler> {
    public:
        RuntimeStaticMemory(const uint32_t size, const uint32_t offset = 0,
                            std::shared_ptr<MMIOHandler> mmio = nullptr)
            : Memory<RuntimeStaticMemory<MMIOHandler>, MMIOHandler>(
                  offset, std::move(mmio)),
              memory(size) {}

        RuntimeStaticMemory(std::vector<uint8_t> mem, const uint32_t offset = 0,
                            std::shared_ptr<MMIOHandler> mmio = nullptr)
            : Memory<RuntimeStaticMemory<MMIOHandler>, MMIOHandler>(
                  offset, std::move(mmio)),
              memory(std::move(mem)) {}

        uint8_t* get_memory() { return &memory[0]; }
        uint32_t get_size() const { return memory.size(); }

    private:
        std::vector<uint8_t> memory;
    };
} // namespace mips_emulator
