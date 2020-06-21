#
#
#    ========== licence begin LGPL
#    Copyright (C) 2000-2003 SAP AG
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

I386_python.lib binary ->$TOOL/lib/I386_python-py22.lib
IA64_python.lib binary ->$TOOL/lib/IA64_python-py22.lib

&if $MACH == I386 && $OSSPEC == LINUX
*.h ascii output=$TOOL/incl/extern/python/sdk-py22/,$TOOL/incl/extern/python/sdk-py22ucs4/
&else
*.h ascii output=$TOOL/incl/extern/python/sdk-py22/
&endif

*.exp ascii ->$TOOL/incl/extern/python/sdk-py22/

regex(.*_pyconfig.h) ascii ->$TOOL/incl/extern/python/sdk-py22/pyconfig.h
regex(.*_pyconfig_ucs4.h) ascii ->$TOOL/incl/extern/python/sdk-py22ucs4/pyconfig.h

Python.h dep=\
    abstract.h,\
    bitset.h,\
    bufferobject.h,\
    cStringIO.h,\
    cellobject.h,\
    ceval.h,\
    classobject.h,\
    cobject.h,\
    codecs.h,\
    compile.h,\
    complexobject.h,\
    descrobject.h,\
    dictobject.h,\
    errcode.h,\
    eval.h,\
    fileobject.h,\
    floatobject.h,\
    frameobject.h,\
    funcobject.h,\
    graminit.h,\
    grammar.h,\
    import.h,\
    intobject.h,\
    intrcheck.h,\
    iterobject.h,\
    listobject.h,\
    longintrepr.h,\
    longobject.h,\
    marshal.h,\
    metagrammar.h,\
    methodobject.h,\
    modsupport.h,\
    moduleobject.h,\
    node.h,\
    object.h,\
    objimpl.h,\
    opcode.h,\
    osdefs.h,\
    parsetok.h,\
    patchlevel.h,\
    pgenheaders.h,\
    py_curses.h,\
    ${MACH}_${OSSPEC}_pyconfig.h,\
&if $MACH == I386 && $OSSPEC == LINUX
    ${MACH}_${OSSPEC}_pyconfig_ucs4.h,\
&endif
    pydebug.h,\
    pyerrors.h,\
    pyfpe.h,\
    pygetopt.h,\
    pymactoolbox.h,\
    pymem.h,\
    pyport.h,\
    pystate.h,\
    pythonrun.h,\
    pythread.h,\
    rangeobject.h,\
    sliceobject.h,\
    stringobject.h,\
    structmember.h,\
    structseq.h,\
    symtable.h,\
    sysmodule.h,\
    token.h,\
    traceback.h,\
    tupleobject.h,\
    ucnhash.h,\
    unicodeobject.h,\
    weakrefobject.h

