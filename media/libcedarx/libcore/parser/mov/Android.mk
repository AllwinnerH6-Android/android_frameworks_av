LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

CEDARX_ROOT=$(LOCAL_PATH)/../../../
include $(CEDARX_ROOT)/config.mk

LOCAL_SRC_FILES = \
                $(notdir $(wildcard $(LOCAL_PATH)/*.c))

LOCAL_C_INCLUDES:= \
    $(CEDARX_ROOT)/ \
    $(CEDARX_ROOT)/libcore \
    $(CEDARX_ROOT)/libcore/base/include \
    $(CEDARX_ROOT)/libcore/parser/include \
    $(CEDARX_ROOT)/libcore/stream/include \
    $(CEDARX_ROOT)/external/include/adecoder \
    $(TOP)/frameworks/av/media/libcedarc/include \
    $(TOP)/frameworks/av/media/libcedarc/vdecoder/include \
    $(TOP)/frameworks/av/media/libcedarc/sdecoder/include \
    $(TOP)/external/zlib \
    $(CEDARX_ROOT)/libcore/parser/base/id3base

######################################################################
#$(warning CONF_PRODUCT_STB=$(CONF_PRODUCT_STB))
#$(warning TARGET_BOARD_PLATFORM=$(TARGET_BOARD_PLATFORM))
#$(warning BOARD_USE_PLAYREADY=$(BOARD_USE_PLAYREADY))
#$(warning CONF_ANDROID_VERSION=$(CONF_ANDROID_VERSION))
ifeq ($(CONF_PRODUCT_STB), yes)
ifneq (, $(filter $(TARGET_BOARD_PLATFORM), petrel tulip))
    ifeq ($(BOARD_USE_PLAYREADY), 1)
        ifeq ($(CONF_ANDROID_VERSION), 9)
	BOARD_USE_PLAYREADY_NEW := 1
	LOCAL_C_INCLUDES += \
            $(TOP)/frameworks/av/media/include \
            $(TOP)/frameworks/av/media/libmedia/include \
            $(TOP)/frameworks/native/include \
            $(TOP)/system/core/base/include

        LOCAL_SRC_FILES += \
                $(notdir $(wildcard $(LOCAL_PATH)/*.cpp))
        else
            ifeq ($(PLAYREADY_DEBUG), 1)
            PLAYREADY_DIR:= $(TOP)/hardware/aw/playready
            include $(PLAYREADY_DIR)/config.mk
            LOCAL_CFLAGS += $(PLAYREADY_CFLAGS)
            LOCAL_C_INCLUDES += \
               $(PLAYREADY_DIR)/source/inc                 \
               $(PLAYREADY_DIR)/source/oem/common/inc      \
               $(PLAYREADY_DIR)/source/oem/ansi/inc        \
               $(PLAYREADY_DIR)/source/results             \
               $(PLAYREADY_DIR)/source/tools/shared/common
            else
            include $(TOP)/hardware/aw/playready/config.mk
            LOCAL_CFLAGS += $(PLAYREADY_CFLAGS)
            LOCAL_C_INCLUDES +=                         \
                $(TOP)/hardware/aw/playready/include/inc                 \
                $(TOP)/hardware/aw/playready/include/oem/common/inc      \
                $(TOP)/hardware/aw/playready/include/oem/ansi/inc        \
                $(TOP)/hardware/aw/playready/include/results             \
                $(TOP)/hardware/aw/playready/include/tools/shared/common
            endif
        endif
    endif
endif
endif

ifeq ($(BOARD_USE_PLAYREADY), 1)
    LOCAL_CFLAGS += -DBOARD_USE_PLAYREADY=${BOARD_USE_PLAYREADY}
    LOCAL_CFLAGS += -DBOARD_PLAYREADY_USE_SECUREOS=${BOARD_PLAYREADY_USE_SECUREOS}
    LOCAL_CFLAGS += -DBOARD_USE_PLAYREADY_NEW=${BOARD_USE_PLAYREADY_NEW}
else
    LOCAL_CFLAGS += -DBOARD_USE_PLAYREADY=0
    LOCAL_CFLAGS += -DBOARD_PLAYREADY_USE_SECUREOS=0
    LOCAL_CFLAGS += -DBOARD_USE_PLAYREADY_NEW=0
endif

ifeq ($(DBOARD_USE_PLAYREADY_NEW),1)
LOCAL_SHARED_LIBRARIES := libbinder libdrmframework
endif
######################################################################

LOCAL_CFLAGS += $(CDX_CFLAGS)

LOCAL_SHARED_LIBRARIES += libz
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libcdx_mov_parser

ifeq ($(TARGET_ARCH),arm)
    LOCAL_CFLAGS += -Wno-psabi
endif

include $(BUILD_STATIC_LIBRARY)


