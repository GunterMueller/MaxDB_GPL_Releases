/*! 
  -----------------------------------------------------------------------------
   module: hls24.h
   -----------------------------------------------------------------------------
   responsible:  SteffenS

  special area: Replication Manager
 
  description:  Default and NULL value handling

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
#ifndef HLS24
#define HLS24

#include "gls00.h"

/*!
  -----------------------------------------------------------------------------
 
  function:     ls24InsertDefaultVal

  -----------------------------------------------------------------------------

  description:  Checks the data type of a column to decide which kind of default
                value to assign. Then calls the respective function to insert
                the right value.
                Except of NULL-defaults and other defaults no defined bytes are
                inserted. In these two cases the defined byte is inserted!!!!!!!

  arguments:    FieldBasicInfo  [IN]    - reference, contains all needed information
                                          on the column
                ColDefault      [IN]    - user defined default value for this column,
                                          delivered by the kernel on request
                ColLenType      [IN]    - length type value for this column,
                                          delivered by the kernel on request
                pszBuffer       [OUT]   - Pointer to string to be filled with default val.
                lBufferLength   [IN]    - Length of buffer which has to be filled.
                                          NOT USED so far.
                DefaultStruct   [IN]    - Structure holding information about user and
                                          user group. Defaults to NULL-pointer to make it
                                          possible to call from functions not knowing
                                          those values.
                pszOtherDefault [IN]    - User defined default value to be inserted.
                                          Defaults to NULL-pointer (see above)

  returnvalue:  lValueLen               - Length of default value given back
                -1                      - unknown kind of default
  -----------------------------------------------------------------------------
*/
int ls24InsertDefaultVal(tls00_FieldBasics&     FieldBasicInfo,
                         tsp7_len_type_Enum     ColLenType,
                         tsp7_default_Enum      ColDefault,
                         tsp00_Addr             pszBuffer,
                         tsp00_Int4             lBufferLength,
                         tls00_PredefDefault    *DefaultStruct,
                         tls00_String           *pszOtherDefault);



/*!
  -----------------------------------------------------------------------------
   function:     ls24InsertAssignedVal
  -----------------------------------------------------------------------------
  description:  Defines values to insert assigned by special keywords in the
                load command. Special masks define if the value may be asserted
                into the column of the given data type.

  arguments:    pColumn         [IN]    - structure holding all information
                                          on the column to load assigned value
                FieldBasicInfo  [IN]    - contains all db type information of the column
                pszValue        [OUT]   - String to be filled with assigned val
                lBufferLength   [IN]    - Length of buffer pszValue which has to be filled;
                                          NOT USED so far
                DefaultStruct   [IN]    - Information about user and user group.
                                          Defaults to NULL-pointer to make it possible
                                          to call from functions not knowing those values.

  returnvalue:  lLength                     - Length of assigned value given back
                errIncompatibleConst_els98  - Error: value and column type are not compatible
                errUnknownAssignment_els98  - unknown assignment
                errInvalidNumber_els98      - error converting sequence number assignment
  -----------------------------------------------------------------------------
*/
int ls24InsertAssignedVal(tls00_Column*         pColumn,
                          tls00_FieldBasics&    FieldBasicInfo,
                          tsp00_Addr            pszValue,
                          tsp00_Int4            lBufferLength,
                          tls00_PredefDefault   *DefaultStruct = NULL);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls24SpecialFormat

  -----------------------------------------------------------------------------

  description:  Defines values to insert assigned by special keywords in the
                load command. Special masks define if the value may be asserted
                into the column of the given data type.

  arguments:    Value           [IN]    - referenz auf string struktur mit
                                          umzuwandelndem/zu pruefendem wert und seiner
                                          laenge
                FieldBasicInfo  [IN]    - reference, contains all needed information of
                                          the column
                FileFormat      [IN]    - structure that contains information on external
                                          user defined format.

  returnvalue:  errOK_els00             - insertion without error
                errInvalidBool_els98    - Error: wrong value for bool constant
                errWrongDateTime_els98  - Error: wrong value for date, time or
                                          timestamp constant
  -----------------------------------------------------------------------------
*/
int ls24SpecialFormat(tls00_String&         Value,
                      tls00_FieldBasics&    FieldBasicInfo,
                      tls00_FileFormat&     FileFormat);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls24ExtractDateTime

  -----------------------------------------------------------------------------

  description:  Wandelt einen Zeit-, Datums- oder Zeitstempelwert aus dem
                datenbankinternen format INTERNAL in das vom benutzer gewuenschte
                format um.
                Der umgewandelte wert wird in der variablen pszDestValue
                zurueckgegeben. Diese muss der aufrufer vorher mit der richtigen
                laenge angelegt haben.

  arguments:    pszSrcValue      [IN]  - char pointer,
                                         dieser zeiger zeigt auf den umzuwandelnden wert
                pszDestValue     [OUT] - char pointer,
                                         dieser zeiger enthaelt den umgewandelnten wert
                lDestValueLength [OUT] - reference to integer, laenge von pszDestValue
                DataType         [IN]  - datentyp anhand dessen die umwandlungsfunktion
                                         ausgewaehlt wird
                FileFormat       [IN]  - structure that contains information on external
                                         user defined format.

  returnvalue:  errOK_els00 - umwandlung ohne fehler
                -1          - unbekannter datentyp fuer wert
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls24ExtractDateTime(tsp00_Addr           pszSrcValue,
                    tsp00_Addr           pszDestValue,
                    tsp00_Longint       &lDestValueLength,
                    tsp00_DataType_Enum  DataType,
                    tls00_FileFormat    *FileFormat);

/*!
  -----------------------------------------------------------------------------
 
  function:     ls24GetNILConstants - overloaded (FOR FASTLOAD)

  -----------------------------------------------------------------------------

  description:  Bereitet das einfuegen eines NULLwertes in eine spalte vor.

                Folgende faelle sind zu unterscheiden:
                1. Spalte erlaubt nullwerte
                  a. Der datensatz (DS) in der datendatei entspricht dem
                     nullwertindikator
                     ---> laenge des wertes wird auf INSERT_NULL gesetzt.
                  b. Der DS ist leer
                     ---> keine aenderung der laenge (kein fehler!)
                  c. Der DS entspricht NICHT dem nullwertindikator.
                     ---> DS ist realer DS und damit keine aenderung der laenge.
                
                2. Spalte erlaubt keine nullwerte aber defaultwerte
                   ---> laenge des wertes wird auf INSERT_NULL gesetzt; dieser
                   fall ist vom aufrufer weiter zu behandeln.
                
                3. Spalte erlaubt keine nullwerte
                   --> fehler an den aufrufer

  arguments:    Value       [IN/OUT]    - reference, structure containing the user
                                          data to compare with in case of DEFAULT NULL.
                                          The length in the structure will be adapted
                                          by this function.
                DataLine    [IN]        - In case there is no DEFAULT NULL condition
                                          but a normal condition like (IF POS ... )
                                          DataLine contains the data line to extract
                                          the value to compare with from. 
                ColumnMode  [IN]        - enum type containing the column mode
                                          possible values: mod_syskey, mod_key
                                                           mod_mandatory,
                                                           mod_optional, mod_dropped
                                          These values are specific to fastload!
                NILCond     [IN]        - Reference, structure containing information
                                          about NULL condition of the column.
                Default     [IN]        - enum type, indication the default 'behavior'
                                          of the column.
                FileFormat  [IN]        - reference, structure containing information
                                          about the data in the data file

  returnvalue:  errOK_els00             - Everything is fine.
                                          Wird auch zurueckgegeben, wenn der uebergebene wert
                                          nicht der null-wert, sondern ein realer wert ist
                                          und hier nicht behandelt werden muss.
                errNullNotAllowed_els98 - Error: column does not allow NULL values.
  -----------------------------------------------------------------------------
*/
int ls24GetNILConstants(tls00_String&       Value,
                        tls00_String        DataLine,
                        tsp00_ColMode_Enum  ColumnMode,
                        tls00_CondSpec&     NILCond,
                        tsp7_default_Enum   Default,
                        tls00_FileFormat &  FileFormat);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls24GetNILConstants - overloaded (FOR DATALOAD)

  -----------------------------------------------------------------------------

  description:  see the function for fastload

  arguments:    Value       [IN/OUT]    - reference, structure containing the user
                                          data to compare with in case of DEFAULT NULL.
                                          The length in the structure will be adapted
                                          by this function.
                DataLine    [IN]        - In case there is no DEFAULT NULL condition
                                          but a normal condition like (IF POS ... )
                                          DataLine contains the data line to extract
                                          the value to compare with from. 
                ColumnMode  [IN]        - enum type containing the column mode
                                          possible values: sp1ot_mandatory,
                                                           sp1ot_optional,
                                                           sp1ot_default,
                                                           sp1ot_escape_char
                                          These values are specific to fastload!
                NILCond     [IN]        - Reference, structure containing information
                                          about NULL condition of the column.
                Default     [IN]        - enum type, indication the default 'behavior'
                                          of the column.
                FileFormat  [IN]        - reference, structure containing information
                                          about the data in the data file

  returnvalue:  errOK_els00             - Everything is fine.
                                          Wird auch zurueckgegeben, wenn der uebergebene wert
                                          nicht der null-wert, sondern ein realer wert ist
                                          und hier nicht behandelt werden muss.
                errNullNotAllowed_els98 - Error: column does not allow NULL values.
  -----------------------------------------------------------------------------
*/
int ls24GetNILConstants(tls00_String&       Value,
                        tls00_String        DataLine,
                        tsp1_param_opt_set  ColumnMode,
                        tls00_CondSpec&     NILCond,
                        tls00_FileFormat&   FileFormat);


/*
  -----------------------------------------------------------------------------
  function:     ls24SurrogateIncrease
  -----------------------------------------------------------------------------

  description:  Increases syskey value by 1.
                Only the last 8 bytes of the key are necessary for this
                operation.
  
  arguments:    Key     [IN] - last value of syskey that is to increase (only the last
                                8 bytes)
                lLength [IN] - length of syskey (defaults to 8)

  returnvalue:  errOK_els00             - in case of NO errors
                errSyskeyExceeded_els98 - syskey exceeds maximum
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls24SurrogateIncrease(unsigned char* pValue);


//*
//*     All following prototypes (i41...) are for compatibility reasons with the respective
//*     pascal source vin41 where no prototypes exist.
//*
externPascal void i41dget(tin_date_mask&    DateStruct,
                          tsp00_Addr        pszSourceDate,
                          int               lDateLength,
                          tsp00_Addr        pszDestDate,
                          int&              lError);

externPascal void i41tget(tin_date_mask&    TimeStruct,
                          tsp00_Addr        pszSourceTime,
                          int               lTimeLength,
                          tsp00_Addr        pszDestTime,
                          int&              lError);

externPascal void i41tsget(tin_date_mask&   TimeStampStruct,
                           tsp00_Addr       pszSourceTimeStamp,
                           int              lTimeStampLength,
                           tsp00_Addr       pszDestTimeStamp,
                           int&             lError);

externPascal void i41dput(tin_date_mask&  DateStruct,
                          tsp00_Addr      pszDate,      // data from kernel, at least 8 bytes long
                          tin_date_mask&  OutDateStruct);       // wird offensichtlich gefuellt
 
externPascal void i41tput(tin_date_mask&  TimeStruct,
                          tsp00_Addr      pszTime,      // data from kernel, at least 20 bytes long
                          tin_date_mask&  OutTimeStruct);       // wird offensichtlich gefuellt

externPascal void i41tsput(tin_date_mask&  TimeStampStruct,
                           tsp00_Addr      pszTimeStamp,        // data from kernel, at least 20 bytes long
                           tin_date_mask&  OutTimeStampStruct);     // wird offensichtlich gefuellt

#endif /*HLS24*/