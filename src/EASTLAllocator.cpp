#include <EASTL/allocator.h>
#include <new>
#include <cstdio>
#include <cstdarg>

void* operator new[](size_t size, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
    return ::operator new(size);
}

void* operator new[](size_t size, size_t alignment, size_t /*alignmentOffset*/, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
    return ::operator new(size, std::align_val_t(alignment));
}

void operator delete[](void* p, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/) EA_NOEXCEPT
{
    ::operator delete(p);
}

void operator delete[](void* p, size_t alignment, size_t /*alignmentOffset*/, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/) EA_NOEXCEPT
{
    ::operator delete(p, std::align_val_t(alignment));
}

namespace EA::StdC
{
    int Vsnprintf(char* __restrict pDestination, unsigned __int64 n,
        const char* __restrict pFormat, char* arguments)
    {
        return ::vsnprintf(pDestination, static_cast<size_t>(n), pFormat, (va_list)arguments);
    }
}