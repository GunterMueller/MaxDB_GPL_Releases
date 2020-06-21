#
#   FrameControl
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

regex(*\.hpp$)              inc=Kernel_Common.hpp

# ------------------------------------------------------------------------------
# Errors and messages:
# ------------------------------------------------------------------------------
#
FrameCtrl_Messages.genmsg       ascii

FrameCtrl_Messages.hpp          inc=SAPDBErr_MessageList.hpp

FrameCtrl_Exception.hpp         inc=SAPDBErr_MessageList.hpp

# -------------------------------------------------------------------------------------
# Interfaces and basic types:
# -------------------------------------------------------------------------------------
#

FrameCtrl_Types.hpp             inc=SAPDBTrace_Topic.hpp

FrameCtrl_ICommon.hpp           inc=Kernel_Common.hpp

FrameCtrl_IConverter.hpp        inc=Data_PageFrame.hpp

FrameCtrl_IIOManager.hpp        inc=Data_PageFrame.hpp

FrameCtrl_ILog.hpp              inc=Data_PageFrame.hpp

FrameCtrl_IDataCache.hpp        inc=Data_PageFrame.hpp

FrameCtrl_IFileDirectory.hpp    inc=Data_PageFrame.hpp

FrameCtrl_IRestartPage.hpp      inc=Data_PageFrame.hpp

FrameCtrl_IBlockAllocator.hpp   inc=Data_PageFrame.hpp,SAPDBMem_IBlockAllocator.hpp

FrameCtrl_ICreateIndex.hpp      inc=Data_PageFrame.hpp


# ------------------------------------------------------------------------------
# Manager:
# ------------------------------------------------------------------------------
#

FrameCtrl_Manager.hpp       inc=+FrameCtrl_ICommon.hpp,\
                                FrameCtrl_IConverter.hpp,\
                                FrameCtrl_IIOManager.hpp,\
                                FrameCtrl_ILog.hpp,\
                                FrameCtrl_IDataCache.hpp,\
                                FrameCtrl_IFileDirectory.hpp,\
                                FrameCtrl_Types.hpp,\
                                Data_PageFrame.hpp,\
                                FrameCtrl_IConverter.hpp,\
                                FrameCtrl_IRestartPage.hpp,\
                                FrameCtrl_IBlockAllocator.hpp,\
                                FrameCtrl_ICreateIndex.hpp,\
                                RTETask_ITask.hpp

&if $COMPUTERNAME == P116655 
FrameCtrl_Manager.cpp -DH01SAVEDEBUG
&endif

FrameCtrl_Manager.cpp       inc=+hbd20_3.h,\
                                 FrameCtrl_Exception.hpp,\
                                 FrameCtrl_Messages.hpp,\
                                 FrameCtrl_Manager.hpp,\
                                 KernelParam_FrameControl.hpp,\
                                 RTE_Crash.hpp,\
                                 RTEMem_Allocator.hpp,\
                                 RTEMem_PageAllocator.hpp,\
                                 RTEConf_ParameterAccess.hpp,\
                                 RTEConf_ParameterAccessKernelInterface.hpp,\
                                 SAPDB_ToString.hpp,\
                                 RTETask_ITask.hpp,\
                                 Kernel_VTrace.hpp
                                 
# ------------------------------------------------------------------------------
# Wrapper:
# ------------------------------------------------------------------------------
#

FrameCtrl_Wrapper.cpp       inc=gsp00,\
                                gsp03,\
                                gbd00,\
                                hgg01_1.h,\
                                FrameCtrl_Manager.hpp,\
                                FrameCtrl_ICommon.hpp,\
                                FrameCtrl_IDataCache.hpp



