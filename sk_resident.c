#include <stdio.h>
#include <dos.h>
#include <i86.h>

#include "sk_resident.h"
#include "sk_dos.h"
#include "vbe.h"

#pragma data_seg("RESIDENT", "CODE");
#pragma code_seg("RESIDENT", "CODE");

unsigned char __far sk_palette[4*256];
static unsigned short __far sk_video_mode_list[SK_VIDEO_MODE_COUNT] = {0};
static unsigned short __far sk_psp_segment = 0;
static sk_org_interrupt_handler_t sk_previous_video_handler = NULL;
static sk_org_interrupt_handler_t sk_previous_multiplex_handler = NULL;
static unsigned short __far sk_base_video_mode = 0;
static bool __far sk_video_mode_list_filled = false;
static bool __far sk_mode_active = false;
static int __far sk_palette_bpc = 6;

static void __declspec(naked) chain_interrupt(sk_org_interrupt_handler_t handler)
{
  _asm
  {
    mov     cx, ax
    mov     ax, dx
    mov     sp, bp
    xchg    cx, 20[bp]
    xchg    ax, 22[bp]
    pop     dx
    pop     dx
    pop     es
    pop     ds
    pop     di
    pop     si
    pop     bp
    pop     bx
    pop     bx
    pop     dx
    retf
  }
}

static int __declspec(naked) __far call_interrupt(sk_org_interrupt_handler_t handler)
{
  _asm
  {
    pop bx
    pop cx
    pushf
    push cx
    push bx
    push dx
    push ax
    mov bx, 16[bp]
    mov dx, 18[bp]
    mov cx, 20[bp]
    mov ax, 22[bp]
    retf
  }
}

static void __interrupt __far sk_video_int_handler(union INTPACK registers)
{    
  if(registers.w.ax == 0x4F00)
  {
    // Get Controller Info
    VbeInfoBlock __far * info_block = (VbeInfoBlock __far*) MK_FP(registers.w.es, registers.w.di);
    registers.w.ax = call_interrupt(sk_previous_video_handler);
    if(registers.w.ax == VBE_RESULT_OK)
    {
      if(!sk_video_mode_list_filled)
      {
        unsigned short __far * vbe_modes = info_block->VideoModePtr;
        unsigned short __far * sk_video_modes = sk_video_mode_list;
        *sk_video_modes++ = SK_VIDEO_MODE_NUMBER;
        while(*vbe_modes != VBE_LAST_VIDEO_MODE)
        {
          *sk_video_modes++ = *vbe_modes++;
        }
        *sk_video_modes = VBE_LAST_VIDEO_MODE;
        sk_video_mode_list_filled = true;
      }
      info_block->VideoModePtr = sk_video_mode_list;
    }
  }
  else if(registers.w.ax == 0x4F01)
  {
    // Get Video Mode Info
    VbeModeInfoBlock __far * info_block = (VbeModeInfoBlock __far*) MK_FP(registers.w.es, registers.w.di);
    const int mode = registers.w.cx;
    if(mode == SK_VIDEO_MODE_NUMBER)
    {
      registers.w.cx = sk_base_video_mode;
    }
    registers.w.ax = call_interrupt(sk_previous_video_handler);
    if(registers.w.ax == VBE_RESULT_OK)
    {
      registers.w.cx = mode;
      info_block->WinASegment = 0;
      info_block->WinBSegment = 0;
      info_block->ModeAttributes |= VBE_MODE_ATTRIBUTE_NOT_WINDOWED;      
    }
  }
  else if(registers.w.ax == 0x4F02)
  {
    // Set Video mode (vbe)
    const int mode = registers.w.bx & ~(VBE_SET_MODE_LINEAR | VBE_SET_MODE_DO_NOT_CLEAR_DISPLAY);
    if(mode == SK_VIDEO_MODE_NUMBER)
    {
      sk_mode_active = true;
      sk_palette_bpc = 6;
      registers.w.bx = sk_base_video_mode | VBE_SET_MODE_LINEAR;
    }
    else
    {
      sk_mode_active = false;
    }
    chain_interrupt(sk_previous_video_handler);
  }
  else if(registers.h.ah == 0)
  {
    // set video mode
    sk_mode_active = false;
    chain_interrupt(sk_previous_video_handler);
  }
  else if(registers.w.ax == 0x4F08 && sk_mode_active)
  {
    if(registers.h.bl == 0)
    {
      if(registers.h.bh == 6 || registers.h.bh == 8)
      {
        sk_palette_bpc = registers.h.bh;
        registers.w.ax = VBE_RESULT_OK;
      }
      else
      {
        registers.h.bh = sk_palette_bpc;
        registers.w.ax = VBE_RESULT_INVALID;
      }
    }
    else
    {
      registers.h.bh = sk_palette_bpc;
      registers.w.ax = VBE_RESULT_OK;
    }
  }
  else if(registers.w.ax == 0x4F09 && sk_mode_active && sk_palette_bpc == 8)
  {
    // Set/Get palette data
    if(registers.h.bl == 0 || registers.h.bl == 2)
    {
      const unsigned char __far* user_palette = (const unsigned char __far*) MK_FP(registers.w.es, registers.w.di);
      unsigned char __far* sk_palette_ptr = sk_palette;
      const int count = registers.w.cx;
      int i = 0;
      for(; i < count; ++i)
      {
        *sk_palette_ptr++ = *user_palette++ >> 2;
        *sk_palette_ptr++ = *user_palette++ >> 2;
        *sk_palette_ptr++ = *user_palette++ >> 2;
        *sk_palette_ptr++ = *user_palette++ >> 2;
      }

      registers.w.es = FP_SEG(sk_palette);
      registers.w.di = FP_OFF(sk_palette);
      chain_interrupt(sk_previous_video_handler);
    }
    else
    {
      unsigned char __far* user_palette = (unsigned char __far*) MK_FP(registers.w.es, registers.w.di);
      const int count = registers.w.cx;

      registers.w.ax = call_interrupt(sk_previous_video_handler);
      if(registers.w.ax == VBE_RESULT_OK)
      {
        int i = 0;
        for(;i<count; ++i)
        {
          *user_palette++ <<= 2;
          *user_palette++ <<= 2;
          *user_palette++ <<= 2;
          *user_palette++ <<= 2;
        }
      }
    }
  }
  else
  {
    chain_interrupt(sk_previous_video_handler);
  }
}

static void __interrupt __far sk_multiplex_int_handler(union INTPACK registers)
{
	if (registers.h.ah == MULTIPLEX_INSTANCE_NUMBER)
	{
		switch (registers.h.al)
		{      
		case MULTIPLEX_SERVICE_CHECK:
			registers.h.al	= 0xFF;
			break;

    case MULTIPLEX_SERVICE_GET_SEGMENT:
      registers.w.ax = 0;
      registers.w.bx = sk_psp_segment;
      break;
    
    case MULTIPLEX_SERVICE_OLD_VIDEO_HANDLER:
      {
        registers.w.ax = 0;
        registers.w.es = FP_SEG(sk_previous_video_handler);
        registers.w.di = FP_OFF(sk_previous_video_handler);        
      }
      break;
    
    case MULTIPLEX_SERVICE_OLD_MULTIPLEX_HANDLER:
      {
        registers.w.ax = 0;
        registers.w.es = FP_SEG(sk_previous_multiplex_handler);
        registers.w.di = FP_OFF(sk_previous_multiplex_handler);        
      }
      break;

		default:
			registers.w.ax	= -1;
			break;
		}
	}
	else
  {
    chain_interrupt(sk_previous_multiplex_handler);
  }
  return;
}

void __far sk_store_psp_segment(unsigned short segment)
{
  sk_psp_segment = segment;
}

unsigned short __far sk_retrieve_psp_segment()
{
  return sk_psp_segment;
}

void __far sk_store_previous_video_handler(sk_org_interrupt_handler_t handler)
{
  sk_previous_video_handler = handler;
}

sk_org_interrupt_handler_t __far sk_retrieve_previous_video_handler()
{
  return sk_previous_video_handler;
}

void __far sk_store_previous_multiplex_handler(sk_org_interrupt_handler_t handler)
{
  sk_previous_multiplex_handler = handler;
}

sk_org_interrupt_handler_t __far sk_retrieve_previous_multiplex_handler()
{
  return sk_previous_multiplex_handler;
}

void __far sk_store_base_video_mode(unsigned short mode)
{
  sk_base_video_mode = mode;
}

sk_interrupt_handler_t __far sk_get_video_handler()
{
  return sk_video_int_handler;
}

sk_interrupt_handler_t __far sk_get_multiplex_handler()
{
	return sk_multiplex_int_handler;
}
