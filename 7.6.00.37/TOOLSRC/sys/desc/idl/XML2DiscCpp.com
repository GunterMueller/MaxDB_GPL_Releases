# @(#)   desc/IdlChecker.com   73.03.xx  5-JUN-2000
#
#
#regex(.*\.c$) -I$WRK/incl/idl/xml2ispc
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
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#


X2DC_Attribute_Checker.hpp  inc=XML2DiscCpp.hpp
X2DC_Cpp_Type_Registry.hpp  inc=:idl/xml2ispc/Ispc_vsp001.hpp,XML2DiscCpp.hpp,\
                                 X2DC_WarnAndErrHandler.hpp
X2DC_WarnAndErrHandler.hpp  inc=:idl/expat/expat.h
X2DC_GenApplHeaderFile.hpp  inc=X2DC_Cpp_Type_Registry.hpp
X2DC_GenDisclosureCode.hpp  inc=X2DC_Cpp_Type_Registry.hpp

X2DC_WarnAndErrHandler.cpp  inc=X2DC_WarnAndErrHandler.hpp
X2DC_Attribute_Checker.cpp  inc=X2DC_Attribute_Checker.hpp,X2DC_WarnAndErrHandler.hpp
X2DC_Cpp_Type_Registry.cpp  inc=X2DC_Cpp_Type_Registry.hpp
X2DC_GenApplHeaderFile.cpp  inc=X2DC_GenApplHeaderFile.hpp
X2DC_GenDisclosureCode.cpp  inc=X2DC_GenDisclosureCode.hpp

XML2DiscCpp.cpp             inc=XML2DiscCpp.hpp,:idl/expat/expat.h,\
                                X2DC_Attribute_Checker.hpp,X2DC_Cpp_Type_Registry.hpp,\
                                X2DC_GenApplHeaderFile.hpp,X2DC_GenDisclosureCode.hpp
