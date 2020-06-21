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

*.lib binary  ->$TOOL/lib/${MACH}_python-py20.lib

*.h ascii ->$TOOL/incl/extern/python/sdk-py20/

*.exp ascii ->$TOOL/incl/extern/python/sdk-py20/

regex(.*_config.h) ascii ->$TOOL/incl/extern/python/sdk-py20/config.h

Python.h dep=\
    abstract.h,\
    bitset.h,\
    bufferobject.h,\
    cStringIO.h,\
    ceval.h,\
    classobject.h,\
    cobject.h,\
    codecs.h,\
    compile.h,\
    complexobject.h,\
    ${MACH}_${OSSPEC}_config.h,\
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
    pydebug.h,\
    pyerrors.h,\
    pyfpe.h,\
    pymem.h,\
    pyport.h,\
    pystate.h,\
    pythonrun.h,\
    pythread.h,\
    rangeobject.h,\
    sliceobject.h,\
    stringobject.h,\
    structmember.h,\
    sysmodule.h,\
    token.h,\
    traceback.h,\
    tupleobject.h,\
    ucnhash.h,\
    unicodeobject.h

