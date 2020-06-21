
#
# include dependencies (do not edit)
#
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

PkgCheck_Checker.cpp inc=\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Checker.hpp,gsp00,vsp001,\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Iterator.hpp,\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Util.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_VarData.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp
PkgCheck_Checker.hpp inc=
PkgCheck_Iterator.cpp inc=\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Iterator.hpp,\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Util.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp
PkgCheck_Iterator.hpp inc=vsp001
PkgCheck_SimpleTests.cpp inc=\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_SimpleTests.hpp,vsp001
PkgCheck_SimpleTests.hpp inc=\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Test.hpp
PkgCheck_Test.cpp inc=\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Test.hpp,vsp001,\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Util.hpp,\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Checker.hpp,\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Iterator.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp
PkgCheck_Test.hpp inc=
PkgCheck_TestMain.cpp inc=\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Test.hpp,\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_SimpleTests.hpp
PkgCheck_Util.cpp inc=\
    :SAPDB/PacketInterface/PacketCheck/PkgCheck_Util.hpp,vsp001
PkgCheck_Util.hpp inc=

