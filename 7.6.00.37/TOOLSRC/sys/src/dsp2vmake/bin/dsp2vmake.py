#!/usr/bin/env python
# dsp2vmake.py
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


import sys
import os
import string
import re
from cStringIO import StringIO
from UserDict import UserDict

try:
    import warnings
    warnings.filterwarnings ("ignore", ".*xmllib")
except ImportError:
    pass

import xmllib

import optlib

_perforce = "$Change: 592486 $"
_version = _perforce [1:-1]

#
# import specialCases.py (should be in $SRC)
#
vmakePath = None
try:
    def addSRC (dirname):
        return os.path.join (dirname, 'sys', 'src')

    vmakePath = filter (None, string.split (os.environ ['VMAKE_PATH'], ','))
    srcPath = map (addSRC, vmakePath)
    sys.path [:0] = srcPath
except KeyError:
    pass

import specialCases

if vmakePath:
    del sys.path [:len (vmakePath)]

def loadSpecialAttribute (name, default = None):
    try:
        result = getattr (specialCases, name)
    except AttributeError:
        result = default
    return result

def checkSpecialCases ():
    #
    # check fileAddOn variable
    #
    fileAddOn = loadSpecialAttribute ('fileAddOn')
    if not fileAddOn:
        return
    for element in fileAddOn:
        try:
            kind, matcher, addOn = element
        except ValueError:
            raise 'specialCases.py:fileAddOn: wrong format', element
        if kind == 'extension':
            if type (matcher) != type (''):
                raise 'specialCases.py:fileAddOn: matcher for "extension" must be string with file extension', repr (matcher)
        elif kind == 'extensions':
            if type (matcher) != type ([]):
                raise 'specialCases.py:fileAddOn: matcher for "extensions" must be list of file extensions', repr (matcher)
        elif kind == 'match':
            if type (matcher) != type (''):
                raise 'specialCases.py:fileAddOn: matcher for "match" must be string with regular expression', repr (matcher)
            try:
                re.compile (matcher)
            except:
                exckind, excvalue, backtrace = sys.exc_info ()
                raise 'specialCases.py:fileAddOn: ' + str (excvalue), repr (matcher)
        elif kind == 'matchany':
            if type (matcher) != type ([]):
                raise 'specialCases.py:fileAddOn: matcher for "match" must be list of regular expressions', repr (matcher)
            for rex in matcher:
                try:
                    re.compile (rex)
                except:
                    exckind, excvalue, backtrace = sys.exc_info ()
                    raise 'specialCases.py:fileAddOn: ' + str (excvalue), repr (rex)
        else:
            raise 'specialCases.py:fileAddOn: wrong tag', kind

def findFileAddOn (filename):
    fileAddOn = loadSpecialAttribute ('fileAddOn')
    if not fileAddOn:
        return None
    for kind, matcher, addOn in fileAddOn:
        if kind == 'extension':
            if filename [-len (matcher):] == matcher:
                return addOn
        elif kind == 'extensions':
            for extension in matcher:
                if filename [-len (extension):] == extension:
                    return addOn
        elif kind == 'match':
            if re.search (matcher, filename):
                return addOn
        elif kind == 'matchany':
            for rex in matcher:
                if re.search (rex, filename):
                    return addOn

checkSpecialCases ()

#
# import imkdepLib
#
try:
    import vmake.imkdepLib
    dependencySection = vmake.imkdepLib.DepUpdater.dependencySection
except ImportError:
    dependencySection = """#
# include dependencies (do not edit)
#
"""

#
# import imkdep
#
imkdep = None
if os.environ.get ('SRC') and os.environ.get ('DESC'):
    try:
        import imkdep
    except ImportError:
        pass

true = 'true'
false = None

def fatalError (msg):
    sys.stderr.write ('dsp2vmake.py: %s\n' % msg)

def trace (fmt, *args):
    if args:
        line = fmt % args
    else:
        line = '%s' % fmt
    sys.stdout.write (line + '\n')

def normpath (fname):
    return os.path.normpath (os.path.normcase (fname))

def extension (fname):
    return os.path.splitext (fname) [1]


def smartOpen (fname, mode):
    dirname = os.path.dirname (fname)
    if not os.path.exists (dirname):
        os.makedirs (dirname)
    return open (fname, mode)

def stripQuotes (text):
    if (text [0] in "\"\'") and (text [0] == text [-1]):
        return text [1:-1]
    else:
        return text

_includeExtensions = ['.h', '.hpp']
_allowExtensions = ['.c', '.cpp', '.cxx', '.tli', '.h', '.tlh',
    '.inl', '.rc', '.def', '.idl', '.xml']
_denyExtensions = []

def allowFileByExtension (fname):
    extension = os.path.splitext (fname) [1]
    if len (_allowExtensions) > 0:
        return extension in _allowExtensions
    if len (_denyExtensions) > 0:
        return extension not in _denyExtensions
    return true

class Dsp2Vmake:
    rcRE = re.compile ('.*[.]rc$')
    defRE = re.compile ('.*[.]def$')
    def __init__ (self, dollarSrc, dollarDesc, knownLibs, options):
        self.returnCode = 0
        self.dollarSrc = normpath (dollarSrc)
        self.srcLen = len (self.dollarSrc) + 1
        self.dollarDesc = normpath (dollarDesc)
        self.descLen = len (self.dollarDesc) + 1
        self.knownLibs = knownLibs
        self.options = options
        self.layer = LayerDict ()
        self.optionSampler = OptionSampler ()
        self.specialTargetHandling = [
            (self.rcRE, self.win32OnlyWrite),
            (self.defRE, self.win32OnlyWrite),
        ]
        if specialCases.__dict__.get ('xmlidl'):
            self.nameTranslations = [
                (re.compile ('(.*)Ispc[.]cpp$'), r'\1.xml'),
            ]
        else:
            self.nameTranslations = [
                (re.compile ('(.*)Ispc[.]cpp$'), r'\1.idl'),
            ]
        self.additionalLayer = specialCases.getAdditionalLayer (options.inDEV)
        self.specialCases = specialCases.cases
        self.specialLibraries = self.compileSpecialLibraries (specialCases.specialLibraries)
        try:
            self.sourceTranslation = specialCases.sourceTranslation
        except AttributeError:
            self.sourceTranslation = None
        # self.externalLibraries = specialCases.externalLibraries
        try:
            self.variantPerProject = specialCases.variantPerProject
        except AttributeError:
            self.variantPerProject = None
        self.defaultLibs = loadSpecialAttribute ('defaultLibs', [])

    def compileSpecialLibraries(self, specialLibraries):
        result = []
        for pattern, template in specialLibraries:
            result.append ((re.compile (pattern), template))
        return result


    def getTargetKinds (self, args):
        if self.options.vcproj:
            self.getVcProjTargetKinds (args)
        else:
            self.getDspTargetKinds (args)

    def getDspTargetKinds (self, args):
        if args:
            dspFiles = []
            for arg in args:
                ext = os.path.splitext (arg) [1]
                if ext == '.dsw':
                    parser = WorkspaceParser (arg)
                    for dsp, dependencies in parser.getDependencies ():
                        dspFiles.append (dsp)
                else:
                    dspFiles.append (os.path.abspath (arg))
        else:
            dspFiles = self.getAllDSP (self.dollarSrc)
        rex = DspFile.reTargetType
        self.knownLibs = {}
        for dsp in dspFiles:
            data = open (dsp, 'r').read ()
            match = rex.search (data)
            if match:
                ext = self.targetTypeMap [match.group (1)]
                most, dspExt = os.path.splitext (dsp)
                base = os.path.basename (most)
                relative = '::' + most [self.srcLen:] + ext
                relative = string.replace (relative,  '\\', '/')
                key = string.lower (base)
                self.knownLibs [key] = relative

    def getVcProjTargetKinds (self, args):
        projectFiles = self.getAllVcProj (self.dollarSrc)
        self.knownLibs = {}
        parser = ProjectTypeParser ()
        for projectFile in projectFiles:
            ext = parser.parse (projectFile)
            most, vcprojExt = os.path.splitext (projectFile)
            base = os.path.basename (most)
            relative = '::' + most [self.srcLen:] + ext
            relative = string.replace (relative,  '\\', '/')
            key = string.lower (base)
            self.knownLibs [key] = relative

    def translate (self, inDEV, dependencies = None):
        """translates all .dsp in inDEV to equivalent
            vmake descriptions in outSys/desc
        """
        if inDEV [-1] in "/\\":
            inDEV = inDEV [:-1]
        if self.options.vcproj:
            dspFiles = self.getAllVcProj (inDEV)
        else:
            dspFiles = self.getAllDSP (inDEV)
        if dependencies:
            for absolute in dspFiles:
                solutionDependencies = dependencies.getProjectDependencies (absolute)
                self.translateOneFile (absolute,
                    solutionDependencies = solutionDependencies)
        else:
            for absolute in dspFiles:
                self.translateOneFile (absolute)

    def getAllDSP (self, inDEV):
        """gets the names of all .dsp files in inDEV

        returns: a list containing both the names relative
            to inDEV and the absolute names.
        """
        result = []
        def filterDSP (list, dirname, names):
            for name in names:
                if name [-4:] == '.dsp':
                    fullpath = os.path.join (dirname, name)
                    list.append (fullpath)
        os.path.walk (inDEV, filterDSP, result)
        result = filter (self.isValidPattern, result)
        return result

    def getAllVcProj (self, inDEV):
        """gets the names of all .dsp files in inDEV

        returns: a list containing both the names relative
            to inDEV and the absolute names.
        """
        result = []
        def filterVcProj (list, dirname, names):
            for name in names:
                if name [-7:] == '.vcproj':
                    fullpath = os.path.join (dirname, name)
                    list.append (fullpath)
        os.path.walk (inDEV, filterVcProj, result)
        result = filter (self.isValidPattern, result)
        return result

    def translateWorkspace (self, workspaceFile):
        parser = WorkspaceParser (workspaceFile)
        for dsp, dependencies in parser.getDependencies ():
            self.translateOneFile (dsp, dependencies)

    def isValidPattern (self, filename):
        for pattern in self.options.ignorePatterns:
            if string.find (string.lower (filename), pattern) != -1:
                # trace ('ignoring %s because of %s', filename, pattern)
                return None
        return 1

    targetTypeMap = {
        'Console Application': '.lnk',
        'Application': '.lnk',
        'Dynamic-Link Library': '.dld',
        'Generic Project': '.mac',
        'Static Library': '.lib',
    }
    def translateOneFile (self, absolute, dependencies = [],
            solutionDependencies = []):
        dirname = os.path.dirname (absolute)
        #
        # parse input file
        #
        if self.options.verbose:
            trace (absolute)
        if self.options.vcproj:
            dspFile = VcProjFile (absolute, self.optionSampler,
                self.options.smartVariants, self.variantPerProject)
        else:
            try:
                dspFile = DspFile (absolute, self.optionSampler,
                    self.options.smartVariants, self.variantPerProject)
            except DspParseError, err:
                sys.stderr.write (str (err) + '\n')
                return
        #
        # create output file
        #
        if not dspFile.targetType:
            #if self.options.verbose:
            #    print 'skipping'
            return
        targetExtension = self.targetTypeMap [dspFile.targetType]
        relativeTarget = os.path.splitext (absolute [self.srcLen:]) [0] + targetExtension
        shortDescription = os.path.basename (relativeTarget)
        shortDescription = os.path.splitext (shortDescription) [0]
        #if targetExtension in ['.lib', '.dld']:
        #   shortAlias = string.lower (split3 (relativeTarget) [1])
        #   self.knownLibs [shortAlias] = '::' + string.replace (relativeTarget, '\\', '/')
        outputFile = os.path.join (self.dollarDesc, relativeTarget)
        stream = smartOpen (outputFile, 'w')
        stream.write ('# ' + string.replace (relativeTarget, '\\', '/') + '\n')
        stream.write ('# generated by dsp2vmake: ' + _version + '\n\n')
        #
        # write options
        #
        windowsLibs, binaryLibs, madeLibs, nodefaultlibs = self.getReferencedLibs (dspFile)
        if len (windowsLibs) + len (self.defaultLibs) > 0:
            stream.write ('&if $OS == WIN32\n')
            for libname in self.defaultLibs:
                stream.write ('-lib=' + libname + '\n')
            libsWritten = []
            for libname, variant in windowsLibs:
                # self.writeVariant (stream, '-lib=' + libname, variant)
                # make it a bit simpler, it's variant independent anyway
                if libname not in libsWritten:
                    self.writeVariant (stream, '-lib=' + libname, stdVariant)
                    libsWritten.append (libname)
            stream.write ('&endif\n')
        if len (binaryLibs) > 0:
            for replacement, variant in binaryLibs:
                self.writeVariant (stream, replacement, variant)
        if len (nodefaultlibs) > 0:
            stream.write ('&if $OS == WIN32\n')
            for libname, variant in nodefaultlibs:
                self.writeVariant (stream, '-nodefaultlib:' + libname, variant)
            stream.write ('&endif\n')
        if targetExtension == '.dld':
            stream.write ('\n? vmake option: no_static_lib\n')
        #
        # write targets
        #
        localFiles = StringIO ()
        hasLocalFiles = None
        localFiles.write ('\n? defaultlayer : ' +
            string.replace (dirname [self.srcLen:], '\\', '/') + '\n')
        otherFiles = StringIO ()
        lastDir = ''
        for source in dspFile.noCompileSources ():
            sourceFilename = self.translateFilename (source.name)
            if self.sourceTranslation:
                sourceFilename = self.sourceTranslation (sourceFilename, shortDescription)
            fullpath = os.path.normpath (os.path.join (dirname, sourceFilename))
            dirpart, basepart = self.splitpath (fullpath)
            self.layer.addEntry (dirpart, basepart, source, dspFile.filename)
            ext = extension (sourceFilename)
            if ext in _includeExtensions:
                continue
            if not allowFileByExtension (sourceFilename):
                continue
            source.removeIgnoredVariants (self.options.ignoreVariant)
            source.restrictToOnlyVariants (self.options.onlyVariant)
            emit, variants = self.getVariants (source)
            if not emit:
                continue
            addOn = self.moduleAddOn (ext)
            dirpart, basepart = os.path.split (fullpath)
            if dirpart == dirname:
                self.writeTarget (localFiles,
                    source.currentConfig.filenameVariant (basepart),
                    addOn, variants)
                hasLocalFiles = 1
            else:
                srcPath = string.replace (fullpath [self.srcLen:], '\\', '/')
                srcDir = os.path.dirname (srcPath)
                if srcDir != lastDir:
                    otherFiles.write ('\n')
                    lastDir = srcDir
                self.writeTarget (otherFiles,
                    ':' + source.currentConfig.filenameVariant (srcPath),
                    addOn, variants)
        if hasLocalFiles:
            stream.write (localFiles.getvalue ())
            stream.write ('\n')
        stream.write (otherFiles.getvalue ())
        if len (madeLibs) > 0:
            stream.write ('\n')
            for libname, variant in madeLibs:
                self.writeVariant (stream, self.findKnownLib (libname), variant)
        if dependencies:
            stream.write ('\n# from .dsw workspace file\n')
        for depfile in dependencies:
            relative = depfile [self.srcLen:]
            relative = string.replace (relative, '\\', '/')
            relative = os.path.splitext (relative) [0]
            stream.write ('::' + relative + '\n')
        if solutionDependencies:
            stream.write ('\n# from .sln solution file\n')
            for depfile in solutionDependencies:
                ext = os.path.splitext (depfile) [1]
                if ext not in ['.dld', '.lib']:
                    continue
                stream.write (depfile + '\n')
        self.addFileSpecifics (stream, relativeTarget)
        stream.close ()

    def getVariants (self, source):
        variants = []
        excludeCount = 0
        for config in source.getConfigs ():
            exclude = config.properties.get ('Exclude_From_Build')
            if exclude:
                excludeCount = excludeCount + 1
            variants.append ((config.globalVariant (), not exclude))
        if excludeCount == 0:
            result = (1, None)
        elif excludeCount == len (variants):
            result = (0, None)
        else:
            result = (1, variants)
        return result

    def findKnownLib (self, libname):
        key = os.path.basename (libname)
        key = string.lower (os.path.splitext (key) [0])
        knownLibs = self.knownLibs
        if knownLibs.has_key (key):
            return knownLibs [key]
        if key [-1] == 'd':
            key = key [:-1]   # try as debug lib
            if knownLibs.has_key (key):
                return knownLibs [key]
        return libname

    def translateFilename (self, sourceName):
        if os.sep != '\\':
            sourceName = string.replace (sourceName, '\\',  os.sep)
        for rex, replacement in self.nameTranslations:
            if rex.match (sourceName):
                return rex.sub (replacement, sourceName)
        return sourceName

    def writeTarget (self, stream, filename, addOn, variants):
        for rex, function in self.specialTargetHandling:
            if rex.match (filename):
                function (stream, filename, addOn, variants)
                break
        else:
            self.normalTargetWrite (stream, filename, addOn, variants)

    def normalTargetWrite (self, stream, filename, addOn, variants):
        line = filename + addOn + '\n'
        if not variants:
            stream.write (line)
        else:
            variants.sort ()
            variants.reverse ()
            variantsWritten = []
            intro = '&if '
            for variant, include in variants:
                variant = filter (None, variant)
                if variant in variantsWritten:
                    continue
                variantsWritten.append (variant)
                if variant:
                    list = map (lambda var: 'variant (' + var + ')', variant)
                    str = string.join (list, ' && ')
                    stream.write ('%s %s\n' % (intro, str))
                else:
                    stream.write ('&else\n')
                if include:
                    stream.write (line)
                else:
                    stream.write ('# Exclude_From_Build\n')
                intro = '&elif '
            stream.write ('&endif\n')

    def win32OnlyWrite (self, stream, filename, addOn, variants):
        stream.write ('&if $OS == WIN32\n')
        self.normalTargetWrite (stream,  filename,  addOn,  variants)
        stream.write ('&endif\n')

    def getReferencedLibs (self, dspFile):
        windowsLibs = []
        binaryLibs = []
        madeLibs = []
        nodefaultlibs = []
        moduleOptions = dspFile.moduleOptions
        for config in moduleOptions.configurations.values ():
            for option, value in config.linkOptions:
                if option == '-lib':
                    replacement = self.handleSpecialLibs (value)
                    if replacement:
                        targetList = binaryLibs
                        value = replacement
                    elif value [0] == '.':
                        targetList = madeLibs
                    else:
                        targetList = windowsLibs
                    if os.sep != '\\':
                        value = string.replace (value,  '\\',  os.sep)
                    entry = (value, config.variantNames)
                    if entry not in targetList:
                        targetList.append (entry)
                elif option == '/nodefaultlib':
                    nodefaultlibs.append ((value, config.variantNames))
        #windowsLibs.sort ()
        #binaryLibs.sort ()
        #madeLibs.sort ()
        return windowsLibs, binaryLibs, madeLibs, nodefaultlibs

    def handleSpecialLibs (self, libname):
        for rex, template in self.specialLibraries:
            match = rex.match (libname)
            if match:
                replacement = reExpand (template, rex, match)
                return replacement
        return None

    def writeVariant (self, stream, line, variantNames):
        if variantNames == stdVariant:
            stream.write (line + '\n')
        else:
            variantNames = filter (None, variantNames)
            list = map (lambda var: 'variant (' + var + ')', variantNames)
            str = string.join (list, ' && ')
            stream.write ('&if ' + str + '\n')
            stream.write (line + '\n')
            stream.write ('&endif\n')

    def moduleAddOn (self, extension):
        if extension == '.def':
            result = ' definition'
        elif extension == '.rc':
            result = ' remake'
        else:
            result = ''
        return result

    def splitpath (self, absolute):
        relative = absolute [self.srcLen:]
        return os.path.split (relative)

    def writeComDescriptions (self):
        layer = self.layer.items ()
        for path in self.additionalLayer:
            layer.append ((path, []))
        layer.sort ()
        for path, entries in layer:
            if self.options.unifiedSys:
                fname = os.path.join (path, os.path.basename (path) + '.com')
            else:
                fname = path + '.com'
            outfile = os.path.join (self.dollarDesc, fname)
            if self.options.verbose:
                trace (outfile)
            stream = smartOpen (outfile, 'w')
            stream.write ('# ' + string.replace (fname, '\\', '/') + '\n')
            stream.write ('# generated by dsp2vmake: ' + _version + '\n\n')
            for glob in self.options.globals:
                stream.write (glob + '\n')
            if len (entries) > 0:
                entries = self.detectConflictingComEntries (entries)
                for entry, configuredItem, dspFile in entries:
                    configuredItem.removeIgnoredVariants (self.options.ignoreVariant)
                # self.dumpDescriptionEntries (entries)
                starDescription = self.extractCommonOptions (entries, path)
                self.writeDescriptionVariants ('*', starDescription, None, stream)
                entries.sort ()
                for entry, configuredItem, dspFile in entries:
                    configuredItem.normalizeOptions ()
                for entry, configuredItem, dspFile in entries:
                    self.writeDescriptionVariants (entry, configuredItem, dspFile, stream)
            #
            # add platform specific rules
            #
            self.addFileSpecifics (stream, fname)
            self.addDirectoryRules (stream, path)
            stream.write (dependencySection)
            stream.close ()
            #
            # handle include dependencies
            #
            if imkdep and not self.options.nodep:
                args = ['-noinames', '-unifiedSys', '-forAPO', '-force', outfile]
                #if self.options.verbose:
                #    args.insert (0, '-v')
                rc = optlib.simulateMain (imkdep,  args)
                if rc:
                    self.returnCode = rc

    def addFileSpecifics (self, stream, relative):
        relative = string.replace (relative,  '\\',  '/')
        for case in self.specialCases:
            if relative [-len (case.filename):] == case.filename:
                case.apply (stream, self.options)
        for src in srcPath:
            fullpath = os.path.join (src, relative + '.plus')
            if os.path.exists (fullpath):
                stream.write ("\n# included from %s.plus\n" % relative)
                data = open (fullpath, 'r').read ()
                stream.write (data)
                stream.write ("\n# end include\n")
                break
        #
        # search for matching pattern in specialCases
        #
        addOn =  findFileAddOn (relative)
        if addOn:
            stream.write ('\n# from specialCases.fileAddOn\n')
            stream.write (addOn)
            stream.write ('\n')

    def addDirectoryRules (self, stream, path):
        adder = loadSpecialAttribute ('comAdditions')
        if adder is None:
            return
        stream.write ('# addDirectoryRules for %s\n' % path)
        dir = string.replace (path, '\\', '/')
        superdir = os.path.basename (dir)
        fullpath = os.path.join (self.options.inDEV, path)
        if not os.path.isdir (fullpath):
            if self.options.strict:
                raise 'Directory does not exist', path
            else:
                trace ('Directory does not exist: %s',  path)
                stream.write ('# end addDirectoryRules for %s\n' % path)
                return
        files = os.listdir (fullpath)
        for filename in files:
            basename = os.path.splitext (filename) [0]
            addition = adder (filename, locals ())
            if addition:
                stream.write (addition)
        stream.write ('# end addDirectoryRules for %s\n' % path)


    def dumpDescriptionEntries (self, entries):
        for entry, configuredItem, projectFile in entries:
            trace ('=== %s', configuredItem.name)
            for config in configuredItem.configurations.values ():
                trace ('   %s %s', config.variantNames, config.options)

    def detectConflictingComEntries (self, entries):
        entries.sort ()
        result = []
        lastEntry, lastItem, lastDsp = entries [0]
        result.append ((lastEntry, lastItem, lastDsp))
        for entry, configuredItem, dspFile in entries [1:]:
            if entry == lastEntry:
                for config in configuredItem.getConfigs ():
                    last = lastItem.configurations.get (config.variantNames)
                    if not last:
                        # variant not in last entry, add to list
                        lastItem.configurations [config.variantNames] = config
                    else:
                        remLast, common, remConfig = setSplit (last.options, config.options)
                        if remLast or remConfig:
                            if self.options.warn:
                                trace ('Conflict: %s variant (%s) common: %s',  entry, config.variantNames, common)
                                trace ('--- %s: %s',  lastDsp, remLast)
                                trace ('--- %s: %s', dspFile, remConfig)
            else:
                lastEntry, lastItem, lastDsp = lastTuple = entry, configuredItem, dspFile
                result.append (lastTuple)
        return result

    def extractCommonOptions (self, entries, path):
        allVariantNames = {}
        for entry, configuredItem, dspFile in entries:
            for variantName in configuredItem.getVariantNames ():
                allVariantNames [variantName] = 1
        starDescription = ConfiguredItem ('*')
        del starDescription.configurations [stdVariant]
        allVariantNames = allVariantNames.keys ()
        allVariantNames.sort ()
        if path in ['atp\\atplib', 'atp/atplib']:
            allVariantNames.reverse ()
        for variantName in allVariantNames:
            variantConfigs = self.filterVariantOptions (entries, variantName)
            commons = variantConfigs.extractCommonOptions (variantName)
            for entry in commons:
                starDescription.configurations [entry.variantNames] = entry
        if not starDescription.configurations.has_key (stdVariant):
            # make sure that an empty common definition gets written
            stdConfig = Configuration (stdVariant)
            stdConfig.isLocked = 1
            starDescription.configurations [stdVariant] = stdConfig
        starDescription.normalizeOptions ()
        return starDescription

    def filterVariantOptions (self, entries, variantName):
        list = []
        for entry, configuredItem, dspFile in entries:
            list.extend (configuredItem.filter (variantName, true))
        result = VariantSet (list)
        return result


    def dumpEntries (self, entries):
        for entry, configuredItem, dspFile in entries:
            trace (entry)
            configuredItem.dump ()

    def findOptionCandidates (self, entries, configId):
        for entry, configuredItem, dspFile in entries:
            try:
                config = configuredItem.configurations [configId]
                return config.options [:]
            except KeyError:
                pass

    def isCommonOption (self, entries, configId, optdef):
        result = true
        for entry, configuredItem, dspFile in entries:
            try:
                config = configuredItem.configurations [configId]
                if optdef not in config.options:
                    return false
            except KeyError:
                pass
        return result

    def removeCommonOption (self, entries, configId, optdef):
        result = true
        for entry, configuredItem, dspFile in entries:
            try:
                config = configuredItem.configurations [configId]
                config.options.remove (optdef)
            except KeyError:
                pass
        return result

    def writeDescriptionVariants (self, entry, configuredItem, dspFile, stream):
        if dspFile and self.options.debug:
            stream.write ('# from ' + dspFile + '\n')
        count = 0
        for config in configuredItem.extraConfigs ():
            line = self.sourceDescription (entry, config)
            if line:
                if string.find (line, '-WIN32-only-') == -1:
                    stream.write (line + '\n')
                else:
                    stream.write ('&if $OSSPEC == WIN32 && ! defined BIT64\n')
                    win32line = string.replace (line, '-WIN32-only-', '-')
                    stream.write (win32line + '\n')
                    stream.write ('&else\n')
                    otherline = re.sub ('-WIN32-only-([0-9a-zA-Z]+)', '', line)
                    stream.write (otherline + '\n')
                    stream.write ('&endif\n')
                count = count + 1
        if (count == 0) and self.options.debug:
            stream.write ('# %s\n' % entry)

    def sourceDescription (self, fname, configuration, unconditional = false):
        #if not configuration.options and not unconditional \
        #        and configuration.variantNames != stdVariant:
        #    return '# ' + fname + ' ' + `configuration.variantNames` + '\n'
        #    return None
        if not configuration.options:
            return None
        result = fname
        varstring = string.join (filter (None,  configuration.variantNames),  ',')
        if varstring:
            result = result + ':variant(' + varstring + ')'
        for option, value in configuration.options:
            result = result + ' -' + option [1:]
            if value:
                result = result + unquote (value)
        return result


class DspFile:
    """Parser for MS Dev project files
    """
    allowSource = true
    optionsToIgnore = specialCases.optionsToIgnore
    shortOptionsToIgnore = specialCases.shortOptionsToIgnore
    definesToIgnore = specialCases.definesToIgnore
    try:
        shortWindows32Only = specialCases.shortWindows32Only
    except AttributeError:
        shortWindows32Only = []

    def __init__ (self, fname, optionSampler, smartVariants = None,
            variantPerProject = None):
        self.filename = fname
        self.targetType = None
        self.moduleOptions = ConfiguredItem ('')
        self.currentSource = self.moduleOptions
        self.sources = []
        matchList = []
        for kind in ['TargetType', 'Source', 'EndSource', 'Add', 'Prop',
                'If', 'EndIf', 'SpecialBuild', 'EndSpecialBuild']:
            matchList.append ((getattr (self, 're' + kind), getattr (self, 'match' + kind)))
        self.matcher = RegexEvents (matchList)
        self.optionSampler = optionSampler
        self.smartVariants = smartVariants
        if variantPerProject:
            variantName = os.path.basename (fname)
            self.variantName = os.path.splitext (variantName) [0]
        else:
            self.variantName = None
        self.inConditionalConfig = None
        #
        self.parseFile (fname)
        self.sources = filter (lambda source: not source.isExcludedFromBuild (),
            self.sources)
        if self.variantName:
            for source in self.sources:
                source.addProjectVariant (self.variantName)

    def noCompileSources (self):
        result = []
        for source in self.sources:
            appendSource = true
            filetype = os.path.splitext (source.name) [1]
            if filetype in ['.h', '.hpp']:
                appendSource = false
            elif source.name == '$(InputPath)':
                appendSource = false
            if appendSource:
                result.append (source)
        return result


    def parseFile (self, fname):
        lines = open (fname, 'rt').readlines ()
        self.matcher.parseLines (lines)
        for item in self.sources:
            item.mergeCommonEntries (self.moduleOptions)

    reTargetType = re.compile ('# TARGTYPE "Win32 [(]x86[)] ([^"]+)" 0x([0-9]+)')
    def matchTargetType (self, match):
        self.targetType = match.group (1)

    reSource = re.compile ('SOURCE=(.+)')
    def matchSource (self, match):
        if self.allowSource:
            filename = match.group (1)
            newItem = ConfiguredItem (filename)
            if allowFileByExtension (filename):
                self.sources.append (newItem)
            self.currentSource = newItem
            #if self.variantName:
            #    self.currentSource.switchToCopy (self.variantName)
            #    self.currentSource.addProperty ('VMAKEVARIANT', (self.variantName,))
            #    self.currentSource.addOption ('/D', 'VMAKEVARIANT_' + self.variantName)


    reEndSource = re.compile ('# End Source File')
    def matchEndSource (self, match):
        self.currentSource = self.moduleOptions

    reSpecialBuild = re.compile ('# Begin Special Build Tool')
    def matchSpecialBuild (self, match):
        self.allowSource = false

    reEndSpecialBuild = re.compile ('# End Special Build Tool')
    def matchEndSpecialBuild (self, match):
        self.allowSource = true

    reAdd = re.compile ('# ADD (.*)')
    def matchAdd (self, match):
        line = match.group (1)
        line = string.replace (line, 'Program Files', 'Program_Files')
        line = string.replace (line, 'Microsoft Visual Studio', 'Microsoft_Visual_Studio')
        words = string.split (line)
        if words [0] == 'BASE':
            return   # BASE options are obsolete
            del words [0]
        tool = words [0]
        del words [0]
        parse = {'CPP': self.parseCPPOptions, 'LINK32': self.parseLINK32Options}\
            .get (tool, self.parseAnyOptions)
        for option, value in parse (words):
            self.optionSampler.addOption (tool, option, value)
            if tool == 'CPP':
                self.currentSource.addOption (option, value)
            elif tool == 'LINK32':
                self.currentSource.addLinkOption (option, value)

    variantRE = re.compile ('"?VMAKEVARIANT_([a-zA-Z0-9_]+)"?')
    def parseCPPOptions (self, words):
        result = []
        if words [0] == 'BASE':
            return   # BASE options are obsolete
            index = 1
        else:
            index = 0
        words.append ('/') # makes looking for value easier
        while index < len (words) - 1:
            option = words [index]
            value = words [index + 1]
            if value [0] == '/':
                value = None
            else:
                index = index + 1
            storeValue = None
            if option == '/GR':
                option = '-rtti'
                storeValue = true
            elif option [:2] in self.shortWindows32Only:
                option = '-WIN32-only-' + option [1:]
                storeValue = true
            elif (option in self.optionsToIgnore
                    or option [:2] in self.shortOptionsToIgnore):
                # common options are set through toolvars.pl or variants
                pass
            elif option [:2] == '/M':
                option = '-Mx=' + option
                storeValue = true
            elif option [:2] == '/I':
                if value [:2] == '".':
                    # relative paths are ignored, replaced be /I %WRK%/incl
                    pass
                elif value == '"$(LC_INC)"':
                    # don't know how to make this common
                    pass
                elif string.find (value, 'VC98') != -1:
                    # this one isn't necessary anyway
                    pass
                else:
                    storeValue = true
            elif option [:2] == '/D':
                if len (option) > 2:
                    value = option [2:]
                    option = '/D'
                if value in self.definesToIgnore:
                    pass
                else:
                    storeValue = true
                    if value:
                        match = self.variantRE.match (value)
                        if match:
                            if self.variantName:
                                storeValue = false
                            else:
                                variant = match.group (1)
                                self.currentSource.switchToCopy (variant)
                                self.currentSource.addProperty ('VMAKEVARIANT', (variant,))
                        elif string.find (value, 'LATEST_CHANGE') != -1:
                            value = 'LATEST_CHANGE=$LATEST_CHANGE'
                            # much too common
                            storeValue = false
                        elif string.find (value, 'BRANCH_INFO') != -1:
                            value = 'BRANCH_INFO=$BRANCH_INFO'
            elif option [:2] == '/Y':
                # currently no support for precompiled header
                pass
            else:
                storeValue = true
            if storeValue:
                result.append ((option, value))
            index = index + 1
        return result

    def parseLINK32Options (self, words):
        result = []
        for word in words:
            if word [0] == '/':
                colonPos = string.find (word, ':')
                if colonPos == -1:
                    option = word
                    value = None
                else:
                    option = word [:colonPos]
                    value = unquote (word [colonPos + 1:])
            else:
                option = '-lib'
                value = word
            result.append ((option, value))
        return result


    def parseAnyOptions (self, words):
        result = []
        index = 1
        words.append ('/') # makes looking for value easier
        while index < len (words) - 1:
            option = words [index]
            value = words [index + 1]
            if value [0] == '/':
                value = None
            else:
                index = index + 1
            index = index + 1
            result.append ((option, value))
        return result

    reProp = re.compile ('# PROP (.*)')
    def matchProp (self, match):
        words = string.split (match.group (1))
        if words [0] == 'BASE':
            return   # BASE options are obsolete
            del words [0]
        assert len (words) == 2, 'more than 2 words in # PROP ' + `words`
        self.currentSource.addProperty (words [0], words [1])

    reIf = re.compile ('!(ELSE)?IF  "[$][(]CFG[)]" == "([^"]+)"')
    def matchIf (self, match):
        configName = match.group (2)
        shortName = string.split (configName) [-1]
        configId = (shortName,)
        if self.smartVariants:
            for special in ['Release', 'Debug']:
                splen = len (special)
                if shortName [:splen] == special:
                    part1 = shortName [:splen]
                    part2 = shortName [splen:]
                    configId = (part1, part2)
                    break
        self.currentSource.setCurrentConfiguration (configId)
        self.inConditionalConfig = 1

    reEndIf = re.compile ('!ENDIF')
    def matchEndIf (self, match):
        if self.inConditionalConfig:
            self.currentSource.setStdConfiguration ()
            self.inConditionalConfig = None

class DspParseError:
    def __init__ (self, msg, fname):
        self.msg = msg
        self.fname = fname

    def __str__ (self):
        return '.dsp Parse error: ' + self.msg + ' in ' + self.fname

class RegexEvents:
    def __init__ (self, reList):
        self.reList = reList

    def parseLines (self, lines):
        reList = self.reList
        for line in lines:
            for rex, action in reList:
                match = rex.match (line)
                if match:
                    action (match)

    def parseFile (self, fname):
        self.parseLines (open (fname, 'rt').readlines ())


class VcProjFile (xmllib.XMLParser):
    """Parser for new style MS Dev project files
    """
    allowSource = true
    optionsToIgnore = specialCases.optionsToIgnore
    shortOptionsToIgnore = specialCases.shortOptionsToIgnore
    definesToIgnore = map (stripQuotes, specialCases.definesToIgnore) + ['$(NoInherit)', '$(NOINHERIT)']

    def __init__ (self, fname, optionSampler, smartVariants = None,
            variantPerProject = None):
        xmllib.XMLParser.__init__ (self)
        self.filename = fname
        self.targetType = 'Generic Project'
        self.moduleOptions = ConfiguredItem ('')
        self.currentSource = self.moduleOptions
        self.sources = []
        self.optionSampler = optionSampler
        self.smartVariants = smartVariants
        if variantPerProject:
            variantName = os.path.basename (fname)
            self.variantName = os.path.splitext (variantName) [0]
        else:
            self.variantName = None
        self.inConditionalConfig = None
        #
        self.parseFile (fname)
        self.sources = filter (lambda source: not source.isExcludedFromBuild (),
            self.sources)
        if self.variantName:
            for source in self.sources:
                source.addProjectVariant (self.variantName)
        #for source inx self.sources:
        #    source.dump ()
    def noCompileSources (self):
        result = []
        for source in self.sources:
            appendSource = true
            filetype = os.path.splitext (source.name) [1]
            if filetype in ['.h', '.hpp']:
                appendSource = false
            elif source.name == '$(InputPath)':
                appendSource = false
            if appendSource:
                result.append (source)
        return result


    def parseFile (self, fname):
        data = open (fname, 'r').read ()
        self.feed (data)
        for item in self.sources:
            item.mergeCommonEntries (self.moduleOptions)

    def start_Tool (self, attributes):
        toolName = attributes ['Name']
        outputFile = attributes.get ('OutputFile')
        if toolName == 'VCLibrarianTool':
            if outputFile:
                self.targetType = 'Static Library'
        elif toolName == 'VCLinkerTool':
            if outputFile:
                targetExt = os.path.splitext (outputFile) [1]
                if targetExt == '.dll':
                    self.targetType = 'Dynamic-Link Library'
                elif targetExt == '.exe':
                    self.targetType = 'Console Application'
            for lib in splitAttribute (attributes, 'AdditionalDependencies', ' '):
                self.currentSource.addLinkOption ('-lib', lib)
        elif toolName == 'VCCLCompilerTool':
            self.addPreprocessorDefinitions (attributes.get ('PreprocessorDefinitions'))
            for includePath in splitAttribute (attributes, 'AdditionalIncludeDirectories', ',;', ['$(NoInherit)']):
                if (includePath [0] != '.') and (includePath != '$(LC_INC)'):
                    self.currentSource.addOption ('-I', includePath)
            if attributes.get ('RuntimeTypeInfo', 'NO') == 'YES':
                self.currentSource.addOption ('-rtti')
            self.translateRuntimeLibrary (attributes)
            self.translateOptimizerOptions (attributes)

    runtimeLibraryMapping = {
        "0": 'MT',
        "1": 'MTd',
        "2": 'MD',
        "3": 'MDd',
        "4": 'ML',
        "5": 'MLd',
        }
    def translateRuntimeLibrary (self, attributes):
        libcode = attributes.get ('RuntimeLibrary')
        if libcode is None:
            return
        mapped = self.runtimeLibraryMapping.get (libcode)
        if mapped is None:
            raise "Don't know how to translate RuntimeLibrary=" + libcode + " to command line"
        self.currentSource.addOption ('-Mx=-' + mapped)

    def translateOptimizerOptions (self, attributes):
        winO = 'win32-only-O'
        winG = 'win32-only-G'
        self.translateOption (attributes, 'Optimization', winO,
            {'0': 'd', '1': '1', '2': '2', '3': 'x', '4': None})
        self.translateOption (attributes, 'GlobalOptimizations', winO,
            {'TRUE': 'g', 'FALSE': None})
        self.translateOption (attributes, 'InlineFunctionExpansion', winO,
            {'1': 'b1', '2': 'b2', '0': None})
        self.translateOption (attributes, 'EnableIntrinsicFunctions', winO,
            {'TRUE': 'i', 'FALSE': None})
        self.translateOption (attributes, 'ImproveFloatingPointConsistency', winO,
            {'TRUE': 'p', 'FALSE': None})
        self.translateOption (attributes, 'FavorSizeOrSpeed', winO,
            {'0': None, '1': 't', '2': 's'})
        self.translateOption (attributes, 'OmitFramePointers', winO,
            {'TRUE': 'y', 'FALSE': None})
        self.translateOption (attributes, 'EnableFiberSafeOptimizations', winG,
            {'TRUE': 'T', 'FALSE': None})
        self.translateOption (attributes, 'OptimizeForProcessor', winG,
            {'0': None, '1': '5', '2': '6'})
        self.translateOption (attributes, 'OptimizeForWindowsApplication', winG,
            {'TRUE': 'G', 'FALSE': None})

    def translateOption (self, attributes, name, clOption, mapping):
        value = attributes.get (name)
        if value is None:
            return
        try:
            mapped = mapping [value]
        except KeyError:
            raise 'Cannot translate option', (name, value)
        if mapped is None:
            return
        return '-' + clOption + mapped

    def start_File (self, attributes):
        if self.allowSource:
            filename = attributes ['RelativePath']
            newItem = ConfiguredItem (filename)
            if allowFileByExtension (filename):
                self.sources.append (newItem)
            self.currentSource = newItem

    def end_File (self):
        self.currentSource = self.moduleOptions

    variantRE = re.compile ('"?VMAKEVARIANT_([a-zA-Z0-9_]+)"?')
    def addPreprocessorDefinitions (self, asString):
        if not asString:
            return
        asList = string.split (asString, ';')
        if (len (asList) == 1) and (string.find (asString, ',') != -1):
            asList = string.split (asString, ',')
        for value in asList:
            if value in self.definesToIgnore:
                continue
            storeValue = true
            if value:
                match = self.variantRE.match (value)
                if match:
                    if self.variantName:
                        storeValue = false
                    else:
                        variant = match.group (1)
                        self.currentSource.switchToCopy (variant)
                        self.currentSource.addProperty ('VMAKEVARIANT', (variant,))
                elif string.find (value, 'LATEST_CHANGE') != -1:
                    value = 'LATEST_CHANGE=$LATEST_CHANGE'
                    # much too common
                    storeValue = false
                elif string.find (value, 'BRANCH_INFO') != -1:
                    value = 'BRANCH_INFO=$BRANCH_INFO'
            if storeValue:
                self.currentSource.addOption ('/D', value)

    def start_Configuration (self, attributes):
        shortName = string.split (attributes ['Name'], '|') [0]
        configId = (shortName,)
        if self.smartVariants:
            for special in ['Release', 'Debug']:
                splen = len (special)
                if shortName [:splen] == special:
                    part1 = shortName [:splen]
                    part2 = shortName [splen:]
                    configId = (part1, part2)
                    break
        self.currentSource.setCurrentConfiguration (configId)
        self.inConditionalConfig = 1

    def end_Configuration (self):
        if self.inConditionalConfig:
            self.currentSource.setStdConfiguration ()
            self.inConditionalConfig = None

    def start_FileConfiguration (self, attributes):
        self.start_Configuration (attributes)
        if attributes.get ("ExcludedFromBuild") == 'TRUE':
            self.currentSource.addProperty ('Exclude_From_Build', 1)

    def end_FileConfiguration (self):
        self.end_Configuration ()


stdVariant = ('',)

class ConfiguredItem:
    def __init__ (self, name):
        if name:
            name = unquote (name)
        self.name = name
        self.configurations = {}
        self.setStdConfiguration ()

    def __str__ (self):
        return ('[' + self.name + ' at '
            + `self.currentConfig.variantNames` + ' '
            + `self.configurations.keys ()` + ']')


    def setCurrentConfiguration (self, configId, extended = false):
        try:
            self.currentConfig = self.configurations [configId]
        except KeyError:
            self.currentConfig = Configuration (configId, extended)
            configId = self.currentConfig.variantNames
            self.configurations [configId] = self.currentConfig
        return self.currentConfig

    def setStdConfiguration (self):
        self.setCurrentConfiguration (stdVariant)

    def switchToCopy (self, variantName):
        assert self.currentConfig.variantNames == stdVariant, 'Can\'t handle nested variants yet'
        # variantNames = (variantName,) + self.currentConfig.variantNames
        self.setCurrentConfiguration ((variantName,), true)

    def addOption (self, option, value = None):
        optdef = (option, value)
        currentOptions = self.currentConfig.options
        if optdef not in currentOptions:
            currentOptions.append (optdef)

    def addLinkOption (self, option, value = None):
        optdef = (option, value)
        currentOptions = self.currentConfig.linkOptions
        if optdef not in currentOptions:
            currentOptions.append (optdef)

    def addProperty (self, name, value):
        self.currentConfig.properties [name] = value

    def extraConfigs (self):
        result = []
        for config in self.configurations.values ():
            if config.variantNames != '':
                result.append (config)
        return result

    def getConfigs (self):
        return self.configurations.values ()

    def getVariantNames (self):
        result = []
        for config in self.configurations.values ():
            result.extend (list (config.variantNames))
        return result

    def normalizeOptions (self):
        return
        self.clearEmptyConfigs ()
        set = VariantSet (self.configurations.values ())
        commons = set.extractCommonOptions ()
        self.clearEmptyConfigs ()
        for entry in commons:
            self.configurations [entry.variantNames] = entry

    def clearEmptyConfigs (self):
        for id, config in self.configurations.items ():
            if not config.options and not config.isLocked:
                del self.configurations [id]

    def mergeCommonEntries (self, otherConfiguredItem):
        defaultConfig = otherConfiguredItem.configurations [stdVariant]
        isEmpty = self.configurations.keys () == [stdVariant]
        #
        # merge global variants into self
        #
        for config in otherConfiguredItem.configurations.values ():
            try:
                localConfig = self.configurations [config.variantNames]
                self.mergeOneVariant (localConfig,  config)
            except KeyError:
                if isEmpty:
                    self.configurations [config.variantNames] = config.copy ()
        #
        # merge global variants into local variants
        #
        if not isEmpty and (len (self.configurations.keys ()) == 2):
            self.clearEmptyConfigs ()
            values = self.configurations.values ()
            if len (values) > 0:
                variantConfig = values [0]
                if '' not in variantConfig.variantNames:
                    # append global variants to variant configs
                    for config in otherConfiguredItem.configurations.values ():
                        if config.variantNames == stdVariant:
                            continue
                        oldNames = variantConfig.variantNames
                        newNames = oldNames + config.variantNames
                        newConfig = self.setCurrentConfiguration (newNames)
                        newConfig.options = variantConfig.options + config.options
                        newConfig.properties.update (variantConfig.properties)
                        newConfig.properties.update (config.properties)
                        try:
                            del self.configurations [oldNames]
                        except KeyError:
                            pass
        #
        # merge defaults into local-only variants
        #
        for localConfig in self.configurations.values ():
            if otherConfiguredItem.configurations.has_key (localConfig.variantNames):
                pass # already handled by code above
            else:
                if '' not in localConfig.variantNames:
                    # extended variant, handled in next loop
                    continue
                self.mergeOneVariant (localConfig,  defaultConfig)
        #
        # merge local defaults into local variants
        #
        if self.configurations.has_key (stdVariant):
            localDefaults = self.configurations [stdVariant]
            for localConfig in self.configurations.values ():
                if localConfig.variantNames != stdVariant:
                    self.mergeOneVariant (localConfig,  localDefaults)
        #
        #
        #
        self.clearEmptyConfigs ()

    def mergeOneVariant (self, localConfig, config):
        for optdef in config.options:
            if optdef not in localConfig.options:
                localConfig.options.insert (0, optdef)
        for name, value in config.properties.items ():
            if not localConfig.properties.has_key (name):
                localConfig.properties [name] = value

    def removeDuplicateOptions (self):
        for config in self.configurations.values ():
            list = []
            for optdef in config.options:
                if optdef not in list:
                    list.append (optdef)
            config.options = list

    def extractCommonEntries (self):
        commonConfig = self.configurations [stdVariant]
        otherConfigs = self.extraConfigs ()
        if len (otherConfigs) == 0:
            return
        #
        # move common options to commonConfig
        #
        candidates = otherConfigs [0].options [:]
        for optdef in candidates:
            if self.isCommonOption (optdef):
                commonConfig.options.append (optdef)
                for config in otherConfigs:
                    config.options.remove (optdef)
        #
        # move common properties to commonConfig
        #
        for name, value in otherConfigs [0].properties.items ():
            if self.isCommonProperty (name, value):
                commonConfig.properties [name] = value
                for config in otherConfigs:
                    del config.properties [name]

    def isCommonProperty (self, name, value):
        result = true
        for config in self.configurations.values ():
            if config.variantNames == '':
                continue
            if config.properties.get (name) != value:
                return false
        return result

    def filter (self, variantName, returnList = false):
        return VariantSet (self.configurations.values ()).filter (variantName,  returnList)

    def removeIgnoredVariants (self, ignoreList):
        for variantNames in self.configurations.keys ():
            for name in ignoreList:
                if name in variantNames:
                    del self.configurations [variantNames]
                    continue

    def restrictToOnlyVariants (self, onlyList):
        if not onlyList:
            # empty list => generate all
            return
        for variantNames in self.configurations.keys ():
            for name in variantNames:
                if name and (name not in onlyList):
                    del self.configurations [variantNames]
                    break

    def isExcludedFromBuild (self):
        possiblyExcluded = None
        for name, config in self.configurations.items ():
            if name == stdVariant:
                pass
            elif not config.properties.get ('Exclude_From_Build'):
                return None
            else:
                possiblyExcluded = 1
        return possiblyExcluded

    def dump (self, comment = '', filter = []):
        if filter:
            found = None
            for entry in filter:
                if string.find (self.name, entry) != -1:
                    found = 1
                    break
            if not found:
                return
        trace ('=== %s %s', self.name, comment)
        items = self.configurations.items ()
        items.sort ()
        for name, config in items:
            trace ('   %s %s %s', config.variantNames, config.options, sortedDict (config.properties))

    def addProjectVariant (self, variantName):
        newConfigDict = {}
        for configId, configuration in self.configurations.items ():
            newId = (variantName,) + configId
            configuration.addProjectVariant (variantName)
            newConfigDict [newId] = configuration
        self.configurations = newConfigDict
        self.currentConfig.addProjectVariant (variantName)

class Configuration:
    isLocked = None
    def __init__ (self, variantNames, extended = false):
        assert type (variantNames) == type (())
        if ('' not in variantNames) and not extended:
            variantNames = variantNames + ('',)
        self.variantNames = variantNames
        self.options = []
        self.properties = {}
        self.linkOptions = []

    def __repr__ (self):
        return '<Configuration %s\n\t%s>' % (
            self.variantNames, self.options)

    def copy (self):
        result = Configuration (self.variantNames)
        result.options = self.options [:]
        result.properties = self.properties.copy ()
        return result

    def filenameVariant (self, filename):
        if self.properties.has_key ('VMAKEVARIANT'):
            realVariants = filter (None, self.properties ['VMAKEVARIANT'])
            if len (realVariants) > 0:
                name, ext = os.path.splitext (filename)
                result = name + '-' + string.join (realVariants,  '-') + ext
            else:
                result = filename
        else:
            result = filename
        return result

    def addProjectVariant (self, variantName):
        self.properties ['VMAKEVARIANT'] = (variantName,)
        if variantName not in self.variantNames:
            self.variantNames = (variantName,) + self.variantNames

    def globalVariant (self):
        if self.properties.has_key ('VMAKEVARIANT'):
            projectVariant = self.properties ['VMAKEVARIANT'] [0]
            index = list (self.variantNames).index (projectVariant)
            result = self.variantNames [:index] + self.variantNames [index + 1:]
        else:
            result = self.variantNames
        return result

class VariantSet:
    def __init__ (self, list):
        # data is list of Configuration
        self.list = list

    def dump (self, comment = ''):
        trace ('=== VariantSet %s', comment)
        for entry in self.list:
            trace ('   %s', entry)

    def __getitem__ (self, index):
        return self.list [index]

    def getAllNames (self):
        set = {}
        for entry in self.list:
            for name in entry.variantNames:
                set [name] = 1
        result = set.keys ()
        result.sort ()
        return result

    def filter (self, variantName, returnList = false):
        list = []
        for entry in self.list:
            #if not entry.options:
            #    # empty option lists are ignored
            #    continue
            if variantName in entry.variantNames:
                list.append (entry)
        if returnList:
            result = list
        else:
            result = VariantSet (list)
        return result

    #
    # extract common option for one variant
    #
    def getAvailableOptions (self):
        result = []
        for entry in self.list:
            for optdef in entry.options:
                if optdef not in result:
                    result.append (optdef)
        return result

    def isCommonOption (self, optdef):
        for entry in self.list:
            if optdef not in entry.options:
                return false
        return true

    def getCommonOptions (self):
        options = self.getAvailableOptions ()
        result = filter (self.isCommonOption,  options)
        return result

    def removeOption (self, optdef):
        for entry in self.list:
            entry.options.remove (optdef)

    def removeOptions (self, options):
        for optdef in options:
            self.removeOption (optdef)

    #
    # extract common options for all variants
    #
    def extractCommonOptions (self, *allNames):
        result = []
        if not allNames:
            allNames = self.getAllNames ()
        for name in allNames:
            subset = self.filter (name)
            commonOptions = subset.getCommonOptions ()
            if commonOptions:
                newConfig = Configuration ((name,))
                newConfig.options = commonOptions
                result.append (newConfig)
            subset.removeOptions (commonOptions)
        return result

class LayerDict (UserDict):
    def addEntry (self, path, basename, entry, dspFile):
        try:
            list = self [path]
        except KeyError:
            list = []
            self[path] = list
        list.append ((basename, entry, dspFile))

def sortedItems (dict):
    """return a list of key-value tuples, sorted by key
    """
    items = dict.items ()
    items.sort ()
    return items

class OptionSampler:
    def __init__ (self):
        self.dict = {}

    def addOption (self, tool, option, value):
        try:
            toolOptions = self.dict [tool]
        except:
            toolOptions = {}
            self.dict [tool] = toolOptions
        try:
            optionValues = toolOptions [option]
        except:
            optionValues = {}
            toolOptions [option] = optionValues
        if value:
            optionValues [value] = 1

    def dump (self):
        for tool, options in sortedItems (self.dict):
            trace (tool)
            for option, values in sortedItems (option):
                trace ('    %s', options)
                for value, dummy in sortedItems (values):
                    trace ('        %s', value)

class WorkspaceParser:
    def __init__ (self, filename):
        self.short2long = {}
        self.dependencies = {}
        matchList = []
        for kind in ['Project', 'Depname']:
            matchList.append ((getattr (self, 're' + kind), getattr (self, 'match' + kind)))
        self.matcher = RegexEvents (matchList)
        self.basedir = os.path.dirname (os.path.abspath (filename))
        self.parseFile (filename)

    def parseFile (self, filename):
        self.matcher.parseFile (filename)

    reProject = re.compile ('Project: "([^"]+)"=([^ ]+) - Package Owner=<4>')
    def matchProject (self, match):
        short, long = match.group (1, 2)
        long = os.path.normpath (os.path.join (self.basedir, long))
        self.short2long [short] = long
        self.currentDependencies = []
        self.dependencies [long] = self.currentDependencies

    reDepname = re.compile (r'\s*Project_Dep_Name (.*)')
    def matchDepname (self, match):
        self.currentDependencies.append (match.group (1))

    def getDependencies (self):
        result = []
        items = self.dependencies.items ()
        items.sort ()
        for dsp, dependencies in items:
            absDep = map (self.transShort, dependencies)
            result.append ((dsp, absDep))
        return result

    def transShort (self, name):
        return self.short2long [name]

class ProjectTypeParser (xmllib.XMLParser):
    def parse (self, fname):
        self.reset ()
        data = open (fname, 'r').read ()
        try:
            self.feed (data)
        except 'FoundProjectKind', ext:
            return ext
        return '.mac'

    def start_Tool (self, attributes):
        toolName = attributes ['Name']
        outputFile = attributes.get ('OutputFile')
        if toolName == 'VCLibrarianTool':
            if outputFile:
                raise 'FoundProjectKind', '.lib'
        elif toolName == 'VCLinkerTool':
            targetExt = outputFile [-4:]
            if targetExt == '.dll':
                raise 'FoundProjectKind', '.dld'
            elif targetExt == '.exe':
                raise 'FoundProjectKind', '.lnk'

    def end_Configuration (self):
        raise 'FoundProjectKind', '.mac'

class Solution:
    def __init__ (self, name, path, guid):
        self.name = name
        self.path = path
        self.guid = guid
        self.dependencies = []

    def addDependency (self, project):
        self.dependencies.append (project)

    def __str__ (self):
        return "[Project %s]" % self.name


class SolutionParser7:
    def __init__ (self, fname, targetMapping):
        self.parse (fname, targetMapping)

    def parse (self, fname, targetMapping):
        stream = open (fname, 'r')
        self.readProjects (stream, os.path.dirname (fname))
        self.readDependencies (stream, targetMapping)
        stream.close ()

    def readProjects (self, stream, basedir):
        self.projects = []
        self.guidMapping = {}
        rex= re.compile ('Project[(]"[^"]+"[)] = "([^"]+)", "([^"]+)", "([^"]+)"')
        while 1:
            line = stream.readline ()
            if string.strip (line) == 'Global':
                return
            match = rex.match (line)
            if match:
                name, path, guid = match.group (1, 2, 3)
                fullpath = os.path.abspath (os.path.join (basedir, path))
                solution = Solution (name, fullpath, guid)
                self.projects.append (solution)
                self.guidMapping [guid] = solution

    def readDependencies (self, stream, targetMapping):
        line = stream.readline ()
        while string.strip (line) != 'GlobalSection(ProjectDependencies) = postSolution':
            if not line:
                return
            line = stream.readline ()
        rex = re.compile ('({[^}]+}).[0-9]+ = ({[^}]+})')
        while 1:
            line = stream.readline ()
            match = rex.search (line)
            if match:
                higher, lower = match.group (1, 2)
                higherProj = self.guidMapping [higher]
                lowerProj = self.guidMapping [lower]
                try:
                    target = targetMapping [string.lower (lowerProj.name)]
                    higherProj.addDependency (target)
                except KeyError:
                    pass
                    # print 'readDependencies: no target for', lowerProj.name
            else:
                break

    def getProjectDependencies (self, projectfile):
        for project in self.projects:
            if projectfile == project.path:
                return project.dependencies
        return None

class SolutionParser8:
    def __init__ (self, fname, targetMapping):
        self.parse (fname, targetMapping)

    def parse (self, fname, targetMapping):
        stream = open (fname, 'r')
        self.readProjects (stream, os.path.dirname (fname), targetMapping)
        stream.close ()

    def readProjects (self, stream, basedir, targetMapping):
        self.projects = []
        self.guidMapping = {}
        projectRE = re.compile ('Project[(]"[^"]+"[)] = "([^"]+)", "([^"]+)", "([^"]+)"')
        depRE = re.compile (r'\s*([{][^}]+[}])\s*=\s*([{][^}]+[}])')
        inDependencies = None
        while 1:
            line = stream.readline ()
            # print string.strip (line)
            if not line:
                break
            match = projectRE.match (line)
            if match:
                name, path, guid = match.group (1, 2, 3)
                fullpath = os.path.abspath (os.path.join (basedir, path))
                solution = Solution (name, fullpath, guid)
                solution.dependencyGuids = []
                self.projects.append (solution)
                self.guidMapping [guid] = solution
                continue
            if string.strip (line) == 'ProjectSection(ProjectDependencies) = postProject':
                inDependencies = 1
                continue
            if string.strip (line) == 'EndProjectSection':
                inDependencies = 1
                continue
            if inDependencies:
                match = depRE.match (line)
                if match:
                    dep1, dep2 = match.group (1, 2)
                    assert dep1 == dep2
                    solution.dependencyGuids.append (dep1)
        #
        # map guid dependencies to project dependencies
        #
        for project in self.projects:
            for depGuid in project.dependencyGuids:
                depProject = self.guidMapping [depGuid]
                target = targetMapping [string.lower (depProject.name)]
                project.addDependency (depProject)

    def getProjectDependencies (self, projectfile):
        for project in self.projects:
            if projectfile == project.path:
                return project.dependencies
        return None

def parseSolution (fname, targetMapping):
    stream = open (fname, 'r')
    line = stream.readline ()
    stream.close ()
    version = string.strip (line) [-4:]
    if version == '7.00':
        result = SolutionParser7 (fname, targetMapping)
    elif version == '8.00':
        result = SolutionParser8 (fname, targetMapping)
    else:
        raise 'no support for Solution file version ' + version
    return result

def reExpand (template, rex, match):
    variables = {}
    for varname, index in rex.groupindex.items ():
        variables [varname] = match.group (index)
    return template % variables

def unquote (str):
    if str[0] == '"' and str [-1] == '"':
        str = str [1:-1]
    return str

def split3 (filename):
    """split a filename into directory, basename, extension
    """
    directory, basename = os.path.split (filename)
    basename, extension = os.path.splitext (basename)
    return directory, basename, extension

def equalFiles (file1, file2):
    file1 = string.replace (file1, '\\', '/')
    return string.lower (file1) == string.lower (file2)

def setSplit (set1, set2):
    """split two lists into

       * elements only in set1
       * elements in both lists
       * elements only in set2
    """
    common = []
    rem1 = []
    rem2 = []
    for elem in set1:
        if elem in set2:
            common.append (elem)
        else:
            rem1.append (elem)
    for elem in set2:
        if elem in set1:
            pass
        else:
            rem2.append (elem)
    return rem1, common, rem2

def sortedDict (dict):
    result = dict.items ()
    result.sort ()
    return result

def splitAttribute (attributes, name, splitchars, excludes = []):
    try:
        asString = attributes [name]
    except KeyError:
        return []
    if splitchars == " ":
        asList = string.split (asString)
    elif len (splitchars) > 1:
        asList = re.split ('[' + splitchars + ']', asString)
    else:
        asList = string.split (asString, splitchars)
    result = []
    for item in asList:
        if item in excludes:
            continue
        result.append (string.replace (item, '\\', '/'))
    return result

def splitOnMultipleChars (string, splitChars):
    return r


knownLibs = {
    'allapi'    : '::aps/api/api/AllApi.lib',
    'stpapi'    : '::aps/api/StpApi/StpApi.lib',
    'tloapi'    : '::aps/api/TloApi/TloApi.lib',
    'apsbase'   : '::aps/base/apsbase.lib',
    'apscore'   : '::aps/core/apscore.lib',
    'atpapi'    : '::atp/api/AtpApi/AtpApi.lib',
    'atpcoreapi': '::atp/api/AtpCoreApi/AtpCoreApi.lib',
    'atplib'    : '::atp/atplib/atplib.lib',
    'deltalib2' : '::atp/deltalib2/deltalib2.lib',
    'ltt'       : '::ltt/LTT.lib',
    'ts'        : '::TimeSeries/Ts.lib',
    'tsapi'     : '::TimeSeries/api/tsapi.lib',
    'tssnp'     : '::TimeSeries/TsSnp/TsSnp.lib',
}

def loadCache (filename):
    if not os.path.exists (filename):
        return knownLibs
    execfile (filename, globals ())
    return knownLibs

def readConfig (options):
    if os.path.exists (options.config):
        locals = {}
        execfile (options.config, locals)
        for varname in ['inDEV', 'outSys']:
            configValue = locals.get (varname)
            if configValue and not getattr (options, varname):
                setattr (options, varname, configValue)

def checkOptions (options):
    readConfig (options)
    if options.ignore:
        if not os.path.exists (options.ignore):
            sys.stderr.write ('dsp2vmake: Cannot open file "%s" (from -ignore)\n')
            sys.exit (2)
        lines = open (options.ignore, 'rt').readlines ()
        lines = map (string.strip, lines)
        lines = map (string.lower, lines)
        lines = filter (None, lines)
        options.ignorePatterns = lines
    else:
        options.ignorePatterns = []
    try:
        options.ignorePatterns.extend (map (string.lower, specialCases.filesToIgnore))
    except AttributeError:
        pass
    options.globals.append ('*.cpp -except')
    if not options.inDEV:
        fatalError ('options -inDEV not set')
    if not options.outSys:
        fatalError ('options -outSys not set')
    if options.solutions:
        options.solutions = os.path.join (options.inDEV, options.solutions)
    options.nowarn = not options.warn

def stripComment (line):
    hashPos = string.find (line, '#')
    if hashPos != -1:
        line = line [:hashPos]
    return string.strip (line)

def getArgs (options, args):
    if not options.projectfiles:
        return args
    if os.path.exists (options.projectfiles):
        localName = options.projectfiles
    else:
        localName = os.path.join (options.inDEV, options.projectfiles)
    lines = open (localName, 'r').readlines ()
    lines = map (stripComment, lines)
    lines = filter (None, lines)
    result = []
    for line in lines:
        fname = string.replace (line, '\\', os.sep)
        fname = os.path.join (options.inDEV, fname)
        if not os.path.exists (fname):
            raise 'missing file in -projectfiles ' + options.projectfiles, line
        result.append (fname)
    return result + args

def main (options, args):
    """translates MS Dev projects to vmake descriptions

    """
    checkOptions (options)
    if options.unifiedSys:
        outDesc = os.path.join (options.outSys, 'src')
    else:
        outDesc = os.path.join (options.outSys, 'desc')
    # knownLibs = loadCache (options.cache)
    translator = Dsp2Vmake (options.inDEV, outDesc, None, options)
    args = getArgs (options, args)
    translator.getTargetKinds (args)
    if options.solutions:
        dependencies = parseSolution (options.solutions, translator.knownLibs)
    else:
        dependencies = None
    if args:
        for arg in args:
            ext = os.path.splitext (arg) [1]
            arg = os.path.abspath (arg)
            if ext == '.dsw':
                translator.translateWorkspace (arg)
            else:
                if dependencies:
                    solutionDependencies = dependencies.getProjectDependencies (arg)
                    translator.translateOneFile (arg,
                        solutionDependencies = solutionDependencies)
                else:
                    translator.translateOneFile (arg)
    else:
        translator.translate (options.inDEV, dependencies)
    translator.writeComDescriptions ()
    return translator.returnCode

def _options ():
    configFile = os.path.join (os.environ ['TOOL'], 'data', 'dsp2vmake.conf')
    default_inDEV = os.environ.get ('SRC')
    try:
        default_outSys = os.path.join (os.environ ['OWN'], 'sys')
    except KeyError:
        default_outSys = None
    return [
        # (optstring, varname, typechar, default, help)
        ('inDEV', None, ':', default_inDEV, 'input directory'),
        ('outSys', None, ':', default_outSys, 'output directory'),
        ('config', None, ':', configFile, 'name of configFile'),
        ('unifiedSys', None, '', true, '$DESC == $SRC'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('ignore', None, ':', None, """file containing file names to ignore.
             Matches anywhere in the file name
                 \\aps\\ would exclude all dirs under aps
                 xyz.dsp would exclude any file named xyz.dsp
             """),
        ('projectfiles', None, ':', None, 'file containing project files'),
        ('globals', None, '::', None, 'to insert in every .com'),
        #('cache', None, ':', 'dsp.cache', 'cache for dsp types'),
        ('debug', None, '', None, 'debug output'),
        ('nodep', None, '', None, 'don\'t generate dependency information'),
        ('warn', None, '', 1, 'generate warnings'),
        ('ignoreVariant', None, '::', None, 'don\'t generate this variant'),
        ('onlyVariant', None, '::', None, 'generate only these variants'),
        ('dumbVariants', None, '', 1, 'don\'t split DebugSim into Debug, Sim'),
        ('smartVariants', None, '', None, 'split DebugSim into Debug, Sim'),
        ('vcproj', None, '', None, 'search for new style .vcproj project files'),
        ('solutions', None, ':', None, 'use .sln file for dependencies'),
        ('strict', None, '', None, 'generate errors instead of some warnings'),
        ('version', None, '', _version, 'version information'),
        ]

def _getDefaultOptions ():
    argv = sys.argv [1:]
    import vmake.yutil
    try:
        fname = vmake.yutil.findFile (os.path.join (os.environ ['OWN'], 'sys/src/options.conf'))
        lines = open (fname, 'r').readlines ()
        for line in lines:
            hashpos = string.find (line, '#')
            if hashpos != -1:
                line = line [:hashpos]
            words = string.split (line)
            if (len (words) > 0) and (words [0] == 'dsp2vmake:'):
                argv = words [1:] + argv
    except vmake.yutil.notFound:
        pass
    return argv

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options (), _getDefaultOptions (),
        optvar = 'DSP2VMAKE_OPTION')

