/*!
  @file           Catalog_Object.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          base class of all catalog objects

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


#ifndef CATALOG_OBJECT_HPP
#define CATALOG_OBJECT_HPP

#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

class Catalog_AuthorizationObject;
class Catalog_TableObject;
class Catalog_MessBlockObject;
class SAPDBMem_IRawAllocator;
class SQLMan_Context;

/*!
   @brief base class of all objects stored in the Catalog_SessionCache
            
          Catalog_Objects describe objects of the database, for example users, tables, views etc.
          Furhtermore they describe temporary objects created in the course of sql execution,
          for example execution plans.
*/

class Catalog_Object : public Catalog_Object_Pascal
{
#define TYPE_MESSBLOCK        cak_cmessblock         //!< type flag of Catalog_MessBlockObject
#define TYPE_TABLE            cak_ctable             //!< type flag of Catalog_TableObject
#define TYPE_RESULT           cak_cresult            //!< type flag of Catalog_TableObject(temporary)
#define TYPE_RESULTNAME       cak_cresname           //!< type flag of result name object
#define TYPE_SHARED_SQL       cak_csharedSQL_info    //!< type flag of Catalog_SharedSqlObject  
#define TYPE_METHOD           cak_emethod            //!< type flag of Catalog_DBProcObject
#define TYPE_METHODFILEINFO   cak_emethodfileinfo    //!< type flag of Catalog_DBProcFileObject
#define TYPE_METHODINFO       cak_emethodinfo        //!< type flag of Catalog_DBProcInfoObject
#define TYPE_TRIGGERPARSEINFO cak_ctriggerparsinfo   //!< type flag for trigger parse infos
#define TYPE_AUTHORIZATION    cak_cuser              //!< type flag for authorizations (role, user, schema)
#define TYPE_AUTHORIZATIONREF cak_cuserref           //!< type flag for authorization references
#define TYPE_SHORTFIELDINFO   cak_cshortinfo         //!< type flag for short field information (parameter description) 
#define TYPE_COLUMNNAMES      cak_ccolnames          //!< type flag for column name information
#define TYPE_CONTAINER        cak_ccontainer         //!< type flag for container information
#define TYPE_VIEWTEXT         cak_cviewtext          //!< type flag for a viewtext information
#define TYPE_TABLESTATISTICS  cak_cstatistics        //!< type flag for a table statistics information

#define SQL_ROW_NOT_FOUND           100              //!< sql code row not found
#define SQL_DUPLICATE_KEY           200              //!< sql code duplicate key
#define SQL_DUPLICATE_SECONDARY_KEY 250              //!< sql code duplicate index key  

public :
	/*!
	   @brief standard keylength 12
	*/
	enum { DefaultKeyLen = 12 };
   	/*!
	   @brief offset of a standard catalog object with keylength 12
	*/
	enum { DefaultOffset = cak_sysbufferoffset };
    /*!
       @brief incopy return codes
     */
    enum IncopyResult { icp_ok, icp_not_found, icp_error };

    /*!
       @brief defines the key of a Catalog_Object. Keys are used to identify catalog objects. 
    */
	class Catalog_Key : public tgg00_SysInfoKey
	{
	public :
        Catalog_Key(); //!< default constructor

        /*!
           @brief copy constructor for a standard key
           @param standardKey source key to be copied 
        */
        Catalog_Key(const tsp00_C12& standardKey);
        /*!
           @brief copy constructor
         */
        Catalog_Key(const Catalog_Object::Catalog_Key& Source);
        /*!
           @brief creates a copy of the key
         */
        Catalog_Key* CreateCopy(SAPDBMem_IRawAllocator& alloc) const;
        /*!
           @brief computes a hash value from the key
         */
        inline SAPDB_UInt4 HashValue() const;
        /*!
           @brief returns the key length
         */
        inline int Length() const;
        /*!
           @brief puts the entrytype into the key
           @param entryType type flag to be assigned
        */
        inline void SetEntryType(unsigned char entryType); 
        /*!
           @brief puts a result id into the key 
         */
        inline void SetResultId(int resultId);
        /*!
           @brief puts a surrogate into the key
           @param surrogate, surrogate to be assigned
        */
        inline void SetSurrogate(const Catalog_ObjectId& surrogate);
		/*!
		   @brief returns true, if the current key and the given key are equal
		 */
		inline bool operator== (const Catalog_Key& Key) const;
        /*!
           @brief assignment
         */
        inline void operator= (const Catalog_Key& Key); 
        /*!
           @brief operator +
         */
        inline void operator++();
	};

	/*!
       @brief constructor, initializes keylen and record length
       @param keyLen  the length of the key
       @param infoLen the length of the info part of the object
    */
	Catalog_Object (int keyLen, int infoLen);
    /*!
       @brief removes the instance from the heap identified by the allocator
     */
    void Destroy (SAPDBMem_IRawAllocator& Allocator);
	/*!
	   @brief gets pointer to information part of the catalog object
    */
	const void* GetInfo(int& infoLen) const;

    /*!
       @brief creates an object from the catalog tables using the specified allocator 
     */
    static Catalog_Object* Incopy(SQLMan_Context& Context, const Catalog_Key& Key, SAPDBMem_IRawAllocator& allocator, bool NoWait);
    
    /*!
       @brief invalidates the transaction info of the given catalog object.
       @param p memory containing the catalog object
       @param offset catalog object starts at offset in p
    */
    static void InvalidateTransaction(void* p, int offset);
	/*!
	   @brief stores information in a catalog object
    */
	int PutInfo (void* pInfo, int infoLength);
    /*!
       @brief returns the type of the object
    */
    int Type() const;
    /*!
       @brief casts current record to a Catalog_AuthorizationObject
    */
    inline operator Catalog_AuthorizationObject&()
    {
        return *reinterpret_cast<Catalog_AuthorizationObject*>(this);
    }
    /*!
       @brief casts current record to a Catalog_TableObject
    */
    inline operator Catalog_TableObject&()
    {
        return *reinterpret_cast<Catalog_TableObject*>(this);
    }
    /*!
       @brief casts current record to a Catalog_MessBlockObject
    */
    inline operator Catalog_MessBlockObject&()
    {
        return *reinterpret_cast<Catalog_MessBlockObject*>(this);
    }
    /*!
       @brief creates a copy using a given allocator
    */
    void* CreateCopy (SAPDBMem_IRawAllocator& allocator, int offset, SAPDB_UInt4& memoryUsage);
    /*!
	   @brief returns the catalog key
     */
	inline const Catalog_Key& GetCatalogKey() const;
	/*!
       @brief returns the address of the object key
    */
    inline const void* GetKey() const;
    /*!
       @brief returns the length of the object key
    */
    inline int KeyLength() const;
    /*!
       @brief returns the length of the object
    */
    inline int Length() const;
    /*!
       @brief stores the catalog object into the catalog tables
     */
    void Outcopy (SQLMan_Context& context, bool isUpdate);
};

inline const Catalog_Object::Catalog_Key& Catalog_Object::GetCatalogKey() const
{
	return *reinterpret_cast<const Catalog_Object::Catalog_Key*>(&this->syskey());
}

inline const void* Catalog_Object::GetKey() const
{
    return &this->syskey().all();
};

inline int Catalog_Object::KeyLength() const
{
    return this->syskey().skeylen;
}

inline int Catalog_Object::Length() const
{
    return this->b_sl();
}

inline SAPDB_UInt4 Catalog_Object::Catalog_Key::HashValue() const
{
    SAPDB_UInt4  val   = 0;
    unsigned int shift = 0;
    for (int ix = 0; ix < this->skeylen; ++ix)
    {
        val ^= (this->all()[ix]) << shift;
        shift += 8;
        if (shift >= sizeof(int) * 8)
        {
            shift = 0;
        }
    }
    return val;
}

inline int Catalog_Object::Catalog_Key::Length() const
{
    return skeylen;
}

inline void Catalog_Object::Catalog_Key::SetEntryType(unsigned char entryType)
{
    this->sentrytyp()[1] = entryType;
}

inline void Catalog_Object::Catalog_Key::SetResultId(int resultId)
{
    this->stempid() = resultId;
}

inline void Catalog_Object::Catalog_Key::SetSurrogate(const Catalog_ObjectId& surrogate)
{
    SAPDB_MemCopyNoCheck (&this->stableid()[0], &surrogate[0], sizeof(surrogate));
}

inline bool Catalog_Object::Catalog_Key::operator== (const Catalog_Object::Catalog_Key& Key) const
{
	return ((this->skeylen == Key.skeylen) && (0 == memcmp (&this->all(), &Key.all(), this->skeylen))); 
}

inline void Catalog_Object::Catalog_Key::operator= (const Catalog_Key& Key)
{
    skeylen = Key.skeylen;
    SAPDB_MemCopyNoCheck (&this->all()[0], &Key.all()[0], Key.skeylen);
}

inline void Catalog_Object::Catalog_Key::operator++()
{
    ++slinkage()[1];
}

#endif