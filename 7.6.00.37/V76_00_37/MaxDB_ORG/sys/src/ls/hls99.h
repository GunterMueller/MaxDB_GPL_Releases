/* 
  -----------------------------------------------------------------------------
 
  module: hls99.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Manager
 
  description:  trace

  version:      7.4

  -----------------------------------------------------------------------------
 
  Copyright (c) 1997-2005 SAP AG-2003
 
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
#ifndef HLS99
#define HLS99

#include "gls00.h"


#undef  TRACER_CLASS_MEO00
#define TRACER_CLASS_MEO00 tls99_TraceClass


#if COMPILEMODE_MSP00 >= SLOW_MSP00

#define TRACE_INIT_MLS99(Layer, Text, FileName)     localTrace.ls99Init (Layer, Text, FileName)
#define TRACE_ENDE_MLS99(Layer, Text)               localTrace.ls99Ende (Layer, Text)
#define TRACE_INFO_MLS99(Layer, Text, DBInfo)       localTrace.ls99Info (Layer, Text, DBInfo)
#define TRACE_USER_MLS99(Layer, DBInfo)             localTrace.ls99User (Layer, DBInfo)
#define TRACE_PCKT_MLS99(Layer, SqlPacket)          localTrace.ls99Pckt (Layer, SqlPacket)
#define TRACE_BUFF_MLS99(Layer, Text, Len, Buf)     localTrace.ls99Buff (Layer, Text, Len, Buf)
#define TRACE_TAB2_MLS99(Layer, Table)              localTrace.ls99Tab2 (Layer, Table)
#define TRACE_TABPARAM_MLS99(Layer, Table)          localTrace.ls99TabParam (Layer, Table)
#define TRACE_PRNF_MLS99(args)                      localTrace.ls99PrnF args

#else

#define TRACE_INIT_MLS99(Layer, Text, FileName)
#define TRACE_ENDE_MLS99(Layer, Text)
#define TRACE_INFO_MLS99(Layer, Text, DBInfo)
#define TRACE_USER_MLS99(Layer, DBInfo)
#define TRACE_PCKT_MLS99(Layer, SqlPacket)
#define TRACE_BUFF_MLS99(Layer, Text, Len, Buf)
#define TRACE_TAB2_MLS99(Layer, Table)
#define TRACE_TABPARAM_MLS99(Layer, Table)
#define TRACE_PRNF_MLS99(args)

#endif


#define BUF_SIZE_LS99 1024


class tls99_TraceClass : public tsp00_Tracer
{
  private:
    const char              *RoutineName_ls99;
    static tsp00_Int4        m_TraceFileHandle;
    static char             *m_pszTraceFileName;
    static bool              bTraceFileInit_ls99;
    static tsp00_Int2        lSubCount;
    
    char                     m_szTraceText[BUF_SIZE_LS99];
    tsp00_ErrTextc           m_ErrorText;                       // Internal error text (used if something went wrong
                                                                // at writing to trace)
    tsp05_RteFileError       m_RteError;


    static char const *pszPartKindName[25];
    static char const *pszSQLModeName[6];
    static char const *pszDataTypeName[39];
    static char const *pszMsgTypeName[17];
    static char const *pszProducer[5];
    static char const *pszLengthType[3];
    static char const *pszColumnMode[5];
    static char const *pszDefaultType[12];


  private:
    bool ls99_TraceIsOn(const char Method = 0x00);
    void ls99_TraceBuf(tsp00_Int4 TraceBufLen, tsp00_Addr TraceBuf);
    int  ls99_Write();
    int  ls99_Write(char *pszBuffer);



  public:
    tls99_TraceClass (const char *Param = NULL);
    ~tls99_TraceClass ();

    void
    ls99Init(const _TCHAR *Layer, const _TCHAR *Text, const _TCHAR *FileName);
    
    void
    ls99Ende(const _TCHAR *Layer, const _TCHAR *Text);
    
    void
    ls99Info(const _TCHAR *Layer, const _TCHAR *Text, tls00_DBInfo* DBInfo);
    
    void
    ls99User(const _TCHAR *Layer, tls00_DBInfo *DBInfo);

    void
    ls99Pckt(const _TCHAR *Layer, tsp1_packet* SqlPacket);

    void
    ls99Buff(const _TCHAR *Layer, const _TCHAR *Text, tsp00_Int4 Len, _TCHAR *Buf);

    void
    ls99Tab2(const _TCHAR *Layer, tls00_TableDescription *Table);
    void
    ls99TabParam(const _TCHAR *Layer, tls00_TableDescription *Table);

    void
    ls99PrnF(const _TCHAR *Layer, const _TCHAR *format, ...);
};


#endif /*HLS99*/