#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

class MyAllocator : public SAPDBMem_IRawAllocator
{
public: 
    MyAllocator(SAPDBMem_IRawAllocator *allocator)
    :m_alloc_count(0),
     m_dealloc_count(0),
     m_alloc_size(0),
     m_dealloc_size(0),
     m_wrapped_instance(allocator)
    {}

    virtual void *Allocate(SAPDB_ULong ByteCount)
    {
    	ByteCount+=8;
        m_alloc_count++;
        m_alloc_size += ByteCount;
        void * erg = m_wrapped_instance->Allocate(ByteCount);
        memcpy (erg, &ByteCount, 8);
        erg = ((char*)erg)+8;
        return erg; 
    }
    
    virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint)
    {
    	ByteCount+=8;
        m_alloc_count++;
        m_alloc_size += ByteCount;
        void * erg = m_wrapped_instance->Allocate(ByteCount, Hint);
        memcpy (erg, &ByteCount, 8);
        erg = ((char*)erg)+8;
        return erg; 
    }

    virtual void Deallocate(void* p)
    {   
      	m_dealloc_count++;
        p = ((char*)p)-8;
      	m_dealloc_size  += *((SAPDB_ULong*)p);
        m_wrapped_instance->Deallocate(p);
    }

    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &,SAPDB_ULong &) const
    {
        return;
    }

    virtual void GetCallStatistics(SAPDB_ULong &,SAPDB_ULong &) const
    {
        return;
    }
    
    SQLDBC_Int8 getAllocCount()
    {
        return m_alloc_count;
    }
    SQLDBC_Int8 getDeAllocCount()
    {
        return m_dealloc_count;
    }
    SQLDBC_Int8 getAllocBytes()
    {
        return m_alloc_size;
    }
    SQLDBC_Int8 getDeAllocBytes()
    {
        return m_dealloc_size;
    }

private:
    SQLDBC_Int8 m_alloc_size;
    SQLDBC_Int8 m_dealloc_size;
    SQLDBC_Int8 m_alloc_count;
    SQLDBC_Int8 m_dealloc_count;
    SAPDBMem_IRawAllocator *m_wrapped_instance;
};