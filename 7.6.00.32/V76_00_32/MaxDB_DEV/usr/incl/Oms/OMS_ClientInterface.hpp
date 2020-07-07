/*!
 * \file    OMS_ClientInterface.hpp
 * \author  MarkusSi, Roterring
 * \brief   Clientinterface.
 */
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

#if !defined (__liveCache_INCLUDED)
#define __liveCache_INCLUDED

#if _MSC_VER > 1100
#define PLACEMENT_DELETE 1
#endif

#undef  AFX_EXT_CLASS
#define AFX_EXT_CLASS DECLSPEC_IMPORT

#include "Oms/OMS_Defines.h"

#include "ilivecachesink.h" /* no check */
#include "livecachebase.h"  /* no check */

DEFINE_GUID(IID_IliveCacheSink, 
            0x8580E621,0x243F,0x11d1,0xB5,0xA1,0xAC,0x7F,0x01,0x06,0x8C,0x36);

#define LC_VERSION  760032

// Register the class information in the session specific class directory
#define OMS_REGISTER_CLASS(a,h)      {a::omsRegClass(h, #a, new(h, OmsAbstractObject::USE_IN_REGISTRY_ONLY) a);}
#define OMS_REGISTER_VARCLASS(g,n,h) {(h).omsRegVarClass(g, n);}
// Old style inheritence, should not be used anymore 
#define OMS_REGISTER_INHCLASS(a,b,h) {a::omsRegClass(h, #a, new(h, OmsAbstractObject::USE_IN_REGISTRY_ONLY) a), b::omsGetGuid());

// Old style. should not be used any more. Containers are created if they do not exist already.
#define OMS_REG_CLASS(a,s,cno,h)                      {a::omsRegClass(h, #a, sizeof(a), new(h, OmsObject LIVECACHE_OBJECT_TMPLPAR(a, 0)::USE_IN_REGISTRY_ONLY) a, s,cno);}
#define OMS_REG_CLASS_WITH_PARTITIONED_KEY(a,s,cno,h) {a::omsRegClass(h, #a, sizeof(a), new(h, OmsObject LIVECACHE_OBJECT_TMPLPAR(a, 0)::USE_IN_REGISTRY_ONLY) a, s,cno,true);}
#define OMS_REG_CLASS_PREFIX(a,prefixSize,h)          {a::omsRegClass(h, #a, prefixSize, new(h, OmsObject LIVECACHE_OBJECT_TMPLPAR(a, 0)::USE_IN_REGISTRY_ONLY) a);}
#define OMS_REG_CLASS_INH(a,s,cno,b,h)                {a::omsRegClass(h, #a, sizeof(a), b::omsGetGuid(),new(h, OmsObject LIVECACHE_OBJECT_TMPLPAR(a, 0)::USE_IN_REGISTRY_ONLY) a, s,cno);}

extern
bool omsIsUnicodeInstance();

extern
bool omsIsOneDBSystem();

extern
void* omsMalloc(size_t size);

extern 
void omsFree(void* p);

extern
void* omsMallocArray(size_t size);

extern 
void omsFreeArray(void* p);

extern
void omsGetLiveCacheVersion(char*, int);

extern
unsigned int omsGetLCVersion();

extern
void omsSimConsoleMessage(const char*);

// include all other stuff directly
#include "Oms/OMS_IterDirection.hpp"
#include "Oms/OMS_CriticalSection.hpp"
#include "Oms/OMS_SimDumpCollector.hpp"
#include "Oms/OMS_Oid.hpp"
#include "Oms/OMS_VarOid.hpp"
#include "Oms/OMS_TypedOid.hpp"
#include "Oms/OMS_Exceptions.hpp"
#include "Oms/OMS_ObjByClsIter.hpp"
#include "Oms/OMS_KeyRangeIter.hpp"
#include "Oms/OMS_CallbackInterface.hpp"
#include "Oms/OMS_AllocatorInterface.hpp"
#include "Oms/OMS_GlobalAnchorObject.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_AbstractObject.hpp"
#include "Oms/OMS_Object.hpp"
#include "Oms/OMS_LockObject.hpp"
#include "Oms/OMS_Sequence.hpp"
#include "Oms/OMS_Stream.hpp"
#include "Oms/OMS_DbpCurrency.hpp"
#include "Oms/OMS_DbpLongCurrency.hpp" /* PTS 1123699 */
#include "Oms/OMS_ArrayObject.hpp"
#include "Oms/OMS_LockScopes.hpp"
#include "Oms/OMS_DateTime.hpp" /* PTS 1131538 */

#endif // __liveCache_INCLUDED
