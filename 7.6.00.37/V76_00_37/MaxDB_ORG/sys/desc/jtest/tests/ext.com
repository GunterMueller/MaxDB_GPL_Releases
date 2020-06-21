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

&if defined SAPDB_INTERNAL

DBMTest.seq ->$OWN/test/jtest/tests/ext/ ascii
DBMTest.cfg ->$OWN/test/jtest/tests/ext/ ascii
oltptest.seq ->$OWN/test/jtest/tests/ext/ ascii
oltptest.cfg ->$OWN/test/jtest/tests/ext/ ascii concom=yes
odbc.seq ->$OWN/test/jtest/tests/ext/ ascii
odbc.cfg ->$OWN/test/jtest/tests/ext/ ascii
oltptest_unicode.seq ->$OWN/test/jtest/tests/ext/ ascii
oltptest_unicode.cfg ->$OWN/test/jtest/tests/ext/ ascii
odbc_unicode.seq ->$OWN/test/jtest/tests/ext/ ascii
odbc_unicode.cfg ->$OWN/test/jtest/tests/ext/ ascii
sut.seq ->$OWN/test/jtest/tests/ext/ ascii concom=yes \
&if defined LC_STATE
	LC_STATE=$LC_STATE
&else
	LC_STATE=DEV
&endif 
sut.cfg ->$OWN/test/jtest/tests/ext/ ascii
sqltest.seq ->$OWN/test/jtest/tests/ext/ ascii
sqltest.cfg ->$OWN/test/jtest/tests/ext/ ascii
mutsqldbc.seq ->$OWN/test/jtest/tests/ext/ ascii
mutsqldbc.cfg ->$OWN/test/jtest/tests/ext/ ascii

&else

DBMTest.seq ->$OWN/test/jtest/tests/ ascii
DBMTest.cfg ->$OWN/test/jtest/tests/ ascii
oltptest.seq ->$OWN/test/jtest/tests/ ascii
oltptest.cfg ->$OWN/test/jtest/tests/ ascii concom=yes
odbc.seq ->$OWN/test/jtest/tests/ ascii
odbc.cfg ->$OWN/test/jtest/tests/ ascii
oltptest_unicode.seq ->$OWN/test/jtest/tests/ ascii
oltptest_unicode.cfg ->$OWN/test/jtest/tests/ ascii
odbc_unicode.seq ->$OWN/test/jtest/tests/ ascii
odbc_unicode.cfg ->$OWN/test/jtest/tests/ ascii
sut.seq ->$OWN/test/jtest/tests/ ascii concom=yes \
&if defined LC_STATE
	LC_STATE=$LC_STATE
&else
	LC_STATE=DEV
&endif 
sut.cfg ->$OWN/test/jtest/tests/ ascii
sqltest.seq ->$OWN/test/jtest/tests/ ascii
sqltest.cfg ->$OWN/test/jtest/tests/ ascii
mutsqldbc.seq ->$OWN/test/jtest/tests/ ascii
mutsqldbc.cfg ->$OWN/test/jtest/tests/ ascii

&endif
