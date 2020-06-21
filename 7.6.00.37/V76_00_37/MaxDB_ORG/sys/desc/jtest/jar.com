
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

testframe.jar ->$OWN/test/jtest/frame/ binary
stedi.jar ->$OWN/test/jtest/frame/ binary
lctest.jar ->$OWN/test/jtest/tests/ binary
oltptest.jar ->$OWN/test/jtest/tests/ binary
performance.jar ->$OWN/test/jtest/tests/ binary
benchmark.jar ->$OWN/test/jtest/tests/ binary
WebDAVTests.jar ->$OWN/test/jtest/tests/ binary
j2ee.jar ->$OWN/test/jtest/tests/ binary
bcprov_jdk14_125.jar ->$OWN/test/jtest/tests/ binary
msgserver.jar ->$OWN/test/jtest/tests/ binary
syncmanacc.jar ->$OWN/test/jtest/tests/ binary
syncmancli.jar ->$OWN/test/jtest/tests/ binary
syncmangui.jar ->$OWN/test/jtest/tests/ binary
syncservice.jar ->$OWN/test/jtest/tests/ binary
syncmantest.jar ->$OWN/test/jtest/tests/ binary
syncmantests.jar ->$OWN/test/jtest/tests/ binary
upgradetest.jar ->$OWN/test/jtest/tests/ binary
packettest.jar ->$OWN/test/jtest/tests/ binary
guitest.jar ->$OWN/test/jtest/tests/ binary
