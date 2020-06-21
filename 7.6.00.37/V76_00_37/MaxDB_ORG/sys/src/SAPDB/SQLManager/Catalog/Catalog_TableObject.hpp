/*!
  @file           Catalog_TableObject.hpp
  @author         ThomasA
  @brief          representation of a table descriptor in 
  @ingroup        Catalog

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



\endif
*/


#ifndef CATALOG_TABLEOBJECT_HPP
#define CATALOG_TABLEOBJECT_HPP
#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

/*!
   @brief alignment
 */
#if defined BIT64
#define CATALOG_ALIGN_MASK 7 
#else
#define CATALOG_ALIGN_MASK 3 
#endif

class SAPDBMem_IRawAllocator;
class SQLMan_Context;

/*!
  @brief encapsulation of pascal type tak_baserecord
*/

class Catalog_TableObject : public Catalog_TableObjectPascal
{
public :
    /*!
       @brief key in the catalog of a record decribing a table object
     */
    class Catalog_Key : public Catalog_Object::Catalog_Key
    {
    public :
        /*!
           @brief constructs the key of an authorization object
         */
        Catalog_Key(const Catalog_ObjectId& TableId);
    };

	/*!
	   @brief creates a copy of this using the given allocator
    */
    void* CreateCopy (SAPDBMem_IRawAllocator& allocator, int offset, SAPDB_UInt4& heapUsage);
  	/*!
	   @brief creates a copy of this using the given allocator
    */
    void* CreateCopy (SAPDBMem_IRawAllocator& allocator);
    /*!
       @brief destructs the instance
     */
    void Destroy (SAPDBMem_IRawAllocator& Allocator); 
	/*!
	   @brief deletes the memory containing the column information of the in the given allocator
    */
	void DestroyColumns(SAPDBMem_IRawAllocator& allocator);
    /*!
       @brief returns the internal name of the table
     */
    inline Catalog_ObjectId GetObjectId() const;
    /*!
	   @brief increments the reference counter of the column buffer
    */
    inline void IncColBufferRefCount();
    /*!
	   @brief decrements the reference counter of the column buffer. If the counter becomes 0
              the column buffer is released using the given allocator
    */
    inline void DecColBufferRefCount(SAPDBMem_IRawAllocator& allocator);
    /*!
      @brief stores the table object persistently 
    */
    void outcopy(SQLMan_Context& context);
    /*!
      @brief transforms the consistent layout into a Catalog_TableObject 
    */
     /*!
       @brief creates an object from the catalog tables using the specified allocator 
     */
    static Catalog_Object* Incopy(SQLMan_Context& Context, const Catalog_ObjectId& TableId, SAPDBMem_IRawAllocator& Allocator);
#if !(defined(SUN) || defined(SOLARIS))
private :
#endif
    struct NextBuffer //!< successor column buffer
    {
        NextBuffer*   m_next;       //!< next successor buffer
        unsigned char m_colBuffer;  //<! first byte of column buffer 
        NextBuffer();               //!< constructor
        static int BufferOffset();  //!< returns the offset of m_colBuffer
        void* operator new (size_t size, SAPDBMem_IRawAllocator& allocator, int bufferLen); //<! operator new
    };
#if defined(SUN) || defined(SOLARIS)
private :
#endif
    struct ColumnBuffer //!< container for column descriptors
    {
        SAPDB_Int4                       m_sharedFlag; //!< flag for AK Cache, allows to identify shared sql buffer
        SAPDB_Int4                       m_refCount;   //!< reference counter
        Catalog_TableObject::NextBuffer* m_next;       //!< pointer to next column buffer
        SQLMan_Identifier                m_tableName;  //!< the table name
        unsigned char                    m_colBuffer;  //!< first byte of column buffer
        static int BufferOffset();            //!< returns the offset of m_colBuffer  
        void IncRefCount();                   //!< increments the reference count
        int  DecRefCount(SAPDBMem_IRawAllocator& allocator); //!< decrements the reference count
        ColumnBuffer (const SQLMan_Identifier& tableName);   //!< constructor
        void* operator new (size_t size, SAPDBMem_IRawAllocator& allocator, int bufferLen); //!< operator new
    };
    inline static int GetColumnInfoLength (Catalog_ColumnInfo& columnInfo); //!< calculated the requiried length for a column desription
    inline static int CalcAlignedLength(int length);                        //!< calculates the aligned length
    static RTESync_Spinlock m_refCountSynchronizer;   //!< global synchronization object for reference counting
    friend struct Catalog_TableObject::ColumnBuffer;  //!< allows ColumnBuffer access to Catalog_TableObject
    friend struct Catalog_TableObject::NextBuffer;    //!< allows NextBuffer access to Catalog_TableObject
};

inline void Catalog_TableObject::IncColBufferRefCount()
{
    REINTERPRET_CAST(ColumnBuffer*, bptr)->IncRefCount();
}

//--------------------------------------------------------------------------------

inline void Catalog_TableObject::DecColBufferRefCount(SAPDBMem_IRawAllocator& allocator)
{
    if (bptr)
    {
        REINTERPRET_CAST(ColumnBuffer*, bptr)->DecRefCount(allocator);
        bptr = 0;
    }
}

//--------------------------------------------------------------------------------

inline int Catalog_TableObject::GetColumnInfoLength (Catalog_ColumnInfo& columnInfo)
{
    return sizeof(columnInfo) - sizeof(columnInfo.ccolumnn()) + columnInfo.ccolumnn_len();
}

//--------------------------------------------------------------------------------

inline Catalog_ObjectId Catalog_TableObject::GetObjectId() const
{
    return bsurrogate;
}

//--------------------------------------------------------------------------------

inline int Catalog_TableObject::CalcAlignedLength(int length)
{
    return (length + CATALOG_ALIGN_MASK) & ~(CATALOG_ALIGN_MASK);
}

#endif
