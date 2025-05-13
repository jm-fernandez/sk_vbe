#include <i86.h>
#include "vbe.h"

int vbe_get_controller_info(VbeInfoBlock *infoBlock)
{
  union REGS input;
  union REGS output;
  struct SREGS regs;

  infoBlock->VESASignature[0] = 'V';
  infoBlock->VESASignature[1] = 'B';
  infoBlock->VESASignature[2] = 'E';
  infoBlock->VESASignature[3] = '2';

  segread(&regs);

  input.w.ax = 0x4F00;
  input.w.di = FP_OFF(infoBlock);
  regs.es = regs.ds;

  return int86x(0x10, &input, &output, &regs);
}

int vbe_get_video_mode_info(unsigned int mode, VbeModeInfoBlock *modeInfoBlock)
{
  union REGS input;
  union REGS output;
  struct SREGS regs;

  segread(&regs);

  input.w.ax = 0x4F01;
  input.w.cx = mode;
  input.w.di = FP_OFF(modeInfoBlock);
  regs.es = regs.ds;
  return int86x(0x10, &input, &output, &regs);
}
