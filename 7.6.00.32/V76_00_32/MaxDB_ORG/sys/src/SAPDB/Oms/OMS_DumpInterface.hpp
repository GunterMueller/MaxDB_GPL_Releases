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
#ifndef __OMS_DUMPINTERFACE_HPP
#define __OMS_DUMPINTERFACE_HPP

#define LABEL_OMS_CLASS "OMSCLASS", 1901
#define LABEL_OMS_CONTAINER "CONTAINER", 1902
#define LABEL_OMS_VERSION "OMSVERS", 1903
#define LABEL_OMS_CONTEXT "OMSCNTXT", 1904
#define LABEL_OMS_SESSION "OMSSESS", 1905
#define LABEL_OMS_BEFORE_IMAGE "OMSBIMAGE", 1906
#define LABEL_OMS_OID_CACHE "OMSCACHE", 1907
#define LABEL_OMS_LOCAL_CLS_DIR "OMSLOCAL", 1908
#define LABEL_OMS_LOCAL_CLS_HASH "OMSLCLS ", 1909
#define LABEL_OMS_LOCAL_GUID_HASH "OMSLGUID", 1910
#define LABEL_OMS_LOCAL_CLASS "OMSLCLS", 1911
#define LABEL_OMS_BOUND_VERSION "OMSBNDV", 1912

class OMS_DumpInterface {
public :
  virtual void* GetOmsSession() = 0;
  virtual void  SetDumpLabel (const char* label, int dumpType) = 0;
  virtual void  Dump (void* p, int len) = 0;
  virtual bool  Synchronize() const = 0;
};

#endif  // __OMS_DUMPINTERFACE_HPP