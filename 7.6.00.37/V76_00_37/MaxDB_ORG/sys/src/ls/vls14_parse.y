/*
  -----------------------------------------------------------------------------
  module: vls14_parse.y
  -----------------------------------------------------------------------------

  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  parser grammar

  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

%{
#include "gls00.h"
#include "geo47.h"
#include "hls07.h"
#include "hls13.h"
#include "hls15.h"

#define STRCPY_LS14(DA,DL,SA,SL)    DA = new char [SL + 1]; \
                                    if (DA == NULL) DL = 0; \
                                    else { DL = SL; SAPDB_memcpy (DA, SA, DL); DA[DL] = DEF_BYTE_LS00; }
%}

/* Startsymbol */
%start  known_statements

/* typenlose Terminalsymbole = Schluesselworte */
%token  kwADSM
%token  kwALL
%token  kwAND
%token  kwANSI
%token  kwAPPEND
%token  kwASCII
%token  kwAUTOCOMMIT
%token  kwBACK
%token  kwBACKUPTOOL
%token  kwBINARY
%token  kwBOOLEAN
%token  kwBY
%token  kwCATALOG
%token  kwCATALOGEXTRACT
%token  kwCATALOGLOAD
%token  kwCHAR
%token  kwCHECKCONSTRAINT
%token  kwCODETYPE
%token  kwCODEPAGE
%token  kwCOMPRESSED
%token  kwCONFIGURATION
%token  kwCOUNT
%token  kwCREATESCHEMAMAP
%token  kwCURRENT
%token  kwCURRENT_SCHEMA
%token  kwDATA
%token  kwDATAEXTRACT
%token  kwDATALOAD
%token  kwDATAUPDATE
%token  kwDATE
%token  kwDB
%token  kwDBEXTRACT
%token  kwDBLOAD
%token  kwDB2
%token  kwDDL
%token  kwDDLNOINDEX
%token  kwDECIMAL
%token  kwDEFAULT
%token  kwDELIMITER
%token  kwDROPSCHEMAMAP
%token  kwDTL
%token  kwDUPLICATES
%token  kwEBCDIC
%token  kwEBID
%token  kwENCRYPTION
%token  kwEUR
%token  kwEXCLUDE
%token  kwEXPORT
%token  kwFASTLOAD
%token  kwFILE
%token  kwFOR
%token  kwFORMAT
%token  kwFORMATTED
%token  kwHEX
%token  kwHILO
%token  kwIF
%token  kwIGNORE
%token  kwINCLUDE
%token  kwINFILE
%token  kwINSTALLATION
%token  kwINTERNAL
%token  kwINTEGER
%token  kwISO
%token  kwISOLATION_LEVEL
%token  kwJIS
%token  kwKEY
%token  kwLANGUAGE
%token  kwLOGOWNER
%token  kwMAPSCHEMAS
%token  kwPACKAGE
%token  kwREMARK
%token  kwIMPORT
%token  kwLOHI
%token  kwLONGFILE
%token  kwMAXERRORCOUNT
%token  kwMAXRECORDS
%token  kwMAXROWS
%token  kwMESSAGE
%token  kwNOHEADER
%token  kwNODATA
%token  kwNODDL
%token  kwNOLOG
%token  kwNOT
%token  kwNSR
%token  kwNULL
%token  kwNUMBER
%token  kwODBCTRACE
%token  kwOFF
%token  kwON
%token  kwOR
%token  kwORACLE
%token  kwORDER
%token  kwOTHERWISE
%token  kwOUTFILE
%token  kwOUTFIELDS
%token  kwPAGE
%token  kwPAGES
%token  kwPIPE
%token  kwPOS
%token  kwREAL
%token  kwRECORD
%token  kwRECORDS
%token  kwREJECT
%token  kwRELEASE
%token  kwRESTART
%token  kwROUND
%token  kwROWS
%token  kwSCALE
%token  kwSCHEMA
%token  kwSCHEMAMAP
%token  kwSEPARATOR
%token  kwSEQNO
%token  kwSERVERDB
%token  kwSET
%token  kwSIZE
%token  kwSQLMODE
%token  kwSTAMP
%token  kwSTART
%token  kwSTATISTICS
%token  kwSQLID
%token  kwSSL
%token  kwSYSDATE
%token  kwSYSTEMATTRIBUTE
%token  kwTABLE
%token  kwEXPORTTABLE
%token  kwIMPORTTABLE
%token  kwTABLEFASTLOAD
%token  kwTABLEUNLOAD
%token  kwTABLEUPDATE
%token  kwTAPE
%token  kwTIME
%token  kwTIMESTAMP
%token  kwTO
%token  kwTRANSACTION
%token  kwTRANSPORT
%token  kwTRUNC
%token  kwUCS2
%token  kwUID
%token  kwUPDATE
%token  kwUSA
%token  kwUSAGE
%token  kwUSE
%token  kwUSER
%token  kwUSERKEY
%token  kwUSERGROUP
%token  kwUSESCHEMAMAP
%token  kwUTF8
%token  kwVERSION
%token  kwWITH
%token  kwXML
%token  kwXMLNOTABLE
%token  kwZONED

%left   kwOR
%left   kwAND
%left   kwNOT
%right  kwSEPARATOR, kwDELIMITER


/* typisierte Terminalsymbole = Rueckgabewerte */
%token <C_Char>     sgnOPEN
%token <C_Char>     sgnCLOSE
%token <C_Char>     sgnCOMMA
%token <C_Char>     sgnPERCENT
%token <C_Char>     sgnPOINT
%token <C_Char>     sgnCOLON

%token <C_Char2>    opEQ
%token <C_Char2>    opLA
%token <C_Char2>    opLE
%token <C_Char2>    opMI
%token <C_Char2>    opNE
%token <C_Char2>    opSE
%token <C_Char2>    opSM

%token <C_Char>     dtCHAR
%token <C_String>   dtDNS_NAME
%token <C_Char>     dtHEXCHAR
%token <C_String>   dtDB_IDENTIFIER
%token <C_String>   dtINTEGER_LITERAL
%token <C_String>   dtNEG_POS_INTEGER_LITERAL
%token <C_String>   dtSTRING
%token <C_String>   dtSINGLE_QUOTED_STRING
%token <C_String>   dtDOUBLE_QUOTED_STRING
%token <C_String>   dtURI_REFERENCE



/* Assignment of variables to types */
%type <C_Int>       valBLOCK_COUNT
%type <C_Int>       valCOLUMN_ID
%type <C_Int>       valEND
%type <C_Int>       valERRCOUNT
%type <C_Int>       valFRACTION
%type <C_Int>       valISOLATION_LEVEL
%type <C_Int>       valRECORD_LENGTH
%type <C_Int>       valSCALE_FACTOR
%type <C_Int>       valSTART
%type <C_Int>       valSTART_SEQNO
%type <C_Int>       valINCREMENT_SEQNO
%type <C_Int>       valTRANSACTION_SIZE
%type <C_Int>       valUSAGE


%type <C_String>    valBOOL_VALUES
%type <C_String>    valCODEPAGE_ID
%type <C_String>    valCOLUMN_NAME
%type <C_String>    valCOLUMN_NAME_UNCHANGED
%type <C_String>    valDATE_TIME_FORMAT_MASK
%type <C_String>    valDBNAME
%type <C_String>    valDBNODE
%type <C_String>    valDELIMITER
%type <C_String>    valEBID
%type <C_String>    valEND_POS
%type <C_String>    valFILE_NAME
%type <C_String>    valLITERAL
%type <C_String>    valNUMBER_LAYOUT
%type <C_String>    valOS_USERNAME
%type <C_String>    valPACKAGEGUID
%type <C_String>    valPASSWORD
%type <C_String>    valREMARK
%type <C_String>    valSEPARATOR
%type <C_String>    valTABLE_NAME
%type <C_String>    valSCHEMAMAP_NAME
%type <C_String>    valSCHEMA_NAME
%type <C_String>    valSTART_POS
%type <C_String>    valUNKNOWN_KEYWORD
%type <C_String>    valUSERKEY
%type <C_String>    valUSERNAME


/* Zuordnung der Nicht-terminalen Symbole zu Typen */
%type <C_Column>                acc_column_spec
%type <C_MultColSpec>           acc_column_spec_mlt
%type <C_Bool>                  append_opt
%type <C_BackupTool>            backup_type
%type <C_BoolSpec>              bool_spec
%type <C_TransformationCmd>     catalogextract_stmt
%type <C_TransformationCmd>     catalogload_stmt
%type <C_Bool>                  char_opt
%type <C_CodeSpec>              code_spec
%type <C_CodeType>              code_type
%type <C_ConfigurationType>     configuration_type
%type <C_Int>                   codepage_spec
%type <C_Column>                column_assignment
%type <C_Column>                column_descriptor
%type <C_String>                column_id
%type <C_Column>                column_id_spec
%type <C_String>                column_names
%type <C_OpType>                comp_op
%type <C_CondSpec>              condition
%type <C_Int>                   count_spec
%type <C_TransformationCmd>     columnextract_stmt
%type <C_TransformationCmd>     columnload_stmt
%type <C_TransformationCmd>     columnupdate_stmt
%type <C_TransformationCmd>     createschemamap_stmt
%type <C_DateTimeSpec>          date_spec
%type <C_DateTime>              date_time_type
%type <C_Assign>                db2_value
%type <C_TransformationCmd>     dropschemamap_stmt
%type <C_BackupIdName>          backup_spec_opt
%type <C_TransformationCmd>     dbpartextract_stmt
%type <C_TransformationCmd>     dbpartload_stmt
%type <C_FileSpec>              dbstatistics_outfile_opt
%type <C_FileSpec>              dbstatistics_infile_opt
%type <C_Char>                  delimiter_spec
%type <C_DeviceType>            devicetype_spec
%type <C_DeviceType>            devicetype_spec_opt
%type <C_Duplicate>             duplicates_clause
%type <C_Duplicate>             duplicates_clause_opt
%type <C_EncryptionProtocol>    encryption_spec_opt
%type <C_Bool>                  exclude_opt
%type <C_TransformationCmd>     extract_files_spec
%type <C_FileSpec>              configurationfile_spec_opt
%type <C_FileSpec>              cataloginfile_spec_opt
%type <C_FileSpec>              catalogoutfile_spec
%type <C_FileSpec>              catalogoutfile_spec_opt
%type <C_FileSpec>              datainfile_spec
%type <C_FileSpec>              datainfile_spec_opt
%type <C_FileSpec>              dataoutfile_spec_opt
%type <C_FileSpec>              packagefile_spec_opt
%type <C_FieldFormat>           field_format
%type <C_FieldPos>              field_pos
%type <C_FileFormat>            file_format
%type <C_FileFormat>            file_format_spec
%type <C_FormatSpec>            format_spec
%type <C_FileExtract>           file_extract_opt
%type <C_Int>                   fraction
%type <C_Int>                   fraction_opt
%type <C_DateTimeSpec>          free_dt_mask
%type <C_FieldAsgn>             generate_spec
%type <C_Bool>                  hex_opt
%type <C_CondSpec>              if_condition
%type <C_CondSpec>              if_condition_opt
%type <C_FileSpec>              infile_spec
%type <C_SwapKind>              int_spec
%type <C_Int>                   integer_value
%type <C_Int>                   integer_neg_pos_value
%type <C_Bool>                  internal_opt
%type <C_Assign>                internal_value
%type <C_Int>                   isolevel_spec_opt
%type <C_Int>                   isolevel_spec
%type <C_Column>                key_column_spec
%type <C_Column>                lit_column_spec
%type <C_Column>                load_column_spec
%type <C_MultColSpec>           load_column_spec_mlt
%type <C_FileSpec>              longfile_spec
%type <C_MultFileSpec>          longfile_spec_mlt_opt
%type <C_CodeSpec>              longfile_code_spec_opt
%type <C_CodeSpec>              longfile_code_spec
%type <C_MultSchemaSpec>        map_schema_spec
%type <C_MultSchemaSpec>        map_schema_spec_opt
%type <C_Int>                   noheader_opt
%type <C_Bool>                  nolog_opt
%type <C_SessionInfo>           node_opt
%type <C_CondSpec>              null_condition
%type <C_CondSpec>              null_condition_opt
%type <C_FieldAsgn>             null_assign
%type <C_FieldAsgn>             null_assign_opt
%type <C_String>                null_spec
%type <C_Char2>                 number_spec
%type <C_Bool>                  on_off
%type <C_FieldOption>           options
%type <C_InOutType>             oracle_format
%type <C_Assign>                oracle_value
%type <C_String>                order_clause
%type <C_String>                order_clause_opt
%type <C_FileSpec>              outfile_spec
%type <C_FileSpec>              outfile_spec_opt
%type <C_Column>                output_column
%type <C_MultColSpec>           output_column_opt
%type <C_MultColSpec>           output_column_lst
%type <C_PartSpec>              part_spec
%type <C_PartSpec>              part_spec_all_user
%type <C_String>                password_spec
%type <C_Restore>               restore_spec
%type <C_Restore>               restore_spec_opt
%type <C_FieldOption>           round_or_trunc_spec
%type <C_Restart>               restartpackage_opt
%type <C_FieldOption>           scale_spec
%type <C_String>                schemaname_stmt_opt
%type <C_String>                schemaname_stmt
%type <C_MultSchemaSpec>        schema_map_mlt
%type <C_SchemaMap>             schema_map
%type <C_String>                select_expression
%type <C_Char>                  separator_spec
%type <C_FieldAsgn>             sequence_number
%type <C_SessionInfo>           serverdb_stmt_opt
%type <C_SessionInfo>           serverdb_stmt
%type <C_Column>                set_column_spec
%type <C_MultColSpec>           set_column_spec_mlt
%type <C_Column>                simple_column_spec
%type <C_Condition>             simple_condition
%type <C_SqlMode>               sql_mode
%type <C_DBSqlCmd>              sql_stmt
%type <C_RemarkCmd>             remark_stmt
%type <C_DateTimeSpec>          standard_dt_mask
%type <C_TableSpec>             table_name
%type <C_TableSpec>             table_spec
%type <C_TransformationCmd>     tableextract_stmt
%type <C_TransformationCmd>     tableload_stmt
%type <C_DateTimeSpec>          time_spec
%type <C_DateTimeSpec>          timestamp_spec
%type <C_Usage>                 usage_spec
%type <C_Usage>                 usage_spec_opt
%type <C_SessionInfo>           user_stmt
%type <C_TransformationCmd>     useschemamap_stmt



/* Begin of grammar definition ***********************************************************/
%%

integer_neg_pos_value   : integer_value { $$ = $1; }
                        | opMI integer_value { $$ = -$2; }
                        ;

valBLOCK_COUNT      : integer_value { $$ = $1; }
                    ;
valCOLUMN_ID        : integer_value { $$ = $1; }
                    ;
valEND              : integer_value { $$ = $1; }
                    ;
valERRCOUNT         : integer_value { $$ = $1; }
                    ;
valFRACTION         : integer_value { $$ = $1; }
                    ;
valINCREMENT_SEQNO  : integer_neg_pos_value { $$ = $1; }
                    ;
valISOLATION_LEVEL  : integer_value { $$ = $1; }
                    ;
valRECORD_LENGTH    : integer_value { $$ = $1; }
                    ;
valSCALE_FACTOR     : integer_value { $$ = $1; }
                    ;
valSTART            : integer_value { $$ = $1; }
                    ;
valSTART_SEQNO      : integer_neg_pos_value { $$ = $1; }
                    ;
valTRANSACTION_SIZE : integer_value { $$ = $1; }
                    ;
valUSAGE            : integer_value { $$ = $1; }
                    ;


integer_value       : dtINTEGER_LITERAL
                      {
                          // the delivered strings from parser are NOT terminated by '\0'
                          tsp00_Addr pszValue = NULL;
                          tsp00_Int4 len = 0;
                          STRCPY_LS14(pszValue, len, $1.strAddr_ls00, $1.strLeng_ls00);
                          if (NULL == pszValue)
                          {
                              ls14_ShowError(errNoMoreMemory_els98);
                              YYERROR;
                          }

                          int l = ls07CheckIntValueRange(pszValue);
                          if (0 != l)
                          {
                              $$ = atoi(pszValue);
                          }
                          else  //Error
                          {
                              MsgInstance_ls14->ls98MsgToString(layParser_els98,
                                                                lcpCommand_ls14->cmdRpyDat_ls00,
                                                                errInvalidInteger_els98,
                                                                pszValue);
                              lcpCommand_ls14->cmdReqErr_ls00 = errInvalidInteger_els98;

                              delete [] pszValue;
                              pszValue = NULL;

                              YYERROR;
                          }

                          delete [] pszValue;
                          pszValue = NULL;
                      }
                    ;

// dtDB_IDENTIFIER ist vom typ tls00_String und wird als wert $1 von lex zurueckgeliefert. Der char*-teil
// in $1 ist ein pointer auf den entsprechenden teil im kommando, das gerade analysiert wird. Daher sollte
// bei weitere verarbeitung dieses wertes selbiger erst einmal kopiert werden mittels SAPDB_memcpy oder strncpy.
valBOOL_VALUES      : dtSINGLE_QUOTED_STRING    /* A boolean value may not exceed 10 characters so far */
                        {
                            $$.strAddr_ls00 = $1.strAddr_ls00;
                            $$.strLeng_ls00 = $1.strLeng_ls00;
                        }
                    ;
valCODEPAGE_ID     : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls05StrToUpr ($$.strAddr_ls00, $$.strLeng_ls00);
                        }
                    | dtDNS_NAME
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls05StrToUpr ($$.strAddr_ls00, $$.strLeng_ls00);
                        }
                    | dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                        }
                    ;

valCOLUMN_NAME      : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            ls05StrToUpr($$.strAddr_ls00, $$.strLeng_ls00);
                        }
                    | dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                        }
                    ;

valCOLUMN_NAME_UNCHANGED :  dtDB_IDENTIFIER
                            {
                                $$.strAddr_ls00 = $1.strAddr_ls00; $$.strLeng_ls00 = $1.strLeng_ls00;
                            }
                         | dtDOUBLE_QUOTED_STRING
                            {
                                $$.strAddr_ls00 = $1.strAddr_ls00; $$.strLeng_ls00 = $1.strLeng_ls00;
                            }
                         ;

valDATE_TIME_FORMAT_MASK: dtSINGLE_QUOTED_STRING
                            {
                                $$.strAddr_ls00 = $1.strAddr_ls00; $$.strLeng_ls00 = $1.strLeng_ls00;
                            }
                        ;

/* bei valDBNAME wird der wert selbst veraendert, deshalb muss er umkopiert werden */
valDBNAME           : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls05StrToUpr ($$.strAddr_ls00, $$.strLeng_ls00);
                        }
                    | dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                        }
                    ;

valDBNODE           : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls05StrToUpr ($$.strAddr_ls00, $$.strLeng_ls00);
                        }
                     | dtDNS_NAME
                       {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls05StrToUpr ($$.strAddr_ls00, $$.strLeng_ls00);
                       }
                     | dtURI_REFERENCE
                       {
                           STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                           if ($$.strAddr_ls00 == NULL)
                           {
                               ls14_ShowError(errNoMoreMemory_els98);
                               YYERROR;
                           }
                       }
                     | dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                        }
                    ;

valDELIMITER        : dtSINGLE_QUOTED_STRING
                        {
                            $$.strAddr_ls00 = $1.strAddr_ls00; $$.strLeng_ls00 = $1.strLeng_ls00;
                        }
                    ;

valEBID             : dtSINGLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00-2);
                        }
                    ;

valEND_POS          : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                        }
                    ;

valFILE_NAME        : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00, $1.strLeng_ls00);
                        }
                    | dtSINGLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00-2);
                        }
                    | dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                        }
                    ;

valMAX_POS          : kwMAXROWS
                        {
                        }
                     | kwMAXRECORDS
                        {
                        }
                    ;

valLITERAL          : dtSINGLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00-2);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                        }
                    ;

valNUMBER_LAYOUT     : dtSINGLE_QUOTED_STRING
                            {
                              $$.strAddr_ls00 = $1.strAddr_ls00; $$.strLeng_ls00 = $1.strLeng_ls00;
                            }
                        ;

valOS_USERNAME      : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                        }
                    ;

valREMARK           : dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            else
                            {
                                $$.strLeng_ls00 = $1.strLeng_ls00 - 2;
                            }
                        }
                    ;

valPACKAGEGUID        : dtSINGLE_QUOTED_STRING
                        {
                            $$.strAddr_ls00 = new char[$1.strLeng_ls00 - 1];    // hochkommas werden nicht mit eingetragen
                            memset($$.strAddr_ls00, '\0', $1.strLeng_ls00 - 1);
                            SAPDB_memcpy($$.strAddr_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                        }
                        ;

valPASSWORD         : dtINTEGER_LITERAL
                        {
                            tls00_String &myString = $$;
                            tls00_String &mynextString = $1;
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls14_ChangeUSE_USERString($1);  // manipulate request string to make it printable without password
                        }
                      | dtDB_IDENTIFIER
                        {
                            tls00_String &myString = $$;
                            tls00_String &mynextString = $1;
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls05StrToUpr ($$.strAddr_ls00, $$.strLeng_ls00);

                            ls14_ChangeUSE_USERString($1);  // manipulate request string to make it printable without password
                        }
                      | dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00-2);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls14_ChangeUSE_USERString($1);  // manipulate request string to make it printable without password
                        }
                    ;

valSEPARATOR        : dtSINGLE_QUOTED_STRING
                        {
                            $$.strAddr_ls00 = $1.strAddr_ls00; $$.strLeng_ls00 = $1.strLeng_ls00;
                        }
                    ;

valSTART_POS        : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                        }
                    ;

valTABLE_NAME       : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            ls05StrToUpr($$.strAddr_ls00, $$.strLeng_ls00);
                            bIdentifierIsQuoted_ls14 = false;
                            
                            char* p = $$.strAddr_ls00;
                            int i = 1;
                        }
                    | dtDNS_NAME       // necessary because the parser finds names like owner.table as DNS_NAME
                       {
                            // Coming here the name is a composition of owner name and table name and
                            // it is NOT quoted, so we have to make the string UPPER case
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls05StrToUpr ($$.strAddr_ls00, $$.strLeng_ls00);
                            bIdentifierIsQuoted_ls14 = false;
                            
                            char* p = $$.strAddr_ls00;
                            int i = 1;
                       }
                    | dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                            bIdentifierIsQuoted_ls14 = true;
                        }
                    ;
                    
valSCHEMA_NAME    : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            ls05StrToUpr($$.strAddr_ls00, $$.strLeng_ls00);
                            char* p = $$.strAddr_ls00;
                            int i = 1;
                        }
                    | dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                        }
                    ;                    
                    
valSCHEMAMAP_NAME   : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            ls05StrToUpr($$.strAddr_ls00, $$.strLeng_ls00);
                        }
                    | dtDOUBLE_QUOTED_STRING
                        {
                             STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                        }
                    ;                     

valUNKNOWN_KEYWORD  : dtDB_IDENTIFIER
                        {
                            $$.strAddr_ls00 = $1.strAddr_ls00;
                            $$.strLeng_ls00 = $1.strLeng_ls00;
                        }
                    ;

valUSERKEY          : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls05StrToUpr ($$.strAddr_ls00, $$.strLeng_ls00);
                        }
                    | dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00-2);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                        }
                    ;

valUSERNAME         : dtDB_IDENTIFIER
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, $1.strAddr_ls00, $1.strLeng_ls00);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            ls05StrToUpr($$.strAddr_ls00, $$.strLeng_ls00);
                        }
                    | dtDOUBLE_QUOTED_STRING
                        {
                            STRCPY_LS14($$.strAddr_ls00, $$.strLeng_ls00, &$1.strAddr_ls00[1], $1.strLeng_ls00 - 2);
                            if ($$.strAddr_ls00 == NULL)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                        }
                    ;

/* Load commands: ***********************************************************************/

known_statements    : load_command
                        {
                        }
                    | kwRELEASE
                        {
                            lcpCommand_ls14->cmdScanRC_ls00 = srRelease_els00;
                        }
                    ;

load_command        : session_stmt
                    {
                        if (srUnknown_els00 == lcpCommand_ls14->cmdScanRC_ls00)
                        {
                            lcpCommand_ls14->cmdScanRC_ls00 = srCmdSession_els00;
                        }
                        else
                        {
                            // This is a hack as we cannot syntactical distinguish between
                            // SQL SET statements and Loader SET statements; so all SET statements
                            // are handled in rule set_stmt; in case of SQL SET statement the
                            // parser creates in that rule already a special sql stmt structure and sets the return
                            // value lcpCommand_ls14->cmdScanRC_ls00 to srFctSqlStmt_els00
                            //
                            // To be able to scan/parse only the first N tokens of a statement
                            // without error message of unrecognized further tokens in that statement
                            // we call YYACCEPT here
                            YYACCEPT;
                        }
                    }
                    | remark_stmt
                    {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctRemark_els00;
                    }
                    | transformation_stmt
                    ;

transformation_stmt : createschemamap_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctCreateSchemaMap_els00;
                        }
                    | useschemamap_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctUseSchemaMap_els00;
                        }
                    | dropschemamap_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctDropSchemaMap_els00;
                        }
                    | dbpartextract_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctPartExtract_els00;
                        }
                    | dbpartload_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctPartLoad_els00;
                        }
                    | catalogextract_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctCatExtract_els00;
                        }
                    | catalogload_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctCatLoad_els00;
                        }
                    | tableextract_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctTabExtract_els00;
                        }                        
                    | tableload_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctTabLoad_els00;
                        }
                    | columnextract_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctColExtract_els00;
                        }
                    | columnload_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctColLoad_els00;
                        }  
                    | columnupdate_stmt
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctColUpdate_els00;
                        }
                    | sql_stmt /* REQUEST als SQL-Statement an DB senden */
                        {
                            lcpCommand_ls14->cmdScnDat_ls00 = (tsp00_Addr) $1;
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctSqlStmt_els00;
                            YYACCEPT;
                        }
                        ;

/* session statements: ******************************************************************/

session_stmt        : kwAUTOCOMMIT on_off
                      {
                          lcpDBInfo_ls14->dbiSession_ls00.siAutoCommit_ls00 = $2;
                          if (true == $2)
                          {
                              lcpCommand_ls14->cmdSubFunc_ls00 = sfAutoCommitON_els00;
                          }
                      }
                    | kwINSTALLATION on_off      { lcpDBInfo_ls14->dbiSession_ls00.siInstallOn_ls00  = $2; }
                    | kwMESSAGE      on_off      { lcpDBInfo_ls14->dbiSession_ls00.siMessageOn_ls00  = $2; }
                    | kwSQLMODE      sql_mode    { lcpDBInfo_ls14->dbiSession_ls00.siSQLMode_ls00    = $2; }
                    | kwUSE          schemaname_stmt     
                         { 
                            tls00_SessionInfo *RC;
                            ls14_TemplateNewStruct(RC);
                            if (NULL == RC)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            SAPDB_memcpy (RC, &lcpDBInfo_ls14->dbiSession_ls00, sizeof(tls00_SessionInfo));
                            SAPDB_strcpy (RC->siSchemaName_ls00, $2.strAddr_ls00);
                            lcpCommand_ls14->cmdSubFunc_ls00 = sfUseUser_els00;
                            ScanReturnType = RC;
                         }
                    | kwUSE user_stmt
                        {
                            // dispatcher must delete the structure behind $2
                            ScanReturnType = $2;

                            _tcscpy(lcpCommand_ls14->cmdReqDat_ls00, pszMyUserStmt_ls14);
                            if (NULL != pszMyUserStmt_ls14)     // delete pointer used only for this statement
                            {
                                delete [] pszMyUserStmt_ls14;
                                pszMyUserStmt_ls14 = NULL;
                            }
                        }
                    | kwUSE serverdb_stmt
                        {
                            // Der dispatcher muss die struktur loeschen, die hinter $2 steht
                            ScanReturnType = $2;
                        }
                    | kwSET set_stmt
                    | kwVERSION
                        {
                            ls14_GetVersionInformation();
                        }
                    ;

on_off              : kwON  { $$ = true; }
                    | kwOFF { $$ = false; }
                    ;

sql_mode            : kwINTERNAL    { $$ = sp1sm_internal; }  /* adabas entspricht internal */
                    | kwANSI        { $$ = sp1sm_ansi; }
                    | kwDB2         { $$ = sp1sm_db2; }
                    | kwORACLE      { $$ = sp1sm_oracle; }
                    ;
                    
schemaname_stmt_opt : /* empty */
                        {
                            $$.strAddr_ls00 = NULL;
                        }
                    | schemaname_stmt
                        {
                            $$ = $1;
                        }
                    ;

schemaname_stmt     : kwSCHEMA valSCHEMA_NAME
                      {
                            $$ = $2;                            
                      }
                    ;
                    
createschemamap_stmt : kwCREATESCHEMAMAP valSCHEMAMAP_NAME kwMAPSCHEMAS schema_map_mlt
                       {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct  (RC);     
                            if (NULL == RC)
                                YYERROR;
                            SAPDB_memcpy (&RC->trSchemaMap_ls00, $4, sizeof(tls00_MultSchemaSpec));
                            SAPDB_strcpy ( RC->trSchemaMap_ls00.msSchemaMapName_ls00, $2.strAddr_ls00);
                            if (0 == SAPDB_strcasecmp( RC->trSchemaMap_ls00.msSchemaMapName_ls00, "ID"))
                            {
                                ls14_ShowError(errSchemaMapIdentity);
                                YYERROR;
                            }
                            //RC->trInternal_ls00 = true;
                            $$ = RC;
                                
                       }
useschemamap_stmt   : kwUSESCHEMAMAP  valSCHEMAMAP_NAME
                       { 
                            tls00_TransformationCmd *RC1;
                            ls14_TemplateNewStruct  (RC1);     
                            if (NULL == RC1)
                                YYERROR;
                            tls00_MultSchemaSpec  *RC2;
                            ls14_TemplateNewStruct(RC2);     
                            if (NULL == RC2)
                                YYERROR;
                            SAPDB_strcpy (RC2->msSchemaMapName_ls00, $2.strAddr_ls00);
                            SAPDB_memcpy (&RC1->trSchemaMap_ls00, &RC2, sizeof(tls00_MultSchemaSpec));
                            SAPDB_strcpy ( RC1->trSchemaMap_ls00.msSchemaMapName_ls00, RC2->msSchemaMapName_ls00);
                            RC1->trSchemaMap_ls00.msCount_ls00 = 0;
                            //RC1->trInternal_ls00 = true;
                            $$ = RC1;
                      }                                            
dropschemamap_stmt  : kwDROPSCHEMAMAP  valSCHEMAMAP_NAME
                      {
                            tls00_TransformationCmd *RC1;
                            ls14_TemplateNewStruct  (RC1);     
                            if (NULL == RC1)
                                YYERROR;
                            tls00_MultSchemaSpec  *RC2;
                            ls14_TemplateNewStruct(RC2);     
                            if (NULL == RC2)
                                YYERROR;
                            SAPDB_strcpy ( RC2->msSchemaMapName_ls00, $2.strAddr_ls00);
                            SAPDB_memcpy (&RC1->trSchemaMap_ls00, &RC2, sizeof(tls00_MultSchemaSpec));
                            SAPDB_strcpy ( RC1->trSchemaMap_ls00.msSchemaMapName_ls00, RC2->msSchemaMapName_ls00);
                            if (0 == SAPDB_strcasecmp( RC1->trSchemaMap_ls00.msSchemaMapName_ls00, "ID"))
                            {
                                ls14_ShowError(errSchemaMapIdentity);
                                YYERROR;
                            }
                            RC1->trSchemaMap_ls00.msCount_ls00 = 0;
                            RC1->trInternal_ls00 = true;
                            $$ = RC1;
                      }
                    ;       
                      
user_stmt           : kwUSER valUSERNAME password_spec schemaname_stmt_opt serverdb_stmt_opt isolevel_spec_opt nolog_opt
                        {
                            if ( (0 != $6) &&
                                 (1 != $6) && (10 != $6) && (15 != $6) &&
                                 (2 != $6) && (20 != $6) &&
                                 (3 != $6) && (30 != $6) )
                            {
                                MsgInstance_ls14->ls98MsgToString(layParser_els98, lcpCommand_ls14->cmdRpyDat_ls00, errWrongIsolationLevel_els98);
                                lcpCommand_ls14->cmdReqErr_ls00 = errWrongIsolationLevel_els98;

                                if (NULL != $5->si_pDBNode_ls00)
                                {
                                    delete $5->si_pDBNode_ls00;
                                    $5->si_pDBNode_ls00 = NULL;
                                }
                                delete $5;
                                delete $2.strAddr_ls00;
                                delete $3.strAddr_ls00;
                                YYERROR;
                            }
                            $$ = $5;
                            ls14_SetUser   ($$, $2, $3);
                            if ( $4.strAddr_ls00 != NULL)
                                SAPDB_strcpy($$->siSchemaName_ls00 , $4.strAddr_ls00);
                            else
                                SAPDB_strcpy($$->siSchemaName_ls00 , $2.strAddr_ls00);
                            $$->siDBSessionIsoLevel_ls00 = $6;
                            $$->siNolog_ls00             = $7;

                            lcpCommand_ls14->cmdSubFunc_ls00 = sfUseUser_els00;

                            delete $2.strAddr_ls00;
                            delete $3.strAddr_ls00;
                        }
                    | kwUSERKEY valUSERKEY
                        {
                            tls00_SessionInfo *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }

                            SAPDB_memcpy (RC, &lcpDBInfo_ls14->dbiSession_ls00, sizeof(tls00_SessionInfo));

                            if (NULL != RC->siUserKey_ls00)
                            {
                                delete RC->siUserKey_ls00;
                                RC->siUserKey_ls00 = NULL;
                            }

                            RC->siUserKey_ls00 = new char[$2.strLeng_ls00 + 1];
                            strncpy(RC->siUserKey_ls00, $2.strAddr_ls00, $2.strLeng_ls00);
                            RC->siUserKey_ls00[$2.strLeng_ls00] = '\0';
                            delete $2.strAddr_ls00;

                            $$ = RC;
                        }
                    ;

serverdb_stmt_opt   : /* empty */
                        {
                            tls00_SessionInfo *RC;
                            ls14_TemplateNewStruct(RC);
                            if (NULL == RC)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            SAPDB_memcpy (RC, &lcpDBInfo_ls14->dbiSession_ls00, sizeof(tls00_SessionInfo));
                            ls14_SetDBNode(RC, lcpDBInfo_ls14->dbiSession_ls00.si_pDBNode_ls00);

                            $$ = RC;
                        }
                    | serverdb_stmt
                        {
                            $$ = $1;
                        }
                    ;

serverdb_stmt       : kwSERVERDB valDBNAME node_opt
                        {
                            $$ = $3;
                            ls14_SetDBName ($$, $2);
                            delete $2.strAddr_ls00;
                            
                            SAPDB_Int4 rc = ls14_BuildDatabaseURI($$);
                            if (errOK_els00 != rc)
                            {
                                YYERROR;
                            }
                        }
                    ;

encryption_spec_opt : /* empty */
                        {
                            $$ = epUnknown_els00;
                        }
                    | kwENCRYPTION kwSSL
                        {
                            $$ = epSSL_els00;
                        }
                    ;

password_spec       :   /* password must not be empty */
                          {
                              MsgInstance_ls14->ls98MsgToString(layParser_els98, lcpCommand_ls14->cmdRpyDat_ls00, errNoPasswordSupplied_els98);
                              lcpCommand_ls14->cmdReqErr_ls00 = errNoPasswordSupplied_els98;
                              YYERROR;
                          }
                      | valPASSWORD
                          {
                              $$ = $1;
                          }
                      ;

node_opt            : /* empty */
                        {
                            tls00_SessionInfo *RC;
                            ls14_TemplateNewStruct(RC);     // neue struktur anlegen
                            if (NULL == RC)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            SAPDB_memcpy (RC, &lcpDBInfo_ls14->dbiSession_ls00, sizeof(tls00_SessionInfo));
                            ls14_SetDBNode(RC, lcpDBInfo_ls14->dbiSession_ls00.si_pDBNode_ls00);
                            
                            // siEncryptionProtocol_ls00 is per default set to epUnknown_els00
                            
                            $$ = RC;
                        }
                    | kwON valDBNODE encryption_spec_opt
                        {
                            tls00_SessionInfo *RC;
                            ls14_TemplateNewStruct(RC);     // neue struktur anlegen
                            if (NULL == RC)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            SAPDB_memcpy (RC, &lcpDBInfo_ls14->dbiSession_ls00, sizeof(tls00_SessionInfo));
                            
                            RC->siEncryptionProtocol_ls00 = $3;
                            ls14_SetDBNode(RC, lcpDBInfo_ls14->dbiSession_ls00.si_pDBNode_ls00, &$2);

                            delete $2.strAddr_ls00;

                            $$ = RC;
                        }
                    ;

nolog_opt           : /* empty */
                        {
                            $$ = false;
                        }
                    | kwNOLOG
                        {
                            $$ = true;
                        }
                    ;

set_stmt            :/* empty */
                        {
                            SAPDB_Int4  lPos = 0;
                            char szLoaderProtocol[256];
                            char szLoaderPackages[256];
                            szLoaderProtocol[0] = '\0';
                            szLoaderPackages[0] = '\0';
                            lcpCommand_ls14->cmdRpyDat_ls00[0] = '\0';
                            MsgInstance_ls14->ls98GetLogFileName(szLoaderProtocol);
                            MsgInstance_ls14->ls98GetLoaderPath (szLoaderPackages);
                            lPos = sp77sprintf(lcpCommand_ls14->cmdRpyDat_ls00 + lPos,1024, "Loader protocol: '%s'\n"        , szLoaderProtocol);
                            lPos = sp77sprintf(lcpCommand_ls14->cmdRpyDat_ls00 + lPos,1024, "Loader packages: '%spackages'\n", szLoaderPackages);
                        }
                    | kwCONFIGURATION  configuration_type
                      {
                          lcpDBInfo_ls14->dbiSession_ls00.siConfigurationType_ls00 = $2;
                      }
                    | kwLOGOWNER valOS_USERNAME
                      {
                            tsp00_Int4  rc = errOK_els00;
                            tsp00_C256c szErrorText;
                            char *pszUserName = $2.strAddr_ls00;

                            rc = MsgInstance_ls14->ls98ChangeLogFileOwner(pszUserName, szErrorText);
                            if (errOK_els00 != rc)
                            {
                                MsgInstance_ls14->ls98MsgToString(layParser_els98, lcpCommand_ls14->cmdRpyDat_ls00,
                                                                  errChangeLogFileOwner_ls98, pszUserName, szErrorText.asCharp());
                                lcpCommand_ls14->cmdReqErr_ls00 = errChangeLogFileOwner_ls98;
                            }
                       }
                     | kwBACKUPTOOL backup_type
                       {
                         lcpDBInfo_ls14->dbiSession_ls00.siBackupTool_ls00 = $2;
                       }
                     | kwBOOLEAN valLITERAL
                       {
                            // setzt den defaultwert fuer boolwerte um fuer diese session

                            tsp00_Longint Index = strchr($2.strAddr_ls00, SLASH_LS00) - $2.strAddr_ls00;
                            if ( (Index < 0)                                    ||
                                 (Index > MAX_BOOLVALUE_LENGTH_LS00)            ||
                                 ($2.strLeng_ls00 > 2*MAX_BOOLVALUE_LENGTH_LS00) )
                            {
                                ls14_ShowError(errWrongBooleanSpec_els98);
                                delete $2.strAddr_ls00;
                                YYERROR;
                            }
                            else
                            {
                                tls00_SessionInfo* SI = &lcpDBInfo_ls14->dbiSession_ls00;
                                SAPDB_memcpy (SI->siTrueVal_ls00, $2.strAddr_ls00, Index);
                                SI->siTrueVal_ls00[Index] = '\0';

                                Index += 1;
                                SAPDB_memcpy (SI->siFalseVal_ls00, &$2.strAddr_ls00[Index], $2.strLeng_ls00 - Index);
                                SI->siFalseVal_ls00[$2.strLeng_ls00 - Index] = '\0';

                                delete $2.strAddr_ls00;
                            }
                        }
                    | kwCHECKCONSTRAINT on_off
                    {
                        lcpDBInfo_ls14->dbiSession_ls00.siCheckConstraint_ls00 = $2;
                    }
                    | kwCODEPAGE codepage_spec
                        {
                            //$$ = $1;

                            // nothing to do here
                        }
                    | kwCODETYPE code_type
                        {   lcpDBInfo_ls14->dbiSession_ls00.siCodeType_ls00 = $2; }
                    | kwCOMPRESSED valLITERAL
                        {
                            tsp00_Int4 rc = errOK_els00;

                            // First and last char MUST BE a SLASH!
                            if ( (SLASH_LS00 != $2.strAddr_ls00[0]) || 
                                 (SLASH_LS00 != $2.strAddr_ls00[$2.strLeng_ls00 - 1]) )
                            {
                                rc = errWrongDelSepSpec_els98;
                            }
                            else
                            {
                                char *_pszTmp = &$2.strAddr_ls00[1];        // Set pointer behind first SLASH to work with

                                // Search for next SLASH that separates specs of separator and delimiter
                                char *_p_slashpos = strchr(_pszTmp, SLASH_LS00);
                                if (NULL == _p_slashpos)    // Suppose: SET COMPRESSED '/''/\'/'
                                {   // No slash found at all? Well, can't be
                                    rc = errWrongDelSepSpec_els98;
                                }
                                else
                                {
                                    tsp00_Uint4 len = _p_slashpos - _pszTmp;
                                    if (len > 0)        // for len == 0 suppose: SET COMPRESSED '//\'/'
                                    {
                                        if (BACKSLASH_LS00 == *(_p_slashpos - 1))   // Suppose: SET COMPRESSED '/\//\'/'
                                        {
                                            _p_slashpos = strchr(_p_slashpos + sizeof(char), SLASH_LS00);
                                            // in case of _p_slashpos = NULL the separator is the backslash itself
                                            // in this case we use the length computed above
                                            if ( (NULL != _p_slashpos) && (_p_slashpos != &$2.strAddr_ls00[$2.strLeng_ls00 - 1]) )
                                            {
                                                len = _p_slashpos - _pszTmp;
                                            }
                                        }
                                    }   // end if (len > 0)


                                    //*
                                    //*     Get separator
                                    //*
                                    char szStr[3];
                                    rc = ls14_GetSeparatorDelimiter(_pszTmp, len, szStr);
                                    if (errOK_els00 == rc)
                                    {
                                        if (DEF_BYTE_LS00 == szStr[0])
                                        {
                                            rc = errWrongDelSepSpec_els98;
                                        }
                                        else
                                        {
                                            // Set seperator in session structure
                                            lcpDBInfo_ls14->dbiSession_ls00.siSeperat_ls00[0] = szStr[0];

                                            //*
                                            //*     Get delimiter
                                            //*
                                            _pszTmp += len*sizeof(char) + sizeof(char);
                                            len = &$2.strAddr_ls00[$2.strLeng_ls00-1] - _pszTmp;
                                            rc = ls14_GetSeparatorDelimiter(_pszTmp, len, szStr);
                                            if (errOK_els00 == rc)
                                            {
                                                // Set delimiter in session structure
                                                lcpDBInfo_ls14->dbiSession_ls00.siDelimit_ls00[0] = szStr[0];
                                            }

                                        }
                                    }   // end if (errOK_els00 == rc)
                                }   // end else of if (NULL == _p_slashpos)
                            }   // end else of if ( (SLASH_LS00 != $2.strAddr_ls00[0]) || ...

                            delete $2.strAddr_ls00;

                            if (errOK_els00 != rc)
                            {
                                ls14_ShowError(errWrongDelSepSpec_els98);
                                YYERROR;
                            }
                        }
                    | kwDATE date_time_type
                        {   
                            lcpDBInfo_ls14->dbiSession_ls00.siDateType_ls00 = $2;
                        }
                    | kwDECIMAL valLITERAL
                        {
                            if ( ($2.strLeng_ls00 < 3)              ||
                                 ($2.strLeng_ls00 > 5)              ||
                                 ($2.strAddr_ls00[0] != SLASH_LS00) ||
                                 ($2.strAddr_ls00[$2.strLeng_ls00 - 1] != SLASH_LS00) )
                            {
                                ls14_ShowError(errWrongDecimalSpec_els98);
                                delete $2.strAddr_ls00;
                                YYERROR;
                            }
                            else
                            {
                                tls00_SessionInfo* SI = &lcpDBInfo_ls14->dbiSession_ls00;
                                if ($2.strAddr_ls00[1] == SLASH_LS00)
                                {
                                    /*SI->siThsdChar_ls00 = DEF_BYTE_LS00;*/
                                    SI->siThsdChar_ls00 = NO_THSD_GROUPS_LS00;
                                }
                                else
                                {
                                    SI->siThsdChar_ls00 = $2.strAddr_ls00[1];
                                }

                                if ($2.strAddr_ls00[$2.strLeng_ls00 - 2] == SLASH_LS00)
                                {
                                    /*SI->siDecPoint_ls00 = DEF_BYTE_LS00;*/
                                    SI->siDecPoint_ls00 = POINT_LS00;
                                }
                                else
                                {
                                    SI->siDecPoint_ls00 = $2.strAddr_ls00[$2.strLeng_ls00 - 2];
                                }

                                delete $2.strAddr_ls00;
                            }
                        }
                    | kwFORMAT date_time_type
                      {
                           lcpDBInfo_ls14->dbiSession_ls00.siFormat_ls00 = $2; 
                           char   szErr[MAX_REPLY_LENGTH_LS00];
                           _TCHAR szCmd[256];

                           tsp00_Int4 lErrorPos = 0;
                           szErr[0] = 0;
                           szCmd[0] = 0;
                           sp77sprintf(szCmd, 1024, "SET FORMAT %s ", pszDateTimeName_ls00[lcpDBInfo_ls14->dbiSession_ls00.siFormat_ls00] );

                           lcpCommand_ls14->cmdReqErr_ls00 = STATIC_CAST(tsp00_Int4, ls13ExecIntSQLStmt(lcpDBInfo_ls14, 0, szCmd, szErr, lErrorPos));

                            if (errOK_els00 != lcpCommand_ls14->cmdReqErr_ls00)
                            {
                                YYERROR;
                            }
                      }
                      | isolevel_spec
                        {
                            // Check ISOLATION LEVEL before doing anything else
                            if ( (0 != $1) &&
                                 (1 != $1) && (10 != $1) && (15 != $1) &&
                                 (2 != $1) && (20 != $1) &&
                                 (3 != $1) && (30 != $1) )
                            {
                                MsgInstance_ls14->ls98MsgToString(layParser_els98, lcpCommand_ls14->cmdRpyDat_ls00, errWrongIsolationLevel_els98);
                                lcpCommand_ls14->cmdReqErr_ls00 = errWrongIsolationLevel_els98;
                                YYERROR;
                            }
                            /* Create a new session info structure and force a reconnect */
                            tls00_SessionInfo *RC;
                            ls14_TemplateNewStruct(RC);
                            if (NULL == RC)
                            {
                                ls14_ShowError(errNoMoreMemory_els98);
                                YYERROR;
                            }
                            SAPDB_memcpy(RC, &lcpDBInfo_ls14->dbiSession_ls00, sizeof(tls00_SessionInfo));
                            RC->siDBSessionIsoLevel_ls00 = $1;
                            lcpDBInfo_ls14->dbiLoaderSessionIsoLevel_ls00 = $1;

                            // dispatcher must delete the structure behind $2
                            ScanReturnType = RC;
                        }
                      | kwLANGUAGE valLITERAL
                        {
                            bool bErr = false;
                            if ($2.strLeng_ls00 != 3)
                            {
                                bErr = true;
                            }
                            else
                            {
                                ls05StrToUpr($2.strAddr_ls00, $2.strLeng_ls00);
                                if ( (0 == memcmp("DEU", $2.strAddr_ls00, $2.strLeng_ls00))  ||
                                     (0 == memcmp("ENG", $2.strAddr_ls00, $2.strLeng_ls00) ) )
                                {
                                    SAPDB_memcpy(lcpDBInfo_ls14->dbiSession_ls00.siLanguage_ls00,
                                           $2.strAddr_ls00,
                                           $2.strLeng_ls00);
                                }
                                else
                                {
                                    bErr = true;
                                }
                            }
                            delete $2.strAddr_ls00;
                            if (true == bErr)
                            {
                                MsgInstance_ls14->ls98MsgToString(layParser_els98,
                                                                  lcpCommand_ls14->cmdRpyDat_ls00,
                                                                  errWrongLangSpec_els98);
                                lcpCommand_ls14->cmdReqErr_ls00 = errWrongLangSpec_els98;
                                YYERROR;
                            }
                        }
                    | kwMAXERRORCOUNT valERRCOUNT
                        {
                            if (0 != $2)        // in case the user has set the error count to 0
                            {                   // use the default value 1
                                lcpDBInfo_ls14->dbiSession_ls00.siMaxAdmissibleErrors = $2;
                            }
                        }
                    | kwNULL valLITERAL
                        {
                            if ($2.strLeng_ls00 > MAX_DEFNULL_SPEC_LENGTH_LS00)
                            {
                                ls14_ShowError(errWrongNullSpec_els98);
                                delete $2.strAddr_ls00;
                                YYERROR;
                            }
                            else
                            {
                                tls00_SessionInfo* SI = &lcpDBInfo_ls14->dbiSession_ls00;
                                memset (SI->siNullSpec_ls00, BLANK_LS00, sizeof (tsp00_C20));
                                SAPDB_memcpy (SI->siNullSpec_ls00.asCharp(), $2.strAddr_ls00, $2.strLeng_ls00);
                                delete $2.strAddr_ls00;
                            }
                        }
                    | kwODBCTRACE on_off
                    {
                         lcpDBInfo_ls14->dbiSession_ls00.siODBCTrace_ls00 = $2;
                    }
                    | kwSYSTEMATTRIBUTE on_off
                    {
                         lcpDBInfo_ls14->dbiSession_ls00.siSystemAttribute_ls00 = $2;
                    }
                    | kwTIME date_time_type
                        {   lcpDBInfo_ls14->dbiSession_ls00.siTimeType_ls00 = $2; }
                    | kwTIMESTAMP date_time_type
                        {   lcpDBInfo_ls14->dbiSession_ls00.siStampType_ls00 = $2;}
                    | kwTRANSACTION kwSIZE  valTRANSACTION_SIZE
                        {
                            lcpDBInfo_ls14->dbiSession_ls00.siTASize_ls00 = $3;
                        }
                    | kwCURRENT_SCHEMA opEQ valSCHEMA_NAME
                        {
/*                            tls00_DBSqlCmd *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;

                            RC->dscStmtLen_ls00 = STATIC_CAST( tsp00_Int4, strlen(lcpCommand_ls14->cmdReqDat_ls00) );
                            RC->dscSqlStmt_ls00 = new _TCHAR[RC->dscStmtLen_ls00 + 1];
                            SAPDB_memcpy(RC->dscSqlStmt_ls00, lcpCommand_ls14->cmdReqDat_ls00, RC->dscStmtLen_ls00);
                            RC->dscSqlStmt_ls00[RC->dscStmtLen_ls00] = '\0';

                            lcpCommand_ls14->cmdScnDat_ls00 = REINTERPRET_CAST(char*, RC);
                            lcpCommand_ls14->cmdScanRC_ls00 = srFctSqlStmt_els00;
*/


                            // This is a hack. It should better fill the structure ScanReturnType and
                            // trigger ls10ExecChangeUser as it is done by a 'use user' command
                            lcpCommand_ls14->cmdReqErr_ls00 = ls15SwitchSchema(lcpDBInfo_ls14,
                                                                               REINTERPRET_CAST(SAPDB_UTF8*, $3.strAddr_ls00),
                                                                               lcpCommand_ls14->cmdRpyDat_ls00);
                            if (errOK_els00 != lcpCommand_ls14->cmdReqErr_ls00)
                            {
                                YYERROR;
                            }
                            else
                            {
                                SAPDB_strcpy(lcpDBInfo_ls14->dbiSession_ls00.siSchemaName_ls00 , $3.strAddr_ls00);
                            }
                        }
                    ;

date_time_type      : kwINTERNAL { $$ = dtInternal_els00; }
                    | kwEUR      { $$ = dtEUR_els00; }
                    | kwISO      { $$ = dtISO_els00; }
                    | kwJIS      { $$ = dtJIS_els00; }
                    | kwUSA      { $$ = dtUSA_els00; }
                    ;

backup_type         : kwNSR  { $$ = btTypeNSR_ls00;  }
                    | kwADSM { $$ = btTypeADSM_ls00; }
                    | kwBACK { $$ = btTypeBACK_ls00; }
                    ;

configuration_type  : kwINCLUDE  { $$ = ctInclude_els00;}
                    | kwEXCLUDE  { $$ = ctExclude_els00;}

                    ;
code_type           : kwASCII  { $$ = ctAscii_els00; }
                    | kwEBCDIC { $$ = ctEbcdic_els00; }
                    | kwUCS2
                      {
                          short swapTest = 1;
                          $$ = (*REINTERPRET_CAST(char*, &swapTest)) ? ctUCS2Swapped_els00 : ctUCS2_els00;
                      }
                    | kwUTF8   { $$ = ctUTF8_els00; }
                    ;

codepage_spec       : /*empty*/
                        {
                            // This rule doesn't return anything valuable but rather update the code page
                            // structure in session info

                            tls00_SessionInfo* pSI = &lcpDBInfo_ls14->dbiSession_ls00;

                            // Initialize code page structure with default code page (ISO-8859-1) if
                            // not already set
                            if (0 != _tcsnicmp(pSI->siCodePage_ls00.name.asCharp(),
                                               pszDefaultCodePageName_ls00,
                                               pSI->siCodePage_ls00.name.length()))
                            {
                                pSI->siCodePage_ls00.name.Init();
                                pSI->siCodePage_ls00.name.rawAssign(pszDefaultCodePageName_ls00);

                                memset(pSI->siCodePage_ls00.map, 0, sizeof(pSI->siCodePage_ls00.map));

                                lcpCommand_ls14->cmdReqErr_ls00 = STATIC_CAST(tsp00_Int4, ls15LoadCodePage(lcpDBInfo_ls14,lcpCommand_ls14->cmdRpyDat_ls00));
                            }

                            if (errOK_els00 != lcpCommand_ls14->cmdReqErr_ls00)
                            {
                                YYERROR;
                            }
                        }
                    | valCODEPAGE_ID
                        {
                            tls00_SessionInfo* pSI = &lcpDBInfo_ls14->dbiSession_ls00;

                            // Copy code page name to code page structure and initialize mapping table
                            if (0 != _tcsnicmp($1.strAddr_ls00, pSI->siCodePage_ls00.name.asCharp(), $1.strLeng_ls00))
                            {
                                tsp81_CodePage keepCodePage;
                                keepCodePage.name.Init();
                                keepCodePage.name.rawAssign(pSI->siCodePage_ls00.name.asCharp());
                                SAPDB_memcpy(keepCodePage.map, pSI->siCodePage_ls00.map, 256*sizeof(tsp00_Uint2));
                                
                                
                                pSI->siCodePage_ls00.name.Init();
                                pSI->siCodePage_ls00.name.rawAssign($1.strAddr_ls00);

                                memset(pSI->siCodePage_ls00.map, 0, sizeof(pSI->siCodePage_ls00.map));

                                lcpCommand_ls14->cmdReqErr_ls00 = STATIC_CAST(tsp00_Int4, ls15LoadCodePage(lcpDBInfo_ls14, lcpCommand_ls14->cmdRpyDat_ls00));
                                
                                if (errOK_els00 != lcpCommand_ls14->cmdReqErr_ls00)
                                {
                                    // Restore code page
                                    pSI->siCodePage_ls00.name.Init();
                                    pSI->siCodePage_ls00.name.rawAssign(keepCodePage.name.asCharp());
                                    SAPDB_memcpy(pSI->siCodePage_ls00.map, keepCodePage.map, 256*sizeof(tsp00_Uint2));
                                }
                            }

                            delete $1.strAddr_ls00;
                            $1.strAddr_ls00 = 0;

                            if (errOK_els00 != lcpCommand_ls14->cmdReqErr_ls00)
                            {
                                YYERROR;
                            }
                        }
                    ;

isolevel_spec_opt   :   /* empty */
                        {
                            $$ = lcpDBInfo_ls14->dbiLoaderSessionIsoLevel_ls00;
                        }
                    | isolevel_spec
                        {
                            $$ = $1;
                        }
                    ;
isolevel_spec       : kwISOLATION_LEVEL valISOLATION_LEVEL
                        {
                            $$ = $2;
                        }
                    ;

/* remark_stmt ****************************************************************************/

remark_stmt         : kwREMARK valREMARK
                      {
                          tls00_RemarkCmd *RC;
                          ls14_TemplateNewStruct(RC);     
                          if (NULL == RC)
                          {
                              delete [] $2.strAddr_ls00;
                              YYERROR;
                          }

                          RC->dscRemarkLen_ls00 = $2.strLeng_ls00;
                          RC->dscRemark_ls00 = new _TCHAR[RC->dscRemarkLen_ls00 + 1];
                          _tcscpy(RC->dscRemark_ls00, $2.strAddr_ls00);

                          delete [] $2.strAddr_ls00;

                          $$ = RC;
                      }
                    ;
                    
/* transformation_stmt ************************************************************************/

/* transport db part **************************************************************************/

dbpartload_stmt       : kwIMPORT part_spec duplicates_clause_opt map_schema_spec_opt
                                 configurationfile_spec_opt
                                 cataloginfile_spec_opt  
                                 datainfile_spec_opt  load_column_spec_mlt longfile_spec_mlt_opt
                                 dbstatistics_infile_opt
                                 packagefile_spec_opt
                                 restartpackage_opt
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct  (RC);     
                            if (NULL == RC)
                                YYERROR;

                            RC->trTransSpec_ls00 = tsImport_ls00;
                            SAPDB_memcpy (&RC->trPartSpec_ls00         , $2 , sizeof (tls00_PartSpec));
                            RC->trDplMode_ls00                         = $3;
                            SAPDB_memcpy (&RC->trSchemaMap_ls00        , $4 , sizeof(tls00_MultSchemaSpec));
                            SAPDB_memcpy (&RC->trConfigurationFile_ls00, $5 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy (&RC->trCatalogFile_ls00      , $6 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy (&RC->trDataFile_ls00         , $7 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy (&RC->trColSpec_ls00          , $8 , sizeof (tls00_MultColSpec));
                            SAPDB_memcpy (&RC->trColLongSpec_ls00      , $9 , sizeof (tls00_MultFileSpec));
                            RC->trStatisticFile_ls00 =                   $10;  // Attention: May be the NULL pointer!
                            SAPDB_memcpy (&RC->trPackageFile_ls00,       $11, sizeof (tls00_FileSpec));
                            RC->trRestart_ls00                       =  *$12;
                            
                            delete $2; delete $5; delete $6; delete $7; delete $8; delete $9; delete $11; delete $12;
                            
                            if (true == RC->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00)
                            {
                                // Ignore any encoding for data files in format PAGES
                                RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                            }   

                            if (0 != ls14_CheckTransformationStreams( RC ) )
                            {
                                delete RC;
                                YYERROR;
                            }
                            $$ =RC;

                        }
                        | kwDBLOAD 
                                configurationfile_spec_opt
                                cataloginfile_spec_opt  
                                datainfile_spec_opt  
                                packagefile_spec_opt restartpackage_opt
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct  (RC);     
                            if (NULL == RC)
                                YYERROR;

                            RC->trTransSpec_ls00 = tsImport_ls00;
                            RC->trPartSpec_ls00.psPartType_ls00 = ptAll_els00 ;
                            SAPDB_memcpy(&RC->trConfigurationFile_ls00, $2 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trCatalogFile_ls00      , $3 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trDataFile_ls00         , $4 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trPackageFile_ls00      , $5 , sizeof (tls00_FileSpec));
                            RC->trRestart_ls00                      =  *$6 ;

                            delete $2; delete $3; delete $4; delete $5; delete $6;

                            if (true == RC->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00)
                            {
                                // Ignore any encoding for data files in format PAGES
                                RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                            }

                            if (0 != ls14_CheckTransformationStreams( RC ) )
                            {
                                delete RC;
                                YYERROR;
                            }
                            
                            $$ =RC;
                        } 
                       ;
                       
dbpartextract_stmt    : kwEXPORT  part_spec restore_spec_opt 
                                  configurationfile_spec_opt
                                  catalogoutfile_spec_opt  
                                  dataoutfile_spec_opt
                                  dbstatistics_outfile_opt
                                  packagefile_spec_opt
                                  restartpackage_opt
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;
                                
                            RC->trTransSpec_ls00 = tsExport_ls00;
                            SAPDB_memcpy(&RC->trPartSpec_ls00          , $2 , sizeof (tls00_PartSpec));
                            RC->trRestore_ls00                         = $3;
                            SAPDB_memcpy(&RC->trConfigurationFile_ls00 , $4 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trCatalogFile_ls00       , $5 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trDataFile_ls00          , $6 , sizeof (tls00_FileSpec));
                            RC->trStatisticFile_ls00                   = $7; // Attention: May be the NULL pointer!
                            SAPDB_memcpy(&RC->trPackageFile_ls00,        $8, sizeof (tls00_FileSpec));
                            RC->trRestart_ls00                       =  *$9 ;

                            delete $2; delete $4; delete $5; delete $6; delete $8; delete $9;
                            
                            if (true == RC->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00)
                            {
                                // Ignore any encoding for data files in format PAGES
                                RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                            }
                            else if (true == RC->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
                            {
                                // We must use the encoding type of the database in case of USER|ALL
                                if ( (RC->trPartSpec_ls00.psPartType_ls00 > ptTable_els00) &&
                                     (ctUnknown_els00 == RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) )
                                {
                                    RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = lcpDBInfo_ls14->dbiEncoding_ls00;
                                }
                            }

                            if (ctUnknown_els00 == RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                            {
                                if (false == RC->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00)
                                {
                                    RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                                }
                                else
                                {
                                    RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                                }
                            }


                            if (0 != ls14_CheckTransformationStreams( RC ) )
                            {
                                delete RC;
                                YYERROR;
                            }
                            
                            $$ =RC;
                        }
                        | kwDBEXTRACT 
                                  configurationfile_spec_opt
                                  catalogoutfile_spec  
                                  dataoutfile_spec_opt
                                  packagefile_spec_opt restartpackage_opt
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct  (RC);     
                            if (NULL == RC)
                                YYERROR;
                                
                            RC->trTransSpec_ls00 = tsExport_ls00;
                            RC->trPartSpec_ls00.psPartType_ls00 = ptAll_els00 ;
                            SAPDB_memcpy(&RC->trConfigurationFile_ls00, $2 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trCatalogFile_ls00, $3 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trDataFile_ls00, $4 , sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trPackageFile_ls00, $5 , sizeof (tls00_FileSpec));
                            RC->trRestart_ls00  =  *$6 ;

                            delete $2; delete $3; delete $4;delete $5;delete $6;
                                                        
                            if (true == RC->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00)
                            {
                                // Ignore any encoding for data files in format PAGES
                                RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                            }
                            else if (true == RC->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
                            {
                                // We must use the encoding type of the database in case of USER|ALL
                                RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = lcpDBInfo_ls14->dbiEncoding_ls00;
                            }

                            if (ctUnknown_els00 == RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                            {
                                if (false == RC->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00)
                                {
                                    RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                                }
                                else
                                {
                                    RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                                }
                            }


                            if (0 != ls14_CheckTransformationStreams( RC ) )
                            {
                                delete RC;
                                YYERROR;
                            }

                            $$ =RC;
                        } 
                       ;                       
                       
/* transport catalog ***********************************************************************/

catalogload_stmt    : kwCATALOGLOAD part_spec 
                                    infile_spec 
                                    packagefile_spec_opt restartpackage_opt internal_opt
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct  (RC);     
                            if (NULL == RC)
                                YYERROR;

                            RC->trTransSpec_ls00 = tsImport_ls00;
                            SAPDB_memcpy (&RC->trPartSpec_ls00    , $2, sizeof (tls00_PartSpec));
                            SAPDB_memcpy (&RC->trCatalogFile_ls00 , $3, sizeof (tls00_FileSpec));
                            SAPDB_memcpy (&RC->trPackageFile_ls00 , $4, sizeof (tls00_FileSpec));
                            if ( RC->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == false &&
                                 RC->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == false    )
                            {
                                 RC->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00  = true;
                                
                            }
                            RC->trRestart_ls00  =*$5;
                            RC->trInternal_ls00 = $6;

                            delete $2; delete $3; delete $4;delete $5;
                            $$ =RC;
                        }
                    ;
                    
catalogextract_stmt : kwCATALOGEXTRACT part_spec 
                                       outfile_spec 
                                       packagefile_spec_opt restartpackage_opt internal_opt
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct  (RC);     
                            if (NULL == RC)
                                YYERROR;
                            RC->trTransSpec_ls00 = tsExport_ls00;
                            SAPDB_memcpy (&RC->trPartSpec_ls00    , $2, sizeof (tls00_PartSpec));
                            SAPDB_memcpy (&RC->trCatalogFile_ls00 , $3, sizeof (tls00_FileSpec));
                            SAPDB_memcpy (&RC->trPackageFile_ls00 , $4, sizeof (tls00_FileSpec));
                            RC->trRestart_ls00                    =*$5;
                            RC->trInternal_ls00                   = $6;
                            if ( RC->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == false &&
                                 RC->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == false    )
                            {
                                 RC->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00  = true;
                                
                            }
                            if (ctUnknown_els00 == RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                            {
                                if (false == RC->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00)
                                {
                                    RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                                }
                                else
                                {
                                    RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                                }
                            }

                            delete $2; delete $3; delete $4;delete $5;
                            $$ =RC;
                        }
                    ;                                        

/* transport table *************************************************************************/

tableextract_stmt   : kwEXPORTTABLE part_spec restore_spec_opt
                                    configurationfile_spec_opt
                                    catalogoutfile_spec_opt
                                    dataoutfile_spec_opt 
                                    packagefile_spec_opt restartpackage_opt internal_opt  exclude_opt
                      {
                          tls00_TransformationCmd *RC;
                          ls14_TemplateNewStruct  (RC);     
                          if (NULL == RC)
                              YYERROR;
                          RC->trTransSpec_ls00 = tsExport_ls00;
                          SAPDB_memcpy(&RC->trPartSpec_ls00         ,  $2, sizeof (tls00_PartSpec));
                          if (RC->trPartSpec_ls00.psPartType_ls00  == ptTable_els00)
                          {
                              RC->trTable_ls00.tsTabOwner_ls00 .rawAssign(RC->trPartSpec_ls00.psTabOwner_ls00.asCharp());
                              RC->trTable_ls00.tsTabSchema_ls00.rawAssign(RC->trPartSpec_ls00.psTabSchema_ls00.asCharp());
                              RC->trTable_ls00.tsTabName_ls00  .rawAssign(RC->trPartSpec_ls00.psTabName_ls00.asCharp());
                          }
                          RC->trRestore_ls00                        =  $3;
                          SAPDB_memcpy(&RC->trConfigurationFile_ls00,  $4, sizeof (tls00_FileSpec));
                          SAPDB_memcpy(&RC->trCatalogFile_ls00      ,  $5, sizeof (tls00_FileSpec));
                          SAPDB_memcpy(&RC->trDataFile_ls00         ,  $6, sizeof (tls00_FileSpec));
                          SAPDB_memcpy(&RC->trPackageFile_ls00      ,  $7, sizeof (tls00_FileSpec));
                          RC->trRestart_ls00                        = *$8;
                          RC->trInternal_ls00                       =  $9;
                          RC->trExclude_ls00                        =  $10;

                          size_t lUserInputLength = strlen(lcpCommand_ls14->cmdReqDat_ls00);
                          RC->trFullUserInput_ls00 = new char[lUserInputLength + 1];
                          SAPDB_memcpy(RC->trFullUserInput_ls00, lcpCommand_ls14->cmdReqDat_ls00, lUserInputLength);
                          RC->trFullUserInput_ls00[lUserInputLength] = '\0';

                          delete $2; delete $4; delete $5; delete $6; delete $7; delete $8;
                                                    
                          if (true == RC->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00)
                          {
                              // Ignore any encoding for data files in format PAGES
                              RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                          }
                          else if (true == RC->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00)
                          {
                              // In case the part type is USER or even ALL we must use the encoding type of the
                              if ( (ctUnknown_els00 == RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) &&
                                   (RC->trPartSpec_ls00.psPartType_ls00 > ptTable_els00) )
                              {
                                  RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = lcpDBInfo_ls14->dbiEncoding_ls00;
                              }
                          }
                          if (ctUnknown_els00 == RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                          {
                              if (false == RC->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00)
                              {
                                  RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                              }
                              else
                              {
                                  RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                              }
                          }
                          if (0 != ls14_CheckTransformationStreams( RC ) )
                          {
                              delete RC;
                              YYERROR;
                          }
                          $$ = RC;
                      }
                      | kwEXPORTTABLE table_spec restore_spec_opt order_clause_opt
                                      configurationfile_spec_opt
                                      catalogoutfile_spec_opt
                                      dataoutfile_spec_opt longfile_spec_mlt_opt
                                      packagefile_spec_opt restartpackage_opt internal_opt  exclude_opt
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct  (RC);     
                            if (NULL == RC)
                                YYERROR;
                            RC->trTransSpec_ls00 = tsExport_ls00;
                            SAPDB_memcpy(&RC->trTable_ls00            ,  $2, sizeof (tls00_TableSpec));
                            RC->trPartSpec_ls00.psPartType_ls00 = ptTable_els00;
                            RC->trPartSpec_ls00.psTabOwner_ls00.rawAssign(RC->trTable_ls00.tsTabOwner_ls00.asCharp());
                            RC->trPartSpec_ls00.psTabName_ls00 .rawAssign(RC->trTable_ls00.tsTabName_ls00.asCharp());
                            RC->trPartSpec_ls00.psTabSchema_ls00.rawAssign(RC->trTable_ls00.tsTabSchema_ls00.asCharp());
                            
                            RC->trRestore_ls00                        =  $3;
                            RC->trSelOrdStr_ls00              = new char[$4.strLeng_ls00 + 1];
                            SAPDB_memcpy(RC->trSelOrdStr_ls00,           $4.strAddr_ls00, $4.strLeng_ls00);
                            RC->trSelOrdStr_ls00[$4.strLeng_ls00] = '\0';
                            SAPDB_memcpy(&RC->trConfigurationFile_ls00,  $5, sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trCatalogFile_ls00      ,  $6, sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trDataFile_ls00,           $7, sizeof (tls00_FileSpec));
                            SAPDB_memcpy(&RC->trColLongSpec_ls00,        $8, sizeof (tls00_MultFileSpec));
                            SAPDB_memcpy(&RC->trPackageFile_ls00,        $9, sizeof (tls00_FileSpec));
                            RC->trRestart_ls00                        =*$10;
                            RC->trInternal_ls00                       = $11;
                            RC->trExclude_ls00                        = $12;

                            delete $2; delete $4.strAddr_ls00; delete $5; delete $6; delete $7; delete $8; delete $9; delete $10;
                            
                            size_t lUserInputLength = strlen(lcpCommand_ls14->cmdReqDat_ls00);
                            RC->trFullUserInput_ls00 = new char[lUserInputLength + 1];
                            SAPDB_memcpy(RC->trFullUserInput_ls00, lcpCommand_ls14->cmdReqDat_ls00, lUserInputLength);
                            RC->trFullUserInput_ls00[lUserInputLength] = '\0';

                            if (true == RC->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00)
                            {
                                // Ignore any encoding for data files in format PAGES
                                RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                            }
                            else
                            {
                                if ( (true == RC->trInternal_ls00) &&
                                     (ctUnknown_els00 == RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) )
                                {
                                    RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = lcpDBInfo_ls14->dbiOI_Encoding_ls00;
                                }
                            }
                            if (ctUnknown_els00 == RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                            {
                                if (false == RC->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00)
                                {
                                    RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                                }
                                else
                                {
                                    RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                                }
                            }
                            if (0 != ls14_CheckTransformationStreams( RC ) )
                            {
                                delete RC;
                                YYERROR;
                            }
                            $$ = RC;
                        }
                    ;
                    
tableload_stmt      : kwIMPORTTABLE part_spec duplicates_clause_opt
                                    configurationfile_spec_opt   
                                    cataloginfile_spec_opt
                                    datainfile_spec_opt  
                                    packagefile_spec_opt restartpackage_opt  internal_opt exclude_opt
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct  (RC);     
                            if (NULL == RC)
                                YYERROR;
                            RC->trTransSpec_ls00 = tsImport_ls00;
                            SAPDB_memcpy (&RC->trPartSpec_ls00         , $2, sizeof (tls00_PartSpec));
                            RC->trDplMode_ls00                         = $3;
                            if (RC->trPartSpec_ls00.psPartType_ls00  == ptTable_els00)
                            {
                                RC->trTable_ls00.tsTabOwner_ls00 .rawAssign(RC->trPartSpec_ls00.psTabOwner_ls00.asCharp());
                                RC->trTable_ls00.tsTabSchema_ls00.rawAssign(RC->trPartSpec_ls00.psTabSchema_ls00.asCharp());
                                RC->trTable_ls00.tsTabName_ls00  .rawAssign(RC->trPartSpec_ls00.psTabName_ls00.asCharp());
                            }
                            SAPDB_memcpy (&RC->trConfigurationFile_ls00, $4, sizeof (tls00_FileSpec));
                            SAPDB_memcpy (&RC->trCatalogFile_ls00      , $5, sizeof (tls00_FileSpec));
                            SAPDB_memcpy (&RC->trDataFile_ls00         , $6, sizeof (tls00_FileSpec));
                            SAPDB_memcpy (&RC->trPackageFile_ls00      , $7, sizeof (tls00_FileSpec));
                            RC->trRestart_ls00                         =*$8;
                            RC->trInternal_ls00                        = $9;
                            RC->trExclude_ls00                         = $10;

                            delete $2; delete $4; delete $5; delete $6; delete $7; delete $8;
                            
                            if (false == RC->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00 &&
                                false == RC->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 )
                            {
                                RC->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00 = true;
                            }
                            
                            // constraint on page format
                            if (true == RC->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00)
                            {
                                // Ignore any encoding for data files in format PAGES
                                RC->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                            }
                            
                            if (0 != ls14_CheckTransformationStreams( RC ) )
                            {
                                delete RC;
                                YYERROR;
                            }

                            $$ =RC;
                        }
                      | kwIMPORTTABLE table_spec duplicates_clause_opt
                                      cataloginfile_spec_opt
                                      datainfile_spec_opt   load_column_spec_mlt longfile_spec_mlt_opt usage_spec_opt
                                      packagefile_spec_opt  restartpackage_opt   internal_opt exclude_opt
                      {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;
                            RC->trOracle_ls00  = false;
                            RC->trTransSpec_ls00 = tsImport_ls00;
                            SAPDB_memcpy(&RC->trTable_ls00, $2, sizeof(tls00_TableSpec));
                            
                            RC->trPartSpec_ls00.psPartType_ls00 = ptTable_els00;
                            RC->trPartSpec_ls00.psTabOwner_ls00.rawAssign(RC->trTable_ls00.tsTabOwner_ls00.asCharp());
                            RC->trPartSpec_ls00.psTabName_ls00 .rawAssign(RC->trTable_ls00.tsTabName_ls00.asCharp());
                            RC->trPartSpec_ls00.psTabSchema_ls00.rawAssign(RC->trTable_ls00.tsTabSchema_ls00.asCharp());
                            RC->trDplMode_ls00                  = $3;
                            SAPDB_memcpy(&RC->trCatalogFile_ls00, $4, sizeof(tls00_FileSpec));
                            SAPDB_memcpy(&RC->trDataFile_ls00,    $5, sizeof(tls00_FileSpec));
                            SAPDB_memcpy(&RC->trColSpec_ls00,     $6, sizeof(tls00_MultColSpec));
                            SAPDB_memcpy(&RC->trColLongSpec_ls00, $7, sizeof(tls00_MultFileSpec));
                            SAPDB_memcpy(&RC->trUsage_ls00,       $8, sizeof(tls00_Usage));
                            SAPDB_memcpy(&RC->trPackageFile_ls00, $9, sizeof(tls00_FileSpec));
                            RC->trRestart_ls00               =   *$10;
                            RC->trInternal_ls00              =    $11;
                            RC->trExclude_ls00               =    $12;

                            delete $2; delete $4; delete $5; delete $6;delete $7;delete $8;delete $9;delete $10;
                            
                            if (0 != ls14_CheckTransformationStreams( RC ) )
                            {
                                delete RC;
                                YYERROR;
                            }
                            if (0 != ls14_CheckFormat(&RC->trColSpec_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoCompress_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoFormatted_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoBinary_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00))
                            {
                                delete RC;
                                YYERROR;
                            }
                            $$ = RC;
                       } 
                    ;
                    
/* transport data *****************************************************************************/

columnload_stmt       :  kwDATALOAD part_spec_all_user 
                                    infile_spec
                                    configurationfile_spec_opt 
                                    packagefile_spec_opt  restartpackage_opt internal_opt exclude_opt
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;

                            RC->trTransSpec_ls00 = tsImport_ls00;
                            SAPDB_memcpy(&RC->trPartSpec_ls00,          $2, sizeof(tls00_PartSpec));
                            SAPDB_memcpy(&RC->trDataFile_ls00,          $3, sizeof(tls00_FileSpec));
                            SAPDB_memcpy(&RC->trConfigurationFile_ls00, $4, sizeof(tls00_FileSpec));
                            SAPDB_memcpy(&RC->trPackageFile_ls00,       $5, sizeof(tls00_FileSpec));

                            RC->trUsage_ls00.uUsageValue_ls00 =  UNDEFINED_LS00;
                            RC->trRestart_ls00  =*$6;
                            RC->trInternal_ls00 = $7;
                            RC->trExclude_ls00  = $8;

                            delete $2; delete $3; delete $4; delete $5;delete $6;

                            $$ = RC;
                        }                    
                    | kwDATALOAD kwTABLE table_spec duplicates_clause_opt 
                          load_column_spec_mlt infile_spec  longfile_spec_mlt_opt
                          restartpackage_opt   internal_opt exclude_opt
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;

                            RC->trOracle_ls00  = false;
                            RC->trTransSpec_ls00 = tsImport_ls00;
                            SAPDB_memcpy(&RC->trTable_ls00,    $3, sizeof(tls00_TableSpec));
                            RC->trDplMode_ls00         = $4;
                            SAPDB_memcpy(&RC->trColSpec_ls00,  $5, sizeof(tls00_MultColSpec));
                            SAPDB_memcpy(&RC->trDataFile_ls00,   $6, sizeof(tls00_FileSpec));
                            SAPDB_memcpy(&RC->trColLongSpec_ls00, $7, sizeof(tls00_MultFileSpec));
                            RC->trUsage_ls00.uUsageValue_ls00 =  UNDEFINED_LS00;
                            RC->trRestart_ls00         =*$8;
                            RC->trInternal_ls00        = $9;
                            RC->trExclude_ls00         = $10;

                            delete $3; delete $5; delete $6; delete $7;delete $8;

                            if (0 != ls14_CheckFormat(&RC->trColSpec_ls00,                 // Reference to MultColumn
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoCompress_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoFormatted_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoBinary_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00))
                            {
                                delete RC;
                                YYERROR;
                            }

                            RC->trPartSpec_ls00.psPartType_ls00 = ptTable_els00;
                            RC->trPartSpec_ls00.psTabOwner_ls00.rawAssign(RC->trTable_ls00.tsTabOwner_ls00.asCharp());
                            RC->trPartSpec_ls00.psTabName_ls00.rawAssign(RC->trTable_ls00.tsTabName_ls00.asCharp());
                            RC->trPartSpec_ls00.psTabSchema_ls00.rawAssign(RC->trTable_ls00.tsTabSchema_ls00.asCharp());
                            $$ = RC;
                        }
                    | kwDATALOAD kwORACLE kwTABLE table_spec  
                        load_column_spec_mlt infile_spec
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;
                            RC->trTransSpec_ls00 = tsImport_ls00;
                            SAPDB_memcpy(&RC->trTable_ls00,     $4, sizeof(tls00_TableSpec));
                            RC->trPartSpec_ls00.psPartType_ls00 = ptTable_els00;
                            RC->trPartSpec_ls00.psTabName_ls00.rawAssign (RC->trTable_ls00.tsTabName_ls00.asCharp());
                            RC->trPartSpec_ls00.psTabOwner_ls00.rawAssign(RC->trTable_ls00.tsTabOwner_ls00.asCharp());
                            SAPDB_memcpy(&RC->trColSpec_ls00,   $5, sizeof(tls00_MultColSpec));
                            SAPDB_memcpy(&RC->trDataFile_ls00,  $6, sizeof(tls00_FileSpec));
                            RC->trOracle_ls00  = true;
                            RC->trDplMode_ls00 = dupUnknown_els00;
                            RC->trUsage_ls00.uUsageValue_ls00 =  UNDEFINED_LS00;

                            delete $4; delete $5; delete $6;

                            if (0 != ls14_CheckFormat(&RC->trColSpec_ls00,                 // Reference to MultColumn
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoCompress_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoFormatted_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoBinary_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00))
                            {
                                delete RC;
                                YYERROR;
                            }
                            $$ = RC;
                        }
                    | kwFASTLOAD usage_spec_opt kwTABLE table_spec
                                 load_column_spec_mlt infile_spec
                        {
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                            {
                                delete $4;  delete $5;  delete $6;
                                YYERROR;
                            }
                            RC->trTransSpec_ls00 = tsImport_ls00;
                            SAPDB_memcpy(&RC->trUsage_ls00,     $2, sizeof(tls00_Usage));
                            RC->trUsage_ls00.uFastMode_ls00 = true;
                            SAPDB_memcpy(&RC->trTable_ls00,     $4, sizeof(tls00_TableSpec));
                            
                            RC->trPartSpec_ls00.psPartType_ls00 = ptTable_els00;
                            RC->trPartSpec_ls00.psTabName_ls00.rawAssign (RC->trTable_ls00.tsTabName_ls00.asCharp());
                            RC->trPartSpec_ls00.psTabOwner_ls00.rawAssign(RC->trTable_ls00.tsTabOwner_ls00.asCharp());
                            RC->trPartSpec_ls00.psTabSchema_ls00.rawAssign(RC->trTable_ls00.tsTabSchema_ls00.asCharp());
                            
                            SAPDB_memcpy(&RC->trColSpec_ls00,   $5, sizeof(tls00_MultColSpec));
                            SAPDB_memcpy(&RC->trDataFile_ls00,  $6, sizeof(tls00_FileSpec));

                            delete $2;  delete $4;  delete $5;  delete $6;
                            
                            if (0 != ls14_CheckFormat(&RC->trColSpec_ls00, 
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoCompress_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoFormatted_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoBinary_ls00,
                                                      RC->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00))
                            {
                                delete RC;
                                YYERROR;
                            }
                            $$ = RC;
                        }
                    ;          
                    
columnextract_stmt    :   kwDATAEXTRACT part_spec order_clause_opt  
                                        extract_files_spec 
                                        restartpackage_opt internal_opt exclude_opt
                        {
                            // dataextract all/user/table records only allowed as internal command
                            if ( ( $2->psPartType_ls00 > ptTable_els00 && false == $4->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 ) ||
                                 ( dtypePipe_ls00 == $4->trCmdFile_ls00.fsDeviceType_ls00 && $6 == false)    )
                            {
                                 MsgInstance_ls14->ls98MsgToString(layParser_els98,
                                                                   lcpCommand_ls14->cmdRpyDat_ls00,
                                                                   errNotImplemented_els98, "Command ");
                                 lcpCommand_ls14->cmdReqErr_ls00 = errNotImplemented_els98;
                                 delete $2; delete $4; delete $5;
                                 YYERROR;
                            }
                            // ATTENTION:
                            // tls00_TransformationCmd created in rule extract_files_spec.

                            if ( ($2->psPartType_ls00 > ptTable_els00) && (0 != $3.strLeng_ls00) )
                            {
                                MsgInstance_ls14->ls98MsgToString(layParser_els98,
                                                                  lcpCommand_ls14->cmdRpyDat_ls00,
                                                                  errNotImplemented_els98, "Command ");
                                lcpCommand_ls14->cmdReqErr_ls00 = errNotImplemented_els98;

                                delete $2; delete $4; delete $5;
                                YYERROR;
                            }

                            //*
                            //* Now fill structure tls00_TransformationCmd
                            //*
                            $$ = $4;

                            $$->trTransSpec_ls00 = tsExport_ls00;
                            SAPDB_memcpy(&$$->trPartSpec_ls00, $2, sizeof(tls00_PartSpec));
                            delete $2;

                            $$->trSelOrdStr_ls00 = new char[$3.strLeng_ls00 + 1];
                            SAPDB_memcpy($$->trSelOrdStr_ls00, $3.strAddr_ls00, $3.strLeng_ls00);
                            $$->trSelOrdStr_ls00[$3.strLeng_ls00] = '\0';
                            $$->trRestart_ls00  = *$5;
                            $$->trInternal_ls00 =  $6;
                            $$->trExclude_ls00  =  $7;
                            size_t lUserInputLength = strlen(lcpCommand_ls14->cmdReqDat_ls00);
                            $$->trFullUserInput_ls00 = new char[lUserInputLength + 1];
                            SAPDB_memcpy($$->trFullUserInput_ls00, lcpCommand_ls14->cmdReqDat_ls00, lUserInputLength);
                            $$->trFullUserInput_ls00[lUserInputLength] = '\0';

                            // depending on the fact wether the command is internally build (by the Loader itself)
                            // or externally (by the user) decide wether to assign a default value to the encoding
                            // type of the data file
                            // In case the command is directly from the user and (s)he did not define an encoding
                            // we pass this information as it is
                            if ( (true == $$->trInternal_ls00) &&
                                 (ctUnknown_els00 == $$->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) )
                            {
                                $$->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = lcpDBInfo_ls14->dbiOI_Encoding_ls00;
                            }
                        }
                     | kwDATAEXTRACT restore_spec part_spec 
                                     order_clause_opt extract_files_spec internal_opt exclude_opt
                        {
                            // dataextract all/user/table records only allowed as internal command
                            if ( ($3->psPartType_ls00 > ptTable_els00)                            &&
                                 ( (false == $5->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) ||
                                   (0     != $4.strLeng_ls00)                                    ) )
                            {
                                MsgInstance_ls14->ls98MsgToString(layParser_els98,
                                                                  lcpCommand_ls14->cmdRpyDat_ls00,
                                                                  errNotImplemented_els98, "Command ");
                                lcpCommand_ls14->cmdReqErr_ls00 = errNotImplemented_els98;

                                delete $3; delete $5;
                                YYERROR;
                            }
                            // Did user specify output file for commands created?
                            if ( (NULL        == $5->trCmdFile_ls00.fsFileName_ls00) &&
                                 (SAPDB_FALSE == $6) &&
                                 (NULL        == $5->trPackageFile_ls00.fsFileName_ls00) )
                            {
                                _TCHAR szValue[16];
                                szValue[0] = 0;
                                switch ($2)
                                {
                                    case resDataload_els00:
                                    {
                                        _tcscpy(szValue, "DATALOAD");
                                        break;
                                    }
                                    case resFastload_els00:
                                    {
                                        _tcscpy(szValue, "FASTLOAD");
                                        break;
                                    }
                                    default:
                                    {
                                        _tcscpy(szValue, "DATAUPDATE");
                                    }
                                }

                                MsgInstance_ls14->ls98MsgToString(layParser_els98,
                                                                  lcpCommand_ls14->cmdRpyDat_ls00,
                                                                  errCtrlFileSpecMissing_els98,
                                                                  szValue);
                                lcpCommand_ls14->cmdReqErr_ls00 = errCtrlFileSpecMissing_els98;
                                delete $3; delete $5;
                                YYERROR;
                            }

                            //*
                            //* Now fill structure tls00_TransformationCmd
                            //*
                            $$ = $5;
                            $$->trRestore_ls00  = $2;
                            $$->trInternal_ls00 = $6;
                            $$->trExclude_ls00  = $7;
                            $$->trTransSpec_ls00 = tsExport_ls00;

                            SAPDB_memcpy(&$$->trPartSpec_ls00, $3, sizeof(tls00_PartSpec));
                            delete $3;

                            $$->trSelOrdStr_ls00 = new char[$4.strLeng_ls00 + 1];
                            SAPDB_memcpy($$->trSelOrdStr_ls00, $4.strAddr_ls00, $4.strLeng_ls00);
                            $$->trSelOrdStr_ls00[$4.strLeng_ls00] = '\0';

                            size_t lUserInputLength = strlen(lcpCommand_ls14->cmdReqDat_ls00);
                            $$->trFullUserInput_ls00 = new char[lUserInputLength + 1];
                            SAPDB_memcpy($$->trFullUserInput_ls00, lcpCommand_ls14->cmdReqDat_ls00, lUserInputLength);
                            $$->trFullUserInput_ls00[lUserInputLength] = '\0';

                            // depending on the fact wether the command is internally build (by the Loader itself)
                            // or externally (by the user) decide wether to assign a default value to the encoding
                            // type of the data file
                            // In case the command is directly from the user and (s)he did not define an encoding
                            // we pass this information as it is
                            if ( (true == $$->trInternal_ls00) &&
                                 (ctUnknown_els00 == $$->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) )
                            {
                                $$->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = lcpDBInfo_ls14->dbiOI_Encoding_ls00;
                            }

                        }
                      | kwDATAEXTRACT select_expression output_column_opt extract_files_spec
                                      internal_opt      exclude_opt
                        {
                            // ATTENTION:
                            // tls00_TransformationCmd created in rule extract_files_spec.

                            if ( (dtypePipe_ls00 == $4->trDataFile_ls00.fsDeviceType_ls00) &&
                                 (false          == $4->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00) &&
                                 (false          == $5))
                            {
                                 MsgInstance_ls14->ls98MsgToString(layParser_els98,
                                                                   lcpCommand_ls14->cmdRpyDat_ls00,
                                                                   errNotImplemented_els98, "Command ");
                                 lcpCommand_ls14->cmdReqErr_ls00 = errNotImplemented_els98;

                                 delete $3; delete $4;
                                 YYERROR;
                            }

                            //*
                            //* Now fill structure tls00_TransformationCmd
                            //*
                            $$ = $4;
                            $$->trTransSpec_ls00 = tsExport_ls00;
                            $$->trRestore_ls00 = resUnknown_els00;

                            size_t lUserInputLength = strlen(lcpCommand_ls14->cmdReqDat_ls00);
                            $$->trFullUserInput_ls00 = new char[lUserInputLength + 1];
                            SAPDB_memcpy($$->trFullUserInput_ls00, lcpCommand_ls14->cmdReqDat_ls00, lUserInputLength);
                            $$->trFullUserInput_ls00[lUserInputLength] = '\0';

                            $$->trSelOrdStr_ls00 = new char[$2.strLeng_ls00 + 1];
                            SAPDB_memcpy($$->trSelOrdStr_ls00, $2.strAddr_ls00, $2.strLeng_ls00);
                            $$->trSelOrdStr_ls00[$2.strLeng_ls00] = '\0';

                            SAPDB_memcpy (&$$->trColSpec_ls00, $3, sizeof(tls00_MultColSpec));
                            delete $3;

                            $$->trInternal_ls00 = $5;
                            $$->trExclude_ls00  = $6;

                            // sort columns by position because we accept in a dataextract
                            // command unsorted column descriptions
                            ls19SortColumns(&$$->trColSpec_ls00);

                            // Da bei dieser auspraegung der dataextract-anweisung eine spaltenliste
                            // angegeben werden kann, muessen wir deren aufbau hier pruefen.
                            tsp00_Int4 rc = ls14_CheckFormatEx(&$$->trColSpec_ls00,
                                                               $$->trDataFile_ls00.fsFormat_ls00.ffoFormatted_ls00,
                                                               $$->trDataFile_ls00.fsFormat_ls00.ffoBinary_ls00,
                                                               $$->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00);
                            if (errOK_els00 != rc)
                            {
                                delete $$->trFullUserInput_ls00;
                                $$->trFullUserInput_ls00 = NULL;
                                delete $$->trSelOrdStr_ls00;
                                $$->trSelOrdStr_ls00 = NULL;
                                delete $$;
                                YYERROR;
                            }

                            // depending on the fact wether the command is internally build (by the Loader itself)
                            // or externally (by the user) decide wether to assign a default value to the encoding
                            // type of the data file
                            // In case the command is directly from the user and (s)he did not define an encoding
                            // we pass this information as it is
                            if ( (true == $$->trInternal_ls00) &&
                                 (ctUnknown_els00 == $$->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00) )
                            {
                                $$->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = lcpDBInfo_ls14->dbiOI_Encoding_ls00;
                            }

                            // Fill part spec at last
                            $$->trPartSpec_ls00.psPartType_ls00 = ptTable_els00;
                       }
                    ;                    
                    
columnupdate_stmt     : kwDATAUPDATE kwTABLE table_spec 
                                   acc_column_spec_mlt set_column_spec_mlt
                                   infile_spec longfile_spec_mlt_opt internal_opt
                        {
                            // check if access columns and set columns are specified
                            if (0 == $4->mcsCount_ls00)
                            {
                                MsgInstance_ls14->ls98MsgToString(layParser_els98,
                                                                  lcpCommand_ls14->cmdRpyDat_ls00,
                                                                  errACCColsMissing_els98);
                                lcpCommand_ls14->cmdReqErr_ls00 = errACCColsMissing_els98;

                                delete $3; delete $4; delete $5; delete $6; delete $7;
                                YYERROR;
                            }

                            if (0 == $5->mcsCount_ls00)
                            {
                                MsgInstance_ls14->ls98MsgToString(layParser_els98,
                                                                  lcpCommand_ls14->cmdRpyDat_ls00,
                                                                  errSETColsMissing_els98);
                                lcpCommand_ls14->cmdReqErr_ls00 = errSETColsMissing_els98;

                                delete $3; delete $4; delete $5; delete $6; delete $7;
                                YYERROR;
                            }


                            // fill dataupdate command structure
                            tls00_TransformationCmd *RC;
                            ls14_TemplateNewStruct  (RC);     
                            if (NULL == RC)
                                YYERROR;

                            RC->trTransSpec_ls00 = tsUpdate_ls00;
                            RC->trInternal_ls00 = $8;

                            SAPDB_memcpy(&RC->trTable_ls00, $3, sizeof(tls00_TableSpec));

                            // allocate new memory for column structure to be filled
                            RC->trColSpec_ls00.mcsCount_ls00 = $4->mcsCount_ls00 + $5->mcsCount_ls00;
                            RC->trSetColIndex_ls00           = $4->mcsCount_ls00;

                            int i = 0;
                            // qualifiers
                            for (i; i < $4->mcsCount_ls00; ++i)
                            {
                                RC->trColSpec_ls00.mcsColumn_ls00[i] = new tls00_Column;
                                SAPDB_memcpy(RC->trColSpec_ls00.mcsColumn_ls00[i], $4->mcsColumn_ls00[i], sizeof(tls00_Column));
                                delete $4->mcsColumn_ls00[i];
                            }
                            //modifiers
                            for (i = 0; i < $5->mcsCount_ls00; ++i)
                            {
                                RC->trColSpec_ls00.mcsColumn_ls00[i+$4->mcsCount_ls00] = new tls00_Column;
                                SAPDB_memcpy(RC->trColSpec_ls00.mcsColumn_ls00[i+$4->mcsCount_ls00], $5->mcsColumn_ls00[i], sizeof(tls00_Column));
                                delete $5->mcsColumn_ls00[i];
                            }
                            SAPDB_memcpy(&RC->trDataFile_ls00,   $6, sizeof(tls00_FileSpec));
                            SAPDB_memcpy(&RC->trColLongSpec_ls00, $7, sizeof(tls00_MultFileSpec));

                            delete $3; delete $4; delete $5; delete $6; delete $7;

                            $$ = RC;
                        }
                    ;                    
                    

/* sql statement *************************************************************/
sql_stmt    : valUNKNOWN_KEYWORD
                {
                    // Found unknown keyword; treat it as SQL statement -> get the whole request string
                    // and put it into sql stmt structure

                    tls00_DBSqlCmd *RC;
                    ls14_TemplateNewStruct(RC);     
                    if (NULL == RC)
                        YYERROR;

                    RC->dscStmtLen_ls00 = STATIC_CAST( tsp00_Int4, strlen(lcpCommand_ls14->cmdReqDat_ls00) );
                    RC->dscSqlStmt_ls00 = new _TCHAR[RC->dscStmtLen_ls00 + 1];
                    SAPDB_memcpy(RC->dscSqlStmt_ls00, lcpCommand_ls14->cmdReqDat_ls00, RC->dscStmtLen_ls00);
                    RC->dscSqlStmt_ls00[RC->dscStmtLen_ls00] = '\0';

                    $$ = RC;
                }
            ;

/****************************************************************************************/

backup_spec_opt         : /* empty */

                        {
                            tls00_BackupIdName * RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;

                            RC->bBackupId_ls00 = butTypeUnknown_els00;

                            $$ = RC;
                        }
                        | kwEBID valEBID

                        {
                            tls00_BackupIdName * RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;

                            RC->bBackupId_ls00     = butTypeEBID_ls00;
                            RC->bBackupIdName_ls00 = new char[$2.strLeng_ls00+1];
                            SAPDB_memcpy(RC->bBackupIdName_ls00, $2.strAddr_ls00, $2.strLeng_ls00);
                            RC->bBackupIdName_ls00[$2.strLeng_ls00] = 0;

                            delete $2.strAddr_ls00;

                            $$ = RC;
                        }
                        ;

devicetype_spec_opt     : /* empty */
                        {
                          $$ = dtypeFile_ls00;
                        }
                        | devicetype_spec
                        ;

devicetype_spec         : kwFILE { $$ = dtypeFile_ls00; }
                        | kwPIPE { $$ = dtypePipe_ls00; }
                        | kwTAPE { $$ = dtypeTape_ls00; }
                        ;


duplicates_clause_opt   : /* empty */
                        {
                            $$ = dupUnknown_els00;
                        }
                        | duplicates_clause { $$ = $1; }
                        ;

duplicates_clause       : kwIGNORE kwDUPLICATES { $$ = dupIgnore_els00; }
                        | kwREJECT kwDUPLICATES { $$ = dupReject_els00; }
                        | kwUPDATE kwDUPLICATES { $$ = dupUpdate_els00; }
                        ;

part_spec               : kwTABLE table_name
                            {
                                tls00_PartSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                // At this point we know that the user name is known from rule table_name
                                RC->psTabOwner_ls00.Init();
                                if (0 != $2->tsTabOwner_ls00.length())
                                {
                                    RC->psTabOwner_ls00.rawAssign($2->tsTabOwner_ls00.asCharp());
                                }
                                RC->psTabSchema_ls00.Init();
                                if (0 != $2->tsTabSchema_ls00.length())
                                {
                                    RC->psTabSchema_ls00.rawAssign($2->tsTabSchema_ls00.asCharp());
                                }

                                RC->psTabName_ls00.Init();
                                RC->psTabName_ls00.rawAssign($2->tsTabName_ls00.asCharp());

                                size_t Len = strlen($2->tsFullTabName_ls00);
                                RC->psFullTabName_ls00 = new char[Len + 1];
                                SAPDB_memcpy(RC->psFullTabName_ls00, $2->tsFullTabName_ls00, Len);
                                RC->psFullTabName_ls00[Len] = '\0';

                                RC->psPartType_ls00 = ptTable_els00;

                                delete [] $2->tsFullTabName_ls00;
                                delete $2;
                                $$ = RC;
                            }
                            
                         | kwSCHEMA valSCHEMA_NAME
                            {
                                tls00_PartSpec *RC;
                                ls14_TemplateNewStruct(RC);   
                                if (NULL == RC)
                                    YYERROR;
                                RC->psPartType_ls00 = ptSchema_els00;
                                RC->psTabOwner_ls00.Init();
                                RC->psTabOwner_ls00.rawAssign(lcpDBInfo_ls14->dbiSession_ls00.siUserName_ls00.asCharp());
                                RC->psTabSchema_ls00.Init();
                                RC->psTabSchema_ls00.rawAssign($2.strAddr_ls00);
                                $$ = RC;
                            }
                          | kwUSER
                            {
                                tls00_PartSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->psPartType_ls00 = ptUser_els00;
                                RC->psTabOwner_ls00.Init();
                                RC->psTabOwner_ls00.rawAssign(lcpDBInfo_ls14->dbiSession_ls00.siUserName_ls00.asCharp());
                                $$ = RC;
                            }  
                          | kwDB
                          {
                                tls00_PartSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->psPartType_ls00 = ptAll_els00;
                                RC->psTabOwner_ls00.Init();
                                RC->psTabOwner_ls00.rawAssign(lcpDBInfo_ls14->dbiSession_ls00.siUserName_ls00.asCharp());
                                $$ = RC;
                          }  
                          | kwALL
                            {
                                tls00_PartSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->psPartType_ls00 = ptAll_els00;
                                RC->psTabOwner_ls00.Init();
                                RC->psTabOwner_ls00.rawAssign(lcpDBInfo_ls14->dbiSession_ls00.siUserName_ls00.asCharp());

                                $$ = RC;
                            }   
                        ;

part_spec_all_user     :  kwALL
                            {
                                tls00_PartSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->psPartType_ls00 = ptAll_els00;
                                RC->psTabOwner_ls00.Init();
                                RC->psTabOwner_ls00.rawAssign(lcpDBInfo_ls14->dbiSession_ls00.siUserName_ls00.asCharp());

                                $$ = RC;
                            }
                          | kwUSER
                            {
                                tls00_PartSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->psPartType_ls00 = ptUser_els00;
                                RC->psTabOwner_ls00.Init();
                                RC->psTabOwner_ls00.rawAssign(lcpDBInfo_ls14->dbiSession_ls00.siUserName_ls00.asCharp());

                                $$ = RC;
                            }
                          | kwSCHEMA valSCHEMA_NAME
                            {
                                tls00_PartSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->psPartType_ls00 = ptSchema_els00;
                                RC->psTabOwner_ls00.Init();
                                RC->psTabOwner_ls00.rawAssign(lcpDBInfo_ls14->dbiSession_ls00.siUserName_ls00.asCharp());
                                RC->psTabSchema_ls00.Init();
                                RC->psTabSchema_ls00.rawAssign($2.strAddr_ls00);
                                $$ = RC;
                            }
                        ;

order_clause_opt        : /* empty */
                            { $$.strAddr_ls00 = NULL; $$.strLeng_ls00 = 0; }
                        | order_clause
                            { $$.strAddr_ls00 = $1.strAddr_ls00; $$.strLeng_ls00 = $1.strLeng_ls00; }
                        ;

order_clause            : kwORDER kwBY column_names
                            { $$.strAddr_ls00 = $3.strAddr_ls00; $$.strLeng_ls00 = $3.strLeng_ls00; }
                        ;

select_expression       : /* Read in a separate function without internal syntax analysis */
                            {

                                if (ls14_ParseSelect(&$$) != errOK_els00)
                                    YYERROR;
                                else yyclearin;
                            }
                        ;

restore_spec            : kwFOR kwDATALOAD           {  $$ = resDataload_els00;  }
                        | kwFOR kwDATALOAD kwNODDL   {  $$ = resDataloadNoDDL_els00;  }
                        | kwFOR kwDATALOAD kwNODATA  {  $$ = resDataloadNoDATA_els00;  }
                        | kwFOR kwFASTLOAD           {  $$ = resFastload_els00;  }
                        | kwFOR kwFASTLOAD kwNODDL   {  $$ = resFastloadNoDDL_els00;  }
                        | kwFOR kwFASTLOAD kwNODATA  {  $$ = resFastloadNoDATA_els00;  }
                        | kwFOR kwDATAUPDATE         {  $$ = resDataUpdt_els00;  }
                        | kwFOR kwIMPORTTABLE          {  $$ = resTableload_els00;  }
                        | kwFOR kwTABLEFASTLOAD      {  $$ = resTableFastload_els00;  }
                        | kwFOR kwTABLEUPDATE        {  $$ = resTableUpdt_els00;  }
                        ;
                        
restore_spec_opt        :   /* empty */
                        {
                             $$ = resUnknown_els00;
                        }  
                        |
                        restore_spec
                        {
                              $$ = $1;
                        }
                        ;                   
usage_spec_opt      : /* empty */
                        {
                            tls00_Usage *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                            {
                                YYERROR;
                            }

                            RC->uFastMode_ls00      = false;
                            RC->uUsageValue_ls00    = 80;
                            RC->uUsingRows_ls00     = false;
                            $$ = RC;
                        }
                    | kwPAGE
                        {
                            tls00_Usage *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                            {
                                YYERROR;
                            }
                            RC->uFastMode_ls00      = true;
                            RC->uKeySequence_ls00   = true;
                            RC->uNoLog_ls00         = false;
                            $$ = RC;
                        }                             
                    | kwPAGE kwNOLOG 
                        {
                            tls00_Usage *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                            {
                                YYERROR;
                            }
                            RC->uFastMode_ls00      = true;
                            RC->uKeySequence_ls00   = true;
                            RC->uNoLog_ls00         = true;
                            $$ = RC;
                        }                         
                    | usage_spec
                        {
                            $$ = $1;
                        }
                    ;

usage_spec          :  kwRECORD 
                        {
                            tls00_Usage *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                            {
                                YYERROR;
                            }

                            RC->uUsageValue_ls00    = 0;
                            RC->uUsingRows_ls00     = false;
                            RC->uKeySequence_ls00   = false;
                            RC->uFastMode_ls00      = false;
                            $$ = RC;
                        }      
                     | kwPAGE usage_spec
                        {
                            $$ = $2;
                            $$->uFastMode_ls00      = true;
                            $$->uKeySequence_ls00   = false;
                        }
                     | kwWITH valUSAGE sgnPERCENT kwUSAGE
                        {
                            tls00_Usage *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                            {
                                YYERROR;
                            }

                            if (($2 < 50) || ($2 > 100) || ($2 < 0))
                            {
                                ls14_ShowError(errWrongUsagePercent_els98);
                                YYERROR;
                            }
                            RC->uUsageValue_ls00    = $2;
                            RC->uKeySequence_ls00   = false;
                            RC->uUsingRows_ls00     = false;
                            $$ = RC;
                        }
                    | kwWITH valUSAGE kwROWS kwUSAGE
                        {
                            tls00_Usage *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                            {
                                YYERROR;
                            }

                            if (0 == $2)
                            {
                                ls14_ShowError(errWrongUsageRows_els98);
                                YYERROR;
                            }
                            RC->uUsageValue_ls00    = $2;
                            RC->uKeySequence_ls00   = false;
                            RC->uUsingRows_ls00     = true;
                            $$ = RC;
                        }
                     | kwWITH valUSAGE kwRECORD
                        {
                            tls00_Usage *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                            {
                                YYERROR;
                            }

                            if (0 == $2)
                            {
                                ls14_ShowError(errWrongUsageRows_els98);
                                YYERROR;
                            }
                            RC->uUsageValue_ls00    = $2;
                            RC->uKeySequence_ls00   = false;
                            RC->uUsingRows_ls00     = true;
                            $$ = RC;
                        }
                    ;

/* table specification ******************************************************************/

table_spec              : table_name if_condition_opt
                            {
                                $$ = $1;
                                $$->tsCondSpec_ls00 = $2;

                            }
                        ;      
                        
table_name              : valSCHEMA_NAME sgnPOINT valTABLE_NAME
                            {
                                tls00_TableSpec *RC;
                                ls14_TemplateNewStruct(RC);     // create new structure
                                if (NULL == RC)
                                    YYERROR;

                                if ( ($1.strLeng_ls00 > KNL_IDENTIFIER_SIZE) || ($3.strLeng_ls00 > KNL_IDENTIFIER_SIZE) )
                                {
                                    MsgInstance_ls14->ls98MsgToString(layParser_els98, lcpCommand_ls14->cmdRpyDat_ls00,
                                                                                       errDBIdentifierTooLong_els98);
                                    lcpCommand_ls14->cmdReqErr_ls00 = errDBIdentifierTooLong_els98;
                                    
                                    delete [] $1.strAddr_ls00;
                                    delete [] $3.strAddr_ls00;
                                    delete RC;
                                    YYERROR;
                                }
                                else
                                {
                                    RC->tsTabOwner_ls00.Init();
                                    
                                    tsp100_VersionResult vres = ls15CheckKernelVersion(lcpDBInfo_ls14, 7, 6, 0,0);
                                    if (IsOlder_esp100 == vres)
                                    {
                                        RC->tsTabOwner_ls00.rawAssign($1.strAddr_ls00);
                                    }
                                    else
                                    {
                                        RC->tsTabOwner_ls00.rawAssign(lcpDBInfo_ls14->dbiSession_ls00.siUserName_ls00);
                                    }
                                    RC->tsTabSchema_ls00.Init();
                                    RC->tsTabSchema_ls00.rawAssign($1.strAddr_ls00);
                                    delete [] $1.strAddr_ls00;

                                    RC->tsTabName_ls00.Init();
                                    RC->tsTabName_ls00.rawAssign($3.strAddr_ls00);
                                    delete [] $3.strAddr_ls00;
                                    
                                    tsp00_Int4 len = RC->tsTabSchema_ls00.length() + RC->tsTabName_ls00.length() + 8;
                                    RC->tsFullTabName_ls00  = new char[len];
                                    sp77sprintf(RC->tsFullTabName_ls00, len, "\"%s\".\"%s\"", RC->tsTabSchema_ls00.asCharp(),
                                                                                              RC->tsTabName_ls00.asCharp());
                                }
                                $$ = RC;
                            }
                        | valTABLE_NAME
                            {
                                tls00_TableSpec *RC;
                                ls14_TemplateNewStruct(RC);     // create new structure
                                if (NULL == RC)
                                    YYERROR;

                                SAPDB_Bool bNoSchemaSpecified = SAPDB_TRUE;

                                // values coming here are either
                                // - the name alone (as in LOAD TABLE "<tab_name>" ... or LOAD TABLE <tab_name> ...);
                                //   in this case the name is in the right form, i.e. UPPER case or not
                                // or
                                // - as composition of owner name and table name with no quotes 
                                //   (as in LOAD TABLE <owner_name>.<tab_name> ...); in this case the name is
                                //   already UPPER case; we have to separate owner and table name

                                // bIdentifierIsQuoted_ls14 indicates the necessity to separate owner and table name;
                                // in case the name was quoted it is not necessary to search for a separator;
                                if (false == bIdentifierIsQuoted_ls14)
                                {
                                    // Find separator of owner name and table name = .
                                    char* _seppos = strchr($1.strAddr_ls00, POINT_LS00);
                                    if (NULL != _seppos)
                                    {
                                        bNoSchemaSpecified = SAPDB_FALSE;

                                        size_t _tlen = 0;
                                        size_t _olen = 0;
                                        char szTmpName[sizeof(tsp00_KnlIdentifierc)];

                                        _olen = _seppos - $1.strAddr_ls00;
                                        _tlen = $1.strLeng_ls00 - (_olen + sizeof(char));

                                        // Check if identifier length is not exceeded
                                        if ( (_olen > KNL_IDENTIFIER_SIZE) || (_tlen > KNL_IDENTIFIER_SIZE) )
                                        {
                                            MsgInstance_ls14->ls98MsgToString(layParser_els98, lcpCommand_ls14->cmdRpyDat_ls00,
                                                                                               errDBIdentifierTooLong_els98);
                                            lcpCommand_ls14->cmdReqErr_ls00 = errDBIdentifierTooLong_els98;
                                            delete [] $1.strAddr_ls00;
                                            delete RC;
                                            YYERROR;
                                        }
                                        else
                                        {
                                            SAPDB_memcpy(szTmpName, $1.strAddr_ls00, _olen);
                                            szTmpName[_olen] = 0;

                                            // Fill table owner
                                            tsp100_VersionResult vres = ls15CheckKernelVersion(lcpDBInfo_ls14, 7, 6, 0,0);
                                            if (IsOlder_esp100 == vres)
                                            {
                                                RC->tsTabOwner_ls00.rawAssign(szTmpName);
                                            }
                                            else
                                            {
                                                RC->tsTabOwner_ls00.rawAssign(lcpDBInfo_ls14->dbiSession_ls00.siUserName_ls00);
                                            }

                                            // Fill table schema
                                            RC->tsTabSchema_ls00.rawAssign(szTmpName);
                                            
                                            // Fill table name
                                            SAPDB_memcpy(szTmpName, _seppos + sizeof(char), _tlen);
                                            szTmpName[_tlen] = 0;
                                            RC->tsTabName_ls00.rawAssign(szTmpName);
                                        }
                                    }   // end else of if (NULL == _seppos)
                                }   // end else of if (true == bIdentifierIsQuoted_ls14)

                                if (SAPDB_TRUE == bNoSchemaSpecified)
                                {                               
                                    RC->tsTabName_ls00.rawAssign($1.strAddr_ls00);
                                    RC->tsTabOwner_ls00.rawAssign(lcpDBInfo_ls14->dbiSession_ls00.siUserName_ls00);

                                    char* _pszSchema = NULL;

                                    if (0 == lcpDBInfo_ls14->dbiSession_ls00.siSchemaName_ls00)
                                    {
                                        _pszSchema  = RC->tsTabOwner_ls00;
                                    }
                                    else
                                    {
                                        _pszSchema  = lcpDBInfo_ls14->dbiSession_ls00.siSchemaName_ls00;
                                    }
                                    RC->tsTabSchema_ls00.rawAssign(_pszSchema);
                                }

                                size_t _len = RC->tsTabName_ls00.length() + RC->tsTabSchema_ls00.length() + 8;
                                RC->tsFullTabName_ls00  = new char[_len];
                                sp77sprintf(RC->tsFullTabName_ls00, _len, "\"%s\".\"%s\"",
                                                                          RC->tsTabSchema_ls00.asCharp(),
                                                                          RC->tsTabName_ls00.asCharp());
                                delete [] $1.strAddr_ls00;
                                $$ = RC;
                            }
                        ;
                        
if_condition_opt        : /* empty */
                            { $$ = NULL; }

                        | if_condition
                            { $$ = $1; }
                        ;

if_condition            : kwIF condition { $$ = $2; }

                        | kwOTHERWISE
                            {
                                tls00_CondSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cosBoolOp_ls00 = boUnknown_els00;
                                RC->cosOtherwise_ls00 = true;
                                RC->cosDefNull_ls00   = false;
                                $$ = RC;
                            }
                        ;

condition               : simple_condition
                            {
                                tls00_CondSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cosBoolOp_ls00 = boUnknown_els00;
                                RC->cosOtherwise_ls00 = false;
                                RC->cosDefNull_ls00   = false;
                                RC->cosCond_ls00 = $1;
                                $$ = RC;
                            }
                        | sgnOPEN condition sgnCLOSE
                            {
                                $$ = $2;
                            }
                        | condition kwAND condition
                            {
                                tls00_CondSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cosBoolOp_ls00 = boAND_els00;
                                RC->cosOtherwise_ls00 = false;
                                RC->cosDefNull_ls00   = false;
                                RC->cosLeft_ls00  = $1;
                                RC->cosRight_ls00 = $3;
                                $$ = RC;
                            }
                        | condition kwOR condition
                            {
                                tls00_CondSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cosBoolOp_ls00 = boOR_els00;
                                RC->cosOtherwise_ls00 = false;
                                RC->cosDefNull_ls00   = false;
                                RC->cosLeft_ls00  = $1;
                                RC->cosRight_ls00 = $3;
                                $$ = RC;
                            }
                        | kwNOT condition
                            {
                                tls00_CondSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cosBoolOp_ls00    = boNOT_els00;
                                RC->cosOtherwise_ls00 = false;
                                RC->cosDefNull_ls00   = false;
                                RC->cosLeft_ls00 = $2;
                                $$ = RC;
                            }
                        ;

simple_condition        : kwPOS field_pos field_format hex_opt comp_op valLITERAL
                            {
                                tls00_Condition *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->conStart_ls00   = $2->fpoStart_ls00;
                                RC->conEnd_ls00     = $2->fpoEnd_ls00;
                                RC->conFormat_ls00  = $3->ffoFormat_ls00;
                                RC->conFrac_ls00    = $3->ffoFrac_ls00;
                                RC->conIsHex_ls00   = $4;
                                RC->conCompOp_ls00  = $5;
                                RC->conCompVal_ls00 = $6.strAddr_ls00;
                                RC->conValLen_ls00  = STATIC_CAST( tsp00_Int4, strlen($6.strAddr_ls00) );
                                delete $2;
                                delete $3;
                                $$ = RC;
                            }
                        ;

field_pos               : valSTART
                            {
                                // scanner makes sure that values are not negative

                                tls00_FieldPos *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fpoStart_ls00 = $1;
                                RC->fpoEnd_ls00   = 0;
                                $$ = RC;
                            }
                        | valSTART_POS      /*handles pos specs like 100-150 which are treated as DB_IDENTIFIER by the parser*/
                            {
                                tls00_FieldPos *RC;
                                ls14_TemplateNewStruct(RC);     // create new structure
                                if (NULL == RC)
                                    YYERROR;

                                tsp00_Int4 rc = errOK_els00;
                                tsp00_Int4 _start = 0;
                                tsp00_Int4 _end   = 0;
                                
                                rc = ls14_SeparateIntValues($1.strAddr_ls00, '-', &_start, &_end, true);
                                delete [] $1.strAddr_ls00;

                                if (errOK_els00 != rc)
                                {
                                    // Function returns error text
                                    lcpCommand_ls14->cmdReqErr_ls00 = rc;
                                    YYERROR;
                                }
                                RC->fpoStart_ls00 = _start;
                                RC->fpoEnd_ls00   = _end;
                                $$ = RC;
                            }
                        | valSTART opMI valEND
                            {
                                // scanner makes sure that values are not negative

                                tls00_FieldPos *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->fpoStart_ls00 = $1;
                                RC->fpoEnd_ls00 = $3;
                                $$ = RC;
                            }
                        ;

field_format            : char_opt
                            {
                                tls00_FieldFormat *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->ffoFormat_ls00 = ioChar_els00;
                                $$ = RC;
                            }
                        | kwDECIMAL fraction_opt
                            {
                                tls00_FieldFormat *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->ffoFormat_ls00 = ioDecimal_els00;
                                RC->ffoFrac_ls00 = $2;
                                $$ = RC;
                            }
                        | kwZONED fraction_opt
                            {
                                tls00_FieldFormat *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->ffoFormat_ls00 = ioZoned_els00;
                                RC->ffoFrac_ls00 = $2;
                                $$ = RC;
                            }
                        | kwINTEGER
                            {
                                tls00_FieldFormat *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->ffoFormat_ls00 = ioInteger_els00;
                                $$ = RC;
                            }
                        | kwREAL
                            {
                                tls00_FieldFormat *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->ffoFormat_ls00 = ioReal_els00;
                                $$ = RC;
                            }
                        ;

fraction_opt            : /* empty */
                            {
                                $$ = UNDEFINED_LS00;
                            }
                        | fraction
                            {
                                $$ = $1;
                            }
                        ;

fraction                : sgnOPEN valFRACTION sgnCLOSE
                          {
                              if ($2 < 0)   /* Check if fraction value is negative */
                              {
                                  ls14_ShowError(errInvalidFraction_els98);
                                  YYERROR;
                              }
                              $$ = $2;
                          }
                        ;

hex_opt                 : /* empty */ { $$ = false; }
                        | kwHEX      { $$ = true; }
                        ;
char_opt                : /* empty */
                            { $$ = false; }
                        | kwCHAR
                            { $$ = true; }
                        ;

comp_op                 : opEQ { $$ = opEqual_els00; }
                        | opSM { $$ = opSmaller_els00; }
                        | opLA { $$ = opLarger_els00; }
                        | opSE { $$ = opSmallerEqual_els00; }
                        | opLE { $$ = opLargerEqual_els00; }
                        | opNE { $$ = opNotEqual_els00; }
                        ;

/* Column specification *****************************************************************/

acc_column_spec_mlt     : /*leer*/
                            {
                                tls00_MultColSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->mcsCount_ls00 = 0;
                                $$ = RC;
                            }
                        | acc_column_spec acc_column_spec_mlt
                            {
                                tls00_MultColSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->mcsColumn_ls00[0] = $1;
                                RC->mcsCount_ls00 += 1;
                                for (int i=0; i<$2->mcsCount_ls00; i++)
                                {
                                    RC->mcsColumn_ls00[RC->mcsCount_ls00] = $2->mcsColumn_ls00[i];
                                    RC->mcsCount_ls00 += 1;
                                }

                                delete $2;
                                $$ = RC;
                            }
                          | kwOUTFIELDS acc_column_spec acc_column_spec_mlt
                            {
                                tls00_MultColSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->mcsColumn_ls00[0] = $2;
                                RC->mcsCount_ls00 += 1;
                                for (int i=0; i<$3->mcsCount_ls00; i++)
                                {
                                    RC->mcsColumn_ls00[RC->mcsCount_ls00] = $3->mcsColumn_ls00[i];
                                    RC->mcsCount_ls00 += 1;
                                }

                                delete $3;
                                $$ = RC;
                            }
                        ;

acc_column_spec         : key_column_spec    { $$ = $1; }
                        | simple_column_spec { $$ = $1; }
                        ;

key_column_spec         : kwKEY simple_column_spec
                            {
                                $$ = $2;
                                $$->colIsKey_ls00 = true;
                                $$->colIsSet_ls00 = false;
                            }
                        ;

simple_column_spec      : column_descriptor
                            {
                                $$ = $1;
                                $$->colIsKey_ls00 = false;
                                $$->colIsSet_ls00 = false;
                            }
                        | column_assignment
                            {
                                $$ = $1;
                                $$->colIsKey_ls00 = false;
                                $$->colIsSet_ls00 = false;
                            }
                        ;

set_column_spec_mlt     : /* empty */
                             {
                                tls00_MultColSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->mcsCount_ls00 = 0;
                                $$ = RC;
                            }
                         | set_column_spec set_column_spec_mlt
                            {
                                tls00_MultColSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->mcsColumn_ls00[0] = $1;
                                RC->mcsCount_ls00 += 1;
                                for (int i=0; i<$2->mcsCount_ls00; i++)
                                {
                                    RC->mcsColumn_ls00[RC->mcsCount_ls00] = $2->mcsColumn_ls00[i];
                                    RC->mcsCount_ls00 += 1;
                                }
                                delete $2;
                                $$ = RC;
                            }
                        ;

set_column_spec         : kwSET load_column_spec
                            {
                                $$ = $2;
                                $$->colIsKey_ls00 = false;
                                $$->colIsSet_ls00 = true;
                            }
                        ;
               
map_schema_spec_opt   :  /* empty */
                           {
                              tls00_MultSchemaSpec  *RC;
                              ls14_TemplateNewStruct(RC);     
                              if (NULL == RC)
                                  YYERROR;
                              RC->msCount_ls00 = 0;
                              RC->msSchemaMapName_ls00[0] = 0;
                              
                              $$ = RC;
                           }
                        | map_schema_spec
                          {
                              $$ = $1;
                          }
                        ;
                        
map_schema_spec       :  kwMAPSCHEMAS valSCHEMAMAP_NAME
                          {
                              tls00_MultSchemaSpec  *RC;
                              ls14_TemplateNewStruct(RC);     
                              if (NULL == RC)
                                  YYERROR;
                              SAPDB_strcpy (RC->msSchemaMapName_ls00, $2.strAddr_ls00);
                              int i;
                              for (i=0; i<30; i++)
                              {
                                  ls14_TemplateNewStruct(RC->msSchemaMap_ls00[i]);  
                              }
                              $$ = RC;
                          }
                        | kwMAPSCHEMAS schema_map_mlt
                          {
                              $$ = $2;
                          }
                        ; 
                        
schema_map_mlt          : schema_map 
                            {
                               tls00_MultSchemaSpec  *RC;
                               ls14_TemplateNewStruct(RC);
                               if (NULL == RC)
                                   YYERROR;
                               ls14_TemplateNewStruct(RC->msSchemaMap_ls00[0]);   
                               SAPDB_memcpy(RC->msSchemaMap_ls00[0], $1, sizeof(tls00_SchemaMap));
                               RC->msCount_ls00 += 1;
                               $$   = RC;
                            } 
                          | schema_map sgnCOMMA schema_map_mlt
                            {
                               tls00_MultSchemaSpec  *RC;
                               ls14_TemplateNewStruct(RC);
                               if (NULL == RC)
                                   YYERROR;
							 
                               ls14_TemplateNewStruct(RC->msSchemaMap_ls00[0]);   
                               SAPDB_memcpy(RC->msSchemaMap_ls00[0], $1, sizeof(tls00_SchemaMap));
                               RC->msCount_ls00 += 1;
                               
                               int i;
                               int j =     $3->msCount_ls00;
                               for (i=0; i<$3->msCount_ls00; i++)
                               {
                                   int j = RC->msCount_ls00;
                                   
                                   ls14_TemplateNewStruct(RC->msSchemaMap_ls00[j]);  
                                   SAPDB_memcpy(RC->msSchemaMap_ls00[j], $3->msSchemaMap_ls00[i], sizeof(tls00_SchemaMap));
                                   
                                   RC->msCount_ls00 += 1;
                               }
                               delete $1;
                               for (i=0; i<$3->msCount_ls00; i++)
                               {
                                   if (NULL != $3->msSchemaMap_ls00[i])  
                                       delete  $3->msSchemaMap_ls00[i];
                               }
                               delete $3;
                               $$   = RC;
                            } 
                        ;                         
                        
schema_map              : valSCHEMA_NAME kwTO valSCHEMA_NAME
                          {     
                              tls00_SchemaMap  *RC;
                              ls14_TemplateNewStruct(RC);     
                              if (NULL == RC)
                                  YYERROR; 
                                  
                              RC->smSourceSchema_ls00.Init();
                              RC->smSourceSchema_ls00.rawAssign($1.strAddr_ls00);
                              delete [] $1.strAddr_ls00;                
                              
                              RC->smTargetSchema_ls00.Init();
                              RC->smTargetSchema_ls00.rawAssign($3.strAddr_ls00);
                              delete [] $3.strAddr_ls00;   
                              
                              $$ = RC;   
                          }
                        ;
                        
load_column_spec_mlt    : /* empty */
                            {
                                tls00_MultColSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->mcsCount_ls00 = 0;
                                $$ = RC;
                            }
                        | load_column_spec load_column_spec_mlt
                            {
                                tls00_MultColSpec     *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                ls14_TemplateNewStruct(RC->mcsColumn_ls00[0]);   
                                SAPDB_memcpy(RC->mcsColumn_ls00[0], $1, sizeof(tls00_Column));
                                RC->mcsCount_ls00 += 1;

                                int i;
                                for (i=0; i<$2->mcsCount_ls00; i++)
                                {
                                    int j = RC->mcsCount_ls00;

                                    ls14_TemplateNewStruct(RC->mcsColumn_ls00[j]); 
                                    SAPDB_memcpy(RC->mcsColumn_ls00[j], $2->mcsColumn_ls00[i], sizeof(tls00_Column));

                                    RC->mcsCount_ls00 += 1;
                                }

                                delete $1;
                                for (i=0; i<$2->mcsCount_ls00; i++)
                                {
                                    if (NULL != $2->mcsColumn_ls00[i])  
                                        delete  $2->mcsColumn_ls00[i];
                                }
                                delete $2;

                                $$ = RC;
                            }
                            | kwOUTFIELDS load_column_spec load_column_spec_mlt
                            {
                                tls00_MultColSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                ls14_TemplateNewStruct(RC->mcsColumn_ls00[0]);  
                                SAPDB_memcpy(RC->mcsColumn_ls00[0], $2, sizeof(tls00_Column));
                                RC->mcsCount_ls00 += 1;

                                int i;
                                for (i=0; i<$3->mcsCount_ls00; i++)
                                {
                                    int j = RC->mcsCount_ls00;

                                    ls14_TemplateNewStruct(RC->mcsColumn_ls00[j]); 
                                    SAPDB_memcpy(RC->mcsColumn_ls00[j], 
                                    $3->mcsColumn_ls00[i], sizeof(tls00_Column));

                                    RC->mcsCount_ls00 += 1;
                                }

                                delete $2;
                                for (i=0; i<$3->mcsCount_ls00; i++)
                                {
                                    if (NULL != $3->mcsColumn_ls00[i])  
                                        delete  $3->mcsColumn_ls00[i];
                                }
                                delete $3;

                                $$ = RC;
                            }
                        ;
                        
load_column_spec        : column_descriptor null_condition_opt
                            {
                                $$ = $1;
                                $$->colIsKey_ls00 = false;
                                $$->colIsSet_ls00 = false;
                                $$->colNullCond_ls00 = $2;
                            }
                        | column_assignment
                            {
                                $$ = $1;
                                $$->colIsKey_ls00 = false;
                                $$->colIsSet_ls00 = false;
                            }
                        ;

column_descriptor       : valCOLUMN_NAME field_pos format_spec
                            {
                                tls00_Column *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->colName_ls00.Init();
                                RC->colName_ls00.rawAssign($1.strAddr_ls00);
                                delete [] $1.strAddr_ls00;

                                SAPDB_memcpy (&RC->colFPos_ls00, $2, sizeof (tls00_FieldPos));
                                SAPDB_memcpy (&RC->colFormat_ls00, &$3->fspFormat_ls00, sizeof (tls00_FieldFormat));
                                SAPDB_memcpy (&RC->colOption_ls00, &$3->fspOption_ls00, sizeof (tls00_FieldOption));
                                delete $2;
                                delete $3;

                                /* Check the length of binary formatted values */
                                int lValLength = RC->colFPos_ls00.fpoEnd_ls00 - RC->colFPos_ls00.fpoStart_ls00 + 1;

                                tsp00_Int4 rc = ls19CheckFormatSpecLength(&RC->colFormat_ls00, lValLength);
                                if (errOK_els00 != rc)
                                {
                                    MsgInstance_ls14->ls98MsgToString(layParser_els98, lcpCommand_ls14->cmdRpyDat_ls00,
                                                                                       STATIC_CAST(ErrorNumbers, rc),
                                                                                       RC->colName_ls00.asCharp());
                                    lcpCommand_ls14->cmdReqErr_ls00 = STATIC_CAST(ErrorNumbers, rc);

                                    delete RC;

                                    YYERROR;
                                }

                                $$ = RC;
                            }
                        ;

null_condition_opt      : /* empty */     { $$ = NULL; }
                        | null_condition { $$ = $1; }
                        ;

null_condition          : kwNULL if_opt condition { $$ = $3; }
                        | kwDEFAULT kwNULL
                            {
                                tls00_CondSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cosBoolOp_ls00 = boUnknown_els00;
                                RC->cosOtherwise_ls00 = false;
                                RC->cosDefNull_ls00   = true;
                                $$ = RC;

                                /* Allocate new condition member for column */
                                $$->cosCond_ls00 = new tls00_Condition;
                                memset ($$->cosCond_ls00, DEF_BYTE_LS00, sizeof (tls00_Condition));
                            }
                        ;

if_opt                  : /* empty */
                        | kwIF
                        ;

column_assignment       : valCOLUMN_NAME valLITERAL
                            {
                                tls00_Column *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->colName_ls00.Init();
                                RC->colName_ls00.rawAssign($1.strAddr_ls00);

                                delete [] $1.strAddr_ls00;

                                RC->colAssign_ls00.fasAsgnTyp_ls00 = asLiteral_els00;
                                RC->colAssign_ls00.fasAsgnVal_ls00 = $2.strAddr_ls00;
                                RC->colAssign_ls00.fasAsgnLen_ls00 = STATIC_CAST( tsp00_Int2, strlen($2.strAddr_ls00) );
                                $$ = RC;
                            }
                        | valCOLUMN_NAME generate_spec
                            {
                                tls00_Column *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->colName_ls00.Init();
                                RC->colName_ls00.rawAssign($1.strAddr_ls00);

                                delete [] $1.strAddr_ls00;

                                SAPDB_memcpy (&RC->colAssign_ls00, $2, sizeof (tls00_FieldAsgn));
                                delete $2;
                                $$ = RC;
                            }
                        ;

generate_spec           : kwUSER
                            {
                                tls00_FieldAsgn *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fasAsgnTyp_ls00 = asUser_els00;
                                $$ = RC;
                            }
                        | internal_value
                            {
                                tls00_FieldAsgn *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fasAsgnTyp_ls00 = $1;
                                $$ = RC;
                            }
                        | db2_value
                            {
                                tls00_FieldAsgn *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fasAsgnTyp_ls00 = $1;
                                $$ = RC;
                            }
                        | oracle_value
                            {
                                tls00_FieldAsgn *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fasAsgnTyp_ls00 = $1;
                                $$ = RC;
                            }
                        | sequence_number { $$ = $1; }
                        ;

internal_value          : kwUSERGROUP   { $$ = asUsergroup_els00; }
                        | kwSTAMP       { $$ = asStamp_els00; }
                        | kwDATE        { $$ = asDate_els00; }
                        | kwTIME        { $$ = asTime_els00; }
                        | kwTIMESTAMP   { $$ = asTimeStamp_els00; }
                        ;

db2_value               : kwCURRENT kwSQLID     { $$ = asDB2Sqlid_els00; }
                        | kwCURRENT kwDATE      { $$ = asDB2Date_els00; }
                        | kwCURRENT kwTIME      { $$ = asDB2Time_els00; }
                        | kwCURRENT kwTIMESTAMP { $$ = asDB2Timestamp_els00; }
                        ;

oracle_value            : kwSYSDATE     { $$ = asOraSysdate_els00; }
                        | kwUID         { $$ = asOraUid_els00; }
                        ;

sequence_number         : kwSEQNO   /* sequence numbers (start value as long as incremental value) may be negative */
                            {
                                tls00_FieldAsgn *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fasAsgnTyp_ls00 = asSeqno_els00;
                                RC->fasSeqNo_ls00 = 0;
                                RC->fasSeqInc_ls00 = 1;
                                $$ = RC;
                            }
                        | kwSEQNO valSTART_SEQNO
                            {
                                tls00_FieldAsgn *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fasAsgnTyp_ls00 = asSeqno_els00;
                                RC->fasSeqNo_ls00 = $2;
                                RC->fasSeqInc_ls00 = 1;
                                $$ = RC;
                            }
                        | kwSEQNO valSTART_SEQNO valINCREMENT_SEQNO
                            {
                                tls00_FieldAsgn *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fasAsgnTyp_ls00 = asSeqno_els00;
                                RC->fasSeqNo_ls00 = $2;
                                RC->fasSeqInc_ls00 = $3;
                                $$ = RC;
                            }
                        ;

format_spec             : field_format hex_opt options
                            {
                                tls00_FormatSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                SAPDB_memcpy (&RC->fspFormat_ls00, $1, sizeof (tls00_FieldFormat));
                                SAPDB_memcpy (&RC->fspOption_ls00, $3, sizeof (tls00_FieldOption));
                                RC->fspFormat_ls00.ffoIsHex_ls00 = $2;
                                delete $1;
                                delete $3;
                                $$ = RC;
                            }
                        | oracle_format
                            {
                                tls00_FormatSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fspFormat_ls00.ffoFormat_ls00 = $1;
                                RC->fspFormat_ls00.ffoIsHex_ls00 = false;
                                RC->fspOption_ls00.fopScale_ls00 = false;
                                RC->fspOption_ls00.fopRound_ls00 = false;
                                RC->fspOption_ls00.fopTrunc_ls00 = false;
                                $$ = RC;
                            }
                        ;

options                 : /* empty */
                            {
                                tls00_FieldOption *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fopScale_ls00  = false;
                                RC->fopRound_ls00  = false;
                                RC->fopTrunc_ls00  = false;
                                // RC->fopAnyOperation_ls00 has the right value: false
                                $$ = RC;
                            }
                        | scale_spec
                            {
                                tls00_FieldOption *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fopScale_ls00  = true;
                                RC->fopRound_ls00  = false;
                                RC->fopTrunc_ls00  = false;
                                RC->fopScVal_ls00  = $1->fopScVal_ls00;

                                RC->fopAnyOperation_ls00 = true;
                                delete $1;
                                $$ = RC;
                            }
                        | round_or_trunc_spec
                            {
                                tls00_FieldOption *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fopScale_ls00  = false;
                                RC->fopRound_ls00  = $1->fopRound_ls00;
                                RC->fopTrunc_ls00  = $1->fopTrunc_ls00;
                                RC->fopRTVal_ls00  = $1->fopRTVal_ls00;

                                RC->fopAnyOperation_ls00 = true;
                                delete $1;
                                $$ = RC;
                            }
                        | scale_spec round_or_trunc_spec
                            {
                                tls00_FieldOption *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fopScale_ls00  = $1->fopScale_ls00;
                                RC->fopRound_ls00  = $2->fopRound_ls00;
                                RC->fopTrunc_ls00  = $2->fopTrunc_ls00;
                                RC->fopScVal_ls00  = $1->fopScVal_ls00;
                                RC->fopRTVal_ls00  = $2->fopRTVal_ls00;

                                RC->fopAnyOperation_ls00 = true;
                                delete $1;
                                delete $2;
                                $$ = RC;
                            }
                        ;

scale_spec              : kwSCALE valSCALE_FACTOR
                            {
                                tls00_FieldOption *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fopScale_ls00 = true;
                                if ($2 > 127)
                                {
                                    ls14_ShowError(errInvalidScale_els98);

                                    delete RC;
                                    YYERROR;
                                }
                                RC->fopScVal_ls00 = $2;
                                $$ = RC;
                            }
                        | kwSCALE opMI valSCALE_FACTOR
                            {
                                tls00_FieldOption *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fopScale_ls00 = true;
                                if (-$3 < -127)
                                {
                                    ls14_ShowError(errInvalidScale_els98);

                                    delete RC;
                                    YYERROR;
                                }
                                RC->fopScVal_ls00 = -$3;
                                $$ = RC;
                            }
                        ;

round_or_trunc_spec     : kwROUND valFRACTION
                            {
                                if ( ($2 < 0) || ($2 > 18) )
                                {
                                    ls14_ShowError(errInvalidRound_els98);
                                    YYERROR;
                                }

                                tls00_FieldOption *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fopRound_ls00 = true;
                                RC->fopTrunc_ls00 = false;
                                RC->fopRTVal_ls00 = $2;
                                $$ = RC;
                            }
                        | kwTRUNC valFRACTION
                            {
                                if ( ($2 < 0) || ($2 > 18) )
                                {
                                    ls14_ShowError(errInvalidTrunc_els98);
                                    YYERROR;
                                }

                                tls00_FieldOption *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fopTrunc_ls00 = true;
                                RC->fopRound_ls00 = false;
                                RC->fopRTVal_ls00 = $2;
                                $$ = RC;
                            }
                        ;

oracle_format           : kwDATE    { $$ = ioOracleDate_els00; }
                        | kwNUMBER  { $$ = ioOracleNumber_els00; }
                        ;

column_names            : valCOLUMN_NAME_UNCHANGED
                            {
                                $$.strAddr_ls00 = $1.strAddr_ls00;
                                $$.strLeng_ls00 = $1.strLeng_ls00;
                            }
                        | valCOLUMN_NAME_UNCHANGED sgnCOMMA column_names
                            {
                                $$.strAddr_ls00 = $1.strAddr_ls00;
                                $$.strLeng_ls00 = $3.strLeng_ls00 + STATIC_CAST(tsp00_Int4, ($3.strAddr_ls00 - $1.strAddr_ls00));
                            }
                        ;

output_column_opt       : /* empty */
                            {
                                tls00_MultColSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->mcsCount_ls00 = 0;
                                $$ = RC;
                            }
                        | kwOUTFIELDS output_column_lst { $$ = $2 ;}
                        ;

output_column_lst       : output_column
                            {
                                tls00_MultColSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->mcsColumn_ls00[0] = $1;
                                RC->mcsCount_ls00 = 1;
                                $$ = RC;
                            }
                        | output_column output_column_lst
                            {
                                tls00_MultColSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->mcsColumn_ls00[0] = $1;
                                RC->mcsCount_ls00 = 1;
                                for (int i=0; i<$2->mcsCount_ls00; i++)
                                {
                                    RC->mcsColumn_ls00[RC->mcsCount_ls00] = $2->mcsColumn_ls00[i];
                                    RC->mcsCount_ls00 += 1;
                                }
                                delete $2;
                                $$ = RC;
                            }
                        ;

output_column           : column_id_spec  { $$ = $1; }
                        | lit_column_spec { $$ = $1; }
                        ;

column_id_spec          : column_id field_pos format_spec null_assign_opt
                            {
                                tls00_Column *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                if ($1.strAddr_ls00 == NULL)
                                {
                                    RC->colNumber_ls00 = $1.strLeng_ls00;
                                }
                                else
                                {
                                    RC->colName_ls00.Init();
                                    RC->colName_ls00.rawAssign($1.strAddr_ls00);

                                    delete [] $1.strAddr_ls00;      // column_id ist aus valCOLUMN_NAME "gebaut"
                                }
                                SAPDB_memcpy (&RC->colFPos_ls00,   $2, sizeof (tls00_FieldPos));
                                SAPDB_memcpy (&RC->colFormat_ls00, &$3->fspFormat_ls00, sizeof (tls00_FieldFormat));
                                SAPDB_memcpy (&RC->colOption_ls00, &$3->fspOption_ls00, sizeof (tls00_FieldOption));
                                SAPDB_memcpy (&RC->colAssign_ls00, $4, sizeof (tls00_FieldAsgn));
                                delete $2;
                                delete $3;
                                delete $4;

                                // Check the length of binary formatted values
                                int lValLength = RC->colFPos_ls00.fpoEnd_ls00 - RC->colFPos_ls00.fpoStart_ls00 + 1;

                                tsp00_Int4 rc = ls19CheckFormatSpecLength(&RC->colFormat_ls00, lValLength);
                                if (errOK_els00 != rc)
                                {
                                    MsgInstance_ls14->ls98MsgToString(layParser_els98, lcpCommand_ls14->cmdRpyDat_ls00,
                                                                                       STATIC_CAST(ErrorNumbers, rc),
                                                                                       RC->colName_ls00.asCharp());
                                    lcpCommand_ls14->cmdReqErr_ls00 = STATIC_CAST(ErrorNumbers, rc);
                                    YYERROR;
                                }

                                $$ = RC;
                            }
                        ;

column_id               : valCOLUMN_NAME
                            {
                                $$.strAddr_ls00 = $1.strAddr_ls00;
                                $$.strLeng_ls00 = $1.strLeng_ls00;
                          }
                        | valCOLUMN_ID
                            {
                                $$.strAddr_ls00 = NULL;
                                $$.strLeng_ls00 = $1;
                            }
                        ;

null_assign_opt         : /* empty */
                            {
                                tls00_FieldAsgn *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                $$ = RC;
                            }
                        | null_assign
                            {
                                $$ = $1;
                            }
                        ;

null_assign             : if_opt kwNULL kwSET valLITERAL
                            {
                                tls00_FieldAsgn *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                {
                                    YYERROR;
                                }

                                RC->fasAsgnVal_ls00 = $4.strAddr_ls00;
                                RC->fasAsgnLen_ls00 = STATIC_CAST( tsp00_Int4, strlen($4.strAddr_ls00) );
                                RC->fasAsgnTyp_ls00 = asLiteral_els00;

                                $$ = RC;
                            }
                        ;

lit_column_spec         : valLITERAL field_pos
                            {
                                tls00_Column *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->colName_ls00.Init();
                                RC->colName_ls00.rawAssign($1.strAddr_ls00);

                                delete [] $1.strAddr_ls00;

                                RC->colIsLit_ls00 = true;
                                SAPDB_memcpy (&RC->colFPos_ls00, $2, sizeof (tls00_FieldPos));
                                delete $2;
                                $$ = RC;
                            }
                        ;

/* file access spezification ************************************************************/

infile_spec             : kwINFILE devicetype_spec_opt backup_spec_opt valFILE_NAME file_format file_extract_opt noheader_opt
                            {
                                tls00_FileSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fsDeviceType_ls00        = $2;

                                tls00_BackupIdName *pb = $3;
                                if (butTypeUnknown_els00 != $3->bBackupId_ls00)
                                {
                                    RC->fsBackupId_ls00.bBackupId_ls00   = $3->bBackupId_ls00;
                                    RC->fsBackupId_ls00.bBackupIdName_ls00 = $3->bBackupIdName_ls00;
                                }
                                else
                                {
                                    delete $3;
                                }

                                RC->fsFileName_ls00          = $4.strAddr_ls00;
                                SAPDB_memcpy (&RC->fsFormat_ls00,    $5, sizeof (tls00_FileFormat));
                                SAPDB_memcpy (&RC->fsExtract_ls00   ,$6, sizeof(tls00_FileExtract));
                                RC->fsFormat_ls00.ffoBytesPerLine_ls00 = $7;
                                RC->fsAppend_ls00 = false;
                                RC->fsFromOra_ls00 = false;
                                RC->fsLongColID_ls00 = 0;
                                delete $5; delete $6;
                                $$ = RC;
                            }
                           | kwDATA kwINFILE devicetype_spec_opt backup_spec_opt valFILE_NAME file_format file_extract_opt noheader_opt
                            {
                                tls00_FileSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->fsDeviceType_ls00        = $3;

                                tls00_BackupIdName *pb = $4;
                                if (butTypeUnknown_els00 != $4->bBackupId_ls00)
                                {
                                    RC->fsBackupId_ls00.bBackupId_ls00   = $4->bBackupId_ls00;
                                    RC->fsBackupId_ls00.bBackupIdName_ls00 = $4->bBackupIdName_ls00;
                                }
                                else
                                {
                                    delete $4;
                                }

                                RC->fsFileName_ls00          = $5.strAddr_ls00;
                                SAPDB_memcpy (&RC->fsFormat_ls00,    $6, sizeof (tls00_FileFormat));
                                SAPDB_memcpy (&RC->fsExtract_ls00   ,$7, sizeof(tls00_FileExtract));
                                RC->fsFormat_ls00.ffoBytesPerLine_ls00 = $8;
                                RC->fsAppend_ls00 = false;
                                RC->fsFromOra_ls00 = false;
                                RC->fsLongColID_ls00 = 0;
                                delete $6; delete $7;
                                $$ = RC;
                            }
                        ;

configurationfile_spec_opt: /*empty*/
                            {
                                tls00_FileSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                $$ = RC;
                            }
                          | kwCONFIGURATION infile_spec
                            {
                                $$ = $2;

                                // Reinitialize
                                bFileFormatNotSpecified_ls14 = false;
                            }
                        ;                        

cataloginfile_spec_opt  : /*empty*/
                            {
                                tls00_FileSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                // Default encoding for catalog files is UTF8!
                                RC->fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                                
                                $$ = RC;
                            }  
                        | kwCATALOG infile_spec
                            {
                                $$ = $2;
                                // Default encoding for catalog files is UTF8!
                                if (ctUnknown_els00 == $$->fsFormat_ls00.ffoCodeType_ls00)
                                {
                                    $$->fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                                }
                                if(  $$->fsFormat_ls00.ffoDdl_ls00 == false &&
                                     $$->fsFormat_ls00.ffoXml_ls00 == false    )
                                {
                                     $$->fsFormat_ls00.ffoDdl_ls00 = true;
                                }
                                // Reinitialize
                                bFileFormatNotSpecified_ls14  = false;
                            }
                        ;                         

datainfile_spec         : kwDATA infile_spec 
                         {
                             // usage_spec_opt
                             $$ = $2;
                             bDataFileFormatNotSpecified_ls14 = bFileFormatNotSpecified_ls14;
                             // Reinitialize bFileFormatNotSpecified_ls14
                             bFileFormatNotSpecified_ls14 = false;
                         }
                        ;
                        
datainfile_spec_opt    : /*empty*/
                        {
                             tls00_FileSpec *RC;
                             ls14_TemplateNewStruct(RC);     
                             if (NULL == RC)
                                 YYERROR;

                             $$ = RC;
                        }
                        | datainfile_spec
                        {
                            $$ = $1;
                        }
                        ;         

dbstatistics_infile_opt : /*empty*/
                            {
                                /*tls00_FileSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                $$ = RC;*/
                                $$ = NULL;
                            }  
                        | kwSTATISTICS infile_spec
                            {
                                $$ = $2;

                                // Reinitialize
                                bFileFormatNotSpecified_ls14  = false;
                            }
                        ;                                                

outfile_spec_opt        : /*empty*/
                        {
                             tls00_FileSpec *RC;
                             ls14_TemplateNewStruct(RC);     
                             if (NULL == RC)
                                 YYERROR;

                             $$ = RC;
                        }  
                        | outfile_spec
                        {
                             $$ = $1;
                        }
                        ;
outfile_spec            : kwOUTFILE devicetype_spec_opt valFILE_NAME file_format file_extract_opt append_opt
                        {
                            tls00_FileSpec *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;

                            RC->fsDeviceType_ls00        = $2;
                            RC->fsFileName_ls00          = $3.strAddr_ls00;
                            SAPDB_memcpy (&RC->fsFormat_ls00    ,$4, sizeof(tls00_FileFormat ));
                            SAPDB_memcpy (&RC->fsExtract_ls00   ,$5, sizeof(tls00_FileExtract));
                            RC->fsAppend_ls00            = $6;
                            RC->fsFromOra_ls00 = false;
                            RC->fsLongColID_ls00 = 0;

                            tls00_FileFormat *ff = $4;
                            tls00_FileExtract *fe = $5;
                            delete $4;
                            delete $5;
                            $$ = RC;
                        }
                        ;

catalogoutfile_spec_opt : /*empty*/
                        {
                            tls00_FileSpec *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;
                            $$ = RC;
                        }  
                        | catalogoutfile_spec
                        {
                            $$ = $1;
                        }
                        ;                   

catalogoutfile_spec     : kwCATALOG outfile_spec
                        {
                            $$ = $2;
                            if (ctUnknown_els00 == $$->fsFormat_ls00.ffoCodeType_ls00)
                            {
                                $$->fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                            }
                            if(  $$->fsFormat_ls00.ffoDdl_ls00 == false &&
                                 $$->fsFormat_ls00.ffoXml_ls00 == false    )
                            {
                                 $$->fsFormat_ls00.ffoDdl_ls00 = true;
                            }
                            // Reinitialize
                            bFileFormatNotSpecified_ls14 = false;
                        }
                        ;

dataoutfile_spec_opt   : /*empty*/
                        {
                            tls00_FileSpec *RC;
                            ls14_TemplateNewStruct(RC);     
                            if (NULL == RC)
                                YYERROR;
                            $$ = RC;
                        }  
                        | kwDATA outfile_spec
                        {
                            // If  ffoCodeType_ls00 is set to ctUnknown_els00 leave it as it is
                            $$ = $2;
                            bDataFileFormatNotSpecified_ls14 = bFileFormatNotSpecified_ls14;
                            // Reinitialize bFileFormatNotSpecified_ls14
                            bFileFormatNotSpecified_ls14 = false;
                        }
                        ;                        

packagefile_spec_opt    : /*empty*/
                            {
                                tls00_FileSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                $$ = RC;
                            }
                        | kwPACKAGE outfile_spec
                            {
                                // Reinitialize
                                bFileFormatNotSpecified_ls14 = false;

                                $$ = $2;
                            }
                        ;
dbstatistics_outfile_opt : /*empty*/
                            {
                                /*tls00_FileSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                $$ = RC;*/
                                $$ = NULL;
                            }
                        | kwSTATISTICS outfile_spec
                            {
                                // Reinitialize
                                bFileFormatNotSpecified_ls14 = false;

                                $$ = $2;
                            }
                        ;

extract_files_spec      : outfile_spec_opt longfile_spec_mlt_opt 
                          configurationfile_spec_opt 
                          catalogoutfile_spec_opt
                          packagefile_spec_opt
                            {
                                tls00_TransformationCmd *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                {
                                    YYERROR;
                                }
                                SAPDB_memcpy (&RC->trDataFile_ls00, $1, sizeof (tls00_FileSpec));
                                SAPDB_memcpy (&RC->trColLongSpec_ls00, $2, sizeof(tls00_MultFileSpec));
                                SAPDB_memcpy (&RC->trConfigurationFile_ls00, $3, sizeof (tls00_FileSpec));
                                SAPDB_memcpy (&RC->trCatalogFile_ls00, $4, sizeof (tls00_FileSpec));
                                SAPDB_memcpy (&RC->trPackageFile_ls00, $5, sizeof (tls00_FileSpec));

                                delete $1;delete $2;delete $3;delete $4;delete $5;

                                if (ctUnknown_els00 == RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                                {
                                    if (false == RC->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00)
                                    {
                                        RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                                    }
                                    else
                                    {
                                        RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                                    }
                                }
                                $$ = RC;
                            }
                         | kwDATA outfile_spec_opt longfile_spec_mlt_opt 
                                     configurationfile_spec_opt 
                                     catalogoutfile_spec_opt
                                     packagefile_spec_opt
                            {
                                tls00_TransformationCmd *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                {
                                    YYERROR;
                                }
                                SAPDB_memcpy (&RC->trDataFile_ls00, $2, sizeof (tls00_FileSpec));
                                SAPDB_memcpy (&RC->trColLongSpec_ls00, $3, sizeof(tls00_MultFileSpec));
                                SAPDB_memcpy (&RC->trConfigurationFile_ls00, $4, sizeof (tls00_FileSpec));
                                SAPDB_memcpy (&RC->trCatalogFile_ls00, $5, sizeof (tls00_FileSpec));
                                SAPDB_memcpy (&RC->trPackageFile_ls00, $6, sizeof (tls00_FileSpec));

                                delete $2;delete $3;delete $4;delete $5;delete $6;

                                if (ctUnknown_els00 == RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                                {
                                    if (false == RC->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00)
                                    {
                                        RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                                    }
                                    else
                                    {
                                        RC->trCatalogFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                                    }
                                }
                                $$ = RC;
                            }
                        | outfile_spec outfile_spec longfile_spec_mlt_opt
                            {
                                tls00_TransformationCmd *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                {
                                    YYERROR;
                                }

                                SAPDB_memcpy (&RC->trCmdFile_ls00,  $1, sizeof (tls00_FileSpec));
                                SAPDB_memcpy (&RC->trDataFile_ls00,  $2, sizeof (tls00_FileSpec));
                                SAPDB_memcpy (&RC->trColLongSpec_ls00, $3, sizeof (tls00_MultFileSpec));

                                delete $1;
                                delete $2;
                                delete $3;

                                if (ctUnknown_els00 == RC->trCmdFile_ls00.fsFormat_ls00.ffoCodeType_ls00)
                                {
                                    if (false == RC->trCmdFile_ls00.fsFormat_ls00.ffoXml_ls00)
                                    {
                                        RC->trCmdFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctUTF8_els00;
                                    }
                                    else
                                    {
                                        RC->trCmdFile_ls00.fsFormat_ls00.ffoCodeType_ls00 = ctAscii_els00;
                                    }
                                }

                                $$ = RC;
                            }
                        ;

longfile_spec_mlt_opt       : /* empty */
                            {
                                tls00_MultFileSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                {
                                    YYERROR;
                                }

                                $$ = RC;
                            }
                        | longfile_spec longfile_spec_mlt_opt
                            {
                                /*
                                   Der 'leere' Regelteil wird IMMER durchlaufen. Darum wird der dort angelegte
                                   pointer auch in dieser anschliessenden regel fuer alle zu erzeugenden
                                   longfileinfos genutzt. Dazu muessen nur die files bei jedem neu dazukommenden
                                   um eins nach 'hinten' verschoben werden, da die fileinfos von 'hinten'
                                   abgearbeitet werden.
                                */
                                tls00_MultFileSpec *RC;

                                RC = $2;
                                for (int i = RC->mfsCount_ls00; i > 0; --i)
                                {
                                    RC->mfsFiles_ls00[i] = RC->mfsFiles_ls00[i-1];
                                }

                                RC->mfsFiles_ls00[0] = $1;
                                ++RC->mfsCount_ls00;

                                $$ = RC;
                            }
                        ;

longfile_spec           : kwLONGFILE devicetype_spec_opt valCOLUMN_ID valFILE_NAME longfile_code_spec_opt
                            {
                                tls00_FileSpec *RC;
                                if (0 != ls14_FillLongfileStruct($2, $3, &$4, $5, &RC) )
                                {
                                    YYERROR;
                                }

                                $$ = RC;
                            }

                        | kwLONGFILE devicetype_spec_opt valCOLUMN_ID longfile_code_spec
                            {
                                // This rule is only to define the code type of the longfile
                                tls00_FileSpec *RC;
                                if (0 != ls14_FillLongfileStruct($2, $3, NULL, $4, &RC) )
                                {
                                    YYERROR;
                                }

                                $$ = RC;
                            }

                        | kwLONGFILE devicetype_spec_opt valCOLUMN_NAME valFILE_NAME longfile_code_spec_opt
                            {
                                tls00_FileSpec *RC;
                                if (0 != ls14_FillLongfileStruct($2, $3, &$4, $5, &RC) )
                                {
                                    YYERROR;
                                }

                                $$ = RC;
                            }
                        | kwLONGFILE devicetype_spec_opt valCOLUMN_NAME longfile_code_spec
                            {
                                // This rule is only to define the code type of the longfile
                                tls00_FileSpec *RC;
                                if (0 != ls14_FillLongfileStruct($2, $3, NULL, $4, &RC) )
                                {
                                    YYERROR;
                                }

                                $$ = RC;
                            }
                        ;

longfile_code_spec_opt  : /* empty */
                            {
                                tls00_CodeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cdsCodeType_ls00 = ctUnknown_els00;
                                $$ = RC;
                            }
                        | longfile_code_spec
                            {
                                $$ = $1;
                            }
                        ;

longfile_code_spec  :     code_spec
                            {
                                $$ = $1;
                            }
                        | kwBINARY
                            {
                                tls00_CodeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cdsCodeType_ls00 = ctIgnore_els00;
                                $$ = RC;
                            }
                        ;

append_opt              : /* empty */ { $$ = false; }
                        | kwAPPEND    { $$ = true ; }
                        ;

restartpackage_opt      : /* empty */
                        {
                             tls00_Restart * RC;
                             ls14_TemplateNewStruct(RC);     
                             if (NULL == RC)
                                    YYERROR;

                             RC->rstRestart_ls00   = false;

                             $$ = RC;

                        }
                        | kwRESTART valPACKAGEGUID
                        {
                             tls00_Restart * RC;
                             ls14_TemplateNewStruct(RC);     
                             if (NULL == RC)
                                    YYERROR;

                             RC->rstPackageid_ls00.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, $2.strAddr_ls00));
                             RC->rstRestart_ls00   = true;

                             $$ = RC;


                        }
                        ;

internal_opt            : /* empty */ { $$ = false; }
                        | kwINTERNAL  { $$ = true;  }
                        ;

exclude_opt             : /* empty */ { $$ = false; }
                        | kwEXCLUDE   { $$ = true;  }
                        ;

noheader_opt            : /* empty */
                            {
                                $$ = 0;
                            }
                        | kwNOHEADER valRECORD_LENGTH
                            {
                                $$ = $2;
                            }
                        ;


file_format             : file_format_spec
                            {
                                //*
                                //*  Use defaults if nothing has been specified by the user
                                //*
                                tls00_SessionInfo* SI = &lcpDBInfo_ls14->dbiSession_ls00;
                                $$ = $1;

                                /*
                                   Nothing specified for $$->ffoCodeType_ls00;
                                   leave the value on ctUnknown_els00 and handle it later
                                   in parser or in loader function
                                */
                                if ($$->ffoCodeType_ls00 == ctUnknown_els00)
                                {
                                    $$->ffoCodeType_ls00 = SI->siCodeType_ls00;     // this might be ctUnknown_els00 anyway
                                                                                    // if the user did not run SET CODETYPE cmd
                                }

                                if ($$->ffoDateType_ls00 == dtUnknown_els00)
                                {
                                    tls00_DateTimeSpec  *pDateSpec;
                                    ls14_TemplateNewStruct(pDateSpec);      
                                    if (NULL == pDateSpec)
                                        YYERROR;

                                    // Parsing the date format for default value 'INTERNAL'
                                    pDateSpec->dtsType_ls00 = SI->siDateType_ls00;
                                    ls14_SpecialDateTimeParse(pDateSpec, 0);


                                    // Only date type and special mask have to be filled.
                                    $$->ffoDateType_ls00 = SI->siDateType_ls00;

                                    $$->ffoSpecialDateMask_ls00.dm_type = pDateSpec->dtsSpecialDateMask_ls00.dm_type;
                                    $$->ffoSpecialDateMask_ls00.mlen    = pDateSpec->dtsSpecialDateMask_ls00.mlen;
                                    strncpy($$->ffoSpecialDateMask_ls00.msk,
                                            pDateSpec->dtsSpecialDateMask_ls00.msk,
                                            pDateSpec->dtsSpecialDateMask_ls00.mlen);

                                    delete pDateSpec;

                                }
                                if ($$->ffoTimeType_ls00 == dtUnknown_els00)
                                {
                                    tls00_DateTimeSpec  *pTimeSpec;
                                    ls14_TemplateNewStruct(pTimeSpec);      
                                    if (NULL == pTimeSpec)
                                        YYERROR;

                                    // Parsing the time format for default value 'INTERNAL'
                                    pTimeSpec->dtsType_ls00 = SI->siTimeType_ls00;
                                    ls14_SpecialDateTimeParse(pTimeSpec, 1);


                                    // Only date type and special mask have to be filled.
                                    $$->ffoTimeType_ls00 = SI->siTimeType_ls00;

                                    $$->ffoSpecialTimeMask_ls00.dm_type = pTimeSpec->dtsSpecialDateMask_ls00.dm_type;
                                    $$->ffoSpecialTimeMask_ls00.mlen    = pTimeSpec->dtsSpecialDateMask_ls00.mlen;
                                    strncpy($$->ffoSpecialTimeMask_ls00.msk,
                                            pTimeSpec->dtsSpecialDateMask_ls00.msk,
                                            pTimeSpec->dtsSpecialDateMask_ls00.mlen);

                                    delete pTimeSpec;

                                }
                                if ($$->ffoTimeStampType_ls00== dtUnknown_els00)
                                {
                                    tls00_DateTimeSpec  *pTimeStampSpec;
                                    ls14_TemplateNewStruct(pTimeStampSpec);     
                                    if (NULL == pTimeStampSpec)
                                        YYERROR;

                                    // Parsing the time stamp format for default value 'INTERNAL'
                                    pTimeStampSpec->dtsType_ls00 = SI->siStampType_ls00;
                                    ls14_SpecialDateTimeParse(pTimeStampSpec, 2);


                                    // Only date type and special mask have to be filled.
                                    $$->ffoTimeStampType_ls00= SI->siStampType_ls00;

                                    $$->ffoSpecialTimeStampMask_ls00.dm_type    = pTimeStampSpec->dtsSpecialDateMask_ls00.dm_type;
                                    $$->ffoSpecialTimeStampMask_ls00.mlen       = pTimeStampSpec->dtsSpecialDateMask_ls00.mlen;
                                    strncpy($$->ffoSpecialTimeStampMask_ls00.msk,
                                            pTimeStampSpec->dtsSpecialDateMask_ls00.msk,
                                            pTimeStampSpec->dtsSpecialDateMask_ls00.mlen);

                                    delete pTimeStampSpec;

                                }
                                if ($$->ffoSeperat_ls00[0] == DEF_BYTE_LS00)
                                {
                                    strcpy($$->ffoSeperat_ls00, SI->siSeperat_ls00);
                                }

                                // Delimiter specification

                                if ($$->ffoDelimit_ls00[0] == DEF_BYTE_LS00)
                                {
                                    $$->ffoDelimit_ls00[0] = SI->siDelimit_ls00[0];
                                }
                                else if ('\x01' == $$->ffoDelimit_ls00[0])
                                {
                                    // Delimiter specified by user but is empty
                                    $$->ffoDelimit_ls00[0] = 0;
                                }

                                // ffoCompress_ls00 is true per default
                                if ( (false == $$->ffoFormatted_ls00) &&
                                     (false == $$->ffoCompress_ls00)  &&
                                     (false == $$->ffoCompact_ls00)   &&
                                     (false == $$->ffoPage_ls00)       )
                                {
                                    // User has not specified any format
                                    bFileFormatNotSpecified_ls14 = true;

                                    $$->ffoCompress_ls00 = true;
                                }

                                if (DEF_BYTE_LS00 == $$->ffoThsdChar_ls00)
                                {
                                    $$->ffoThsdChar_ls00 = SI->siThsdChar_ls00;
                                }
                                if (DEF_BYTE_LS00 == $$->ffoDecPoint_ls00)
                                {
                                    $$->ffoDecPoint_ls00 = SI->siDecPoint_ls00;
                                }

                                /* Boolwerte */
                                if (0 == $$->ffoTrueVal_ls00[0])
                                {
                                    $$->ffoTValLen_ls00 = STATIC_CAST(tsp00_Int2, _tcslen(SI->siTrueVal_ls00) );
                                    SAPDB_memcpy($$->ffoTrueVal_ls00, SI->siTrueVal_ls00, $$->ffoTValLen_ls00);

                                    $$->ffoTrueVal_ls00[$$->ffoTValLen_ls00] = 0;
                                }
                                if (0 == $$->ffoFalseVal_ls00[0])
                                {
                                    $$->ffoFValLen_ls00 = STATIC_CAST(tsp00_Int2, _tcslen(SI->siFalseVal_ls00) );
                                    SAPDB_memcpy($$->ffoFalseVal_ls00, SI->siFalseVal_ls00, $$->ffoFValLen_ls00);

                                    $$->ffoFalseVal_ls00[$$->ffoFValLen_ls00] = 0;
                                }

                                /* NULL-indicator */
                                if (0 == $$->ffoNullSpec_ls00[0])
                                {
                                    $$->ffoNSpecLen_ls00 = STATIC_CAST(tsp00_Int2, SI->siNullSpec_ls00.length());
                                    SAPDB_memcpy($$->ffoNullSpec_ls00, SI->siNullSpec_ls00.asCharp(), $$->ffoNSpecLen_ls00);

                                    $$->ffoNullSpec_ls00[$$->ffoNSpecLen_ls00] = 0;
                                }
                                /* Swap behavior of integer values defaults to the swap behavior of
                                   the machine the server is running on. */
                                if (sw_dummy == $$->ffoIntSpec_ls00)
                                {
                                    $$->ffoIntSpec_ls00.becomes(lcpDBInfo_ls14->dbiOwnSwap_ls00);
                                }
                            }
                        ;

file_format_spec        : /* empty */
                            {
                                tls00_FileFormat *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                // Creating the new struct sets all non pointer members to 0. But
                                // the initial value for the time and date types should evaluate to
                                // dt_unknown = -1. So we have to set it here explicitely.
                                RC->ffoDateType_ls00 = dtUnknown_els00;
                                RC->ffoTimeType_ls00 = dtUnknown_els00;
                                RC->ffoTimeStampType_ls00 = dtUnknown_els00;

                                $$ = RC;
                            }
                            
                        | code_spec file_format_spec
                            {
                                $$ = $2;
                                $$->ffoCodeType_ls00 = $1->cdsCodeType_ls00;
                                $$->ffoCSetLen_ls00  = 0;
                                delete $1;
                            }
                        | number_spec file_format_spec
                            {
                                $$ = $2;
                                $$->ffoThsdChar_ls00 = $1[0];
                                $$->ffoDecPoint_ls00 = $1[1];
                            }
                        | date_spec file_format_spec
                            {
                                $$ = $2;
                                $$->ffoDateType_ls00 = $1->dtsType_ls00;
                                if (dtFreeMask_els00 == $$->ffoDateType_ls00)
                                {
                                    // only if mask is not the standard mask dtsMask_ls00 contains
                                    // usable data and is allocated and no NULL pointer
                                    size_t len = _tcslen($1->dtsMask_ls00);

                                    SAPDB_memcpy($$->ffoDateMask_ls00, $1->dtsMask_ls00, len);
                                    $$->ffoDateMask_ls00[len] = 0;

                                    delete [] $1->dtsMask_ls00;
                                }
                                $$->ffoSpecialDateMask_ls00 = $1->dtsSpecialDateMask_ls00
                            }
                        | time_spec file_format_spec
                            {
                                $$ = $2;
                                $$->ffoTimeType_ls00 = $1->dtsType_ls00;
                                if (dtFreeMask_els00 == $$->ffoTimeType_ls00)
                                {
                                    // only if mask is not the standard mask dtsMask_ls00 contains
                                    // usable data and is allocated and no NULL pointer
                                    size_t len = _tcslen($1->dtsMask_ls00);

                                    SAPDB_memcpy($$->ffoTimeMask_ls00, $1->dtsMask_ls00, len);
                                    $$->ffoTimeMask_ls00[len] = 0;

                                    delete [] $1->dtsMask_ls00;
                                }
                                $$->ffoSpecialTimeMask_ls00 = $1->dtsSpecialDateMask_ls00
                            }
                        | timestamp_spec file_format_spec
                            {
                                $$ = $2;
                                $$->ffoTimeStampType_ls00 = $1->dtsType_ls00;
                                if (dtFreeMask_els00 == $$->ffoTimeStampType_ls00)
                                {
                                    // only if mask is not the standard mask dtsMask_ls00 contains
                                    // usable data and is allocated and no NULL pointer
                                    size_t len = _tcslen($1->dtsMask_ls00);

                                    SAPDB_memcpy($$->ffoTimeStampMask_ls00, $1->dtsMask_ls00, len);
                                    $$->ffoTimeStampMask_ls00[len] = 0;

                                    delete [] $1->dtsMask_ls00;
                                }
                                $$->ffoSpecialTimeStampMask_ls00 = $1->dtsSpecialDateMask_ls00
                            }
                        | null_spec file_format_spec
                            {
                                $$ = $2;

                                //*
                                //* We must handle here the case that the user specified the empty
                                //* value to represent the NULL;
                                //* in this case any count of blanks will be handled as empty value;
                                //* therefore the first thing to do is to trim the delivered value
                                //*
                                tsp00_Longint length = $1.strLeng_ls00;
                                ls05StrTrimRight($1.strAddr_ls00, length, BLANK_LS00, ctAscii_els00);

                                if (0 == length)
                                {
                                    // Empty value as NULL is represented as single blank
                                    $$->ffoNSpecLen_ls00    = 1;
                                    $$->ffoNullSpec_ls00[0] = BLANK_LS00;
                                    // string value is on creation time filled with binary zeroes
                                }
                                else
                                {
                                    $$->ffoNSpecLen_ls00 = $1.strLeng_ls00;
                                    SAPDB_memcpy($$->ffoNullSpec_ls00, $1.strAddr_ls00, $$->ffoNSpecLen_ls00);
                                    // string value is on creation time filled with binary zeroes
                                }

                                delete [] $1.strAddr_ls00;
                            }
                        | bool_spec file_format_spec
                            {
                                $$ = $2;

                                //*
                                //* Separate specification for true and false values.
                                //*
                                $$->ffoTValLen_ls00 = $1->bspTValLen_ls00;
                                SAPDB_memcpy($$->ffoTrueVal_ls00, $1->bspTrueVal_ls00, $1->bspTValLen_ls00);
                                $$->ffoTrueVal_ls00[$1->bspTValLen_ls00] = 0;


                                $$->ffoFValLen_ls00 = $1->bspFValLen_ls00;
                                SAPDB_memcpy($$->ffoFalseVal_ls00, $1->bspFalseVal_ls00, $$->ffoFValLen_ls00);
                                $$->ffoFalseVal_ls00[$$->ffoFValLen_ls00] = 0;

                                delete $1;
                            }
                        | int_spec file_format_spec
                            {
                                $$ = $2;
                                $$->ffoIntSpec_ls00 = $1;
                            }
                        | separator_spec file_format_spec
                            {
                                $$ = $2;
                                if (true == $$->ffoFormatted_ls00)
                                {
                                    ls14_ShowError(errSepDelFormatted_els98);
                                    YYERROR;
                                }

                                $$->ffoSeperat_ls00[0] = $1;
                                $$->ffoSeperat_ls00[1] = '\0';
                                $$->ffoCompress_ls00   = true;
                            }
                        | delimiter_spec file_format_spec
                            {
                                $$ = $2;
                                if (true == $$->ffoFormatted_ls00)
                                {
                                    ls14_ShowError(errSepDelFormatted_els98);
                                    YYERROR;
                                }
                                $$->ffoDelimit_ls00[0] = $1;
                                $$->ffoDelimit_ls00[1] = 0;
                                $$->ffoCompress_ls00   = true;
                            }
                        | kwFORMATTED file_format_spec
                            {
                                $$ = $2;
                                if ( (true == $$->ffoCompress_ls00) ||
                                     (true == $$->ffoCompact_ls00 ) ||
                                     (true == $$->ffoPage_ls00    )    )
                                {
                                    ls14_ShowError(errFormattedCompress_els98);
                                    YYERROR;
                                }
                                else
                                {
                                    if ( ($$->ffoSeperat_ls00[0] != 0) ||
                                         ($$->ffoDelimit_ls00[0] != 0) )
                                    {
                                        ls14_ShowError(errSepDelFormatted_els98);
                                        YYERROR;
                                    }
                                }
                                $$->ffoFormatted_ls00 = true;
                            }
                        | kwCOMPRESSED file_format_spec
                            {
                                $$ = $2;
                                if ( (true == $$->ffoFormatted_ls00) ||
                                     (true == $$->ffoCompact_ls00)   ||
                                     (true == $$->ffoPage_ls00    )     )
                                {
                                    ls14_ShowError(errFormattedCompress_els98);
                                    YYERROR;
                                }
                                $$->ffoCompress_ls00 = true;
                            }
                        | kwRECORDS file_format_spec
                            {
                                $$ = $2;
                                if ( (true == $$->ffoFormatted_ls00) ||
                                     (true == $$->ffoCompress_ls00)  ||
                                     (true == $$->ffoPage_ls00    )     )
                                {
                                    ls14_ShowError(errFormattedCompress_els98);
                                    YYERROR;
                                }
                                $$->ffoCompact_ls00 = true;
                            }
                        | kwPAGES file_format_spec
                            {
                                $$ = $2;
                                if ( (true == $$->ffoFormatted_ls00) ||
                                     (true == $$->ffoCompress_ls00 ) ||
                                     (true == $$->ffoCompact_ls00  )    )
                                {
                                    ls14_ShowError(errFormattedCompress_els98);
                                    YYERROR;
                                }
                                $$->ffoPage_ls00 = true;
                            }
                        | kwBINARY file_format_spec
                            {
                                $$ = $2;
                                $$->ffoBinary_ls00 = true;
                            }
                        | kwDDL file_format_spec
                            {
                                $$ = $2;
                                $$->ffoDdl_ls00 = true;
                                $$->ffoXml_ls00 = false;
                                $$->ffoXmlNoTable_ls00 = false;
                            }
                        | kwDDLNOINDEX file_format_spec
                            {
                                $$ = $2;
                                $$->ffoDdlNoIndex_ls00 = true;
                                $$->ffoXml_ls00 = false;
                                $$->ffoXmlNoTable_ls00 = false;
                            }   
                        | kwDTL file_format_spec usage_spec
                            {
                                $$ = $2;
                                $$->ffoDtl_ls00   = true;
                                SAPDB_memcpy(&$$->ffoUsage_ls00,$3,sizeof(tls00_Usage));
                            }                               
                        | kwXML file_format_spec
                            {
                                $$ = $2;
                                $$->ffoXml_ls00 = true;
                                $$->ffoXmlNoTable_ls00 = false;
                                $$->ffoDdl_ls00 = false;
                            }
                        | kwXMLNOTABLE file_format_spec
                        {
                            $$ = $2;
                            $$->ffoXml_ls00 = true;
                            $$->ffoXmlNoTable_ls00 = true;
                            $$->ffoDdl_ls00 = false;
                        }
                        | count_spec file_format_spec
                            {
                                $$ = $2;
                                $$->ffoCntSpec_ls00 = $1;
                            }
                        ;

code_spec               : kwASCII
                            {
                                tls00_CodeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cdsCodeType_ls00 = ctAscii_els00;
                                $$ = RC;
                            }
                        | kwEBCDIC
                            {
                                tls00_CodeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cdsCodeType_ls00 = ctEbcdic_els00;
                                $$ = RC;
                            }
                        | kwUCS2
                            {
                                tls00_CodeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                // Define right UCS2
                                short swapTest = 1;
                                RC->cdsCodeType_ls00 = (*REINTERPRET_CAST(char*, &swapTest)) ?
                                                        ctUCS2Swapped_els00 : ctUCS2_els00;
                                $$ = RC;
                            }
                        | kwUTF8
                            {
                                tls00_CodeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->cdsCodeType_ls00 = ctUTF8_els00;
                                $$ = RC;
                            }
                        ;
number_spec             : kwDECIMAL valNUMBER_LAYOUT
                            {
                                if ( ($2.strLeng_ls00 < 5)              ||
                                     ($2.strLeng_ls00 > 7)              ||
                                     ($2.strAddr_ls00[1] != SLASH_LS00) ||
                                     ($2.strAddr_ls00[$2.strLeng_ls00 - 2] != SLASH_LS00) )
                                {
                                    MsgInstance_ls14->ls98MsgToString(layParser_els98,
                                                                      lcpCommand_ls14->cmdRpyDat_ls00,
                                                                      errWrongNumberLayout_els98);
                                    lcpCommand_ls14->cmdReqErr_ls00 = errWrongNumberLayout_els98;
                                    YYERROR;
                                }
                                else
                                {
                                    if ($2.strAddr_ls00[2] == SLASH_LS00)
                                        $$[0] = DEF_BYTE_LS00;
                                    else
                                        $$[0] = $2.strAddr_ls00[2];

                                    if ($2.strAddr_ls00[$2.strLeng_ls00 - 3] == SLASH_LS00)
                                        $$[1] = DEF_BYTE_LS00;
                                    else
                                        $$[1] = $2.strAddr_ls00[$2.strLeng_ls00 - 3];
                                }
                            }
                        ;

date_spec               : kwDATE standard_dt_mask
                            {
                                tsp00_Int4 rc = ls14_SpecialDateTimeParse($2, 0);
                                if (errOK_els00 != rc)
                                {
                                    delete $2;
                                    YYERROR;
                                }
                                else
                                {
                                    $$ = $2;
                                }
                            }
                        | kwDATE free_dt_mask
                            {
                                tsp00_Int4 rc = ls14_SpecialDateTimeParse($2, 0);
                                if (errOK_els00 != rc)
                                {
                                    delete $2;
                                    YYERROR;
                                }
                                else
                                {
                                    $$ = $2;
                                }
                            }
                        ;

time_spec               : kwTIME standard_dt_mask
                            {
                                tsp00_Int4 rc = ls14_SpecialDateTimeParse($2, 1);
                                if (errOK_els00 != rc)
                                {
                                    delete $2;
                                    YYERROR;
                                }
                                else
                                {
                                    $$ = $2;
                                }
                            }
                        | kwTIME free_dt_mask
                            {
                                tsp00_Int4 rc = ls14_SpecialDateTimeParse($2, 1);
                                if (errOK_els00 != rc)
                                {
                                    delete $2;
                                    YYERROR;
                                }
                                else
                                {
                                    $$ = $2;
                                }
                            }
                        ;

timestamp_spec          : kwTIMESTAMP standard_dt_mask
                            {
                                tsp00_Int4 rc = ls14_SpecialDateTimeParse($2, 2);
                                if (errOK_els00 != rc)
                                {
                                    delete $2;
                                    YYERROR;
                                }
                                else
                                {
                                    $$ = $2;
                                }
                            }
                        | kwTIMESTAMP free_dt_mask
                            {
                                tsp00_Int4 rc = ls14_SpecialDateTimeParse($2, 2);
                                if (errOK_els00 != rc)
                                {
                                    delete $2;
                                    YYERROR;
                                }
                                else
                                {
                                    $$ = $2;
                                }
                            }
                        ;

standard_dt_mask        : kwEUR
                            {
                                tls00_DateTimeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                // RC->dtsMask_ls00 und dtsMaskLen_ls00 werden nicht gesetzt, da
                                // sie fuer diesen fall nicht weiter ausgewertet werden.

                                RC->dtsType_ls00 = dtEUR_els00;

                                $$ = RC;
                            }
                        | kwUSA
                            {
                                tls00_DateTimeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                // RC->dtsMask_ls00 und dtsMaskLen_ls00 werden nicht gesetzt, da
                                // sie fuer diesen fall nicht weiter ausgewertet werden.

                                RC->dtsType_ls00 = dtUSA_els00;
                                $$ = RC;
                            }
                        | kwJIS
                            {
                                tls00_DateTimeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                // RC->dtsMask_ls00 und dtsMaskLen_ls00 werden nicht gesetzt, da
                                // sie fuer diesen fall nicht weiter ausgewertet werden.

                                RC->dtsType_ls00 = dtJIS_els00;
                                $$ = RC;
                            }
                        | kwISO
                            {
                                tls00_DateTimeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                // RC->dtsMask_ls00 und dtsMaskLen_ls00 werden nicht gesetzt, da
                                // sie fuer diesen fall nicht weiter ausgewertet werden.

                                RC->dtsType_ls00 = dtISO_els00;
                                $$ = RC;
                            }
                        | kwINTERNAL
                            {
                                tls00_DateTimeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                // RC->dtsMask_ls00 und dtsMaskLen_ls00 werden nicht gesetzt, da
                                // sie fuer diesen fall nicht weiter ausgewertet werden.
                                RC->dtsType_ls00 = dtInternal_els00;
                                $$ = RC;
                            }
                        ;

free_dt_mask            : valDATE_TIME_FORMAT_MASK
                            {
                                tls00_DateTimeSpec *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                // ATTENTION: mask string comes decorated with double quotes
                                int length = $1.strLeng_ls00 - 2;

                                RC->dtsMask_ls00 = new char[length + 1];        // terminating \0 included
                                SAPDB_memcpy(RC->dtsMask_ls00, &$1.strAddr_ls00[1], length);
                                RC->dtsMask_ls00[length] = '\0';

                                RC->dtsType_ls00 = dtFreeMask_els00;

                                $$ = RC;
                            }
                        ;

null_spec               : kwNULL valLITERAL
                            {
                                if ($2.strLeng_ls00 > MAX_DEFNULL_SPEC_LENGTH_LS00)
                                {
                                    ls14_ShowError(errWrongNullSpec_els98);
                                    delete $2.strAddr_ls00;
                                    YYERROR;
                                }
                                else
                                {
                                    $$ = $2;
                                }
                            }
                        ;

bool_spec               : kwBOOLEAN valBOOL_VALUES
                            {
                                tsp00_Addr SlashPos = strchr ($2.strAddr_ls00, SLASH_LS00);
                                if ((SlashPos == NULL) || (SlashPos - $2.strAddr_ls00 >= $2.strLeng_ls00 - 1))
                                {
                                    ls14_ShowError(errWrongBooleanSpec_els98);
                                    $$ = NULL;
                                    YYERROR;
                                }
                                else
                                {
                                    tls00_BoolSpec *RC;
                                    ls14_TemplateNewStruct(RC);     
                                    if (NULL == RC)
                                        YYERROR;

                                    RC->bspTrueVal_ls00 = &$2.strAddr_ls00[1];
                                    RC->bspTValLen_ls00  = STATIC_CAST(tsp00_Int4, (SlashPos - &$2.strAddr_ls00[1]) );

                                    RC->bspFalseVal_ls00 = &SlashPos[1];
                                    RC->bspFValLen_ls00  = STATIC_CAST(tsp00_Int4, (&$2.strAddr_ls00[$2.strLeng_ls00 - 1] - &SlashPos[1]) );
                                    if ( (RC->bspTValLen_ls00 > MAX_BOOLVALUE_LENGTH_LS00) ||
                                         (RC->bspFValLen_ls00 > MAX_BOOLVALUE_LENGTH_LS00) )
                                    {
                                        ls14_ShowError(errWrongBooleanSpec_els98);
                                        delete RC;
                                        YYERROR;
                                    }
                                    $$ = RC;
                                }
                            }
                        ;

int_spec                : kwINTEGER kwHILO { $$.becomes(sw_normal); }
                        | kwINTEGER kwLOHI { $$.becomes(sw_full_swapped); }
                        ;

separator_spec          : kwSEPARATOR valSEPARATOR
                            {
                                char szTmpStr[2];   // separator consists of only 1 char
                                tsp00_Int4 rc = ls14_GetSeparatorDelimiter(&$2.strAddr_ls00[1],
                                                                           $2.strLeng_ls00-2*sizeof(char),
                                                                           szTmpStr);
                                if (errOK_els00 == rc)
                                {
                                    if (DEF_BYTE_LS00 == szTmpStr[0])
                                    {
                                        rc = errWrongDelSepSpec_els98;  // empty separator not allowed
                                    }
                                    else
                                    {
                                        $$ = szTmpStr[0];
                                    }
                                }
                                if (errOK_els00 != rc)
                                {
                                    ls14_ShowError(errWrongDelSepSpec_els98);
                                    YYERROR;
                                }
                            }
                        ;

delimiter_spec          : kwDELIMITER valDELIMITER
                            {
                                char szTmpStr[2];   // separator consists of only 1 char
                                tsp00_Int4 rc = ls14_GetSeparatorDelimiter(&$2.strAddr_ls00[1],
                                                                           $2.strLeng_ls00-2*sizeof(char),
                                                                           szTmpStr);
                                if (errOK_els00 == rc)
                                {
                                    if (DEF_BYTE_LS00 == szTmpStr[0])
                                    {
                                        // We need to flag here that a delimiter was specified
                                        // and that this one is empty. If no delimiter was
                                        // specified at all the value becomes '0'.
                                        $$ = '\x01';        // a non printable value that cannot be specified, too
                                    }
                                    else
                                    {
                                        $$ = szTmpStr[0];
                                    }
                                }
                                if (errOK_els00 != rc)
                                {
                                    ls14_ShowError(errWrongDelSepSpec_els98);
                                    YYERROR;
                                }
                            }
                        ;

count_spec              : kwCOUNT valBLOCK_COUNT
                          {
                              $$ = $2;      // Scanner makes sure that values are not negative
                          }
                        ;

file_extract_opt        : /* empty */
                            {
                                tls00_FileExtract *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->feStart_ls00.lnuRemainder_ls00  = UNDEFINED_LS00;
                                RC->feEnd_ls00.lnuRemainder_ls00    = UNDEFINED_LS00;
                                $$ = RC;
                            }
                        | kwSTART valSTART valMAX_POS
                            {
                                tls00_FileExtract *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->feStart_ls00.lnuRemainder_ls00 = $2;
                                RC->feEnd_ls00.lnuRemainder_ls00   = UNDEFINED_LS00;
                                $$ = RC;
                            }
                        | kwSTART valSTART_POS valEND_POS
                            {
                                tls00_FileExtract *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                tsp00_Int4 rc = errOK_els00;
                                rc = ls14_SeparateIntValues($2.strAddr_ls00, ':',
                                                            REINTERPRET_CAST(tsp00_Int4*, &RC->feStart_ls00.lnu2GB_Blocks_ls00),
                                                            REINTERPRET_CAST(tsp00_Int4*, &RC->feStart_ls00.lnuRemainder_ls00));
                                if (errOK_els00 == rc)
                                {
                                    rc = ls14_SeparateIntValues($3.strAddr_ls00, ':',
                                                                REINTERPRET_CAST(tsp00_Int4*, &RC->feEnd_ls00.lnu2GB_Blocks_ls00),
                                                                REINTERPRET_CAST(tsp00_Int4*, &RC->feEnd_ls00.lnuRemainder_ls00));
                                }
                                delete [] $2.strAddr_ls00;
                                delete [] $3.strAddr_ls00;
                                
                                if (errOK_els00 != rc)
                                {
                                    // Function returns error text
                                    lcpCommand_ls14->cmdReqErr_ls00 = rc;
                                    YYERROR;
                                }
                                $$ = RC;
                         }
                         | kwSTART valSTART valEND
                            {
                                tls00_FileExtract *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->feStart_ls00.lnuRemainder_ls00 = $2;
                                RC->feEnd_ls00.lnuRemainder_ls00   = $3;
                                $$ = RC;
                            } 
                        | kwSTART valSTART valEND kwRECORDS
                            {
                                tls00_FileExtract *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;
                                RC->feStart_ls00.lnuRemainder_ls00 = $2;
                                RC->feEnd_ls00.lnuRemainder_ls00   = $3;
                                $$ = RC;
                            }
                         
                        | kwSTART valSTART sgnCOLON valSTART valEND sgnCOLON valEND
                            {
                                tls00_FileExtract *RC;
                                ls14_TemplateNewStruct(RC);     
                                if (NULL == RC)
                                    YYERROR;

                                RC->feStart_ls00.lnu2GB_Blocks_ls00 = $2;
                                RC->feStart_ls00.lnuRemainder_ls00  = $4;
                                RC->feEnd_ls00.lnu2GB_Blocks_ls00   = $5;
                                RC->feEnd_ls00.lnuRemainder_ls00    = $7;
                                $$ = RC;
                            }
                        ;

%%
/* End of grammar definition *************************************************************/
