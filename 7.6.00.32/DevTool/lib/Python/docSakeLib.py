
import string
import re
import os
import shutil

from acquisition import Acquest
from FileSystem import makeRelative, File
from NoWeaver import NoWeaver
from docLib import genDate
from DocHeader import DocHeader
from cppDocLib import CppDoc
import html

_version = 0.1

class DocSakeFlags:
    unconditional = None
    noAction = None
    verbose = None
    recursive = None
    index = 'index.html'

defaultFlags = DocSakeFlags ()

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
            return self.factory (item)
        else:
            return item

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

class MainDoc (DocSake):
    docDir = ''
    sourceContent = 'yadda yadda yadda'
    sourceExtensions = []
    docExtensions = []
    pkgExtensions = ['.docpkg']
    backupPatterns = []
    weaver = NoWeaver ()

    def __init__ (self, sourceFS, fname):
        DocSake.__init__ (self, sourceFS.file ())
        self.loadConfigFile (self.dir)
        fname = sourceFS.file (fname).name   # make relative
        self.mainTarget = self.factory (fname)
        self.mainTarget.isMain = 1

    def __str__ (self):
        return '<MainDoc at %x>' % id (self)

    def list (self, indent = ''):
        if self.mainTarget:
            self.mainTarget.list (indent)

    def make (self, targetFS, flags = defaultFlags):
        if self.mainTarget:
            topFile = File (flags.index, targetFS)
            self.docHeader = DocHeader (topFile, self.weaver)
            self.weaver.docHeader = self.docHeader
            ok, result = self.mainTarget.make (targetFS, flags)
            self.weaver.genMainIndex (topFile.directory ())
            del self.docHeader
        else:
            ok, result = None, None
        return ok, result

class AbstractDocPackage (DocSake):
    def list (self, indent = ''):
        DocSake.list (self, indent)
        for member in self.memberList ():
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
        if not flags.recursive and not self.isMain:
            htmlfile = self.htmlfileName (targetFS)
            info = (self.name (), htmlfile, '')
            return 1, info
        ok = 1
        wantIndex = None
        if flags.verbose:
            print "%-20s %s" % (self.__class__.__name__, self.file)
        docHeader = self.acquire ('docHeader')
        assert (docHeader != None)
        sourcehtml = self.htmlfileName (self.file.filesystem)
        targethtml = sourcehtml.mapTo (targetFS)
        docHeader.pushPackage (targethtml)
        memberList = self.memberList ()
        memberdocs = []
        if hasattr (self, 'index'):
            weaver = self.acquire ('weaver')
            assert (weaver != None)
            weaver.pushIndex (self.index, targethtml.directory ())
            wantIndex = 1
        for member in memberList:
            thisOK, info = member.make (targetFS, flags)
            assert info != None
            memberdocs.append (info)
            ok = ok and thisOK
            if ok and wantIndex:
                weaver.save ()
        if sourcehtml.exists ():
            if flags.unconditional or sourcehtml.isNewerThan (targethtml):
                shutil.copy2 (sourcehtml.shellName (), targethtml.shellName ())
            info = (self.name (), targethtml, '')
        else:
            info = self.genOverview (targetFS, docHeader, memberdocs)
        if wantIndex:
            weaver.genCurrentIndex ()
            weaver.popIndex ()
        docHeader.popPackage ()
        return ok, info

#     def genOverview (self, targetFS, docHeader, memberdocs):
#         # get package description
#         description = self.getDescription ()
#         # todo: get old descriptions
#         #     merge and save
#         #     set new and updated
#         # open html stream
#         mappedFile = self.file.mapTo (targetFS)
#         docDir = self.dir.mapTo (targetFS)
#         htmlfile = docDir.join (self.name () + '.html')
#         title = 'Package ' + self.name ()
#         targetFS.require (htmlfile.name)
#         htmlstream = html.HTMLFile (htmlfile.shellName (), title,
#             bgcolor = self.acquire ('bgcolor', '#FFFFFF'))
#         # write header
#         htmlstream.write (docHeader.packageHeader (htmlfile))
#         htmlstream.titleHeader (title)
#         htmlstream.pushTag ('Blockquote')
#         # write description
#         if description:
#             htmlstream.write (description)
#         # write links to members
#         htmlstream.pushTag ('DL')
#         htmlstream.pseudoBreak ()
#         for name, file, memberDesc in memberdocs:
#             # write link
#             htmlstream.tag ('DT')
#             relative = file.relativeTo (docDir)
#             htmlstream.write (html.href (relative, str (name)) + '\n')
#             # write description
#             if memberDesc:
#                 htmlstream.tag ('DD')
#                 htmlstream.write (string.strip (memberDesc))
#                 htmlstream.pseudoBreak ()
#         htmlstream.popTag ()
#         # close file
#         htmlstream.line ()
#         genDate (htmlstream)
#         htmlstream.close ()
#         return (self.name (), htmlfile, description)

    def genOverview (self, targetFS, docHeader, memberdocs):
        # get package description
        description = self.getDescription ()
        # todo: get old descriptions
        #     merge and save
        #     set new and updated
        # open html stream
        mappedFile = self.file.mapTo (targetFS)
        docDir = self.dir.mapTo (targetFS)
        htmlfile = docDir.join (self.name () + '.html')
        title = 'Package ' + self.name ()
        targetFS.require (htmlfile.name)
        htmlstream = html.HTMLFile (htmlfile.shellName (), title,
            bgcolor = self.acquire ('bgcolor', '#FFFFFF'))
        # write header
        htmlstream.write (docHeader.packageHeader (htmlfile))
        htmlstream.titleHeader (title)
        htmlstream.pushTag ('Blockquote')
        # write description
        if description:
            htmlstream.write (description)
        # write links to members
        htmlstream.write ("""
<TABLE BORDER="1" CELLPADDING="3" CELLSPACING="0" WIDTH="90%">
<TR BGCOLOR="#CCCCFF">
<TD COLSPAN=2><FONT SIZE="+2">
<B>Summary</B></FONT></TD>
</TR>
""")
        htmlstream.pseudoBreak ()
        for name, file, memberDesc in memberdocs:
            # write link
            htmlstream.pushTag ('TR', BGCOLOR = "white")
            htmlstream.pushTag ('TD', WIDTH = "15%")
            relative = file.relativeTo (docDir)
            anchor = html.href (relative, str (name))
            htmlstream.write (html.tagged ('B', anchor))
            htmlstream.popTag ()
            # write description
            htmlstream.pushTag ('TD')
            if memberDesc:
                htmlstream.write (string.strip (memberDesc))
            else:
                htmlstream.write ('&nbsp;')
            htmlstream.popTag () # TD
            htmlstream.popTag () # TR
            htmlstream.pseudoBreak ()
        htmlstream.write ('</TABLE>')
        # close file
        htmlstream.line ()
        genDate (htmlstream)
        htmlstream.close ()
        return (self.name (), htmlfile, description)

class DocPackage (AbstractDocPackage):
    def loadMembers (self):
        shellName = self.file.shellName ()
        if not shellName:
            raise 'FileNotFound', (shellName, str (self.file))
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
        result = (self.file.basename (), targetFile, 'ignored')
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
                    result = (self.file.basename (), targetFile, documenter.getShortdesc ())
                else:
                    result = (self.file.basename (), targetFile, 'cppDoc Error')
                weaver = self.acquire ('weaver')
                # weaver.addFilesToIndex
                weaver.endTransaction (ok)
        return ok, result

class DocFile (DocSake):
    pass

_documenters = {
    '.h': CppDoc,
    '.hpp': CppDoc,
    }

