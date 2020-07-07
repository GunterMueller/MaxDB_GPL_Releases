/*!
  @file           Tools_Arguments.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          argument parsing - Specification

\if EMIT_LICENCE

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



\endif
*/

#ifndef _Tools_Arguments_HPP_
#define _Tools_Arguments_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "SAPDB/ToolsCommon/Tools_Array.hpp"

/*!
  @brief     argument object

  This class may parse a string with arguments or options. The
  class builts a list of name/value pairs from the tokens 
  of input string. A name value/pair is a token with a assign 
  character ('=') inside. To enclose withspaces in a token use 
  doublequotes ('"'). To mask special characters use the
  backslash ('\'). Please regard there are some shells which
  will interpret and remove special characters like doublequotes 
  and backslashes too.

  Examples for tokens and their results:

  <ul>
  <li><b>name=value</b>&nbsp;&nbsp;      This creates a name/value pair.</li>
  <li><b>value</b>&nbsp;&nbsp;           A token without assign character ('=') creates a value without name.</li>
  <li><b>"name=value"</b>&nbsp;&nbsp;    This create a value without name, because the assign character is inside the doublequotes.</li>
  <li><b>"name"="value"</b>&nbsp;&nbsp;  This create a name/value pair. There may be whitespaces inside name or value.</li>
  <li><b>=value</b>&nbsp;&nbsp;          This creates a value without name.</li>
  <li><b>name=</b>&nbsp;&nbsp;           This creates a name without value.</li>
  <li><b>-name</b>&nbsp;&nbsp;           This creates a name without value.</li>
  <li><b>-name=value</b>&nbsp;&nbsp;     This creates a name/value pair.</li>
  </ul>
 */
class Tools_Arguments {

private:
  /*!
    @brief   private Copy  constructor 
  */
  Tools_Arguments
      ( const Tools_Arguments &copy ) { }

public:
  /*!
    @brief   Default constructor 
  */
  Tools_Arguments
    ( );

  /*!
    @brief   Destructor
  */
  ~Tools_Arguments
    (  );

  /*!
    @brief   Set arguments
  */
  void setArguments
    ( const char * src );

  /*!
    @brief  read count of mane/value pairs
  */
  SAPDB_Int getCount
    ( );

  /*!
    @brief  read a value by number
    @param  nValue    [IN] - number 
  */
  Tools_DynamicUTF8String getValue
    ( SAPDB_Int nValue );

  /*!
    @brief  read a value by name
    @param  sName         [IN] - name of value
    @param  bCasesensitiv [IN] - case sensitivity flag
  */
  Tools_DynamicUTF8String getValue
    ( const char * sName );

  /*!
    @brief  read the name by number
    @param  nValue    [IN] - number 
  */
  Tools_DynamicUTF8String getName
    ( SAPDB_Int nValue );

  /*!
    @brief  read the requested token from the buffer
    @param  pBuffer              [IN] - zero terminated argument buffer
    @param  nRequestedToken      [IN] - number of token (starts with 1)
    @param  oToken               [OUT]- target buffer of predefined type
    @param  pToken               [IN] - pointer to target buffer for the token or NULL if the caller 
                                        only wants to check the existence of a token
    @param  nMaxLengthOfToken    [IN] - length of the buffer pToken for the token or 0 if the caller 
                                        only wants to check the existence of a token
    @param  bWithEnclosingQuotes [IN] - obtain enclosing double quotes in the target buffer (or not)

    @return true if the token was found
  */
  static bool getTokenFromBuffer
    ( const char * pBuffer,
      const long   nRequestedToken,
      char       * pToken,
      const long   nMaxLengthOfToken,
      bool         bWithEnclosingQuotes = false);

  typedef tsp00_C512c TokenBuffer;

  static bool getTokenFromBuffer
    ( const char  * pBuffer,
      const long    nRequestedToken,
      TokenBuffer & oToken,
      bool          bWithEnclosingQuotes = false);

private:
  Tools_Array<const char *> m_pNames;
  Tools_Array<const char *> m_pValues;
  char *                    m_Buffer;
  SAPDB_Int                 m_nCount;

}; // end class Tools_Arguments


#endif /* _Tools_Arguments_HPP_ */
