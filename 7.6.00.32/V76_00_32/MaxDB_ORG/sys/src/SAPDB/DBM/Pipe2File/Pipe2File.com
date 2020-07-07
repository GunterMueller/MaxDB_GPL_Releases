# *******************************************************************************
#  module:       Pipe2File.com
#  ------------------------------------------------------------------------------
#  responsible:  TiloH
#  special area: Database Manager's pipe2file program
#  description:  compilation description 
#  see also:     
#  ------------------------------------------------------------------------------
#                         Copyright by SAP AG, 2002
# *******************************************************************************
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2002-2005 SAP AG
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

DBMP2F_TransferBuffer.hpp   inc=DBMP2F_DeviceDirection.hpp
DBMP2F_TransferBuffer.cpp   inc=DBMP2F_TransferBuffer.hpp


DBMP2F_DeviceDirection.cpp  inc=DBMP2F_DeviceDirection.hpp

DBMP2F_Device.hpp           inc=DBMP2F_DeviceDirection.hpp,DBMP2F_TransferBuffer.hpp,DBMP2F_EventChecker.hpp
DBMP2F_Device.cpp           inc=DBMP2F_Device.hpp

DBMP2F_Null.hpp             inc=DBMP2F_Device.hpp
DBMP2F_Null.cpp             inc=DBMP2F_Null.hpp

DBMP2F_NamedDevice.hpp      inc=DBMP2F_Device.hpp
DBMP2F_NamedDevice.cpp      inc=DBMP2F_NamedDevice.hpp,SAPDB_string.h

DBMP2F_File.hpp             inc=hni34.h,DBMP2F_NamedDevice.hpp
DBMP2F_File.cpp             inc=DBMP2F_File.hpp

DBMP2F_VersionedFile.hpp    inc=hcn36.h,hni34.h,DBMP2F_NamedDevice.hpp
DBMP2F_VersionedFile.cpp    inc=DBMP2F_VersionedFile.hpp,SAPDB_string.h

DBMP2F_Pipe.hpp             inc=hni34.h,DBMP2F_NamedDevice.hpp
DBMP2F_Pipe.cpp             inc=DBMP2F_Pipe.hpp

DBMP2F_Test.hpp             inc=DBMP2F_Device.hpp,hcn36.h
DBMP2F_Test.cpp             inc=DBMP2F_Test.hpp,SAPDB_string.h

DBMP2F_CompareDevice.hpp    inc=DBMP2F_Device.hpp
DBMP2F_CompareDevice.cpp    inc=DBMP2F_CompareDevice.hpp


DBMP2F_DataHandler.hpp  inc=hcn36.h
DBMP2F_DataHandler.cpp  inc=DBMP2F_DataHandler.hpp

DBMP2F_DataHandlerWithDevice.hpp  inc=DBMP2F_DataHandler.hpp,DBMP2F_Device.hpp,DBMP2F_TransferBuffer.hpp,DBMP2F_EventChecker.hpp,DBMP2F_Event.hpp
DBMP2F_DataHandlerWithDevice.cpp  inc=DBMP2F_DataHandlerWithDevice.hpp

DBMP2F_DataSource.hpp   inc=DBMP2F_TransferBuffer.hpp,DBMP2F_DataHandlerWithDevice.hpp
DBMP2F_DataSource.cpp   inc=DBMP2F_DataSource.hpp

DBMP2F_DataSink.hpp     inc=DBMP2F_TransferBuffer.hpp,DBMP2F_DataHandlerWithDevice.hpp
DBMP2F_DataSink.cpp     inc=DBMP2F_DataSink.hpp

DBMP2F_DataComparer.hpp inc=hcn36.h,DBMP2F_TransferBuffer.hpp,DBMP2F_DataSink.hpp #,DBMP2F_CompareDevice.hpp
DBMP2F_DataComparer.cpp inc=DBMP2F_DataComparer.hpp


DBMP2F_Controller.hpp    inc=

DBMP2F_TransferController.hpp    inc=DBMP2F_Controller.hpp,DBMP2F_TransferBuffer.hpp,DBMP2F_DataSource.hpp,DBMP2F_DataSink.hpp
DBMP2F_TransferController.cpp    inc=DBMP2F_TransferController.hpp

DBMP2F_CompareController.hpp     inc=DBMP2F_Controller.hpp,DBMP2F_TransferBuffer.hpp,DBMP2F_DataSource.hpp,DBMP2F_DataComparer.hpp
DBMP2F_CompareController.cpp     inc=DBMP2F_CompareController.hpp


DBMP2F_Event.hpp                inc=hni31.h,DBMP2F_Reaction.hpp
DBMP2F_Event.cpp                inc=DBMP2F_Event.hpp

DBMP2F_Reaction.hpp             inc=hcn36.h
DBMP2F_Reaction.cpp             inc=DBMP2F_Reaction.hpp

DBMP2F_ReactionDelay.hpp        inc=DBMP2F_Reaction.hpp
DBMP2F_ReactionDelay.cpp        inc=DBMP2F_ReactionDelay.hpp,hcn36.h

DBMP2F_ReactionSignalDone.hpp   inc=DBMP2F_Reaction.hpp
DBMP2F_ReactionSignalDone.cpp   inc=DBMP2F_ReactionSignalDone.hpp

DBMP2F_ReactionWait.hpp         inc=DBMP2F_Reaction.hpp
DBMP2F_ReactionWait.cpp         inc=DBMP2F_ReactionWait.hpp

DBMP2F_ReactionRemoveFile.hpp   inc=DBMP2F_Reaction.hpp
DBMP2F_ReactionRemoveFile.cpp   inc=DBMP2F_ReactionRemoveFile.hpp,hni34.h

DBMP2F_ReactionShowSpeed.hpp    inc=DBMP2F_Reaction.hpp
DBMP2F_ReactionShowSpeed.cpp    inc=DBMP2F_ReactionShowSpeed.hpp

DBMP2F_ReactionMessage.hpp      inc=DBMP2F_Reaction.hpp
DBMP2F_ReactionMessage.cpp      inc=DBMP2F_ReactionMessage.hpp

DBMP2F_EventChecker.hpp         inc=hni31.h,DBMP2F_Event.hpp,DBMP2F_Reaction.hpp
DBMP2F_EventChecker.cpp         inc=DBMP2F_EventChecker.hpp

DBMP2F_Main.cpp inc=hcn36.h,hni32.h,DBMP2F_TransferController.hpp,DBMP2F_CompareController.hpp,DBMP2F_Pipe.hpp,DBMP2F_File.hpp,DBMP2F_VersionedFile.hpp,DBMP2F_Null.hpp,DBMP2F_Test.hpp,DBMP2F_Event.hpp,DBMP2F_ReactionDelay.hpp,DBMP2F_ReactionWait.hpp,DBMP2F_ReactionSignalDone.hpp,DBMP2F_ReactionRemoveFile.hpp,DBMP2F_ReactionShowSpeed.hpp,DBMP2F_ReactionMessage.hpp

DBMP2F_Icon.ico binary ->$WRK/incl/SAPDB/
DBMP2F_Resource.rc inc=gsp100.h dep=DBMP2F_Icon.ico
