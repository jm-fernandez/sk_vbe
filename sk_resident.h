#ifndef RESIDENT_H_
#define RESIDENT_H_

#include "vbe.h"

#define MULTIPLEX_INSTANCE_NUMBER       0xEE

#define MULTIPLEX_SERVICE_CHECK                     0
#define MULTIPLEX_SERVICE_GET_SEGMENT               1
#define MULTIPLEX_SERVICE_OLD_VIDEO_HANDLER         2
#define MULTIPLEX_SERVICE_OLD_MULTIPLEX_HANDLER     3

#define SK_VIDEO_MODE_NUMBER            0x01FF
#define SK_VIDEO_MODE_COUNT             64

typedef void (__interrupt __far *sk_org_interrupt_handler_t)();
typedef void (__interrupt __far *sk_interrupt_handler_t)(union INTPACK registers);

void __far sk_store_psp_segment(unsigned short segment);
unsigned short __far sk_retrieve_psp_segment();
void __far sk_store_previous_video_handler(sk_org_interrupt_handler_t handler);
sk_org_interrupt_handler_t __far sk_retrieve_previous_video_handler();
void __far sk_store_previous_multiplex_handler(sk_org_interrupt_handler_t handler);
sk_org_interrupt_handler_t __far sk_retrieve_previous_multiplex_handler();
void __far sk_store_base_video_mode(unsigned short mode);
sk_interrupt_handler_t __far sk_get_video_handler();
sk_interrupt_handler_t __far sk_get_multiplex_handler();


#endif // RESIDENT_H_
