#include "include/gdt.h"
#include "include/global_defs.h"
#include <stdint.h>

__attribute__((aligned(0x1000))) struct GDT_Entry null_seg = {

    .base = 0,
    .limit = 0x00000000,
    .access_byte = 0x00,
    .flags = 0x00

};

struct GDT_Entry Kernel_cs = {

    .base = 0,
    .limit = 0xFFFFF,
    .access_byte = GDTAccessKernelCode,
    .flags = 0xA

};

struct GDT_Entry Kernel_ds = {

    .base = 0,
    .limit = 0xFFFFF,
    .access_byte = GDTAccessKernelData,
    .flags = 0xC

};

struct GDT_Entry User_cs = {

    .base = 0,
    .limit = 0xFFFFF,
    .access_byte = GDTAccessUserCode,
    .flags = 0xA

};

struct GDT_Entry User_ds = {

    .base = 0,
    .limit = 0xFFFFF,
    .access_byte = GDTAccessUserData,
    .flags = 0xC

};

void encodeGdtEntry(uint8_t *target, struct GDT_Entry source)
{
    // Check the limit to make sure that it can be encoded
    if (source.limit > 0xFFFFF)
    {
        kerror("GDT cannot encode limits larger than 0xFFFFF");
    }

    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] = (source.limit >> 16) & 0x0F;

    // Encode the base
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;

    // Encode the access byte
    target[5] = source.access_byte;

    // Encode the flags
    target[6] |= (source.flags << 4);
}