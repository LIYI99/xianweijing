
INCLUDE_PATH =  -I./include
INCLUDE_PATH += -I/home/liyi/710X_SDK/GK710X_LinuxSDK_v1.1.0/applications/adi/include

COPS = $(INCLUDE_PATH) -O2 -O3 

#COPS = $(INCLUDE_PATH) -D ARM_ASM -O2 -O3 -Wall  
CROSS_COMPILE=$(CROSS_COMPILE_HEAD)arm-goke-linux-uclibcgnueabi-


CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)as
AR=$(CROSS_COMPILE)ar
STRIP=$(CROSS_COMPILE)strip

CSRC =  netcam_overlay.c netcam_overlay_cfg.c overlay.c

COBJS = ${CSRC:%.c=%.o}
COBJS_M = ${CSRC_M:%.c=%.o}



TARGET = libosdlibdemo.a

$(TARGET) : $(COBJS) 
	$(AR) -r  -o $(TARGET) $(COBJS)
	
%.o: %.c
	$(CC) -c $(COPS) -I./include -o $@ $<

%.o : %.s
	$(CC) -c $(COPS) -I./include -o $@ $<

install:
	cd ./

clean :
	/bin/rm -f *.o
	/bin/rm -f $(TARGET)

