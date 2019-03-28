TEMPLATE = lib

DESTDIR=$$top_builddir/libs/
MOC_DIR=$$top_builddir/tmp/crashhandler/breakpad
OBJECTS_DIR=$$top_builddir/tmp/crashhandler/breakpad


CONFIG += staticlib 
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += src/src/

win32 {


    HEADERS +=  src/src/common/windows/string_utils-inl.h \
                src/src/common/windows/guid_string.h \
                src/src/client/windows/handler/exception_handler.h \
                src/src/client/windows/common/ipc_protocol.h \
                src/src/google_breakpad/common/minidump_format.h \
                src/src/google_breakpad/common/breakpad_types.h \
                src/src/client/windows/crash_generation/crash_generation_client.h \
                src/src/common/scoped_ptr.h \
                src/src/common/windows/http_upload.h

    SOURCES +=  src/src/client/windows/handler/exception_handler.cc \
                src/src/client/windows/crash_generation/client_info.cc \
                src/src/client/windows/crash_generation/minidump_generator.cc \
                src/src/common/windows/string_utils.cc \
                src/src/common/windows/guid_string.cc \
                src/src/client/windows/crash_generation/crash_generation_client.cc \
                src/src/client/windows/crash_generation/crash_generation_server.cc \
                src/src/common/windows/http_upload.cc
}

mac {
    

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

linux {

    SOURCES +=  src/src/client/linux/crash_generation/crash_generation_client.cc \
                src/src/client/linux/handler/exception_handler.cc \
                src/src/client/linux/handler/minidump_descriptor.cc \
                src/src/client/linux/minidump_writer/minidump_writer.cc \
                src/src/client/linux/minidump_writer/linux_dumper.cc \
                src/src/client/linux/minidump_writer/linux_ptrace_dumper.cc \
                src/src/client/linux/microdump_writer/microdump_writer.cc \
                src/src/client/linux/dump_writer_common/ucontext_reader.cc \
                src/src/client/linux/dump_writer_common/thread_info.cc \
                src/src/client/linux/log/log.cc \
                src/src/client/minidump_file_writer.cc \
                src/src/common/linux/linux_libc_support.cc \
                src/src/common/linux/file_id.cc \
                src/src/common/linux/memory_mapped_file.cc \
                src/src/common/linux/safe_readlink.cc \
                src/src/common/linux/guid_creator.cc \
                src/src/common/linux/elfutils.cc \
                src/src/common/linux/http_upload.cc \
                src/src/common/string_conversion.cc \
                src/src/common/convert_UTF.c


    HEADERS +=  src/src/client/linux/minidump_writer/cpu_set.h \
                src/src/client/linux/minidump_writer/proc_cpuinfo_reader.h \
                src/src/client/linux/handler/exception_handler.h \
                src/src/client/linux/crash_generation/crash_generation_client.h \
                src/src/client/linux/handler/minidump_descriptor.h \
                src/src/client/linux/minidump_writer/minidump_writer.h \
                src/src/client/linux/minidump_writer/line_reader.h \
                src/src/client/linux/minidump_writer/linux_dumper.h \
                src/src/client/linux/minidump_writer/linux_ptrace_dumper.h \
                src/src/client/linux/minidump_writer/directory_reader.h \
                src/src/client/linux/microdump_writer/microdump_writer.h \
                src/src/client/linux/dump_writer_common/ucontext_reader.h \
                src/src/client/linux/dump_writer_common/thread_info.h \
                src/src/client/linux/log/log.h \
                src/src/client/minidump_file_writer-inl.h \
                src/src/client/minidump_file_writer.h \
                src/src/common/linux/linux_libc_support.h \
                src/src/common/linux/http_upload.h \
                src/src/common/linux/eintr_wrapper.h \
                src/src/common/linux/ignore_ret.h \
                src/src/common/linux/file_id.h \
                src/src/common/linux/memory_mapped_file.h \
                src/src/common/linux/safe_readlink.h \
                src/src/common/linux/guid_creator.h \
                src/src/common/linux/elfutils.h \
                src/src/common/linux/elfutils-inl.h \
                src/src/common/linux/elf_gnu_compat.h \
                src/src/common/using_std_string.h \
                src/src/common/memory.h \
                src/src/common/basictypes.h \
                src/src/common/memory_range.h \
                src/src/common/string_conversion.h \
                src/src/common/convert_UTF.h \
                src/src/google_breakpad/common/minidump_format.h \
                src/src/google_breakpad/common/minidump_size.h \
                src/src/google_breakpad/common/breakpad_types.h \
                src/common/scoped_ptr.h \
                src/src/third_party/lss/linux_syscall_support.h
}
