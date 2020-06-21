/*!*****************************************************************************

  module      : XMLMergeErrorHandling.hpp

  ----------------------------------------------------------------------------

  responsible : PeterG (Goertzp)

  special area: Merging XML files
  description : Error handling
				
  ----------------------------------------------------------------------------

    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
    ========== licence end

*******************************************************************************/


#ifndef XMLMergeErrorHandling__HPP
#define XMLMergeErrorHandling__HPP


#define  C_TOO_FEW_INPUT_PARAMS                                     1
#define  C_XML_FILE_FIRST                                           2
#define  C_FILENAME_TOO_LONG                                        3
#define  C_NO_XML_FILE                                              4
#define  C_BAD_INPUT_FILENAME                                       5
#define  C_BAD_INPUT                                                6
#define  C_FILE_OPEN_ERROR                                          7
#define  C_FILE_CLOSE_ERROR                                         8
#define  C_TOO_MANY_DOUBLE_QUOTES                                   9
#define  C_PATH_TOO_LONG                                           10
#define  C_FILENAME_NOT_FOUND                                      11
#define  C_BAD_IMPORT_TAG                                          12
#define  C_BAD_INCLUDE_TAG                                         13
#define  C_FGETS_FAILED                                            14
#define  C_BAD_CALL_OF_IMPORTSTARTTAGOK                            15
#define  C_NOTONLY_BLANKS_BEFORE_IMPORTSTARTLAB                    16
#define  C_ADDLAB_IN_IMPORTSTARTLINE                               17
#define  C_IMPORTSTARTRAB_MISSING                                  18
#define  C_ADDRAB_IN_IMPORTSTARTLINE                               19
#define  C_BAD_IMPORTSTARTRAB                                      20
#define  C_NOTONLY_BLANKS_BEHIND_IMPORTSTARTRAB                    21
#define  C_BAD_CALL_OF_IMPORTENDTAGOK                              22
#define  C_NOTONLY_BLANKS_BEFORE_IMPORTENDLAB                      23
#define  C_NOTONLY_BLANKS_BEHIND_IMPORTENDRAB                      24
#define  C_FIRSTLAB_NOT_XMLDECLLAB                                 25
#define  C_NOTONLY_BLANKS_BEFORE_XMLDECLLAB                        26
#define  C_ADDLAB_IN_XMLDECLLAB_LINE                               27
#define  C_NONEMPTYLINE_BEFORE_XMLDECLSTART                        28
#define  C_ADDLAB_INSIDE_XMLDECL                                   29
#define  C_FIRSTRAB_NOT_XMLDECLRAB                                 30
#define  C_NOTONLY_BLANKS_BEHIND_XMLDECLRAB                        31
#define  C_XMLDECLTAG_NOT_FOUND                                    32
#define  C_SECONDLAB_NOT_WRAPPERSTARTLAB                           33
#define  C_NOTONLY_BLANKS_BEFORE_WRAPPERSTARTLAB                   34
#define  C_ADDLAB_IN_WRAPPERSTART_LABLINE                          35
#define  C_ADDLAB_INSIDE_WRAPPERSTART                              36
#define  C_NOTONLY_EMPTYLINES_BETWEEN_XMLDECLEND_AND_WRAPPERSTART  37
#define  C_SECONDRAB_NOT_WRAPPERSTARTRAB                           38
#define  C_NOTONLY_BLANKS_BEHIND_WRAPPERSTARTRAB                   39
#define  C_WRAPPERSTARTTAG_NOT_FOUND                               40
#define  C_NOTONLY_EMPTYLINES_BETWEEN_IMPORT_START_AND_END         41
#define  C_NOTONLY_BLANKS_BEFORE_WRAPPERENDLAB                     42
#define  C_WRAPPEREND_NOT_FOUND                                    43
#define  C_BAD_CALL_OF_INCLUDETAGOK                                44
#define  C_NOTONLY_BLANKS_BEFORE_INCLUDELAB                        45
#define  C_ADDLAB_IN_INCLUDELINE                                   46
#define  C_INCLUDERAB_MISSING                                      47
#define  C_NOTONLY_BLANKS_BEHIND_INCLUDERAB                        48
#define  C_BLANK_IN_FILENAME                                       49
#define  C_BLANK_IN_INPUT_FILENAME                                 50
#define  C_BAD_WELLFORMED_VALUE                                    51
#define  C_CPPSQLDBC_NOT_SUPPORTED                                 52 /* PTS 1138438 */
#define  C_BAD_IMPORT_WRAPPER                                      53 /* PTS 1139910 */

#define  XMLMerge_MsgCnt                                           53 /* PTS 1139910 */


void XMLMerge_ErrorMsg( const int msgNo );
void XMLMerge_ErrorMsg( const int msgNo, char * p );
void XMLMerge_ErrorMsg( const int msgNo, char * p1, char * p2 );
void XMLMerge_ErrorMsg( const int msgNo, char * p1, char * p2, char * p3 );

void XMLMerge_WarnMsg( const int msgNo );
void XMLMerge_WarnMsg( const int msgNo, char * p );
void XMLMerge_WarnMsg( const int msgNo, char * p1, char * p2 );
void XMLMerge_WarnMsg( const int msgNo, char * p1, char * p2, char * p3 );


#endif

