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

*.par     ->$OWN/test/jtest/tests/parameters/ ascii
*.param   ->$OWN/test/jtest/tests/parameters/%basename.par ascii
*.ramp ascii

Precompiler.par      concom=yes
PrecompilerUni.par   concom=yes
Odbc.par             concom=yes
OdbcUni.par          concom=yes
RecoverFromPreviousBuild.par concom=yes \
&if defined LC_STATE
LC_STATE=$LC_STATE
&else
LC_STATE=DEV
&endif 
sut.par              setbuildinfo dep=:config/Buildinfo/VERSIONS concom=yes
sut_weekend.par      setbuildinfo dep=:config/Buildinfo/VERSIONS
sut.par.ramp         ->$OWN/test/jtest/tests/parameters/sut.par setbuildinfo dep=:config/Buildinfo/VERSIONS
sut_weekend.par.ramp ->$OWN/test/jtest/tests/parameters/sut_weekend.par setbuildinfo dep=:config/Buildinfo/VERSIONS
SQLDBCTest.par       concom=yes
sut_bln.par	     setbuildinfo dep=:config/Buildinfo/VERSIONS
UpgradeTest.param      setbuildinfo
mutsqldbc_long.par	ascii concom=yes
mutsqldbc_weekend.par	ascii concom=yes
mutsqldbc.par	ascii concom=yes
SQLManagerTables.par      concom=yes
sqltest_no_ac.par 	concom=yes RELSTAT=$RELSTAT 
sqltest.par 		concom=yes RELSTAT=$RELSTAT 
sqltestdba.par  	concom=yes RELSTAT=$RELSTAT 
LoaderTestInterpreter.par 		concom=yes RELSTAT=$RELSTAT 

