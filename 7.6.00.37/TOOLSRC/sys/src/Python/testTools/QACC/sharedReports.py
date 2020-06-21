
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

import string
import os
import time
from cStringIO import StringIO

import qaccDB
import qaccConfig
import reportLib

releases = qaccConfig.releases

platforms = qaccConfig.platforms

def generateAll ():
    session = qaccDB.extendedConnect ()
    #ReleaseReportGenerator ('%s.html').generate (session)
    PlatformReportGenerator ('%s.html').generate (session)
    #generateSpecials (session)

class ReportGenerator:
    makeConfOrder = ['release', 'platform', 'bits']
    sutConfOrder = makeConfOrder + ['speed', 'encoding']

    def __init__ (self, linkPattern):
        self.linkPattern = linkPattern

    def generate (self, session):
        for parameter in self.getParameters ():
            mainp = self.mainProperty
            self.overviewReport (session, openFile (mainp, parameter, 'Overview'), parameter)
            self.makeReport (session, openFile (mainp, parameter, 'make'), parameter)
            self.sutReport (session, openFile (mainp, parameter, 'sut'), parameter)

    def getTitle (self):
        return string.capitalize (self.mainProperty)


    def getParameters (self):
        raise 'subclass responsability', 'getParameters'

    def getConfOrder (self, order):
        result = []
        for entry in order:
            if entry != self.mainProperty:
                result.append (entry)
        return result

    def overviewReport (self, session, stream, parameter, configIDs = None):
        title = self.getTitle ()
        #stream = openFile (title, parameter, 'Overview')
        report = reportLib.TargetGroupReport (session, 'Main', 'imf',
            confSelect = [(self.mainProperty, parameter)],
            confOrder = self.getConfOrder (self.makeConfOrder))
        report.displayHTML ('Main', 'CURRENT', stream, configIDs = configIDs)

        link = self.linkPattern % buildReportLink (title, parameter, 'make')
        stream.write ('<p><a href="%s">Full Details</a></p>\n' % link)
        # sut totals
        report = reportLib.DirReport (session, 'SUT',
            confSelect = [(self.mainProperty, parameter)],
            confOrder = self.getConfOrder (self.sutConfOrder))
        link = self.linkPattern % buildReportLink (title, parameter, 'sut')
        report.displayHTML ('SUT', 'CURRENT', stream, 1, 1, configIDs = configIDs)
        stream.write ('<p><a href="%s">Full Details</a></p>\n' % link)
        # close file
        #stream.write ('</body></html>\n')
        #stream.close ()

    def makeReport (self, session, stream, parameter, configIDs = None):
        target = 'imf'
        #stream = openFile (self.getTitle (), parameter, target)
        report = reportLib.DirReport (session, target,
            confSelect = [(self.mainProperty, parameter)],
            confOrder = self.getConfOrder (self.makeConfOrder))
        report.displayHTML (target, 'CURRENT', stream)
        #stream.write ('</body></html>\n')
        #stream.close ()

    def sutReport (self, session, stream, parameter, configIDs = None):
        #stream = openFile (self.getTitle (), parameter, 'sut')
        report = reportLib.DirReport (session, 'SUT',
            confSelect = [(self.mainProperty, parameter)],
            confOrder = self.getConfOrder (self.makeConfOrder))
        report.displayHTML ('SUT', 'CURRENT', stream, 1)
        #stream.write ('</body></html>\n')
        #stream.close ()

class ReleaseReportGenerator (ReportGenerator):
    mainProperty = 'release'
    def getParameters (self):
        return releases

class PlatformReportGenerator (ReportGenerator):
    mainProperty = 'platform'
    def getParameters (self):
        return platforms

def generateSpecials (session):
    generatingScripting (session, openFile ('Scripting', '', 'Overview'))

def generatingScripting (session, stream, parameter = None, configIDs = None):
    # stream = openFile ('Scripting', '', 'Overview')
    report = reportLib.TargetGroupReport (session, 'Scripting', 'imf',
        confSelect = [('bits', '32')],
        confOrder = ['release', 'platform'])
    report.displayHTML ('Scripting', 'CURRENT', stream)
    # stream.write ('</body></html>\n')
    # stream.close ()


def openFile (propname, propvalue, title):
    fname = buildFilename (propname, propvalue, title)
    fullname = os.path.join (qaccConfig.outputDir, fname)
    stream = open (fullname, 'wt')
    stream.write ('''<html>
<head><title>%(propname)s %(propvalue)s: %(title)s</title></head>
<body>
<h1 align="center">%(propname)s %(propvalue)s: %(title)s</h1>
<hr>
''' % locals ())
    return stream

def buildFilename (propname, propvalue, title):
    return '%s-%s-%s.html' % (propname, propvalue, title)

def buildReportLink (propname, propvalue, title):
    return '%s-%s-%s' % (propname, propvalue, title)

_specialReports = {
    'Scripting--Overview': generatingScripting,
}

def htmlIntro (title):
    return '''
<HTML>
<HEAD>
  <TITLE>%(title)s</TITLE>
</HEAD>
<BODY>
<H2>
  %(title)s
</H2>
<P>
  <HR>
''' % locals ()


def getCachedReport (session, reportName):
    result = None
    cursor = session.sqlX ('select %(text)s from %(ReportCache)s where %(title)s = ?',
        [reportName])
    row = cursor.next ()
    if row:
        longStream = row [0]
        if longStream:
            resultStream = StringIO ()
            buf = longStream.read (4092)
            while buf:
                resultStream.write (buf)
                buf = longStream.read (4092)
            result = resultStream.getvalue ()
    return result

def openHTMLStream (title):
    htmlStream = StringIO ()
    htmlStream.write (htmlIntro (title))
    return htmlStream

def nowForSQL ():
    return time.strftime ("%Y%m%d%H%M%S", time.localtime (time.time ()))

def findReportMethod (reportName, linkPattern):
    notFoundValue = (None, None, None)
    words = string.split (reportName, '-')
    if len (words) != 3:
        return notFoundValue
    kind, parameter, subkind = words
    if kind == 'Platform':
        generator = PlatformReportGenerator (linkPattern)
    elif kind == 'Release':
        generator = ReleaseReportGenerator (linkPattern)
    else:
        return notFoundValue
    try:
        method = getattr (generator, string.lower (subkind) + 'Report')
    except AttributeError:
        return notFoundValue
    title = '%s %s: %s' % (kind, parameter, subkind)
    return method, parameter, title

def reportError (msg):
    template = htmlIntro ('QACC Error') + '''
<p>
<b>Error: %s</b>
</p>
'''
    return template % msg


def genHTMLReport (session, reportName = None, nocache = None,
        linkPrefix = '', linkSuffix = ''):
    if reportName is None:
        return indexHTML (linkPrefix, linkSuffix)
    else:
        linkPattern = linkPrefix + '%s' + linkSuffix
        if not nocache:
            # look in cache
            result = getCachedReport (session, reportName)
            if result:
                # print '********** using cached result **************'
                return result
            deleteOldEntry = None
        else:
            deleteOldEntry = 1
        # detect create method
        if _specialReports.has_key (reportName):
            method = _specialReports [reportName]
            parameter = None
            title = reportName
        else:
            method, parameter, title = findReportMethod (reportName, linkPattern)
            if method is None:
                return reportError ('Report ' + reportName + ' not found')
        # open stream
        htmlStream = openHTMLStream (title)
        # call method
        configurationIds = []
        method (session, htmlStream, parameter, configurationIds)
        # close stream
        htmlStream.write ('<hr>\n<i>generated: '
            + time.strftime ("%Y-%m-%d %H:%M:%S", time.localtime (time.time ()))
            + '</i>\n</body></html>\n')
        result = htmlStream.getvalue ()
        # store in cache
        if deleteOldEntry:
            session.sqlX ('delete from %(ReportCache)s where %(title)s = ?',
                [reportName])
        session.sqlX ('insert into %(ReportCache)s values (?, ?)',
            [reportName, result])
        # store configurations
        for configID in configurationIds:
            session.sqlX ('insert into %(ReportConfigs)s values (?, ?)',
                [configID, reportName])
        session.commit ()
        return result

def indexHTML (linkPrefix, linkSuffix):
    return (htmlIntro ('Ergebnisse der Nachtläufe') + '''
<H3>
  Nach Release:
</H3>
<BLOCKQUOTE>
  <A HREF="%(linkPrefix)sRelease-72-Overview%(linkSuffix)s">7.2</A> -
  <A HREF="%(linkPrefix)sRelease-73-Overview%(linkSuffix)s">7.3</A> -
  <A HREF="%(linkPrefix)sRelease-74-Overview%(linkSuffix)s">7.4</A>
</BLOCKQUOTE>
<H3>
  Nach Plattform:
</H3>
<BLOCKQUOTE>
  <A HREF="%(linkPrefix)sPlatform-aix-Overview%(linkSuffix)s">aix</A> -
  <A HREF="%(linkPrefix)sPlatform-dec-Overview%(linkSuffix)s">dec</A> -
  <A HREF="%(linkPrefix)sPlatform-hp-Overview%(linkSuffix)s">hp</A> -
  <A HREF="%(linkPrefix)sPlatform-linux-Overview%(linkSuffix)s">linux</A> -
  <A HREF="%(linkPrefix)sPlatform-sni-Overview%(linkSuffix)s">sni</A> -
  <A HREF="%(linkPrefix)sPlatform-sun-Overview%(linkSuffix)s">sun</A> -
  <A HREF="%(linkPrefix)sPlatform-win-Overview%(linkSuffix)s">win</A>
</BLOCKQUOTE>
<H3>
  Sonstiges:
</H3>
<BLOCKQUOTE>
  <A HREF="%(linkPrefix)sScripting--Overview%(linkSuffix)s">Scripting</A>
</BLOCKQUOTE>
<P>
  <HR>
<H3>
  Erweiterungen:
</H3>
<H4>
  Make-Targets erweitern
</H4>
<BLOCKQUOTE>
  <CODE>editQaccGroup.py -groupName Main -add
  &lt;additionalTarget.mac&gt;</CODE><BR>
  Die Dateierweiterung ist notwendig.
</BLOCKQUOTE>
<H4>
  Zus&auml;tzliche Testprotokolle aufnehmen
</H4>
<OL>
  <LI>
    analog zu <CODE>qaccSUT.py</CODE> ein Script schreiben, das den Inhalt der
    Protokolle in die Datenbank &uuml;berf&uuml;hrt
  <LI>
    Aufruf des Scripts in die Nachtlauf-Scripte einbauen
  <LI>
    in <CODE>sharedReports.py</CODE> in der Methode overviewReport den Code f&uuml;r
    die SUTs kopieren und anpassen
  <LI>
    in <CODE>sharedReports.py</CODE> ein neue Methode &lt;xxx&gt;Report ()
    einf&uuml;hren
  <LI>
    die in der Methode generate aufrufen
</OL>
<H4>
  Eigene Reports erzeugen (analog zu
  <A HREF="Scripting--Overview">Scripting</A>)
</H4>
<OL>
  <LI>
    eine eigene Target-Gruppe erzeugen:<BR>
    <CODE>editQaccGroup.py -groupName &lt;MyProject&gt; -add
    &lt;target1.mac&gt;</CODE> <CODE>&lt;target2.mac&gt;</CODE>
  <LI>
    in <CODE>sharedReports.py</CODE> die Routine generateScripting kopieren und
    anpassen
  <LI>
    die neue Methode in generateSpecials () aufrufen
</OL>
<P>
</BODY></HTML>
''' % locals ())
