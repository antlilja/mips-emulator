#pragma once
#include "mips-emulator/memory.hpp"

namespace mips_emulator {

    template <typename MMIOHandler = NullMMIO>
    class RuntimeStaticMemory
        : public Memory<RuntimeStaticMemory<MMIOHandler>, MMIOHandler> {
    public:
        RuntimeStaticMemory(const uint32_t size,
                            std::shared_ptr<MMIOHandler> mmio = nullptr)
            : Memory<RuntimeStaticMemory<MMIOHandler>, MMIOHandler>(
                  std::move(mmio)),
              memory(new uint8_t[size]), size(size) {}

        uint8_t* get_memory() { return memory; }
        uint32_t get_size() const { return size; }

    private:
        uint8_t* memory;
        uint32_t size;
    };
} // namespace mips_emulator
