include config.mak

CSRC =  image_object_pool.c image_xbuddy_mempool.c image_logsrv.c image_host_event_dect.c image_argb_ayuv.c image_zoom_func.c  image_sdk_core.c image_mouse_event.c image_text_put.c image_png_put.c 
CSRC += gk_device_init.c 
CSRC += xw_logsrv.c xw_png_load.c xw_date_show.c xw_top_menu_show.c xw_main_menu_show.c xw_line_show.c xw_main_line_manger_show.c xw_main_isp_show.c xw_preview_show.c xw_text_prompt_box.c 
CSRC += xw_window_def_func.c xw_update.c xw_msg_prv.c  xw_main_start.c 
CSRC += xw_key_get.c
#xw_msg_pub.c xw_test_freshen.c

SLSRC = 

COBJS = ${CSRC:%.c=%.o}

SOBJS = ${SLSRC:%.s=%.o}
COPS  = -g  -Wall $(INCLUDE_PATH)

LIBS += ./lib/libadi.a
LIBS += ./lib/libimage.a
LIBS += ./lib/libpng.a
LIBS += ./lib/libz.a


TARGET = libimageui.a

$(TARGET) : $(COBJS) $(SOBJS) 
	$(AR) -r $(TARGET)  *.o
%.o: %.c
	$(CC) -c $(COPS) -I. -o $@ $<

%.o : %.s
	$(CC) -c $(COPS) -I. -o $@ $<

share:
	$(CC) $(COPS) $(CSRC) -shared -o libimageui.so
clean :
	/bin/rm -f *.o
	/bin/rm -r *.a
	/bin/rm  mouse-test
test:
	$(CC)  $(COPS)    main_loop_new.c xw_msg_pub.c  $(TARGET) -o mouse-test $(LIBS) -lpthread -lm 
	cp mouse-test  /nfs_rootfs/rootfs_uClibc/usr/local/bin
build:
	cp xw_msg_pub.h  xw_msg_pub.c  xw_main_start.h  ../../xw_build
	cp libimageui.a ../../xw_build




