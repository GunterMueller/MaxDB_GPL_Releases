/*!================================================================

 module:        PIn_ReplayParser.cpp

 -------------------------------------------------------------------

 responsible:   TTK

 special area:  SQL

 description:   ReplyParser

 -------------------------------------------------------------------

 Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




 ===================================================================*/

#include "SAPDB/PacketInterface/PIn_ReplyParser.h"
#include "hsp40.h"

PIn_ReplyParser::AcceptKind PIn_ReplyParser::ParseReply (const PIn_ReplySegment & replySegment)
{
    PIn_Part part = replySegment.GetFirstPart ();
    AcceptKind accepted = handled_e;
    tsp1_part_header * header;
    tsp1_part_kind_Enum partKind;
    int argCount;
    int dataLen;
    const tsp00_Byte * dataPtr;

    while (part.IsValid () && (accepted != abort_e)) {
        header = part.GetRawHeader ();
        partKind = header->sp1p_part_kind;
        argCount = header->sp1p_arg_count;
        dataLen = header->sp1p_buf_len;
        dataPtr = part.GetReadData ();

        switch (partKind) {
            case sp1pk_appl_parameter_description:
                accepted = this->HandleApplParameterDescription (argCount, dataLen, dataPtr);
                break;
            case sp1pk_columnnames:
                accepted = this->HandleColumnnames (argCount, dataLen,
                    REINTERPRET_CAST (const char *, dataPtr));
                break;
            case sp1pk_command:
                accepted = this->HandleCommand (argCount, dataLen,
                    REINTERPRET_CAST (const char *, dataPtr));
                break;
            case sp1pk_conv_tables_returned:
                accepted = this->HandleConvTablesReturned (argCount, dataLen, dataPtr);
                break;
            case sp1pk_data:
                accepted = this->HandleData (argCount, dataLen, dataPtr);
                break;
            case sp1pk_errortext:
                accepted = this->HandleErrortext (argCount, dataLen,
                    REINTERPRET_CAST (const char *, dataPtr));
                break;
            case sp1pk_getinfo:
                accepted = this->HandleGetinfo (argCount, dataLen, dataPtr);
                break;
            case sp1pk_modulname:
                accepted = this->HandleModulname (argCount, dataLen,
                    REINTERPRET_CAST (const char *, dataPtr));
                break;
            case sp1pk_page:
                accepted = this->HandlePage (argCount, dataLen, dataPtr);
                break;
            case sp1pk_parsid:
                accepted = this->HandleParsid (argCount, dataLen,
                    REINTERPRET_CAST (const tin02_ParseId *, dataPtr));
                break;
            case sp1pk_parsid_of_select:
                accepted = this->HandleParsidOfSelect (argCount, dataLen,
                    REINTERPRET_CAST (const tin02_ParseId *, dataPtr));
                break;
            case sp1pk_resultcount:
                {
                    tsp00_NumError num_err;
                    tsp00_Int4 iCount;
                    s40glint ((unsigned char*)dataPtr, 2, dataLen, iCount, num_err);
                    if ( num_err !=  num_ok) 
                        iCount = -1;
                    accepted = this->HandleResultcount (argCount, dataLen, iCount);
                }
                break;
            case sp1pk_resulttablename:
                accepted = this->HandleResulttablename (argCount, dataLen,
                    REINTERPRET_CAST (const char *, dataPtr));
                break;
            case sp1pk_shortinfo:
                accepted = this->HandleShortinfo (argCount, dataLen,
                    REINTERPRET_CAST (const tsp1_param_info *, dataPtr));
                break;
            case sp1pk_user_info_returned:
                accepted = this->HandleUserInfoReturned (argCount, dataLen, dataPtr);
                break;
            case sp1pk_surrogate:
                accepted = this->HandleSurrogate (argCount, dataLen, dataPtr);
                break;
            case sp1pk_bdinfo:
                accepted = this->HandleBdinfo (argCount, dataLen, dataPtr);
                break;
            case sp1pk_longdata:
                accepted = this->HandleLongdata (argCount, dataLen, dataPtr);
                break;
            case sp1pk_tablename:
                accepted = this->HandleTablename (argCount, dataLen,
                    REINTERPRET_CAST (const char *, dataPtr));
                break;
            case sp1pk_session_info_returned:
                accepted = this->HandleSessionInfoReturned (argCount, dataLen, dataPtr);
                break;
            case sp1pk_output_cols_no_parameter:
                accepted = this->HandleOutputColsNoParameter (argCount, dataLen, dataPtr);
                break;
            case sp1pk_key:
                accepted = this->HandleKey (argCount, dataLen, dataPtr);
                break;
            case sp1pk_serial:
                accepted = this->HandleSerial (argCount, dataLen, dataPtr);
                break;
            case sp1pk_relative_pos:
                accepted = this->HandleRelativePos (argCount, dataLen, dataPtr);
                break;
            case sp1pk_abap_istream:
                accepted = this->HandleAbapIstream (argCount, dataLen, dataPtr);
                break;
            case sp1pk_abap_ostream:
                accepted = this->HandleAbapOstream (argCount, dataLen, dataPtr);
                break;
            case sp1pk_abap_info:
                accepted = this->HandleAbapInfo (argCount, dataLen, dataPtr);
                break;
            default:
                accepted = unknown_e;
                break;
        }
        if (accepted == unknown_e) {
            accepted = this->HandleUnknownPart (partKind, argCount, dataLen, dataPtr);
        }
        part = replySegment.GetNextPart (part);
    }
    return accepted;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleUnknownPart (
    tsp1_part_kind_Param partKind,
    int argCount,
    int len,
    const tsp00_Byte * data)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleApplParameterDescription (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleColumnnames (
    int argCount,
    int dataLen,
    const char * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleCommand (
    int argCount,
    int dataLen,
    const char * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleConvTablesReturned (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleData (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleErrortext (
    int argCount,
    int dataLen,
    const char * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleGetinfo (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleModulname (
    int argCount,
    int dataLen,
    const char * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandlePage (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleParsid (
    int argCount,
    int dataLen,
    const tin02_ParseId * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleParsidOfSelect (
    int argCount,
    int dataLen,
    const tin02_ParseId * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleResultcount (
    int argCount,
    int dataLen,
    long data)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleResulttablename (
    int argCount,
    int dataLen,
    const char * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleShortinfo (
    int argCount,
    int dataLen,
    const tsp1_param_info * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleUserInfoReturned (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleSurrogate (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleBdinfo (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleLongdata (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleTablename (
    int argCount,
    int dataLen,
    const char * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleSessionInfoReturned (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleOutputColsNoParameter (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleKey (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleSerial (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleRelativePos (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleAbapIstream (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleAbapOstream (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

PIn_ReplyParser::AcceptKind PIn_ReplyParser::HandleAbapInfo (
    int argCount,
    int dataLen,
    const teo00_Byte * dataPtr)
{
    return unknown_e;
}

/*----------------------------------------*/

