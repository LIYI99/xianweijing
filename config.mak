PWD_PATH		= /home/liyi/idscam/xianweijing


CONF_PATH 		=$(PWD_PATH)/config.mak

ADI_INC_PATH 	= $(PWD_PATH)/inc/adi/
IMAGE_INC_PATH	= $(PWD_PATH)/inc/image/
PNG_INC_PATH 	= $(PWD_PATH)/png/include


#output path
PUT_INCLUDE 	=$(PWD_PATH)
PUT_OBJ			=$(PWD_PATH)/obj


#INCLUDE_PATH 	= -I$(D_LIST_DIR)
#INCLUDE_PATH 	+= -I$(D_QUEUE_DIR)
INCLUDE_PATH 	+= -I$(ADI_INC_PATH)
#INCLUDE_PATH 	+= -I$(THREAD_POOL_DIR)
INCLUDE_PATH	+= -I$(IMAGE_INC_PATH)
INCLUDE_PATH 	+= -I$(PNG_INC_PATH)



COPS = $(INCLUDE_PATH)
#ARM_ASM -O2 -O3 -Wall
CROSS_COMPILE =arm-goke-linux-uclibcgnueabi-

CC=$(CROSS_COMPILE)gcc
CCC=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar
AS=$(CROSS_COMPILE)as
STRIP=$(CROSS_COMPILE)strip



 


