#
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2002-2005 SAP AG
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

try:
    from pythondef.dbm import *
    from pythondef.dbm import _buildInfo
except ImportError:
    def importVersionSpecific ():
        import sys
        pythonVersion = sys.version [:3]
        shortVersion = pythonVersion [0] + pythonVersion [2]
        subpackage = 'python-py%s' % shortVersion
        if hasattr (sys, 'maxunicode') and (sys.maxunicode > 65535):
            subpackage = subpackage + 'ucs4'
        module = __import__ (subpackage + '.dbm', globals (), locals (), ['*'])
        globals ().update (module.__dict__)
    importVersionSpecific ()
    del importVersionSpecific

