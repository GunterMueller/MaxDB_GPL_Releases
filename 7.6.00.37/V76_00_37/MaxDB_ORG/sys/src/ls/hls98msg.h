/*! 
  -----------------------------------------------------------------------------
  module: hls98msg.h
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Server protocol class
                
  version:      7.6.
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

#ifndef HLS98MSG_H
#define HLS98MSG_H

#include <assert.h>
#include "gls00.h"      // bindet stdio.h,... mit ein


#if defined(_WIN32)
#define NEW_LINE "\r\n"
#else
#define NEW_LINE "\n"
#endif



/*********************** LAYER NUMMERN ******************************************/
typedef enum
{
    layParser_els98         = 0,
    layScanner_els98,
    laySQLConnect_els98,
    laySQLInterface_els98,
    layGRec_els98,
    layBasicConnect_els98,
    layOIHandling_els98,
    layUserConnect_els98,
    layDataLoad_els98,
    layVFile_els98,
    layFastload_els98,
    layTableExtract_els98,
    layDataExtract_els98,
    layDispatch_els98,
    layCondition_els98,
    layCursor_els98,        // modul ls12
    layTableLoad_els98,
    layDataUpdate_els98,
    layCatalogExtract_els98,
    layCatalogLoad_els98,
    layDBExtract_els98,
    layDBLoad_els98,

    layEndOfArray_els98
} LayerNumbers;


/********************** MESSAGE NUMMERN *****************************************/

// Fehlernummernraum ist -25999 bis -25000 und 25000 bis 25999
typedef enum
{
    // mapping of db errors that may be returned by functions called from repman
    dberrUnknownTableName_els00                 = -4004,
    dberrParseAgain_els98                       = -8,
    dberrRowNotFound_els98                      = 100,


    
    // 800er-fehler : bei auftreten sofort an die hotline melden

    // errors in parser or scanner (no message text for these errors; will be mentioned as
    // error id in message errInternal_els98)
    errParseInternal_els98                      = -25899,
    errParseNoScanner_els98,                    //= -25898
    errScanNoAction_els98,                      //= -25897
    errScanNoBuffer_els98,                      //= -25896
    errScanInputFailed_els98,                   //= -25895
    errScanBufOverflow_els98,                   //= -25894

    // internal errors at load execution
    errUnknownDefault_els98,                    //= -25893

    // internal error while converting string/data to unicode or vice versa
    errFromUCS2SourceExhausted_ls98             = -25874,
    errFromUCS2TargetExhausted_ls98,            //= -25873
    errToUCS2SourceExhausted_ls98,              //= -25872
    errToUCS2TargetExhausted_ls98,              //= -25871
    errEncodingSourceExhausted_els98,           //= -25870
    errEncodingTargetExhausted_els98,           //= -25869

    errInternalSQL_els98                        = -25809,
    errConvertPageSize_els98,                   //= -25808
    errUnknownPart_els98,                       //= -25807
    errWriteBuf2Part_els98,                     //= -25806
    errInitSegment_els98,                       //= -25805
    errInitPart_els98,                          //= -25804
    errFinishPart_els98,                        //= -25803
    errInitPacket_els98,                        //= -25802        // Dieser fehler tritt eigentlich nur in vls03.cpp auf
    errInternal_els98,                          //= -25801
    
    errSchemaMapIdentity                        = -25705,
    errSchemaMapDefinition,                     //= -25704,
    errSchemaMapExists_els98,                   //= -25703,
    errSchemaMapNotExists_els98,                //= -25702,
    errSchemaMapPrivilegesNotExists_els98,      //= -25701,
    errSchemaMapSchemasNotExists_els98,         //= -25700,

    errTLWrongKernelVersion_els98               = -25604,
    errTLWrongLoaderVersion_els98,              //= -25603
    errTLTableNotEmpty_els98,                   //= -25602
    errTLTableNotExisting_els98,                //= -25601
    
    errUnloadStatistics_els98                   = -25552,
    errLoadStatistics_els98,                    //= -25551
    
    // File handling errors
    errFileWrongEncoding_els98                  = -25508,
    errFileTruncate_els98,                      //= -25507,
                                                //= -25506
    errFileErase_els98                          = -25505,
    errFileWrite_els98,                         //= -25504
    errFileRead_els98,                          //= -25503
    errFileClose_els98,                         //= -25502
    errFileOpen_els98,                          //= -25501

    errMaxErrorCountReached_els98               = -25500,
    errDLDataNotCompletelyLoaded_els98,         // = -25499
    errDLNoDataLoaded_els98,                    // = -25498

    // Special FASTLOAD errors
    errFL_IntegrityViolation_els98              = -25451,


    //*
    //* Errors when using special ASCII code pages
    //*
    errInvalidASCIICPValues_els98               = -25405,
    errInvalidHexInCPFile_els98,                //= -25404
    errOpen_ReadCPFile_els98,                   //= -25403
    errCreate_GrantCPTable_els98,               //= -25402
    errReadCPTable_els98,                       //= -25401

    //*
    //*  ERRORS AT LOAD EXECUTION
    //*
    
    //*
    //*  Errors processing sql statements
    //*
    errSelectColumnNames_els98                  = -25395,
    errParseAgainFailed_els98,                  //= -25394
    errCheckTableExistence_els98,               //= -25393
    errODBCErrors_els98,                        //= -25392
    errSQLStmtTooLong_els98,                    //= -25391

    errDEAmbiguousTableName_els98,              //= -25390

    //* Errors at conversion between different encodings
    errConvertingEncoding_els98                 = -25371,

    // Errors reading values from data file
    errCheckHeader_els98                        = -25366,
    errCheckTrailer_els98,                       //= -25365
    errTableToLoadNotCurrentTable_els98,        //= -25364
    errCorruptMetaData_els98,                   //= -25363
                                                //= -25362
    errCorruptDataFile_els98                    = -25361,

    
    //* Errors processing LONG values
    errProcessLongsWithGetVal_els98             = -25357,
    errProcessLongsWithPutVal_els98,            //= -25356
    errProcessLongs_els98,                      //= -25355
    errLongfileSpecMissing_els98,               //= -25354
    errLongFileNumGTMax_els98,                  //= -25353
    errAnalyzeLongSum_els98,                    //= -25352
    errNoLongFileInfo_els98,                    //= -25351
    

    errIncompatibleEncodings_els98              = -25350,
    errTEExtractDataOfCertainTable_els98,       //= -25349
    errTLSrcAndTgtSwapDifferent_els98,          //= -25348,
    errTLSrcAndTgtEncodingDifferent_els98,      //= -25347
    errTLPacketSizeTooSmall_els98,              //= -25346
    errTLSchemeDifferences_els98,               //= -25345
    errWrongFieldLength_els98,                  //= -25344
    errNotInSelList_els98,                      //= -25343
    errTLTableNotRestored_els98,                //= -25342
    err_DE_GenerateCreateCmd_els98,             //= -25341
                                                //= -25340
    errTLRestartFileOpen_els98                  = -25339,
    errTLRestartEntryNotFound_els98,            //= -25338
    errTLTableDataNotFound_els98,               //= -25337
    errTLSingleTabFileMultTab_els98,            //= -25336
    errTLMultTabFileSingleTab_els98,            //= -25335
    errTEFileCounterExceeded_els98,             //= -25334
    errGetIndexDefinition_els98,                //= -25333
    errGetTableDefinition_els98,                //= -25332
    errRestrictedStmt_els98,                    //= -25331
    errWrongTEPageSize_els98,                   //= -25330
    errWrongTEFileType_els98,                   //= -25329
    errErrorAtDataExtract_els98,                //= -25328
    errAtExtractedRec_els98,                    //= -25327
    errNumberTruncAtExtract_els98,              //= -25326
    errCompatibilityCheckSum_els98,             //= -25325
    errLengthCheckSum_els98,                    //= -25324
    errIncompatibleNullValue_els98,             //= -25323
    errTruncatedConditionValue_els98,           //= -25322
    errInvalidNumCondition_els98,               //= -25321
                                                //= -25320
    errMissingData_els98                        = -25319,
    errSyskeyExceeded_els98,                    //= -25318
    errIncompatibleConst_els98,                 //= -25317
    errLongColIllegal_els98,                    //= -25316
    errIncompatibleDataTypes_els98,             //= -25315
    errFractionTooSmall_els98,                  //= -25314
    errNumberOverflow_els98,                    //= -25313
    // Dieser fehler sollte ohne fehlertext auskommen erstmal, den gibts nur, um anzuzeigen
    // dass bei der umwandlung einer benutzergelieferten zahl in eine interne number 
    // die number um die letzten stellen gekuerzt wurde.
    // Das ist somit eine warnung.
    errNumberTruncated_els98,                   //= -25312
    errNoDataForNNCol_els98,                    //= -25311
    errNullNotAllowed_els98,                    //= -25310
    errInvalidBool_els98,                       //= -25309
    errConstraintViolation_els98,               //= -25308
    errInvalidDbyte_els98,                      //= -25307
    errInvalidHex_els98,                        //= -25306
    errKeyLength_els98,                         //= -25305
    errNoDataForKeyCol_els98,                   //= -25304
    errKeyOrder_els98,                          //= -25303
    errInvalidNumber_els98,                     //= -25302
    errWrongDateTime_els98,                     //= -25301,

    //*
    //*     Parser error or scanner error
    //*
    errSQLSyntax_els98                          = -25281,


    //*
    //* Errors parsing commands/statements
    //*
    errColAndCompact_els98                        = -25252,
    errCodepageChangeOnlyBySysdba_els98,        //= -25251,
    errBuildingEncryptionURIFailed_els98,       //= -25250
    errWrongIsolationLevel_els98,               //= -25249
    errDatastreamFormatnotSupported,            //= -25248
    errCatalogOrDatastreamNeeded,               //= -25247
    errCatalogAndDatastreamNeeded,              //= -25246
    errPackagestreamNeeded,                     //= -25245
    errLoaderConstraintViolation_els98,         //= -25244
    errErrorParsingDateTimeMask_els98,          //= -25243
    errDBIdentifierTooLong_els98,               //= -25242
    errWrongFullQualTableNameSpec_els98,        //= -25241
    errNoPasswordSupplied_els98,                //= -25240
    errInvalidInteger_els98,                    //= -25239
    errACCColsMissing_els98,                    //= -25238
    errSETColsMissing_els98,                    //= -25237
    errColOrderFormatted,                       //= -25236
    errColOrderCompressed,                      //= -25235
    errEndPosAndFormatted_els98,                //= -25234
    errEndPosAndCompress_els98,                 //= -25233
    errWrongKeyword_els98,                      //= -25232
    errRTNotAllowed_els98,                      //= -25231
    errCtrlFileSpecMissing_els98,               //= -25230
    errCompareOperator_els98,                   //= -25229
    errWrongDateTimeFormat_els98,               //= -25228
    errMaxConditionCount_els98,                 //= -25227
    errMaxColCountExceeded_els98,               //= -25226
    errWrongNumberLayout_els98,                 //= -25225
    errWrongLangSpec_els98,                     //= -25224
    errWrongNullSpec_els98,                     //= -25223
    errWrongBooleanSpec_els98,                  //= -25222
    errWrongDecimalSpec_els98,                  //= -25221
    errWrongDelSepSpec_els98,                   //= -25220
    errPosNotAscending_els98,                   //= -25219
    errNoColAndFormatted_els98,                 //= -25218
    errEndPosLTStartPos_els98,                  //= -25217
    errFormattedRequired_els98,                 //= -25216
    errWrongCodePageSpec_els98,                 //= -25215
    errSepDelFormatted_els98,                   //= -25214
    errFormattedCompress_els98,                 //= -25213
    errInvalidTrunc_els98,                      //= -25212
    errInvalidRound_els98,                      //= -25211
    errInvalidScale_els98,                      //= -25210
    errInvalidFraction_els98,                   //= -25209
    errInvalidPosition_els98,                   //= -25208
    errUsageNotApplicable_els98,                //= -25207
    errWrongUsageRows_els98,                    //= -25206
    errWrongUsagePercent_els98,                 //= -25205
    errWrongZonedLength_els98,                  //= -25204
    errWrongDecimalLength_els98,                //= -25203
    errWrongRealLength_els98,                   //= -25202
    errWrongIntegerLength_els98,                //= -25201


    errNotImplemented_els98                     = -25109,
    //*
    //*     Interne Fehler des servers
    //*
	errCurrentSchemaCmd_els98                   = -25024,    
                                                //= -25023
    errCancelRequest_els98                      = -25022,
    errCurrentUserCmd_els98,                    //= -25021
    errNoMoreMemory_els98,                      //= -25020
    errLogFileOpen_els98,                       //= -25019
    errChangeLogFileOwner_ls98,                 //= -25018

    errNoUserConnection_els98                   = -25012,
    errInternSQL_els98,                         //= -25011
    errSimpleSQL_els98,                         //= -25010,
    errDbComm_els98,                            //= -25009,
    // vls10.cpp
    // eigentlich keine fehlermeldung, sondern die meldung
    //  dass die verbindung des vorhergehenden users wiederhergestellt wurde.
    // Insofern ist es also doch eine fehermeldung, als es bedeutet, dass der neue user nicht
    // angemeldet werden konnte
    errPreviousUserRestored_els98,               //= -25008
    
    //*
    //*     allgemeine, schwerwiegende, aber zu behebende fehler
    //*
    errConnectNoServerDB_els98,                 //= -25007
    errSqlConnectUserkey_els98,                 //= -25006
    errSQLConnectUser_els98,                    //= -25005
    errSqlConnectDb_els98,                      //= -25004
    errDbHost_els98,                            //= -25003
    errDBConnectNoDBName_els98,                 //= -25002
                                                //= -25001
    msgSimpleMsg_els98                          = 0,


    //* Statistics messages given to client upon return
    statInsertedRejectedRows_els98              = 25201,
    statUpdatedInvalidRows_els98,               //= 25202,
    statNumExtractedRecords_els98,              //= 25203,
    statNumExportedPages_els98,                 //= 25204,
    statNumImportedPages_els98,                 //= 25205,
    statNumTablesTransformed_Catalog_els98,     //= 25206,
    statNumTablesTransformed_Data_els98,        //= 25207,
    statNULLValuesInserted_els98                = 25214,


    //*
    //*     WARNUNGEN_WARNUNGEN_WARNUNGEN_WARNUNGEN_WARNUNGEN_WARNUNGEN_WARNUNGEN_WARNUNGEN
    //*
    wrnUsageIgnoredRows_els98                   = 25599,
    wrnConfigurationStreamNotSupported_els98,   //= 25600,
    errNILValueTruncated_els98,                 //= 25601
    errIntValueOverflow_els98,                  //= 25602
    errLiteralTruncated_els98,                  //= 25603
    errSurplusLongInfo_els98,                   //= 25604
    errNoDatafileHeader_ls98,                   //= 25605
    wrnTL_LongsButDifferentSwap_ls98,           //= 25606
    wrnUsageIgnoredPercent_els98,               //= 25607
    wrnNoRestartFileFound_els98,                //= 25608
    wrnLoaderConstraintViolation_els98,         //= 25609
    wrnDEforFLCreatesDLStmt_els98,              //= 25610

    //*
    //*     status messages
    //* these messages are logged WITHOUT the message number
    //*
    msgRemark_els98                             = 25900,
    msgUserConnectionLog_els98,                 //= 25901
    msgReleaseUserComm_els98,                   //= 25902
    msgReleaseUserRollback_els98,               //= 25903
    msgStartCommand_els98,                      //= 25904
    msgStopCommand_els98,                       //= 25905
    msgCommandExecuting_els98,                  //= 25906
    msgInsertedRejectedRows_els98,              //= 25907
    msgUpdatedInvalidRows_els98,                //= 25908
    msgLastCommInputLine_els98,                 //= 25909
    msgLastInsInputLine_els98,                  //= 25910
    msgNumExtractedRecords_els98,               //= 25911
    msgNumExtractedRecordsNoTabName_els98,      //= 25912
    msgTableNotImport_els98,                    //= 25913
    msgNoCtrlCmdAtDataextract_els98,            //= 25914
    msgStartServer_els98,                       //= 25915
    msgVersionString_els98,                     //= 25916
    msgTL_InputFile_els98,                      //= 25917
    msgInstanceInformation_els98,               //= 25918
    msgLastCommInputLine2_els98,                //= 25919     this message is necessary for different handling of paramters
    msgInsertedRejectedRows2_els98,             //= 25920     this message is necessary for different handling of paramters
    msgSequenceRangeExhausted_els98,            //= 25921 
    msgExecuteSystemCommand_els98,              //= 25922 
    msgModuleFileName_els98 ,                   //= 25923
    msgEnvironmentPath_els98,                   //= 25924
    msgReconnected_els98,                       //= 25925
    errAtRowWithTabname_els98                   = 25940,
    errAtRow_els98,                             //= 25941
    errAtPos_els98,                             //= 25942

    // Messages for 
    // CATALOGEXTRACT/CATALOAGLOAD 
    // TABLEEXTRACT/TABLELOAD
    // DATAEXTRACT/DATALOAD
    msgNumTabToLoad_els98                       = 25943,
    msgTL0Successful_els98,                     //= 25944
    msgDESuccessful_els98,                      //= 25945
    msgNumViewsTransformed_els98,               //= 25946
    msgNumSynonymsTransformed_els98,            //= 25947
    msgNumTabTransformed_els98,                 //= 25948
    msgNumViewToTransform_els98,                //= 25949
    msgNumSynonymToTransform_els98,             //= 25950
    msgNumTabToTransform_els98,                 //= 25951
    msgExtractedFailedTables_els98,             //= 25952
    msgTESuccessful_els98,                      //= 25953
    msgLoadedRejectedTables_els98,              //= 25954
    msgTLSuccessful_els98,                      //= 25955
    msgConfigPackage_els98,                     //= 25956
    msgNumTabExIncludedTransport_els98,         //= 25957
    msgNumTabExcludedExport_els98,              //= 25958
    msgNumTabExcludedImport_els98,              //= 25959
    msgImportPackage_els98,                     //= 25960
    msgExportPackage_els98,                     //= 25961
    msgExecSchemaPackage_els98,                 //= 25962
    msgExecDataPackage_els98,                   //= 25963
    msgDLSuccessful_els98,                      //= 25964
    msgEmptyDataFile_els98,                     //= 25965
    msgErrNo_LT_MaxAdmissibleErrNo_els98,       //= 25966

    errAtRow2_els98                             = 25966,   // TODOTODO - Int8 instead of Int4 and the same message for all
    errAtPos2_els98,                            //= 25967
    msgNULLValuesInserted_els98,                //= 25968

// do not use
    errEndOfArray_els98                         = 26000
} ErrorNumbers;




const char * const pszCommentLine       = "// *"NEW_LINE;
const char * const pszErrorLineHeader   = "// E ";
const char * const pszRemarkLineHeader  = "// R\t"; //!!LogMessage
const char * const pszInfoLineHeader    = "// M\t";
const char * const pszWarningLineHeader = "// W ";
const char * const pszStatisticsMsgDelimiter    = "---\n";

#define ERROR_NUM_BEGIN -26000
#define ERROR_NUM_END   -25000
#define WARNING_NUM_BEGIN 25600
#define WARNING_NUM_END   25700
#define INFO_NUM_BEGIN 25900
#define INFO_NUM_END   26000

#define MAX_OI_SIZE_LS98 128*1024                // size corresponds to max size of message buffer of DB

/***************** KLASSE LoaderMessages **************************************/


class LoaderMessages
{
// Construction/Deconstruction
protected:
    LoaderMessages(); // Constructor

public:
    ~LoaderMessages();

// Attributes
private:
    static LoaderMessages  *_msgsinstance;

    _TCHAR                   m_LoaderDataPath_c[PATH_MXSP00];
    _TCHAR                   m_LoaderApplPath_c[PATH_MXSP00];
    _TCHAR                   m_ConfigPath_c[PATH_MXSP00];
    _TCHAR                   m_LoaderLogPath_c[PATH_MXSP00];
    _TCHAR                   m_TransportLogPath_c[PATH_MXSP00];
    _TCHAR                   m_TransportPath_c[PATH_MXSP00];
    static _TCHAR           *m_LogFileName_c;
    static tsp00_Int4        m_LogFile;          // File handle
    
    tsp00_ErrTextc           m_ErrorText;        // Interner fehlertext, wenn beim dateibearbeiten etwas schiefgeht
        
    // Every function manipulating one of the following vars must reinitialize them
    char                     m_szMessageForFile[MAX_OI_SIZE_LS98];
    char                     m_szMessageForStr[MAX_REPLY_LENGTH_LS00];
    
    char                    *m_pszMsg;
    char                    *pszLayer_ls98;      // Der layerstring ist immer 4 bytes lang + NULL byte !


public:
    static LoaderMessages  *Instance();   
    int                     ls98InitLogFile         (const char* pszDBName);
    void                    ls98InitLoaderPath      ();
    void                    ls98GetLoaderPath       (char *pszLoaderPath);
    void                    ls98GetLoaderLogPath    (char *pszLoaderPath);
    void                    ls98GetLoaderConfigPath (char *pszLoaderPath);
    static void             ls98SetLogFileName      (tsp00_VFilenamec pszFileName);
    void                    ls98GetLogFileName      (char *pszLogFileName);
    void                    ls98GetErrorText        (char *pszErrorText);
    int                     ls98ChangeLogFileOwner  (tsp00_Addr pszUsername, tsp00_C256c & szErrorText);
        
    /*!
    -----------------------------------------------------------------------------
 
      function:     ls98PrintMessage

    -----------------------------------------------------------------------------

      description:  Gibt einen fehlerstring passend zur gemeldeten fehlernummer aus.
                    Die layerinformation wird momentan nicht beachtet.

      arguments:    lLayer         [IN]    - enum parameter des typs LayerNumbers,
                                             der den layer, in dem eine meldung angelegt
                                             wurde, identifiziert.
                                             WIRD Z.Z. NICHT AUSGEWERTET.
                    lMessageNumber [IN]    - enum parameter des typs ErrorNumbers
                                             fehler- bzw. meldungsnummer
                    weitere param  [IN]    - variabel viele weitere parameter fuer die
                                             meldung

      returnvalue:  No
    -----------------------------------------------------------------------------
    */
    void ls98PrintMessage(const LayerNumbers lLayer, const ErrorNumbers lMessageNumber, ...);

    
    /*!
    -----------------------------------------------------------------------------
 
      function:     ls98MsgToString

    -----------------------------------------------------------------------------

      description:  Gibt einen fehlerstring passend zur gemeldeten fehlernummer NUR
                    in den angegebenen String aus.
                    Die layerinformation wird momentan nicht beachtet.

      arguments:    lLayer         [IN]    - enum parameter des typs LayerNumbers,
                                             der den layer, in dem eine meldung angelegt
                                             wurde, identifiziert.
                                             WIRD Z.Z. NICHT AUSGEWERTET.
                    pszDestString  [IN]    - zeiger auf den string, in den der msgtext
                                             kopiert werden soll.
                    lMessageNumber [IN]    - enum parameter des typs ErrorNumbers
                                             fehler- bzw. meldungsnummer
                    weitere param  [IN]    - variabel viele weitere parameter fuer die
                                             meldung

      returnvalue:  No
    -----------------------------------------------------------------------------
    */
    void ls98MsgToString(const LayerNumbers lLayer,
                         char               *pszDestString,
                         const ErrorNumbers lMessageNumber,
                         ...);

    /*!
    -----------------------------------------------------------------------------
      function:     ls98AppendStatusToString
    -----------------------------------------------------------------------------
      description:  Appends an error/status message to the return string.

      arguments:    lLayer         [IN]    - enum parameter des typs LayerNumbers,
                                             der den layer, in dem eine meldung angelegt
                                             wurde, identifiziert.
                                             WIRD Z.Z. NICHT AUSGEWERTET.
                    pszDestString  [IN]    - zeiger auf den string, in den der msgtext
                                             kopiert werden soll.
                    lMessageNumber [IN]    - enum parameter des typs ErrorNumbers
                                             fehler- bzw. meldungsnummer
                    more param     [IN]    - variabel viele weitere parameter fuer die
                                             meldung

      returnvalue:  No
    -----------------------------------------------------------------------------
    */
    void ls98AppendStatusToString(const LayerNumbers  lLayer,
                                  char*               pszDestString,
                                  const ErrorNumbers  lMessageNumber,
                                  ...);


    /*!
    -----------------------------------------------------------------------------
 
      function:     ls98Msg

    -----------------------------------------------------------------------------

      description:  Gibt einen fehlerstring passend zur gemeldeten fehlernummer
                    ins logfile aus und in den angegebenen String.
                    Es wird keine layerinformation mitgegeben.
                    Der ausgabestring darf 1024 zeichen nicht ueberschreiten
                    (abschliessende NULL eingeschlossen) !!!

      arguments:    lLayer         [IN]    - enum parameter des typs LayerNumbers,
                                             der den layer, in dem eine meldung angelegt
                                             wurde, identifiziert.
                                             WIRD Z.Z. NICHT AUSGEWERTET.
                    pszDestString  [IN]    - zeiger auf den string, in den der msgtext
                                             kopiert werden soll.
                    lMessageNumber [IN]    - enum parameter des typs ErrorNumbers
                                             fehler- bzw. meldungsnummer
                    weitere param  [IN]    - variabel viele weitere parameter fuer die
                                             meldung

      returnvalue:  No
    -----------------------------------------------------------------------------
    */
    void ls98Msg(const LayerNumbers lLayer,
                 char               *pszDestString,
                 const ErrorNumbers lMessageNumber,
                 ...);


    /*!
    -----------------------------------------------------------------------------
 
      function:     ls98EFromRcToFile

    -----------------------------------------------------------------------------

      description:  Verwandelt den Fehlertext in einem rueckgabestring in einen
                    Fehlertext fuer die protokolldatei.
                    Notwendig, da manchmal nur die rueckgabestrings (insbesondere
                    im parser) zurueckgegeben werden und der fehler noch ins logfile
                    muss. Text in Rueckgabestring und in protokolldatei unterscheiden
                    sich aber in der formatierung.

      attention:    Nur fuer Fehler!

      arguments:    lLayer       [IN]    - enum parameter des typs LayerNumbers,
                                            der den layer, in dem eine meldung angelegt
                                            wurde, identifiziert.
                                            WIRD Z.Z. NICHT AUSGEWERTET.
                    pszSrcString [IN]    - zeiger; msgtext fuer rueckgabestring (enthaelt
                                            den zugehoeringen text in bereits formatierter form)
                    lMsgNr       [IN]    - enum parameter des typs ErrorNumbers
                                            fehler- bzw. meldungsnummer

      returnvalue:  No
    -----------------------------------------------------------------------------
    */
    void ls98EFromRcToFile(const  LayerNumbers lLayer,
                           char  *pszSrcStr,
                           const  ErrorNumbers lMsgNr);

private:
    void ls98_GetMessageText(const ErrorNumbers MessageNumber);
    void ls98_GetLayerText(const LayerNumbers Layer);
    int ls98_Write();
    void ls98_Init();
    void ls98_FormatMsg(const ErrorNumbers lMsgNr, bool  bMsgToFileOnly, va_list arglist);
    void ls98_AppendMsgToString(const ErrorNumbers lMsgNr, bool bFirstMsgToAppend, va_list arglist);
    void ls98_MakeLogFileUTF8();
};




//*
//* class Loader_RTStatistics
//*
class Loader_RTStatistics
{
// Construction/Deconstruction
protected:
    Loader_RTStatistics();

public:
    //~Loader_RTStatistics();

// Attributes
public:
    typedef enum
    {
        LONGLONGDATA = 0,
        SHORTLONGDATA,
        PRIMARYDATA,
        LONGDATA,               // sum of LONGLONGDATA and SHORTLONGDATA
        ALLDATA                 // sum of PRIMARYDATA and LONGLONGDATA and SHORTLONGDATA
    } TypeOfPages;

private:
    static Loader_RTStatistics*     pStatisticsInstance;

    SAPDB_Int4                      GatheringInterval;
    
    // Statistics for transforming DB, USER or SCHEMA - catalog objects: tables
    SAPDB_Int4                      CntCatalogObjTransformed;
    SAPDB_Int4                      CntCatalogObjExcluded;
    SAPDB_Int4                      CntCatalogObjFailed;

    // Statistics for transforming DB, USER or SCHEMA - data
    SAPDB_Int4                      CntTablesTransformed;                      // counts successfully exported/imported (data) tables
    SAPDB_Int4                      CntTablesExcluded;
    SAPDB_Int4                      CntTablesFailed;
    
    // Single table statistics
    SAPDB_Int8                      CntPrimaryData;
    SAPDB_Int8                      CntLongData;
    SAPDB_Int8                      CntShortLongData;
    SAPDB_Int8                      CntLongLongData;

    // Auxiliary counter
    SAPDB_Int8                      Counter;


public:
    static Loader_RTStatistics*     Instance();
    void                            Init();

    inline void                     IncreaseCounter_TablesTranformed()          { ++CntTablesTransformed; }
    inline void                     SetTablesTranformed(SAPDB_Int4 Count)       { CntTablesTransformed = Count; }
    inline SAPDB_Int4               GetTablesTranformed()                       { return CntTablesTransformed; }


    inline void                     IncreaseCounter_TablesExcluded()            { ++CntTablesExcluded; }
    inline void                     SetTablesExcluded(SAPDB_Int4 Count)         { CntTablesExcluded = Count; }
    inline SAPDB_Int4               GetTablesExcluded()                         { return CntTablesExcluded; }


    inline void                     IncreaseCounter_TablesFailed()              { ++CntTablesFailed; }
    inline void                     SetTablesFailed(SAPDB_Int4 Count)           { CntTablesFailed = Count; }
    inline SAPDB_Int4               GetTablesFailed()                           { return CntTablesFailed; }

    inline void                     IncreaseCounter_CatalogObjTransformed()     { ++CntCatalogObjTransformed; }
    inline void                     SetCatalogObjTransformed(SAPDB_Int4 Count)  { CntCatalogObjTransformed = Count; }
    inline SAPDB_Int4               GetCatalogObjTransformed()                  { return CntCatalogObjTransformed; }

    inline void                     IncreaseCounter_CatalogObjExcluded()        { ++CntCatalogObjExcluded; }
    inline void                     SetCatalogObjExluded(SAPDB_Int4 Count)      { CntCatalogObjExcluded = Count; }
    inline SAPDB_Int4               GetCatalogObjExluded()                      { return CntCatalogObjExcluded; }

    inline void                     IncreaseCounter_CatalogObjFailed()          { ++CntCatalogObjFailed; }
    inline void                     SetCatalogObjFailed(SAPDB_Int4 Count)       { CntCatalogObjFailed = Count; }
    inline SAPDB_Int4               GetCatalogObjFailed()                       { return CntCatalogObjFailed; }

    inline void                     IncreaseCounter(SAPDB_Int8 Count)           { Counter += Count; }
    inline void                     IncreaseCounter()                           { ++Counter; }
    inline void                     SetCounter(SAPDB_Int8 Count)                { Counter = Count; }
    inline SAPDB_Int8               GetCounter()                                { return Counter; }

    inline void IncreasePagesCounter(TypeOfPages Type)
    {
        // Make sure to allow only LONGLONGDATA and SHORTLONGDATA and PRIMARYDATA
        assert((LONGLONGDATA == Type) || (SHORTLONGDATA == Type) || (PRIMARYDATA == Type));

        if (LONGLONGDATA == Type)
        {
            ++CntLongData;
        }
        else if (SHORTLONGDATA == Type)
        {
            ++CntShortLongData;
        }
        else
        {
            ++CntPrimaryData;
        }
    }

    inline void IncreasePagesCounter(TypeOfPages Type, SAPDB_Int8 Count)
    {
        // Make sure to allow only LONGLONGDATA and SHORTLONGDATA and PRIMARYDATA
        assert((LONGLONGDATA == Type) || (SHORTLONGDATA == Type) || (PRIMARYDATA == Type));

        if (LONGLONGDATA == Type)
        {
            CntLongData += Count;
        }
        else if (SHORTLONGDATA == Type)
        {
            CntShortLongData += Count;
        }
        else
        {
            CntPrimaryData += Count;
        }
    }

    inline void SetPagesCounter(TypeOfPages Type, SAPDB_Int8 Count)
    {
        // Make sure to allow only LONGLONGDATA and SHORTLONGDATA and PRIMARYDATA
        assert((LONGLONGDATA == Type) || (SHORTLONGDATA == Type) || (PRIMARYDATA == Type));

        if (LONGLONGDATA == Type)
        {
            CntLongData = Count;
        }
        else if (SHORTLONGDATA == Type)
        {
            CntShortLongData = Count;
        }
        else
        {
            CntPrimaryData = Count;
        }
    }

    inline SAPDB_Int8 GetPagesCount(TypeOfPages Type)
    {
        if (LONGLONGDATA == Type)
        {
            return CntLongData;
        }
        else if (SHORTLONGDATA == Type)
        {
            return CntShortLongData;
        }
        else if (LONGDATA == Type)
        {
            return (CntShortLongData + CntLongData);
        }
        else if (ALLDATA == Type)
        {
            return (CntPrimaryData + CntShortLongData + CntLongData);
        }
        else
        {
            return CntPrimaryData;
        }
    }


};
// class Loader_RTStatistics

#endif /*HLS98MSG_H*/
