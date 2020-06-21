#
#    ========== licence begin LGPL
#    Copyright (C) 2000 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
#    ========== licence end
#

Ispc_geo36.h                     inc=Ispc_vsp001.hpp
IspcCGenHFile.h                  inc=Ispc_InterfaceTypeList.hpp,Ispc_InterfaceStructRegistry.hpp,Ispc_InterfaceConstList.hpp
IspcCGenInspecCode.h             inc=IspcIdl2Code.h,Ispc_ggg250.h,Ispc_ggg251.h
Ispc_InterfaceTypeList.cpp       inc=Ispc_InterfaceTypeList.hpp
Ispc_InterfaceStructRegistry.cpp inc=Ispc_InterfaceStructRegistry.hpp
Ispc_InterfaceConstList.cpp      inc=Ispc_InterfaceConstList.hpp,IspcErrorHandling.h
IspcCGenHFile.cpp                inc=IspcCGenHFile.h,IspcIdl2Code.h,IspcErrorHandling.h
IspcCGenInspecCode.cpp           inc=IspcCGenInspecCode.h,IspcErrorHandling.h,Ispc_geo36.h,IspcVersion.h
IspcErrorHandling.cpp            inc=IspcErrorHandling.h
IspcIdl2Code.cpp                 inc=:idl/expat/expat.h,IspcErrorHandling.h,IspcIdl2Code.h,IspcCGenHFile.h,IspcCGenInspecCode.h,\
                                     IspcVersion.h \
                                 dep=IspcVersion.h -DXML_BYTE_ORDER=21
