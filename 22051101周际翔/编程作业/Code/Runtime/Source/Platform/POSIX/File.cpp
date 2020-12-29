// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file File.cpp
* @author JXMaster
* @date 2020/9/27
*/
#include <Runtime/PlatformDefines.hpp>
#ifdef LUNA_PLATFORM_POSIX

#define LUNA_RUNTIME_API LUNA_EXPORT

#include "File.hpp"
#include <Runtime/Unicode.hpp>
#include <Runtime/File.hpp>
#include <Runtime/Error.hpp>

#include <libgen.h>
#include <errno.h>

#ifdef LUNA_PLATFORM_MACOS
#include <libproc.h>
#endif

namespace Luna
{
    RV File::read(void* buffer, usize size, usize* read_bytes)
    {
        lutsassert();
        usize sz = ::read(m_fd, buffer, size);
        if(sz == -1)
        {
            if(read_bytes)
            {
                *read_bytes = 0;
            }
            return e_bad_system_call;
        }
        if(read_bytes)
        {
            *read_bytes = sz;
        }
        return s_ok;
    }
    RV File::write(const void* buffer, usize size, usize* write_bytes)
    {
        lutsassert();
        usize sz = ::write(m_fd, buffer, size);
        if(sz == -1)
        {
            if(write_bytes)
            {
                *write_bytes = 0;
            }
            return e_bad_system_call;
        }
        if(write_bytes)
        {
            *write_bytes = sz;
        }
        return s_ok;
    }
    R<u64> File::size()
    {
        struct stat stat;
        if(!fstat(m_fd, &stat))
        {
            return R<u64>::success(stat.st_size);
        }
        return R<u64>::failure(e_bad_system_call);
    }
    RV File::set_size(u64 sz)
    {
        lutsassert();
        return ftruncate(m_fd, sz) ? e_bad_system_call : s_ok;
    }
    R<u64> File::tell()
    {
        lutsassert();
        
        off_t r = lseek(m_fd, 0, SEEK_CUR);
        if(r == (off_t)-1)
        {
            return R<u64>::failure(e_bad_system_call);
        }
        return R<u64>::success((u64)r);
    }
    RV File::seek(i64 offset, ESeekMode mode)
    {
        lutsassert();
        int origin;
        switch(mode)
        {
        case ESeekMode::begin:
            origin = SEEK_SET;
            break;
        case ESeekMode::current:
            origin = SEEK_CUR;
            break;
        case ESeekMode::end:
            origin = SEEK_END;
            break;
        }
        off_t r = lseek(m_fd, offset, origin);
        if(r == (off_t)-1)
        {
            return e_bad_system_call;
        }
        return s_ok;
    }
    RV File::flush()
    {
        lutsassert();
        int r = fsync(m_fd);
        return r == 0 ? s_ok : e_bad_system_call;
    }
	RV BufferedFile::read(void *buffer, usize size, usize *read_bytes)
    {
        lutsassert();
        usize sz = fread(buffer, 1, size, m_file);
        if(read_bytes)
        {
            *read_bytes = sz;
        }
        if(sz != size)
        {
            if(feof(m_file))
            {
                clearerr(m_file);
                return s_ok;
            }
            clearerr(m_file);
            return e_bad_system_call;
        }
        return s_ok;
    }
    RV BufferedFile::write(const void* buffer, usize size, usize* write_bytes)
    {
        lutsassert();
        usize sz = fwrite(buffer, 1, size, m_file);
        if(write_bytes)
        {
            *write_bytes = sz;
        }
        if(sz != size)
        {
            clearerr(m_file);
            return e_bad_system_call;
        }
        return s_ok;
    }
    R<u64> BufferedFile::size()
    {
        lutsassert();
        int fd = fileno(m_file);
        struct stat stat;
        if(!fstat(fd, &stat))
        {
            return R<u64>::success(stat.st_size);
        }
        return R<u64>::failure(e_bad_system_call);
    }
    RV BufferedFile::set_size(u64 sz)
    {
        lutsassert();
        int fd = fileno(m_file);
        return ftruncate(fd, sz) ? e_bad_system_call : s_ok;
    }
    R<u64> BufferedFile::tell()
    {
        lutsassert();
        long r = ftell(m_file);
        if(r < 0)
        {
            clearerr(m_file);
            return R<u64>::failure(e_bad_system_call);
        }
        return R<u64>::success((u64)r);
    }
    RV BufferedFile::seek(i64 offset, ESeekMode mode)
    {
        lutsassert();
        int origin;
        switch(mode)
        {
        case ESeekMode::begin:
            origin = SEEK_SET;
            break;
        case ESeekMode::current:
            origin = SEEK_CUR;
            break;
        case ESeekMode::end:
            origin = SEEK_END;
            break;
        }
        if(fseek(m_file, offset, origin))
        {
            clearerr(m_file);
            return e_bad_system_call;
        }
        return s_ok;
    }
    RV BufferedFile::flush()
    {
        lutsassert();
        if(fflush(m_file))
        {
            clearerr(m_file);
            return e_bad_system_call;
        }
        return s_ok;
    }

    RV AsyncFileRequest::wait()
    {
        struct aiocb* d = &m_data;
        int r = aio_suspend(&d, 1, NULL);
        return r == 0 ? s_ok : e_bad_system_call;
    }

    RV AsyncFileRequest::try_wait()
    {
        int err = aio_error(&m_data);
        if(err == 0)
        {
            return s_ok;
        }
        if(err == EINPROGRESS)
        {
            return e_timeout;
        }
        if(err == ECANCELED)
        {
            return e_user_canceled;
        }
        return e_bad_system_call;
    }

    R<usize> AsyncFileRequest::transferred_size()
    {
        auto rv = try_wait();
        if(failed(rv))
        {
            return rv.result();
        }
        return R<usize>::success(aio_return(&m_data));
    }

    RP<IAsyncFileRequest> AsyncFile::read(void* buffer, usize size, u64 file_offset)
    {
        struct aiocb rd;
        bzero(&rd, sizeof(rd));
        rd.aio_buf = buffer;
        rd.aio_nbytes = size;
        rd.aio_fildes = m_fd;
        rd.aio_offset = file_offset;
        int ret = aio_read(&rd);
        if(ret < 0)
        {
            return e_bad_system_call;
        }
        P<AsyncFileRequest> req = memnewref<AsyncFileRequest>();
        req->m_data = rd;
        return P<IAsyncFileRequest>(req);
    }

    RP<IAsyncFileRequest> AsyncFile::write(const void* buffer, usize size, u64 file_offset)
    {
        struct aiocb rd;
        bzero(&rd, sizeof(rd));
        rd.aio_buf = const_cast<void*>(buffer);
        rd.aio_nbytes = size;
        rd.aio_fildes = m_fd;
        rd.aio_offset = file_offset;
        int ret = aio_write(&rd);
        if(ret < 0)
        {
            return e_bad_system_call;
        }
        P<AsyncFileRequest> req = memnewref<AsyncFileRequest>();
        req->m_data = rd;
        return P<IAsyncFileRequest>(req);
    }

    R<u64> AsyncFile::size()
    {
        struct stat stat;
        if(!fstat(m_fd, &stat))
        {
            return R<u64>::success(stat.st_size);
        }
        return R<u64>::failure(e_bad_system_call);
    }
    
    c8 g_process_path[1024];
    
    void file_init()
    {
#ifdef LUNA_PLATFORM_LINUX
        char path[1024];
        luassert_always(readlink("/proc/self/exe", path, 1024) != -1);
        char* dir = dirname(path);
        strlcpy(g_process_path, dir, 1024);
#else
        pid_t pid = getpid();
        int ret = proc_pidpath(pid, g_process_path, sizeof(g_process_path));
        luassert_always(ret > 0);
#endif
    }
    
    LUNA_RUNTIME_API RP<IFile> platform_open_file(const c8* filename, EFileOpenFlag flags, EFileCreationMode creation)
    {
        lucheck(filename);
        if((flags & EFileOpenFlag::user_buffering) != EFileOpenFlag::none)
        {
            // use buffered version.
            const char* mode;
            FILE* f = NULL;
            errno_t err;
            if (((flags & EFileOpenFlag::read) != EFileOpenFlag::none) && ((flags & EFileOpenFlag::write) != EFileOpenFlag::none))
            {
                // update mode.
                switch (creation)
                {
                case EFileCreationMode::create_always:
                    mode = "w+b";
                    f = fopen(filename, mode);
                    break;
                case EFileCreationMode::create_new:
                    if (succeeded(platform_file_attribute(filename)))
                    {
                        return e_already_exists;
                    }
                    mode = "w+b";
                    f = fopen(filename, mode);
                    break;
                case EFileCreationMode::open_always:
                    if (succeeded(platform_file_attribute(filename)))
                    {
                        mode = "r+b";
                        f = fopen(filename, mode);
                    }
                    else
                    {
                        mode = "w+b";
                         f = fopen(filename, mode);
                    }
                    break;
                case EFileCreationMode::open_existing:
                    mode = "r+b";
                    f = fopen(filename, mode);
                    break;
                case EFileCreationMode::open_existing_as_new:
                    if (succeeded(platform_file_attribute(filename)))
                    {
                        mode = "w+b";
                        f = fopen(filename, mode);
                    }
                    else
                    {
                        return e_not_found;
                    }
                    break;
                default:
                    lupanic();
                    break;
                }
            }
            else if (((flags & EFileOpenFlag::read) != EFileOpenFlag::none))
            {
                // read only mode,
                switch (creation)
                {
                case EFileCreationMode::create_always:
                case EFileCreationMode::create_new:
                case EFileCreationMode::open_existing_as_new:
                case EFileCreationMode::open_always:
                    return e_not_supported;    // Creates a new empty file and read-only from it has no meaning.
                    break;
                case EFileCreationMode::open_existing:
                    mode = "rb";
                    f = fopen(filename, mode);
                    break;
                default:
                    lupanic();
                    break;
                }
            }
            else if (((flags & EFileOpenFlag::write) != EFileOpenFlag::none))
            {
                // write only mode.
                switch (creation)
                {
                case EFileCreationMode::create_always:
                    mode = "wb";
                    f = fopen(filename, mode);
                    break;
                case EFileCreationMode::create_new:
                    if (succeeded(platform_file_attribute(filename)))
                    {
                        return e_already_exists;
                    }
                    mode = "wb";
                    f = fopen(filename, mode);
                    break;
                case EFileCreationMode::open_always:
                    if (succeeded(platform_file_attribute(filename)))
                    {
                        mode = "r+b";
                        f = fopen(filename, mode);
                    }
                    else
                    {
                        mode = "wb";
                        f = fopen(filename, mode);
                    }
                    break;
                case EFileCreationMode::open_existing:
                    mode = "r+b";
                    f = fopen(filename, mode);
                    break;
                case EFileCreationMode::open_existing_as_new:
                    if (succeeded(platform_file_attribute(filename)))
                    {
                        mode = "wb";
                        f = fopen(filename, mode);
                    }
                    else
                    {
                        return e_not_found;
                    }
                    break;
                default:
                    lupanic();
                    break;
                }
            }
            if (!f)
            {
                err = errno;
                switch (err)
                {
                case EPERM:
                    return e_access_denied;
                case ENOENT:
                    return e_not_found;
                default:
                    get_error().set(e_bad_system_call, "fopen failed with err code %d", err);
                    return e_user_failure;
                }
            }
            P<BufferedFile> file = memnewref<BufferedFile>();
            file->m_file = f;
            if ((flags & EFileOpenFlag::read) != EFileOpenFlag::none)
            {
                file->m_flags |= EStreamFlag::readable;
            }
            if ((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
            {
                file->m_flags |= EStreamFlag::writable;
                file->m_flags |= EStreamFlag::resizable;
            }
            file->m_flags |= EStreamFlag::tellable;
            file->m_flags |= EStreamFlag::sizable;
            file->m_flags |= EStreamFlag::seekable;
            file->m_flags |= EStreamFlag::buffered;
            return P<IFile>(file);
        }
        else
        {
            int f = 0;
            if((flags & EFileOpenFlag::read) != EFileOpenFlag::none)
            {
                if((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
                {
                    f = O_RDWR;
                }
                else
                {
                    f = O_RDONLY;
                }
            }
            else
            {
                if((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
                {
                    f = O_WRONLY;
                }
                else
                {
                    return e_bad_arguments;
                }
            }
            int fd;
            switch(creation)
            {
            case EFileCreationMode::create_always:
                f |= O_CREAT | O_TRUNC;
                fd = open(filename, f, 0666);
                break;
            case EFileCreationMode::create_new:
                if(succeeded(platform_file_attribute(filename)))
                {
                    return e_already_exists;
                }
                f |= O_CREAT;
                fd = open(filename, f, 0666);
                break;
            case EFileCreationMode::open_always:
                f |= O_CREAT;
                fd = open(filename, f, 0666);
                break;
            case EFileCreationMode::open_existing:
                fd = open(filename, f, 0);
                break;
            case EFileCreationMode::open_existing_as_new:
                f |= O_TRUNC;
                fd = open(filename, f, 0);
                break;
            }
            if(fd == -1)
            {
                errno_t err = errno;
                switch (err)
                {
                case EPERM:
                    return e_access_denied;
                case ENOENT:
                    return e_not_found;
                default:
                    get_error().set(e_bad_system_call, "fopen failed with err code %d", err);
                    return e_user_failure;
                }
            }
            P<File> file = memnewref<File>();
            file->m_fd = fd;
            file->m_flags = EStreamFlag::tellable | EStreamFlag::sizable | EStreamFlag::seekable;
            if ((flags & EFileOpenFlag::read) != EFileOpenFlag::none)
            {
                file->m_flags |= EStreamFlag::readable;
            }
            if ((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
            {
                file->m_flags |= EStreamFlag::writable;
                file->m_flags |= EStreamFlag::resizable;
            }
            return P<IFile>(file);
        }
    }

    LUNA_RUNTIME_API RP<IAsyncFile> platform_open_file_async(const c8* path, EFileOpenFlag flags, EFileCreationMode creation)
    {
        if((flags & EFileOpenFlag::user_buffering) != EFileOpenFlag::none)
        {
            return e_bad_arguments;
        }
        int f = 0;
        if((flags & EFileOpenFlag::read) != EFileOpenFlag::none)
        {
            if((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
            {
                f = O_RDWR;
            }
            else
            {
                f = O_RDONLY;
            }
        }
        else
        {
            if((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
            {
                f = O_WRONLY;
            }
            else
            {
                return e_bad_arguments;
            }
        }
        int fd;
        switch(creation)
        {
        case EFileCreationMode::create_always:
            f |= O_CREAT | O_TRUNC;
            fd = open(path, f, 0666);
            break;
        case EFileCreationMode::create_new:
            if(succeeded(platform_file_attribute(path)))
            {
                return e_already_exists;
            }
            f |= O_CREAT;
            fd = open(path, f, 0666);
            break;
        case EFileCreationMode::open_always:
            f |= O_CREAT;
            fd = open(path, f, 0666);
            break;
        case EFileCreationMode::open_existing:
            fd = open(path, f, 0);
            break;
        case EFileCreationMode::open_existing_as_new:
            f |= O_TRUNC;
            fd = open(path, f, 0);
            break;
        }
        if(fd == -1)
        {
            errno_t err = errno;
            switch (err)
            {
            case EPERM:
                return e_access_denied;
            case ENOENT:
                return e_not_found;
            default:
                get_error().set(e_bad_system_call, "fopen failed with err code %d", err);
                return e_user_failure;
            }
        }
        P<AsyncFile> file = memnewref<AsyncFile>();
        file->m_fd = fd;
        return P<IAsyncFile>(file);
    }

    LUNA_RUNTIME_API R<FileAttribute> platform_file_attribute(const c8* filename)
    {
        struct stat s;
        int r = stat(filename, &s);
        if(r != 0)
        {
            return e_bad_system_call;
        }
        FileAttribute attribute;
        attribute.size = s.st_size;
        attribute.last_access_time = s.st_atime;
        attribute.last_write_time = s.st_mtime;
#ifdef LUNA_PLATFORM_MACOS
        attribute.creation_time = s.st_birthtime;
#else
        attribute.creation_time = 0;
#endif
        attribute.attributes = EFileAttributeFlag::none;
        if(S_ISDIR(s.st_mode))
        {
            attribute.attributes |= EFileAttributeFlag::directory;
        }
        if(S_ISCHR(s.st_mode))
        {
            attribute.attributes |= EFileAttributeFlag::character_special;
        }
        if(S_ISBLK(s.st_mode))
        {
            attribute.attributes |= EFileAttributeFlag::block_special;
        }
        return attribute;
    }

    LUNA_RUNTIME_API RV platform_copy_file(const c8* src_path, const c8* dest_path, bool fail_if_exists)
    {
        lucheck(src_path && dest_path);
        constexpr u64 max_buffer_sz = 1_mb;
        u8* buf = (u8*)memalloc(max_buffer_sz);
        lutry
        {
            lulet(from_file, platform_open_file(src_path, EFileOpenFlag::read, EFileCreationMode::open_existing));
            P<IFile> to_file;
            if(fail_if_exists)
            {
                luset(to_file, platform_open_file(dest_path, EFileOpenFlag::write, EFileCreationMode::create_new));
            }
            else
            {
                luset(to_file, platform_open_file(dest_path, EFileOpenFlag::write, EFileCreationMode::create_always));
            }
            lulet(copy_size, from_file->size());
            u64 sz = copy_size;
            
            while(sz)
            {
                usize copy_size_onetime = min<usize>(sz, max_buffer_sz);
                luexp(from_file->read(buf, copy_size_onetime, nullptr));
                luexp(to_file->write(buf, copy_size_onetime, nullptr));
                sz -= copy_size_onetime;
            }
        }
        lucatch
        {
            if(buf)
            {
                memfree(buf);
                buf = nullptr;
            }
            return lures;
        }
        if(buf)
        {
            memfree(buf);
            buf = nullptr;
        }
        return s_ok;
    }

    LUNA_RUNTIME_API RV platform_move_file(const c8* src_path, const c8* dest_path, bool allow_copy, bool fail_if_exists)
    {
        if(fail_if_exists && succeeded(platform_file_attribute(dest_path)))
        {
            return e_already_exists;
        }
        int res = ::rename(src_path, dest_path);
        if(res != 0 && allow_copy)
        {
            // Try to copy&delete.
            lutry
            {
                luexp(platform_copy_file(src_path, dest_path, fail_if_exists));
                luexp(platform_delete_file(src_path));
            }
            lucatchret;
        }
        else
        {
            return (res == 0) ? s_ok : e_bad_system_call;
        }
    }

    LUNA_RUNTIME_API RV platform_delete_file(const c8* path)
    {
        int res = ::remove(path);
        return (res == 0) ? s_ok : e_bad_system_call;
    }

    LUNA_RUNTIME_API RP<IFileIterator> platform_open_dir(const c8* path)
    {
        DIR* dir = ::opendir(path);
        if(dir == NULL)
        {
            errno_t err = errno;
            switch(err)
            {
            case EACCES:
                return e_access_denied;
            case EMFILE:
            case ENFILE:
                return e_busy;
            case ENOENT:
                return e_not_found;
            case ENOMEM:
                return e_bad_memory_alloc;
            case ENOTDIR:
                return e_not_directory;
            default:
                return e_bad_system_call;
            }
        }
        P<FileIterator> iter = memnewref<FileIterator>();
        iter->m_dir = dir;
        iter->m_dirent = ::readdir(dir);
        return P<IFileIterator>(iter);
    }

    LUNA_RUNTIME_API RV platform_create_dir(const c8* path)
    {
        int r = mkdir(path, 0755);
        if(r != 0)
        {
            errno_t err = errno;
            switch(errno)
            {
            case EACCES:
                return e_access_denied;
            case EEXIST:
                return e_already_exists;
            case ENAMETOOLONG:
                return e_data_too_long;
            case ENOENT:
                return e_not_found;
            case ENOTDIR:
                return e_not_directory;
            default:
                return e_bad_system_call;
            }
        }
        return s_ok;
    }

    LUNA_RUNTIME_API RV platform_remove_dir(const c8* path)
    {
        int r = rmdir(path);
        if(r != 0)
        {
            errno_t err = errno;
            switch(errno)
            {
            case EACCES:
                return e_access_denied;
            case EBUSY:
                return e_busy;
            case ENAMETOOLONG:
                return e_data_too_long;
            case ENOENT:
                return e_not_found;
            case ENOTDIR:
                return e_not_directory;
            default:
                return e_bad_system_call;
            }
        }
        return s_ok;
    }

    LUNA_RUNTIME_API u32 get_current_dir(u32 buffer_length, c8* buffer)
    {
        char* path = ::getcwd(nullptr, 0);
        u32 len = (u32)strlen(path);
        if(buffer && buffer_length)
        {
            strncpy(buffer, path, buffer_length);
        }
        ::free(path);
        return len;
    }

    LUNA_RUNTIME_API RV set_current_dir(const c8* path)
    {
        int r = ::chdir(path);
        if(r != 0)
        {
            errno_t err = errno;
            switch(errno)
            {
            case EACCES:
                return e_access_denied;
            case ENAMETOOLONG:
                return e_data_too_long;
            case ENOENT:
                return e_not_found;
            case ENOTDIR:
                return e_not_directory;
            default:
                return e_bad_system_call;
            }
        }
        return s_ok;
    }

    LUNA_RUNTIME_API const c8* get_process_path()
    {
        return g_process_path;
    }
}
#endif
