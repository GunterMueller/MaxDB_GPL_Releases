/*!================================================================

 module:        PIn_ReplyParser.h

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

#ifndef PIn_REPLYPARSER_H
#define PIn_REPLYPARSER_H

#include "SAPDB/PacketInterface/PIn_Packet.h"

/*!
    Class: PIn_ReplyParser

    description: iterates over the parts of a reply

        The PIn_ReplyParser will call the appropriate virtual function
        depending on the part kind. The part data will be converted
        whenever possible.

        When such a method returns unknown_e (the default behaviour), the PIn_ReplyParser
        will call handleUnknownPart.

        Some of the part kinds are only used for requests.

        This class is meant to be subclassed. Different kinds of reply
        - as identified by the function code - could be handled by different
        subclasses.

            * a ConnectReplyParser (handleUserInfoReturned)
            * a SelectReplyParser (handleResulttablename, handleShortinfo)
            * a ParseReplyParser (handleParsid, handleShortinfo)
            * a PacketTracer (all)

        <BR>
        It is possible to add a handleErrorEvent or handleFunctionCodeEvent.
        But as reacting to an SQL error is very different from reacting to
        a specific part, this is probably best left to a separate
        ErrorParser Object.

 */
class PIn_ReplyParser
{
public:
    /*! Declaration: AcceptKind

        description: enum returned by handler functions

            - <code>handled_e</code>: part has been handled by function
            - <code>abort_e</code>: an error occurred, stop processing the segment
            - <code>unknown_e</code>: part has not been handled, continue
     */
    enum AcceptKind { handled_e, abort_e, unknown_e};
    /*! EndDeclaration: AcceptKind */

    /*! Function: ParseReply
        description: iterates over a ReplySegment

        returns: abort_e when one part handler returns abort, handled_e otherwise
     */
    virtual AcceptKind ParseReply (const PIn_ReplySegment & replySegment);

protected:
    /*! Function: HandleUnknownPart
        description: called when a specific Handler returns unknown_e

            This could be used to raise an exception in the quick version.
            This allows to detect some protocol changes during development
            when a command returns additional unexpected parts.
     */
    virtual AcceptKind HandleUnknownPart (
            tsp1_part_kind_Param partKind,
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Chapter: part handler

        description: to be overridden in subclasses
     */

    /*! Function: HandleApplParameterDescription
        description: called for parts of kind sp1pk_appl_parameter_description
     */
    virtual AcceptKind HandleApplParameterDescription (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleColumnnames
        description: called for parts of kind sp1pk_columnnames
     */
    virtual AcceptKind HandleColumnnames (
            int argCount,
            int len,
            const char * data);

    /*! Function: HandleCommand
        description: called for parts of kind sp1pk_command
     */
    virtual AcceptKind HandleCommand (
            int argCount,
            int len,
            const char * data);

    /*! Function: HandleConvTablesReturned
        description: called for parts of kind sp1pk_conv_tables_returned
     */
    virtual AcceptKind HandleConvTablesReturned (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleData
        description: called for parts of kind sp1pk_data
     */
    virtual AcceptKind HandleData (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleErrortext
        description: called for parts of kind sp1pk_errortext
     */
    virtual AcceptKind HandleErrortext (
            int argCount,
            int len,
            const char * data);

    /*! Function: HandleGetinfo
        description: called for parts of kind sp1pk_getinfo
     */
    virtual AcceptKind HandleGetinfo (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleModulname
        description: called for parts of kind sp1pk_modulname
     */
    virtual AcceptKind HandleModulname (
            int argCount,
            int len,
            const char * data);

    /*! Function: HandlePage
        description: called for parts of kind sp1pk_page
     */
    virtual AcceptKind HandlePage (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleParsid
        description: called for parts of kind sp1pk_parsid
     */
    virtual AcceptKind HandleParsid (
            int argCount,
            int len,
            const tin02_ParseId * data);

    /*! Function: HandleParsidOfSelect
        description: called for parts of kind sp1pk_parsid_of_select
     */
    virtual AcceptKind HandleParsidOfSelect (
            int argCount,
            int len,
            const tin02_ParseId * data);

    /*! Function: HandleResultcount
        description: called for parts of kind sp1pk_resultcount
     */
    virtual AcceptKind HandleResultcount (
            int argCount,
            int len,
            long data);

    /*! Function: HandleResulttablename
        description: called for parts of kind sp1pk_resulttablename
     */
    virtual AcceptKind HandleResulttablename (
            int argCount,
            int len,
            const char * data);

    /*! Function: HandleShortinfo
        description: called for parts of kind sp1pk_shortinfo
     */
    virtual AcceptKind HandleShortinfo (
            int argCount,
            int len,
            const tsp1_param_info * data);

    /*! Function: HandleUserInfoReturned
        description: called for parts of kind sp1pk_user_info_returned
     */
    virtual AcceptKind HandleUserInfoReturned (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleSurrogate
        description: called for parts of kind sp1pk_surrogate
     */
    virtual AcceptKind HandleSurrogate (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleBdinfo
        description: called for parts of kind sp1pk_bdinfo
     */
    virtual AcceptKind HandleBdinfo (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleLongdata
        description: called for parts of kind sp1pk_longdata
     */
    virtual AcceptKind HandleLongdata (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleTablename
        description: called for parts of kind sp1pk_tablename
     */
    virtual AcceptKind HandleTablename (
            int argCount,
            int len,
            const char * data);

    /*! Function: HandleSessionInfoReturned
        description: called for parts of kind sp1pk_session_info_returned
     */
    virtual AcceptKind HandleSessionInfoReturned (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleOutputColsNoParameter
        description: called for parts of kind sp1pk_output_cols_no_parameter
     */
    virtual AcceptKind HandleOutputColsNoParameter (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleKey
        description: called for parts of kind sp1pk_key
     */
    virtual AcceptKind HandleKey (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleSerial
        description: called for parts of kind sp1pk_serial
     */
    virtual AcceptKind HandleSerial (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleRelativePos
        description: called for parts of kind sp1pk_relative_pos
     */
    virtual AcceptKind HandleRelativePos (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleAbapIstream
        description: called for parts of kind sp1pk_abap_istream
     */
    virtual AcceptKind HandleAbapIstream (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleAbapOstream
        description: called for parts of kind sp1pk_abap_ostream
     */
    virtual AcceptKind HandleAbapOstream (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! Function: HandleAbapInfo
        description: called for parts of kind sp1pk_abap_info
     */
    virtual AcceptKind HandleAbapInfo (
            int argCount,
            int len,
            const tsp00_Byte * data);

    /*! EndChapter: part handler */

};

/*!
    EndClass: PIn_ReplyParser
 */



#endif

