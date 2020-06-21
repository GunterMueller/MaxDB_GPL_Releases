/*! 
  -----------------------------------------------------------------------------
  module: Tools_Template.cpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: universal stuff for web access
  description:  a class for creating simple Tables in SAP MiniApps style
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------



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




*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "ToolsCommon/Tools_Template.hpp"

/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
 */
#define COMMENT_BEGIN           "<!--"
#define COMMENT_END             " -->"
#define TEMPLATE_BEGIN          "TemplateKey"
#define TEMPLATE_END            "EndTemplateKey"
#define VALUE_MARKER            "%"

#define VALLIST_CHAR            '*'

#define VALLIST_VALMARKER_BEGIN "%*"
#define VALLIST_VALMARKER_END   "*%"

#ifdef _WIN32
  #define STAT  _stat
#else
  #define STAT  stat
#endif

/*    
  =============================================================================
  class:        Tools_TemplateValue
  =============================================================================
 */
Tools_TemplateValue :: Tools_TemplateValue ( const SAPDB_UTF8 * szName,
                                             const SAPDB_UTF8 * pBegin,
                                             const SAPDB_UTF8 * pEnd )
{
  m_strName.Assign(szName);
  m_pBegin = pBegin;
  m_pEnd   = pEnd;  
}

Tools_TemplateValue :: ~Tools_TemplateValue ( )
{
}

/*    
  =============================================================================
  class:        Tools_Template
  =============================================================================
 */

/*
  -----------------------------------------------------------------------------
  public constructor Tools_Template :: Tools_Template
  -----------------------------------------------------------------------------
 */
Tools_Template  :: Tools_Template ( sapdbwa_WebAgent & wa,
                                    const SAPDB_UTF8 * szFile ) 
{
  m_pFileBuffer       = NULL;
  m_pMaster           = this;
  m_bError            = false;

  char                    szPath[256] = "";
  Tools_DynamicUTF8String aPath;

  wa.GetDocumentRoot(szPath, 200); 

  aPath.Assign(szPath)
       .Append("/")
       .Append(SAPDBWA_DOCUMENT_ROOT_HTML)
       .Append("/")
       .Append(szFile);

  readFile(aPath);

} // end Tools_Template :: Tools_Template
/*
  -----------------------------------------------------------------------------
 */
Tools_Template :: Tools_Template (       Tools_Template * pMaster,
                                         const SAPDB_UTF8        * szName,
                                         const SAPDB_UTF8        * pBegin, 
                                         const SAPDB_UTF8        * pEnd,
                                         const SAPDB_Bool          bValueList)
{
  m_pFileBuffer       = NULL;
  m_pMaster           = pMaster;
  m_bError            = false;

  analyzeTemplate(szName, pBegin, pEnd, bValueList);

} // end Tools_Template :: Tools_Template

/*
  -----------------------------------------------------------------------------
  public destructor Tools_Template  :: ~Tools_Template
  -----------------------------------------------------------------------------
 */
Tools_Template  :: ~Tools_Template ()
{
  // delete objects in the value list
  HTMLTemplateValueList::iterator  aValueListIterator = m_TemplateValueList.begin();
  while (aValueListIterator != m_TemplateValueList.end()) {
    delete (*aValueListIterator);
    (*aValueListIterator) = NULL;
    ++aValueListIterator;
  } // end while

  // delete objects in the template list
  HTMLTemplateList::iterator  aTemplateListIterator = m_TemplateList.begin();
  while (aTemplateListIterator != m_TemplateList.end()) {
    delete (*aTemplateListIterator);
    (*aTemplateListIterator) = NULL;
    ++aTemplateListIterator;
  } // end while

  // delete the filebuffer
  if (m_pFileBuffer != NULL) {
    delete [] m_pFileBuffer;
    m_pFileBuffer = NULL;
  } // end if
} // end Tools_Template  :: ~Tools_Template

/*
  -----------------------------------------------------------------------------
  public function Tools_Template :: writePage
  -----------------------------------------------------------------------------
 */
void Tools_Template :: writePage ( const Tools_TemplateWriter & aWriter,
                                      bool                      bWithHeader)
{
  if (m_bError) {
    if (bWithHeader) {
      aWriter.sendHeader();
    } // end if

    aWriter.writeBuffer("<html><head><title>Tools_Template - Error</title></head><body>");
    aWriter.writeBuffer(m_strError.StrPtr());
    aWriter.writeBuffer("</body></html>");
  } else {
  
    SAPDB_Int2 nWriteCount = 0;
    SAPDB_Int2 nIndex      = 0;
  
    if ( initTemplate( ) ) {

      if (bWithHeader) {
        aWriter.sendHeader();
      } // end if

      if (m_strName.Empty()) {
        nWriteCount = 1;
      } else {
        nWriteCount = m_pMaster->askForWriteCount(m_strName.StrPtr());
      } // end if

      if (nWriteCount >= 0) {
        for (nIndex = 0; nIndex < nWriteCount; ++nIndex) {
          if (m_TemplateList.empty()) {
            if (!m_bValueList) {
              writeValue(aWriter);
            } else {
              writeValueList(aWriter);
            }
          } else {
            writeTemplates(aWriter);
          } // end if
        } // end for
      } else {
        nIndex = 0;
        while (m_pMaster->askForContinue(m_strName.StrPtr())) {
          if (m_TemplateList.empty()) {
            if (!m_bValueList) {
              writeValue(aWriter);
            } else {
              writeValueList(aWriter);
            }
          } else {
            writeTemplates(aWriter);
          } // end if
          nIndex++;
        } // end while
      } // end if

    } // end if

  } // end if

} // end Tools_Template :: writePage

/*
  -----------------------------------------------------------------------------
  public function Tools_Template :: writeTemplates
  -----------------------------------------------------------------------------
 */
void Tools_Template :: writeTemplates ( const Tools_TemplateWriter & aWriter )
{
  const SAPDB_UTF8 * pCurrent = m_pBegin;

  // iterate the trough the objects list
  HTMLTemplateList::iterator  aListIterator = m_TemplateList.begin();

  while (aListIterator != m_TemplateList.end()) {

    // write before the next template / respectively after the last template
    if (((*aListIterator)->objectBegin() - pCurrent) > 0) {
      aWriter.writeBuffer(pCurrent, (SAPDB_Int4) ((*aListIterator)->objectBegin() - pCurrent));
    } // end if

    // write the template
    (*aListIterator)->writePage(aWriter, false);
    
    pCurrent = (*aListIterator)->objectEnd();

    ++aListIterator;
  } // end while

  // write after the last template
  if ((m_pEnd - pCurrent) > 0) {
    aWriter.writeBuffer(pCurrent, (SAPDB_Int4) (m_pEnd - pCurrent));
  } // end if

} // end Tools_Template :: writeTemplates

/*
  -----------------------------------------------------------------------------
  public function Tools_Template :: writeValue
  -----------------------------------------------------------------------------
 */
void Tools_Template :: writeValue ( const Tools_TemplateWriter & aWriter )
{
  const SAPDB_UTF8 * pCurrent = m_pBegin;

  if (m_pBeginOfValue != NULL) {
    // write before the value
    if ((m_pBeginOfValue - m_pBegin) > 0) {
      aWriter.writeBuffer(m_pBegin, (SAPDB_Int4) (m_pBeginOfValue - m_pBegin));
    } // end if

    // write the value
    aWriter.writeBuffer(m_pMaster->askForValue(m_strName.StrPtr()).StrPtr());

    // write after the value
    if ((m_pEnd - m_pEndOfValue) > 0) {
      aWriter.writeBuffer(m_pEndOfValue, (SAPDB_Int4) (m_pEnd - m_pEndOfValue));
    } // end if
  } else {
    if ((m_pEnd - m_pBegin) > 0) {
      aWriter.writeBuffer(m_pBegin, (SAPDB_Int4) (m_pEnd - m_pBegin));
    } // end if
  } // end if

} // end Tools_Template :: writeValue

/*
  -----------------------------------------------------------------------------
  public function Tools_Template :: writeValueList
  -----------------------------------------------------------------------------
 */
void Tools_Template :: writeValueList ( const Tools_TemplateWriter & aWriter )
{
  const SAPDB_UTF8 * pCurrent = m_pBegin;

  // iterate the trough the value list
  HTMLTemplateValueList::iterator  aListIterator = m_TemplateValueList.begin();

  while (aListIterator != m_TemplateValueList.end()) {

    // write before the next template / respectively after the last template
    if (((*aListIterator)->objectBegin() - pCurrent) > 0) {
      aWriter.writeBuffer(pCurrent, (SAPDB_Int4) ((*aListIterator)->objectBegin() - pCurrent));
    } // end if

    // write the value
    aWriter.writeBuffer(m_pMaster->askForValue((*aListIterator)->objectName()).StrPtr());

    pCurrent = (*aListIterator)->objectEnd();

    ++aListIterator;
  } // end while

  // write after the last value
  if ((m_pEnd - pCurrent) > 0) {
    aWriter.writeBuffer(pCurrent, (SAPDB_Int4) (m_pEnd - pCurrent));
  } // end if

} // end Tools_Template :: writeValueList

/*
  -----------------------------------------------------------------------------
  private function Tools_Template :: readFile
  -----------------------------------------------------------------------------
 */
void Tools_Template :: readFile ( Tools_DynamicUTF8String & szFile )
{
  bool                bState  = false;
  FILE              * hFile;
  struct STAT         fileInfo;
  const SAPDB_Char  * pError = "";

  if (m_pFileBuffer != NULL) {
    delete [] m_pFileBuffer;
    m_pFileBuffer = NULL;
  } // end if

  if (STAT (szFile.CharPtr(), &fileInfo) == 0) {
    // allocate buffer
    m_pFileBuffer = new SAPDB_UTF8[fileInfo.st_size + 1];

    if (m_pFileBuffer != NULL) {
     m_pFileBuffer[0] = '\0';

     // open template file
     hFile = fopen(szFile.CharPtr(), "rb");

      if (hFile != NULL ) {
        // read file
        if (fread(m_pFileBuffer, fileInfo.st_size, 1, hFile) == 1) {
          bState = true;
        } else {
          pError = "fread";
        } // end if

        // close file
        fclose(hFile);
      } else {
        pError = "fopen";
      } // end if

    } // end if

  } else {
    pError = "fstat";
  } // end if

  if (bState) {
    m_pFileBuffer[fileInfo.st_size] = '\0';
    analyzeTemplate(_Tools_UTF8Ptr(""), m_pFileBuffer, &m_pFileBuffer[fileInfo.st_size], SAPDB_FALSE);
  } else {
    if (m_pFileBuffer != NULL) {
      delete [] m_pFileBuffer;
      m_pFileBuffer = NULL;
    } // end if
    m_bError = true;

    m_strError.Assign("Error reading file <b>\"")
              .Append(szFile)
              .Append("\"</b>! (")
              .Append(pError)
              .Append(")");
  } // end if

} // end Tools_Template :: readFile


/*
  -----------------------------------------------------------------------------
  private function Tools_Template :: analyzeTemplate
  -----------------------------------------------------------------------------
 */
void Tools_Template :: analyzeTemplate ( const SAPDB_UTF8 * szName,
                                         const SAPDB_UTF8 * pBegin, 
                                         const SAPDB_UTF8 * pEnd,
                                         const SAPDB_Bool   bValueList )
{

  m_strName.Assign(szName);

  m_pBegin         = pBegin;
  m_pEnd           = pEnd;  
  m_pBeginOfValue  = NULL;
  m_pEndOfValue    = NULL;

  const SAPDB_UTF8        * pBeginOfTemplate = NULL;
  const SAPDB_UTF8        * pEndOfTemplate   = NULL;
  int                       nCounter         = 0;
  Tools_DynamicUTF8String   strTemplate;
  SAPDB_Bool                bTemplValList = SAPDB_FALSE;

  pBeginOfTemplate = findNextTemplate(m_pBegin, m_pEnd, strTemplate, &bTemplValList);

  // read nested objects
  while (pBeginOfTemplate != NULL && pBeginOfTemplate <= m_pEnd) {
    pEndOfTemplate = findEndOfTemplate(pBeginOfTemplate, m_pEnd, strTemplate.StrPtr());
    if (pEndOfTemplate != NULL && pEndOfTemplate  <= m_pEnd) {
      Tools_Template * aNewTemplate = new Tools_Template(m_pMaster, strTemplate.StrPtr(), pBeginOfTemplate, pEndOfTemplate, bTemplValList);

      if (aNewTemplate != NULL) {
        m_TemplateList.push_back(aNewTemplate);
        nCounter++;
      } // end if
    } else {
      m_bError   = true;
      m_strError.Assign("Error end-of-template <b>\"")
                .Append(strTemplate)
                .Append("\"</b> not found!");
    } // end if

    pBeginOfTemplate = findNextTemplate(pEndOfTemplate, m_pEnd, strTemplate, &bTemplValList);
  } // end while

  if (m_TemplateList.empty()) {

    m_bValueList = bValueList;

    if (!bValueList) {
      // look for value
      Tools_DynamicUTF8String strSearch;

      strSearch.Assign(VALUE_MARKER)
               .Append(szName)
               .Append(VALUE_MARKER);

      m_pBeginOfValue = _Tools_UTF8Ptr(strstr(_Tools_CharPtr(pBegin), strSearch.CharPtr()));
      if (m_pBeginOfValue != NULL) {
        m_pEndOfValue = m_pBeginOfValue + strSearch.Length();
      } // end if
    } else {
      // look for values
      const SAPDB_UTF8 * pCurrent = NULL;
      const SAPDB_UTF8 * pBeginOfValue = NULL;
      const SAPDB_UTF8 * pEndOfValue   = NULL;
      Tools_DynamicUTF8String  strValue;

      pCurrent = m_pBegin;
      while ( pCurrent != NULL ) {

        pCurrent = findNextValue(pCurrent, m_pEnd, &pBeginOfValue, &pEndOfValue, strValue);

        if ( pBeginOfValue != NULL && pEndOfValue != NULL ) {
          Tools_TemplateValue * aNewTemplateValue = new Tools_TemplateValue(strValue.StrPtr(), pBeginOfValue, pEndOfValue);

          if (aNewTemplateValue != NULL) {
            m_TemplateValueList.push_back(aNewTemplateValue);
          } // end if
        } // end if
      
      } // end while

    } // end if
    
  } // end if

} // end Tools_Template :: analyzeTemplate

/*
  -----------------------------------------------------------------------------
  private function Tools_Template :: findNextTemplate
  -----------------------------------------------------------------------------
 */
const SAPDB_UTF8 * Tools_Template :: findNextTemplate (const SAPDB_UTF8        * pBuffer,
                                                       const SAPDB_UTF8        * pEnd,
                                                       Tools_DynamicUTF8String & strTemplate,
                                                       SAPDB_Bool              * bTemplValList)
{
   
  const SAPDB_UTF8  *  pReturn       = NULL;
  const SAPDB_UTF8  *  pCurrent      = NULL;
  const SAPDB_UTF8  *  pBeginOfToken = NULL;
  const SAPDB_UTF8  *  pEndOfToken   = NULL;
  Tools_DynamicUTF8String      strToken;

  if (pBuffer != NULL) {

    // look for comment
    pCurrent = _Tools_UTF8Ptr(strstr(_Tools_CharPtr(pBuffer), COMMENT_BEGIN));

    while (pCurrent != NULL && pReturn == NULL && pCurrent <= pEnd) {
      // copy pointer
      pBeginOfToken = pCurrent;
      // skip comment
      pBeginOfToken = pBeginOfToken + strlen(COMMENT_BEGIN);
      // skip whitescpace
      while (isspace(*pBeginOfToken) && *pBeginOfToken != '\0') ++pBeginOfToken;
      // search end of token
      pEndOfToken = pBeginOfToken;
      while (!isspace(*pEndOfToken) && *pEndOfToken != '\0') ++pEndOfToken;
      // extract token
      strToken.Assign(pBeginOfToken, (Tools_DynamicUTF8String::ElementCount) (pEndOfToken - pBeginOfToken));
      // look for keyword
      if (strToken.Compare(TEMPLATE_BEGIN) == 0) {
        pBeginOfToken = pEndOfToken;
        // skip whitescpace
        while (isspace(*pBeginOfToken) && *pBeginOfToken != '\0') ++pBeginOfToken;
        // search end of token
        pEndOfToken = pBeginOfToken;
        while (!isspace(*pEndOfToken) && *pEndOfToken != '\0') ++pEndOfToken;
        // Extract Name of Template
        strTemplate.Assign(pBeginOfToken, (Tools_DynamicUTF8String::ElementCount) (pEndOfToken - pBeginOfToken));
        // value list ?
        if (strTemplate[strTemplate.Length() - 1] == VALLIST_CHAR) {
          *bTemplValList = SAPDB_TRUE;
        } else {
          *bTemplValList = SAPDB_FALSE;
        }
        // find end of comment
        pReturn = _Tools_UTF8Ptr(strstr(_Tools_CharPtr(pEndOfToken), COMMENT_END));
        if (pReturn != NULL) {
          // skip end of comment
          pReturn = pReturn + strlen(COMMENT_END);
        } // end if
      } //  end if

      pCurrent = _Tools_UTF8Ptr(strstr(_Tools_CharPtr(pEndOfToken), COMMENT_BEGIN));
    } // end while

  } // end if

  return pReturn;
} // end Tools_Template :: findNextTemplate

/*
  -----------------------------------------------------------------------------
  private function Tools_Template :: findEndOfTemplate
  -----------------------------------------------------------------------------
 */
const SAPDB_UTF8 * Tools_Template :: findEndOfTemplate ( const SAPDB_UTF8 * pBuffer,
                                                         const SAPDB_UTF8 * pEnd,
                                                         const SAPDB_UTF8 * szTemplate)
{
   
  const SAPDB_UTF8        * pReturn       = NULL;
  const SAPDB_UTF8        * pCurrent      = NULL;
  const SAPDB_UTF8        * pBeginOfToken = NULL;
  const SAPDB_UTF8        * pEndOfToken   = NULL;
  Tools_DynamicUTF8String   strToken;

  if (pBuffer != NULL) {

    // look for comment
    pCurrent = _Tools_UTF8Ptr(strstr(_Tools_CharPtr(pBuffer), COMMENT_BEGIN));

    while (pCurrent != NULL && pReturn == NULL && pCurrent <= pEnd) {
      // copy pointer
      pBeginOfToken = pCurrent;
      // skip comment
      pBeginOfToken = pBeginOfToken + strlen(COMMENT_BEGIN);
      // skip whitescpace
      while (isspace(*pBeginOfToken) && *pBeginOfToken != '\0') ++pBeginOfToken;
      // search end of token
      pEndOfToken = pBeginOfToken;
      while (!isspace(*pEndOfToken) && *pEndOfToken != '\0') ++pEndOfToken;
      // extract token
      strToken.Assign(pBeginOfToken, (Tools_DynamicUTF8String::ElementCount) (pEndOfToken - pBeginOfToken));
      // look for keyword
      if (strToken.Compare(TEMPLATE_END) == 0) {
        pBeginOfToken = pEndOfToken;
        // skip whitescpace
        while (isspace(*pBeginOfToken) && *pBeginOfToken != '\0') ++pBeginOfToken;
        // search end of token
        pEndOfToken = pBeginOfToken;
        while (!isspace(*pEndOfToken) && *pEndOfToken != '\0') ++pEndOfToken;
        // Extract Name of Template
        strToken.Assign(pBeginOfToken, (Tools_DynamicUTF8String::ElementCount) (pEndOfToken - pBeginOfToken));
        if (strToken.Compare(szTemplate) == 0) {
          // copy pointer
          pReturn = pCurrent;
        } // end if
      } //  end if

      pCurrent = _Tools_UTF8Ptr(strstr(_Tools_CharPtr(pEndOfToken), COMMENT_BEGIN));
    } // end if

  } // end if

  return pReturn;
} // end Tools_Template :: findEndOfTemplate

/*
  -----------------------------------------------------------------------------
  private function Tools_Template :: findNextValue
  -----------------------------------------------------------------------------
 */
const SAPDB_UTF8 * Tools_Template :: findNextValue (const SAPDB_UTF8 *        pBuffer,
                                                    const SAPDB_UTF8 *        pEnd,
                                                    const SAPDB_UTF8 **       pBeginOfValue,
                                                    const SAPDB_UTF8 **       pEndOfValue,
                                                    Tools_DynamicUTF8String & strValue)
{
  const SAPDB_UTF8 * pReturn       = NULL;
  const SAPDB_UTF8 * pCurrent      = NULL;
  const SAPDB_UTF8 * pBeginOfToken = NULL;
  const SAPDB_UTF8 * pEndOfToken   = NULL;

  *pBeginOfValue = NULL;
  *pEndOfValue = NULL;

  if (pBuffer != NULL) {

    // look for value begin
    pCurrent = _Tools_UTF8Ptr(strstr(_Tools_CharPtr(pBuffer), VALLIST_VALMARKER_BEGIN));

    if (pCurrent != NULL && pCurrent <= pEnd) {
      // copy pointer
      *pBeginOfValue = pCurrent;

      // skip value marker
      pCurrent = pCurrent + strlen(VALLIST_VALMARKER_BEGIN);

      // copy pointer
      pBeginOfToken = pCurrent;

      if (pCurrent <= pEnd) {

        // look for value end
        pCurrent = _Tools_UTF8Ptr(strstr(_Tools_CharPtr(pCurrent), VALLIST_VALMARKER_END));

        if (pCurrent != NULL && pCurrent <= pEnd) {
          // copy pointer
          pEndOfToken = pCurrent;

          // extract token
          strValue.Assign(pBeginOfToken, (Tools_DynamicUTF8String::ElementCount) (pEndOfToken - pBeginOfToken));
          
          // skip value marker
          pCurrent = pCurrent + strlen(VALLIST_VALMARKER_END);

          // copy pointer
          *pEndOfValue = pCurrent;

          pReturn = pCurrent;
        } // end if
      } // end if
    } // end if
  } // end if

  return pReturn;
} // end Tools_Template :: findNextValue

/*!
  -----------------------------------------------------------------------------
  public function Tools_Template :: ASCIIToUTF8
  -----------------------------------------------------------------------------
 */
Tools_DynamicUTF8String Tools_Template :: ASCIIToUTF8
      ( const char * pSource )
{
  int nSource    = (int) strlen(pSource);
  char * pTarget = new char[nSource * 2];

  Tools_UTF8Basis::ASCIIConstPointer srcBeg  = (Tools_UTF8Basis::ASCIIConstPointer) pSource;
  Tools_UTF8Basis::ASCIIConstPointer srcEnd  = (Tools_UTF8Basis::ASCIIConstPointer) (pSource + nSource);
  Tools_UTF8Basis::ASCIIConstPointer srcAt   = NULL;
  Tools_UTF8Basis::UTF8Pointer      destBeg  = (Tools_UTF8Basis::UTF8Pointer)       pTarget;
  Tools_UTF8Basis::UTF8ConstPointer destEnd  = (Tools_UTF8Basis::UTF8ConstPointer)  (pTarget + (nSource * 2));
  Tools_UTF8Basis::UTF8Pointer      destAt   = NULL;    

  Tools_UTF8Basis::ConvertFromASCII(srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt);

  pTarget[((char *) destAt) - pTarget]     = 0;

  Tools_DynamicUTF8String sReturn = (Tools_DynamicUTF8String::ConstPointer) pTarget;

  delete [] pTarget;

  return sReturn;
} // end Tools_Template :: ASCIIToUTF8
