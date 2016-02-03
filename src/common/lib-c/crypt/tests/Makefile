AXIS_USABLE_LIBS = UCLIBC GLIBC
include $(EBOX_TOP_DIR)/Rules.elinux

PROGS     = crypto_test

CFLAGS   += -D_DEBUG -Werror

INSTDIR   = $(prefix)/ebox/bin/
INSTMODE  = 0755
INSTOWNER = root
INSTGROUP = root

SRCS := \
	$(wildcard ../block/*.c) \
	$(wildcard ../rsa/*.c) \
	$(wildcard ../hash/*.c) \
	$(wildcard ../vigik/*.c) \
	$(wildcard ../utils/*.c) \
	main.c
	
OBJS := $(patsubst %.c,%.o,$(SRCS))

$(PROGS): $(OBJS)
	$(CC) $^ -o $@
	rm $(OBJS)

all:	$(PROGS)

install:	$(PROGS)
	$(INSTALL) -d $(INSTDIR)
	$(INSTALL) -m $(INSTMODE) -o $(INSTOWNER) -g $(INSTGROUP) $(PROGS) $(INSTDIR)
	
clean:
	rm -f $(PROGS)
	rm -f $(OBJS)	
