/*!
  @file           Loader_ErrorCode.hpp
  @author         DL SAP DB TOOLS
  @brief          Error Codes for the Loader
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
#ifndef LOADER_ERRORCODE_H
#define LOADER_ERRORCODE_H



typedef enum
{
    // Error numbers correspond (mainly) to error numbers in Loader

    // internal errors at load execution
    LOA_ERR_UNKNOWN_DEFAULT                     = -25893,

    LOA_ERR_ENCODING_SRC_EXHAUSTED              = -25870,
    LOA_ERR_ENCODING_TGT_EXHAUSTED,             //= -25869

    LOA_ERR_UNKNOWN_PART                        = -25807,

    LOA_ERR_INTERNAL                            = -25801,

    LOA_ERR_COLUMN_SPEC_MISSING                 = -25799,
    LOA_ERR_CMD_NOT_INITIALIZED,                //= -25798
    LOA_ERR_INVALID_PARAMETERINDEX,             //= -25797
    LOA_ERR_INVALID_HOSTTYPE,                   //= -25796

    LOA_ERR_NULL_PARAMETERADDR                  = -25794,
    LOA_ERR_NEGATIVE_BUFFERLEN,                 //= -25793
    LOA_ERR_INVALID_LENGTHINDICATOR,            //= -25792
    LOA_ERR_INVALID_ROWARRAYSIZE,               //= -25791
    LOA_ERR_PARAMETER_NOT_SET,                  //= -25790

    LOA_ERR_TOO_MANY_FLCMD_OBJECTS             = -25701,

    LOA_ERR_SQL_CMD_EXECUTION_FAILED            = -25395,
    LOA_ERR_CONVERTING_ENCODING                 = -25371,

    LOA_ERR_EMPTY_NOT_ALLOWED              = -25320,
    LOA_ERR_MISSING_DATA,                       //= -25319,
    LOA_ERR_SYSKEY_EXCEEDED,                    //= -25318
    LOA_ERR_INCOMPATIBLE_CONST_LENGTH,          //= -25317,       // new: (former errIncompatibleConst_els98); speziell fuer die Laenge; sollte Meldung auch enthalten - value too long

    LOA_ERR_LONG_COL_NOT_ALLOWED,               //= -25316

    LOA_ERR_NUMBER_OVERFLOW                    = -25313,

    // Dieser fehler sollte ohne fehlertext auskommen erstmal, den gibts nur, um anzuzeigen
    // dass bei der umwandlung einer benutzergelieferten zahl in eine interne number 
    // die number um die letzten stellen gekuerzt wurde.
    // Das ist somit eine warnung.
    LOA_ERR_NUMBER_TRUNCATED,                    //= -25312

    LOA_ERR_MISSING_DATA_NN_COL,                //= -25311
    LOA_ERR_NULL_NOT_ALLOWED,                   //= -25310

    LOA_ERR_INVALID_HEX                         = -25306,
    LOA_ERR_KEY_LENGTH,                         //= -25305

    LOA_ERR_RECOVER_KEY_ORDER_VIOLATION         = -25304,
    LOA_ERR_KEY_ORDER                           = -25303,

    LOA_ERR_WRONG_DATE_TIME                     = -25301,

    //*
    //*     Parser error or scanner error
    //*
    LOA_ERR_GET_USERNAME_FAILED                 = -25015,
    LOA_ERR_SQLDBC_CONNECT_FAILED,             //= -25014,
    LOA_ERR_CONNECTION_ALREADY_OPEN,           //= -25013,
    LOA_ERR_NO_CONNECTION,                     //= -25012,
    LOA_ERR_INTERNAL_SQL_FAILED,               //= -25011,

    LOA_ERR_DB_COMMUNICATION_FAILURE            = -25009,

    LOA_ERR_OK                                    = 0,

    // do not use
    LOA_END_OF_ARRAY                         = 26000
} LOADER_ErrorCode;


#endif // LOADER_ERRORCODE_H