#
# RunTime Environment / Security
#
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

*         -except
#
# No exception handling code variant
#
*-noex    -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
#

RTESec_Authentication.cpp inc=\
    :SAPDB/RunTime/Security/RTESec_Authentication.hpp,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp,\
    :SAPDB/RunTime/Security/Crypto/RTECrypto_RNG.hpp,\
    :SAPDB/RunTime/Security/RTESec_SCRAMMD5.h,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_VarData.hpp,\
    :SAPDB/RunTime/Security/RTESec_DumpHex.hpp
RTESec_Authentication.hpp inc=
RTESec_Authentication.h inc=
RTESec_Base64.c inc=
RTESec_Base64.h inc=gsp01.h,:SAPDB/RunTime/RTE_Types.h
RTESec_DumpHex.cpp inc=:SAPDB/RunTime/Security/RTESec_DumpHex.hpp
RTESec_DumpHex.hpp inc=
RTESec_HMACMD5.c inc=:SAPDB/RunTime/Security/RTESec_MD5.h,\
    :SAPDB/RunTime/Security/RTESec_HMACMD5.h
RTESec_HMACMD5.h inc=gsp01.h,:SAPDB/RunTime/RTE_Types.h,\
    :SAPDB/RunTime/Security/RTESec_MD5.h
RTESec_MCF.c inc=
RTESec_MCF.h inc=gsp01.h,:SAPDB/RunTime/RTE_Types.h
RTESec_MD5.c inc=:SAPDB/RunTime/Security/RTESec_MD5.h
RTESec_MD5.h inc=gsp01.h,:SAPDB/RunTime/RTE_Types.h

RTESec_SAPSSL.cpp inc=\
:sap/700/saptype.h,:sap/700/saptypeb.h,:sap/700/saptypec.h,\
    :sap/700/sapuc.h,:sap/700/sapuc2.h,:sap/700/err.h,:sap/700/nixx.h,\
    :sap/700/dptrace.h,:sap/700/ssslxx.h,:sap/700/sapwrap.h,:sap/700/dlxx.h,:sap/700/cdkapi.h,\
	geo007_2.h,heo47.h,:SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
	:SAPDB/RunTime/Security/RTESec_SSL.h,\
    :SAPDB/RunTime/Security/RTESec_SAPSSL.h,\
    :SAPDB/RunTime/System/RTESys_SharedLib.h,heo46.h
RTESec_SAPSSL-l.cpp -DSTUB_MODULE inc=\
:sap/700/saptype.h,:sap/700/saptypeb.h,:sap/700/saptypec.h,\
    :sap/700/sapuc.h,:sap/700/sapuc2.h,:sap/700/err.h,:sap/700/nixx.h,\
    :sap/700/dptrace.h,:sap/700/ssslxx.h,\
    :SAPDB/RunTime/Security/RTESec_SAPSSL.h,\
    :SAPDB/RunTime/System/RTESys_SharedLib.h,heo46.h,geo007_2.h
RTESec_SAPSSL.h inc=gsp01.h,:SAPDB/RunTime/RTE_Types.h
RTESec_SCRAMMD5.c inc=:SAPDB/RunTime/Security/RTESec_MD5.h,\
    :SAPDB/RunTime/Security/RTESec_HMACMD5.h,\
    :SAPDB/RunTime/Security/RTESec_SCRAMMD5.h,\
    :SAPDB/RunTime/Security/RTESec_DumpHex.hpp
RTESec_SCRAMMD5.h inc=gsp01.h,:SAPDB/RunTime/RTE_Types.h
RTESec_SCRAMTest.c inc=:SAPDB/RunTime/Security/RTESec_SCRAMMD5.h,\
    :SAPDB/RunTime/Security/RTESec_HMACMD5.h,\
    :SAPDB/RunTime/Security/RTESec_MD5.h,\
    :SAPDB/RunTime/Security/RTESec_MCF.h

RTESec_SSL-u.cpp	-DUSER -DSDB_LC_STATE=$LC_STATE inc=heo01.h,heo46.h,geo007_2.h,geo40.h,\
    :SAPDB/RunTime/Communication/RTEComm_ParseURI.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/RunTime/Security/RTESec_SAPSSL.h,\
    :SAPDB/RunTime/Security/RTESec_SSL.h,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h \
&if $OSSPEC not in [ WIN32 ]
    dep=RTE_saveUNIXcalls.h
&else
	dep=                                        
&endif

RTESec_SSL-s.cpp	-DSERVER inc=heo01.h,heo46.h,geo007_2.h,geo40.h,\
    :SAPDB/RunTime/Communication/RTEComm_ParseURI.hpp,\
    :SAPDB/RunTime/RTE_MessageList.hpp,\
    :SAPDB/RunTime/Security/RTESec_SAPSSL.h,\
    :SAPDB/RunTime/Security/RTESec_SSL.h,\
    :SAPDB/SAPDBCommon/SAPDB_sprintf.h \
&if $OSSPEC not in [ WIN32 ]
    dep=RTE_saveUNIXcalls.h
&else
	dep=                                        
&endif

RTESec_SSL.h inc=gsp01.h

