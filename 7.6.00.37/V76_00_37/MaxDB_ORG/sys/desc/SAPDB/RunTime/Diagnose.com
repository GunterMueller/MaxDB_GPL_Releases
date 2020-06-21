#
# RunTime Environment Descriptions
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

* -ansi
*-k -DKERNEL_LZU
#
RTEDiag_ServerCrashHist.c -DKERNEL_LZU
#
# include dependencies (do not edit)
#
RTEDiag_ClientCrashHist.c inc=gsp00,geo007_1.h\
,:SAPDB/RunTime/RTE_Types.h\
,:SAPDB/RunTime/Diagnose/RTEDiag_ClientCrashHist.h\
,:SAPDB/RunTime/Diagnose/RTEDiag_CommonCrashHist.h
RTEDiag_ClientCrashHist.h inc=
RTEDiag_CommonCrashHist*.c inc=gsp00,geo007_1.h\
,:SAPDB/RunTime/RTE_Types.h\
,:SAPDB/RunTime/Diagnose/RTEDiag_CommonCrashHist.h,geo50_0.h,hsp77.h,geo57.h,SAPDB_string.h\
,heo15.h\
&if $OSSPEC not in [ WIN32 ]
,hen40.h\
&endif

RTEDiag_CommonCrashHist.h inc=
RTEDiag_ServerCrashHist.c inc=:SAPDB/RunTime/RTE_Types.h,gsp00,geo00.h\
,geo00_2.h,geo002.h,geo007_1.h,heo11.h\
,:SAPDB/RunTime/Diagnose/RTEDiag_ServerCrashHist.h\
,:SAPDB/RunTime/Diagnose/RTEDiag_CommonCrashHist.h,RTESys_IO.h\
,heo47.h,heo52.h,geo50_0.h,hsp77.h,geo57.h,:SAPDB/RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h,SAPDB_Names.h\
,KernelParam_RunTime.h,SAPDB_string.h\
&if $OSSPEC = WIN32
,gos00.h\
&endif

RTEDiag_ServerCrashHist.h inc=

RTEDiag_Appldiag.h inc=RTE_Types.h

RTEDiag_Appldiag.cpp inc=RTESys_Time.h,RTEDiag_Appldiag.h,hsp77.h,SAPDB_MemCopyMoveNoCheck.hpp

RTEDiag_SymbolResolutionLock.h inc=SAPDB_Types.h
RTEDiag_SymbolResolutionLock.cpp inc=RTEDiag_SymbolResolutionLock.h,\
    RTESync_NamedSpinlock.hpp,\
    RTESys_Spinlock.h,\
    RTEMem_RteAllocator.hpp,\
    SAPDBMem_NewDestroy.hpp,\
    geo002.h,\
    geo50_0.h
