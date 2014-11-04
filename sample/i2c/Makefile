#===============================================================================
# export variable
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
SDK_DIR := $(CURDIR)/../..
include $(SDK_DIR)/base.mak
endif

include $(SAMPLE_DIR)/base.mak

#===============================================================================
# local variable
#===============================================================================
CFLAGS += $(CFG_HI_SAMPLE_CFLAGS)
CFLAGS += -I$(SDK_INC_DIR) -I$(SAMPLE_DIR)/common


LIBPATH = -L$(SDK_STATIC_LIB_DIR) -L$(SDK_SHARE_LIB_DIR)
LIB = -Wl,--start-group $(SYS_LIBS) $(HI_LIBS) -Wl,--end-group

APP1 = i2c_read
OBJS1 = sample_i2c_read.o

APP2 = i2c_write
OBJS2 = sample_i2c_write.o

APP3 = temperature_show
OBJS3 = temperature_show.o

APP4 = clock_show
OBJS4 = clock_show.o

APP5 = clock_set
OBJS5 = clock_set.o
#===============================================================================
# rules
#===============================================================================
.PHONY: all clean install uninstall

all:$(APP1) $(APP2) $(APP3) $(APP4) $(APP5) install

%.o: %.c
	$(AT)$(CC) $(CFLAGS) -c -o $@ $^

$(APP1): $(OBJS1)
	$(AT)$(CC) -o $@ $^ $(LIBPATH) $(LIB)

$(APP2): $(OBJS2)
	$(AT)$(CC) -o $@ $^ $(LIBPATH) $(LIB)

$(APP3): $(OBJS3)
	$(AT)$(CC) -o $@ $^ $(LIBPATH) $(LIB)

$(APP4): $(OBJS4)
	$(AT)$(CC) -o $@ $^ $(LIBPATH) $(LIB)

$(APP5): $(OBJS5)
	$(AT)$(CC) -o $@ $^ $(LIBPATH) $(LIB)

clean:
	-$(AT)rm -f $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5)
	-$(AT)rm -f $(APP1) $(APP2) $(APP3) $(APP4) $(APP5)

install:
	cp $(APP3) $(ROOTBOX_DIR)/usr/sbin
	cp $(APP4) $(ROOTBOX_DIR)/usr/sbin
	cp $(APP5) $(ROOTBOX_DIR)/usr/sbin

uninstall:
