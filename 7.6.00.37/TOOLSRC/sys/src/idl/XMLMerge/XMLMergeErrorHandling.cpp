/*!*****************************************************************************

  module      : XMLMergeErrorHandling.cpp

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


/* with these two includes working with VS98 */
/* #include <stdio.h>                        */
/* #include <iostream.h>                     */


/* as in IspcErrorHandling.cpp */
#include <stdio.h>
#if defined(_WIN32) && (_MSC_VER >= 1300)
#include <iostream>
#include <fstream>
#define endl std::endl
#define cout std::cout
#else
#include <iostream.h>
#endif


#include "idl/XMLMerge/XMLMergeErrorHandling.hpp"


static const char * EMsgPrefix = "XMLMerge Error ";
static const char * WMsgPrefix = "XMLMerge Warning ";


static const char * XMLMerge_Msgs[XMLMerge_MsgCnt] = {
  /* C_TOO_FEW_INPUT_PARAMS */
  "Input parameter(s) missing!",
  /* C_XML_FILE_FIRST */
  "XML file to be processed must be first input parameter, in form -F<...>!",
  /* C_FILENAME_TOO_LONG */
  "File names may not exceed 250 characters!",
  /* C_NO_XML_FILE */
  "Files must have extension .xml!",
  /* C_BAD_INPUT_FILENAME */
  "Name of file to be processed may not be of form <...>__mgd.xml!",
  /* C_BAD_INPUT */
  "Additional input parameters must be of form -I<...>!",
  /* C_FILE_OPEN_ERROR */
  "File %s can not be opened!",
  /* C_FILE_CLOSE_ERROR */
  "File %s can not be closed!",
  /* C_TOO_MANY_DOUBLE_QUOTES */
  "In file %s beside delimiters for file name additional double quote found in line!",
  /* C_PATH_TOO_LONG */
  "Search path too long; skipped!",
  /* C_FILENAME_NOT_FOUND */
  "In file %s name of file to be imported or included can not be determined!",
  /* C_BAD_IMPORT_TAG */
  "IMPORT tag not allowed in file %s!",
  /* C_BAD_INCLUDE_TAG */
  "INCLUDE tag not allowed in file %s!",
  /* C_FGETS_FAILED */
  "fgets failed in file %s!",
  /* C_BAD_CALL_OF_IMPORTSTARTTAGOK */
  "Function importStartTagOK called with line containing no IMPORT tag!",
  /* C_NOTONLY_BLANKS_BEFORE_IMPORTSTARTLAB */
  "In file %s in line containing '<IMPORT FILE=' not only leading blanks found!",
  /* C_ADDLAB_IN_IMPORTSTARTLINE */
  "In file %s in line containing '<IMPORT FILE=' illegal additional '<' found!",
  /* C_IMPORTSTARTRAB_MISSING */
  "In file %s in line containing '<IMPORT FILE=' closing '>' not found!",
  /* C_ADDRAB_IN_IMPORTSTARTLINE */
  "In file %s in line containing '<IMPORT FILE=' illegal additional '>' found!",
  /* C_BAD_IMPORTSTARTRAB */
  "In file %s in line containing '<IMPORT FILE=' closing '>' may not have preceding '/'!",
  /* C_NOTONLY_BLANKS_BEHIND_IMPORTSTARTRAB */
  "In file %s in line containing '<IMPORT FILE=' behind closing '>' not only trailing blanks found!",
  /* C_BAD_CALL_OF_IMPORTENDTAGOK */
  "Function importEndTagOK called with line containing no IMPORT end tag!",
  /* C_NOTONLY_BLANKS_BEFORE_IMPORTENDLAB */
  "In file %s in line containing '</IMPORT>' not only leading blanks found!",
  /* C_NOTONLY_BLANKS_BEHIND_IMPORTENDRAB */
  "In file %s in line containing '</IMPORT>' not only trailing blanks found!",
  /* C_FIRSTLAB_NOT_XMLDECLLAB */
  "In file %s first '<' not in '<?xml'!",
  /* C_NOTONLY_BLANKS_BEFORE_XMLDECLLAB */
  "In file %s in line containing '<?xml' not only leading blanks found!",
  /* C_ADDLAB_IN_XMLDECLLAB_LINE */
  "In file %s in line containing '<?xml' illegal additional '<' found!",
  /* C_NONEMPTYLINE_BEFORE_XMLDECLSTART */
  "In file %s before line containing '<?xml' not only empty lines found!",
  /* C_ADDLAB_INSIDE_XMLDECL */
  "In file %s behind line containing '<?xml' illegal additional '<' found!",
  /* C_FIRSTRAB_NOT_XMLDECLRAB */
  "In file %s first '>' not in '?>'!",
  /* C_NOTONLY_BLANKS_BEHIND_XMLDECLRAB */
  "In file %s in line containing '?>' not only trailing blanks found!",
  /* C_XMLDECLTAG_NOT_FOUND */
  "In file %s '<?xml' or '?>' not found!",
  /* C_SECONDLAB_NOT_WRAPPERSTARTLAB */
  "In file %s second '<' not in '%s'!",
  /* C_NOTONLY_BLANKS_BEFORE_WRAPPERSTARTLAB */
  "In file %s in line containing '%s' not only leading blanks found!",
  /* C_ADDLAB_IN_WRAPPERSTART_LABLINE */
  "In file %s in line containing '%s' illegal additional '<' found!",
  /* C_ADDLAB_INSIDE_WRAPPERSTART */
  "In file %s behind line containing '%s' illegal additional '<' found!",
  /* C_NOTONLY_EMPTYLINES_BETWEEN_XMLDECLEND_AND_WRAPPERSTART */
  "In file %s between line containing '?>' and line containing '%s' not only empty lines found!",
  /* C_SECONDRAB_NOT_WRAPPERSTARTRAB */
  "In file %s second '>' not in '%s'!",
  /* C_NOTONLY_BLANKS_BEHIND_WRAPPERSTARTRAB */
  "In file %s in line containing '%s' not only trailing blanks found!",
  /* C_WRAPPERSTARTTAG_NOT_FOUND */
  "In file %s '%s' or '%s' not found!",
  /* C_NOTONLY_EMPTYLINES_BETWEEN_IMPORT_START_AND_END */
  "In file %s between line containing '<IMPORT FILE= ... >' and line containing '</IMPORT>' not only empty lines found!",
  /* C_NOTONLY_BLANKS_BEFORE_WRAPPERENDLAB */
  "In file %s in line containing '%s' not only leading blanks found!",
  /* C_WRAPPEREND_NOT_FOUND */
  "In file %s '%s' not found!",
  /* C_BAD_CALL_OF_INCLUDETAGOK */
  "Function includeTagOK called with line containing no INCLUDE tag!",
  /* C_NOTONLY_BLANKS_BEFORE_INCLUDELAB */
  "In file %s in line containing '<INCLUDE FILE=' not only leading blanks found!",
  /* C_ADDLAB_IN_INCLUDELINE */
  "In file %s in line containing '<INCLUDE FILE=' illegal additional '<' found!",
  /* C_INCLUDERAB_MISSING */
  "In file %s in line containing '<INCLUDE FILE=' no '/>' found!",
  /* C_NOTONLY_BLANKS_BEHIND_INCLUDERAB */
  "In file %s in line containing '<INCLUDE FILE= ... />' not only trailing blanks found!",
  /* C_BLANK_IN_FILENAME */
  "In file %s blank in file name found!",
  /* C_BLANK_IN_INPUT_FILENAME */
  "Input file name contains blank(s)!",
  /* C_BAD_WELLFORMED_VALUE */
  "In file %s in line containing '<INCLUDE FILE= ... />' bad value for 'WELLFORMED' found!",
  /* C_CPPSQLDBC_NOT_SUPPORTED */
  "Tag CPPSQLDBC not supported for file %s!",
  /* C_BAD_IMPORT_WRAPPER */
  "Illegal wrapper specification for file to be imported %s!"
};


void XMLMerge_ErrorMsg( const int msgNo ) {
  if ( (0 < msgNo) && (msgNo <= XMLMerge_MsgCnt) ) {
    cout << EMsgPrefix << msgNo << ": " << XMLMerge_Msgs[msgNo-1] << endl;
  }
}

void XMLMerge_ErrorMsg( const int msgNo, char * p ) {
  if ( (0 < msgNo) && (msgNo <= XMLMerge_MsgCnt) ) {
    char buffer[256];
    if (p) {
      sprintf( buffer, XMLMerge_Msgs[msgNo-1], p );
      cout << EMsgPrefix << msgNo << ": " << buffer << endl;
    }
  }
}

void XMLMerge_ErrorMsg( const int msgNo, char * p1, char * p2 ) {
  if ( (0 < msgNo) && (msgNo <= XMLMerge_MsgCnt) ) {
    char buffer[256];
    if ( p1 && p2 ) {
      sprintf( buffer, XMLMerge_Msgs[msgNo-1], p1, p2 );
      cout << EMsgPrefix << msgNo << ": " << buffer << endl;
    }
  }
}

void XMLMerge_ErrorMsg( const int msgNo, char * p1, char * p2, char * p3 ) {
  if ( (0 < msgNo) && (msgNo <= XMLMerge_MsgCnt) ) {
    char buffer[256];
    if ( p1 && p2 && p3 ) {
      sprintf( buffer, XMLMerge_Msgs[msgNo-1], p1, p2, p3 );
      cout << EMsgPrefix << msgNo << ": " << buffer << endl;
    }
  }
}

void XMLMerge_WarnMsg( const int msgNo ) {
  if ( (0 < msgNo) && (msgNo <= XMLMerge_MsgCnt) ) {
    cout << WMsgPrefix << msgNo << ": " << XMLMerge_Msgs[msgNo-1] << endl;
  }
}

void XMLMerge_WarnMsg( const int msgNo, char * p ) {
  if ( (0 < msgNo) && (msgNo <= XMLMerge_MsgCnt) ) {
    char buffer[256];
    if (p) {
      sprintf( buffer, XMLMerge_Msgs[msgNo-1], p );
      cout << WMsgPrefix << msgNo << ": " << buffer << endl;
    }
  }
}

void XMLMerge_WarnMsg( const int msgNo, char * p1, char * p2 ) {
  if ( (0 < msgNo) && (msgNo <= XMLMerge_MsgCnt) ) {
    char buffer[256];
    if ( p1 && p2 ) {
      sprintf( buffer, XMLMerge_Msgs[msgNo-1], p1, p2 );
      cout << WMsgPrefix << msgNo << ": " << buffer << endl;
    }
  }
}

void XMLMerge_WarnMsg( const int msgNo, char * p1, char * p2, char * p3 ) {
  if ( (0 < msgNo) && (msgNo <= XMLMerge_MsgCnt) ) {
    char buffer[256];
    if ( p1 && p2 && p3 ) {
      sprintf( buffer, XMLMerge_Msgs[msgNo-1], p1, p2, p3 );
      cout << WMsgPrefix << msgNo << ": " << buffer << endl;
    }
  }
}

