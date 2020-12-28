// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file File.hpp
* @author JXMaster
* @date 2020/9/27
*/
#pragma once
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include <Runtime/TSAssert.hpp>
#include <Runtime/IFile.hpp>
#include <Runtime/IAsyncFile.hpp>
#include <Runtime/Interface.hpp>
#include <Runtime/IFileIterator.hpp>
#include <Runtime/TSAssert.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <aio.h>

namespace Luna
{
	class File final : public IFile
	{
	public:
		lucid("{5dd7c162-8bc3-4f9a-8324-44372d2026a9}");
		luiimpl(File, IFile, IStream, IObject);
		lutsassert_lock();

        EStreamFlag m_flags;
        int m_fd;

		File() :
            m_fd(-1){}
		~File()
        {
            if(m_fd != -1)
            {
                ::close(m_fd);
                m_fd = -1;
            }
        }

		virtual EStreamFlag flags() override
        {
            return m_flags;
        }
		virtual RV read(void* buffer, usize size, usize* read_bytes) override;
		virtual RV write(const void* buffer, usize size, usize* write_bytes) override;
		virtual R<u64> size() override;
		virtual RV set_size(u64 sz) override;
		virtual R<u64> tell() override;
		virtual RV seek(i64 offset, ESeekMode mode) override;
		virtual RV flush() override;
	};

	class BufferedFile : public IFile
	{
	public:
		lucid("{60414fd8-de96-46a7-b5f0-1f98971751fa}");
		luiimpl(BufferedFile, IFile, IStream, IObject);
		lutsassert_lock();
        
        EStreamFlag m_flags;
        FILE* m_file;

		BufferedFile() :
            m_file(nullptr) {}
		~BufferedFile()
        {
            if(m_file)
            {
                fclose(m_file);
                m_file = nullptr;
            }
        }
        
		virtual EStreamFlag flags() override
        {
            return m_flags;
        }
		virtual RV read(void* buffer, usize size, usize* read_bytes) override;
		virtual RV write(const void* buffer, usize size, usize* write_bytes) override;
		virtual R<u64> size() override;
		virtual RV set_size(u64 sz) override;
		virtual R<u64> tell() override;
		virtual RV seek(i64 offset, ESeekMode mode) override;
		virtual RV flush() override;
	};

    class AsyncFileRequest final : public IAsyncFileRequest
    {
    public:
        lucid("{888190c6-63cd-4661-b8ba-df754ef9b183}");
        luiimpl(AsyncFileRequest, IAsyncFileRequest, IWaitable, IObject);
        struct aiocb m_data;
        
        virtual RV wait() override;
        virtual RV try_wait() override;
        virtual R<usize> transferred_size() override;
    };

    class AsyncFile final : public IAsyncFile
    {
    public:
        lucid("{{0197c08e-79e9-43d0-a3d4-d73f0df17360}}");
        luiimpl(AsyncFile, IAsyncFile, IObject);
        int m_fd;
        
        virtual RP<IAsyncFileRequest> read(void* buffer, usize size, u64 file_offset) override;
        virtual RP<IAsyncFileRequest> write(const void* buffer, usize size, u64 file_offset) override;
        virtual R<u64> size() override;
    };

	class FileIterator final : public IFileIterator
	{
	public:
		lucid("{3d7b9537-8936-406f-9722-218663857583}");
		luiimpl(FileIterator, IFileIterator, IObject);
		lutsassert_lock();
        
        DIR* m_dir;
        struct dirent* m_dirent;

		FileIterator() :
            m_dir(nullptr),
            m_dirent(nullptr) {}
		~FileIterator()
        {
            if(m_dir)
            {
                closedir(m_dir);
                m_dir = nullptr;
            }
        }

		virtual bool valid() override
        {
            return m_dirent != nullptr;
        }

		virtual const c8* filename() override
        {
            if(valid())
            {
                return m_dirent->d_name;
            }
            return nullptr;
        }

		virtual EFileAttributeFlag attribute() override
        {
            if(valid())
            {
                EFileAttributeFlag flags = EFileAttributeFlag::none;
                if(DT_BLK & m_dirent->d_type)
                {
                    flags |= EFileAttributeFlag::block_special;
                }
                if(DT_CHR & m_dirent->d_type)
                {
                    flags |= EFileAttributeFlag::character_special;
                }
                if(DT_DIR & m_dirent->d_type)
                {
                    flags |= EFileAttributeFlag::directory;
                }
                return flags;
            }
            return EFileAttributeFlag::none;
        }

		virtual bool move_next() override
        {
            if(valid())
            {
                m_dirent = ::readdir(m_dir);
            }
            return valid();
        }
	};

	void file_init();
}

#endif
