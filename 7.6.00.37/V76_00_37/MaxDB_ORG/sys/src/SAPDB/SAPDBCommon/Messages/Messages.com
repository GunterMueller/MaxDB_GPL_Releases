# src/SAPDB/SAPDBCommon/Messages/Messages.com
# -> only generated header files for messages
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


* inc=Msg_List.hpp,Msg_Arg.hpp,Msg_SimpleArg.hpp
#
# Not automatically generated (used for testing new features of Msg_List and Msg_Arg...
#
SDBMsg_TestMessages.h inc=Msg_TestList.hpp,Msg_TestArg.hpp

# not automatically generated
SDBMsg_UniqueMessageNumber.hpp inc=
