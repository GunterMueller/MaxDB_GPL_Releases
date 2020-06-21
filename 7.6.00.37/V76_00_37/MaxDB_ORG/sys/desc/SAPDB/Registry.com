#
# desc/SAPDB/Registry.com
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

# Interface
Reg_Registry.h		inc=Reg_Common.h
#&if $OS = UNIX
Reg_Registry.c		inc=Reg_Registry.h,Reg_Win.h,Reg_Unix.h
#&else
#Reg_Registry.c		inc=Reg_Registry.h,Reg_Win.h
#&endif

# Unix abstraction layer
#&if $OS in [UNIX]
Reg_Unix.h			inc=:SAPDB/Registry/Reg_Common.h
Reg_Unix.c			inc=hwd03waapi.h,Reg_Unix.h
#&endif

# Windows abstraction layer
#&if $OS in [WIN32]
Reg_Win.h			inc=Reg_Common.h
Reg_Win.c			inc=Reg_Win.h
#&endif

# Common functions
Reg_Common.h		inc=SAPDB_Types.h,hwd03waapi.h,gsp05
Reg_Common.c		inc=Reg_Common.h


