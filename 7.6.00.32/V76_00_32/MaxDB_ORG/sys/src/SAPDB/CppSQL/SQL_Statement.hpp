/*!
 * \file    SQL_Statement.hpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Statement
 */

/*

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


#ifndef SQL_STATEMENT_HPP
#define SQL_STATEMENT_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Oms/OMS_DbpBase.hpp"

#ifndef PIN_H
#include "PacketInterface/PIn.h"
#endif

#include "hsp81.h"
#include "livecachetypes.h"   /*no check*/


/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

class SqlCol;
class SQL;
class PIn_Part;
class SQL_SessionContext;


/************************************************************************/
/* Type definitions                                                     */
/************************************************************************/

typedef struct SQL_LongDesc {
  tsp00_LongDescriptor dsc;
  int                  varPos;
  int                  bufPos;
  short                colIdx;
} SQL_LongDesc;

typedef enum { 
  ECO521_SQL,
  ECO521_FETCH_DESCRIBE
} eco521_stmt_type;

// PTS 1119960 
typedef enum {
  parseid,
  parseid_of_select
} parseid_type;


/************************************************************************/
/* Class SQL_Statement                                                  */
/************************************************************************/

class SQL_Statement {
private:
  int                            m_ref_count;
  SQL_SessionContext &           m_session_context;
  union { char           * c;
          tsp81_UCS2Char * uc;
          DbpTypeUnicode * dc; } m_statement;
/*  char *                       m_statement; */
  boolean                        m_prepared;
  boolean                        m_reset_parms;       /* PTS 1109560 */
  eco521_stmt_type               m_stmt_type;
  tsp00_KnlIdentifierc           m_res_tabname;
  tin02_ParseId                  m_parseid;
  tin02_ParseId                  m_parseid_of_select;
  int                            m_parseid_len;
  int                            m_unicode;
  boolean                        m_mass_cmd;          /* PTS 1119960 */
  boolean                        m_multi_sing_exec;   /* PTS 1119960 */
  boolean                        m_multi_sing_fetch;  /* PTS 1119960 */
  boolean                        m_multi_sing_insert; /* PTS 1119960 */
  boolean                        m_mass_fetch;        /* PTS 1119960 */
  int                            m_fetch_count;       /* PTS 1119960 */
  int                            m_max_fetch_count;   /* PTS 1119960 */
  tsp00_Byte *                   m_data_ptr;          /* PTS 1119960 */
  SQL_LongDesc *                 m_longdesc_ptr;     /* PTS 1123241 */
  bool *                         m_longdata_notnull; /* PTS 1123241 */
  bool *                         m_longdata_inplace; /* PTS 1123241 */
  int                            m_longdesc_pos;     /* PTS 1123241 */
  int                            m_longdesc_cnt;     /* PTS 1123241 */
  int                            m_longdesc_cntr;    /* PTS 1123241 */
  tsp00_Byte *                   m_longdata_ptr;     /* PTS 1123241 */
  bool                           m_longdata_buf;     /* PTS 1123241 */
  struct structSqlDA {
    structSqlDA();   
    int            inp_col_idx;  /* PTS 1119960, short before */
    int            sqlmax;       /* number of allocated SqlCol entries; PTS 1119960, short before */
    short          sqln;         /* number of in/out SqlCol entries */
    short          long_col_num; /* number of long columns actually used */
    int            fix_buf_len;
    int            orig_fix_buf_len;
    short          has_long_in_col;
    short          has_long_out_col;
    SqlCol *       sqlvar;
    SQL_LongDesc * longDesc;
  }                              m_sqlda;
  void reallocColDescs( int i = 20 );     /* PTS 1119960, short before */
public:
  SQL_Statement( SQL_SessionContext * sessContext, const char * stmt );
  SQL_Statement( SQL_SessionContext * sessContext,
                 const DbpTypeUnicode *stmt );
  void           appendNewColDesc(const SqlCol & p);
  int            addResCntParm( PIn_Part * part, int swi, int cnt ); /* PTS 1119960 */
  int            addInputParms( PIn_Part * part );
  int            addInputParms( int offset, PIn_Part * part ); /* PTS 1119960 */
  int            addMassInputParms( PIn_Part * part, int start_row, int end_row ); /* PTS 1119960 */
  int            getInpColIdx() {return m_sqlda.inp_col_idx;} /* PTS 1119960 */
  int            addOpenLongDataInput( PIn_Part * part );
  int            addOpenLongDataInput( PIn_Part * part, int offset ); /* PTS 1119960 */
  int            addOpenLongDataOutput( PIn_Part * part );
  int            addOpenLongDataOutputMass(PIn_Part * part); /* PTS 1123241 */
  int            addInputFinishLongData( PIn_Part * part );
  int            addInputFinishLongData( PIn_Part * part, int offset ); /* PTS 1119960 */
  int            addRef();
  int            createLongDescriptors();
  int            createLongDescriptors(int offset); /* PTS 1119960 */
  int            dropLongDescriptors();
  int            execute();
  int            executeMass(); /* PTS 1119960 */
  SQL_LongDesc * getLongDescPtr(short idx);
  SQL_LongDesc * getLongDescPtrMass(short idx); /* PTS 1123241 */
  int            getOutputParms( int argCount, int dataLen,
                                 const teo00_Byte * dataPtr );
  int            getOutputParmsMass( int argCount, int dataLen,    /* PTS 1123241 */
                                     const teo00_Byte * dataPtr ); /* PTS 1123241 */
  int            getNextOutputParms (const teo00_Byte * dataPtr); /* PTS 1119960 */
  int            getNextOutputParmsMass();     /* PTS 1123241 */
  int            getNextLongOutputParmsMass(); /* PTS 1123241 */
//  unsigned char  getParseFlag1(); /* PTS 1119960 */
//  unsigned char  getParseFlag2(); /* PTS 1119960 */
//  unsigned char  getParseInfo( parseid_type id, int bytenr ); /* PTS 1119960 */
  int            getLongDesc( int argCount, int dataLen,
                              const tsp00_Byte * dataPtr );
  int            getLongDescMass( int argCount, int dataLen,
                                  const tsp00_Byte * dataPtr ); /* PTS 1123241 */
  int            getLongDescAndValue( int argCount, int dataLen,
                                      const tsp00_Byte * dataPtr);
  int            getLongDescAndValueMass( int argCount, int dataLen,
                                          const tsp00_Byte * dataPtr); /* PTS 1123241 */
  short          hasLongInput();
  short          hasLongInput(int offset); /* PTS 1119960 */
  short          hasLongOutput();
  short          hasLongOutputMass(); /* PTS 1123241 */
  int            prepare( const pasbool parsingAgain = false );
  int            release();
  boolean        hasInputParms();
  void           setParsid( int len, const tin02_ParseId * data );
  void           setParsidOfSelect( int len, const tin02_ParseId * data );
  void           setResultTablename( int len, const char * data );
  void           setSqlDA_sqln( int argCount ); /* PTS 1119960, short before */
  void           setSqlDA_sfi( int i, const tsp1_param_info * sfi ); /* PTS 1119960, short i before */
  void           setSqlDA_sfi( short i, int j, int len,
                               const tsp1_param_info * sfi ); /* PTS 1119960 */
  void           shiftSqlCols( int offset, int nr ); /* PTS 1119960 */
  void           setResetParms();               /* PTS 1109560 */
  void           setMassCmdFlag();              /* PTS 1119960 */
  void           removeMassCmdFlag();           /* PTS 1119960 */
  void           setMultiSingExecFlag();        /* PTS 1119960 */
  void           removeMultiSingExecFlag();     /* PTS 1119960 */
  void           setMultiSingFetchFlag();       /* PTS 1119960 */
  boolean        multSingFetch();               /* PTS 1119960 */
  void           setMultiSingInsertFlag();      /* PTS 1119960 */
  boolean        multSingInsert();              /* PTS 1119960 */
  boolean        multSingExec();                /* PTS 1119960 */
  boolean        isMassCmd();                   /* PTS 1119960 */
  short          getSqln();                     /* PTS 1119960 */
  short          getSqlDA_inp_col_idx();        /* PTS 1119960 */
  short          getNumVars () const;
  short          getNumCols () const;
  short          getNumOutCols () const;
  int            getParsid(const tin02_ParseId ** data) const;
  int            getParsidInfo1() const {return m_parseid[10];} /* PTS 1116114 FF 2002-06-10 */
  int            getParsidOfSelectInfo1() const {return m_parseid_of_select[10];} /* PTS 1123241 */
  int            getFixBufLength( ) const;
  void           setOrigFixBufLength(int fix_buf_len); /* PTS 1119960 */
  int            getOrigFixBufLength( ) const;         /* PTS 1119960 */
  short          getLongColNum() const;
  boolean        getPreparedFlag() const;
  const char *   getStatement() const;
  const tsp81_UCS2Char * getUniStatement() const;
  int            isUnicode() const;
  eco521_stmt_type       getStmtType() const;
  void *         operator new (size_t sz);
  void           operator delete (void * p);
# if defined (EXT_CLIENT_SQL)
  int            connect( const char * hostname, const char * dbname,
                          const char * user, const char * pwd,
                          const char* isolevel);
# endif
# if !defined (EXT_CLIENT_SQL)
  void           dumpBadAsciidataInfo( int prmNo, int offset, int pos ); /* PTS 1130629 */
# endif
  SqlCol & operator[] (int);
};


#endif /* SQL_STATEMENT_HPP */

