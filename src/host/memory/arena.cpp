#include "arena.h"
#include <cassert>
#include <cstring>
#ifndef WIN32
#include <sys/mman.h>
#endif

namespace pound::host::memory
{
arena_t arena_init(size_t capacity)
{

    // TODO(GloriousTaco:memory): Replace malloc with a windows memory mapping API.
#ifdef WIN32
    auto data = static_cast<uint8_t*>(malloc(sizeof(size_t) * capacity));
#else
    void* data = ::mmap(nullptr, capacity, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (data == MAP_FAILED)
    {
        return {0, 0, nullptr};  // Return invalid arena on failure
    }
#endif

    (void)std::memset(data, POISON_PATTERN, capacity);
    memory::arena_t arena = {
        .capacity = capacity,
        .size = 0,
        .data = data,
    };
    return arena;
}
// new more memsafe code (ownedbywuigi) (i give up on windows compatibility for now, will stick to the old unsafe code)

const void* arena_allocate(memory::arena_t* arena, const std::size_t size)
{
    assert(arena != nullptr);
    assert(arena->size + size <= arena->capacity);
    const void* const data = static_cast<uint8_t*>(arena->data) + arena->size;
    arena->size += size;
    return data;
}
void arena_reset(memory::arena_t* arena)
{
    assert(nullptr != arena);
    assert(nullptr != arena->data);
    arena->size = 0;
    (void)std::memset(arena->data, POISON_PATTERN, arena->capacity);
}
void arena_free(memory::arena_t* arena)
{
    assert(arena != nullptr);
    arena->capacity = 0;
    arena->size = 0;
    // TODO(GloriousTaco:memory): Replace free with a memory safe alternative.
    free(arena->data);
}
}  // namespace pound::host::memory
