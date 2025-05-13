#ifndef VBE_H_
#define VBE_H_

#include <stdbool.h>


#define VBE_RESULT_OK                    0x004F
#define VBE_RESULT_ERROR                 0x014F
#define VBE_RESULT_NOT_SUPPORTED         0x024F
#define VBE_RESULT_INVALID               0x034F

#define VBE_CAPABILITIES_PROGRAMABLE_DAC                   1
#define VBE_CAPABILITIES_CONTROLLER_NOT_VGA_COMPATIBLE     2
#define VBE_CAPABILITIES_PROGRAM_RAMDAC_USING_BLANK        4

#define VBE_MODE_ATTRIBUTE_SUPPORTED     1
#define VBE_MODE_ATTRIBUTE_TTY_OUTPUT    4
#define VBE_MODE_ATTRIBUTE_COLOR         8
#define VBE_MODE_ATTRIBUTE_GRAPHIC       16
#define VBE_MODE_ATTRIBUTE_VGA           32
#define VBE_MODE_ATTRIBUTE_NOT_WINDOWED  64
#define VBE_MODE_ATTRIBUTE_LINEAR        128

#define VBE_SET_MODE_LINEAR                16384
#define VBE_SET_MODE_DO_NOT_CLEAR_DISPLAY  32768

#define VBE_LAST_VIDEO_MODE               ((unsigned short)-1)


#pragma pack(push, 1)
typedef struct VBE_PMInfoBlock {
  unsigned int Signature;
  unsigned short int EntryPoint; 
  unsigned short int PMInitialize;
  unsigned short int BIOSDataSel;
  unsigned short int A0000Sel;
  unsigned short int B0000Sel;
  unsigned short int B8000Sel;
  unsigned short int CodeSegSel;
  unsigned char InProtectMode;
  unsigned char Checksum;
} VBE_PMInfoBlock;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct VbeModeInfoBlock_ {
  unsigned short ModeAttributes;
  unsigned char WinAAttributes;
  unsigned char WinBAttributes;
  unsigned short WinGranularity;
  unsigned short WinSize;
  unsigned short WinASegment;
  unsigned short WinBSegment;
  void (__far * WinFuncPtr)(void);
  unsigned short BytesPerScanLine;
  unsigned short XResolution;
  unsigned short YResolution;
  unsigned char XCharSize;
  unsigned char YCharSize;
  unsigned char NumberOfPlanes;
  unsigned char BitsPerPixel;
  unsigned char NumberOfBanks;
  unsigned char MemoryModel;
  unsigned char BankSize;
  unsigned char NumberOfImagePages;
  unsigned char res1;
  unsigned char RedMaskSize;
  unsigned char RedFieldPosition;
  unsigned char GreenMaskSize;
  unsigned char GreenFieldPosition;
  unsigned char BlueMaskSize;
  unsigned char BlueFieldPosition;
  unsigned char RsvdMaskSize;
  unsigned char RsvdFieldPosition;
  unsigned char DirectColorModeInfo;
  /* VESA 2.0 */
  unsigned long PhysBasePtr;
  unsigned int OffScreenMemOffset;
  unsigned short OffScreenMemSize;
  /* VESA 3.0 */
  unsigned short LinBytesPerScanLine;
  unsigned char BnkNumberOfImagePages;
  unsigned char LinNumberOfImagePages;
  unsigned char LinRedMaskSize;
  unsigned char LinRedFieldPosition;
  unsigned char LinGreenMaskSize;
  unsigned char LinGreenFieldPositiondb;
  unsigned char LinBlueMaskSize;
  unsigned char LinBlueFieldPosition;
  unsigned char LinRsvdMaskSize;
  unsigned char LinRsvdFieldPosition;
  unsigned int MaxPixelClock;  
  unsigned char res2[189];
} VbeModeInfoBlock;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct VbeInfoBlock_ {
  char VESASignature[4];
  unsigned short VESAVersion;
  char __far *OEMStringPtr;
  unsigned long Capabilities;
  unsigned short __far *VideoModePtr;
  unsigned short TotalMemory;
  unsigned short OemSoftwareRev;
  char __far *OemVendorNamePtr;
  char __far *OemProductNamePtr;
  char __far *OemProductRevPtr;
  char reserved[222];
  char OemData[256];
} VbeInfoBlock;
#pragma pack(pop)

int vbe_get_controller_info(VbeInfoBlock * infoBlock);
int vbe_get_video_mode_info(unsigned int mode, VbeModeInfoBlock * modeInfoBlock);

#endif	/* VBE_H_ */
