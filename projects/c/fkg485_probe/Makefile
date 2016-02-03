SOURCE_DIR:=$(SRC_DIR)/fkg485_probe/src
OBJECT_DIR:=./obj

include $(TOP_DIR)/Makefile.rules

PROGS     = $(OUT_DIR)/fkg485_probe

SRCS := \
  $(COMMON_DIR)/lib-c/utils/stri.c \
  $(COMMON_DIR)/lib-c/utils/strl.c \
  $(COMMON_DIR)/lib-c/utils/timeval.c \
  $(SHARED_DIR)/485/fkg485_channel.c \
  $(SHARED_DIR)/485/fkg485_comm.c \
  $(SHARED_DIR)/485/fkg485_proto.c \
  $(SHARED_DIR)/485/fkg485_thread.c \
  $(SHARED_DIR)/485/fkg485_mk1.c \
  $(SHARED_DIR)/fkg_commands.c \
  $(SHARED_DIR)/fkg_devices.c \
  $(SHARED_DIR)/fkg_handler.c \
  $(SHARED_DIR)/fkg_verbose.c \
  $(SHARED_DIR)/events.c \
  $(wildcard $(LIB_DIR)/src/*.c) \
  $(wildcard $(SOURCE_DIR)/*.c)
	
_OBJS := $(patsubst %.c,%$(OBJECTEXT),$(SRCS))
_OBJS:=$(subst $(COMMON_DIR),$(OBJECT_DIR),$(_OBJS))
_OBJS:=$(subst $(SHARED_DIR),$(OBJECT_DIR),$(_OBJS))
_OBJS:=$(subst $(LIB_DIR),$(OBJECT_DIR),$(_OBJS))
OBJS:=$(subst $(SOURCE_DIR),$(OBJECT_DIR),$(_OBJS))

$(PROGS): $(OBJS)
	mkdir -p $(OUT_DIR)
	$(CC) $^ -lpthread -o $@
	@for i in $(PROGS) ; do \
			$(STRIP) $$i; \
	done
	
all:	$(PROGS)

install:	$(PROGS)
	cp -f $(PROGS) $(INSTDIR)
	
clean:
	rm -f $(PROGS)
	rm -f $(OBJS)	

strip: $(PROGS)
	@for i in $(PROGS) ; do \
			$(STRIP) $$i; \
	done
