include config.mak

CSRC =  object_pool.c logsrv.c xw_logsrv.c image_argb_ayuv.c image_zoom_func.c xw_text_prompt_box.c gk_image_sdk_new.c gk_mouse_event.c image_text_put.c image_png_put.c gk_device_init.c 
CSRC += xw_png_load.c xw_date_show.c xw_top_menu_show.c xw_main_menu_show.c xw_line_show.c xw_main_line_manger_show.c xw_main_isp_show.c xw_preview_show.c 
CSRC += xw_test_freshen.c
SLSRC = 

COBJS = ${CSRC:%.c=%.o}

SOBJS = ${SLSRC:%.s=%.o}
COPS  = -g  -Wall $(INCLUDE_PATH)

LIBS += ./lib/libadi.a
LIBS += ./lib/libimage.a
LIBS += ./png/libpng.a
LIBS += ./zlib/libz.a


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
	$(CC)  $(COPS) -O2  main_loop_new.c  $(TARGET) -o mouse-test $(LIBS) -lpthread -lm 
	cp mouse-test  /710x_rootfs_2.1/rootfs_uClibc/usr/local/bin




