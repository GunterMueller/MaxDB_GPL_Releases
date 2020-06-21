#*****************************************************************************
#
#  module      : XML2LcaCode.com
#
#  -------------------------------------------------------------------------
#
#  responsible : PeterG (GoertzP)
#
#  special area: XML To Application Code
#
#  -------------------------------------------------------------------------
#
#  copyright:    Copyright by SAP AG, 2000
#
#
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
#*****************************************************************************


XML2LcaCode.hpp            inc=:idl/expat/expat.h

X2AC_GenApplCode.hpp       inc=XML2LcaCode.hpp

X2AC_GenApplCode.cpp       inc=X2AC_GenApplCode.hpp,X2AC_WarnAndErrHandler.hpp

X2AC_WarnAndErrHandler.cpp inc=X2AC_WarnAndErrHandler.hpp

XML2LcaCode.cpp            inc=XML2LcaCode.hpp,\
                               X2AC_GenApplCode.hpp,X2AC_WarnAndErrHandler.hpp



