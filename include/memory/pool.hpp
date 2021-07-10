#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <utility>
#include <cstdio>

class MemoryPool
{
public:
    explicit MemoryPool(size_t size) noexcept : memory(nullptr), current_pointer(nullptr), block_size(size)
    {
        allocate_memory(block_size);
    }

    ~MemoryPool()
    {
        release_memory();
    }

    template <typename T, typename... Args>
    T* allocate(Args&&... args) noexcept
    {
        if (need_realloc())
        {
            resize_memory(block_size * 2);
        }

        current_pointer = static_cast<char*>(current_pointer) + sizeof(T);

        new (current_pointer) T(std::forward<Args>(args)...);

        return static_cast<T*>(current_pointer);
    }

private:
    void allocate_memory(size_t bytes) noexcept
    {
        memory = malloc(bytes);
        current_pointer = memory;
        block_size = bytes;
    }

    void release_memory() noexcept
    {

        free(memory);
        memory = nullptr;
        current_pointer = nullptr;
        block_size = 0;
    }

    /// Fails...
    void resize_memory(size_t new_bytes) noexcept
    {
        memory = realloc(memory, new_bytes);
    }

    static size_t pointer_difference(void* begin, void* end) noexcept
    {
        return static_cast<char*>(end) - static_cast<char*>(begin);
    }

    bool need_realloc() const noexcept
    {
        return pointer_difference(memory, current_pointer) >= block_size;
    }

    void* memory;
    void* current_pointer;
    size_t block_size;
};

#endif // MEMORY_POOL_HPP
