
# simHTTP.py
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


from testTools.QACC import sharedReports
from testTools.QACC import qaccDB

def main (options, args):
    if len (args) == 0:
        args = [None]
    session = qaccDB.extendedConnect ()
    try:
        for arg in args:
            report = sharedReports.genHTMLReport (session, arg, options.nocache)
            print report
    finally:
        session.rollback ()
        del session


def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('nocache', None, '', None, 'ignore cached report'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
