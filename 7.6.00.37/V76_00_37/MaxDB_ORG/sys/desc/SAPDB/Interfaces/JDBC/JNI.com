# @(#)   desc/SAPDB/Interfaces/JDBC/JNI.com   7.3   11/02/2002
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

#
# include dependencies (do not edit)
#
nativeComm.c inc=:extern/jni/jni.h,gsp00,heo102.h,heo03.h,gsp09.h
nativeCommNew.cpp inc=:extern/jni/jni.h,gsp00,heo02.h,\
    :SAPDB/RunTime/ClientKernelCommunication/RTECKC_ClientCommunication.hpp,\
    :SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp

