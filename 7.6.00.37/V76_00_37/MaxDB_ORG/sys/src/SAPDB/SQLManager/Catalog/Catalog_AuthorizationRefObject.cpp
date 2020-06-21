/*

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


*/

#include "SAPDB/SQLManager/SQLMan_Types.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_IAuthorization.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_AuthorizationRefObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp"
#include "SAPDB/SQLManager/SQLMan_Context.hpp"

#include "SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

//-----------------------------------------------------------------------------

Catalog_AuthorizationRefObject::Catalog_Key::Catalog_Key(const SQLMan_Identifier& Name)
{
     SAPDB_MemCopyNoCheck (&this->stableid()[0], &Name[0], sizeof(this->stableid()));
     this->sentrytyp()[0] = 0;
     this->sentrytyp()[1] = TYPE_AUTHORIZATIONREF;
     this->slinkage()[0]  = 0;
     this->slinkage()[1]  = 1;
     SAPDB_MemCopyNoCheck (&this->sidentifier()[0], &Name[sizeof(this->stableid())], sizeof(Name) - sizeof(this->stableid()));
     this->skeylen = sizeof(this->sentrytyp()) + sizeof(this->slinkage()) + sizeof(Name);
}

//-----------------------------------------------------------------------------

void Catalog_AuthorizationRefObject::Catalog_Key::GetName(SQLMan_Identifier& Name) const
{
     SAPDB_MemCopyNoCheck (&Name[0], &this->stableid()[0], sizeof(this->stableid()));
     SAPDB_MemCopyNoCheck (&Name[sizeof(this->stableid())], &this->sidentifier()[0], 
         sizeof(Name) - sizeof(this->stableid()));
}

//-----------------------------------------------------------------------------

Catalog_AuthorizationRefObject::Catalog_AuthorizationRefObject(const SQLMan_Identifier Name, const SQLMan_Surrogate AuthId)
: Catalog_Object(0,0)
{
     new((void*) this) Catalog_Key(Name);
     suserref().ru_user_id = AuthId;
}

//-----------------------------------------------------------------------------

Catalog_Object* Catalog_AuthorizationRefObject::Incopy(SQLMan_Context& Context, const SQLMan_Identifier& Name, SAPDBMem_IRawAllocator& Allocator)
{
    const bool cUseCache = true;
    Catalog_UserObjectPascal authObject;
    if (Catalog_IAuthorization::GetInstance().LoadAuthorizationObject (Context, Name, cUseCache, &authObject))
    {
        Catalog_AuthorizationRefObject* pObj = reinterpret_cast<Catalog_AuthorizationRefObject*>(Allocator.Allocate(SizeOf()));
        if (pObj)
        {
            return new((void*) pObj) Catalog_AuthorizationRefObject(Name, 
                reinterpret_cast<Catalog_AuthorizationObject*>(&authObject)->GetObjectId());
        }
        else
        {
            Context.GetErrorList().AppendNewMessage (Msg_List(Msg_List::Error, SDBMSG_CATALOG_NO_MEMORY, Msg_OptArg("MEMREQUEST",SizeOf())));
//            Context.SetNoMoreMemoryError(__FILE__, __LINE__);
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------

int Catalog_AuthorizationRefObject::SizeOf()
{
    return sizeof(tak_userrefrecord);
}
