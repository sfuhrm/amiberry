LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/src \
                    $(LOCAL_PATH)/src/osdep \
                    $(LOCAL_PATH)/src/threaddep \
                    $(LOCAL_PATH)/src/include \
                    $(LOCAL_PATH)/src/archivers \
                    $(LOCAL_PATH)/guisan-dev/include \
                    $(LOCAL_PATH)/$(SDL_PATH)/include

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS := -DCPU_arm -DARM_HAS_DIV -DARMV6T2 -DARMV6_ASSEMBLY -DANDROIDSDL -DAMIBERRY -D_REENTRANT
else ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
LOCAL_CFLAGS := -DCPU_AARCH64 -DANDROIDSDL -DAMIBERRY
endif

LOCAL_CPPFLAGS := -std=gnu++14  -pipe -frename-registers -Wno-shift-overflow -Wno-narrowing -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed
LOCAL_LDFLAGS += -fuse-ld=gold -Lguisan-dev/lib

# Add your application source files here...
LOCAL_SRC_FILES := src/archivers/7z/BraIA64.c \
                   	src/archivers/7z/Delta.c \
                   	src/archivers/7z/Sha256.c \
                   	src/archivers/7z/XzCrc64.c \
                   	src/archivers/7z/XzDec.c \
                   	src/akiko.cpp \
                    src/ar.cpp \
                    src/aros.rom.cpp \
                    src/audio.cpp \
                    src/autoconf.cpp \
                    src/blitfunc.cpp \
                    src/blittable.cpp \
                    src/blitter.cpp \
                    src/blkdev.cpp \
                    src/blkdev_cdimage.cpp \
                    src/bsdsocket.cpp \
                    src/calc.cpp \
                    src/cd32_fmv.cpp \
                    src/cd32_fmv_genlock.cpp \
                    src/cdrom.cpp \
                    src/cfgfile.cpp \
                    src/cia.cpp \
                    src/crc32.cpp \
                    src/custom.cpp \
                    src/devices.cpp \
                    src/disk.cpp \
                    src/diskutil.cpp \
                    src/dlopen.cpp \
                    src/drawing.cpp \
                    src/events.cpp \
                    src/expansion.cpp \
                    src/fdi2raw.cpp \
                    src/filesys.cpp \
                    src/flashrom.cpp \
                    src/fpp.cpp \
                    src/fsdb.cpp \
                    src/fsdb_unix.cpp \
                    src/fsusage.cpp \
                    src/gayle.cpp \
                    src/gfxboard.cpp \
                    src/gfxutil.cpp \
                    src/hardfile.cpp \
                    src/hrtmon.rom.cpp \
                    src/ide.cpp \
                    src/inputdevice.cpp \
                    src/keybuf.cpp \
                    src/main.cpp \
                    src/memory.cpp \
                    src/native2amiga.cpp \
                    src/rommgr.cpp \
                    src/rtc.cpp \
                    src/savestate.cpp \
                    src/scsi.cpp \
                    src/statusline.cpp \
                    src/traps.cpp \
                    src/uaelib.cpp \
                    src/uaeresource.cpp \
                    src/zfile.cpp \
                    src/zfile_archive.cpp \
                    src/archivers/7z/7zAlloc.cpp \
                    src/archivers/7z/7zBuf.cpp \
                    src/archivers/7z/7zCrc.cpp \
                    src/archivers/7z/7zCrcOpt.cpp \
                    src/archivers/7z/7zDec.cpp \
                    src/archivers/7z/7zIn.cpp \
                    src/archivers/7z/7zStream.cpp \
                    src/archivers/7z/Bcj2.cpp \
                    src/archivers/7z/Bra.cpp \
                    src/archivers/7z/Bra86.cpp \
                    src/archivers/7z/LzmaDec.cpp \
                    src/archivers/7z/Lzma2Dec.cpp \
                    src/archivers/7z/Xz.cpp \
                    src/archivers/dms/crc_csum.cpp \
                    src/archivers/dms/getbits.cpp \
                    src/archivers/dms/maketbl.cpp \
                    src/archivers/dms/pfile.cpp \
                    src/archivers/dms/tables.cpp \
                    src/archivers/dms/u_deep.cpp \
                    src/archivers/dms/u_heavy.cpp \
                    src/archivers/dms/u_init.cpp \
                    src/archivers/dms/u_medium.cpp \
                    src/archivers/dms/u_quick.cpp \
                    src/archivers/dms/u_rle.cpp \
                    src/archivers/lha/crcio.cpp \
                    src/archivers/lha/dhuf.cpp \
                    src/archivers/lha/header.cpp \
                    src/archivers/lha/huf.cpp \
                    src/archivers/lha/larc.cpp \
                    src/archivers/lha/lhamaketbl.cpp \
                    src/archivers/lha/lharc.cpp \
                    src/archivers/lha/shuf.cpp \
                    src/archivers/lha/slide.cpp \
                    src/archivers/lha/uae_lha.cpp \
                    src/archivers/lha/util.cpp \
                    src/archivers/lzx/unlzx.cpp \
                    src/archivers/mp2/kjmp2.cpp \
                    src/archivers/wrp/warp.cpp \
                    src/archivers/zip/unzip.cpp \
                    src/caps/caps_amiberry.cpp \
                    src/machdep/support.cpp \
                    src/osdep/bsdsocket_host.cpp \
                    src/osdep/cda_play.cpp \
                    src/osdep/charset.cpp \
                    src/osdep/fsdb_host.cpp \
                    src/osdep/amiberry_hardfile.cpp \
                    src/osdep/keyboard.cpp \
                    src/osdep/mp3decoder.cpp \
                    src/osdep/picasso96.cpp \
                    src/osdep/writelog.cpp \
                    src/osdep/amiberry.cpp \
                    src/osdep/amiberry_filesys.cpp \
                    src/osdep/amiberry_input.cpp \
                    src/osdep/amiberry_gfx.cpp \
                    src/osdep/amiberry_gui.cpp \
                    src/osdep/amiberry_rp9.cpp \
                    src/osdep/amiberry_mem.cpp \
                    src/osdep/amiberry_whdbooter.cpp \
                    src/osdep/sigsegv_handler.cpp \
                    src/sounddep/sound.cpp \
                    src/osdep/gui/UaeRadioButton.cpp \
                    src/osdep/gui/UaeDropDown.cpp \
                    src/osdep/gui/UaeCheckBox.cpp \
                    src/osdep/gui/UaeListBox.cpp \
                    src/osdep/gui/InGameMessage.cpp \
                    src/osdep/gui/SelectorEntry.cpp \
                    src/osdep/gui/ShowHelp.cpp \
                    src/osdep/gui/ShowMessage.cpp \
                    src/osdep/gui/SelectFolder.cpp \
                    src/osdep/gui/SelectFile.cpp \
                    src/osdep/gui/CreateFilesysHardfile.cpp \
                    src/osdep/gui/EditFilesysVirtual.cpp \
                    src/osdep/gui/EditFilesysHardfile.cpp \
                    src/osdep/gui/PanelAbout.cpp \
                    src/osdep/gui/PanelPaths.cpp \
                    src/osdep/gui/PanelQuickstart.cpp \
                    src/osdep/gui/PanelConfig.cpp \
                    src/osdep/gui/PanelCPU.cpp \
                    src/osdep/gui/PanelChipset.cpp \
                    src/osdep/gui/PanelCustom.cpp \
                    src/osdep/gui/PanelROM.cpp \
                    src/osdep/gui/PanelRAM.cpp \
                    src/osdep/gui/PanelFloppy.cpp \
                    src/osdep/gui/PanelHD.cpp \
                    src/osdep/gui/PanelInput.cpp \
                    src/osdep/gui/PanelDisplay.cpp \
                    src/osdep/gui/PanelSound.cpp \
                    src/osdep/gui/PanelMisc.cpp \
                    src/osdep/gui/PanelSavestate.cpp \
                    src/osdep/gui/main_window.cpp \
                    src/osdep/gui/Navigation.cpp \
                    src/osdep/gui/androidsdl_event.cpp \
                    src/osdep/gui/PanelOnScreen.cpp

ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
LOCAL_SRC_FILES += src/osdep/aarch64_helper.s
else ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_SRC_FILES += src/osdep/arm_helper.s
endif

LOCAL_SRC_FILES += src/newcpu.cpp \
                    src/newcpu_common.cpp \
                    src/readcpu.cpp \
                    src/cpudefs.cpp \
                    src/cpustbl.cpp \
                    src/cpuemu_0.cpp \
                    src/cpuemu_4.cpp \
                    src/cpuemu_11.cpp \
                    src/cpuemu_40.cpp \
                    src/cpuemu_44.cpp \
                    src/jit/compemu.cpp \
                    src/jit/compstbl.cpp \
                    src/jit/compemu_fpp.cpp \
                    src/jit/compemu_support.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf SDL2_mixer xml2 mpeg2

LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -llog -lz

include $(BUILD_SHARED_LIBRARY)