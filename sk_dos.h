#ifndef SK_DOS_H_
#define SK_DOS_H_

#include <i86.h>

#define PHYSICAL_ADDRESS(segment, register) (((segment) * 16) + (register))
#define FP_PHYSICAL_ADDRESS(ptr) PHYSICAL_ADDRESS(FP_SEG(ptr), FP_OFF(ptr))
#define PARAGRAPH_SIZE                      16
#define PARAGRAPHS(size)                    (((size) + PARAGRAPH_SIZE - 1) / PARAGRAPH_SIZE)
#endif // SK_DOS_H_
