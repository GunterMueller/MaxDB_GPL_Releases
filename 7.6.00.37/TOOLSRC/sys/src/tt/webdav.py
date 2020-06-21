#!/usr/bin/env python
# webdav.py 
#
# schreibt ein File in den webdav Server
#
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


if __name__ == '__main__':
    
    import httplib
    import sys
    import string
    
    fname = sys.argv[1]
    print "debug >> fname: " + fname + "\n"
    msg = open (fname, 'r').read ()
    webdavname = 'webdav/daily_makes/' + fname
    print "debug >> webdavname: " + webdavname + "\n"

    h = httplib.HTTP('pts.wdf.sap.corp:1080')
    h.putrequest('PUT', webdavname)
    h.putheader('Host', 'PTS')
    h.putheader('Content-Type','text/plain')
    h.putheader('Content-Length',str(len(msg)))
    h.endheaders()
    h.send(msg)
    errcode, errmsg, headers = h.getreply()
    print "debug >> send rc:", errcode , errmsg



