# -----------------------------------------------------------------------------
# 
# PURPOSE
# Handling Description for Interface tests
# 
# RELEASE:  SAP DB 8.0
# 
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

* -I$OWN/sys/wrk/sdk/incl
* inc=:SAPDB/Loader/Loader_Loader.hpp,:SAPDB/Loader/FastloaderTests/FastloaderTestUtils.h
&ifdebug
* -DCATCH_ALL
&endif

&if $MACH in [ SUN ]
regex(\w+\.cpp) -features=tmplife
&endif



#
# include dependencies (do not edit)
#
FastloaderTestMain.h inc=
FastloaderTestUtils.cpp inc=:SAPDB/Loader/Loader_Loader.hpp,\
    :SAPDB/Loader/FastloaderTests/FastloaderTestUtils.h,\
    :SAPDB/RunTime/MemoryManagement/RTEMem_RawAllocator.hpp
FastloaderTestUtils.h inc=:SAPDB/Interfaces/sapdbifr.h,\
    :SAPDB/Interfaces/tests/Util.h,\
    :SAPDB/Loader/FastloaderTests/FastloaderTestMain.h,\
    :SAPDB/Loader/Loader_Loader.hpp
MultiInitialize.cpp inc=:SAPDB/Interfaces/sapdbifr.h,\
    :SAPDB/Loader/Loader_Types.hpp,:SAPDB/Loader/Loader_Loader.hpp,\
    :SAPDB/Loader/FastloaderTests/FastloaderTestUtils.h
SimpleFLCommand.cpp inc=:SAPDB/Interfaces/sapdbifr.h,\
    :SAPDB/Loader/Loader_Loader.hpp,\
    :SAPDB/Loader/FastloaderTests/FastloaderTestUtils.h
SimpleGetVersion.cpp inc=:SAPDB/Interfaces/sapdbifr.h,\
    :SAPDB/Loader/Loader_Loader.hpp,\
    :SAPDB/Loader/FastloaderTests/FastloaderTestUtils.h

