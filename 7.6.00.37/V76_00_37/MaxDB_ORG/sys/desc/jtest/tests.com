
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

InfoPar.txt ->$OWN/test/jtest/tests/parameters/ ascii

*.cfg ascii concom=no ->$OWN/test/jtest/tests/ \
&if defined SAPDB_PURPOSE
    PURPOSE=$SAPDB_PURPOSE \
&else
    PURPOSE=ALL \
&endif
&if defined LC_STATE
    LC_STATE=$LC_STATE
&else
    LC_STATE=DEV
&endif 

*.seq ascii concom=no ->$OWN/test/jtest/tests/ \
&if defined SAPDB_PURPOSE
    PURPOSE=$SAPDB_PURPOSE \
&else
    PURPOSE=ALL \
&endif
&if defined LC_STATE
    LC_STATE=$LC_STATE
&else
    LC_STATE=DEV
&endif 

oltptest.cfg concom=yes
ifatest.cfg concom=yes
sut.seq concom=yes
sut_weekend.seq concom=yes
sut_bln.cfg concom=yes
sut_bln.seq concom=yes
sut_weekend_bln.cfg concom=yes
sut_weekend_bln.seq concom=yes
oltptest_pltf.cfg concom=yes
pretest_lc.seq concom=yes uncond
clients.cfg concom=yes
lcapps_night.cfg concom=yes
syncmantests.cfg concom=yes
syncmantests.seq concom=yes 
perltest.seq concom=yes
perltest.cfg concom=yes

DBMTest.cfg nocomfile setbuildinfo ascii ->$TEST/jtest/tests/

tests.pfl ->$WRK/config/ ascii concom=yes uncond \
&if defined SAPDB_PURPOSE
    PURPOSE=$SAPDB_PURPOSE \
&else
    PURPOSE=ALL \
&endif
&if defined LC_STATE
    LC_STATE=$LC_STATE
&else
    LC_STATE=DEV
&endif 
