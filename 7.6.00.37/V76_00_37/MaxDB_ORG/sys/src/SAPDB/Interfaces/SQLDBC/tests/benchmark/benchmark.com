# -----------------------------------------------------------------------------
# 
# PURPOSE
# Description for SQLDBC Benchmark Test 
# 
# RELEASE:  SAP DB 7.5
# 
# -----------------------------------------------------------------------------
#
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

* inc=:SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h
&ifdebug
* -DCATCH_ALL
&endif

&if $MACH in [ SUN ] || $OSSPEC == SOLARIS
regex(\w+\.cpp) -features=tmplife
&endif
sel100k.cpp inc=+heo02.h
