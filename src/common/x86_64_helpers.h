#include <stdint.h>

#define MASK_XMM 0x2
#define MASK_YMM 0x4
#define MASK_MASKREG 0x20
#define MASK_ZMM0_15 0x40
#define MASK_ZMM16_31 0x80

typedef struct
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} cpuid_out;

static inline uint32_t xgetbv_eax(uint32_t xcr) {
    uint32_t eax;
#if defined(__GNUC__) || defined(__clang__)
    __asm__ ( ".byte 0x0f, 0x01, 0xd0" : "=a"(eax) : "c"(xcr));
#else
    // This should work for MSVC.
    // Other compilers are not supported.
    uint32_t a;
    __asm {
        mov ecx, xcr
        _emit 0x0f
        _emit 0x01
        _emit 0xd0 ; // xgetbv
        mov a, eax
    }
    eax = a;
#endif
    return eax;
}

static unsigned int has_mask(const uint32_t value, const uint32_t mask)
{
  return (value & mask) == mask;
}

static inline unsigned int is_bit_set(const uint32_t val, const unsigned int bit_pos)
{
    return val & (1 << bit_pos) ? 1 : 0;
}

static inline void cpuid(cpuid_out* out, const uint32_t eax_leaf)
{
    const uint32_t ecx_leaf = 0;
    uint32_t output[4] = { 0 };

#if defined(__GNUC__) || defined(__clang__)
    uint32_t eax, ebx, ecx, edx;
    __asm__("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(eax_leaf), "c"(ecx_leaf) : );
    output[0] = eax;
    output[1] = ebx;
    output[2] = ecx;
    output[3] = edx;
#else
    // This should work for MSVC.
    // Other compilers are not supported.
    __asm {
        mov eax, eax_leaf
        mov ecx, ecx_leaf
        cpuid;
        mov esi, output
        mov[esi], eax
        mov[esi + 4], ebx
        mov[esi + 8], ecx
        mov[esi + 12], edx
    }
#endif
    out->eax = output[0];
    out->ebx = output[1];
    out->ecx = output[2];
    out->edx = output[3];
}
