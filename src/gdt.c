#include "include/gdt.h"
#include "include/global_defs.h"
#include <stdint.h>

extern void breakpoint();
extern void serial_debug(int);

uint8_t TssStack[0x100000];
uint8_t ist1Stack[0x100000];
uint8_t ist2Stack[0x100000];

PACKED ALIGN_4K struct GDT_Entry null_seg = {

    .base = 0,
    .limit = 0x00000000,
    .access_byte = 0x00,
    .flags = 0x00

};

PACKED ALIGN_4K struct GDT_Entry Kernel_cs = {

    .base = 0,
    .limit = 0xFFFFF,
    .access_byte = GDTAccessKernelCode,
    .flags = 0xA

};

PACKED ALIGN_4K struct GDT_Entry Kernel_ds = {

    .base = 0,
    .limit = 0xFFFFF,
    .access_byte = GDTAccessKernelData,
    .flags = 0xC

};

PACKED ALIGN_4K struct GDT_Entry User_cs = {

    .base = 0,
    .limit = 0xFFFFF,
    .access_byte = GDTAccessUserCode,
    .flags = 0xA

};

PACKED ALIGN_4K struct GDT_Entry User_ds = {

    .base = 0,
    .limit = 0xFFFFF,
    .access_byte = GDTAccessUserData,
    .flags = 0xC

};

/* struct TSS_Entry TSS = {

    .base = &TSS,
    .limit = sizeof(TSS),
    .access_byte = 0x89,
    .flags = 0x0,
    .rsp0 = TssStack,
    .ist1 = ist1Stack,
    .ist2 = ist2Stack

};
 */


extern void gdt_load(uint16_t, uint64_t);
extern void reloadSegs();

void encodeGdtEntry(uint8_t *target, struct GDT_Entry source)
{
    // Check the limit to make sure that it can be encoded
    if (source.limit > 0xFFFFF)
    {
        serial_debug(0x47);
        serial_debug(0x44);
        serial_debug(0x54);
        serial_debug(0x20);
        serial_debug(0x4C);
        serial_debug(0x69);
        serial_debug(0x6D);
        serial_debug(0x69);
        serial_debug(0x74);
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

void LoadGDT_Stage1()
{

    encodeGdtEntry(0x1000, null_seg);
    encodeGdtEntry(0x1008, Kernel_cs);
    encodeGdtEntry(0x1010, Kernel_ds);
    encodeGdtEntry(0x1018, User_cs);
    encodeGdtEntry(0x1020, User_ds);

    
    reloadSegs();

    // breakpoint();
}