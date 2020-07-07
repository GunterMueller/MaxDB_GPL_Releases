# ------------------------------------------------------------------------------
#
# KernelCommon ParameterNames
#
# ------------------------------------------------------------------------------
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

# include dependencies (do not edit)
#
KernelParam_Converter.hpp inc=
KernelParam_DataAccess.hpp inc=
KernelParam_DataCache.hpp inc=
KernelParam_FileDir.hpp inc=
KernelParam_FrameControl.hpp inc=
KernelParam_IOManager.hpp inc=
KernelParam_Join.hpp inc=
KernelParam_Kernel.hpp inc=
KernelParam_Pager.hpp inc=
KernelParam_QueryRewrite.hpp inc=
KernelParam_RunTime.h inc=
KernelParam_SharedSQL.hpp inc=
KernelParam_TempHashTable.hpp inc=

