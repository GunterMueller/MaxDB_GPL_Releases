
#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
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

MemDbg_Callback.hpp     inc=MemDbg_Exports.hpp \
                        ->$INSTROOT/incl/MemoryDebugger/MemDbg_Callback.hpp
MemDbg_Callback.cpp     -DMEMDBG_IMPLEMENTATION inc=MemDbg_Callback.hpp,MemDbg_Interface.hpp

MemDbg_Config.hpp       ->$INSTROOT/incl/MemoryDebugger/MemDbg_Config.hpp
MemDbg_Config.cpp       -DMEMDBG_IMPLEMENTATION inc=MemDbg_Config.hpp,MemDbg_Header.hpp

MemDbg_Exports.hpp      ->$INSTROOT/incl/MemoryDebugger/MemDbg_Exports.hpp

MemDbg_Interface.hpp    inc=MemDbg_Exports.hpp \
                        ->$INSTROOT/incl/MemoryDebugger/MemDbg_Interface.hpp
MemDbg_Interface.cpp    -DMEMDBG_IMPLEMENTATION inc=MemDbg_Interface.hpp,MemDbg_Config.hpp,\
                        MemDbg_Callback.hpp,MemDbg_Header.hpp,MemDbg_Pure.h

MemDbg_PureApi.c        inc=

MemDbg_Test.cpp         inc=MemDbg_Interface.hpp,MemDbg_Callback.hpp,MemDbg_Config.hpp
