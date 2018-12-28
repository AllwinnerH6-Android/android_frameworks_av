LOCAL_PATH:= $(call my-dir)

#
# libmediaplayerservice
#

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../libcedarx/config.mk

# LOCAL_PROPRIETARY_MODULE is set true in cedarx/config.mk,
# so we should set false here
LOCAL_PROPRIETARY_MODULE := false

LOCAL_SRC_FILES:=               \
    ActivityManager.cpp         \
    MediaPlayerFactory.cpp      \
    MediaPlayerService.cpp      \
    MediaRecorderClient.cpp     \
    MetadataRetrieverClient.cpp \
    StagefrightRecorder.cpp     \
    TestPlayerStub.cpp          \

LOCAL_SHARED_LIBRARIES :=       \
    libaudioclient              \
    libbinder                   \
    libcamera_client            \
    libcrypto                   \
    libcutils                   \
    libgui                      \
    liblog                      \
    libdl                       \
    libhidlbase                 \
    libhidlmemory               \
    libmedia                    \
    libmedia_omx                \
    libmediaextractor           \
    libmediametrics             \
    libmediadrm                 \
    libmediametrics             \
    libmediautils               \
    libmemunreachable           \
    libpowermanager             \
    libstagefright              \
    libstagefright_foundation   \
    libstagefright_httplive     \
    libutils                    \
    android.hardware.media.omx@1.0 \
    libawplayer                 \
    libxplayer                  \
    libaw_output                \
    libawmetadataretriever      \

ifeq ($(TARGET_PLATFORM),homlet)
    LOCAL_SHARED_LIBRARIES += libisomountmanagerservice
endif

LOCAL_STATIC_LIBRARIES :=       \
    libstagefright_nuplayer     \
    libstagefright_rtsp         \
    libstagefright_timedtext    \

LOCAL_EXPORT_SHARED_LIBRARY_HEADERS := libmedia

LOCAL_C_INCLUDES :=                                                 \
    frameworks/av/media/libstagefright/include               \
    frameworks/av/media/libstagefright/rtsp                  \
    frameworks/av/media/libstagefright/wifi-display          \
    frameworks/av/media/libstagefright/webm                  \
    $(LOCAL_PATH)/include/media                              \
    frameworks/av/include/camera                             \
    frameworks/native/include/media/openmax                  \
    frameworks/native/include/media/hardware                 \
    external/tremolo/Tremolo                                 \

LOCAL_C_INCLUDES +=          \
    $(TOP)/frameworks/av/media/libcedarx/android_adapter/awplayer/   \
    $(TOP)/frameworks/av/media/libcedarx/android_adapter/output/   \
    $(TOP)/frameworks/av/media/libcedarx/android_adapter/metadataretriever/       \
    $(TOP)/frameworks/av/media/libcedarc/include

ifeq ($(TARGET_PLATFORM),homlet)
LOCAL_C_INCLUDES += \
    $(TOP)/vendor/aw/homlet/framework/isomountmanager/include
endif

LOCAL_CFLAGS += -Werror -Wno-error=deprecated-declarations -Wall

ifeq ($(TARGET_PLATFORM),homlet)
LOCAL_CFLAGS += -DSUPPORT_BDMV
endif
LOCAL_MODULE:= libmediaplayerservice

LOCAL_32_BIT_ONLY := true

LOCAL_SANITIZE := cfi
LOCAL_SANITIZE_DIAG := cfi

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
