# -----------------------------------------------------------------------------
# 
# PURPOSE
# Handling Description for files related to the C++ SQL class tests
# 
# RELEASE:  SAP DB 7.6 
# 
# CHANGES
# 12/05/2005 Burkhard:  First time CheckIn 

# -----------------------------------------------------------------------------
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

*x -DIMF
lcsqldbcObj.h dep=OMS_DbpGlobals.hpp

lcsqldbc.xml lang=r dep=lcsqldbcObj.h obj=lcsqldbcIspc.o \
             exdep=$WRK/incl/lcsqldbc.h,$WRK/incl/lcsqldbc_i.c \
             unpack=no noversion \
             -xml2ispc
&if ($OSSPEC!=WIN32 or defined BIT64) and not ($MACH == SUN || $OSSPEC == SOLARIS)
lcsqldbc.xml -novalidate
&endif

lcsqldbcMain.cpp -except dep=lcsqldbc.xml	inc=lcsqldbcObj.h
lcsqldbcObj.cpp	 -except dep=lcsqldbc.xml	inc=lcsqldbcObj.h


