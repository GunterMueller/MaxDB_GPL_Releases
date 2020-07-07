
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
import re
import os
import shutil
import whrandom
import time

from stat import ST_MODE

from acquisition import Acquest
import html

from FileSystem import makeRelative, File
from NoWeaver import NoWeaver
from docLib import genDate
from DocHeader import DocHeader
from cppDocLib import CppDoc
import DocDirDescription
import TestCaseDescription

try:
    import pythoncom
    from win32com import storagecon

    FMTID_UserDefinedProperties = "{F29F85E0-4FF9-1068-AB91-08002B27B3D9}"

    PIDSI_TITLE           =   0x00000002
    PIDSI_SUBJECT         =   0x00000003
    PIDSI_AUTHOR          =   0x00000004
    PIDSI_KEYWORDS        =   0x00000005
    PIDSI_COMMENTS        =   0x00000006

except ImportError:
    _hasCom = None

_version = "1.3.02"

class Info:
    title = None
    responsible = None
    abstract = None
    description = None
    keywords = None

    def __init__ (self, fname, dom, **args):
        self.title = fname
        for key, value in args.items ():
            setattr (self, key, value)
        if hasattr (dom, 'source') and (infoDom.source == "MSOffice"):
            if _hasCom:
                self.readOfficeProperties (fname)
            else:
                # no Com extensions installed
                pass
        else:
            self.readDOMProperties (dom)

    def readOfficeProperties (self, shellName):
        if not pythoncom.StgIsStorageFile (filename):
            self.abstract = 'not a Storage file'

        flags = storagecon.STGM_READ | storagecon.STGM_SHARE_EXCLUSIVE
        stg = pythoncom.StgOpenStorage (filename, None, flags)

        try:
            pss = stg.QueryInterface (pythoncom.IID_IPropertySetStorage)
        except pythoncom.com_error:
            self.abstract = ''
            return

        # open properties
        ps = pss.Open (FMTID_UserDefinedProperties)
        props = PIDSI_TITLE, PIDSI_SUBJECT, PIDSI_KEYWORDS
        self.title, self.abstract, keywordText = ps.ReadMultiple (props)
        self.keywords = string.split (keywordText, ',')

    def readDOMProperties (self, dom):
        for propname in ['title', 'abstract', 'description']:
            try:
                text = getattr (dom, propname).pcdata
                setattr (self, propname, text)
            except AttributeError:
                pass
        if hasattr (dom, 'keywords') and dom.keywords:
            keywords = []
            for keywordNode in dom.keywords:
                keywords.append (string.strip (keywordNode.pcdata))
            self.keywords = keywords

class DocSakeFlags:
    unconditional = None
    noAction = None
    verbose = None
    recursive = None

defaultFlags = DocSakeFlags ()

class DocSakeError:
    pass

class PackageNotFound (DocSakeError):
    def __init__ (self, packageName):
        self.packageName = packageName

    def __str__ (self):
        return '<PackageNotFound: ' + self.packageName + '>'

class DocSake (Acquest):
    configFile = '.docconf'
    isMain = None

    def __init__ (self, file, context = None):
        Acquest.__init__ (self, context)
        self.file = file
        if file.isdir ():
            self.dir = file
        else:
            self.dir = file.directory ()
        self.loadConfig ()

    def __str__ (self):
        return '<' + self.__class__.__name__ + ' ' + self.file.name + '>'

    def showError (self, msg):
        sys.stderr.write (msg + '\n')

    def loadConfig (self):
        try:
            superDir = self.acquisitionContext.dir
        except AttributeError:
            # nasty hack because MainDoc hasn't a valid acquisitionContext
            return
        diff = makeRelative (self.dir.name, superDir.name)
        if not diff:
            return
        parts = filter (None, string.split (diff, '/'))
        dir = superDir
        for part in parts:
            dir = dir.join (part)
            self.loadConfigFile (dir)

    def loadConfigFile (self, dir):
        file = dir.join (self.configFile)
        if file.exists ():
            shellName = file.shellName ()
            # print 'loading Config', shellName
            execfile (shellName, self.__dict__)

    def list (self, indent = ''):
        print indent + self.file.basename ()

    def factory (self, fname):
        try:
            if fname [-1] in ['/', '\\']:
                fname = fname [:-1]
        except IndexError:
            pass # empty fname is OK
        file = self.dir.join (fname)
        if self.isBackup (file):
            return None
        if file.isdir ():
            if file.basename () == self.acquire ('docDir'):
                constructor = DocTree
            elif file.join (self.acquire ('sourceContent')).exists ():
                if file.join ('.docignore').exists ():
                    constructor = None
                else:
                    constructor = SourceDir
            else:
                constructor = None
        else:
            ext = file.extension ()
            if ext in self.acquire ('sourceExtensions'):
                constructor = DocSource
            elif ext in self.acquire ('docExtensions'):
                constructor = DocFile
            elif ext in self.acquire ('pkgExtensions'):
                constructor = DocPackage
            else:
                constructor = None
        if constructor != None:
            result = constructor (file, self)
        else:
            result = None
        return result

    def factoryFilter (self, item):
        if type (item) == type (''):
            result = self.factory (item)
        else:
            result = item
        return result

    def strings2Objects (self, list):
        result = filter (None, map (self.factoryFilter, list))
        return result

    def isBackup (self, file):
        try:
            res = self.backupREs
        except AttributeError:
            res = map (re.compile, self.acquire ('backupPatterns'))
            self.backupREs = res
        base = file.basename ()
        for r in res:
            if r.match (base):
                return 1
        return None

    def getBodyAttributes (self):
        attributes = self.acquire ('htmlBodyAttributes')
        return attributes

    def alreadyDone (self, target):
        targetsMade = self.acquire ('targetsMade')
        key = str (target)
        if targetsMade.has_key (key):
            return 1
        else:
            targetsMade [key] = 1
            return None

class MainDoc (DocSake):
    docDir = ''
    sourceContent = 'yadda yadda yadda'
    sourceExtensions = []
    docExtensions = []
    pkgExtensions = ['.docpkg']
    backupPatterns = []
    weaver = NoWeaver ()

    def __init__ (self, sourceFS, docsrcFS, fname):
        DocSake.__init__ (self, sourceFS.file ())
        self.docsrcFS = docsrcFS
        self.loadConfigFile (self.dir)
        fname = sourceFS.file (fname).name   # make relative
        self.mainTarget = self.factory (fname)
        if self.mainTarget == None:
            raise PackageNotFound (fname)
        self.mainTarget.isMain = 1
        self.docDirsCopied = {}
        self.targetsMade = {}
        self.detectRelease ()

    def __str__ (self):
        return '<MainDoc at %x>' % id (self)

    def detectRelease (self):
        try:
            relver = os.environ ['RELVER']
            self.release = relver [1] + '.' + relver [2]
        except:
            self.release = ''

    def list (self, indent = ''):
        if self.mainTarget:
            self.mainTarget.list (indent)

    def make (self, targetFS, flags = defaultFlags):
        if self.mainTarget:
            topFile = File ('index.html', targetFS)
            self.docHeader = DocHeader (topFile, self.weaver)
            self.weaver.docHeader = self.docHeader
            ok, result = self.mainTarget.make (targetFS, flags)
            if flags.index:
                self.weaver.genMainIndex (topFile.directory ())
            self.weaver.save ()
            del self.docHeader
        else:
            ok, result = None, None
        return ok, result

class AbstractDocPackage (DocSake):
    def list (self, indent = ''):
        DocSake.list (self, indent)
        memberlist = self.memberList ()
        for member in memberlist:
            member.list (indent + '    ')

    def __repr__ (self):
        return '<' + self.__class__.__name__ + ' ' + self.name () + '>'

    def memberList (self):
        try:
            result = self.members
        except AttributeError:
            self.loadMembers ()
            if hasattr (self, 'members'):
                self.members = self.strings2Objects (self.members)
            else:
                self.members = []
            result = self.members
        return result

    def loadMembers (self):
        print 'not implemented: loadMembers for', self.__class__.__name__

    def make (self, targetFS, flags):
        if self.alreadyDone (self.file):
            noMake = 1
        elif self.isMain:
            noMake = None
        elif flags.recursive:
            noMake = None
        else:
            noMake = 1
        if noMake:
            sourceFile = self.file
            weaver = self.acquire ('weaver')
            title, href, abstract, isPackage, lastUpdated = weaver.getFileInfo (str (sourceFile))
            result = DocInfo (title, File (href, sourceFile.filesystem), abstract,
                isPackage, lastUpdated)
            return 1, result
        ok = 1
        wantIndex = None
        if flags.verbose:
            print "%-20s %s" % (self.__class__.__name__, self.file)
        #
        # doc dir description
        #
        dirDescription, docdir = self.loadDocDirDescription ()
        if docdir and flags.recursive:
            excludeInfo = ExcludeInfo (self, dirDescription)
            self.makeDocDir (targetFS, docdir, excludeInfo, flags)
        docHeader = self.acquire ('docHeader')
        assert (docHeader != None)
        sourcehtml = self.htmlfileName (self.file.filesystem)
        targethtml = sourcehtml.mapTo (targetFS)
        docHeader.pushPackage (targethtml)
        #
        # cpp doc members
        #
        memberList = self.memberList ()
        memberdocs = []
        weaver = self.acquire ('weaver')
        assert (weaver != None)
        if hasattr (self, 'index'):
            weaver.pushIndex (self.index, targethtml.directory ())
            wantIndex = 1
        for member in memberList:
            thisOK, info = member.make (targetFS, flags)
            assert info != None
            memberdocs.append (info)
            ok = ok and thisOK
            if ok and wantIndex:
                weaver.save ()
        #
        # generate overview
        #
        if None:  # set this to true if overview of changed pages is wanted
            if wantIndex:
                updatedDocs = weaver.getUpdatedForCurrentIndex ()
            else:
                updatedDocs = None
        else:
            updatedDocs = None
        info = self.genOverview (targetFS, docHeader, dirDescription,
            memberdocs, updatedDocs)
        weaver.addFileInfo (info.title, str (self.file), str (info.file), info.abstract, info.isPackage, info.lastUpdated)
        if wantIndex:
            weaver.genCurrentIndex ()
            weaver.popIndex ()
        docHeader.popPackage ()
        return ok, info

    def loadDocDirDescription (self):
        docsrcFS = self.acquire ('docsrcFS')
        assert docsrcFS != None
        pkgdocDir = File (self.dir.name, docsrcFS)
        ddd = pkgdocDir.join (self.name () + '.ddd')
        if ddd.exists ():
            result = self.loadDirDescription (ddd)
        else:
            result = self.genDefaultDirDescription ()
        return result, pkgdocDir

    def genDefaultDirDescription (self):
        result = DocDirDescription.Directory ()
        info = DocDirDescription.Info ()
        title = DocDirDescription.Title ()
        title.pcdata = 'Package ' + self.name ()
        info.title = title
        description = self.getDescription ()
        if description:
            abstract = DocDirDescription.Abstract ()
            abstract.pcdata = description
        else:
            abstract = None
        info.abstract = abstract
        info.description = None
        info.keywords = None
        result.info = info
        result.documents = None
        result.filesToIgnore = None
        return result

    def loadDirDescription (self, ddd):
        data = ddd.text ()
        result = DocDirDescription.parseData (data)
        return result

    def genOverview (self, targetFS, docHeader, dirDescription,
            memberdocs, updatedDocs):
        dirInfo = Info (str (self.file), dirDescription.info)
        mappedFile = self.file.mapTo (targetFS)
        docDir = self.dir.mapTo (targetFS)
        htmlfile = docDir.join (self.name () + '.html')
        title = dirInfo.title
        release = self.acquire ('release')
        targetFS.require (htmlfile.name)
        htmlstream = html.HTMLFile (htmlfile.shellName (),
            release + ': ' +title,
            bgcolor = self.acquire ('bgcolor', '#FFFFFF'))
        # write header
        htmlstream.write (docHeader.packageHeader (htmlfile))
        htmlstream.titleHeader (title + ' - ' + release)
        htmlstream.pushTag ('Blockquote')
        # write description
        if dirInfo.abstract:
            htmlstream.write (html.paragraph (dirInfo.abstract))
        if dirInfo.description:
            htmlstream.write (html.paragraph (dirInfo.description))
        self.insertInfo (str (htmlfile), dirInfo)
        # write links to members
        documents = self.extractDocuments (dirDescription, docDir)
        packages = self.extractPackages (memberdocs)
        sources = self.extractSources (memberdocs)
        tests = self.extractTests (targetFS)
        # updated = self.extractUpdated (memberdocs)
        for title, memberList in [
                ('Documents', documents),
                ('Packages', packages),
                ('Headerfiles', sources),
                ('Tests', tests)]:
            if memberList:
                self.writeTable (htmlstream, docDir, title, memberList)
                htmlstream.lineBreak ()
        # self.writeTestDirectory (htmlstream)
        if updatedDocs:
            self.writeTable (htmlstream, docDir, 'Updated', updatedDocs)
            htmlstream.lineBreak ()
        # close file
        htmlstream.line ()
        genDate (htmlstream)
        htmlstream.close ()
        if dirInfo.abstract:
            abstract = dirInfo.abstract
        else:
            abstract = None
        return DocInfo (self.name (), htmlfile, abstract, 'true', 0)

    def extractDocuments (self, dirInfo, docDir):
        result = []
        if not dirInfo.documents:
            return result
        for document in dirInfo.documents:
            if document.file:
                href = docDir.join (document.file.HREF)
            elif document.ref:
                href = document.ref.HREF
            if document.info:
                info = Info (href, document.info)
                title = info.title
                abstract = info.abstract
            else:
                title = href
                abstract = ''
            result.append (DocInfo (title, href, abstract, None))
            if document.info:
                self.insertInfo (str (href), info)
        return result

    def insertInfo (self, href, info):
        if not info.keywords:
            return
        weaver = self.acquire ('weaver')
        weaver.setCurrentFile (href)
        for keyword in info.keywords:
            weaver.addKeyword (keyword)

    def extractPackages (self, memberdocs):
        result = []
        for info in memberdocs:
            if info.isPackage:
                result.append (info)
        return result

    def extractSources (self, memberdocs):
        result = []
        for info in memberdocs:
            if not info.isPackage:
                result.append (info)
        return result

    def extractUpdated (self, memberdocs):
        list = []
        margin = time.time () - updatedSpan
        for info in memberdocs:
            if info.lastUpdated > margin:
                list.append ((info.lastUpdated, info))
        if not list:
            return []
        list.sort ()
        result = []
        for timestamp, info in list:
            result.append (info)
        return result

    tableTitle = '''
<TABLE BORDER="1" CELLPADDING="3" CELLSPACING="0" WIDTH="90%%">
<TR BGCOLOR="#CCCCFF" VALIGN="top">
<TD COLSPAN=2><FONT SIZE="+2">
<B>%s</B></FONT></TD>
</TR>
'''
    def writeTable (self, htmlstream, docDir, title, memberList):
        htmlstream.write (self.tableTitle % title)
        htmlstream.pseudoBreak ()
        for info in memberList:
            # write link
            htmlstream.pushTag ('TR', BGCOLOR = "white")
            htmlstream.pushTag ('TD', WIDTH = "15%")
            if type (info.file) == type (''):
                # a normal href, left unchanged
                relative = info.file
            else:
                # an absolute doc address, made relative to caller
                relative = info.file.relativeTo (docDir)
            anchor = html.href (relative, str (info.title))
            htmlstream.write (html.tagged ('B', anchor))
            htmlstream.popTag ()
            # write description
            htmlstream.pushTag ('TD')
            htmlstream.write (info.formatDescription ('&nbsp;'))
            htmlstream.popTag () # TD
            htmlstream.popTag () # TR
            htmlstream.pseudoBreak ()
        htmlstream.write ('</TABLE>')

    def extractTests (self, targetFS):
        result = []
        # get source dir
        testDir = self.file.join ('test')
        # list files, filter tcd
        filenames = testDir.listdir ()
        filenames = filter (lambda f: f [-4:] == '.tcd', filenames)
        # make target dir
        targetDir = testDir.mapTo (targetFS)
        # convert tcd to html
        for filename in filenames:
            # make full name
            descriptionText = testDir.join (filename).text ()
            # read into FOM
            testDescription = TestCaseDescription.parseData (descriptionText)
            # create target name
            basename = filename [:-4] + '.html'
            targetName = targetDir.join (basename)
            targetFS.require (targetName.name)
            # generate
            testDescription .genHTMLFile (targetName.shellName ())
            # extract data
            result.append (DocInfo (testDescription.getTitle (),
                testDir.join (basename),
                testDescription.getAbstract (), None))
        return result

    testNames = ['None', 'Nada', 'rien', 'Zilch',
        'You mean besides compiling and linking?',
        'Real Soon Now']

    def writeTestDirectory (self, htmlstream):
        htmlstream.write (self.tableTitle % "Tests")
        htmlstream.pseudoBreak ()
        htmlstream.pushTag ('TR', BGCOLOR = "white")
        htmlstream.pushTag ('TD', COLSPAN = 2)
        index = int (whrandom.random () * len (self.testNames))
        htmlstream.write (self.testNames [index]
            + """<i>- [This is a reminder that there is currently
            no concept to integrate documentation of available tests]""")
        htmlstream.popTag (2)
        htmlstream.write ('</TABLE>')
        htmlstream.lineBreak ()

    def makeDocDir (self, targetFS, docdir, excludeInfo, flags):
        unconditional = flags.unconditional
        docDirsCopied = self.acquire ('docDirsCopied')
        assert docDirsCopied != None
        key = str (docdir)
        if docDirsCopied.has_key (key):
            return
        else:
            docDirsCopied [key] = 1
        targetDir = docdir.mapTo (targetFS)
        targetDir.mkdir ()
        list = docdir.listdir ()
        for file in list:
            if excludeInfo.allow (file):
                full = docdir.join (file)
                if full.isdir ():
                    if flags.verbose:
                        print "%-20s %s" % ("DocDir", full)
                    targetdir = full.mapTo (targetFS)
                    targetdir.mkdir ()
                    self.makeDocDir (targetFS, full, excludeInfo, flags)
                else:
                    self.copyFile (targetFS, full, unconditional)

    def copyFile (self, targetFS, source, unconditional):
        target = source.mapTo (targetFS)
        if unconditional or source.isNewerThan (target):
            targetName = target.shellName ()
            shutil.copy2 (source.shellName (), targetName)
            privs = os.stat (targetName)[ST_MODE]
            newPrivs = privs | 222   # make writable
            if privs != newPrivs:
                os.chmod (targetName, newPrivs)

class ExcludeInfo:
    def __init__ (self, context, dirDescription):
        self.reExclude = [re.compile ('.*[.]ddd$')]
        self.fileExclude = []
        self.extractExcludeInfo (dirDescription)

    def extractExcludeInfo (self, dirDescription):
        if not dirDescription.filesToIgnore:
            return
        for item in dirDescription.filesToIgnore:
            if isinstance (item, DocDirDescription.File):
                self.fileExclude.append (item.HREF)
            elif isinstance (item, DocDirDescription.FilePattern):
                pattern = item.pattern
                try:
                    style = item.style
                except AttributeError:
                    style = "re"
                if style == "re":
                    self.reExclude.append (re.compile (pattern))
                else:
                    raise 'unsupported pattern style', style
            else:
                raise 'Invalid Element in <FilesToIgnore>', item

    def allow (self, fname):
        for expr in self.reExclude:
            if expr.match (fname):
                return None
        #for expr in self.regexExclude:
        #    if expr.match (fname) != -1:
        #        return None
        if fname in self.fileExclude:
            return None
        return 1

class DocPackage (AbstractDocPackage):
    def loadMembers (self):
        shellName = self.file.shellName ()
        if not shellName:
            raise PackageNotFound (str (self.file))
        execfile (shellName, self.__dict__)

    def getDescription (self):
        if hasattr (self, 'description'):
            result = self.description
        else:
            result = ''
        return result

    def name (self):
        fullname = self.file.basename ()
        main, ext = os.path.splitext (fullname)
        return main

    def htmlfileName (self, targetFS):
        docDir = self.dir.mapTo (targetFS)
        htmlfile = docDir.join (self.name () + '.html')
        return htmlfile

class SourceDir (AbstractDocPackage):
    def loadMembers (self):
        list = self.file.listdir ()
        members = self.strings2Objects (list)
        packageMembers = []
        fileMembers = []
        for member in members:
            if isinstance (member, AbstractDocPackage):
                packageMembers.append (member)
            else:
                fileMembers.append (member)
        self.members = packageMembers + fileMembers

    def name (self):
        return self.file.basename ()

    def htmlfileName (self, targetFS):
        docDir = self.dir.mapTo (targetFS)
        htmlfile = docDir.join (self.name () + '.html')
        return htmlfile

    contentsRE = re.compile ('\n([a-zA-Z][a-zA-Z ]*):')

    def parseContentsFile (self, contentsFile):
        text = contentsFile.text ()
        # parse into entries
        entries = self.contentsRE.split ('\n' + text)
        # get description entry
        for i in xrange (len (entries)):
            if entries [i] == 'contents':
                return entries [i + 1]
        return ''

    def getDescription (self):
        contentsFile = self.acquire ('sourceContent')
        if not contentsFile:
            return ''
        contentsFile = self.dir.join (contentsFile)
        if contentsFile.exists ():
            description = self.parseContentsFile (contentsFile)
        else:
            description = ''
        return description

class DocTree (AbstractDocPackage):
    pass

class DocSource (DocSake):
    def make (self, targetFS, flags):
        sourceFile = self.file
        targetFile = File (sourceFile.name, targetFS)
        targetFile = targetFile.setExtension ('.html')
        unconditional = flags.unconditional
        noAction = flags.noAction
        verbose = flags.verbose or noAction
        ok = 1
        result = DocInfo (self.file.basename (), targetFile, 'ignored', None, sourceFile.getmtime ())
        if unconditional or sourceFile.isNewerThan (targetFile):
            ext = sourceFile.extension ()
            documenter = _documenters [ext] (self.getBodyAttributes ())
            if verbose:
                print "%-20s %s" % (documenter.__class__.__name__, sourceFile)
            if noAction:
                pass
            else:
                self.promoteContext (documenter)
                ok = documenter.readSource (sourceFile,
                    requireComments = None)
                if ok:
                    outfile = targetFS.require (targetFile.name)
                    documenter.genHTML (targetFile)
                    result = DocInfo (self.file.basename (), targetFile, documenter.getShortdesc (), None, sourceFile.getmtime ())
                else:
                    result = DocInfo (self.file.basename (), targetFile, 'cppDoc Error', None, sourceFile.getmtime ())
                weaver = self.acquire ('weaver')
                weaver.addFileInfo (result.title, str (self.file), str (result.file), result.abstract,
                    result.isPackage, result.lastUpdated)
                weaver.endTransaction (ok)
                documenter.uncoupleDependents ()
        else:
            weaver = self.acquire ('weaver')
            title, href, abstract, isPackage, lastUpdated = weaver.getFileInfo (str (sourceFile))
            result = DocInfo (title, File (href, sourceFile.filesystem), abstract, isPackage, lastUpdated)
        return ok, result

class DocFile (DocSake):
    pass

_documenters = {
    '.h': CppDoc,
    '.hpp': CppDoc,
    }

updatedSpan = 2 * 7 * 24 * 60 * 60   # 2 weeks

class DocInfo:
    def __init__ (self, title, file, abstract = '',
            isPackage = None, lastUpdated = 0):
        self.title = title
        self.file = file
        if abstract is None:
            abstract = ''
        self.abstract = abstract
        self.isPackage = isPackage
        self.lastUpdated = lastUpdated

    def __repr__ (self):
        return '<DocInfo ' + self.title + '>'

    def updated_p (self):
        if time.time () - self.lastUpdated < updatedSpan:
            result = time.strftime (' <FONT color="crimson">Updated %Y-%m-%d</FONT>',
                time.localtime (self.lastUpdated))
        else:
            result = ''
        return result

    def formatDescription (self, default = ''):
        description = self.abstract + self.updated_p ()
        if not description:
            description = default
        return description



