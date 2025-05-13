#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#include "sk_error.h"
#include "sk_dos.h"
#include "sk_command_line.h"
#include "sk_resident.h"
#include "sk_canary.h"
#include "vbe.h"



#define BIOS_VIDEO_INTERRUPT      0x10
#define DOS_MULTIPLEX_INTERRUPT   0x2F

#define MEMORY_PARAGRAPH_IN_BYTES   16

#define SK_TARGET_X_RESOLUTION      640
#define SK_TARGET_Y_RESOLUTION      480
#define SK_TARGET_BPP               8
#define SK_VIDEO_MEMORY_PAGES_REQ   3                                     // 1 Mb of video memory
#define SK_TARGET_ATTRIBUTES        (VBE_MODE_ATTRIBUTE_SUPPORTED \
                                    + VBE_MODE_ATTRIBUTE_COLOR    \
                                    + VBE_MODE_ATTRIBUTE_GRAPHIC  \
                                    + VBE_MODE_ATTRIBUTE_LINEAR)


static bool is_vesa_version_2_found(const VbeInfoBlock * info_block)
{
  return info_block->VESAVersion >> 8 == 2;
}

static bool is_sk_vbe_installed(const VbeInfoBlock * info_block)
{
  bool result = false;
  if (is_vesa_version_2_found(info_block))
  {
      unsigned short __far *mode_ptr = info_block->VideoModePtr;
      if(mode_ptr)
      {
        result = *mode_ptr == SK_VIDEO_MODE_NUMBER;
      }
  }
  return result;
}

static sk_error_t find_vbe_base_mode(const VbeInfoBlock * info_block, unsigned short * mode_id, VbeModeInfoBlock * mode_info_block)
{
  sk_error_t result = sk_error_vbe_driver_not_found;
  if (is_vesa_version_2_found(info_block))
  {
    int mode_count = 1;
    unsigned short __far *mode_ptr = info_block->VideoModePtr;
      
    result = sk_error_vbe_mode_not_found;
    while (*mode_ptr != VBE_LAST_VIDEO_MODE)
    {
      const unsigned short mode = *mode_ptr++;

      ++mode_count;
      if(mode_count >= SK_VIDEO_MODE_COUNT - 1)
      {
        result = sk_error_too_many_video_modes;
        break;

      }
      else if(mode == SK_VIDEO_MODE_NUMBER)
      {
        result = sk_error_video_mode_already_in_use;
        break;
      }
      else
      {
        const int mode_info_result = vbe_get_video_mode_info(mode, mode_info_block);
        if((mode_info_result == VBE_RESULT_OK)
        && (mode_info_block->XResolution == SK_TARGET_X_RESOLUTION)
        && (mode_info_block->YResolution == SK_TARGET_Y_RESOLUTION)
        && (mode_info_block->BitsPerPixel == SK_TARGET_BPP)
        && ((mode_info_block->ModeAttributes & SK_TARGET_ATTRIBUTES) == SK_TARGET_ATTRIBUTES)
        && (mode_info_block->PhysBasePtr != 0)
        && (mode_info_block->NumberOfImagePages >= SK_VIDEO_MEMORY_PAGES_REQ))
        {
          result = sk_success;
          *mode_id = mode;
        }
      }
    }
  }
  return result;
}

static sk_error_t install_sk_vbe(const VbeInfoBlock* info_block, unsigned short mode, const VbeModeInfoBlock* mode_info_block)
{
  sk_org_interrupt_handler_t r = _dos_getvect(DOS_MULTIPLEX_INTERRUPT);
  sk_error_t result = sk_error_multiplex_interrupt;
  union REGS input;
  union REGS output;
  int multiplex_instance_check_result;

  input.w.bx = 0;
  input.w.cx = 0;
  input.w.dx = 0;
  input.h.al	= MULTIPLEX_SERVICE_CHECK;
  input.h.ah	= MULTIPLEX_INSTANCE_NUMBER;

  multiplex_instance_check_result = int86(DOS_MULTIPLEX_INTERRUPT, &input, &output);

  if(output.h.al == 0)
  {
    sk_interrupt_handler_t multiplex_handler = sk_get_multiplex_handler();
    sk_interrupt_handler_t video_handler = sk_get_video_handler();
    sk_org_interrupt_handler_t original_video_handler = _dos_getvect(BIOS_VIDEO_INTERRUPT);
    sk_org_interrupt_handler_t original_multiplex_handler = _dos_getvect(DOS_MULTIPLEX_INTERRUPT);

    sk_store_psp_segment(_psp);
    sk_store_base_video_mode(mode);
    sk_store_previous_video_handler(original_video_handler);
    sk_store_previous_multiplex_handler(original_multiplex_handler);

    _disable();
    _dos_setvect(DOS_MULTIPLEX_INTERRUPT, multiplex_handler);
    _dos_setvect(BIOS_VIDEO_INTERRUPT, video_handler);
    _enable();
    result = sk_success;
  }

  return result;
}

static sk_error_t install_sk_vbe_command()
{
  VbeInfoBlock info_block;
  sk_error_t result = sk_error_vbe_controller_info_error;
  int controller_info_result = vbe_get_controller_info(&info_block);
  if(controller_info_result == VBE_RESULT_OK)
  {
    if(is_sk_vbe_installed(&info_block))
    {
      result = sk_error_vbe_installed;
    }
    else
    {
      VbeModeInfoBlock mode_info_block;
      unsigned short mode;

      result = find_vbe_base_mode(&info_block, &mode, &mode_info_block);
      if(SK_SUCCESS(result))
      {
        result = install_sk_vbe(&info_block, mode, &mode_info_block);
        if(SK_SUCCESS(result))
        {
          const unsigned long	resident_part_size = sk_get_resident_memory_size();
          const unsigned int paragraphs = PARAGRAPHS(resident_part_size);
          const char __far* oem = "(unknown oem)";
          const int vbe_version_digit_1 = ((info_block.VESAVersion >> 12) & 0xF) % 10;
          const int vbe_version_digit_2 = ((info_block.VESAVersion >>  8) & 0xF) % 10;
          const int vbe_version_digit_3 = ((info_block.VESAVersion >>  4) & 0xF) % 10;
          const int vbe_version_digit_4 = ((info_block.VESAVersion >>  0) & 0xF) % 10;

          const int vesa_version = vbe_version_digit_1 * 10 + vbe_version_digit_2;
          const int vesa_sub_version = vbe_version_digit_3 * 10 + vbe_version_digit_4;

          if(info_block.OEMStringPtr && info_block.OEMStringPtr[0] != 0)
          {
            oem = info_block.OEMStringPtr;
          }

          printf("School Kombat VBE\n");
          printf("Detectado driver VBE  %s.\n", oem);
          printf("Detecatada VBE version: %d%d.%d%d\n", vbe_version_digit_1, vbe_version_digit_2, vbe_version_digit_3, vbe_version_digit_4);
          printf("Detectados  %d Mb de memoria en la tarjeta de video\n", info_block.TotalMemory / 16);
          printf("Detectado modo de video compatible: 0x%x\n", mode);
          printf("Aplicacion residente: %d bloques (%d bytes).\n", paragraphs, resident_part_size);
          _dos_keep(0, paragraphs);
        }
      }
    }
  }
  else
  {
    printf("Controller Info Result: %d bloques.\n", controller_info_result);
  }
  return result;
}

static sk_org_interrupt_handler_t get_original_interrupt_handler(int handler_id)
{
  sk_org_interrupt_handler_t result = NULL;
  union REGS input;
  union REGS output;
  struct SREGS regs;

  input.h.al	= handler_id;
  input.h.ah	= MULTIPLEX_INSTANCE_NUMBER;
  segread(&regs);
  if(int86x(DOS_MULTIPLEX_INTERRUPT, &input, &output, &regs) == 0)
  {
    result = MK_FP(regs.es, output.w.di);
  }
  return result;
}

static unsigned short get_resident_segment()
{
  unsigned short result = 0;
  union REGS input;
  union REGS output;

  input.h.al	= MULTIPLEX_SERVICE_GET_SEGMENT;
  input.h.ah	= MULTIPLEX_INSTANCE_NUMBER;

  if(int86(DOS_MULTIPLEX_INTERRUPT, &input, &output) == 0)
  {
    result = output.w.bx;
  }
  return result;
}

static void uninstall_sk_vbe()
{
  sk_org_interrupt_handler_t old_video_handler = get_original_interrupt_handler(MULTIPLEX_SERVICE_OLD_VIDEO_HANDLER);
  sk_org_interrupt_handler_t old_multiplex_handler = get_original_interrupt_handler(MULTIPLEX_SERVICE_OLD_MULTIPLEX_HANDLER);
  const unsigned short resident_segment = get_resident_segment();
  const unsigned short __far* peb_segment = MK_FP(resident_segment, 0x2C);
    
  _disable();
  _dos_setvect(BIOS_VIDEO_INTERRUPT, old_video_handler);
  _dos_setvect(DOS_MULTIPLEX_INTERRUPT, old_multiplex_handler);
  _enable();  

  _dos_freemem(resident_segment);
  _dos_freemem(*peb_segment);
}

static sk_error_t uninstall_sk_vbe_command()
{
  VbeInfoBlock info_block;
  sk_error_t result = sk_error_vbe_controller_info_error;
  if(vbe_get_controller_info(&info_block) == VBE_RESULT_OK)
  {
    if(!is_sk_vbe_installed(&info_block))
    {
      result = sk_error_vbe_not_installed;
    }
    else
    {
      uninstall_sk_vbe();
      result = sk_success;
    }
  }
  return result;
}

int main(int argc, char* argv[])
{
  command_t command;
  sk_error_t result = sk_parse_command_line(argc, argv, &command);
  if(SK_SUCCESS(result))
  {
    switch(command)
    {
      case install:
        result = install_sk_vbe_command();
        break;
      case uninstall:
        result = uninstall_sk_vbe_command();
        break;
      case help:
        sk_show_help(argv[0]);
    }  
  }

  if(!SK_SUCCESS(result))
  {
    fprintf(stderr, "Error %d: %s\n", result, sk_error_description(result));
    if(result == sk_error_command_line)
    {
      sk_show_help(argv[0]);
    }
  }
  return SK_SUCCESS(result) ? 0 : -1;
}
