
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
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
#

import sys

def tkinterInstalled ():
    try:
        import Tkinter
        return 'true'
    except ImportError:
        sys.stderr.write ("""
Error: Python GUI 'tkInter' not installed!
Please use \\\\P26326\\InetPub\\ftproot\\TolsNT\\py152.exe
- Tcl/Tk installation only
- add <tcl dir>\bin to %PATH
""")
        return None

def assertTkinterInstalled ():
    if not tkinterInstalled ():
        sys.exit (1)

