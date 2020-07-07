/*!
 * \file    OMS_IliveCacheSpec.hpp
 * \author  PeterG (GoertzP)
 * \brief   liveCache special interface
 */

/*


    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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


#ifndef __OMS_IliveCacheSpec_hpp__
#define __OMS_IliveCacheSpec_hpp__


#include "livecachetypes.h" /* no check */


extern "C" {


#define hRes_ilcspec HRESULT


class IliveCacheSpec : public IUnknown
{
public:

    virtual hRes_ilcspec Register() = 0;

    virtual hRes_ilcspec sysSqlSupport(
        /* [out] */ BOOLEAN * sql_support) = 0;

    virtual hRes_ilcspec sysRequest(
        /* [out][in] */ int * lpbuflen,
        /* [size_is][out][in] */ unsigned char * lpv) = 0;

    virtual hRes_ilcspec sysMonitorInit() = 0;

    virtual hRes_ilcspec sysMonitorProlog(
        /* [in] */ int dispId,
        /* [size_is][in] */ unsigned char * lpMethodName) = 0;

    virtual hRes_ilcspec sysMonitorEpilog(
        /* [in] */ int dispId,
        /* [in] */ int runTime) = 0;

    virtual hRes_ilcspec Advise(
        /* [in] */ LPUNKNOWN pUnkSink) = 0;

    virtual hRes_ilcspec Unadvise() = 0;

    virtual hRes_ilcspec sysCleanUp() = 0;

};


} /* extern "C" */


#endif /* __OMS_IliveCacheSpec_hpp__ */

