
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

lvcbenchObj.h      dep=OMS_DbpGlobals.hpp,OMS_ClientInterface.hpp

lvcbench.xml lang=r dep=lvcbenchObj.h obj=lvcbenchIspc.o \
             exdep=$WRK/incl/lvcbench.h,$WRK/incl/lvcbench_i.c \
             unpack=no noversion \
             -xml2ispc -notdc
&if ($OSSPEC!=WIN32 or defined BIT64) and not ($MACH == SUN || $OSSPEC == SOLARIS)
lvcbench.xml -novalidate
&endif

lvcbenchMain.cpp	 -except dep=lvcbench.xml \
                   inc=lvcbenchObj.h
lvcbenchObj.cpp		 -except dep=lvcbench.xml inc=lvcbenchObj.h,\
                   :livecache/lctest/lctestversioniter.h,OMS_MassDerefIterators.hpp


lvcBench_74.cfg ->$OWN/test/jtest/tests/ ascii
lvcBench_reference.cfg ->$OWN/test/jtest/tests/ ascii
lvcBench_reference_74.cfg ->$OWN/test/jtest/tests/ ascii
lvcBench.seq ->$OWN/test/jtest/tests/ ascii
lvcBench_reference.seq ->$OWN/test/jtest/tests/ ascii
