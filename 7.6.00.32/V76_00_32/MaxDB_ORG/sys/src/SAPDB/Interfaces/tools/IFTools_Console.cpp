/*!
  @file           IFTools_Console.cpp
  @author         D039759
  @ingroup        IFR_Trace
  @brief          SAPDBC observation and configuration tool.
  @see

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



\endif
*/

#include "Interfaces/tools/IFTools_Console.h"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"
#include "RunTime/RTE_IniFileHandling.h"
#include "Interfaces/Runtime/Util/IFRUtil_Configuration.h"
#include "Interfaces/Runtime/Util/IFRUtil_Vector.h"
#include "RunTime/InputOutput/RTEIO_DirectoryIterator.hpp"

#include <stdio.h>

static IFR_Int4 strtosize(const char *str, IFR_Int4 len)
{
    char *endptr;
    IFR_Int4 result = strtol(str, &endptr, 0);
    IFR_Int4 curLen= endptr - str;
    while(len > curLen && isspace(*endptr)) {
        ++endptr; ++curLen;
    }
    if(len <= curLen || *endptr == '\0') {
        return result;
    }
    if(len > curLen && (*endptr == 'K' || *endptr == 'k')) {
        result *= 1024;
        ++endptr;++curLen;
    } else if(len > curLen && (*endptr == 'M' || *endptr == 'm')) {
        result *= 1024 * 1024;
        ++endptr;++curLen;
    } else if(len > curLen &&(*endptr == 'G' || *endptr == 'g')) {
        result *= 1024 * 1024 * 1024;
        ++endptr;++curLen;
    } else {
        return -1;
    }
    if(len > curLen &&(*endptr == 'b' || *endptr =='B')) {
        ++endptr;++curLen;
    }
    while(len > curLen && isspace(*endptr)) {
        ++endptr;++curLen;
    }
    if(len <= curLen || *endptr == '\0') {
        return result;
    }
    return -1;
}


struct TraceFlags
{
    TraceFlags(const char *flags)
    :m_call(false),
     m_debug(false),
     m_profile(IFR_TraceProfileAction_NOTSET),
     m_packet(false),
     m_sql(false),
     m_packetsize(1000),
     m_filesize(-1),
     m_timestamp(false),
     m_stop_on_error(false),
     m_errorcode(0),
     m_errorcount(0),
     m_compressed(false)
    {
        char *spos, *p=(char *) alloca(strlen(flags));
        spos = p;
        strcpy(p, flags);
        while(p && *p) {
            char *cpos=strchr(p, ':');
            if(cpos != 0)
                *cpos=0;
            switch(*p) {
            case 'c':
                m_call=true;
                break;
            case 'd':
                m_debug=true;
                break;
            case 'a':
                m_sql=true;
                break;
            case 'z':
                m_compressed=true;
                break;
            case 'p': {
                ++p;
                if(*p) {
                    m_packetsize=strtosize(p,  (strlen(flags)-(p-spos)));
                    if(m_packetsize <= 0) {
                        m_packetsize=1000;
                    }
                }
                m_packet=true;
                break;
            }
            case 's':
                ++p;
                if(*p) {
                    m_filesize=strtosize(p,  (strlen(flags)-(p-spos)));
                    if(m_filesize!=-1 &&
                       m_filesize < 8192) {
                        m_filesize=8192;
                    }
                }
                break;
            case 'G': {
                ++p;
                if(*p) {
                    m_profile=(IFRUtil_TraceSharedMemory_TraceProfileAction) strtosize(p,  (strlen(flags)-(p-spos)));
                    if(m_profile!= IFR_TraceProfileAction_RESET
                       && m_profile!= IFR_TraceProfileAction_COLLECT) {
                        m_profile= IFR_TraceProfileAction_NOTSET;
                    }
                }
                break;
            }
            case 'T':
                m_timestamp = true;
                break;
            case 'e': {
                ++p;
                char *slpos =strchr(p, '/');
                if(slpos == 0) {
                    m_errorcode=atoi(p);
                    if(m_errorcode != 0) {
                        m_stop_on_error = true;
                    }
                } else {
                    *slpos = '\0';
                    m_errorcode = atoi(p);
                    if(m_errorcode != 0) {
                        m_stop_on_error = true;
                        m_errorcount = atoi(slpos + 1);
                        if(m_errorcount < 0) {
                            m_errorcount = 0;
                        }
                    }
                }
                break;
            }
            }
            if(cpos != 0) {
                p=cpos + 1;
            } else {
                p = 0;
            }
        }
    }

    void toString(char *str)
    {
        *str='\0';
        IFR_Bool optionset=false;
        if(m_call) {
            strcat(str, "c");
            optionset = true;
        }
        if(m_debug) {
            strcat(str, optionset ? ":d" : "d");
            optionset = true;
        }
        if(m_sql) {
            strcat(str, optionset ? ":a" : "a");
            optionset = true;
        }
        if(m_compressed) {
            strcat(str, optionset ? ":z" : "z");
            optionset = true;
        }
        if(m_profile != IFR_TraceProfileAction_NOTSET) {
            char buffer[128];
            sp77sprintf(buffer, 128, optionset ? ":G%d" : "G%d", m_profile);
            strcat(str, buffer);
            optionset = true;
        }
        if(m_timestamp) {
            strcat(str, optionset ? ":T" : "T");
            optionset = true;
        }
        if(m_packet) {
            char buffer[128];
            sp77sprintf(buffer, 128, optionset ? ":p%d" : "p%d", m_packetsize);
            strcat(str, buffer);
            optionset = true;
        }
        if(m_filesize > 0) {
            char buffer[128];
            sp77sprintf(buffer, 128, optionset ? ":s%d" : "s%d", m_filesize);
            strcat(str, buffer);
        }

        if(m_stop_on_error) {
            if(m_errorcount == 0) {
                char buffer[100];
                sp77sprintf(buffer, 100, optionset?":e%d":"e%d", m_errorcode);
                strcat(str, buffer);
            } else {
                char buffer[100];
                sp77sprintf(buffer, 100, optionset?":e%d/%d":"e%d/%d",
                            m_errorcode,
                            m_errorcount);
                strcat(str, buffer);
            }
        }

        return;
    }

    IFR_Bool m_call;
    IFR_Bool m_debug;
    IFRUtil_TraceSharedMemory_TraceProfileAction m_profile;
    IFR_Bool m_packet;
    IFR_Bool m_sql;
    IFR_Int4 m_packetsize;
    IFR_Int4 m_filesize;
    IFR_Bool m_timestamp;
    IFR_Bool m_stop_on_error;
    IFR_Int4 m_errorcode;
    IFR_Int4 m_errorcount;
    IFR_Bool m_compressed;
};

#define VERB_STREAM if(!m_verbose) {} else stdoutstream()

//----------------------------------------------------------------------
class IFTools_FileIterator
{
public:
    IFTools_FileIterator (const char * path);
    ~IFTools_FileIterator ();
    IFR_Bool next ();
    const char * currentKey ();
    const char * currentValue ();
    void         close ();

private:
  IFR_Bool nextSection (const char * section);
  IFR_Bool     open (const char * path);
  IFR_Bool     readline ();
  IFR_Bool isOpen(){
    return m_File!=0;
  }
  FILE* m_File;
  char m_buffer[256];
  IFR_Int4 m_valPos;
  IFR_Int4 m_bufLen;
  IFR_Bool m_sectionFinished;

};

IFTools_FileIterator::IFTools_FileIterator (const char * path):
m_File(0),
m_bufLen(0),
m_valPos(0),
m_sectionFinished(IFR_FALSE)
{
  if (! this->open(path)) this->close();
}

IFTools_FileIterator::~IFTools_FileIterator(){
  this->close();
}

void IFTools_FileIterator::close(){
    if (m_File) fclose(m_File);
    m_File= 0;
    m_bufLen = 0;
    m_valPos=0;
}

IFR_Bool IFTools_FileIterator::open(const char * path){
        m_File = fopen(path, "r");
        if (!m_File) {
//			    fprintf(m_sqlcliProp->geterrfile(),ERR_OPEN_FILE, filename, strerror(errno));
    	  return IFR_FALSE;
        }
        //find section
        if (!this->nextSection("[PROFILE]")){
          return IFR_FALSE;
        }
        return IFR_TRUE;
 }

IFR_Bool IFTools_FileIterator::nextSection (const char * section){
  do{
    if (! this->readline()){
      return IFR_FALSE;
    }
  } while (strncmp(section, m_buffer, strlen (section)));
  m_sectionFinished=IFR_FALSE;
  return IFR_TRUE;
}

IFR_Bool IFTools_FileIterator::next (){
  if (m_sectionFinished) return IFR_FALSE;
  while (this->readline()){
    if (m_bufLen && m_buffer[0] == '[') {
      m_sectionFinished = IFR_TRUE;
      return IFR_FALSE;
    }
    if (m_bufLen && m_buffer[0] != '#') return IFR_TRUE;
  }
  return IFR_FALSE;
}

#define CHECK_SPLIT                          \
  if (!isOpen())return 0;                    \
  if (!m_valPos){                            \
    char* pos = strrchr( m_buffer, '=' );    \
    if (!pos) return 0;                      \
    m_valPos= (int) (pos - m_buffer);        \
    m_buffer[m_valPos]=0;                    \
    m_valPos++;                              \
  }                                          \

const char * IFTools_FileIterator::currentKey (){
  CHECK_SPLIT
  return m_buffer;
}
const char * IFTools_FileIterator::currentValue () {
  CHECK_SPLIT
  return &m_buffer[m_valPos];
}


IFR_Bool IFTools_FileIterator::readline(){
  if (!isOpen())return IFR_FALSE;
  m_valPos=0;
  if (fgets(m_buffer,sizeof(m_buffer),m_File) )  {
    m_bufLen = strlen (m_buffer);
    if (m_buffer[m_bufLen-1] == 10) m_buffer[m_bufLen-1]= 0;
  } else {
    m_bufLen = 0;
    if (ferror(m_File)) {
//      fprintf(m_sqlcliProp->geterrfile(),ERR_READ_FILE, filename, strerror(errno));
    }
    this->close();
    return IFR_FALSE;
  }
  return IFR_TRUE;
}

//----------------------------------------------------------------------
IFTools_Output::IFTools_Output(const char *filename)
{
    tsp05_RteFileError error;
    sqlfopenc(filename,
              sp5vf_binary,
              sp5vf_write,
              sp5bk_unbuffered,
              &m_filehandle,
              &error);
    if(error.sp5fe_result != vf_ok) {
        m_filehandle = -1;
    }

    m_traceoptions.currentEntry   = 0;
    m_traceoptions.hex            = false;
    m_traceoptions.inputlength    = (unsigned int)-3;
    m_traceoptions.encoding       = IFR_StringEncodingAscii;
    m_traceoptions.indent         = 0;
    m_traceoptions.flags          = 0x0;
    m_traceoptions.traceableLimit = 1000;
    m_traceoptions.pos            = 0;

}

//----------------------------------------------------------------------
IFTools_Output::~IFTools_Output()
{
    tsp05_RteFileError ignored;
    sqlfclosec(m_filehandle, sp5vf_close_normal, &ignored);
    m_filehandle = -1;
}

//----------------------------------------------------------------------
void
IFTools_Output::write(const char *data, IFR_Int4 length)
{
    if(length < 0) {
        length=strlen(data);
    }

    tsp05_RteFileError error;
    sqlfwritec(m_filehandle,
               data,
               length,
               &error);
    if(error.sp5fe_result != vf_ok) {
        exit(127);
    }
}

//----------------------------------------------------------------------
void
IFTools_Output::writeln(const char *data, IFR_Int4 length)
{
    if(length < 0) {
        length=strlen(data);
    }

    tsp05_RteFileError error;
    sqlfwritec(m_filehandle,
               data,
               length,
               &error);
    if(error.sp5fe_result != vf_ok) {
        exit(127);
    }

    char c[3] = { 0x0D, 0x0A, 0x00 };
#ifdef _WIN32
    sqlfwritec(m_filehandle, c, 2, &error);
#else
    sqlfwritec(m_filehandle, c+1, 1, &error);
#endif
}



//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext*
IFTools_Output::traceflags()
{
    return &m_traceoptions;
}

//----------------------------------------------------------------------
SAPDBMem_IRawAllocator&
IFTools_Output::getAllocator()
{
    return RTEMem_RteAllocator::Instance();
}

//----------------------------------------------------------------------
void
IFTools_Output::traceError(IFR_Int4 errorcode, IFR_Connection *connection)
{}


//----------------------------------------------------------------------
IFTools_Console::IFTools_Console()
{
    m_stdoutwriter = new IFTools_Output("stdout");
    m_stdout       = new IFR_TraceStream(m_stdoutwriter);
    m_stderrwriter = new IFTools_Output("stderr");
    m_stderr       = new IFR_TraceStream(m_stderrwriter);

    m_command = C_NONE;
    m_verbose = false;
    m_force   = false;
    m_pidpresent = false;
    m_pid        = 0;
    m_on         = false;
    m_user       = 0;
    m_errorcode  = 0;
    m_errorcount = 0;

}

//----------------------------------------------------------------------
IFTools_Console::~IFTools_Console()
{
    delete m_stdout;
    delete m_stdoutwriter;
    delete m_stderr;
    delete m_stderrwriter;
}


//----------------------------------------------------------------------
static IFR_Bool
IFTools_Console_matchCommand(const char *check, const char* match, IFR_Int4 necessaryprefix=1)
{
    if((check == 0) || (*check==0)) {
        return false;
    }
    size_t check_len=strlen(check);
    size_t command_len=strlen(match);
    if(strlen(check) < (size_t)necessaryprefix ||  check_len > command_len) {
        return false;
    }
    for(int i=0; i<(int)check_len; ++i) {
        if(check[i]==0 && i >= necessaryprefix-1) {
            return true;
        }
        if(toupper(check[i]) != toupper(match[i])) {
            return false;
        }
    }
    return true;
}


//----------------------------------------------------------------------
static inline
IFR_Retcode print_syntax_error(IFR_TraceStream& stream,
                               const char *expected,
                               const char *arg)
{
    stream << "Error: Syntax error, found "
           << arg
           << ", expected "
           << expected
           << "."
           << endl;
    return IFR_NOT_OK;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::parseArguments(int argc, char **argv)
{
    if(argc == 1) {
        usage(stderrstream());
        return IFR_NOT_OK;
    }
    int pos = 1;
    while(pos < argc) {
        if(strcmp(argv[pos], "-h") == 0) {
            m_command = C_HELP;
            return IFR_OK;
        }
        if(strcmp(argv[pos], "-v") == 0) {
            m_verbose=true;
            ++pos;
            continue;
        }
        if(strcmp(argv[pos], "-f") == 0) {
            m_force=true;
            ++pos;
            continue;
        }
        if(strcmp(argv[pos], "-p") == 0) {
            ++pos;
            if(pos == argc) {
                stderrstream() << "Error: Option -p requires an argument." << endl;
                return IFR_NOT_OK;
            }
            char *endptr;
            m_pid = (IFR_Int4) strtol(argv[pos], &endptr, 10);
            if(*endptr != 0 || m_pid <= 0) {
                stderrstream() << "Error: Invalid process id specified as argument to -p option." << endl;
                return IFR_NOT_OK;
            }
            m_pidpresent=true;
            ++pos;
            continue;
        } else if(strcmp(argv[pos], "-u") == 0) {
            ++pos;
            if(pos == argc) {
                stderrstream() << "Error: Option -p requires an argument." << endl;
                return IFR_NOT_OK;
            }
            m_user = argv[pos];
            ++pos;
            continue;
        } else if(*argv[pos] == '-') {
            stderrstream() << "Error: Unknown option " << argv[pos] << " given." << endl;
            return IFR_NOT_OK;
        } else {
            break;
        }
    }

    if(pos == argc) {
        return IFR_OK;
    }



#define SYNTAX_ERROR(expected) return print_syntax_error(stderrstream(), expected, argv[pos]);




#define CHECK_END(expected)                                                 \
    if(pos == argc) {                                                       \
        stderrstream() << "Error: Unexpected end of command, expected "     \
                       << expected << "." << endl;                          \
        return IFR_NOT_OK;                                                  \
    }

    if(IFTools_Console_matchCommand(argv[pos], "CONFIG", 1)) {
        ++pos;
        CHECK_END("TRACE");
        if(IFTools_Console_matchCommand(argv[pos], "TRACE", 1)) {
            ++pos;
            CHECK_END("one of SHORT,LONG,PROFILE,PACKET,TIMESTAMP,SIZE,FILENAME,SQL");
            if(IFTools_Console_matchCommand(argv[pos], "SHORT",2)) {
                m_command = C_CONFIG_TRACE_SHORT;
            } else if(IFTools_Console_matchCommand(argv[pos], "LONG",1)) {
                m_command = C_CONFIG_TRACE_LONG;
            } else if(IFTools_Console_matchCommand(argv[pos], "SQL",2)) {
                m_command = C_CONFIG_TRACE_SQL;
            } else if(IFTools_Console_matchCommand(argv[pos], "PACKET",2)) {
                m_command = C_CONFIG_TRACE_PACKET;
            } else if(IFTools_Console_matchCommand(argv[pos], "TIMESTAMP",1)) {
                m_command = C_CONFIG_TRACE_TIMESTAMP;
            } else if(IFTools_Console_matchCommand(argv[pos], "SIZE",2)) {
                m_command = C_CONFIG_TRACE_SIZE;
            } else if(IFTools_Console_matchCommand(argv[pos], "FILENAME",1)) {
                m_command = C_CONFIG_TRACE_FILENAME;
            } else if(IFTools_Console_matchCommand(argv[pos], "COMPRESS",1)) {
                m_command = C_CONFIG_TRACE_COMPRESS;
            } else if(IFTools_Console_matchCommand(argv[pos], "STOP", 2)) {
                m_command = C_CONFIG_TRACE_STOP_ON_ERROR;
            } else {
                SYNTAX_ERROR("one of SHORT,LONG,PROFILE,PACKET,TIMESTAMP,SIZE,FILENAME,SQL,STOP");
            }
            if(m_command == C_CONFIG_TRACE_SHORT
               || m_command == C_CONFIG_TRACE_LONG
               || m_command == C_CONFIG_TRACE_TIMESTAMP
               || m_command == C_CONFIG_TRACE_SQL
               || m_command == C_CONFIG_TRACE_COMPRESS) {
                ++pos;
                CHECK_END("one of ON, OFF");
                if(IFTools_Console_matchCommand(argv[pos], "ON", 2)) {
                    m_on = true;
                } else if(IFTools_Console_matchCommand(argv[pos], "OFF", 2)) {
                    m_on = false;
                } else {
                    SYNTAX_ERROR("one of ON, OFF");
                }
            } else if (m_command == C_CONFIG_TRACE_PACKET) {
                ++pos;
                CHECK_END("one of ON, OFF, <size>");

                if(IFTools_Console_matchCommand(argv[pos], "ON", 2)) {
                    m_on   = true;
                    m_size = 1000;
                } else if(IFTools_Console_matchCommand(argv[pos], "OFF", 2)) {
                    m_on = false;
                } else {
                    if(strtosize(argv[pos], strlen(argv[pos])) > 0) {
                        m_on   = true;
                        m_size = strtosize(argv[pos], strlen(argv[pos]));
                    } else {
                        SYNTAX_ERROR("one of ON, OFF, <size>");
                    }
                }
            } else if (m_command == C_CONFIG_TRACE_SIZE) {
                ++pos;
                CHECK_END("one of UNLIMITED, <size>");

                if(IFTools_Console_matchCommand(argv[pos], "UNLIMITED", 1)) {
                    m_size = -1;
                } else {
                    m_size = strtosize(argv [pos], strlen(argv[pos]));
                    if(m_size < 8192) {
                        stderrstream() << "Error: Syntax error, found "
                                       << argv[pos]
                                       << ", expected UNLIMITED or a number > 8192."
                                       << endl;
                        return IFR_NOT_OK;
                    }
                }

            } else if (m_command == C_CONFIG_TRACE_FILENAME) {
                ++pos;
                CHECK_END("<filename>");
                m_filename=argv[pos];
            } else if (m_command == C_CONFIG_TRACE_STOP_ON_ERROR) {
                ++pos;
                CHECK_END("ON");
                if(!IFTools_Console_matchCommand(argv[pos],"ON", 1)) {
                    SYNTAX_ERROR("ON");
                }
                ++pos;
                CHECK_END("ERROR");
                if(!IFTools_Console_matchCommand(argv[pos],"ERROR", 1)) {
                    SYNTAX_ERROR("ON");
                }
                ++pos;
                CHECK_END("<errorcode> or OFF");

                if(IFTools_Console_matchCommand(argv[pos], "OFF", 1)) {
                    m_stop_on_error = false;
                } else {
                    m_stop_on_error = true;
                    m_errorcode = atoi(argv[pos]);
                    if(m_errorcode == 0) {
                        stderrstream() << "Error: Syntax error, found "
                                       << argv[pos]
                                       << ", expected an error code."
                                       << endl;
                        return IFR_NOT_OK;
                    }
                    ++pos;
                    if(pos != argc) {
                        if(!IFTools_Console_matchCommand(argv[pos],"COUNT",1)) {
                            SYNTAX_ERROR("COUNT");
                        }
                        ++pos;
                        CHECK_END("<errorcount>");
                        m_errorcount = atoi(argv[pos]);
                        if(m_errorcount <= 0) {
                            stderrstream() << "Error: Syntax error, found "
                                           << argv[pos]
                                           << ", expected a positive number."
                                       << endl;
                            return IFR_NOT_OK;
                        }
                    }
                }
            }
        } else {
            SYNTAX_ERROR("TRACE");
        }
    } else if(IFTools_Console_matchCommand(argv[pos], "TRACE", 1)) {
        ++pos;
        CHECK_END("one of SHORT,LONG,PROFILE,PACKET,TIMESTAMP,SIZE,REFRESH,OFF");

        if(IFTools_Console_matchCommand(argv[pos], "REFRESH", 1)) {
            m_command = C_TRACE_REFRESH;
        } else if(IFTools_Console_matchCommand(argv[pos], "SHORT", 2)) {
            m_command = C_TRACE_SHORT;
        } else if(IFTools_Console_matchCommand(argv[pos], "LONG", 1)) {
            m_command = C_TRACE_LONG;
        } else if(IFTools_Console_matchCommand(argv[pos], "PACKET", 2)) {
            m_command = C_TRACE_PACKET;
        } else if(IFTools_Console_matchCommand(argv[pos], "SQL", 2)) {
            m_command = C_TRACE_SQL;
        } else if(IFTools_Console_matchCommand(argv[pos], "SIZE", 2)) {
            m_command = C_TRACE_SIZE;
        } else if(IFTools_Console_matchCommand(argv[pos], "OFF", 1)) {
            m_command = C_TRACE_OFF;
            return IFR_OK;
        } else if(IFTools_Console_matchCommand(argv[pos],"STOP", 2)) {
            m_command = C_TRACE_STOP_ON_ERROR;
        } else {
            SYNTAX_ERROR("one of SHORT,LONG,PROFILE,PACKET,TIMESTAMP,SIZE,REFRESH,OFF,SQL,STOP");
        }

        if(m_command == C_TRACE_SHORT
           || m_command == C_TRACE_LONG
           || m_command == C_TRACE_TIMESTAMP
           || m_command == C_TRACE_SQL) {
            ++pos;
            CHECK_END("one of ON, OFF");
            if(IFTools_Console_matchCommand(argv[pos], "ON", 2)) {
                m_on = true;
            } else if(IFTools_Console_matchCommand(argv[pos], "OFF", 2)) {
                m_on = false;
            } else {
                SYNTAX_ERROR("one of ON, OFF");
            }
        } else if (m_command == C_TRACE_PACKET) {
            ++pos;
            CHECK_END("one of ON, OFF, <size>");

            if(IFTools_Console_matchCommand(argv[pos], "ON", 2)) {
                m_on   = true;
                m_size = 1000;
            } else if(IFTools_Console_matchCommand(argv[pos], "OFF", 2)) {
                m_on = false;
            } else {
                if(atoi(argv[pos]) > 0) {
                    m_on   = true;
                    m_size = atoi(argv[pos]);
                } else {
                    SYNTAX_ERROR("one of ON, OFF, <size>");
                }
            }
        } else if (m_command == C_TRACE_SIZE) {
            ++pos;
            CHECK_END("one of UNLIMITED, <size>");

            if(IFTools_Console_matchCommand(argv[pos], "UNLIMITED", 1)) {
                m_size = -1;
            } else {
                m_size = atoi(argv[pos]);
                if(m_size < 8192) {
                    stderrstream() << "Error: Syntax error, found "
                                   << argv[pos]
                                   << ", expected UNLIMITED or a number > 8192."
                                   << endl;
                    return IFR_NOT_OK;
                }
            }

        } else if(m_command == C_TRACE_STOP_ON_ERROR) {
            ++pos;
            CHECK_END("ON");
            if(!IFTools_Console_matchCommand(argv[pos],"ON", 1)) {
                SYNTAX_ERROR("ON");
                }
            ++pos;
            CHECK_END("ERROR");
            if(!IFTools_Console_matchCommand(argv[pos],"ERROR", 1)) {
                SYNTAX_ERROR("ON");
            }
            ++pos;
            CHECK_END("<errorcode> or OFF");

            if(IFTools_Console_matchCommand(argv[pos], "OFF", 1)) {
                m_stop_on_error = false;
            } else {
                m_stop_on_error = true;
                m_errorcode = atoi(argv[pos]);
                if(m_errorcode == 0) {
                    stderrstream() << "Error: Syntax error, found "
                                   << argv[pos]
                                   << ", expected an error code."
                                   << endl;
                    return IFR_NOT_OK;
                }
                ++pos;
                if(pos != argc) {
                    if(!IFTools_Console_matchCommand(argv[pos],"COUNT",1)) {
                        SYNTAX_ERROR("COUNT");
                    }
                    ++pos;
                    CHECK_END("<errorcount>");
                    m_errorcount = atoi(argv[pos]);
                    if(m_errorcount <= 0) {
                        stderrstream() << "Error: Syntax error, found "
                                       << argv[pos]
                                       << ", expected a positive number."
                                       << endl;
                        return IFR_NOT_OK;
                    }
                }
            }
        }
    } else if(IFTools_Console_matchCommand(argv[pos], "SHOW", 1)) {
        ++pos;
        CHECK_END("one of CONFIG,TRACESETTINGS,ALL");
        if(IFTools_Console_matchCommand(argv[pos], "CONFIG", 1)) {
           m_command = C_SHOW_CONFIG;
           return IFR_OK;
        } else if(IFTools_Console_matchCommand(argv[pos], "TRACESETTINGS", 1)) {
           m_command = C_SHOW_TRACESETTINGS;
           return IFR_OK;
        } else if(IFTools_Console_matchCommand(argv[pos], "FILE", 1)) {
           m_command = C_SHOW_TRACEFILE;
           return IFR_OK;
        } else if(IFTools_Console_matchCommand(argv[pos], "ALL", 1)) {
           m_command = C_SHOW_ALL;
           return IFR_OK;
        } else if(IFTools_Console_matchCommand(argv[pos], "PROFILE", 1)) {
           ++pos;
           if(pos == argc) {                                                       \
              m_command = C_SHOW_PROFILE;
              return IFR_OK;
           } else if(IFTools_Console_matchCommand(argv[pos], "DETAILS", 1)){
              m_command = C_SHOW_PROFILE_DETAIL;
              return IFR_OK;
           } else {
             SYNTAX_ERROR("DETAILS");
           }
        } else {
            SYNTAX_ERROR("one of CONFIG,TRACESETTINGS,ALL");
        }
    } else if(IFTools_Console_matchCommand(argv[pos], "PROFILE",1)) {
        ++pos;
        CHECK_END("one of RESET,COLLECT,FILENAME");
        if(IFTools_Console_matchCommand(argv[pos], "RESET", 1)) {
           m_command = C_PROFILE_RESET;
        } else if(IFTools_Console_matchCommand(argv[pos], "COLLECT", 1)) {
           ++pos;
          CHECK_END("one of ON, OFF");
          if(IFTools_Console_matchCommand(argv[pos], "ON", 2)) {
            m_command = C_PROFILE_COLLECT_ON;
          } else if(IFTools_Console_matchCommand(argv[pos], "OFF", 2)) {
            m_command = C_PROFILE_COLLECT_OFF;
          } else {
           SYNTAX_ERROR("one of ON, OFF");
          }
        } else if(IFTools_Console_matchCommand(argv[pos], "FILENAME", 1)) {
            m_command = C_PROFILE_FILENAME;
            ++pos;
            CHECK_END("<filename>");
            m_filename=argv[pos];
        } else {
           SYNTAX_ERROR("one of RESET,COLLECT,FILENAME");
        }
    } else {
        SYNTAX_ERROR("one of CONFIG,TRACE,PROFILE,SHOW");
    }
    return IFR_OK;
}

//----------------------------------------------------------------------
void
IFTools_Console::usage(IFR_TraceStream &usedstream)
{
    usedstream
//         |- Screen Width ---------------------------------------------------------------|
        << "Usage: sqldbc_cons [options] command"                                << endl
        << "Options:"                                                            << endl
        << "  -h           This help screen."                                    << endl
        << "  -v           Verbose operation."                                   << endl
        << "  -p <pid>     Execute command only for the specified process id."   << endl
        << "  -f           Force tool execution, even if a lock file of another" << endl
        << "               instance is found."                                   << endl
        << "  -u <user>    Perform the operation for another user."              << endl
        << "Options must be separate, collating them is not supported."          << endl
        << endl
        << "Commands:"                                                           << endl
        << "  [CONFIG] TRACE SHORT ON|OFF"                                       << endl
        << "    Enables/disables method call trace."                             << endl
        << "  [CONFIG] TRACE LONG  ON|OFF"                                       << endl
        << "    Enables/disables method argument and detail debug trace."        << endl
        << "  [CONFIG] TRACE PACKET ON|OFF|<size>"                               << endl
        << "    Enables/disables packet trace, limiting the size of the traced"  << endl
        << "    object to the specified number of bytes, or 1000 if no size is"  << endl
        << "    specified."                                                      << endl
        << "  [CONFIG] TRACE SQL ON|OFF"                                         << endl
        << "    Enables/disables high level api trace."                          << endl
        << "  [CONFIG] TRACE TIMESTAMP ON|OFF"                                   << endl
        << "    Enables/disables a timestamp prefix on each line that is traced."<< endl
        << "  [CONFIG] TRACE SIZE <size>"                                        << endl
        << "    Limits the size of the trace file to <size> bytes, at least"     << endl
        << "    8192 bytes are required."                                        << endl
        << "  [CONFIG] TRACE STOP ON ERROR <error>|OFF [COUNT <number>]"         << endl
        << "    Stops tracing after the error <error> has happened <number>"     << endl
        << "    times (default is 1), or switches the trace stop feature off."   << endl
        << "  CONFIG TRACE FILENAME <filename>"                                  << endl
        << "    Sets the name of the trace file name. A '%p' in the name is"     << endl
        << "    replaced by the process id."                                     << endl
        << "  TRACE REFRESH"                                                     << endl
        << "    Refreshes the trace settings from the configuration."            << endl
        << "  TRACE OFF "                                                        << endl
        << "    Switches all trace off."                                         << endl
        << "  PROFILE [RESET|COLLECT ON|COLLECT OFF]"                            << endl
        << "    Resets/Collects profile counter values from a running"           << endl
        << "    sqldbc application."                                             << endl
        << "  PROFILE FILENAME <filename>"                                       << endl
        << "    Sets the name of the profile file name. A '%p' in the name is"   << endl
        << "    replaced by the process id."                                     << endl
        << "  SHOW CONFIG"                                                       << endl
        << "    Displays the configuration."                                     << endl
        << "  SHOW TRACESETTINGS"                                                << endl
        << "    Displays the dynamic trace options."                             << endl
        << "  SHOW ALL"                                                          << endl
        << "    Displays configuration and dynamic trace options."               << endl
        << "  SHOW PROFILE [DETAILS]"                                            << endl
        << "    Displays the application profile retrieved by         "          << endl
        << "    PROFILE COLLECT ON in a aggregated/detailed format."             << endl
        << endl
        << "All commands are case-insensitive, commands can be abbreviated."     << endl
        << "If a process id is specified, the options are valid for the"         << endl
        << "specified process only, and are not used for new processes."         << endl
        << "CONFIG commands affect the configuration only, but not running"      << endl
        << "processes (until a TRACE REFRESH)."                                  << endl;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::showConfig()
{
    IFR_Retcode rc;
    char buffer[1024];
    char errortext[512];
    stdoutstream() << endl << "Configuration:" << endl;
    rc = IFRUtil_Configuration::getTraceFileName(m_user, buffer, 1024, errortext, 512);
    if(rc == IFR_OK) {
        stdoutstream() << "  Trace file name    : " << buffer << endl;
    } else if(rc == IFR_DATA_TRUNC) {
        stdoutstream() << "  Trace file name    : " << inputlength(1024) << buffer << endl
                       << "  Warning: Trace file name was truncated, tracing is disabled." << endl;
    } else {
        stderrstream() << "Error getting trace file name (" << errortext << ")." << endl;
        return IFR_NOT_OK;
    }
    rc = IFRUtil_Configuration::getProfileFileName(m_user, buffer, 1024, errortext, 512);
    if(rc == IFR_OK) {
        stdoutstream() << "  Profile file name  : " << buffer << endl;
    } else if(rc == IFR_DATA_TRUNC) {
        stdoutstream() << "  Profile file name  : " << inputlength(1024) << buffer << endl
                       << "  Warning: Profile file name was truncated, output of profile data is disabled." << endl;
    } else {
        stderrstream() << "Error getting profile file name (" << errortext << ")." << endl;
        return IFR_NOT_OK;
    }
    rc = IFRUtil_Configuration::getTraceFlags(m_user, buffer, 1024, errortext, 512);
    if(rc == IFR_OK) {
        stdoutstream() << "  Trace flags        : " << (strlen(buffer)?buffer:"(none set)") << endl;
        if(strlen(buffer)) {
            TraceFlags flags(buffer);
            stdoutstream() << "    Short trace        : " << (flags.m_call ? "enabled" : "disabled") << endl;
            stdoutstream() << "    Long trace         : " << (flags.m_debug ? "enabled" : "disabled") << endl;
            stdoutstream() << "    Profile trace      : " << ((flags.m_profile == IFR_TraceProfileAction_RESET)? "reset counters" : ((flags.m_profile == IFR_TraceProfileAction_COLLECT)? "collect counters":"enable")) << endl;
            stdoutstream() << "    SQL trace          : " << (flags.m_sql ? "enabled" : "disabled") << endl;
            stdoutstream() << "    Time stamp prefix  : " << (flags.m_timestamp ? "enabled" : "disabled") << endl;
            if(flags.m_packet) {
                stdoutstream() << "    Packet trace       : enabled, size limit is " << flags.m_packetsize << endl;
            } else {
                stdoutstream() << "    Packet trace       : disabled" << endl;
            }
            if(flags.m_filesize == -1) {
                stdoutstream() << "    Trace file size    : not limited" << endl;
            } else {
                stdoutstream() << "    Trace file size    : limited to " << flags.m_filesize << " bytes"<< endl;
            }
            if(flags.m_stop_on_error) {
                IFR_Int4 errorcount = flags.m_errorcount>0?flags.m_errorcount:1;
                stdoutstream() << "    Stop on error      : " << flags.m_errorcode << " after " <<
                    errorcount << ((errorcount>1)?" occurrences":" occurence") << endl;
            } else {
                stdoutstream() << "    Stop on error      : disabled." << endl;
            }
        }
    } else {
        stderrstream() << "Error getting trace flags (" << errortext << ")." << endl;
        return IFR_NOT_OK;
    }

    rc = IFRUtil_Configuration::getSharedMemoryName(m_user, buffer, 1024, errortext, 512);
    if(rc == IFR_OK) {
        stdoutstream() << "  Shared memory name : " << buffer << endl;
    } else if(rc == IFR_DATA_TRUNC) {
        stdoutstream() << "  Shared memory name : " << inputlength(1024) << buffer << endl
                       << "  Warning: Shared memory name was truncated, online update of trace flags is disabled." << endl;
    } else {
        stderrstream() << "Error getting shared memory name (" << errortext << ")." << endl;
        return IFR_NOT_OK;
    }

    // Now tell something about file locations
    RTE_Path           fileLocation;
    tsp00_ErrTextc     errTextc;

    if(RTE_GetUserSpecificConfigPath(fileLocation, false, errTextc)) {
        stdoutstream() << "  Files will be created in path "
                       << fileLocation << ","
                       << " unless specified as absolute paths."
                       << endl;
    } else {
        stderrstream() << "Error getting path for trace file and shared memory: "
                       << errTextc.asCharp() << ", " << endl
                       << "tracing will be disabled." << endl;
    }
    return IFR_OK;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::showTracesettings()
{
    IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
    IFRUtil_TraceSharedMemory sharedMemory;

    sharedMemory.attach(sharedMemoryError);
    if(sharedMemoryError) {
        stderrstream() << "Error opening shared memory: "
                       << sharedMemoryError.getErrorText()
                       << ", aborting." << endl;
        return IFR_NOT_OK;
    }
    IFR_Int4 partcount = (sharedMemory.getHeader()->totalSize -
                          SHAREDMEM_HEADER_SIZE)/sizeof(IFRUtil_TraceSharedMemory::Part);
    if(sharedMemory.getHeader()->totalSize == 0) {
        partcount=0;
    }

    stdoutstream() << endl << "Settings:" << endl;
    stdoutstream() << "  Update count : " << sharedMemory.getHeader()->updateCount << endl;
    stdoutstream() << "  Total size   : " << sharedMemory.getHeader()->totalSize << endl;
    stdoutstream() << "    equivalent to " << partcount << " process-specific parts." << endl;
    stdoutstream() << "  Version flag : " << sharedMemory.getHeader()->version << endl;
    if(sharedMemory.getHeader()->globalReRead != 0) {
        stdoutstream() << "  Forced re-read of global configuration with last update." << endl;
    }
    stdoutstream() << endl;
    IFRUtil_TraceSharedMemory::Part *parts=sharedMemory.getParts();
    if(partcount) {
        stdoutstream() <<   "  Process  Update count  Flags" << endl;
        stdoutstream() <<   "  ----------------------------------------------------------------------" << endl;
    }

    for(IFR_Int4 i=0; i<partcount; ++i) {
        char buffer[1024];
        sprintf(buffer, "    %5.d         %5.d  %s",
                parts[i].processID,
                parts[i].readCount,
                parts[i].flags);
        stdoutstream() << buffer << endl;
    }
    return IFR_OK;
}

//----------------------------------------------------------------------
static IFR_Bool matchFilePattern (const char* pattern, const char* file){
  IFR_Int4 filePos=0, patLen = strlen(pattern), fileLen = strlen (file);
  for (int i = 0; i < patLen; i++){
    if (i+1<patLen && pattern[i]=='%' && pattern[i+1]=='p'){
      while (filePos <= fileLen  && isdigit(file[filePos])){
        filePos++; //skip pid
      }
      i++;
      continue;
    } else if (filePos <= fileLen && pattern[i] == file[filePos]){
      filePos++;
      continue;
    }
    return IFR_FALSE;
  }
  return IFR_TRUE;
}

struct ProfileContainer{
  char m_key[50];
  IFR_UInt8 m_val;
};

IFR_Retcode
IFTools_Console::showProfile(IFR_Bool showDetails)
{
    IFR_Retcode rc;
    char buffer[1024];
    char errortext[512];
    char tmpBuffer[50];
    IFR_Bool foundProfileData = IFR_FALSE;
    IFRUtil_Vector<ProfileContainer*> vec (RTEMem_RteAllocator::Instance());

    rc = IFRUtil_Configuration::getFullProfileFileName(m_user, buffer, sizeof(buffer), errortext, sizeof(errortext));
    if(rc == IFR_DATA_TRUNC) {
        stdoutstream() << "Error: Profile file name was truncated, output of profile data is not possible." << endl;
        return IFR_NOT_OK;
    } else if (rc != IFR_OK) {
        stderrstream() << "Error getting profile file name (" << errortext << ")." << endl;
        return IFR_NOT_OK;
    }
#ifdef WIN32
    char *pathEndPos = strrchr(buffer, '\\');
#else
    char *pathEndPos = strrchr(buffer, '/');
#endif
    if (! pathEndPos){
        stdoutstream() << "Error: wrong profile file name, output of profile data is not possible." << endl;
        return IFR_NOT_OK;
    }

    char *strippedFileName = (char*) alloca(strlen(buffer)-(pathEndPos+1-buffer));
    strncpy (strippedFileName,&buffer[pathEndPos+1-buffer],  strlen(buffer)-(pathEndPos-buffer));
    buffer[pathEndPos-buffer]= 0;

    if (m_verbose){
      stdoutstream() << endl << "search path:" << buffer << endl;
      stdoutstream() << endl << "search name pattern:" << strippedFileName << endl;
    }

    RTEIO_DirectoryIterator iter = RTEIO_DirectoryIterator (buffer);

    while (iter.next()){
      if (! iter.currentIsDir()){
        if (m_verbose) stdoutstream() << "analyze file:" << iter.currentName();
        if (matchFilePattern(strippedFileName, iter.currentName())){
          if (m_verbose) stdoutstream() << " pattern does match" << endl;

          foundProfileData = IFR_TRUE;
          if (showDetails){
            stdoutstream() << endl << "Profile: " << iter.currentName() << endl;
            stdoutstream() << "| profile counter             | value                |" << endl;
            stdoutstream() << "| --------------------------- | -------------------- |" << endl;
          }
          IFTools_FileIterator fi = IFTools_FileIterator(iter.currentFullpath());
          while (fi.next()){
            if (showDetails){
              sprintf(tmpBuffer,"| %-27s | %20.*s |", fi.currentKey(), strlen(fi.currentValue()),fi.currentValue());
              stdoutstream() << tmpBuffer << endl;
            } else {
#ifdef _WIN32
              IFR_UInt8 currentVal = _atoi64(fi.currentValue());
#elif defined(LINUX)
              IFR_UInt8 currentVal = atoll(fi.currentValue());
#else
              IFR_UInt8 currentVal = atol(fi.currentValue());
#endif

              IFR_Bool keyFound = IFR_FALSE;
              for (IFR_size_t vecpos=0; vecpos < vec.GetSize(); vecpos++){
                ProfileContainer *elem = (ProfileContainer*) vec[vecpos];
                if (0 == strncmp (elem->m_key, fi.currentKey(), strlen(elem->m_key))){
                  //update
//                  stdoutstream() << "found in vector: " << elem->m_key << endl;
                  elem->m_val+=currentVal;
                  keyFound = IFR_TRUE;
                  break;
                }
              }
              if (! keyFound){
                  //insert
//                  stdoutstream() << "add to vector: " << newProf->m_key << endl;
                  IFR_Bool memok = IFR_TRUE;
                  ProfileContainer *newProf = (ProfileContainer *) RTEMem_RteAllocator::Instance().Allocate(sizeof(ProfileContainer));
                  strcpy (newProf->m_key,fi.currentKey());
                  newProf->m_val=currentVal;
                  vec.InsertEnd(newProf, memok);
              }
            }
          }
        } else {
          if (m_verbose) stdoutstream() << " pattern does not match" << endl;
        }
      }
    }

    if (! showDetails && foundProfileData){
      //print summary
      stdoutstream() << endl << "Profile summary:" << endl;
      stdoutstream() << "| profile counter             | value                |" << endl;
      stdoutstream() << "| --------------------------- | -------------------- |" << endl;

      while (vec.GetSize()){
        IFRUtil_Vector<ProfileContainer*>::Iterator iter = vec.Begin();
        ProfileContainer *elem =  *iter;
        vec.Delete(iter);

#ifdef _WIN32
        sprintf(tmpBuffer,"| %-27s | %20I64u |", elem->m_key, elem->m_val);
#elif defined(LINUX)
        sprintf(tmpBuffer,"| %-27s | %20llu |", elem->m_key, elem->m_val);
#else
        sprintf(tmpBuffer,"| %-27s | %20lu |", elem->m_key, elem->m_val);
#endif

        stdoutstream() << tmpBuffer << endl;
        RTEMem_RteAllocator::Instance().Deallocate(elem);
      }
    }
    if (! foundProfileData){
      stdoutstream() << "no profile data found." << endl;
    }
    return IFR_OK;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configTraceShort()
{
#define GETFLAGS                                                        \
    char buffer[1024];                                                  \
    char errtext[512];                                                  \
    IFR_Retcode rc=IFRUtil_Configuration::getTraceFlags(m_user, buffer, 1024, errtext, 512); \
    if(rc != IFR_OK) {                                                  \
        if(rc == IFR_NOT_OK) {                                          \
            stderrstream() << "Error getting trace flags (" << errtext << ")." << endl; \
        }                                                               \
        return IFR_NOT_OK;                                              \
    }

    GETFLAGS;

    TraceFlags flags(buffer);
    flags.m_call = m_on;
    flags.toString(buffer);

#define PUTFLAGS                                                        \
    rc=IFRUtil_Configuration::setTraceFlags(m_user, buffer, errtext, 512); \
    if(rc == IFR_NOT_OK) {                                              \
        stderrstream() << "Error setting trace flags (" << errtext << ")." << endl; \
    }                                                                   \
    return rc

    PUTFLAGS;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configTraceLong()
{
    GETFLAGS;
    TraceFlags flags(buffer);
    flags.m_debug = m_on;
    flags.toString(buffer);
    PUTFLAGS;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configTraceCompress()
{
    GETFLAGS;
    TraceFlags flags(buffer);
    flags.m_compressed = m_on;
    flags.toString(buffer);
    PUTFLAGS;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configTraceTimestamp()
{
    GETFLAGS;
    TraceFlags flags(buffer);
    flags.m_timestamp = m_on;
    flags.toString(buffer);
    PUTFLAGS;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configTraceProfile(IFRUtil_TraceSharedMemory_TraceProfileAction type)
{
    GETFLAGS;
    TraceFlags flags(buffer);
    flags.m_profile = type;
    flags.toString(buffer);
    PUTFLAGS;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configTraceSQL()
{
    GETFLAGS;
    TraceFlags flags(buffer);
    flags.m_sql = m_on;
    flags.toString(buffer);
    PUTFLAGS;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configTracePacket()
{
    GETFLAGS;
    TraceFlags flags(buffer);
    if(m_on) {
        flags.m_packet     = true;
        flags.m_packetsize = m_size;
    } else {
        flags.m_packet     = false;
        flags.m_packetsize = m_size;
    }
    flags.toString(buffer);
    PUTFLAGS;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configTraceSize()
{
    GETFLAGS;
    TraceFlags flags(buffer);
    flags.m_filesize = m_size;
    flags.toString(buffer);
    PUTFLAGS;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configTraceFilename()
{
    char errortext[512];
    IFR_Retcode rc= IFRUtil_Configuration::setTraceFileName(m_user, m_filename, errortext, 512);
    if(rc == IFR_NOT_OK) {
        stderrstream() << "Error setting trace file name (" << errortext << ")" << endl;
    }
    return rc;
}
//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configProfileFilename()
{
    char errortext[512];
    IFR_Retcode rc= IFRUtil_Configuration::setProfileFileName(m_user, m_filename, errortext, 512);
    if(rc == IFR_NOT_OK) {
        stderrstream() << "Error setting trace file name (" << errortext << ")" << endl;
    }
    return rc;
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::configTraceStopOnError()
{
    GETFLAGS;
    TraceFlags flags(buffer);
    flags.m_stop_on_error = m_stop_on_error;
    flags.m_errorcode     = m_errorcode;
    flags.m_errorcount    = m_errorcount;
    flags.toString(buffer);
    PUTFLAGS;
}


//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::traceRefresh()
{
    if(m_pidpresent) {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }
        sharedMemory.refreshConfig(m_pid);
        return IFR_OK;
    } else {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }
        sharedMemory.update(true);
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::traceStopOnError()
{
    if(m_pidpresent) {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }

        IFRUtil_TraceSharedMemory::Part *p     = sharedMemory.getPart(m_pid);
        char buffer[256];

        if(p == 0) {
            p = sharedMemory.getPart(m_pid, true);
            char errortext[512];
            if(IFRUtil_Configuration::getTraceFlags(m_user, buffer, 256, errortext, 512) != IFR_OK) {
                stderrstream() << "Error getting configuration default ("<< errortext << ")." << endl;
                return IFR_NOT_OK;
            }
        } else {
            memcpy(buffer, p->flags, 256);
        }

        TraceFlags tf(buffer);

        tf.m_stop_on_error = m_stop_on_error;
        tf.m_errorcode = m_errorcode;
        tf.m_errorcount = m_errorcount;

        tf.toString(buffer);
        if(!sharedMemory.setFlags(m_pid, buffer)) {
            stderrstream() << "Error setting trace flags." << endl;
            return IFR_NOT_OK;
        }
        return IFR_OK;
    } else {
        IFR_Retcode rc=configTraceStopOnError();
        if(rc == IFR_OK) {
            rc = traceRefresh();
        }
        return rc;
    }
}


//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::traceShort()
{
    if(m_pidpresent) {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }

        IFRUtil_TraceSharedMemory::Part *p     = sharedMemory.getPart(m_pid);
        char buffer[256];

        if(p == 0) {
            p = sharedMemory.getPart(m_pid, true);
            char errortext[512];
            if(IFRUtil_Configuration::getTraceFlags(m_user, buffer, 256, errortext, 512) != IFR_OK) {
                stderrstream() << "Error getting configuration default (" << errortext << ")." << endl;
                return IFR_NOT_OK;
            }
        } else {
            memcpy(buffer, p->flags, 256);
        }

        TraceFlags tf(buffer);

        // traceShort

        tf.m_call = m_on;

        tf.toString(buffer);
        if(!sharedMemory.setFlags(m_pid, buffer)) {
            stderrstream() << "Error setting trace flags." << endl;
            return IFR_NOT_OK;
        }
        return IFR_OK;
    } else {
        IFR_Retcode rc=configTraceShort();
        if(rc == IFR_OK) {
            rc = traceRefresh();
        }
        return rc;
    }

}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::traceLong()
{
    if(m_pidpresent) {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }

        IFRUtil_TraceSharedMemory::Part *p     = sharedMemory.getPart(m_pid);
        char buffer[256];

        if(p == 0) {
            char errortext[512];
            p = sharedMemory.getPart(m_pid, true);
            if(IFRUtil_Configuration::getTraceFlags(m_user, buffer, 256, errortext, 512) != IFR_OK) {
                stderrstream() << "Error getting configuration default ("<< errortext << ")." << endl;
                return IFR_NOT_OK;
            }
        } else {
            memcpy(buffer, p->flags, 256);
        }

        TraceFlags tf(buffer);

        // traceLong

        tf.m_debug = m_on;

        tf.toString(buffer);
        if(!sharedMemory.setFlags(m_pid, buffer)) {
            stderrstream() << "Error setting trace flags." << endl;
            return IFR_NOT_OK;
        }
        return IFR_OK;
    } else {
        IFR_Retcode rc=configTraceLong();
        if(rc == IFR_OK) {
            rc = traceRefresh();
        }
        return rc;
    }

}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::traceProfile(IFRUtil_TraceSharedMemory_TraceProfileAction type)
{
    if(m_pidpresent) {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }

        IFRUtil_TraceSharedMemory::Part *p     = sharedMemory.getPart(m_pid);
        char buffer[256];

        if(p == 0) {
            p = sharedMemory.getPart(m_pid, true);
            char errortext[512];
            if(IFRUtil_Configuration::getTraceFlags(m_user, buffer, 256, errortext, 512) != IFR_OK) {
                stderrstream() << "Error getting configuration default (" << errortext << ")." << endl;
                return IFR_NOT_OK;
            }
        } else {
            memcpy(buffer, p->flags, 256);
        }

        TraceFlags tf(buffer);


        tf.m_profile = type;


        tf.toString(buffer);
        if(!sharedMemory.setFlags(m_pid, buffer)) {
            stderrstream() << "Error setting trace flags." << endl;
            return IFR_NOT_OK;
        }
        return IFR_OK;
    } else {
        IFR_Retcode rc=configTraceProfile(type);
        if(rc == IFR_OK) {
            rc = traceRefresh();
        }
        return rc;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::traceSQL()
{
    if(m_pidpresent) {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }

        IFRUtil_TraceSharedMemory::Part *p = sharedMemory.getPart(m_pid);
        char buffer[256];

        if(p == 0) {
            p = sharedMemory.getPart(m_pid, true);
            char errortext[512];
            if(IFRUtil_Configuration::getTraceFlags(m_user, buffer, 256, errortext, 512) != IFR_OK) {
                stderrstream() << "Error getting configuration default (" << errortext << ")." << endl;
                return IFR_NOT_OK;
            }
        } else {
            memcpy(buffer, p->flags, 256);
        }

        TraceFlags tf(buffer);


        tf.m_sql = m_on;

        tf.toString(buffer);
        if(!sharedMemory.setFlags(m_pid, buffer)) {
            stderrstream() << "Error setting trace flags." << endl;
            return IFR_NOT_OK;
        }
        return IFR_OK;
    } else {
        IFR_Retcode rc=configTraceSQL();
        if(rc == IFR_OK) {
            rc = traceRefresh();
        }
        return rc;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::traceTimestamp()
{
    if(m_pidpresent) {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }

        IFRUtil_TraceSharedMemory::Part *p = sharedMemory.getPart(m_pid);
        char buffer[256];

        if(p == 0) {
            p = sharedMemory.getPart(m_pid, true);
            char errortext[512];
            if(IFRUtil_Configuration::getTraceFlags(m_user, buffer, 256, errortext, 512) != IFR_OK) {
                stderrstream() << "Error getting configuration default (" << errortext << ")." << endl;
                return IFR_NOT_OK;
            }
        } else {
            memcpy(buffer, p->flags, 256);
        }

        TraceFlags tf(buffer);

        // traceShort

        tf.m_timestamp = m_on;

        tf.toString(buffer);
        if(!sharedMemory.setFlags(m_pid, buffer)) {
            stderrstream() << "Error setting trace flags." << endl;
            return IFR_NOT_OK;
        }
        return IFR_OK;
    } else {
        IFR_Retcode rc=configTraceTimestamp();
        if(rc == IFR_OK) {
            rc = traceRefresh();
        }
        return rc;
    }

}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::tracePacket()
{
    if(m_pidpresent) {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }

        IFRUtil_TraceSharedMemory::Part *p = sharedMemory.getPart(m_pid);
        char buffer[256];

        if(p == 0) {
            p = sharedMemory.getPart(m_pid, true);
            char errortext[512];
            if(IFRUtil_Configuration::getTraceFlags(m_user, buffer, 256, errortext, 512) != IFR_OK) {
                stderrstream() << "Error getting configuration default (" << errortext << ")." << endl;
                return IFR_NOT_OK;
            }
        } else {
            memcpy(buffer, p->flags, 256);
        }

        TraceFlags tf(buffer);

        if(m_on) {
            tf.m_packet     = true;
            tf.m_packetsize = m_size;
        } else {
            tf.m_packet     = false;
            tf.m_packetsize = m_size;
        }

        tf.toString(buffer);
        if(!sharedMemory.setFlags(m_pid, buffer)) {
            stderrstream() << "Error setting trace flags." << endl;
            return IFR_NOT_OK;
        }
        return IFR_OK;
    } else {
        IFR_Retcode rc=configTracePacket();
        if(rc == IFR_OK) {
            rc = traceRefresh();
        }
        return rc;
    }

}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::traceSize()
{
    if(m_pidpresent) {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }

        IFRUtil_TraceSharedMemory::Part *p     = sharedMemory.getPart(m_pid);
        char buffer[256];

        if(p == 0) {
            p = sharedMemory.getPart(m_pid, true);
            char errortext[512];
            if(IFRUtil_Configuration::getTraceFlags(m_user, buffer, 256, errortext, 512) != IFR_OK) {
                stderrstream() << "Error getting configuration default (" << errortext << ")." << endl;
                return IFR_NOT_OK;
            }
        } else {
            memcpy(buffer, p->flags, 256);
        }

        TraceFlags tf(buffer);

        tf.m_filesize = m_size;

        tf.toString(buffer);
        if(!sharedMemory.setFlags(m_pid, buffer)) {
            stderrstream() << "Error setting trace flags." << endl;
            return IFR_NOT_OK;
        }
        return IFR_OK;
    } else {
        IFR_Retcode rc=configTraceSize();
        if(rc == IFR_OK) {
            rc = traceRefresh();
        }
        return rc;
    }

}

//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::traceOff()
{
    if(m_pidpresent) {
        IFR_ErrorHndl sharedMemoryError(RTEMem_RteAllocator::Instance());
        IFRUtil_TraceSharedMemory sharedMemory;
        sharedMemory.attach(sharedMemoryError);
        if(sharedMemoryError) {
            stderrstream() << "Error opening shared memory: "
                           << sharedMemoryError.getErrorText()
                           << ", aborting." << endl;
            return IFR_NOT_OK;
        }

        IFRUtil_TraceSharedMemory::Part *p     = sharedMemory.getPart(m_pid);
        if(p == 0) {
            p = sharedMemory.getPart(m_pid, true);
        }
        if(!sharedMemory.setFlags(m_pid, (char*)"")) {
            stderrstream() << "Error setting trace flags." << endl;
            return IFR_NOT_OK;
        }
        return IFR_OK;
    } else {
        char errortext[512];
        IFR_Retcode rc=IFRUtil_Configuration::setTraceFlags(m_user, (char*)"", errortext, 512);
        if(rc == IFR_OK) {
            rc = traceRefresh();
        } else {
            stderrstream() << "Failed to write trace configuration (" << errortext << ")." << endl;
        }
        return rc;
    }

}


//----------------------------------------------------------------------
IFR_Retcode
IFTools_Console::run()
{
    // the harmless commands
    switch(m_command) {
    case C_NONE:
        stderrstream() << "Error: No command given, use the -h option for an overview " << endl
                       << "on commands and options." << endl;
        return IFR_NOT_OK;
    case C_HELP:
        usage(stdoutstream());
        return IFR_OK;
    case C_SHOW_CONFIG:
        return showConfig();
    case C_SHOW_TRACESETTINGS:
        return showTracesettings();
    case C_SHOW_ALL: {
        IFR_Retcode rc=showConfig();
        if(rc == IFR_OK) {
            rc=showTracesettings();
        }
        return rc;
    }
    case C_SHOW_PROFILE:
       return showProfile(IFR_FALSE);
    case C_SHOW_PROFILE_DETAIL:
       return showProfile(IFR_TRUE);
    case C_CONFIG_TRACE_SHORT:
        return configTraceShort();
    case C_CONFIG_TRACE_LONG:
        return configTraceLong();
    case C_CONFIG_TRACE_TIMESTAMP:
        return configTraceTimestamp();
    case C_CONFIG_TRACE_SIZE:
        return configTraceSize();
    case C_CONFIG_TRACE_PACKET:
        return configTracePacket();
    case C_CONFIG_TRACE_FILENAME:
        return configTraceFilename();
    case C_PROFILE_FILENAME:
        return configProfileFilename();
    case C_CONFIG_TRACE_SQL:
        return configTraceSQL();
    case C_CONFIG_TRACE_STOP_ON_ERROR:
        return configTraceStopOnError();
    case C_CONFIG_TRACE_COMPRESS:
        return configTraceCompress();
    case C_TRACE_REFRESH:
        return traceRefresh();
    case C_TRACE_SHORT:
        return traceShort();
    case C_TRACE_LONG:
        return traceLong();
    case C_TRACE_TIMESTAMP:
        return traceTimestamp();
    case C_PROFILE_COLLECT_ON:
        return traceProfile(IFR_TraceProfileAction_COLLECT);
    case C_PROFILE_COLLECT_OFF:
        return traceProfile(IFR_TraceProfileAction_NOTSET);
    case C_PROFILE_RESET:
        return traceProfile(IFR_TraceProfileAction_RESET);
    case C_TRACE_SIZE:
        return traceSize();
    case C_TRACE_PACKET:
        return tracePacket();
    case C_TRACE_SQL:
        return traceSQL();
    case C_TRACE_STOP_ON_ERROR:
        return traceStopOnError();
    case C_TRACE_OFF:
        return traceOff();
    }
    return IFR_NOT_OK;
}



//----------------------------------------------------------------------
int main(int argc, char **argv)
{
    IFTools_Console console;
    IFR_Retcode rc = console.parseArguments(argc, argv);
    if(rc == IFR_OK) {
        rc = console.run();
    }
    return rc==IFR_OK ? 0 : 1;
}
