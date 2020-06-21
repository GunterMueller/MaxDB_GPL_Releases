
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

MSSim_Coordinator.hpp    inc=MSSim_Dispatcher.hpp,MSSim_Resource.hpp,MSSim_Task.hpp
MSSim_Coordinator.cpp    -except inc=MSSim_Coordinator.hpp,MSSim_PatchedSysFunc.hpp,MSSim_Task.hpp,MSSim_Trace.hpp

MSSim_List.hpp           inc=

MSSim_Dispatcher.hpp     inc=MSSim_List.hpp,MSSim_Task.hpp
MSSim_Dispatcher.cpp     -except inc=MSSim_Dispatcher.hpp,MSSim_Coordinator.hpp,MSSim_Trace.hpp,MSSim_Task.hpp,MSSim_PatchedSysFunc.hpp

MSSim_Trace.hpp          inc=

MSSim_PatchedSysFunc.hpp inc=MSSim_Coordinator.hpp
MSSim_PatchedSysFunc.cpp -except inc=MSSim_PatchedSysFunc.hpp

MSSim_Resource.hpp       inc=MSSim_List.hpp
MSSim_Resource.cpp       -except inc=MSSim_Resource.hpp,MSSim_Dispatcher.hpp

MSSim_Task.hpp           inc=MSSim_Resource.hpp
MSSim_Task.cpp           -except inc=MSSim_Task.hpp

MSSim_Trace.hpp          inc=MSSim_Coordinator.hpp

MSSim_WinIATHook.hpp     inc=
MSSim_WinIATHook.cpp     -except inc=MSSim_WinIATHook.hpp,MSSim_Dispatcher.hpp,MSSim_PatchedSysFunc.hpp

MSSim_WinPatchedSysFunc.cpp -except inc=MSSim_WinIATHook.hpp,MSSim_PatchedSysFunc.hpp,MSSim_Trace.hpp -OPTADD=-Od -OPTSUB=-O2

MSSim_Interface.hpp      inc=
