#!/usr/bin/env python
# ptsReadme.py
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
import string
import os
from cStringIO import StringIO

try:
    import sapdb.sql
    sql = sapdb.sql
except ImportError:
    import sdb.sql
    sql = sdb.sql

def getDescriptionPatches ():
    fname = os.path.join (os.environ ['TOOL'], 'data', 'relinfo.patch')
    locals = {}
    try:
        execfile (fname, locals)
        return locals ['idsToIgnore'], locals ['idsToPatch']
    except IOError:
        return [], {}

def patchDescription (patchDict, id, description):
    patches = patchDict.get (id)
    if patches:
        for search, replace in patches:
            description = string.replace (description, search, replace, 1)
    return description

def getColNames (cursor):
    result = []
    for info in cursor.getDescription ():
        result.append (info [0])
    return result

def dumpCursor (cursor):
    print string.join (getColNames (cursor), '|')
    for row in cursor:
        print string.join (map (str, row), '|')

def parseReleaseOption (str):
    numbers = tuple (map (int, string.split (str, '.')))
    if len (numbers) == 4:
        result = numbers
    elif len (numbers) == 3:
        major = numbers [0] / 10
        minor = numbers [0] % 10
        result = (major, minor, numbers [1], numbers [2])
    else:
        raise 'Invalid release string', str
    return result


def normalizeReleaseString (str):
    return '%d%d.%02d.%02d' % parseReleaseOption (str)

def formatReleaseString (str):
    return '%d.%d.%02d.%02d' % parseReleaseOption (str)

def wordWrapLine (line, maxline = 60):
    result = []
    restline = line
    while len (restline) > maxline:
        # find space in first maxline chars
        spacePos = string.rfind (restline [:maxline], ' ')
        if spacePos == -1:
            # find space in whole restline
            spacePos = string.find (restline, ' ', maxline)
        if spacePos == -1:
            # use whole restline
            spacePos = len (restline)
        result.append (restline [:spacePos])
        restline = restline [spacePos + 1:]
    if restline:
        result.append (restline)
    return result

def wordWrapLines (lines, maxline = 60):
    result = []
    for line in lines:
        result.extend (wordWrapLine (line, maxline))
    return result

_componentsToIgnore = [
    'Description',
    'Development Environment',
    'JTest',
    'JTestFrame',
    'MUT',
    'PTS',
    'R3Setup',
    'XDIAGNOSE',
    'XPU',
    'XWIZARD',
]

_components = [
    "Kernel",
    "System Tables",
    "Setup",
    "Installation",
    "C Precompiler",
    "Precompiler Runtime",
    "JDBC",
    "ODBC",
    "SQLDBC",
    "Call Interface",
    "Interface Runtime",
    "PHP",
    "PL",
    "Scripting",
    "DBMClient",
    "DBMGUI",
    "DBMGUI Second Edition",
    "DBMServer",
    "DB Analyzer",
    "Loader",
    "DEMO-Welt",
    "SQL Studio",
    "SQLCLI",
    "SQLFilter",
    "WWW Server",
    "Web DBM",
    "Web SQL",
    "WebAgent",
    "WebDAV",
    "XML DB",
    "XML Indexing",
    # "Web PTS",
    "Runtime Environment",
    "XSERVER",
    "XCONS",
    "JTest",
    "JTestFrame",
    "LOAD Testsuite",
]

def singleQuote (str):
    return "'" + str + "'"


def list2SQL (list):
    quoted = map (singleQuote, list)
    return string.join (quoted, ', ')

_descKindMapping = [
    "Fixed the following bug",
    "Fixed Problem", # "Bug fixed",
    "New Feature",
]

_shortdescKindMapping = [
    "fixed",
    "fixed",
    "new",
]
_baseDescription = 0
_bugDescription = 1
_featureDescription = 2
_cutoffRelease = '73.00.18'

#
# generating plain text change list
#

def ptsReadme (session, outstream, options):
    fromString = normalizeReleaseString (options.fromString)
    toString = normalizeReleaseString (options.toString)
    write = outstream.write
    outstream.write ('\nMaxDB Version Information (%s to %s)\n\n' % (
        formatReleaseString (fromString),
        formatReleaseString (toString)))
    major, cl, fromBuild, toBuild = options.range
    targetString = "substr (\"TargetVersion\", 1, 2) || '.' || substr (\"TargetCL\", 1, 2) || '.' || substr (\"TargetPL\", 1, 2)"
    cmd = """select \"ErrorID\", %(targetString)s
        FROM PTS.\"ErrorView\"
        WHERE "TargetVersion" = '%(major)s' AND "TargetCL" = '%(cl)s'
        AND  "TargetPL" BETWEEN '%(fromBuild)s' AND '%(toBuild)s'
        AND   \"Status\" = '4'
        AND \"ComponentName\" = ?
        ORDER BY 2 DESC, 1 DESC
        """ % locals ()
    prepared = session.prepare (cmd)
    for compname in _components:
        print compname
        componentCursor = prepared.execute ([compname])
        #fetchDescription = session.prepare ('select first "DESCRIPTIONTYPE", "Description" into ?, ? '
        #    + ' from PTS."Descriptions" where "ErrorID" = ? order by "DESCRIPTIONTYPE"').execute
        fetchDescription = session.prepare ('select "DESCRIPTIONTYPE", "Description"'
            + ' from PTS."Descriptions" where "ErrorID" = ? order by "DESCRIPTIONTYPE" DESC').execute
        toIgnore, toPatch = getDescriptionPatches ()
        introWritten = None
        for row in componentCursor:
            id, release = row
            if id in toIgnore:
                continue
            cursor = fetchDescription ([id])
            (kind, description) = cursor.next ()
            del cursor
            if (kind == _baseDescription) and (release >= _cutoffRelease):
                continue
            if string.find (description, '@INTERNAL@') != -1:
                continue
            if not introWritten:
                outstream.write ('\n' + compname + ':\n')
                outstream.write (('=' * len (compname)) + '=\n\n')
                introWritten = 1
            outstream.write ('    PTS: %s    since: %s\n\n' % (id, formatReleaseString (release)))
            description = patchDescription (toPatch, id, description)
            lines = string.split (description, '\x0d\n')
            outstream.write ('    ' + _descKindMapping [kind] + ':\n')
            for line in wordWrapLines (lines):
                line = string.rstrip (line)
                if line:
                    write ('        ' + line + '\n')
            write ('\n\n')

def plainText (session, options):
    if options.outfile:
        outstream = open (options.outfile, 'w')
    else:
        outstream = sys.stdout
    ptsReadme (session, outstream, options)

#
# generating HTML suitable for http://www.mysql.com/products/maxdb/changes/
#

def html (session, options):
    toString = formatReleaseString (options.toString)
    if not options.outfile:
        options.outfile = 'changes_' + toString + '.html'
    outstream = open (options.outfile, 'w')
    outstream.write (mysqlProlog % locals ())
    chapters, maintext = genHTMLEntries (session, options)
    for text, target, bugCount, newCount in chapters:
        bugText = countFormat (bugCount, '1 problem fixed', ' problems fixed')
        newText = countFormat (newCount, '1 new feature', ' new features')
        texts = filter (None, [bugText, newText])
        statString = '<font size="2">(' + string.join (texts, ', ') + ')</font>'
        outstream.write ('<a href="#' + target + '">' + text + '</a> '
            + statString + '<br>\n')
    outstream.write ('<br>\n')
    outstream.write (maintext)
    outstream.write (mysqlEpilog)

def countFormat (count, one, multiple):
    if count == 0:
        return ''
    if count == 1:
        return one
    return repr (count) + multiple


def openPerforceFile (toString):
    #
    # get location of change logs
    #
    lines = os.popen ('p4 fstat //sapdb/OpenSource/mysql.com/products/maxdb/changes/index.html').readlines ()
    for line in lines:
        words = string.split (line)
        if words [1] == 'clientFile':
            localDir = os.path.dirname (words [2])
            localFile = os.path.join (localDir, 'changes_' + toString + '.html')
            break
    #
    # create a new file
    #
    result = open (localFile, 'w')
    #
    # add to perforce
    #
    os.system ('p4 add ' + localFile)
    #
    #
    #
    return result

def genHTMLEntries (session, options):
    chapters = []
    outstream = StringIO ()
    fromString = normalizeReleaseString (options.fromString)
    toString = normalizeReleaseString (options.toString)
    targetString = "substr (\"TargetVersion\", 1, 2) || '.' || substr (\"TargetCL\", 1, 2) || '.' || substr (\"TargetPL\", 1, 2)"
    cmd = """select \"ErrorID\", %(targetString)s
        FROM PTS.\"ErrorView\"
        WHERE '%(fromString)s' <= %(targetString)s AND %(targetString)s <= '%(toString)s'
        AND   \"Status\" = '4'
        AND \"ComponentName\" = ?
        ORDER BY 2 DESC, 1 DESC
        """ % locals ()
    prepared = session.prepare (cmd)
    lastList = [None, None, None, None]
    bugCount = 0
    newCount = 0
    for compname in _components:
        print compname
        componentCursor = prepared.execute ([compname])
        #fetchDescription = session.prepare ('select first "DESCRIPTIONTYPE", "Description" into ?, ? '
        #    + ' from PTS."Descriptions" where "ErrorID" = ? order by "DESCRIPTIONTYPE"').execute
        fetchDescription = session.prepare ('select "DESCRIPTIONTYPE", "Description"'
            + ' from PTS."Descriptions" where "ErrorID" = ? order by "DESCRIPTIONTYPE" DESC').execute
        toIgnore, toPatch = getDescriptionPatches ()
        introWritten = None
        for row in componentCursor:
            id, release = row
            if id in toIgnore:
                continue
            cursor = fetchDescription ([id])
            (kind, description) = cursor.next ()
            del cursor
            if (kind == _baseDescription) and (release >= _cutoffRelease):
                continue
            if string.find (description, '@INTERNAL@') != -1:
                continue
            if not introWritten:
                lastList [2] = bugCount
                lastList [3] = newCount
                compurl = string.replace (compname, ' ', '_')
                bugCount = 0
                newCount = 0
                lastList = [compname, compurl, -1, -2]
                chapters.append (lastList)
                outstream.write ('<h2><a name="%s">%s</a></h2>\n' % (compurl, compname))
                introWritten = 1
            if kind == _featureDescription:
                newCount = newCount + 1
            else:
                bugCount = bugCount + 1
            description = patchDescription (toPatch, id, description)
            lines = string.split (description, '\x0d\n')
            outstream.write ('<p><b>%s:</b> (PTS Message %s, %s in %s)</p>\n<blockquote>' % (
                _descKindMapping [kind], id, _shortdescKindMapping [kind], formatReleaseString (release)))
            for line in lines:
                line = string.strip (line)
                line = string.replace (line, '&', '&amp;')
                line = string.replace (line, '<', '&lt;')
                outstream.write (line + '<br>\n')
            outstream.write ('</blockquote><br>\n')
    lastList [2] = bugCount
    lastList [3] = newCount
    return chapters, outstream.getvalue ()

mysqlProlog = """<?$pagedata = array(
  'title' => 'Changes in MaxDB&#8482; Version %(toString)s',
);
include '../links.inc';
?>
"""

mysqlEpilog = ""

def listComponents (options):
    username, pwd = string.split (options.userInfo, ',')
    session = sql.connect (string.upper (username), pwd, 'QADB', 'pts')
    cursor = session.sql ("""Select Distinct "ComponentName" from PTS."ErrorView" order by 1""")
    for (component,) in cursor:
        if component in _components:
            line = '+ ' + component
        elif component in _componentsToIgnore:
            line = '- ' + component
        else:
            line = '? ' + component
        print line


#
# main
#
def checkOptions (options):
    if options.fromString is None:
        options.fromString = options.toString
    if options.toString is None:
        options.toString = options.fromString
    fromRel = parseReleaseOption (options.fromString)
    toRel = parseReleaseOption (options.toString)
    if fromRel [:3] != toRel [:3]:
        print 'only Releases of one correction level can be compared'
    major = '%d%d' % (fromRel [0], fromRel [1])
    cl = '%02d' % fromRel [2]
    fromBuild = '%02d' % fromRel [3]
    toBuild = '%02d' % toRel [3]
    options.range = (major, cl, fromBuild, toBuild)

def main (options, args):
    """generates a release info

    call as: ptsReadme.py -from 72.05.01 -to 72.06.30
    """
    checkOptions (options)
    username, pwd = string.split (options.userInfo, ',')
    session = sql.connect (string.upper (username), pwd, 'QADB', 'pts')
    if options.html:
        html (session, options)
    else:
        plainText (session, options)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('u', 'userInfo', ':', 'PTSALLREAD,PTSALLREAD', 'user info (name,pwd)'),
        ('o', 'outfile', ':', None, 'output file'),
        ('from', 'fromString', ':', None, 'from CL (release.CL.PL)'),
        ('to', 'toString', ':', None, 'to CL (release.CL.PL)'),
        ('html', None, '', None, 'generate HTML suitable for mysql.com'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
