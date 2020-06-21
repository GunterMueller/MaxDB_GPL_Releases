# @(#)	jtest/mutsqldbc.com
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2004 SAP AG
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
#


*.pl ascii ->$TEST/jtest/mutsqldbc/ unpack=no concom=yes exec RELSTAT=$RELSTAT 
mutrun.pl ascii ->$TEST/jtest/mutsqldbc/ unpack=no concom=yes exec RELSTAT=$RELSTAT uncond
*.nt ascii ->$TEST/jtest/mutsqldbc/ unpack=no concom=yes
*.ux ascii ->$TEST/jtest/mutsqldbc/ unpack=no concom=yes
*.all ascii ->$TEST/jtest/mutsqldbc/ unpack=no concom=yes
*.txt ascii ->$TEST/jtest/mutsqldbc/ unpack=no concom=no
*.cfg ascii ->$TEST/jtest/mutsqldbc/ unpack=no concom=yes

*.cpp  inc=mut.h  unpack=no concom=no 


flushbuf.c inc=+flushbuf.h,:SAPDB/RunTime/System/RTESys_MSWindows.h

#&if $OS=WIN32
endloss.cpp inc=+flushbuf.h
iendlos.cpp inc=+flushbuf.h
rendlos.cpp inc=+flushbuf.h
#&endif

endloss.cpp inc=+fdircheck.h
rendlos.cpp inc=+fdircheck.h

mut.h inc=heo03.h,gsp01.h,\
	:SAPDB/RunTime/System/RTESys_MSWindows.h,\
	:SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h,\
	:SAPDB/Interfaces/Runtime/IFR_ConnectionItem.h,\
	:SAPDB/Interfaces/Runtime/IFR_Connection.h,\
	:SAPDB/Interfaces/SQLDBC/SQLDBC.h,\
	:jtest/mutsqldbc/allocator.h

allocator.h inc=:SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp

# on linux a compiler bug is possible and a local variable in the application contains a wrong value
&if $OSSPEC = LINUX
rendlos.cpp -NO
&endif	  
