
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

createdb_74.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
createdb_74_logmodedual.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
createdb_74_restore_bigend.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
createdb_74_restore_litend.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
createdb_75.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
createdb_75_restore_bigend.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
createdb_75_restore_litend.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
createlc_74.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
createlc_75.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
fake_lcbackup.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
backup.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
dbdrop.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
nothing.script ->$OWN/test/jtest/tests/input/upgradetest/UpgradeTest/tests/scripts/ ascii
