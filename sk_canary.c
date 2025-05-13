#include "sk_canary.h"
#include "sk_resident.h"
#include "sk_dos.h"

#pragma code_seg("CANARY", "CODE");

unsigned long __far sk_get_resident_memory_size()
{
     unsigned long resident_memory_size;
     const unsigned long this_function_addr = FP_PHYSICAL_ADDRESS(sk_get_resident_memory_size);
     const unsigned short psp_segment = sk_retrieve_psp_segment();
     const unsigned long psp_addr = PHYSICAL_ADDRESS(psp_segment, 0);
     return this_function_addr - psp_addr;
}
