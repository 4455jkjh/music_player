# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := audio
LOCAL_SRC_FILES := native-audio-jni.c ffmpeg.c onload.c
LOCAL_ARM_MODE := arm
# for native audio
LOCAL_LDLIBS    += -lOpenSLES -llog 
ifeq ($(TARGET_ARCH) , arm)
LOCAL_LDLIBS += -lavcodec -lavformat -lavutil -lswresample
else
LOCAL_LDLIBS += -lffmpeg
endif
include $(BUILD_SHARED_LIBRARY)


#nclude $(CLEAR_VARS)
#OCAL_MODULE    := ffmpeg
#OCAL_SRC_FILES := $(TARGET_ARCH)/libffmpeg.so
#nclude $(PREBUILT_SHARED_LIBRARY)
