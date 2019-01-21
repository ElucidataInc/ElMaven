TEMPLATE = lib

DESTDIR=$$top_builddir/libs/
MOC_DIR=$$top_builddir/tmp/crashhandler/breakpad
OBJECTS_DIR=$$top_builddir/tmp/crashhandler/breakpad


CONFIG += staticlib 


mac {
    
    INCLUDEPATH += src/src/ 

    HEADERS +=  src/src/client/mac/handler/exception_handler.h \
	            src/src/client/mac/crash_generation/crash_generation_client.h \
                src/src/client/mac/crash_generation/crash_generation_server.h \
                src/src/client/mac/crash_generation/client_info.h \
                src/src/client/mac/handler/minidump_generator.h \
                src/src/client/mac/handler/dynamic_images.h \
                src/src/client/mac/handler/breakpad_nlist_64.h \
                src/src/client/mac/handler/mach_vm_compat.h \
                src/src/client/minidump_file_writer.h \
                src/src/client/minidump_file_writer-inl.h \
                src/src/common/mac/macho_utilities.h \
                src/src/common/mac/byteswap.h \
                src/src/common/mac/MachIPC.h \
                src/src/common/mac/scoped_task_suspend-inl.h \
                src/src/common/mac/file_id.h \
                src/src/common/mac/macho_id.h \
                src/src/common/mac/macho_walker.h \
                src/src/common/mac/macho_utilities.h \
                src/src/common/mac/bootstrap_compat.h \
                src/src/common/mac/string_utilities.h \
                src/src/common/linux/linux_libc_support.h \
                src/src/common/string_conversion.h \
                src/src/common/md5.h \
                src/src/common/using_std_string.h \
                src/src/common/convert_UTF.h \
                src/src/common/scoped_ptr.h \
                src/src/google_breakpad/common/minidump_exception_mac.h \
                src/src/google_breakpad/common/breakpad_types.h \
                src/src/google_breakpad/common/minidump_format.h \
                src/src/google_breakpad/common/minidump_size.h \
                src/src/third_party/lss/linux_syscall_support.h

  SOURCES +=    src/src/client/mac/handler/exception_handler.cc \
                src/src/client/mac/crash_generation/crash_generation_client.cc \
                src/src/client/mac/crash_generation/crash_generation_server.cc \
                src/src/client/mac/handler/minidump_generator.cc \
                src/src/client/mac/handler/dynamic_images.cc \
                src/src/client/mac/handler/breakpad_nlist_64.cc \
                src/src/client/minidump_file_writer.cc \
                src/src/common/mac/macho_id.cc \
                src/src/common/mac/macho_walker.cc \
                src/src/common/mac/macho_utilities.cc \
                src/src/common/mac/string_utilities.cc \
                src/src/common/mac/file_id.cc \
                src/src/common/mac/MachIPC.mm \
                src/src/common/mac/bootstrap_compat.cc \
                src/src/common/md5.cc \
                src/src/common/string_conversion.cc \
                src/src/common/linux/linux_libc_support.cc \
                src/src/common/convert_UTF.c


  LIBS += -framework CoreServices
}
