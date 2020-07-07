/* 
  -----------------------------------------------------------------------------
 
  module: vls99.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: SAPDB Loader
 
  description:  trace

  version:      7.6.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1997-2005 SAP AG-2004
 
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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




*/


#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "gls00.h"
#include "hls98msg.h"
#include "hls99.h"
#include "heo01.h"

// Initialize static class members
bool         tls99_TraceClass::bTraceFileInit_ls99   = false;
char        *tls99_TraceClass::m_pszTraceFileName    = NULL;
tsp00_Int2   tls99_TraceClass::lSubCount             = 0;
tsp00_Int4   tls99_TraceClass::m_TraceFileHandle     = 0;


char const *tls99_TraceClass::pszPartKindName[25] = {"not used",
                                                     "appl_parameter_description",
                                                     "columnnames",
                                                     "command",
                                                     "conv_tables_returned",
                                                     "data",
                                                     "errortext",
                                                     "getinfo",
                                                     "modulname",
                                                     "page",
                                                     "parsid",
                                                     "parsid_of_select",
                                                     "resultcount",
                                                     "resulttablename",
                                                     "shortinfo",
                                                     "user_info_returned",
                                                     "surrogate",
                                                     "bdinfo",
                                                     "longdata",
                                                     "tablename",
                                                     "session_info_returned",
                                                     "output_cols_no_parameter",
                                                     "key",
                                                     "serial",
                                                     "relative_pos"};

char const *tls99_TraceClass::pszSQLModeName[6] = {"NOT USED", "SESSION", "INTERNAL",
                                                   "ANSI",     "DB2",     "ORACLE"};

char const *tls99_TraceClass::pszDataTypeName[39] = {"fixed   \t", "float   \t", "char asc\t", "char ebc\t",
                                                     "char byt\t", "rowid   \t", "long asc\t", "long ebc\t",
                                                     "long byt\t", "9\t\t",      "date    \t", "time    \t",
                                                     "12\t\t",     "timestmp\t", "14\t\t",     "15\t\t",
                                                     "16\t\t",     "17\t\t",     "18\t\t",     "19\t\t",
                                                     "20\t\t",     "21\t\t",     "22\t\t",     "boolean \t",
                                                     "char uni\t", "25\t\t",     "26\t\t",     "27\t\t",
                                                     "28\t\t",     "smallint\t", "integer \t", "vchr asc\t",
                                                     "vchr ebc\t", "vchr byt\t", "long uni\t", "35\t\t",
                                                     "vchr uni\t", "37\t\t",     "38\t\t"};


// tsp1_producer_Enum
char const *tls99_TraceClass::pszProducer[5] = {"unknown","user","internal","kernel","installation"};

char const *tls99_TraceClass::pszLengthType[3] = {"fix   \t","var   \t","var(l)\t"};

char const *tls99_TraceClass::pszColumnMode[5] = {"skey  \t","key   \t","man   \t","opt   \t","drop   \t"};

char const *tls99_TraceClass::pszDefaultType[12] = {"no def\t","user  \t","group \t","date  \t","time  \t"
                                                    "stamp \t","timest\t","uid   \t","true  \t","false \t",
                                                    "other \t","serial\t"};


/*
  -----------------------------------------------------------------------------
  Chapter: Private functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     ls99_TraceIsOn
  -----------------------------------------------------------------------------
*/
bool tls99_TraceClass::ls99_TraceIsOn (const char Method)
{
    if (Method != 0x00)
    {
        return false;
    }
    else
    {
        return bTraceFileInit_ls99;
    }
}
// ls99_TraceIsOn()


/*
  -----------------------------------------------------------------------------
  function:     ls99_TraceBuf
  -----------------------------------------------------------------------------
*/
void tls99_TraceClass::ls99_TraceBuf(tsp00_Int4 TraceBufLen, tsp00_Addr TraceBuf)
{
    assert(NULL != TraceBuf);

    if (0 == TraceBufLen)
    {
        return;
    }

    tsp00_Int4 BufLine = 0;
    tsp00_Int4 j       = 0;
    tsp00_Int4 k       = 0;

    tsp00_Int4 strCount = 0;
    tsp00_Int4 tmpCount = 0;        // must be a signed because of return code of sp77sprintf

    //*
    //*     Allocate write buffer
    //*
    //* size: 1. every char can be represented as hex (i.e. using 2 bytes)
    //*       2. every row begins with 2 tabs (= 2 * count of rows)
    //*       3. every 8th char to write we insert a blank (= 7 * count of rows)
    //*       4. every column is finished with a \n (= 1* count of rows)
    //*       5. we have a trailing \0;
    //*
    tsp00_Int4 lBufLen = 2*TraceBufLen + (10 * ((TraceBufLen + 31)/32)) + 1;
    char *pszMyBuffer = new char[lBufLen];
    
    //*
    //*     Write trace hex
    //*
    k=0;
    while ( ((k*32) < TraceBufLen) && (tmpCount >= 0) )
    {
        BufLine = 32;           
        if ((TraceBufLen - (k*32)) < BufLine)
        {
            BufLine = TraceBufLen - (k*32);
        }


        pszMyBuffer[strCount] = pszMyBuffer[strCount + 1] = '\t';
        strCount += 2*sizeof(char);
        for (j=0; ( (j < BufLine) && (tmpCount >= 0) ); ++j)
        {
            if ((j != 0) && ((j % 4) == 0))
            {
                pszMyBuffer[strCount] = BLANK_LS00;
                strCount += sizeof(char);
            }
            strCount += (tmpCount = sp77sprintf(pszMyBuffer + strCount,
                                                lBufLen - strCount,
                                                "%.2X", STATIC_CAST(unsigned char, TraceBuf[j + (k*32)]) ) );
        }

        // In case of error (writing over string limits) sp77sprintf returns the negative count of
        // sucessfully converted parameters; so strCount is decreased by this value and we have some
        // space to insert the 0 and stop writing the buffer
        if (tmpCount < 0)
        {
            pszMyBuffer[strCount] = 0;
        }
        else
        {
            pszMyBuffer[strCount] = '\n';
            strCount += sizeof(char);
            ++k;
        }

    }
    pszMyBuffer[strCount-1] = 0;                  // trailing 0
    ls99_Write(pszMyBuffer);      // Ausgeben in die datei

    //*
    //*     Write trace as CHAR
    //*
    pszMyBuffer[0]  = 0;
    strCount        = 0;
    k               = 0; 
    size_t i        = 0;
    while ((k*64) < TraceBufLen) 
    {
        BufLine = 64;           
        if ((TraceBufLen - (k*64)) < BufLine)
        {
            BufLine = TraceBufLen - (k*64);
        }

        pszMyBuffer[0] = pszMyBuffer[1] = '\t';
        for (j=0, i=(2+(k*64)); j < BufLine; ++j, ++i)
        {
            if (STATIC_CAST(unsigned char, TraceBuf[j + (k*64)]) < STATIC_CAST(char, 0x20) )
            {
                pszMyBuffer[i] = '.';
            }
            else
            {
                pszMyBuffer[i] = TraceBuf[j + (k*64)];
            }
        }
        pszMyBuffer[i] = '\n';
        ++k;
    }
    pszMyBuffer[i] = 0;        // remove trailing new line
    ls99_Write(pszMyBuffer);

    delete [] pszMyBuffer;
    pszMyBuffer = NULL;

}
// ls99_TraceBuf()

/*  
  -----------------------------------------------------------------------------
  EndChapter: Private functions implementation
  -----------------------------------------------------------------------------
*/


/*  
  -----------------------------------------------------------------------------
  Chapter: Public functions implementation
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  function:     Constructor
  -----------------------------------------------------------------------------
*/
tls99_TraceClass::tls99_TraceClass (const char *Param)
{
    RoutineName_ls99 = Param;

    if ((RoutineName_ls99 != NULL) && ls99_TraceIsOn())
    {
        m_szTraceText[0] = 0;
        sp77sprintf(m_szTraceText, BUF_SIZE_LS99, "%*s-> %s", lSubCount, " ", RoutineName_ls99);
        ls99_Write();
    }
    lSubCount++;
}
// tls99_TraceClass()


// Der destruktor laesst die tracedatei offen, da er staendig (beim verlassen einer jeden
// funktion, in dem die trace-klasse aufgerufen wird) aufgerufen wird. Die datei wird erst
// ganz zum schluss des servers geschlossen mit ls99Ende.
/*
  -----------------------------------------------------------------------------
  function:     Destructor
  -----------------------------------------------------------------------------
*/
tls99_TraceClass::~tls99_TraceClass ()
{
    lSubCount--;

    if ( (RoutineName_ls99 != NULL) && ls99_TraceIsOn() )
    {
        m_szTraceText[0] = 0;
        sp77sprintf(m_szTraceText, BUF_SIZE_LS99, "%*s<- %s", lSubCount, " ", RoutineName_ls99);
        ls99_Write();
    }
}
// ~tls99_TraceClass()


/*
  -----------------------------------------------------------------------------
  function:     ls99PrnF
  -----------------------------------------------------------------------------
*/
void tls99_TraceClass::ls99PrnF(const _TCHAR *Layer, const _TCHAR *format, ...)
{
    va_list arglist;

    if ((RoutineName_ls99 != NULL) && ls99_TraceIsOn())
    {
        tsp00_Int4 len = 0;
        m_szTraceText[0] = 0;

        len += sp77sprintf(m_szTraceText, BUF_SIZE_LS99, "%*s%s: ", lSubCount+3, " ", Layer);

        va_start(arglist, format);
        sp77vsprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, format, arglist);
        va_end(arglist);

        ls99_Write();
    }
}
// ls99PrnF()


/*
  -----------------------------------------------------------------------------
  function:     ls99Info
  -----------------------------------------------------------------------------
*/
void tls99_TraceClass::ls99Info(const _TCHAR *Layer, const _TCHAR *Text, tls00_DBInfo* DBInfo)
{
    if ((RoutineName_ls99 != NULL) && ls99_TraceIsOn())
    {
        tls00_SessionInfo* S = &DBInfo->dbiSession_ls00;

        tsp00_Int4 len = 0;
        m_szTraceText[0] = 0;

        len  = sp77sprintf(m_szTraceText, BUF_SIZE_LS99, "%s: %s\nDBInfo:\t'%s'\n", Layer, Text, S->siDBName_ls00.asCharp() );
        if (true == S->siSqlStat_ls00)
        {
            len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "\t=> Status: connected, ");
        }
        else
        {
            len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "\t=> Status: not connected, ");
        }
        len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "%i x %i Bytes per Packet, on Node '%s'\n"
                                                                     "\t=> Ref: %i, Service: %i, Terminal: '%s'",
                                                                     DBInfo->dbiPktCount_ls00,
                                                                     DBInfo->dbiPktSize_ls00,
                                                                     S->si_pDBNode_ls00,
                                                                     DBInfo->dbiSqlRef_ls00,
                                                                     STATIC_CAST(int, DBInfo->dbiSqlService_ls00),
                                                                     DBInfo->dbiSqlTerm_ls00.asCharp());
        ls99_Write();
    }
}
// ls99Info()


/*
 -----------------------------------------------------------------------------
  function:     ls99User
 -----------------------------------------------------------------------------
*/
void tls99_TraceClass::ls99User(const _TCHAR *Layer, tls00_DBInfo *DBInfo)
{
    if ( (RoutineName_ls99 != NULL) && ls99_TraceIsOn () )
    {
        int                i = 0;
        tls00_SessionInfo* S = &DBInfo->dbiSession_ls00;

        m_szTraceText[0] =  0;
        i  = sp77sprintf(m_szTraceText, BUF_SIZE_LS99, "%s\nUser Information: "
                                                       "'%s' at '%s'\n"
                                                       "\t%i Bytes per Page, ",
                                                       Layer,
                                                       S->siUserName_ls00.asCharp(), S->siDBName_ls00.asCharp(),
                                                       DBInfo->dbiPageSize_ls00);

        if (S->siAutoCommit_ls00)
        {
            i += sp77sprintf(m_szTraceText + i, BUF_SIZE_LS99 - i,  "AUTOCOMMIT ON, ");
        }
        else
        {
            i += sp77sprintf(m_szTraceText + i, BUF_SIZE_LS99 - i,  "AUTOCOMMIT OFF, ");
        }

        if (S->siMessageOn_ls00)
        {
            i += sp77sprintf(m_szTraceText + i, BUF_SIZE_LS99 - i,  "MESSAGE ON, ");
        }
        else
        {
            i += sp77sprintf(m_szTraceText + i, BUF_SIZE_LS99 - i,  "MESSAGE OFF, ");
        }
        
        if (S->siSQLMode_ls00 > sp1sm_oracle)
        {
            i += sp77sprintf(m_szTraceText + i, BUF_SIZE_LS99 - i,  "SQLMODE UNKNOWN (%i), ", STATIC_CAST(int, S->siSQLMode_ls00) );
        }
        else
        {
            i += sp77sprintf(m_szTraceText + i, BUF_SIZE_LS99 - i,  "SQLMODE %s, ", pszSQLModeName[S->siSQLMode_ls00]);
        }

        if (true == S->siNolog_ls00)
        {
            i += sp77sprintf(m_szTraceText + i, BUF_SIZE_LS99 - i,  "NOLOG, ");
        }

        ls99_Write();
    }
}
// ls99User()


// Die funktion gibt nur die ersten 64 bytes des packetes aus!
/*
 -----------------------------------------------------------------------------
  function:     ls99Pckt
 -----------------------------------------------------------------------------
*/
void tls99_TraceClass::ls99Pckt(const _TCHAR *Layer, tsp1_packet* SqlPacket)
{
    tsp1_segment *SqlSegm;
    tsp1_part    *SqlPart;
    tsp00_Int4    i = 0;
    tsp00_Int4    MaxBufOutput = 0;
    
    
    if ((RoutineName_ls99 != NULL) && ls99_TraceIsOn() )
    {
        m_szTraceText[0] =  0;
        
        if (SqlPacket == NULL)
        {
            sp77sprintf(m_szTraceText, BUF_SIZE_LS99, "%s: \tSQLPacket: Pointer to packet is NULL", Layer);
            ls99_Write();
            return;
        }
        else
        {
            tsp00_Int4 lCount = 0;

            lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "%s: \tSQLPacket:\n", Layer);
            SqlSegm = &SqlPacket->sp1_segm();
            
            //* Packet-Header ausgeben
            if (1 == SqlSegm->sp1s_segm_kind())
            {   
                lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "[REQUEST] ");
            }
            else
            {
                lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "[RECEIVE] ");
            }
            
            switch (SqlPacket->sp1_header.sp1h_mess_code)
            {
                case csp_ascii:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "ascii, ");
                    break;
                }
                case csp_ebcdic:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "ebcdic, ");
                    break;
                }
                case csp_codeneutral:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "neutral, ");
                    break;
                }
                case csp_unicode_swap:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "unicode swapped, ");
                    break;
                }
                case csp_unicode:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "unicode, ");
                    break;
                }
                case csp_unicode_native:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "unicode native, ");
                    break;
                }
                case csp_utf8:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "utf8, ");
                    break;
                }
                default:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "unknown msg-code (%i), ",
                                                         STATIC_CAST(int, SqlPacket->sp1_header.sp1h_mess_code) );
                    break;
                }
            }
            
            switch (SqlPacket->sp1_header.sp1h_mess_swap)
            {
                case sw_full_swapped:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "full swap, ");
                    break;
                }
                case sw_part_swapped:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "part swap, ");
                    break;
                }
                case sw_normal:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "no swap, ");
                    break;
                }
                default:
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "unknown swap (%i), ",
                                                         STATIC_CAST(int, SqlPacket->sp1_header.sp1h_mess_swap) );
                    break;
                }
            }

            //produces output: LOA70300,    (Segmemts: 1, Len: 104, Size: 32288)
            //                 Segment 1 (Offset 0, Len: 104, Parts: 1)
            lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount,
                                                 "%s%.*s,\t(Segmemts = %i, Len = %i, Size = %i)\n\t"
                                                 "Segment %i (Offset = %i, Len = %i, Parts = %i)\n\t\t",
                                                 APPL_NAME_LS00,
                                                 5, SqlPacket->sp1_header.sp1h_appl_version.asCharp(),
                                                 SqlPacket->sp1_header.sp1h_no_of_segm,
                                                 SqlPacket->sp1_header.sp1h_varpart_len,
                                                 SqlPacket->sp1_header.sp1h_varpart_size,
                                                 SqlSegm->sp1s_own_index(),
                                                 SqlSegm->sp1s_segm_offset(),
                                                 SqlSegm->sp1s_segm_len(),
                                                 SqlSegm->sp1s_no_of_parts());

            //* Segment-Header ausgeben
            //produces output:
            //  sql-mode = <sql mode>, msg-type = <msg type>, cmd-type = <cmd type>, commit_immediately = <true|false> 
            if (1 == SqlSegm->sp1s_segm_kind())     // Request
            {
                lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "sql-mode = ");
                
                if (SqlSegm->sp1c_sqlmode() > sp1sm_oracle)
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "UNKNOWN (%i)", STATIC_CAST(int, SqlSegm->sp1c_sqlmode()) );
                }
                else
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, pszSQLModeName[SqlSegm->sp1c_sqlmode()]);
                }

                lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, ", msg-type = ");
                switch (SqlSegm->sp1c_mess_type())
                {
                    case sp1m_dbs:
                    {
                        lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "dbs");
                        break;
                    }
                    case sp1m_parse:
                    {
                        lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "parse");
                        break;
                    }
                    case sp1m_syntax:
                    {
                        lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "syntax");
                        break;
                    }
                    case sp1m_execute:
                    {
                        lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "execute");
                        break;
                    }
                    case sp1m_putval:
                    {
                        lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "putval");
                        break;
                    }
                    case sp1m_getval:
                    {
                        lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "getval");
                        break;
                    }
                    case sp1m_load:
                    {
                        lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "load");
                        break;
                    }
                    case sp1m_unload:
                    {
                        lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "unload");
                        break;
                    }
                    default:
                    {
                        lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "unknown (%i)", (int) SqlSegm->sp1c_mess_type());
                    }
                }

                lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, ", cmd-type = %s",
                                                                                            pszProducer[SqlSegm->sp1c_producer()]);

                lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, ", commit_immediately = ");
                if (SqlSegm->sp1c_commit_immediately())
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "true");
                }
                else
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, "false");
                }

                if (SqlSegm->sp1c_ignore_costwarning())
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, ", ignore_costwarning");
                }
                if (SqlSegm->sp1c_prepare())
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, ", prepare");
                }
                if (SqlSegm->sp1c_with_info())
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, ", with_info");
                }
                if (SqlSegm->sp1c_mass_cmd())
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, ", mass_cmd");
                }
                if (SqlSegm->sp1c_parsing_again())
                {
                    lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount, ", parsing_again");
                }
            }
            else     // Receive
            {
                lCount += sp77sprintf(m_szTraceText + lCount, BUF_SIZE_LS99 - lCount,
                                                              "State: %.*s, Return: %i, ErrPos: %i",
                                                              SqlSegm->sp1r_sqlstate().length(),
                                                              SqlSegm->sp1r_sqlstate().asCharp(),
                                                              SqlSegm->sp1r_returncode(),
                                                              SqlSegm->sp1r_errorpos() );

            }  // if (1 == SqlSegm->sp1s_segm_kind())
            
            ls99_Write();


            tsp00_Int4 PartOffset = sizeof(tsp1_segment_header);
    
            for (i=1; i <= SqlSegm->sp1s_no_of_parts(); ++i)
            {
                m_szTraceText[0] = 0;

                tsp00_Int4  PartLen, AssignedLen;
                tsp00_Int4  HeadSize = sizeof(tsp1_part_header);

                _TCHAR szPartKind[64];

                SqlPart = (tsp1_part*) &SqlPacket->sp1_varpart()[PartOffset];

                //*
                //*     Part-Header ausgeben
                //*
                if (SqlPart->sp1p_part_kind() > sp1pk_relative_pos)
                {
                    sprintf(szPartKind, "unknown part-kind (%i), ", STATIC_CAST(int, SqlPart->sp1p_part_kind()) );
                }
                else
                {
                    strcpy(szPartKind, pszPartKindName[SqlPart->sp1p_part_kind()]);
                }


                // The next statement produces the following output:
//      1.Part: PartKind = bdinfo, Args = 1, Offset = 0, Len = 36192
                sp77sprintf(m_szTraceText, BUF_SIZE_LS99, //"\t%i.Part: PartKind = %s, Args = %i, Offset = %i, Len = %i\n",
                                                          "\t%i.Part: PartKind = %s, Args = %i, Offset = %i, Len = %i",
                                                          i,
                                                          szPartKind,
                                                          SqlPart->sp1p_arg_count(),
                                                          PartOffset,
                                                          SqlPart->sp1p_buf_len());

                ls99_Write();
        
                //* Write part buffer; output limit is 64 chars
                MaxBufOutput = 64;          
                if (SqlPart->sp1p_buf_len() < MaxBufOutput)
                {
                    MaxBufOutput = SqlPart->sp1p_buf_len();
                }
                
                ls99_TraceBuf(MaxBufOutput, SqlPart->sp1p_buf().asCharp());
                
                PartLen     = SqlPart->sp1p_buf_len();
                AssignedLen = ((HeadSize + PartLen + 7) / 8 * 8 );
                PartOffset += AssignedLen;
            
            }   // end for (i=1; i <= SqlSegm->sp1s_no_of_parts(); ++i)
        }   // end else of if (SqlPacket == NULL)
    }   // end if ((RoutineName_ls99 != NULL) && ls99_TraceIsOn() )
}
// ls99Pckt()


/*
 -----------------------------------------------------------------------------
  function:     ls99Buff
 -----------------------------------------------------------------------------
*/
void tls99_TraceClass::ls99Buff(const _TCHAR *Layer, const _TCHAR *Text, tsp00_Int4 Len, _TCHAR *Buf)
{
    if ((RoutineName_ls99 != NULL) && ls99_TraceIsOn () )
    {
        m_szTraceText[0] = 0;

        sp77sprintf(m_szTraceText, BUF_SIZE_LS99, "%s: %s\tbuf(%i)", Layer, Text, Len);
        ls99_Write();        

        ls99_TraceBuf(Len, Buf);
    }
}
// ls99Buff()


/*
 -----------------------------------------------------------------------------
  function:     ls99Tab2
 -----------------------------------------------------------------------------
*/
void tls99_TraceClass::ls99Tab2(const _TCHAR *Layer, tls00_TableDescription *Table)
{
    tsp00_Int4  len = 0;

    if ((RoutineName_ls99 != NULL) && ls99_TraceIsOn() )
    {
        len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, 
                           "%s\tTableDescription:\n"
                           "Tableowner: '%s'\n"
                           "Tablename:  '%s'\n"
                           "FieldCount:  %i\t"
                           "KernelSwap:  %i\t"
                           "KernelLock:  %i\t"
                           "ConstrCount: %i\n"
                           "No\tstmt_no\tdef_no\tfrac\tlen\tio_len\tlentype\tmode\ttype\t\tdefault\tname",
                           Layer,
                           Table->tdAuthid_ls00.asCharp(),
                           Table->tdTablename_ls00.asCharp(),
                           Table->tdFieldCount_ls00,
                           STATIC_CAST(int, Table->tdKernelSwap_ls00),
                           STATIC_CAST(int, Table->tdKernelLoc_ls00),
                           STATIC_CAST(int, Table->tdConstraintCount_ls00));
        ls99_Write();
        
        tsp00_DataType DBDataType;

        for (int i=0; i < Table->tdFieldCount_ls00; ++i)
        {
            len              = 0;
            m_szTraceText[0] = 0;
            DBDataType       = Table->tdFieldsArray_ls00[i]->fiBasics_ls00.fbDataType_ls00;

            len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len,
                                                    "%i\t%i\t\t%i\t\t%i\t\t%i\t%i\t\t",
                                                    i,
                                                    STATIC_CAST(int, Table->tdFieldsArray_ls00[i]->fiStmtColNo_ls00),
                                                    STATIC_CAST(int, Table->tdFieldsArray_ls00[i]->fiDefColNo_ls00),
                                                    STATIC_CAST(int, Table->tdFieldsArray_ls00[i]->fiBasics_ls00.fbFraction_ls00),
                                                    STATIC_CAST(int, Table->tdFieldsArray_ls00[i]->fiBasics_ls00.fbLength_ls00),
                                                    STATIC_CAST(int, Table->tdFieldsArray_ls00[i]->fiBasics_ls00.fbInOutLength_ls00) ); 

            // Dump length type
            if ( (Table->tdFieldsArray_ls00[i]->fiLengthType_ls00 > sp7_var_len_long) ||
                 (Table->tdFieldsArray_ls00[i]->fiLengthType_ls00 < sp7_fix_len) )
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len,
                                   "%i\t\t",
                                   STATIC_CAST(int, Table->tdFieldsArray_ls00[i]->fiLengthType_ls00));
            }
            else
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, pszLengthType[Table->tdFieldsArray_ls00[i]->fiLengthType_ls00]);
            }


            if ( (Table->tdFieldsArray_ls00[i]->fiColMode_ls00 > mod_dropped) ||
                 (Table->tdFieldsArray_ls00[i]->fiColMode_ls00 < mod_syskey) )
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len,
                                                        "%i\t\t",
                                                        STATIC_CAST(int, Table->tdFieldsArray_ls00[i]->fiColMode_ls00));
            }
            else
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len,
                                                        pszColumnMode[Table->tdFieldsArray_ls00[i]->fiColMode_ls00]);
            }

            // Write data type
            if (DBDataType > dabaptabhandle)
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "%i\t\t", STATIC_CAST(int, DBDataType));
            }
            else
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, pszDataTypeName[DBDataType]);
            }



            if ( (Table->tdFieldsArray_ls00[i]->fiDefault_ls00 < sp7_no_default) ||
                 (Table->tdFieldsArray_ls00[i]->fiDefault_ls00 > sp7_default_serial) )
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "%i\t\t",
                                                                                (int) Table->tdFieldsArray_ls00[i]->fiDefault_ls00);
            }
            else
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, pszDefaultType[Table->tdFieldsArray_ls00[i]->fiDefault_ls00]);
                
            }
            len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "%s", Table->tdColName_ls00[i]);

            ls99_Write();  // End of for-loop reached -> flush the stuff
        }   // end for (int i=0; i < Table->tdFieldCount_ls00; i++)
    }
}
// ls99Tab2()


/*
 -----------------------------------------------------------------------------
  function:     ls99TabParam
 -----------------------------------------------------------------------------
*/
void tls99_TraceClass::ls99TabParam(const _TCHAR *Layer, tls00_TableDescription *Table)
{
    tsp00_Int4 len = 0;

    if ((RoutineName_ls99 != NULL) && ls99_TraceIsOn() )
    {
        len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, 
                           "%s\tTableDescription:"NEW_LINE_LS00
                           "Tableowner: '%s'"NEW_LINE_LS00
                           "Tablename:  '%s'"NEW_LINE_LS00
                           "FieldCount:  %i\t"
                           "KernelSwap:  %i\t"
                           "KernelLock:  %i\t"
                           "ConstrCount: %i"NEW_LINE_LS00
                           //"no\tpos\tmode\t\tiotype\tdatatype\tfrac\tlen\tio_len\tname\n",
                           "no\tpos\tmode\t\tiotype\tdatatype\tfrac\tlen\tio_len\tname",
                           Layer,
                           Table->tdAuthid_ls00.asCharp(),
                           Table->tdTablename_ls00.asCharp(),
                           Table->tdFieldCount_ls00,
                           STATIC_CAST(int, Table->tdKernelSwap_ls00),
                           STATIC_CAST(int, Table->tdKernelLoc_ls00),
                           STATIC_CAST(int, Table->tdConstraintCount_ls00));
        ls99_Write();

        len = 0;
        m_szTraceText[0] = 0;

        tls00_ParamInfo *PI = NULL;

        for (int i=0; i < Table->tdFieldCount_ls00; i++)
        {
            PI = Table->tdParamArray_ls00[i];
            len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "%i.\t%i\t", i, PI->piBufpos_ls00);

            if (true == PI->piMode_ls00.includes(sp1ot_mandatory))
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "mandatory\t");
            }
            else if (true == PI->piMode_ls00.includes(sp1ot_optional))
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "optional \t");
            }
            else if (true ==  PI->piMode_ls00.includes(sp1ot_default))
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "default  \t");
            }
            else        // sp1ot_escape_char
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "escape   \t");
            }

            switch (PI->piIoType_ls00)
            {
                case sp1io_input:
                {
                    len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "input\t");
                    break;
                }
                case sp1io_output:
                {
                    len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "output\t");
                    break;
                }
                default:
                {
                    len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "inout\t");
                    break;
                }
            }

            // Write data type
            if (PI->piBasicInfo_ls00.fbDataType_ls00 > dabaptabhandle)
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "%i\t\t", STATIC_CAST(int, PI->piBasicInfo_ls00.fbDataType_ls00));
            }
            else
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, pszDataTypeName[PI->piBasicInfo_ls00.fbDataType_ls00]);
            }


            len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "%i\t%i\t%i\t",PI->piBasicInfo_ls00.fbFraction_ls00,
                                                                  PI->piBasicInfo_ls00.fbLength_ls00,
                                                                  PI->piBasicInfo_ls00.fbInOutLength_ls00);

            if (NULL != Table->tdColName_ls00[i])
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len, "%s", Table->tdColName_ls00[i]);
            }
/*            else
            {
                len += sp77sprintf(m_szTraceText + len, BUF_SIZE_LS99 - len,  "\n");
            }
*/
            ls99_Write();

            len = 0;
            m_szTraceText[0] = 0;
        }   // end for (int i=0; i < Table->tdFieldCount_ls00; i++)
    }   // end if ((RoutineName_ls99 != NULL) && ls99_TraceIsOn() )
}
// ls99TabParam()


// Hier wird genau einmal das trace file geoeffnet. Der dateizeiger ist eine statische variable
// der klasse und kann/sollte von allen objekten genutzt werden.

/*
 -----------------------------------------------------------------------------
  function:     ls99Init
 -----------------------------------------------------------------------------
*/
void tls99_TraceClass::ls99Init(const _TCHAR *Layer, const _TCHAR *Text, const _TCHAR *FileName)
{
	LoaderMessages* pMsgInstance = LoaderMessages::Instance();

    bTraceFileInit_ls99 = false;        // Initialisiere die statische variable
    
    tsp00_Date Date;
    tsp00_Time Time;
    sqldattime(Date, Time);
    
    tsp00_Datec szDate;
    tsp00_Timec szTime;
    szDate.p2c(Date);
    szTime.p2c(Time);
    
    // Hole speicher fuer den dateinamen. Dieser muss in der ende-funktion wieder
    // freigegeben werden!!! Dann uebernimm den gelieferten dateinamen in die interne variable.
	_TCHAR   szLoaderPath[PATH_MXSP00];
    szLoaderPath[0] = '\0';
    pMsgInstance->ls98GetLoaderLogPath(szLoaderPath);
        
    size_t len = _tcslen(szLoaderPath) + _tcslen(FileName);
        
    m_pszTraceFileName = new char[len + 1];      // Initialisiere die member variablen
    m_pszTraceFileName[0] = '\0';

    sp77sprintf(m_pszTraceFileName, len+1, "%s%s", szLoaderPath, FileName);
    
    m_RteError.sp5fe_result.becomes(vf_ok);

    sqlfopenc(m_pszTraceFileName, sp5vf_text, sp5vf_write, sp5bk_unbuffered, &m_TraceFileHandle, &m_RteError);
    if (m_RteError.sp5fe_result != vf_ok)
    {
        // Die datei konnte nicht geoeffnet werden. Da das aber fuer den weiteren fortgang
        // des prozesses notwendig ist, machen wir eine temporaere datei auf und vermerken diesen
        // umstand in dieser datei.
        m_TraceFileHandle = 0;

        delete [] m_pszTraceFileName;
        len += szDate.size() + szTime.size() + sizeof(char);
        m_pszTraceFileName = new char [len + 1];

        sp77sprintf(m_pszTraceFileName, len+1, "%sloader_%s%s.trc", szLoaderPath, szDate.asCharp(), szTime.asCharp());

        sqlfopenc(m_pszTraceFileName, sp5vf_text, sp5vf_write, sp5bk_unbuffered, &m_TraceFileHandle, &m_RteError);
    }
    
    m_szTraceText[0] = 0;

    sp77sprintf(m_szTraceText, BUF_SIZE_LS99, "%s: %s   %s%s", Layer, Text, szDate.asCharp(), szTime.asCharp());
    ls99_Write();

    bTraceFileInit_ls99 = true;
}
// ls99Init()




// Hier wird schlussendlich die datei geschlossen, falls das nicht durch einen
// systemabsturz (:-(( ) sowieso schon geschehen ist. Diese funktion sollte genau
// einmal im gesamten server aufgerufen werden, naemlich am ende.
// Der inhalt irgendwelcher dateipuffer muss NICHT geflusht werden, da wir hier ungepuffert
// arbeiten.
/*
 -----------------------------------------------------------------------------
  function:     ls99Ende
 -----------------------------------------------------------------------------
*/
void tls99_TraceClass::ls99Ende(const _TCHAR *Layer, const _TCHAR *Text)
{
    if ((RoutineName_ls99 != NULL) && ls99_TraceIsOn () )
    {
        sqlfclosec(m_TraceFileHandle, sp5vf_close_normal, &m_RteError);
        if (vf_ok != m_RteError.sp5fe_result)
        {
            m_ErrorText.p2c(m_RteError.sp5fe_text);
        }

        m_TraceFileHandle = 0;
    }

    // Freigeben der in INIT angeforderten resourcen.
    delete [] m_pszTraceFileName;
    m_pszTraceFileName = NULL;

    bTraceFileInit_ls99 = false;
}
// ls99Ende()


/*
  -----------------------------------------------------------------------------
  function:     ls99_Write
  -----------------------------------------------------------------------------
*/
int tls99_TraceClass::ls99_Write()
{
    tsp00_Int4          rc = 0;

    if (0 != m_TraceFileHandle)         // write only if there is a place to write
    {
        m_RteError.sp5fe_result.becomes(vf_ok);

        sqlfwritec(m_TraceFileHandle, m_szTraceText, -1, &m_RteError);
        if (vf_ok != m_RteError.sp5fe_result)     // fehler beim schreiben???
        {
            m_ErrorText.p2c(m_RteError.sp5fe_text);
            rc = -1;
        }
    }
    return rc;
}
// ls99_Write()


/*
  -----------------------------------------------------------------------------
  function:     ls99_Write
  -----------------------------------------------------------------------------
*/
int tls99_TraceClass::ls99_Write(char *pszBuffer)
{
    tsp00_Int4          rc = 0;

    if (0 != m_TraceFileHandle)         // write only if there is a place to write
    {
        m_RteError.sp5fe_result.becomes(vf_ok);

        sqlfwritec(m_TraceFileHandle, pszBuffer, -1, &m_RteError);
        if (vf_ok != m_RteError.sp5fe_result)     // fehler beim schreiben???
        {
            m_ErrorText.p2c(m_RteError.sp5fe_text);
            rc = -1;
        }
    }
    return rc;
}
// ls99_Write()
