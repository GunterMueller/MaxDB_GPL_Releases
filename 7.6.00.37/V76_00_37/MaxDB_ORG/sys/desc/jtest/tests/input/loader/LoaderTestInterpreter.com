
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

SQLReferenceCatalog.dtd ->$OWN/test/jtest/tests/input/loader/LoaderTestInterpreter/ ascii
TestCases.dtd ->$OWN/test/jtest/tests/input/loader/LoaderTestInterpreter/ ascii

*.xml ->$OWN/test/jtest/tests/input/loader/LoaderTestInterpreter/ ascii
*.data ->$OWN/test/jtest/tests/input/loader/LoaderTestInterpreter/ ascii
*.data0000 ->$OWN/test/jtest/tests/input/loader/LoaderTestInterpreter/ ascii
*.data0001 ->$OWN/test/jtest/tests/input/loader/LoaderTestInterpreter/ binary
*.bindata ->$OWN/test/jtest/tests/input/loader/LoaderTestInterpreter/ binary
*.long ->$OWN/test/jtest/tests/input/loader/LoaderTestInterpreter/ ascii
*.catalog ->$OWN/test/jtest/tests/input/loader/LoaderTestInterpreter/ ascii
