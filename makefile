#
# Makefile
#

BINARY_NAME = sk_vbe

BINARY_LAYOUT = order															  &
		clname CODE segment RESIDENT segment CANARY segment BEGTEXT segment _TEXT &
		clname FAR_DATA															  &
		clname BEGDATA segment _NULL segment _AFTERNULL

CFLAGS = -ms -s -oxs -d2 -4 -bt=dos
LFLAGS = system dos debug all $(BINARY_LAYOUT) OPTION MAP=$(BINARY_NAME).map
LIB_TOOL = wlib
LNK_TOOL = wlink
CC_TOOL = wcc

BINARY_EXE_FILE = $(BINARY_NAME).exe


OBJECT_FILES = sk_error.obj						&
			   sk_command_line.obj				&
			   sk_vbe.obj                       &
			   sk_resident.obj					&
			   sk_canary.obj					&
			   vbe.obj

all: $(BINARY_EXE_FILE)

$(BINARY_EXE_FILE): $(OBJECT_FILES)
	$(LNK_TOOL) $(LFLAGS) file {$(OBJECT_FILES)} name $(BINARY_EXE_FILE)

.c.obj:
	$(CC_TOOL) $(CFLAGS) $(FEATURESLT) $<

clean: .symbolic
	del *.obj
	del $(BINARY_NAME).map
	del $(BINARY_EXE_FILE)

