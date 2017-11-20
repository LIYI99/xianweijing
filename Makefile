include config.mak

CSRC =  object_pool.c gk_image_sdk.c gk_mouse_event.c gk_device_init.c 
SLSRC = 

COBJS = ${CSRC:%.c=%.o}
SOBJS = ${SLSRC:%.s=%.o}
LOPTS = -g

LIBS += ./lib/libadi.a
LIBS += ./lib/libimage.a


TARGET = libgkgrap.a

$(TARGET) : $(COBJS) $(SOBJS) 
	$(AR) -r $(TARGET)  *.o

%.o: %.c
	$(CC) -c $(COPS) -I. -o $@ $<

%.o : %.s
	$(CC) -c $(COPS) -I. -o $@ $<

share:
	$(CC) $(COPS) $(CSRC) -shared -o libgkgrap.so
clean :
	/bin/rm -f *.o
	/bin/rm -r *.a
	/bin/rm  mouse-test
test:
	$(CC)  $(COPS) -g  main_loop_new.c  $(TARGET) -o mouse-test $(LIBS) -lpthread
	cp mouse-test  /710x_rootfs_2.1/rootfs_uClibc/usr/local/bin




