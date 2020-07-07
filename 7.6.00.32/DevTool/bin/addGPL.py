
# addGPL.py
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
splitext = os.path.splitext
joinpath = os.path.join
import re
import time

from fileutil import RecursiveFileIterator, isWritable

_version = 0.5
true = 'true'

noAction = None

class TeeStream:
    def __init__ (self, stream1, stream2 = sys.stdout):
        self.writes = [stream1.write, stream2.write]

    def write (self, data):
        for write in self.writes:
            write (data)

class NotImplementedException:
    def __init__ (self, object, method):
        self.clazz = object.__class__
        self.method = method

    def __str__ (self):
        return 'NotImplemented: %s in class %s' % (self.method, self.clazz)

class GPLInserter:
    doInsert = 1
    kind = '?'
    marker = '========== licence begin'
    gpl = None
    lgpl = None

    def __init__ (self):
        pass

    def insertInto (self, fname, licence):
        data = open (fname, 'rt').read ()
        gplFound = self.gplFound (data)
        if not gplFound:
            result = 'inserted ' + licence
            changed = self.insertLicence (data, licence)
            if not noAction:
                mtime = os.path.getmtime (fname)
                open (fname, 'wt').write (changed)
                os.utime (fname, (mtime, mtime))
            else:
                result = result + ' (no action)'
        else:
            result = 'found ' + gplFound
        return result

    def gplFound (self, data):
        markerPos = string.find (data, self.marker)
        if markerPos == -1:
            result = None
        else:
            licStart = markerPos + len (self.marker) + 1
            result = string.strip (data [licStart:licStart + 4])
        return result

    def insertLicence (self, data, licence):
        # find comment start
        pos = self.findCommentStart (data, 0)
        # check white space before
        if (pos == -1) or (string.strip (data [:pos]) != ''):
            # no module comment, prepend licence
            commentedLicence = self.fullCommentedLicence (licence)
            return (commentedLicence % _thisYear) + data
        else:
            # search for comment end
            endPos = self.findCommentEnd (data, pos)
            result = self.insertLicenceIntoComment (data, pos, endPos, licence)
        return result

    def getCommentedLicence (self, licence):
        if licence is GPL:
            return self.getGPL ()
        elif licence is LGPL:
            return self.getLGPL ()
        else:
            raise 'unknown case'
            return licence

class StreamCommentInserter (GPLInserter):
    def getGPL (self):
        if self.gpl == None:
            self.gpl = GPL_pattern
        return self.gpl

    def getLGPL (self):
        if self.lgpl == None:
            self.lgpl = LGPL_pattern
        return self.lgpl

    def findCommentStart (self, data, pos = 0):
        return string.find (data, self.commentBrace [0], pos)

    def findCommentEnd (self, data, pos = 0):
        endPos = string.find (data, self.commentBrace [1], pos)
        if endPos == -1:
            return len (data)
        prevEOL = string.rfind (data, '\n', 0, endPos)
        if (prevEOL > -1) and (not re.search ('\w', data [prevEOL:endPos])):
            # decoration line, keep at end
            endPos = prevEOL + 1
        return endPos

    def fullCommentedLicence (self, licence):
        prefix, suffix = self.commentBrace
        return prefix + '\n' + self.getCommentedLicence (licence) + '\n' + suffix + '\n'

    def insertLicenceIntoComment (self, data, startPos, endPos, licence):
        licenceText = self.getCommentedLicence (licence) % _thisYear
        return data [:endPos] + licenceText + data [endPos:]

def prependLineCommentChar (commentChar, data):
    lines = string.split (data, '\n')
    result = []
    if string.strip (lines [0]) == '':
        result.append (lines [0])
    else:
        result.append (commentChar + lines [0])
    for line in lines [1:-1]:
        result.append (commentChar + line)
    if string.strip (lines [-1]) == '':
        result.append (lines [-1])
    else:
        result.append (commentChar + lines [-1])
    return string.join (result, '\n')

class LineCommentInserter (GPLInserter):
    def getGPL (self):
        if self.gpl == None:
            self.gpl = prependLineCommentChar (self.commentChar, GPL_pattern)
        return self.gpl

    def getLGPL (self):
        if self.lgpl == None:
            self.lgpl = prependLineCommentChar (self.commentChar, LGPL_pattern)
        return self.lgpl

    def fullCommentedLicence (self, licence):
        return self.getCommentedLicence (licence)

    def findCommentStart (self, data, pos = 0):
        result = string.find (data, self.commentChar, pos)
        return result

    def findCommentEnd (self, data, pos = 0):
        eoCommentRE = re.compile ('\n[^' + self.commentChar + ']')
        match = eoCommentRE.search (data, pos)
        if match:
            result = match.start (0)
        else:
            result = len (data)
        return result

    def insertLicenceIntoComment (self, data, startPos, endPos, licence):
        return data [:endPos] + (self.getCommentedLicence (licence) % _thisYear) + data [endPos:]

class VDN_GPLInserter (GPLInserter):
    kind = 'vdn'
    def insertLicence (self, data, licence):
        match = re.search ('.tt 3', data, re.I)
        licence = self.getCommentedLicence (licence) % _thisYear
        if not match:
            return licence + data
        matchStart = match.start ()
        # skip decoration
        eol1 = string.index (data, '\n', matchStart)
        eol2 = string.index (data, '\n', eol1 + 1)
        decoline = data [eol1 + 1:eol2]
        decoline = string.replace (decoline, '*', '')
        decoline = string.strip (decoline)
        if decoline == '':
            insertPoint = eol2 + 1
        else:
            insertPoint = eol1 + 1
        # insert license
        result = data[:insertPoint] + licence + data [insertPoint:]
        return result

    def getGPL (self):
        if self.gpl == None:
            self.gpl = '.nf\n' + GPL_pattern + '.fo\n'
        return self.gpl

    def getLGPL (self):
        if self.lgpl == None:
            self.lgpl = '.nf\n' + LGPL_pattern + '.fo\n'
        return self.lgpl

class GPLNonInserter (GPLInserter):
    doInsert = None
    kind = 'ignore'
    def insertInto (self, fname, licence):
        return 'no change'

class BinaryFile (GPLNonInserter):
    kind = 'ignore (binary)'

class ExternalFile (GPLNonInserter):
    kind = 'ignore (external)'

class GarbageFile (GPLNonInserter):
    kind = 'ignore (garbage)'

class DemoFile (GPLNonInserter):
    kind = 'ignore (demo)'

class TinyFile (GPLNonInserter):
    kind = 'ignore (tiny)'

class JavaGPLInserter (StreamCommentInserter):
    kind = 'Java'
    commentBrace = ('/*', '*/')

class C_GPLInserter (StreamCommentInserter):
    kind = 'C/C++'
    commentBrace = ('/*', '*/')

    def insertLicenceIntoComment (self, data, startPos, endPos, licence):
        licenceText = self.getCommentedLicence (licence) % _thisYear
        licenceText = '\\if EMIT_LICENCE\n' + licenceText + '\\endif\n'
        return data [:endPos] + licenceText + data [endPos:]

class Pascal_GPLInserter (StreamCommentInserter):
    kind = 'Pascal'
    commentBrace = ('(*', '*)')

class Antlr_GPLInserter (StreamCommentInserter):
    kind = 'Antlr'
    commentBrace = ('/*', '*/')

    def insertLicence (self, data, licence):
        # find comment start
        pos = self.findCommentStart (data, 0)
        # check white space before
        if (pos == -1):
            # no module comment, don't know where to add licence
            return data
        else:
            # search for comment end
            endPos = self.findCommentEnd (data, pos)
            result = self.insertLicenceIntoComment (data, pos, endPos, licence)
        return result

class CPC_TestGPLInserter (StreamCommentInserter):
    kind = 'CPC Test'
    commentBrace = ('/*', '*/')

    def insertLicence (self, data, licence):
        if data [-1] == '\n':
            glue = ''
        else:
            glue = '\n'
        return data + glue + '/*' + licence + '*/\n'

class DescriptionGPLInserter (LineCommentInserter):
    kind = 'vmake'
    commentChar = '#'

class MakefileGPLInserter (LineCommentInserter):
    kind = 'make'
    commentChar = '#'

class PerlGPLInserter (LineCommentInserter):
    kind = 'perl'
    commentChar = '#'

class PythonGPLInserter (LineCommentInserter):
    kind = 'python'
    commentChar = '#'

class DefGPLInserter (LineCommentInserter):
    kind = 'DLL def'
    commentChar = ';'

class GenmsgGPLInserter (LineCommentInserter):
    kind = 'genmsg'
    commentChar = '#'

class SqlGPLInserter (LineCommentInserter):
    kind = 'sql'
    commentChar = '#'

class AssemblerGPLInserter (LineCommentInserter):
    kind = 'assembler'
    commentChar = ';'

class DefinitionGPLInserter (LineCommentInserter):
    kind = 'definition'
    commentChar = ';'

class PCF_GPLInserter (StreamCommentInserter):
    kind = 'kernel parameter'
    commentBrace = ('&ifdef COMMENT', '\n&endif')
    def getGPL (self):
        if self.gpl == None:
            self.gpl = prependLineCommentChar ("'", GPL_pattern)
        return self.gpl

    def getLGPL (self):
        if self.lgpl == None:
            self.lgpl = prependLineCommentChar ("'", LGPL_pattern)
        return self.lgpl

    def insertLicenceIntoComment (self, data, startPos, endPos, licence):
        return data [:endPos] + self.getCommentedLicence (licence) + data [endPos:]

class LibGPLInserter (LineCommentInserter):
    kind = 'lib'
    commentChar = '#'

    def insertInto (self, fname, licence):
        stream = open (fname, 'rb')
        magic = stream.read (7)
        stream.close ()
        if magic == '!<arch>':
            result = BinaryFile ().insertInto (fname, licence)
        else:
            result = DescriptionGPLInserter ().insertInto (fname, licence)
        return result

class HTML_GPLInserter (StreamCommentInserter):
    kind = 'HTML'
    commentBrace = ('<!--', '-->')

class InstallationGPLInserter (LineCommentInserter):
    kind = 'installation'
    commentChar = '/'

#
#
#
_extmapping = { }

def addExtensions (inserter, extensions):
    for ext in extensions:
        assert not _extmapping.has_key (ext)
        _extmapping [ext] = inserter

addExtensions (JavaGPLInserter (), ['.java'])

addExtensions (DescriptionGPLInserter (), ['.mac', '.shm', '.lnk',
    '.dld', '.shr', '.rel', '.com'])

addExtensions (C_GPLInserter (), ['.c', '.C', '.cpp', '.h', '.hpp',
    '.BP', '.CP', '.idl', '.pc', '.rc', '.y', '.l', '.ic', '.ch', '.ci'])

addExtensions (Antlr_GPLInserter (), ['.g'])

addExtensions (Pascal_GPLInserter (), ['.P', '.i'])

addExtensions (CPC_TestGPLInserter (), ['.cpc'])

addExtensions (BinaryFile (), ['.bmp', '.bpl', '.exe', '.ino', '.jar',
    '.pyc', '.obj', '.ico', '.Ico', '.car', '.awx', '.chc', '.mc', '.BMP',
    '.rbj', '.res', '.tlb', '.o', '.a', '.dll', '.dbg'])

addExtensions (ExternalFile (), ['.0w', '.ap1', '.ap2', '.ap3', '.ap4',
    '.iodbc', '.iodbe', '.ms1', '.ns1', '.ns2', '.ns4', '.od1', '.rpy',
    '.ti', '.skl', ])

addExtensions (GarbageFile (), ['.h#', '.holger', '.org', '.prt',
    '.tmp', '.error', '.exp', '.idb', '.ilk', '.old',
    '.pdb', '.trg', '.vdn', '.dep',
    '.com_notused', '.dld_not_used', '.lnk_not_used',
    '.mac_unused', '.lnk_unused', '.dld_notused', '.lnk_notused',
    '.OLD', '.check', '.bak', '.opt', '.pch', '.plg'])

addExtensions (DemoFile (), ['.B', '.CX', '.CW', '.CD', '.CE', '.D',
    '.DE', '.IN', '.Id', '.Idl', '.Ie', '.Ied', '.cfg', '.cmd', '.cobpc',
    '.cpnix', '.cs1', '.cs2', '.cunix', '.dbmt', '.de', '.en', '.kt',
    '.lcref', '.proc', '.punix', '.tpunix', '.uunix', '.vdnts',
    '.tpunx', '.upnix', '.vunix', '.cpo'])

addExtensions (DefinitionGPLInserter (), ['.def'])

addExtensions (TinyFile (), ['.dsm', '.dsw', '.dt', '.hlf',
     '.apl', '.bat', '.ind', '.inf', '.ini', '.lng', '.m',
     '.mk', '.mp', '.mt', '.par', '.pt', '.seq',
     '.tmp_def', '.tmp_dsp', '.tmp_h', '.tmp_idl', '.tmp_cpp', '.tmp_dsw',
     '.tmp_rc', '.txt', '.use',
     '.lst', '.rgs', '.imp', '.vpj', '.vpw', '.dic', '.dlg', '.hif',
     '.shared', '.MF', '.sn', '.Idd', '.dsp'])

addExtensions (PerlGPLInserter (), ['.pl', '.pm', '.CHL', '.dbm', '.ipm'])

addExtensions (PythonGPLInserter (), ['.py', '.ipy',
    '.diffscr', '.docconf', '.docpkg'])

addExtensions (GenmsgGPLInserter (), ['.genmsg', '.gencid'])

addExtensions (SqlGPLInserter (), ['.sql'])

addExtensions (AssemblerGPLInserter (), ['.s'])

addExtensions (PCF_GPLInserter (), ['.pcf'])

addExtensions (LibGPLInserter (), ['.lib'])

addExtensions (HTML_GPLInserter (), ['.html', '.htm'])

addExtensions (InstallationGPLInserter (), ['.ins'])

addExtensions (MakefileGPLInserter (), ['.mak'])

_remapping = [
    # (re.compile ('.*[.]java$'), JavaGPLInserter ()),
    (re.compile ('geo001x'), C_GPLInserter ()),
    (re.compile ('vni50c'), C_GPLInserter ()),
    (re.compile ('veo51x'), C_GPLInserter ()),
    (re.compile ('veo01x'), C_GPLInserter ()),
    (re.compile ('ven40c'), C_GPLInserter ()),
    (re.compile ('[vg][a-z][a-z][0-9]+[a-z]*$'), VDN_GPLInserter ()),
    (re.compile ('vp[a-z][0-9]{2}[a-z0-9]*$'), VDN_GPLInserter ()),
    (re.compile ('g[a-z][a-z][0-9]+_[0-9]+$'), VDN_GPLInserter ()),
    #(re.compile ('[vg][a-z][a-z][0-9]+[a-z]*[cx]$'), VDN_GPLInserter ()), # c
    #(re.compile ('[vg][a-z][a-z][0-9]+k?a$'), VDN_GPLInserter ()), # assembler
    #(re.compile ('[vg][a-z][a-z][0-9]+au$'), VDN_GPLInserter ()),
    #(re.compile ('[vg][a-z][a-z][0-9]+d$'), VDN_GPLInserter ()), # dummy
    #(re.compile ('[vg][a-z][a-z][0-9]+i$'), VDN_GPLInserter ()), # inlink
    #(re.compile ('[vg][a-z][a-z][0-9]+u$'), VDN_GPLInserter ()),
    (re.compile ('DirectoryMapping'), DescriptionGPLInserter ()),
    (re.compile ('[Ee]xtra'), DescriptionGPLInserter ()),
    (re.compile ('Langextinfo'), DescriptionGPLInserter ()),
    (re.compile ('NI_TRACEFILE'), GarbageFile ()),
    (re.compile ('python'), BinaryFile ()),
    (re.compile ('R[A-Z][a-z][0-9]{5}'), GarbageFile ()),
    (re.compile ('[mM]akefile'), MakefileGPLInserter ()),
    (re.compile ('Makedefs'), MakefileGPLInserter ()),
    (re.compile ('readme', re.I), TinyFile ()),
    (re.compile ('ISUT[.]me'), TinyFile ()),
    (re.compile ('src' + os.sep + os.sep + 'r3' + os.sep + os.sep), DemoFile ()),
    (re.compile ('src' + os.sep + os.sep + 'rx' + os.sep + os.sep), DemoFile ()),
    (re.compile ('src' + os.sep + os.sep + 'zz' + os.sep + os.sep), DemoFile ()),
    (re.compile ('src' + os.sep + os.sep + 'pu' + os.sep + os.sep + 'oci' + os.sep + os.sep ), ExternalFile ()),
    (re.compile ('src' + os.sep + os.sep + 'wd' + os.sep + os.sep + 'fcgi'), ExternalFile ()),
    (re.compile ('src' + os.sep + os.sep + 'wd' + os.sep + os.sep + 'fastcgi[.]h'), ExternalFile ()),
]

_secondLevelExtensions = ['.LC', '.MB', '.MU', '.MX', '.BX', '.N', '.OC', '.OH',
    '.OH', '.OX', '.od', '.oe', '.Od', '.Oe', '.TP', '.TX', '.WC', '.WH',
    '.mi', '.pic', '.r3', '.r3t', '.oci', '.WE', '.Cd', '.Ce',
    '.ta', '.ty', '.tyl', '.tp', '.tt']

def findInserter (fname):
    inserter = None
    extension = splitext (fname) [1]
    if extension in _secondLevelExtensions:
        base = splitext (fname) [0]
        extension = splitext (base) [1]
    if _extmapping.has_key (extension):
        inserter = _extmapping [extension]
    else:
        for regex, inserterTry in _remapping:
            if regex.search (fname):
                inserter = inserterTry
                break
    return inserter

def insertLicence (log, fname, licence):
    if string.find (fname, '.sniffdir') != -1:
        # log.write (fname + " => ignore Sniff files\n")
        return
    inserter = findInserter (fname)
    if inserter:
        editFile (fname)
        log.write ("%s (%s)" % (fname, inserter.kind))
        result = inserter.insertInto (fname, licence)
        log.write (" => " + licence + '\n')
    else:
        log.write (fname + " => no handler\n")

def splitOnLicence (fname):
    #
    # find begin of licence text
    #
    data = open (fname, 'r').read ()
    licguard = '========== licence begin'
    startpos = string.find (data, licguard)
    if startpos == -1:
        return (None, None, None, None, None, 'no start of lic found')
    # extract licence name
    eoline = string.find (data, '\n', startpos)
    licence = string.strip (data [startpos + len (licguard):eoline])
    # move to start of line
    startpos = string.rfind (data, '\n', 0, startpos)
    if startpos == -1:
        startpos = 0
    #
    # find end of licence text
    #
    endpos = string.find (data, '========== licence end', startpos)
    if endpos == -1:
        return (None, None, None, None, startpos, 'no end of lic found')
    endpos = string.find (data, '\n', endpos)
    if endpos == -1:
        endpos = len (data)
    #
    # extract the source parts
    #
    prelic = data [:startpos]
    postlic = data [endpos:]
    licenceText = data [startpos:endpos]
    #
    # extract copyright year
    #
    match = re.search (r'Copyright\s[(][cC][)]\s(\d+)(-\d+)?', licenceText)
    if match:
        firstyear, lastyear = match.group (1, 2)
        firstyear = int (firstyear)
        if lastyear:
            lastyear = int (lastyear[1:])
        else:
            lastyear = firstyear
    else:
        firstyear = lastyear = _thisYear
    #
    # return
    #
    return (1, prelic, postlic, firstyear, lastyear, licence)


def updateLicence (log, fname, licence, updateYear):
    if string.find (fname, '.sniffdir') != -1:
        # log.write (fname + " => ignore Sniff files\n")
        return
    foundlic, prelic, postlic, firstyear, lastyear, lic = splitOnLicence (fname)
    if not foundlic:
        insertLicence (log, fname, licence)
        return
    if lic not in ['GPL', 'LGPL']:
        log.write (fname + " => external software\n")
    inserter = findInserter (fname)
    if not inserter:
        log.write (fname + " => no handler\n")
        return
    if (lic == licence):
        if not updateYear or (lastyear == _thisYear):
            log.write (fname + " => no change\n")
            return
    #
    #
    #
    log.write ("%s (%s) %s => %s\n" % (fname, inserter.kind, lic, licence))
    if noAction:
        return
    editFile (fname)
    stream = open (fname, 'w')
    #
    # handle text pre license
    #
    prelic = updateCopyrightYear (prelic)
    stream.write (prelic)
    #
    # handle license
    #
    licencePattern = inserter.getCommentedLicence (licence)
    if firstyear == _thisYear:
        yearString = str (_thisYear)
    else:
        yearString = '%d-%d' % (firstyear, _thisYear)
    stream.write (licencePattern % yearString)
    #
    # handle text after license
    #
    match = re.match ('\n+(#\n+)+', postlic)
    if match:
        # special handling to prevent adding \n# for every addGPL call
        postlic = '\n' + postlic [match.end ():]
    postlic = updateCopyrightYear (postlic)
    stream.write (postlic)
    stream.close ()

def updateCopyrightYear (text):
    match = re.search (r'Copyright\s+[(][cC][)]\s+(\d+)(-\d+)?\s+SAP\s+AG', text)
    if match:
        firstyear = int (match.group (1))
        if firstyear != _thisYear:
            replacement = 'Copyright (c) %d-%d SAP AG' % (firstyear, _thisYear)
        else:
            replacement = 'Copyright (c) %d SAP AG' % _thisYear
        copStart, copEnd = match.span ()
        text = text [:copStart] + replacement + text [copEnd:]
    return text

def editFile (fname):
    if not isWritable (fname):
        os.system ('p4 edit ' + fname)


_thisYear = time.localtime (time.time ()) [0]

GPL_pattern = """
    ========== licence begin  GPL
    Copyright (c) %s SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
    ========== licence end
"""

GPL = 'GPL'

LGPL_pattern = """
    ========== licence begin LGPL
    Copyright (c) %s SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
    ========== licence end
"""

LGPL = 'LGPL'

_patterns = {
    'GPL': GPL_pattern,
    'LGPL': LGPL_pattern,
}

def licenceName (licence):
    if licence is GPL:
        result = 'GPL'
    elif licence is LGPL:
        result = 'LGPL'
    else:
        result = 'unknown'
    return result


_kernelLayer = string.split ("ak bd gg kb ra su ta")
_kernelSAPDB = string.split ("""
    Catalog CommandGraphModel ConsistentRead Converter DataAccess
    DataDeviceBlockManager File FrameControl FreeBlockManagement
    GarbageCollector IOManager Kernel KernelCommon liveCache Logging
    Oms Parser Pager
    SQLManager SQLParser StatementManager
    Trace Transaction""")

_kernelDirs = []

for entry in _kernelLayer:
    _kernelDirs.append (joinpath ('src', entry))

for entry in _kernelSAPDB:
    _kernelDirs.append (joinpath ('src', 'SAPDB', entry))

def isKernelFile (fname):
    for entry in _kernelDirs:
        if string.find (fname, entry) != -1:
            return true
    return None

_externalSAP = string.split ("""sap sap45A sap50A astyle base-old
    Net SAPDB\\Perforce SAPDB\\PTS rfcsdk
    cn\\zlib install\\perl pu\\oci
    TclTools\\sdbodbc
    tt\\bison tt\\flex tt\md5sum
    tt\\cpan\\SAPDB\\Install\\zip tt\\cpan\\SAPDB\\Install\\zlib
    tt\\cpan\\Term tt\\cpan\\XML""")
_externalDirs = []

for entry in _externalSAP:
    _externalDirs.append (joinpath ('src', entry))

def isExternalFile (fname):
    for entry in _externalDirs:
        if string.find (fname, entry) != -1:
            return true
    return None

def isGPLRelease ():
    relver = os.environ.get ('RELVER', 'NULL')
    if (relver [0] == 'R') and (relver >= 'R75'):
        return 1
    else:
        return None

def expandFile (infile):
    lines = open (infile, 'r').readlines ()
    fnames = map (string.strip, lines)
    return filter (None, fnames)

def expandArgs (args):
    result = []
    for arg in args:
        if arg [0] == '@':
            result.extend (expandFile (arg [1:]))
        else:
            result.append (arg)
    return result


def main (options, args):
    """inserts GPL into source files

    Arguments to addGPL should be give as absolute paths.
    """
    log = open (options.logfile, 'wt')
    if options.verbose:
        log = TeeStream (log, sys.stdout)
    global noAction
    noAction = options.noAction
    if options.updateYear:
        options.update = 1
    for arg in RecursiveFileIterator (expandArgs (args)):
        if isExternalFile (arg):
            log.write (arg + ' (external) => ignored\n')
            continue
        licence = GPL
        try:
            if options.update:
                updateLicence (log, arg, licence, options.updateYear)
            else:
                insertLicence (log, arg, licence)
        except:
            print 'error in', arg
            print '    ', sys.exc_info () [1]
            pass

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        # ('lgpl', None, '::', None, 'insert LGPL in these dirs'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('n', 'noAction', '', None, 'don\'t change any files'),
        ('update', None, '', None, 'update licence type'),
        ('updateYear', None, '', None, 'update year of copyright'),
        ('version', None, '', _version, 'version information'),
        ('logfile', None, ':', 'addGPL.log', 'name of logFile'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())


