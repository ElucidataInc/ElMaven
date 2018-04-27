TEMPLATE = lib
DESTDIR=$$top_builddir/libs/
MOC_DIR=$$top_builddir/tmp/breakpad
OBJECTS_DIR=$$top_builddir/tmp/breakpad

CONFIG += staticlib

linux {

    INCLUDEPATH += src/

    SOURCES +=  src/client/linux/crash_generation/crash_generation_client.cc \
                src/client/linux/handler/exception_handler.cc \
                src/client/linux/handler/minidump_descriptor.cc \
                src/client/linux/minidump_writer/minidump_writer.cc \
                src/client/linux/minidump_writer/linux_dumper.cc \
                src/client/linux/minidump_writer/linux_ptrace_dumper.cc \
                src/client/linux/microdump_writer/microdump_writer.cc \
                src/client/linux/dump_writer_common/ucontext_reader.cc \
                src/client/linux/dump_writer_common/thread_info.cc \
                src/client/linux/log/log.cc \
                src/client/minidump_file_writer.cc \
                src/common/linux/linux_libc_support.cc \
                src/common/linux/file_id.cc \
                src/common/linux/memory_mapped_file.cc \
                src/common/linux/safe_readlink.cc \
                src/common/linux/guid_creator.cc \
                src/common/linux/elfutils.cc \
                src/common/string_conversion.cc \
                src/common/convert_UTF.c


    HEADERS +=  src/client/linux/minidump_writer/cpu_set.h \
                src/client/linux/minidump_writer/proc_cpuinfo_reader.h \
                src/client/linux/handler/exception_handler.h \
                src/client/linux/crash_generation/crash_generation_client.h \
                src/client/linux/handler/minidump_descriptor.h \
                src/client/linux/minidump_writer/minidump_writer.h \
                src/client/linux/minidump_writer/line_reader.h \
                src/client/linux/minidump_writer/linux_dumper.h \
                src/client/linux/minidump_writer/linux_ptrace_dumper.h \
                src/client/linux/minidump_writer/directory_reader.h \
                src/client/linux/microdump_writer/microdump_writer.h \
                src/client/linux/dump_writer_common/ucontext_reader.h \
                src/client/linux/dump_writer_common/thread_info.h \
                src/client/linux/log/log.h \
                src/client/minidump_file_writer-inl.h \
                src/client/minidump_file_writer.h \
                src/common/linux/linux_libc_support.h \
                src/common/linux/eintr_wrapper.h \
                src/common/linux/ignore_ret.h \
                src/common/linux/file_id.h \
                src/common/linux/memory_mapped_file.h \
                src/common/linux/safe_readlink.h \
                src/common/linux/guid_creator.h \
                src/common/linux/elfutils.h \
                src/common/linux/elfutils-inl.h \
                src/common/linux/elf_gnu_compat.h \
                src/common/using_std_string.h \
                src/common/memory.h \
                src/common/basictypes.h \
                src/common/memory_range.h \
                src/common/string_conversion.h \
                src/common/convert_UTF.h \
                src/google_breakpad/common/minidump_format.h \
                src/google_breakpad/common/minidump_size.h \
                src/google_breakpad/common/breakpad_types.h \
                src/common/scoped_ptr.h \
                src/third_party/lss/linux_syscall_support.h
}






