# Python/distribTools.com
#
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

packBinaryTools.py exec ascii ->$TOOL/bin/
releasePublisher.py exec ascii ->$TOOL/bin/
packToolsUpdate.py exec ascii ->$TOOL/bin/
perforceExchange.py exec ascii ->$TOOL/bin/
baseModules.py exec ascii ->$TOOL/bin/
publishSourceUpdates.py exec ascii ->$TOOL/bin/
createSourceSnapshot.py exec ascii ->$TOOL/bin/
createSourceRPM.py exec ascii ->$TOOL/bin/
fullCVS.py exec ascii ->$TOOL/bin/
updCVS.py exec ascii ->$TOOL/bin/
fullBitKeeper.py exec ascii ->$TOOL/bin/
updBitKeeper.py exec ascii ->$TOOL/bin/
allsources.py exec ascii ->$TOOL/bin/
createSimplePackage.py exec ascii ->$TOOL/bin/

__init__.py ascii ->$TOOL/lib/Python/distribTools/
packBinaryToolsLib.py ascii ->$TOOL/lib/Python/distribTools/
common.py ascii ->$TOOL/lib/Python/distribTools/
releaseInfo.py ascii ->$TOOL/lib/Python/distribTools/
rpmLib.py ascii ->$TOOL/lib/Python/distribTools/
CVS.py ascii ->$TOOL/lib/Python/distribTools/
perforce2cvs.py ascii ->$TOOL/lib/Python/distribTools/
BitKeeper.py ascii ->$TOOL/lib/Python/distribTools/
perforce2bk.py ascii ->$TOOL/lib/Python/distribTools/

