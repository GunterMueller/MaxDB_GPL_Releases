/*!***************************************************************************

  module      : Catalog_SessionCache.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog
  description : encapsulation of pascal AK cache

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

#include "SQLManager/Catalog/Catalog_AuthorizationObject.hpp"
#include "SQLManager/Catalog/Catalog_AuthorizationRefObject.hpp"
#include "SQLManager/Catalog/Catalog_ContainerObject.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"
#include "SQLManager/Catalog/Catalog_TableObject.hpp"
#include "SQLManager/Catalog/Catalog_MessBlockObject.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

//--------------------------------------------------------------------------

Catalog_Object::Catalog_Key::Catalog_Key()
{
	this->sreclen       = 0;
    this->skeylen       = Catalog_Object::DefaultKeyLen; 
    this->svarcoloffset = 0;
    this->svarcolcnt    = 0;
	memset (this->sstandardkey(), 0, sizeof(this->sstandardkey()));
}

//--------------------------------------------------------------------------

Catalog_Object::Catalog_Key::Catalog_Key(const tsp00_C12& standardKey)
{
    this->sreclen       = 0;
    this->skeylen       = Catalog_Object::DefaultKeyLen; 
    this->svarcoloffset = 0;
    this->svarcolcnt    = 0;
    SAPDB_MemCopyNoCheck (&this->sstandardkey()[0], &standardKey[0], sizeof(standardKey));
}

//--------------------------------------------------------------------------

Catalog_Object::Catalog_Key::Catalog_Key(const Catalog_Object::Catalog_Key& Source)
{
    this->sreclen       = 0;
    this->skeylen       = Source.Length(); 
    this->svarcoloffset = 0;
    this->svarcolcnt    = 0;
    SAPDB_MemCopyNoCheck (&this->sstandardkey()[0], &Source.sstandardkey()[0], Source.Length());
}

//--------------------------------------------------------------------------

Catalog_Object::Catalog_Key* Catalog_Object::Catalog_Key::CreateCopy(SAPDBMem_IRawAllocator& alloc) const
{
    void* p = alloc.Allocate(8 + this->skeylen);
    if (p)
    {
        SAPDB_MemCopyNoCheck (p, this, 8 + this->skeylen);
    }
    return reinterpret_cast<Catalog_Object::Catalog_Key*> (p);
}

//--------------------------------------------------------------------------

Catalog_Object::Catalog_Object (int keyLen, int infoLen)
{
   this->b_kl() = keyLen;
   memset (&this->syskey().all(), 0, keyLen);
   this->b_sl() = this->DefaultOffset - this->DefaultKeyLen + keyLen + infoLen;
}

//--------------------------------------------------------------------------

void Catalog_Object::Destroy (SAPDBMem_IRawAllocator& Allocator)
{
    switch (this->Type())
    {
    case TYPE_TABLE :
        {
            return reinterpret_cast<Catalog_TableObject*>(this)->Destroy(Allocator);
        }
    default :
        {
            Allocator.Deallocate (this);
        }
    }
}

//--------------------------------------------------------------------------

const void* Catalog_Object::GetInfo(int& infoLen) const
{
	infoLen = this->b_sl() - (this->DefaultOffset - this->DefaultKeyLen) - this->b_kl();
	return REINTERPRET_CAST(const unsigned char*, this) + (this->DefaultOffset - this->DefaultKeyLen) + this->b_kl();
}

//--------------------------------------------------------------------------

int Catalog_Object::PutInfo (void* pInfo, int infoLength)
{
	// TODO check length
	unsigned char* pDest = REINTERPRET_CAST(unsigned char*, this);
	pDest += this->DefaultOffset - this->DefaultKeyLen + this->b_kl();
	SAPDB_MemCopyNoCheck (pDest, pInfo, infoLength);
	return 0;
}

//--------------------------------------------------------------------------

int Catalog_Object::Type() const
{
  return syskey().sentrytyp()[1];
}

//--------------------------------------------------------------------------

void* Catalog_Object::CreateCopy (SAPDBMem_IRawAllocator& allocator, int offset, SAPDB_UInt4& memoryUsage) 
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_Object::CreateCopy", Catalog_Trace, 5);
	void* p = 0;
    switch (this->Type())
    {
    case TYPE_MESSBLOCK :
        {
            Catalog_MessBlockObject& MessBlock = *this;
            p = MessBlock.CreateCopy (allocator, offset, memoryUsage);
			break;
        }
    case TYPE_TABLE :
    case TYPE_RESULT:
        {
            Catalog_TableObject* pTable = REINTERPRET_CAST(Catalog_TableObject*, this);
            p = pTable->CreateCopy (allocator, offset, memoryUsage);
			break;
        }
    default :
        {
            SQLMAN_TRACE_BUFFER (Catalog_Trace, 5, "Copy Catalog Object", this, this->b_sl());
			memoryUsage = offset + this->b_sl();
            p = REINTERPRET_CAST(unsigned char*, allocator.Allocate(memoryUsage));
            if (p)
            {
                SAPDB_MemCopyNoCheck (REINTERPRET_CAST(unsigned char*, p) + offset, this, this->b_sl());
            }
        }
    }
	if (p)
	{
		memset (p, 0, offset);
	}
	return p;
}

//--------------------------------------------------------------------------

Catalog_Object* Catalog_Object::Incopy(SQLMan_Context& Context, const Catalog_Key& Key, SAPDBMem_IRawAllocator& Allocator, bool NoWait)
{
    switch (Key.sentrytyp()[1])
    {
    case TYPE_AUTHORIZATION :
        return Catalog_AuthorizationObject::Incopy(Context, Key.sauthid(), Allocator);
    case TYPE_AUTHORIZATIONREF :
        {
            SQLMan_Identifier name;
            reinterpret_cast<const Catalog_AuthorizationRefObject::Catalog_Key*>(&Key)->GetName(name);
            return Catalog_AuthorizationRefObject::Incopy(Context, name, Allocator);
        }
    case TYPE_CONTAINER :
        {
            return Catalog_ContainerObject::Incopy (Context, Key, Allocator, NoWait);
        }
    case TYPE_TABLE :
        {
            return Catalog_TableObject::Incopy(Context, Key.stableid(), Allocator);
        }
    default :
        {
            Context.ThrowError (e_not_implemented);
        }
    }
    return 0;
}

//--------------------------------------------------------------------------

void Catalog_Object::InvalidateTransaction(void* p, int offset)
{
   Catalog_Object* pObj = CATALOG_OBJECT_CAST(Catalog_Object, p, offset);
   if (TYPE_MESSBLOCK == pObj->Type())
   {
      Catalog_MessBlockObject& MessBlock = *pObj;
      MessBlock.InvalidateTransaction();
   }
}

//--------------------------------------------------------------------------

void Catalog_Object::Outcopy (SQLMan_Context& context, bool isUpdate)
{
    switch (this->Type())
    {
    case TYPE_AUTHORIZATION :
        {
            Catalog_AuthorizationObject& auth = *this;
            auth.Outcopy (context, isUpdate);
        }
    }
}