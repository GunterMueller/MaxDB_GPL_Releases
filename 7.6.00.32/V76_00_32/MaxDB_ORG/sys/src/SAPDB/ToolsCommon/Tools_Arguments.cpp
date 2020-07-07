/*!
  @file           Tools_Arguments.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          argument parsing - Implementation

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <ctype.h>
#include "SAPDB/ToolsCommon/Tools_Arguments.hpp"

#define MASKCHAR   '\\'
#define ASSIGNCHAR '='
#define STRINGCHAR '"'
#define OPTIONCHAR '-'

/*! @brief   Constructor */
Tools_Arguments :: Tools_Arguments
    ( )
    : m_Buffer(NULL),
      m_nCount(0)
{
} // end Tools_Arguments :: Tools_Arguments

/*! @brief   Destructor */
Tools_Arguments :: ~Tools_Arguments
    (  )
{
    if (m_Buffer != NULL) {
        delete [] m_Buffer;
    } // end if
} // end Tools_Arguments :: ~Tools_Arguments

/*! @brief   public function */
void Tools_Arguments :: setArguments
    (const char * szSource)
{
    size_t nLengthOfBuffer = SAPDB_strlen(szSource) + 1;

    if (m_Buffer != NULL) {
        delete [] m_Buffer;
    } // end if
    m_Buffer = new char[nLengthOfBuffer];
    if (m_Buffer == NULL) {
        return;
    } // end if

    size_t nRemainingLengthOfBuffer = nLengthOfBuffer;
    char * pCurrent = m_Buffer;

    size_t nIndexInSource   = 0;

    const char * pValue    = NULL;
    const char * pName     = NULL;
    
    this->m_nCount       = 0;
    this->m_pValues.clear();
    this->m_pNames.clear();

    // get the next token
    while (getTokenFromBuffer(szSource, this->m_nCount + 1, pCurrent, nRemainingLengthOfBuffer, false)) {

        // compute remaining space in buffer
        nRemainingLengthOfBuffer = nRemainingLengthOfBuffer - strlen(pCurrent) - 1;

        // initialize values
        nIndexInSource     = 0;
        pValue             = pCurrent;
        pName              = NULL;

        while (pCurrent[nIndexInSource] != 0) {
            // search for assign character '='
            if (pCurrent[nIndexInSource] == ASSIGNCHAR) {
                pName  = pValue;
                pValue = pCurrent + nIndexInSource + 1;
                pCurrent[nIndexInSource] = 0;
                break;
            } // end if
            ++nIndexInSource;
        } // end while

        // check for option character
        if ((pName == NULL) && (pValue != NULL) && (*pValue == OPTIONCHAR)) {
            pName  = ++pValue;
            pValue = NULL;
        } // end if

        // add name/value to arrays
        if ((pName != NULL) || (pValue != NULL)) {
          this->m_pValues[this->m_nCount] = pValue;
          this->m_pNames [this->m_nCount] = pName;
          ++this->m_nCount;
        } // end if

        // compute free adress in buffer  
        pCurrent = m_Buffer + (nLengthOfBuffer - nRemainingLengthOfBuffer);
    } // end while

} // end Tools_Arguments ::setArguments

/*! @brief  public member */
Tools_DynamicUTF8String Tools_Arguments :: getValue
    ( const char * sName )
{
    SAPDB_Int nIndex = 0;

    for (nIndex = 0; nIndex < this->m_nCount; ++nIndex) {
        if (SAPDB_strcasecmp(sName, (m_pNames[nIndex] == NULL) ? "" : m_pNames[nIndex]) == 0) {
            return getValue(nIndex + 1);
        } // end if
    } // end for

    return "";
} // end Tools_Arguments :: getValue

/*! @brief  public member */
Tools_DynamicUTF8String Tools_Arguments :: getValue
    ( SAPDB_Int nValue )
{
  if (nValue > 0 && nValue <= this->m_nCount) {
      return (Tools_DynamicUTF8String::ConstPointer) ((m_pValues[nValue - 1] == NULL) ? "" : m_pValues[nValue - 1]);
  } // end if
  
  return "";
} // end Tools_Arguments :: getValue

/*! @brief  public member */
Tools_DynamicUTF8String Tools_Arguments :: getName
    ( SAPDB_Int nValue )
{
  if (nValue > 0 && nValue <= this->m_nCount) {
    return (Tools_DynamicUTF8String::ConstPointer) ((m_pNames[nValue - 1] == NULL) ? "" : m_pNames[nValue - 1]);
  } // end if

  return "";
} // end Tools_Arguments :: getName

/*! @brief  public member */
SAPDB_Int Tools_Arguments :: getCount
    ( )
{
  return this->m_nCount;
} // end Tools_Arguments :: getCount

/*! @brief  static public member */
bool Tools_Arguments :: getTokenFromBuffer
    ( const char * pBuffer,
      const long   nRequestedToken,
      char       * pToken,
      const long   nMaxLengthOfToken,
      bool         bWithEnclosingQuotes )
{
    bool bString    = false;
    bool bMask      = false;
    long nToken     = 0;
    long nTarget    = 0;
    long nMaxLength = nMaxLengthOfToken - 1;
    long nBackslashcounter = 0;

    // initialize token
    if ((pToken != NULL) && (nMaxLengthOfToken > 0)) {
        memset(pToken, 0, nMaxLengthOfToken);
    } // end if

    // no buffer -> no arguments -> no success
    if (*pBuffer != NULL) {
        while (*pBuffer != 0) {
            // find start of token
            while ((*pBuffer != 0) && isspace(*pBuffer)) {
                ++pBuffer;
            } // end if

            // stop on end
            if (*pBuffer == 0) {
                break;
            } // end if

            ++nToken;

            // loop until end of token
            bString = false;
            nTarget = 0;
            nBackslashcounter = 0;
            while ((*pBuffer != 0) && ( bString || (!isspace(*pBuffer)) ) ) {

                // look for double quote
                if (*pBuffer == STRINGCHAR) {

                    // check for mask semantik 
                    bMask = ((nBackslashcounter % 2) == 1);

                    // cut backslashes
                    if ((pToken != NULL) && (nTarget < nMaxLength) && (nToken == nRequestedToken)) {
                        for (nBackslashcounter = (nBackslashcounter+1)/2; nBackslashcounter > 0; --nBackslashcounter) {
                            pToken[--nTarget] = 0;
                        } // end for
                    } // end if
        
                    if (bMask) {
                        // this is a masked double quote becuase there is a odd number of backslashes

                        // add doublequote 
                        if ((pToken != NULL) && (nTarget < nMaxLength) && (nToken == nRequestedToken)) {
                            pToken[nTarget++] = *pBuffer;
                        } // end if
                    } else {
                        // this is the closing doublequote because there is a even number of backslashes

                        // maintain string flag
                        bString = !bString;

                        // add doublequote 
                        if ((pToken != NULL) && (nTarget < nMaxLength) && (nToken == nRequestedToken) && bWithEnclosingQuotes) {
                            pToken[nTarget++] = *pBuffer;
                        } // end if
                    } // end if

                // other character 
                } else {
                    // add character
                    if ((pToken != NULL) && (nTarget < nMaxLength) && (nToken == nRequestedToken)) {
                        pToken[nTarget++] = *pBuffer;
                    } // end if
                } // end if

                // backslash counter
                if (*pBuffer == MASKCHAR) { 
                    ++nBackslashcounter;
                } else {
                    nBackslashcounter = 0;
                } // end if

                ++pBuffer;
            } // end while

            if (nToken == nRequestedToken) {
                return true;
            } // end if
        } // end while
    } // end if

    return false;
} // end Tools_Arguments :: getTokenFromBuffer

/*! @brief  static public member */
bool Tools_Arguments :: getTokenFromBuffer
    ( const char  * pBuffer,
      const long    nRequestedToken,
      TokenBuffer & oToken,
      bool          bWithEnclosingQuotes)
{
    oToken.Init();
    return getTokenFromBuffer(pBuffer, nRequestedToken, oToken, oToken.size(), bWithEnclosingQuotes);
} // end Tools_Arguments :: getTokenFromBuffer
