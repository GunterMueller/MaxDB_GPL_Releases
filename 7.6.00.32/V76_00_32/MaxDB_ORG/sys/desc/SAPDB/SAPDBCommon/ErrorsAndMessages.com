# --------------------------------------------------------------------------
# contents: Dependencies of files in component 'SAPDB/SAPDBCommon/ErrorsAndMessages/'.
# to do:    Reduce the dependencies by using forward declarations!
# --------------------------------------------------------------------------
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

# Compile all modules with exception handling enabled
regex(.*\.cpp$)    -except

SAPDBErr_MessageList.hpp   inc=Msg_List.hpp

SAPDBErr_Assertions.hpp    inc=SAPDBErr_MessageList.hpp,\
                               SAPDB_ToString.hpp,\
                               RTE_CallDebugger.h,\
                               RTE_Crash.hpp \
                           dep=SAPDBErr_Messages.hpp

SAPDBErr_MessageList.cpp   inc=SAPDB_Types.hpp,SAPDBErr_MessageList.hpp,SAPDBMem_Alloca.h,SAPDBErr_MessageList.hpp,RTE_ISystem.hpp,RTEMem_RteAllocator.hpp,SAPDBMem_NewDestroy.hpp,Msg_List.hpp,SAPDB_string.h


SAPDBErr_MessageEventConverter.hpp  inc=geo200.h,SAPDBErr_MessageList.hpp
SAPDBErr_MessageEventConverter.cpp  inc=SAPDBErr_MessageEventConverter.hpp

SAPDBErr_Messages.genmsg   ascii

SAPDBErr_Messages.hpp      inc=SAPDBErr_MessageList.hpp
SAPDBErr_MessageOutput.h   inc=SAPDB_Types.h
SAPDBErr_MessageOutput.c   inc=SAPDBErr_MessageOutput.h

SAPDBErr_Diagnostic.hpp    inc=SAPDB_Types.hpp,RTESync_Spinlock.hpp
SAPDBErr_Diagnostic.cpp    inc=SAPDBMem_NewDestroy.hpp,SAPDBErr_Diagnostic.hpp,RTESync_Factory.hpp,SAPDBErr_Assertions.hpp,RTE_EmergencyChannel.hpp,RTE_IGlobal.hpp,RTEMem_Allocator.hpp

SAPDBErr_Test.cpp          inc=Test_Program.hpp,\
                               SAPDBErr_MessageList.hpp,SAPDBErr_Assertions.hpp

SAPDBErr_Contracts.cpp     inc=SAPDBErr_Contracts.hpp


