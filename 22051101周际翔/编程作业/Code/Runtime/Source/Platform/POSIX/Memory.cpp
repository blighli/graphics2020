// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Memory.cpp
* @author JXMaster
* @date 2020/9/22
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX
#define LUNA_RUNTIME_API LUNA_EXPORT

#include "Memory.hpp"
#include <sys/mman.h>
#include "Errno.hpp"
#include <Runtime/Error.hpp>
#include <pthread.h>
#include <stdlib.h>
#include <unordered_map>

namespace Luna
{
    LUNA_RUNTIME_API R<void*> virtual_reserve(void* address, usize size)
    {
        void* r = mmap(address, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if(r == MAP_FAILED)
        {
            errno_t err = errno;
            get_error().set(e_bad_system_call, u8"virtual_reserve failed with error code %d %s", err, display_errno(err));
            return e_user_failure;
        }
        return r;
    }
    LUNA_RUNTIME_API RV virtual_commit(void* address, usize size, EPageProtection protection)
    {
        int prot;
        switch (protection) {
            case EPageProtection::no_access:
                prot = PROT_NONE;
                break;
            case EPageProtection::read:
                prot = PROT_READ;
                break;
            case EPageProtection::read_write:
                prot = PROT_READ | PROT_WRITE;
                break;
            case EPageProtection::execute:
                prot = PROT_EXEC;
                break;
            case EPageProtection::execute_read:
                prot = PROT_READ | PROT_EXEC;
                break;
            case EPageProtection::execute_read_write:
                prot = PROT_READ | PROT_WRITE | PROT_EXEC;
                break;
            default:
                lupanic();
                break;
        }
        int res = mprotect(address, size, prot);
        if(res != 0)
        {
            errno_t err = errno;
            get_error().set(e_bad_system_call, u8"virtual_commit failed with error code %d %s", err, display_errno(err));
            return e_user_failure;
        }
        return s_ok;
    }
    LUNA_RUNTIME_API RV virtual_decommit(void* address, usize size)
    {
        int res = mprotect(address, size, PROT_NONE);
        if(res != 0)
        {
            errno_t err = errno;
            get_error().set(e_bad_system_call, u8"virtual_commit failed with error code %d %s", err, display_errno(err));
            return e_user_failure;
        }
        return s_ok;
    }
    LUNA_RUNTIME_API RV virtual_release(void* address, usize size)
    {
        int res = munmap(address, size);
        if(res != 0)
        {
            errno_t err = errno;
            get_error().set(e_bad_system_call, u8"virtual_release failed with error code %d %s", err, display_errno(err));
            return e_user_failure;
        }
        return s_ok;
    }
    LUNA_RUNTIME_API RV virtual_protect(void* address, usize size, EPageProtection protection)
    {
        int prot;
        switch (protection) {
            case EPageProtection::no_access:
                prot = PROT_NONE;
                break;
            case EPageProtection::read:
                prot = PROT_READ;
                break;
            case EPageProtection::read_write:
                prot = PROT_READ | PROT_WRITE;
                break;
            case EPageProtection::execute:
                prot = PROT_EXEC;
                break;
            case EPageProtection::execute_read:
                prot = PROT_READ | PROT_EXEC;
                break;
            case EPageProtection::execute_read_write:
                prot = PROT_READ | PROT_WRITE | PROT_EXEC;
                break;
            default:
                lupanic();
                break;
        }
        int res = mprotect(address, size, prot);
        if(res != 0)
        {
            errno_t err = errno;
            get_error().set(e_bad_system_call, u8"virtual_protect failed with error code %d %s", err, display_errno(err));
            return e_user_failure;
        }
    }

    /*
        Heap Allocation Strategy:
     
        * One Heap is composed by several Segments, each segment is 150MB on 32-bit build or 250MB on 64-bit build, and represents one continuious range on the virtual memory address space. Segment is reserved at first page allocation by calling `virtual_reserve`. One Heap can have 128 segments at most.
        * One Segment is composed by several Pages, each page is 64KB and can be commited to/decommited from the physical memory individually by calling `virtual_commit`.
        * One Page is then being devided into several Blocks, each block is 1KB. The blocks are used to fill the real allocator that we call Buckets. There are buckets with different size, for 32-bit builds, the mininum bucket allocates 8-byte memory and increases as times of 8 bytes, and for 64-bit builds, the mininum buckets allocate 16-bit memory and increases as times of 16 byte. On both platforms, the maxinum memory size one bucket can allocate is 512 byte.
     
        Once one allocation is issued, the system guides the allocation based on its size and alignment requirements:
        * If the size is less then or equal to 512 bytes, and the alignment is less or equal to its size, then the memory is allocated from bucket.
        * If the size is greater than 512 bytes, by less than 150MB (on 32-bit) or 250MB (on 64-bit), then Traditional Heap Allocations will be used for the request.
        * If the size is greater than 150MB (on 32-bit) or 250MB (on 64-bit), then one dedicated segment is committed for the allocation.
     
     */

    class MemorySegment
    {
    public:
#if defined LUNA_PLATFORM_32BIT
        static constexpr usize segment_size = static_cast<usize>(150_mb);
#else
        static constexpr usize segment_size = static_cast<usize>(250_mb);
#endif
        static constexpr usize page_size = static_cast<usize>(64_kb);    // This cannot be changed!
        static constexpr usize pages_per_segment = segment_size / page_size;
    private:
        void* m_base_address{ nullptr };
        u32 m_committed_pages{ 0 };
        u8 m_committed_table[pages_per_segment / 8]{ 0 };
        
        void set_committed_table_bit(usize pos, bool value)
        {
            if (value)
            {
                m_committed_table[pos / 8] |= (0x01 << (7 - pos % 8));
            }
            else
            {
                m_committed_table[pos / 8] &= (0xff - (0x01 << (7 - pos % 8)));
            }
        }
        
        bool get_committed_table_bit(usize pos)
        {
            return (m_committed_table[pos / 8] >> (7 - pos % 8)) & 0x01;
        }
    public:
        MemorySegment() {}
        ~MemorySegment();
        //! Allocate n continious pages. Returns nullptr if failed.
        void* allocate_pages(usize page_count, bool enable_execute);
        //! Free pages starts from `first_page_addr`.
        void free_pages(void* first_page_addr, usize page_count);
        //! Frees all pages allocated.
        void free_all_pages();
        //! Gets the base address of the segment.
        void* base_address() const
        {
            return m_base_address;
        }
        //! Gets the number of pages allocated.
        usize allocated_pages() const
        {
            return m_committed_pages;
        }
    };

    void* MemorySegment::allocate_pages(usize page_count, bool enable_execute)
    {
        if (!m_base_address)
        {
            // Initialize segment on first allocation.
            usize addr{ 0 };
            do
            {
                // Allocate unaligned segment.
                usize pages = (MemorySegment::pages_per_segment + 1);
                auto rv = virtual_reserve(nullptr, pages * MemorySegment::page_size);
                if(failed(rv))
                {
                    break;
                }
                addr = reinterpret_cast<usize>(rv.get());

                usize aligned_addr = (addr + MemorySegment::page_size - 1) & ~(MemorySegment::page_size - 1);
                luassert_always(succeeded(virtual_release((void*)addr, pages * MemorySegment::page_size)));
                // Allocate aligned segment.
                // Aligned to page_size.
                // For 64KB pages, the lower 16-bit will guarantee to be 0.
                pages -= 1;
                rv = virtual_reserve(reinterpret_cast<void*>(aligned_addr), pages * MemorySegment::page_size);
                addr = succeeded(rv) ? (usize)rv.get() : 0;
            } while (!addr);
            if (addr)
            {
                m_base_address = (void*)addr;
            }
            else
            {
                //! Failed to create the segment.
                return nullptr;
            }
        }
        i32 scanbit = 0;
        //! The page before the first page to be allocated.
        i32 last_commited = -1;
        while (scanbit < MemorySegment::pages_per_segment)
        {
            if (get_committed_table_bit(scanbit))
            {
                last_commited = scanbit;
            }
            if ((scanbit - last_commited) == page_count)
            {
                break;
            }
            scanbit++;
        }
        if ((scanbit - last_commited) != page_count)
        {
            // Failed to find continuous pages.
            return nullptr;
        }
        // Commit pages.
        usize first_page = static_cast<usize>(last_commited + 1);
        void* addr{ reinterpret_cast<void*>((usize)m_base_address + first_page * MemorySegment::page_size) };
        RV res = s_ok;
        if(enable_execute)
        {
            res = virtual_commit(addr, page_count * MemorySegment::page_size, EPageProtection::execute_read_write);
        }
        else
        {
            res = virtual_commit(addr, page_count * MemorySegment::page_size, EPageProtection::read_write);
        }
        if (succeeded(res))
        {
            // Mark pages in table.
            for (usize i = first_page; i < first_page + page_count; i++)
            {
                set_committed_table_bit(i, true);
            }
            // Add committed pages count.
            m_committed_pages += static_cast<u32>(page_count);
            return addr;
        }
        return nullptr;
    }
    void MemorySegment::free_pages(void* first_page_addr, usize page_count)
    {
        usize addr = (usize)first_page_addr;
        lucheck(m_base_address && (addr >= (usize)m_base_address) && (addr < ((usize)m_base_address + MemorySegment::segment_size)));

        u32 pageid = static_cast<u32>(((usize)first_page_addr - (usize)m_base_address) / MemorySegment::page_size);

        luassert_always(succeeded(virtual_decommit(first_page_addr, page_count * MemorySegment::page_size)));
        
        for (usize i = pageid; i < pageid + page_count; i++)
        {
            if (get_committed_table_bit(i))
            {
                set_committed_table_bit(i, false);
                --m_committed_pages;
            }
        }
    }
    void MemorySegment::free_all_pages()
    {
        if (m_base_address)
        {
            free_pages((void*)m_base_address, MemorySegment::pages_per_segment);
        }
    }
    MemorySegment::~MemorySegment()
    {
        free_all_pages();
        if(m_base_address)
        {
            luassert_always(succeeded(virtual_release(m_base_address, MemorySegment::segment_size)));
            m_base_address = nullptr;
        }
    }

    //! The allocator that allocates memory pages.
    //! Used by both bucket allocator and heap allocator.
    class MemoryPageAllocator
    {
    private:
        MemorySegment m_segments[256];  // Infinite on 32-bit platform, 62.5GB on 64-bit platform, which is almost infinite for a single program.
        u16 m_num_segments;
        bool m_enable_execute;
        
    public:
        MemoryPageAllocator(bool enable_execute) :
            m_enable_execute(enable_execute),
            m_num_segments(0) {}
        ~MemoryPageAllocator() {}
        
        //! Checks if one address is in the address range one of the segments of this allocator.
        bool address_in_segment(void* addr) const;
        
        //! Determines which segments allocates the page.
        u8 locate_segment_by_address(void* addr) const;
        
        void* allocate_pages(usize pages_count);
        void free_pages(void* addr, usize pages_count);
        
        bool is_allocated_from_pages(void* ptr) const
        {
            u8 segmentid = locate_segment_by_address(ptr);
            return segmentid == m_num_segments ? false : true;
        }
    };

    u8 MemoryPageAllocator::locate_segment_by_address(void* addr) const
    {
        usize address = (usize)addr;
        u8 i{ 0 };
        for (; (u16)i < m_num_segments; i++)
        {
            if (m_segments[i].base_address() && (address >= (usize)m_segments[i].base_address()) &&
                (address < ((usize)m_segments[i].base_address() + MemorySegment::segment_size)))
            {
                break;
            }
        }
        return i;
    }
    void* MemoryPageAllocator::allocate_pages(usize num_pages)
    {
        if (num_pages >= MemorySegment::pages_per_segment)
        {
            return nullptr;
        }
        void* addr{ nullptr };
        for (u8 i = 0; (u16)i < m_num_segments; i++)
        {
            addr = m_segments[i].allocate_pages(num_pages, m_enable_execute);
            if (addr)
            {
                break;
            }
        }
        if (!addr && (m_num_segments < 256))
        {
            // Create new segment.
            addr = m_segments[m_num_segments].allocate_pages(num_pages, m_enable_execute);
            if(addr)
            {
                ++m_num_segments;
            }
        }
        return addr;
    }
    void MemoryPageAllocator::free_pages(void* addr, usize num_pages)
    {
        u8 segmentid = locate_segment_by_address(addr);
        luassert((usize)segmentid < m_segments.size());
        m_segments[segmentid].free_pages(addr, num_pages);
    }

    class MemoryBucketAllocator
    {
    public:
        using bucket_id_t = u8;
        static constexpr usize block_size = static_cast<usize>(1_kb);
        static constexpr usize blocks_per_page = MemorySegment::page_size / block_size;    //64, including page control block.
        
        static constexpr usize max_alloc_size = 512;
        
#ifdef LUNA_PLATFORM_64BIT
        static constexpr usize num_buckets = max_alloc_size / 16;    //32
#else
        static constexpr usize num_buckets = max_alloc_size / 8;    //64
#endif
        static constexpr usize bucket_allocator_page_magic_number = 0xF3A2585D;
        static constexpr usize item_magic_number = 0xF315D3FC;

        union Page
        {
            // The first block is overlapped and cannot be used.
            struct PageControlBlock
            {
                //! The magic number that is used to ensure this is a page.
                usize m_magic = bucket_allocator_page_magic_number;
                //! For each block, we record which bucket this block is assigned to.
                //! The value is uncertain if the block is in Free Block List.
                bucket_id_t m_owning_bucket[blocks_per_page];
            };
            static_assert(sizeof(PageControlBlock) < block_size, "PCB is too large!");
            PageControlBlock m_pcb;
            u8 data[block_size * blocks_per_page];
        };
        
        static_assert(sizeof(Page) == MemorySegment::page_size, "Wrong Page Size.");

        struct FreeBlockHeader
        {
            FreeBlockHeader* m_prev;
            FreeBlockHeader* m_next;
        };
        struct FreeItemHeader
        {
            FreeItemHeader* m_next;
            usize m_magic;
        };
        
    private:
        MemoryPageAllocator m_allocator;
        FreeBlockHeader* m_free_blocks;
        FreeItemHeader* m_free_lists[num_buckets];

        usize m_allocated_size;
        usize m_pool_capacity;
        
        static bucket_id_t get_bucket_by_size(usize size)
        {
            luassert(size && size <= max_alloc_size);
#ifdef LUNA_PLATFORM_64BIT
            return static_cast<bucket_id_t>((size - 1) / 16);
#else
            return static_cast<bucket_id_t>((size - 1) / 8);
#endif
        }

        static usize get_size_by_bucket_id(bucket_id_t id)
        {
            if (id >= 0 && id < num_buckets)
            {
#ifdef LUNA_PLATFORM_64BIT
                usize size = (id + 1) * 16;
#else
                usize size = (id + 1) * 8;
#endif
                return size < sizeof(FreeItemHeader) ? sizeof(FreeItemHeader) : size;
            }
            return 0;
        }

        static Page* get_page_by_address(void* addr)
        {
            Page* page = reinterpret_cast<Page*>(reinterpret_cast<usize>(addr) & ~(MemorySegment::page_size - 1));
            if (page->m_pcb.m_magic != bucket_allocator_page_magic_number)
            {
                lupanic();
                return nullptr;
            }
            return page;
        }

        static u32 get_block_index_by_address(void* addr)
        {
            usize pageaddr = reinterpret_cast<usize>(get_page_by_address(addr));
            return static_cast<u32>((reinterpret_cast<usize>(addr) - pageaddr) / block_size);
        }

        static bucket_id_t get_bucket_by_address(void* addr)
        {
            Page* page = get_page_by_address(addr);
            u32 blockID = static_cast<u32>((reinterpret_cast<usize>(addr) - reinterpret_cast<usize>(page)) / block_size);
            return page->m_pcb.m_owning_bucket[blockID];
        }
    public:
        MemoryBucketAllocator(bool enable_execute) :
            m_allocator(enable_execute)
        {
            m_pool_capacity = 0;
            m_allocated_size = 0;
            for (auto& i : m_free_lists)
            {
                i = nullptr;
            }
        }
        ~MemoryBucketAllocator() {}

    private:
        // Allocate one page of memory and insert into free blocks.
        // Insert before the `after` block that is passed into as parameters.
        // set `after` to `nullptr` to make it insert in g_FreeBlocks directly.
        // Returns the first element in new lists, or `nullptr` if failed to allocate.
        FreeBlockHeader* allocate_page(FreeBlockHeader* after);
        void* consume_block();
        bool refill_bucket(bucket_id_t id);
        void* allocate_from_bucket(bucket_id_t id);
    public:
        // alignment must be power of 2.
        static bool can_guarantee_alignment(usize size, usize alignment)
        {
            if (size > max_alloc_size || alignment > max_alloc_size)
            {
                return false;
            }
            if (alignment <= 8)    // Hit this for most case.
            {
                return true;
            }
            if ((size % 16) == 0 && (alignment <= 16))    // Hit this for SIMD.
            {
                return true;
            }
            if (((block_size % size) == 0) && (size % alignment) == 0)
            {
                return true;
            }
            return false;
        }
        void* managed_allocate(usize size, usize& allocated_size);
        usize free_to_bucket(void* addr);
        usize get_size(void* addr);
        bool is_managed_memory(void* ptr) const
        {
            return m_allocator.is_allocated_from_pages(ptr);
        }
    };
    
    MemoryBucketAllocator::FreeBlockHeader* MemoryBucketAllocator::allocate_page(FreeBlockHeader* after)
    {
        Page* page = (Page*)m_allocator.allocate_pages(1);
        if (!page)
        {
            return nullptr;
        }

        m_pool_capacity += MemorySegment::page_size;

        //memset(page->m_PCB.m_OwningBucket, 0xff, sizeof(Page::SPageControlBlock));
        page->m_pcb.m_magic = bucket_allocator_page_magic_number;
        // Build List from block 1 (block 0 is overlapped).
        for (usize i = 1; i < blocks_per_page - 1; i++)
        {
            FreeBlockHeader* b1 = reinterpret_cast<FreeBlockHeader*>((usize)page + block_size * i);
            FreeBlockHeader* b2 = reinterpret_cast<FreeBlockHeader*>((usize)page + block_size * (i + 1));
            b1->m_next = b2;
            b2->m_prev = b1;
        }
        // Insert lists to g_Freelist.
        FreeBlockHeader* first = reinterpret_cast<FreeBlockHeader*>((usize)page + block_size);
        FreeBlockHeader* last = reinterpret_cast<FreeBlockHeader*>((usize)page + block_size * (blocks_per_page - 1));
        if (after)
        {
            last->m_next = after;
            first->m_prev = after->m_prev;
            after->m_prev->m_next = first;
            after->m_prev = last;
        }
        else
        {
            last->m_next = m_free_blocks;
            first->m_prev = nullptr;
            if (m_free_blocks)
            {
                m_free_blocks->m_prev = last;
            }
            m_free_blocks = first;
        }
        return first;
    }

    void* MemoryBucketAllocator::consume_block()
    {
        if (!m_free_blocks)
        {
            // TODO: Add Some GC for consumed but not used blocks.
            allocate_page(nullptr);
        }
        if (m_free_blocks)
        {
            FreeBlockHeader* header = m_free_blocks;
            m_free_blocks = m_free_blocks->m_next;
            if (m_free_blocks)
            {
                m_free_blocks->m_prev = nullptr;
            }
            header->m_next = nullptr;
            header->m_prev = nullptr;
            return header;
        }
        return nullptr;
    }

    bool MemoryBucketAllocator::refill_bucket(bucket_id_t id)
    {
        lucheck(id < NumBuckets);
        if (m_free_lists[id])
        {
            return true;
        }
        // Consume one free block.
        usize block = reinterpret_cast<usize>(consume_block());
        if (!block)
        {
            return false;
        }
        // Get Page Header Address.
        Page* page = get_page_by_address(reinterpret_cast<void*>(block));
        // Get Bucket Index by Calculating offsets.
        u32 blockid = get_block_index_by_address(reinterpret_cast<void*>(block));
        page->m_pcb.m_owning_bucket[blockid] = id;
        // Divide block into items.
        usize itemSize = get_size_by_bucket_id(id);
        usize itemCount = block_size / itemSize;
        for (usize i = 0; i < itemCount - 1; i++)
        {
            FreeItemHeader* p = reinterpret_cast<FreeItemHeader*>(block + itemSize * i);
            p->m_next = reinterpret_cast<FreeItemHeader*>(block + itemSize * (i + 1));
            p->m_magic = item_magic_number;
        }
        FreeItemHeader* first = reinterpret_cast<FreeItemHeader*>(block);
        FreeItemHeader* last = reinterpret_cast<FreeItemHeader*>(block + itemSize * (itemCount - 1));
        last->m_next = nullptr;
        last->m_magic = item_magic_number;
        m_free_lists[id] = first;
        return true;
    }

    void* MemoryBucketAllocator::allocate_from_bucket(bucket_id_t id)
    {
        lucheck(id < NumBuckets);
        if (!m_free_lists[id] && !refill_bucket(id))
        {
            return nullptr;
        }
        FreeItemHeader* item = m_free_lists[id];
        m_free_lists[id] = item->m_next;
        item->m_magic = 0;
        item->m_next = nullptr;
        return item;
    }

    usize MemoryBucketAllocator::free_to_bucket(void* addr)
    {
        // Try to fetch the page.
        Page* page = get_page_by_address(addr);
        if (page->m_pcb.m_magic != bucket_allocator_page_magic_number)
        {
            return 0;
        }
        // Here we assume that this address is correct (actually allocated from page).
        bucket_id_t bucket = get_bucket_by_address(addr);
        FreeItemHeader* header = reinterpret_cast<FreeItemHeader*>(addr);
        if (header->m_magic == item_magic_number)
        {
            // Double delete!
            lupanic_msg("Double Free Detected.");
            return 0;
        }
        header->m_magic = item_magic_number;
        header->m_next = m_free_lists[bucket];
        m_free_lists[bucket] = header;
        usize ret = get_size_by_bucket_id(bucket);
        m_allocated_size -= ret;
        return ret;
    }

    void* MemoryBucketAllocator::managed_allocate(usize size, usize& allocated_size)
    {
        //! Bucket allocators cannot handle allocations whose size is greater than max_alloc_size bytes.
        if (size > max_alloc_size)
        {
            return nullptr;
        }
        bucket_id_t bucket = get_bucket_by_size(size);
        void* ret = allocate_from_bucket(bucket);
        if (ret)
        {
            allocated_size = get_size_by_bucket_id(bucket);
            m_allocated_size += allocated_size;
        }
        return ret;
    }

    usize MemoryBucketAllocator::get_size(void* addr)
    {
        bucket_id_t bucket = get_bucket_by_address(addr);
        if (bucket == 0xff)
        {
            return 0;
        }
        return get_size_by_bucket_id(bucket);
    }

    class Heap
    {
    public:
        pthread_mutex_t m_mutex;
        usize m_allocated;
        MemoryBucketAllocator m_bucket_allocator;
        std::unordered_map<usize, usize> m_heap_allocations;
        c8 m_name[64];
        bool m_thread_safe;
        
        Heap(EHeapCreationFlag flags) :
            m_bucket_allocator(((flags & EHeapCreationFlag::enable_execute) == EHeapCreationFlag::none) ? false : true),
            m_allocated(0)
        {
            m_thread_safe = (flags & EHeapCreationFlag::no_thread_safe) == EHeapCreationFlag::none;
            if(m_thread_safe)
            {
                luna_eval_and_assert_equal(pthread_mutex_init(&m_mutex, nullptr), 0);
            }
            m_name[0] = 0;
        }
        ~Heap()
        {
            for(auto& i : m_heap_allocations)
            {
                void* ptr = (void*)i.first;
                ::free(ptr);
            }
            m_allocated = 0;
            pthread_mutex_destroy(&m_mutex);
        }
        
        const c8* name()
        {
            return m_name;
        }
        void set_name(const c8* name)
        {
            strcpy(m_name, name);
        }
        usize allocated()
        {
            return m_allocated;
        }
        
        void* allocate(usize size, usize alignment = 0);
        void free(void* ptr, usize alignment = 0);
        void* reallocate(void* ptr, usize size, usize alignment = 0);
        usize size(void* ptr, usize alignment = 0);
    };

    void* Heap::allocate(usize size, usize alignment)
    {
        if(!size)
        {
            return nullptr;
        }
        if(m_thread_safe)
        {
            luna_eval_and_assert_equal(pthread_mutex_lock(&m_mutex), 0);
        }
        void* ret {nullptr};
        usize allocated {0};
        // Allocate from bucket.
        if(size <= MemoryBucketAllocator::max_alloc_size &&
           MemoryBucketAllocator::can_guarantee_alignment(size, alignment))
        {
            ret = m_bucket_allocator.managed_allocate(size, allocated);
        }
        if(ret)
        {
            m_allocated += allocated;
            if(m_thread_safe)
            {
                luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
            }
            return ret;
        }
        // Fallback Allocate from system heap.
        if(alignment > max_align)
        {
            int r = posix_memalign(&ret, alignment, size);
            if(r != 0)
            {
                ret = nullptr;
            }
        }
        else
        {
            ret = malloc(size);
        }
        if(ret)
        {
            m_heap_allocations.insert(std::make_pair((usize)ret, size));
            m_allocated += size;
        }
        if(m_thread_safe)
        {
            luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
        }
        return ret;
    }

    void Heap::free(void* ptr, usize alignment)
    {
        if(!ptr)
        {
            return;
        }
        if(m_thread_safe)
        {
            luna_eval_and_assert_equal(pthread_mutex_lock(&m_mutex), 0);
        }
        usize sz;
        if(m_bucket_allocator.is_managed_memory(ptr))
        {
            sz = m_bucket_allocator.free_to_bucket(ptr);
        }
        else
        {
            auto iter = m_heap_allocations.find((usize)ptr);
            sz = iter->second;
            ::free(ptr);
            m_heap_allocations.erase(iter);
        }
        m_allocated -= sz;
        if(m_thread_safe)
        {
            luna_eval_and_assert_equal(pthread_mutex_unlock(&m_mutex), 0);
        }
    }

    void* Heap::reallocate(void* ptr, usize sz, usize alignment)
    {
        void* new_addr { nullptr };
        new_addr = allocate(sz, alignment);
        if (new_addr)
        {
            usize old_size = size(ptr, alignment);
            memcpy(new_addr, ptr, min(old_size, sz));
            free(ptr, alignment);
            return new_addr;
        }
        return nullptr;
    }

    usize Heap::size(void* ptr, usize alignment)
    {
        if(!ptr)
        {
            return 0;
        }
        usize sz;
        if(m_bucket_allocator.is_managed_memory(ptr))
        {
            sz = m_bucket_allocator.get_size(ptr);
        }
        else
        {
            auto iter = m_heap_allocations.find((usize)ptr);
            sz = iter->second;
        }
        return sz;
    }

    class GlobalHeap : public IHeap
    {
    public:
        lucid("{418d0e31-3407-4c2d-8646-6d48b6605efc}");
        luiimpl_static(GlobalHeap, IHeap, IAllocator, IObject);
        
        Heap m_heap;
        
        GlobalHeap() :
            m_heap(EHeapCreationFlag::none) {}
        
        virtual const c8* name() override
        {
            return m_heap.name();
        }
        virtual void set_name(const c8* name) override
        {
            m_heap.set_name(name);
        }
        virtual usize allocated() override
        {
            return m_heap.allocated();
        }
        virtual void* allocate(usize size, usize alignment = 0) override
        {
            return m_heap.allocate(size, alignment);
        }
        virtual void free(void* ptr, usize alignment = 0) override
        {
            m_heap.free(ptr, alignment);
        }
        virtual void* reallocate(void* ptr, usize size, usize alignment = 0) override
        {
            return m_heap.reallocate(ptr, size, alignment);
        }
        virtual usize size(void* ptr, usize alignment = 0) override
        {
            return m_heap.size(ptr, alignment);
        }
    };

    class DedicatedHeap : public IHeap
    {
    public:
        lucid("{fdf0a5ae-c492-4ef7-8c3d-6b4f0d906701}");
        luiimpl(DedicatedHeap, IHeap, IAllocator, IObject);
        
        Heap m_heap;
        
        DedicatedHeap(EHeapCreationFlag flags) :
            m_heap(flags) {}
        
        virtual const c8* name() override
        {
            return m_heap.name();
        }
        virtual void set_name(const c8* name) override
        {
            m_heap.set_name(name);
        }
        virtual usize allocated() override
        {
            return m_heap.allocated();
        }
        virtual void* allocate(usize size, usize alignment = 0) override
        {
            return m_heap.allocate(size, alignment);
        }
        virtual void free(void* ptr, usize alignment = 0) override
        {
            m_heap.free(ptr, alignment);
        }
        virtual void* reallocate(void* ptr, usize size, usize alignment = 0) override
        {
            return m_heap.reallocate(ptr, size, alignment);
        }
        virtual usize size(void* ptr, usize alignment = 0) override
        {
            return m_heap.size(ptr, alignment);
        }
    };

    Unconstructed<GlobalHeap> g_global_heap;

    LUNA_RUNTIME_API IHeap* get_global_heap()
    {
        return &(g_global_heap.get());
    }

    LUNA_RUNTIME_API RP<IHeap> new_heap(EHeapCreationFlag heap_creation_flags, const char* name)
    {
        P<IHeap> heap = memnewref<DedicatedHeap>(heap_creation_flags);
        if(name)
        {
            heap->set_name(name);
        }
        return heap;
    }

    void memory_init()
    {
        g_global_heap.construct();
    }

    void memory_deinit()
    {
        g_global_heap.get().release();
    }
}

#endif
