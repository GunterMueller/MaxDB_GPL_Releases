
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

from Interface import Module, Class, Method, Constructor, ExportedConstants
from Cdcl import Cdcl
from PyGlue import PyGlue, PyType

def commError (resultvar):
    return 'errorResult (%s)' % resultvar

module = Module ('control',
    exceptions = ['communication'])

controlClass = Class ('void', pyname = 'Session',
    destructor = 'destructor_Session')

controlClass.addAll ([
        Method (Cdcl ("""
            void 
            c13release (
                void** session)"""),
            doc = 'disconnect from server', exception = commError),
        Method (Cdcl ("""
            void 
            c13request (
                void* session,
                char* errtext,
                tsp_int4* rc)"""),
            doc = 'send request to server', exception = commError),
        Method (Cdcl ("""
            void 
            c13receive (
                void* session,
                char* errtext,
                tsp_int4* rc)"""),
            doc = 'receive results from server', exception = commError),
        Method (Cdcl ("""
            void 
            c13execute (
                void* session,
                char* errtext,
                tsp_int4* rc)"""),
            doc = 'request and receive in one step', exception = commError),
        Method (Cdcl ("""
            void 
            c13cmd (
                void* sessionParm,
                char* cmd,
                tsp_int4 cmdLen,
                tsp_int4* outLen,
                char* errtext,
                tsp_int4* rc)"""),
            doc = 'send comand to server and receive result', exception = commError),
        Method (Cdcl ("""
            void 
            c13packetwrite (
                void *session,
                char* data,
                tsp_int4 len,
                tsp_int4* rc)"""),
            doc = 'write command to packet', exception = commError),
        Method (Cdcl ("""
            void 
            c13packetread (
                void *session,
                char* data,
                tsp_int4* len,
                tsp_int4* rc)"""),
            doc = 'read part of the result', exception = commError),
        Method (Cdcl ("""
            void 
            c13packetupto (
                void *session,
                char* data,
                tsp_int4 len,
                int searchChar,
                tsp_int4* rc)"""),
            doc = 'read from the answer upto delimiter', exception = commError),
        Method (Cdcl ("""
            void 
            c13packetstats (
                void *session,
                tsp_int4 *packetSize,
                tsp_int4 *packetLen,
                tsp_int4 *packetPos)"""),
            doc = 'give sizes of packet', exception = commError),
        Method (Cdcl ("""
            """),
            doc = 'disconnect from server', exception = commError),
        ])
        
module.addAll ([
        controlClass,
        Method (Cdcl ("""
            void c13connect (
                    const char* servernode,
                    const char* dbname,
                    const char* dbroot,
                    void** sessionOut,
                    char* errtext,
                    tsp_int4* rc)"""),
            doc = 'Connect ro control server', exception = commError),
        Method (Cdcl ("""
            void c13connect (
                    const char* servernode,
                    const char* dbname,
                    const char* dbroot,
                    const char* serverpgm,
                    void** sessionOut,
                    char* errtext,
                    tsp_int4* rc)"""),
            doc = 'Connect ro serverpgm server', exception = commError),
        ])
        
if __name__ == "__main__":
    try:
        fname = module.name + 'module.c'
        generator = PyGlue (fname)
        #for mapping in typeMappings:
        #    generator.addType (mapping)
        module.writeGlue (generator)
    except KeyboardInterrupt:
        sys.stderr.write ("Aborted\n")
        sys.exit (255)
