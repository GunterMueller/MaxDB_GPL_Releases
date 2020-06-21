
# serverTimeTable.py
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


import string
import re
import os
import time

import html

class AtParser:
    def parseServer (self, serverName):
        lines = os.popen ("at \\\\" + serverName).readlines ()
        if len (lines) == 1:
            raise lines [0]
        result = []
        for line in lines:
            if string.find (line, 'devstudio') != -1:
                result.append (self.parseLine (line, serverName))
        return result

    def parseLine (self, line, serverName):
        timeString = line [36:45]
        timeString = self.makeTime (timeString)
        days = string.strip (line [12:36])
        if days == 'Each M T W Th F':
            days = 'M - F'
        cmdLine = string.split (line [51:])
        release = cmdLine [2]
        dummy, tool = os.path.split (cmdLine [3])
        return (timeString, days, release, tool, serverName)

    def makeTime (self, timeString):
        h = int (timeString [:2])
        m = int (timeString [3:5])
        return "%02d:%02d" % (h, m)

class MSSchedulerParser:
    def parseServer (self, serverName):
        stream = os.popen ("lsSchedule \\\\" + serverName)
        lines = stream.readlines ()
        rc = stream.close ()
        if rc != None:
            print serverName, 'Error', rc
            return []
        print serverName
        result = []
        for line in lines:
            nextJob = self.parseLine (line, serverName)
            if nextJob:
                result.append (nextJob)
        return result

    def parseLine (self, line, serverName):
        jobname, timeString, command = string.split (line, '\t')
        if len (timeString) == 4:
            timeString = '0' + timeString
        release = self.parseRelease (command)
        if not release:
            return None
        tool = self.parseTool (command)
        return (timeString, '?', release, tool, serverName)

    releaseRE = re.compile (r'.*profiles\\prof([a-zA-Z0-9_]+)')

    def parseRelease (self, command):
        match = self.releaseRE.match (command)
        if match:
            result = match.group (1)
        else:
            result = None
        return result

    def parseTool (self, command):
        if command [-2:] == '"\n':
            command = command [:-2]
        bslashPos = string.rfind (command, '\\')
        if bslashPos != -1:
            result = command [bslashPos + 1:]
        else:
            result = '?'
        return result

class TimeTableWriter:
    def genHTML (self, outfile, tuples):
        title = 'Stundenplan'
        # open file
        stream = html.HTMLFile (outfile, title,
                background="/Images/backgrounds/sqlback.gif",
                bgcolor="#ffffff")
        stream.titleHeader (title)
        # open table
        stream.pushTag ('table', border = 2, cellpadding = 5,
                align = 'center')
        # gen table header
        stream.write ("""
    <TR>
        <TH>Zeit</TH>
        <TH>Tag</TH>
        <TH>Release</TH>
        <TH>Script</TH>
        <TH>Server</TH>
    </TR>
""")
        # gen rows
        rowfmt = """
    <TR>
        <TD>%s</TD>
        <TD>%s</TD>
        <TD>%s</TD>
        <TD>%s</TD>
        <TD>%s</TD>
    </TR>
"""
        for t in tuples:
            stream.write (rowfmt % t)
        # close table
        stream.popTag ()
        # write gen data
        stream.line ()
        stream.write (time.strftime ('Generated %d.%m.%Y',
                time.localtime (time.time ())))
        # close stream
        stream.close ()

def main (options, args):
    """Creates a web page containing the schedules
    of several servers ['p26326', 'p26327', 'p30883'].

    """
    if len (args) == 0:
        args = ['p26326', 'p26327', 'p30883']
    parser = MSSchedulerParser ()
    tuples = []
    for arg in args:
        tuples = tuples + parser.parseServer (arg)
    tuples.sort ()
    writer = TimeTableWriter ()
    writer.genHTML (options.outfile, tuples)
    print 'Output written to ' + options.outfile

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('o', 'outfile', ':', r'\\p26326\InetPub\wwwroot\Entwicklung\verschiedenes\STUNDENPLAN.html', 'name of output file'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

