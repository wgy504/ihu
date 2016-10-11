CC=gcc

STATIC_BUILD?=y
M32?=n

SDKROOT?=../..
INSTALLDIR?=$(SDKROOT)/binaries

ifneq ($(WINDIR),)
	EXE_EXT=.exe
endif

# verbosity switch
V?=0

ifeq ($(STATIC_BUILD),y)
	LDFLAGS+=-static
endif

ifeq ($(M32),y)
	CFLAGS+=-m32
	LDFLAGS+=-m32
endif

ifeq ($(V),0)
	V_CC = @echo "  CC    " $@;
	V_LINK = @echo "  LINK  " $@;
	V_CLEAN = @echo "  CLEAN ";
	V_CLEAN_TEMP_FILES = @echo "  CLEAN_TEMP_FILES ";
	V_STRIP = @echo "  STRIP " $@;
	V_INSTALL = @echo "INSTALL " $<;
else
	V_OPT = '-v'
endif

CFLAGS+=-std=gnu99 -Wall -O2 -Wl,-Map,$@.map

ifeq ($(V),2)
	CFLAGS+=--verbose --save-temps -fverbose-asm
	LDFLAGS+=-Wl,--verbose
endif

EXEC=mkimage
OBJS=mkimage.o sw_aes.o

# how to compile C files
%.o : %.c
	$(V_CC)$(CC) $(CFLAGS) -c $< -o $@

all: $(EXEC)

$(EXEC): $(OBJS)
	$(V_LINK)$(CC) $(LDFLAGS) -o $@$(EXE_EXT) $(OBJS) $(LDLIBS)
	$(V_STRIP)strip -s $@$(EXE_EXT)

clean:
	$(V_CLEAN)rm -f $(V_OPT) $(EXEC)$(EXE_EXT) *.[ois]

install: $(EXEC)
	$(V_INSTALL)install $(EXEC)$(EXE_EXT) $(INSTALLDIR)
