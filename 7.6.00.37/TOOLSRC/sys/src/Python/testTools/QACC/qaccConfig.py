
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

outputDir = r'\\P26326\InetPub\wwwroot\Resource\Nachtlauf'
outputDir = r'd:\TOOLSRC\develop\tmp\Nachtlauf'

releases = ['R72', 'R73', 'R74', 'R75']
releases = ['R72']

# platforms = ['aix', 'dec', 'hp', 'linux', 'sni', 'sun', 'sunos', 'win']
platforms = ['linux']

username = 'QACC'
password = 'QACC'

#dbname = 'db72'
#host = 'p48047.ber.sap.corp'
dbname = 'PTS'
host = 'P48117.ber.sap.corp'
connectOptions = ''

connectArgs = (username, password, dbname, host, connectOptions)

