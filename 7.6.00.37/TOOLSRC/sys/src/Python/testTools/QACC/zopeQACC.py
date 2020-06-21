
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

from sql import sqlHelper, SessionPool
from testTools.QACC import qaccConfig, sharedReports

sessionPool = SessionPool.SessionPool (qaccConfig.connectArgs,
    sessionDict = sqlHelper.SelfDict ())

def report (self, reportName = None, nocache = None):
    reload (sharedReports)
    session = sessionPool.getSession ()
    try:
        result = sharedReports.genHTMLReport (session, reportName, nocache,
            linkPrefix = 'report?reportName=')
    finally:
        sessionPool.releaseSession (session)
    return result

if __name__ == "__main__":
    print report (None)

