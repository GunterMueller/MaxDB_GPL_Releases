
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
import Ctype
from AbstractGlue import MappedType, AbstractGlue, AbstractModuleGlue
from Interface import Class

class PerlMap (MappedType):
    def __init__ (self, decl, getPattern, putPattern, destructor = None, initialize = None):
        MappedType.__init__ (self, decl)
        self.getPattern = getPattern
        self.putPattern = putPattern
        self.destructorFunction = destructor
        self.initialize = initialize

    def getArg (self, name, index):
        return self.getPattern % locals ()

    def putArg (self, name):
        return self.putPattern % locals ()

    def destructor (self, name):
        destructor = self.destructorFunction
        if destructor is None:
            return None
        else:
            return destructor % locals ()

class PerlModuleGlue (AbstractModuleGlue):
    language = 'Perl'

    def addStandardTypes (self):
        for decl, get, put in [
                ('char *', '%(name)s = (char *) SvPV (ST(%(index)d), na)', 'sv_setpv (ST (argvi++), (char *) %(name)s)'),
                ('int', '%(name)s = (int) SvIV (ST(%(index)d))', 'sv_setiv (ST (argvi++), (IV) %(name)s)'),
                ('long', '%(name)s = (long) SvIV (ST(%(index)d))', 'sv_setiv (ST (argvi++), (IV) %(name)s)'),
                ]:
            self.addType (PerlMap (decl, get, put))

    def defaultType (self, decl):
        # print 'no match for', decl, 'using default'
        return PerlMap (decl, 'noConversion', 'noConversion')

    def __init__ (self, fname, typeDict = Ctype.TypeDict):
        self.simpleBinary = None
        AbstractModuleGlue.__init__ (self)
        self.typeDict = typeDict
        self.fname = fname

    def close (self):
        if self.cStream:
            self.cStream.close ()
            self.cStream = None
        if self.pmStream:
            self.pmStream.write ("\n# End of module\n\n1;\n")
            self.pmStream.close ()
            self.pmStream = None

    __del__ = close

    def writeModuleGlue (self, module):
        self.openCStream (module)
        self.methodWriter = PerlMethodGlue (self.cStream, self)
        self.constructorWriter = PerlConstructorGlue (self.cStream, self)
        self.openPMStream (module)
        module.perlPrefix = self.modPath (module) + 'c::'
        self.declareExceptions (module)
        self.writeSnippet (module.snippets, 'precode', 'Perl_')
        for item in module.items:
            item.writeGlue (self)
        #mtableName = module.pyname + 'ModuleMethods'
        #self.writeMethodTable (mtableName, module.items)
        self.writeInitfunc (module, '')
        self.close ()

    def openCStream (self, module):
        self.cStream = open (self.fname, "w")
        self.writeModuleHeader (self.cStream, module, 'Perl')
        self.cStream.write ('''
#include <stddef.h>

#ifdef __cplusplus
#include <math.h>
#include <stdlib.h>
extern "C" {
#else
#define bool char
#define HAS_BOOL 1
#endif

#define PERL_GLUE 1
#define Glue_BEGIN_ALLOW_THREADS
#define Glue_END_ALLOW_THREADS

#include "patchlevel.h"/* no check */

#if defined (ACTIVE_PERL)
    #if PATCHLEVEL < 6
        #define PERL_OBJECT
    #endif
#endif

#include "EXTERN.h"/* no check */
#include "perl.h"/* no check */
#if defined (PERL_OBJECT)
#include "perlCAPI.h"/* no check */
#endif
#include "XSUB.h"/* no check */
#undef free
#undef malloc
#undef realloc
#include <string.h>
#if defined (bool)
#undef bool
#endif
#if defined (PERL_OBJECT)
    #if !defined (PL_sv_undef)
        #define PL_sv_undef sv_undef
    #endif
    #if !defined (PL_sv_yes)
        #define PL_sv_yes sv_yes
    #endif
    #if !defined (PL_errgv)
        #define PL_errgv errgv
    #endif
#endif
#ifdef __cplusplus
}
#endif

#if defined (_WIN32)
#define GLUEEXPORT __declspec(dllexport)
#else
#define GLUEEXPORT
#endif

#define LIT(c) CONST_CAST(char *, c)
#define CROAKBUF_SIZE 200

#define MF__
#if defined (__cplusplus)
#define externC extern "C"
#else
#define externC
#endif

#define SL_None &PL_sv_undef
#define SL_isNone(val) (!SvOK (val))
#define SL_isTrue(val) SvTRUE (val)

static const char * invalidArgCount_C = "Invalid number of arguments";

static void *
getGluePointer (SV * sv, const char * className, char * croakBuf)
{
    IV   tmp;

    if (SvGMAGICAL(sv)) {
        mg_get(sv);
    }

    /* Check to see if this is an object */
    if (sv_isobject(sv)) {
        SV *tsv = (SV*) SvRV(sv);
        if ((SvTYPE(tsv) == SVt_PVHV)) {
            MAGIC *mg;
            if (SvMAGICAL(tsv)) {
                mg = mg_find(tsv,'P');
                if (mg) {
                    SV *rsv = mg->mg_obj;
                    if (sv_isobject(rsv)) {
                        tmp = SvIV((SV*)SvRV(rsv));
                    }
                }
            }
            else {
                strcpy (croakBuf, "Not a valid pointer value");
                return NULL;
            }
        }
        else {
            tmp = SvIV((SV*)SvRV(sv));
        }
    }
    else if (SL_isNone (sv)) {            /* Check for undef */
        strcpy (croakBuf, "Undef object not allowed");
        return NULL;
    }
    else if (SvTYPE(sv) == SVt_RV) {       /* Check for NULL pointer */
        if (!SvROK(sv)) {
            strcpy (croakBuf, "NULL object not allowed");
        }
        else {
            strcpy (croakBuf, "NULL object not allowed");
        }
        return NULL;
    }
    else {                                 /* Dont know what it is */
        strcpy (croakBuf, "Not a valid pointer value");
        return NULL;
    }
    /* Now see if the types match */

    if (!sv_isa (sv, (char *) className)) {
        sprintf (croakBuf, "Object is of class %s (should be %s)",
                HvNAME(SvSTASH(SvRV(sv))), className);
        return NULL;
    }
    return (void*) tmp;
}


''')

    def openPMStream (self, module):
        self.pmStream = open (module.pyname + '.pm', "w")
        self.methodWriter.pmStream = self.pmStream
        self.constructorWriter.pmStream = self.pmStream
        fullpath = self.modPath (module)
        self.pmStream.write ("""# This file was automatically generated by glue
#
#   ========== licence begin  GPL
#   Copyright (c) 2000-2006 SAP AG
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
#   ========== licence end
""")
        self.pmStream.write ("""

package %(fullpath)spackage;

$%(fullpath)spackage::extensionLoaded = 0;

if(defined(&Win32::BuildNumber)) {
    package %(fullpath)saperl;
    require Exporter;
    require DynaLoader;
    @ISA = qw(Exporter DynaLoader);
    package %(fullpath)sc;
    eval {
        bootstrap %(fullpath)saperl;
        @EXPORT = qw( );
        $%(fullpath)spackage::extensionLoaded = 1;
    };
}

if (!$%(fullpath)spackage::extensionLoaded) {
    package %(fullpath)scperl;
    require Exporter;
    require DynaLoader;
    @ISA = qw(Exporter DynaLoader);
    package %(fullpath)sc;
    bootstrap %(fullpath)scperl;
    @EXPORT = qw( );
}

""" % locals ())
        self.pmStream.write ("""

# ------- FUNCTION WRAPPERS --------

package %s;

my %%OWNER = ();

""" % fullpath)

    def modPath (self, module):
        if module.modpath:
            result = string.join (module.modpath + [module.pyname], '::')
        else:
            result = ''
        return result

    def declareExceptions (self, module):
        if not module.exceptions:
            return
        write = self.cStream.write
        write ('/*\n * Exceptions\n */\n\n')
        for exception in module.exceptions:
            write ('static const char * %s_%s = "%s";\n'
                % (exception.name, module.pyname, exception.name))
        write ('\n')

    def writeInitfunc (self, module, mtablename):
        write = self.cStream.write
        fullpath = self.modPath (module)
        bootname = 'boot_' + string.replace (fullpath, ':', '_')
        if not self.simpleBinary:
            write ("""
#if defined (ACTIVE_PERL)
#define %s %saperl
#else
#define %s %scperl
#endif
""" % ((bootname,) * 4))
        write ('\nexternC GLUEEXPORT XS(%s)\n{\n' % bootname)
        self.writeBlock ([
                'dXSARGS;',
                'char * file = CONST_CAST (char *, __FILE__);',
                'cv = cv;',
                'items = items;'
               ], '    ')
        for item in [module] + module.items:
            for method in item.methods ():
                if self.supports (method):
                    pyname = method.pyname
                    write ('    newXS (LIT ("%sc::%s_%s"), %s_%s, file);\n'
                            % (fullpath, method.context.pyname,
                                    pyname, method.context.pyname, pyname))
        for item in module.items:
            if isinstance (item, Class):
                write ('    newXS (LIT("%sc::%s_destructor"), %s_destructor, file);\n'
                    % (fullpath, item.pyname, item.pyname))
        self.writeSnippet (module.snippets, 'initroutine', 'Perl_')
        self.writeBlock ([
            'ST(0) = &PL_sv_yes;',
            'XSRETURN(1);',
            ], '    ')
        write ('}\n\n')

    # class generation

    def writeClassGlue (self, clazz):
        self.writeNativeStruct (clazz)
        self.writeSnippet (clazz.snippets, 'precode', 'Perl_')
        clazz.perlPrefix = clazz.context.perlPrefix #obsolete + clazz.pyname
        self.writePMClassEntry (clazz)
        for item in clazz.items:
            item.writeGlue (self)
        self.writeDestructor (clazz)
        self.writePMClassExit (clazz)

    def writeNativeStruct (self, clazz):
        cStream = self.cStream
        selfname = clazz.pyname
        cStream.write ('static const char * %sClassID = "%s";\n\n' % (selfname, selfname))
        cStream.write ('typedef struct %sObjectT {\n    const char * classID;\n'
            % selfname)
        # cStream.write ('    SV * perlself;\n');
        if clazz.isEmbedded:
            cStream.write ('    %s nself;\n' % clazz.name)
        else:
            cStream.write ('    %s *nself;\n' % clazz.name)
        self.writeSnippet (clazz.snippets, 'instance vars', 'Perl_', '    ')
        cStream.write ('} %sObjectT;\n\n' % selfname)
        cStream.write ('#define is%sObject(op) ((op)->classID == %sClassID)\n\n'
            % (selfname, selfname))
        cStream.write ('static const char * notClass%s_C = "Object not of class %s";\n'
                % (selfname, selfname))
        cStream.write ("""
static %sObjectT *
new%s ()
{
    %sObjectT * result;

    Newz (0, result, 1, %sObjectT);
    return result;
}

static void
free%s (
    %sObjectT * value)
{
    Safefree (value);
}
""" % ((selfname,) * 6))

    def writePMClassEntry (self, clazz):
        write = self.pmStream.write
        write ('\n### class %s ###\n' % clazz.pyname)
        for line in [
                'package %s;\n' % clazz.pyname,
                'use strict;',
                '',
                'sub TIEHASH {',
                '    my ($classname,$obj) = @_;',
                '    return bless $obj, $classname;'
                '}',
                '',
                'sub CLEAR { }',
                ]:
            write (line + '\n')
        if clazz.constructor != None:
            write ('''
sub new {
    my $clazzName = "%s";
    my $self = shift;
    $self = %s%s_%s(@_);
    return undef if (!defined($self));
    bless $self, $clazzName;
    $OWNER{$self} = 1;
    my %%retval;
    tie %%retval, $clazzName, $self;
    return bless \%%retval, $clazzName;
}

''' % (clazz.pyname, clazz.context.perlPrefix,
        clazz.context.pyname, clazz.constructor.pyname))
        write ('''
sub DESTROY {
    my ($self) = @_;
    return unless defined $self;

    # Only do something on the inner (non-tied) object
    if (ref($self) ne 'HASH' && exists $OWNER{$self}) {
        %s%s_destructor ($self);
        delete $OWNER{$self};
    }
}
''' % (clazz.context.perlPrefix, clazz.pyname,))

    def writePMClassExit (self, clazz):
        write = self.pmStream.write
        write ('### end of class %s ###\n' % clazz.pyname)
        write ('\npackage %s;\n' % clazz.context.pyname)

    def writeDestructor (self, clazz):
        pyname = clazz.pyname
        typename = clazz.name
        if clazz.isEmbedded:
            getSelf = '&self->nself'
        else:
            getSelf = 'self->nself'
        freeFunction = clazz.attributes.get ('perlFreeFunction')
        if freeFunction:
            destruction = freeFunction + ' (self)'
        else:
            if clazz.destructor != None:
                destructor = '%s (nself)' % clazz.destructor
            else:
                destructor = '/* no destructor */'
            destruction = '%s;\n    free%s (self)' % (destructor, pyname)
        self.cStream.write ('''
/*----------------------------------------*/

externC XS (%(pyname)s_destructor)
{
    ROUTINE_DBG_MSP00 ("%(pyname)s_destructor");
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    %(pyname)sObjectT* self;
    %(typename)s* nself;

    cv = cv;
    if (items != 1) {
        croak (invalidArgCount_C);
    }
    self = (%(pyname)sObjectT*) getGluePointer (ST(0), %(pyname)sClassID, croakBuf);
    if ((self == NULL) || !is%(pyname)sObject (self)) {
        croak (croakBuf);
    }
    nself = %(getSelf)s;
    %(destruction)s;
    XSRETURN (0);
}

''' % locals ())

    # constant part generation

    def writeConstantPart (self, constpart):
        cStream = self.cStream
        funcname = 'initConstants%d' % constpart.context.constantParts
        cStream.write ('static void\n%s (PyObject* module)\n{\n' % funcname)
        functionTag (cStream, funcname)
        constpart.context.constantParts = constpart.context.constantParts + 1
        cStream.write ("""
    PyObject*   v;
    int         i;
    static struct {
        const char* name;
        %s          val;
    } items [] = {""" % constpart.argType (constpart.typechar))
        for name in constpart.names:
            cStream.write ('\n        {"%s", %s},' % (name, name))
        cStream.write ('''
        {NULL}};

    for (i = 0; i < %d; ++i) {
        v = Py_BuildValue ("%s", items [i].val);
        PyDict_SetItemString (module, items [i].name, v);
        Py_DECREF (v);
    }
}

''' % (len (constpart.names), constpart.typechar))

class PerlMethodGlue (AbstractGlue):
    language = 'Perl'

    def __init__ (self, cStream, moduleGlue):
        self.cStream = cStream
        self.moduleGlue = moduleGlue

    def findType (self, decl):
        return self.moduleGlue.findType (decl)

    def writeGlue (self, method):
        if not self.supports (method):
            return
        cStream = self.cStream
        self.writeProcDeclaration (method)
        snippetsFound = self.writeSnippet (method.snippets, 'code', 'Perl_')
        if not snippetsFound:
            self.needsCleanup = 0
            self.writeMethodBody (method)
            self.writeMethodEnd (method)
        self.cStream.write ('}\n\n')
        self.writePmWrapper (method)

    def writePmWrapper (self, method):
        self.pmStream.write ("""
sub %s {
""" % method.pyname)
        #if isinstance (method.context, Class):
        #    self.pmStream.write ('    $args[0] = tied(%{$args[0]});\n')
        self.pmStream.write ('    return %s%s_%s(@_);\n}\n' % (method.context.perlPrefix, method.context.pyname, method.pyname))

    def writeMethodBody (self, method):
        self.writeInArgs (method)
        snippetsFound = self.writeSnippet (method.snippets, 'nativeCall', 'Perl_')
        if not snippetsFound:
            self.writeNativeCall (method)
        self.writeOutArgs (method)

    def writeProcDeclaration (self, method):
        funcname = '%s_%s' % (method.context.pyname, method.pyname)
        self.cStream.write ('/*----------------------------------------*/\n\n')
        self.cStream.write ('externC XS (%s)\n{\n'
                % funcname)
        functionTag (self.cStream, funcname)

    def writeInArgs (self, method):
        cStream = self.cStream
        context = method.context
        fmtstr = ""
        callList = []
        self.writeBlock ([
            'int ok = 1;',
            'int argvi = 0;',
            'char croakBuf [CROAKBUF_SIZE];',
            'dXSARGS;',
            ], '    ')
        # self.declareResultVar (cStream, method)
        #
        # create self
        #
        if context.isClass:
            cStream.write ('    %sObjectT* self;\n' % context.pyname)
            cStream.write ('    %s* nself;\n' % context.name)
            argStart = 1
        else:
            argStart = 0
        #
        # declare other parms (and init defaults)
        #
        parms = method.parms
        minArgs = argStart
        maxArgs = argStart
        scriptingInput = []
        for i in xrange (argStart, len (parms)):
            parm = parms [i]
            argstr = parm.type
            typeMapper = self.findType (argstr)
            if parm.isScriptingInput ():
                scriptingInput.append (parm)
                maxArgs = maxArgs + 1
                if not parm.defaultVal:
                    minArgs = minArgs + 1
            for varDcl in typeMapper.varDcls (parm.name, parm.type, parm.defaultVal):
                cStream.write ('    %s;\n' % varDcl)
        #
        # initialize arguments
        #
        cStream.write ('\n')
        for i in xrange (argStart, len (parms)):
            parm = parms [i]
            argstr = parm.type
            typeMapper = self.findType (argstr)
            if typeMapper.initialize:
                cStream.write ("    memset (&%s, '\\0', sizeof (%s));\n" 
                    % (parm.name, parm.name))
        #
        # check argcount
        #
        if minArgs == maxArgs:
            condition = 'items != %d' % minArgs
        else:
            condition = '(items < %d) || (items > %d)' % (minArgs, maxArgs),
        self.writeBlock ([
            '',
            'cv = cv;',
            'if (%s) {' % condition,
            '    croak (invalidArgCount_C);',
            '}',
            ], '    ')
        #
        # get self
        #
        if context.isClass:
            className = context.pyname
            block = [
                'self = (%sObjectT*) getGluePointer (ST(0), %sClassID, croakBuf);' % (className, className),
                'if ((self == NULL) || !is%sObject (self)) {' % className,
                '    croak (croakBuf);',
                '}',
                # 'self->perlself = ST (0);',
                'nself = self->nself;',
                ]
            if context.isEmbedded:
                block [-1] = 'nself = &self->nself;'
            self.writeBlock (block, '    ')
        #
        # get arguments
        #
        if len (scriptingInput) > 0:
            cStream.write ('    switch (items) {\n')
            for i in xrange (len (scriptingInput) - 1, -1, -1):
                parm = scriptingInput [i]
                index = i + argStart
                typeMapper = self.findType (parm.type)
                popCode = typeMapper.getArg (parm.name, index)
                cStream.write ('        case %d: %s;\n'
                        % (index + 1, popCode))
            cStream.write ('    }\n')
            self.writeErrorCondition ('!ok', method)
        if method.context.precondition:
            self.writeErrorCondition ('!' + (method.context.precondition % 'self'), method)

    def declareResultVar (self, cStream, method):
        if method.returns:
            cStream.write ('    %s result;\n' % method.returns.type)

    def writeNativeCall (self, method):
        cStream = self.cStream
        cStream.write ('    ')
        if method.returns:
            cStream.write (self.resultLVal () + ' = ')
        cStream.write (method.name + ' (')
        args = []
        if method.parms:
            if method.context.isClass:
                parms = method.parms [1:]
            else:
                parms = method.parms
            for parm in parms:
                if parm != method.returns:
                    typeMapper = self.findType (parm.type)
                    callParmString = typeMapper.asCallParameter (parm.name)
                    if parm.isOutput ():
                        callParmString = '&' + callParmString
                    args.append (callParmString)
        if method.context.isClass:
            args.insert (0, 'nself')
        cStream.write (string.joinfields (args, ', '))
        cStream.write (');\n')
        if method.exception:
            self.writeResultException (method)

    def resultLVal (self):
        return 'result'

    def writeResultException (self, method):
        self.writeErrorCondition (method.exception (self.resultLVal ()), method)

    def writeOutArgs (self, method):
        write = self.cStream.write
        write ('    /* outargs */\n')
        for parm in method.parms:
            if parm.isScriptingOutput ():
                #self.writeBlock ([
                #    'if (argvi >= items) {',
                #    '    EXTEND (sp, 1);',
                #    '}',
                #    'ST (argvi) = sv_newmortal ();',
                #    ], '    ')
                typeMapper = self.findType (parm.type)
                setResult = typeMapper.putArg (parm.name)
                write ('    ST (argvi) = sv_newmortal ();\n')
                write ('    %s;\n' % setResult)
                # conversion = typeMapper.c2py (parm.name)
                # if conversion:
                #     self.writeErrorCondition ('!' + conversion, method)

    def writeErrorCondition (self, condition, method, additionalCode = []):
        self.needsCleanup = 1
        self.writeCompound ('if (%s)' % condition, [
                            'ok = 0;' ]
                            + additionalCode + [
                            'goto cleanup_label;'],
                            '    ')

    def writeMethodCleanup (self, method, indent = '    '):
        for parm in method.parms:
            typeMapper = self.findType (parm.type)
            destructor = typeMapper.destructor (parm.name)
            if destructor:
                self.cStream.write (indent + destructor + ';\n')

    def writeErrorCleanup (self, method, indent = '    '):
        pass

    def writeMethodEnd (self, method):
        self.writeCleanup (method)
        self.writeReturn (method)

    def writeCleanup (self, method):
        if self.needsCleanup:
            cStream = self.cStream
            cStream.write ('  cleanup_label:\n')
            self.writeMethodCleanup (method, '    ')
            cStream.write ('    if (!ok) {\n')
            self.writeErrorCleanup (self)
            cStream.write ('        croak (croakBuf);\n')
            cStream.write ('    }\n')

    def writeReturn (self, method):
        self.cStream.write ('    XSRETURN (argvi);\n')

class PerlConstructorGlue (PerlMethodGlue):
    def writeObjectConstruction (self, method):
        tname = method.clazz.pyname
        #self.cStream.write ('    Newz (0, newObj, 1, %sObjectT);\n' % tname);
        self.cStream.write ('    newObj = new%s ();\n' % tname);
        self.writeErrorCondition ('allocFailed (newObj, croakBuf)', method,
            additionalCode = ['newObj = NULL;'])
        #self.cStream.write ("    memset (newObj, '\\0', sizeof (%sObjectT));\n"
        #        % tname)
        self.cStream.write ('    newObj->classID = %sClassID;\n' % tname)

    def declareResultVar (self, cStream, method):
        cStream.write ('    %sObjectT * newObj = NULL;\n' % method.clazz.pyname)

    def resultLVal (self):
        return 'newObj->nself'

    def writeInArgs (self, method):
        self.declareResultVar (self.cStream, method)
        PerlMethodGlue.writeInArgs (self, method)
        self.writeObjectConstruction (method)

    def writeOutArgs (self, method):
        self.writeBlock ([
            '/* outargs */',
            'ST (argvi) = sv_newmortal ();',
            'sv_setref_pv (ST (argvi++), (char *)%sClassID, (void *) newObj);' % method.clazz.pyname,
            ], '    ')

    def writeErrorCleanup (self, method, indent = '    '):
        PerlMethodGlue.writeErrorCleanup (self, method, indent = '        ')
        self.cStream.write ('''        if (newObj != NULL) {
            free (newObj);
        }\n''')

    def writePmWrapper (self, method):
        clazz = method.clazz
        self.pmStream.write ('''
sub %s {
    my $clazzName = "%s";
    my $self = %s%s_%s(@_);
    return undef if (!defined($self));
    bless $self, $clazzName;
    $OWNER{$self} = 1;
    my %%retval;
    tie %%retval, $clazzName, $self;
    return bless \%%retval, $clazzName;
}

''' % (method.pyname, clazz.pyname, clazz.context.perlPrefix,
        method.context.pyname, method.pyname))

def functionTag (cStream, name):
    cStream.write ('    ROUTINE_DBG_MSP00 ("%s");\n' % name)

_popArgCode = {
        's' : 'SvPV (ST(%d), na)',
        'i' : 'SvIV (ST(%d))',
        }

_pushArgCode = {
        's': '    sv_setpv ((SV*) ST (argvi++), (char *) %(name)s);\n',
        'i': '    sv_setiv (ST (argvi++), (IV) %(name)s);\n',
        'O': '    sv_setpv ((SV*) ST (argvi++), (char *) %(name)s);\n',
        }

PerlGlue = PerlModuleGlue
