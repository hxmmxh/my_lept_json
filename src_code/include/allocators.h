#include "leptjson.h"

LEPTJSON_NAMESPACE_BEGIN

#ifndef LEPTJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY
#define LEPTJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY (64 * 1024)
#endif

class CrtAllocator
{
public:
    static const bool NeedFree = true;
    //表明该配置器需要在成员析构后再调用free函数
    void *Malloc(size_t size)
    {
        if (size) //如果分配的size为0,则简单返回null
            return std::malloc(size);
        else
            return NULL;
    }
    void *Realloc(void *originalPtr, size_t originalSize, size_t newSize)
    {
        (void)originalSize;
        //防止编译器编译时报警告的用法。有些变量如果未曾使用，在编译时是会报错，从而有些导致编译不过，所以才会出现这种用法。
        //而此语句在代码中没有具体意义，只是告诉编译器该变量已经使用了。
        if (newSize == 0)
        {
            std::free(originalPtr);
            return NULL;
        }
        return std::realloc(originalPtr, newSize);
    }
    static void Free(void *ptr) { std::free(ptr); }
};

//内存池
template <typename BaseAllocator = CrtAllocator>
class MemoryPoolAllocator
{
public:
    static const bool kNeedFree = false;
    //用块大小初始化分配器
    MemoryPoolAllocator(size_t chunkSize = DefaultChunkCapacity, BaseAllocator *baseAllocator = 0)
        : chunkHead_(0), chunk_capacity_(chunkSize), userBuffer_(0), baseAllocator_(baseAllocator), ownBaseAllocator_(0)
    {
    }
    //用用户提供的缓冲去初始化
    //! Constructor with user-supplied buffer.
    /*! The user buffer will be used firstly. When it is full, memory pool allocates new chunk with chunk size.

        The user buffer will not be deallocated when this allocator is destructed.

        \param buffer User supplied buffer.
        \param size Size of the buffer in bytes. It must at least larger than sizeof(ChunkHeader).
        \param chunkSize The size of memory chunk. The default is kDefaultChunkSize.
        \param baseAllocator The allocator for allocating memory chunks.
    */
    MemoryPoolAllocator(void *buffer, size_t size, size_t chunkSize = DefaultChunkCapacity, BaseAllocator *baseAllocator = 0)
        : chunkHead_(0), chunk_capacity_(chunkSize), userBuffer_(buffer), baseAllocator_(baseAllocator), ownBaseAllocator_(0)
    {
        LEPTJSON_ASSERT(buffer != 0);
        LEPTJSON_ASSERT(size > sizeof(ChunkHeader));
        chunkHead_ = reinterpret_cast<ChunkHeader *>(buffer);
        chunkHead_->capacity = size - sizeof(ChunkHeader);
        chunkHead_->size = 0;
        chunkHead_->next = 0;
    }

    //! Destructor.
    /*! This deallocates all memory chunks, excluding the user-supplied buffer.
    */
    ~MemoryPoolAllocator()
    {
        Clear();
        delete ownBaseAllocator_;
    }

    //! Deallocates all memory chunks, excluding the user-supplied buffer.
    void Clear()
    {
        while (chunkHead_ && chunkHead_ != userBuffer_)
        {
            ChunkHeader *next = chunkHead_->next;
            baseAllocator_->Free(chunkHead_);
            chunkHead_ = next;
        }
        if (chunkHead_ && chunkHead_ == userBuffer_)
            chunkHead_->size = 0; // Clear user buffer
    }

    //! Computes the total capacity of allocated memory chunks.
    /*! \return total capacity in bytes.
    */
    size_t Capacity() const
    {
        size_t capacity = 0;
        for (ChunkHeader *c = chunkHead_; c != 0; c = c->next)
            capacity += c->capacity;
        return capacity;
    }

    //! Computes the memory blocks allocated.
    /*! \return total used bytes.
    */
    size_t Size() const
    {
        size_t size = 0;
        for (ChunkHeader *c = chunkHead_; c != 0; c = c->next)
            size += c->size;
        return size;
    }

    //! Allocates a memory block. (concept Allocator)
    void *Malloc(size_t size)
    {
        if (!size)
            return NULL;

        size = LEPTJSON_ALIGN(size);
        //当前块剩余容量不够就重新加入一个新块
        if (chunkHead_ == 0 || chunkHead_->size + size > chunkHead_->capacity)
            if (!AddChunk(chunk_capacity_ > size ? chunk_capacity_ : size))
                return NULL;
        //当前的地址等于chunkhead加上chunkheader自身的字节数加上已有的size数
        void *buffer = reinterpret_cast<char *>(chunkHead_) + LEPTJSON_ALIGN(sizeof(ChunkHeader)) + chunkHead_->size;
        chunkHead_->size += size;
        return buffer;
    }

    //! Resizes a memory block (concept Allocator)
    void *Realloc(void *originalPtr, size_t originalSize, size_t newSize)
    {
        if (originalPtr == 0)
            return Malloc(newSize);

        if (newSize == 0)
            return NULL;

        originalSize = LEPTJSON_ALIGN(originalSize);
        newSize = LEPTJSON_ALIGN(newSize);

        // Do not shrink if new size is smaller than original
        if (originalSize >= newSize)
            return originalPtr;

        // Simply expand it if it is the last allocation and there is sufficient space
        if (originalPtr == reinterpret_cast<char *>(chunkHead_) + LEPTJSON_ALIGN(sizeof(ChunkHeader)) + chunkHead_->size - originalSize)
        {
            size_t increment = static_cast<size_t>(newSize - originalSize);
            if (chunkHead_->size + increment <= chunkHead_->capacity)
            {
                chunkHead_->size += increment;
                return originalPtr;
            }
        }

        // Realloc process: allocate and copy memory, do not free original buffer.
        if (void *newBuffer = Malloc(newSize))
        {
            if (originalSize)
                std::memcpy(newBuffer, originalPtr, originalSize);
            return newBuffer;
        }
        else
            return NULL;
    }

    //! Frees a memory block (concept Allocator)
    static void Free(void *ptr) { (void)ptr; } // Do nothing

private:
    //禁止拷贝构造函数
    MemoryPoolAllocator(const MemoryPoolAllocator &rhs) = delete;
    //禁止拷贝赋值函数
    MemoryPoolAllocator &operator=(const MemoryPoolAllocator &rhs) = delete;
    //创建一个新块，容量为capacity
    bool AddChunk(size_t capacity)
    {
        if (!baseAllocator_)
            ownBaseAllocator_ = baseAllocator_ = new BaseAllocator();
        //分配的空间要加上ChunkHeader的大小,用LEPTJSON_ALIGN调整成8的倍数为了字节对齐
        //在当前的chunkhead之前插入新创建的chunk
        if (ChunkHeader *chunk = reinterpret_cast<ChunkHeader *>(baseAllocator_->Malloc(LEPTJSON_ALIGN(sizeof(ChunkHeader)) + capacity)))
        {
            chunk->capacity = capacity;
            chunk->size = 0;
            chunk->next = chunkHead_;
            chunkHead_ = chunk;
            return true;
        }
        else
            return false;
    }
    static const int DefaultChunkCapacity = LEPTJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY; //默认的块大小
    //块储存在链表结构中
    //每个块用Chunk header表示
    struct ChunkHeader
    {
        size_t capacity;   //块的容量
        size_t size;       //当前块已用的字节数
        ChunkHeader *next; //链表中的下一个块
    };

    ChunkHeader *chunkHead_; //链表所指向的块结点
    size_t chunk_capacity_;  // 每个块被创建时的最小容量
    void *userBuffer_;       //用户提供的缓冲区
    BaseAllocator *baseAllocator_;
    BaseAllocator *ownBaseAllocator_;
};

LEPTJSON_NAMESPACE_END
