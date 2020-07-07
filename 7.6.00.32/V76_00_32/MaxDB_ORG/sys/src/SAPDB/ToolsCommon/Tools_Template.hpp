/*! 
  -----------------------------------------------------------------------------
  module: Tools_Template.hpp
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

#ifndef _Tools_Template_HPP_
#define _Tools_Template_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "hwd02waapi.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ToolsCommon/Tools_Vector.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        Tools_TemplateWriter

  description:  superclass for TemplateWriter classes

                This is a class with one pure virtual function. This function
                will be used by the Tools_Template class for writing the
                page data with Tools_Template::writePage.

                Users of Tools_Template should derive a class from 
                Tools_TemplateWriter an implent the real writing function
  -----------------------------------------------------------------------------
 */
class Tools_TemplateWriter
{
public:
  /*!
    -------------------------------------------------------------------------
    function:     Tools_TemplateWriter
    -------------------------------------------------------------------------
    description:  Constructor
    -------------------------------------------------------------------------
   */
   Tools_TemplateWriter() {}

  /*!
    -------------------------------------------------------------------------
    function:     writeBuffer
    -------------------------------------------------------------------------
    description:  write a buffer to the target
                 
                  This virtual function is used by the Tools_Template class
                  to write the template contents to a target.

    arguments:    pBuffer    [IN]  - the buffer
                  nLength    [IN]  - length to write. You can use length of 0 
                                     for a null terminated string.
    -------------------------------------------------------------------------
   */
   virtual void writeBuffer(const void * pBuffer, SAPDB_Int4 nLength = 0) const = 0;

  /*!
    -------------------------------------------------------------------------
    function:     sendHeader
    -------------------------------------------------------------------------
    description:  send header information to writer object
                 
                  This function is called from Tools_Template::writePage
                  before any data ist written with writeBuffer. In case of
                  writing to a WebAgent reply object you can you use this function
                  for sendig the HTTP header.

    -------------------------------------------------------------------------
   */
   virtual void sendHeader() const = 0;
}; 
/*! EndClass: Tools_TemplateWriter */

/*!
  -----------------------------------------------------------------------------
  Class:        Tools_TemplateWriterWA

  description:  Implementation of a Tools_TemplateWriter 

                This is a class is derived from Tools_TemplateWriter and
                uses the WebAgent Reply object as the target for the
                data written with the writeBuffer function
  -----------------------------------------------------------------------------
 */
class Tools_TemplateWriterWA : public Tools_TemplateWriter
{
public:
  /*!
    -------------------------------------------------------------------------
    function:     Tools_TemplateWriterWA
    -------------------------------------------------------------------------
    description:  Copy Constructor

    arguments:    aWriter [IN] - constant reference to an other writer object
    -------------------------------------------------------------------------
   */
  Tools_TemplateWriterWA(const Tools_TemplateWriterWA & aWriter)
      { m_aReply = aWriter.m_aReply;}
    
  /*!
    -------------------------------------------------------------------------
    function:     Tools_TemplateWriterWA
    -------------------------------------------------------------------------
    description:  Constructor

    arguments:    aReply [IN] - a WebAgent Reply object
    -------------------------------------------------------------------------
   */
  Tools_TemplateWriterWA(sapdbwa_HttpReply & aReply) 
      {m_aReply = &aReply;}

  /*!
    -------------------------------------------------------------------------
    function:     writeBuffer
    -------------------------------------------------------------------------
    description:  write a buffer to the WebAgent Reply object

    arguments:    pBuffer    [IN]  - the buffer
                  nLength    [IN]  - length to write. You can use length of 0 
                                     for a null terminated string.
    -------------------------------------------------------------------------
   */
  virtual void writeBuffer(const void * pBuffer, SAPDB_Int4 nLength) const
      {m_aReply->SendBody(pBuffer, nLength);}

  /*!
    -------------------------------------------------------------------------
    function:     sendHeader
    -------------------------------------------------------------------------
    description:  send http header information

    -------------------------------------------------------------------------
   */

   virtual void sendHeader() const {
     m_aReply->InitHeader(sapdbwa_Status_Ok, "text/html", NULL, NULL, NULL, NULL);
     m_aReply->SetHeader ("Expires", "Sat, 01 Jan 1990 00:00:00 GMT");
     m_aReply->SendHeader();
   } 

private:
  sapdbwa_HttpReply * m_aReply;
}; 

/*! EndClass: Tools_TemplateWriterWA */

/*!
  -----------------------------------------------------------------------------
  Class:        Tools_TemplateValue
  -----------------------------------------------------------------------------

  description:  HTML Template Value 

                An instance of this class represents a template value.
  -----------------------------------------------------------------------------
 */
class Tools_TemplateValue
{
public:

  Tools_TemplateValue ( const SAPDB_UTF8 * szName,
                        const SAPDB_UTF8 * pBegin,
                        const SAPDB_UTF8 * pEnd );

  ~Tools_TemplateValue ( );

  const SAPDB_UTF8 * objectName  ( ) { return m_strName.StrPtr( ); }
  const SAPDB_UTF8 * objectBegin ( ) { return m_pBegin; }
  const SAPDB_UTF8 * objectEnd   ( ) { return m_pEnd; }

private:
  Tools_DynamicUTF8String m_strName;

  const SAPDB_UTF8 * m_pBegin;
  const SAPDB_UTF8 * m_pEnd;
};

/*! EndClass: Tools_TemplateValue */


/*!
  -----------------------------------------------------------------------------
  Class:        Tools_Template
  -----------------------------------------------------------------------------

  description:  HTML Template 

                An instance of this class represents a HTML-Page Template.
                To use the HTML template mechnism you shoudl derive this
                class and overwrite the virtual meber function askForWriteCount
                and askForValue. 
  -----------------------------------------------------------------------------
 */
class Tools_Template
{
public:

/*!  
  Chapter: "normal" member functions
 */

  /*!
    -------------------------------------------------------------------------
    function:     Tools_Template
    -------------------------------------------------------------------------
    description:  Constructor

    arguments:    wa       [IN]  - webagent instance
                  szFile   [IN]  - name of the template file

    -------------------------------------------------------------------------
   */
   Tools_Template (       sapdbwa_WebAgent & wa,
                       const SAPDB_UTF8       * szFile );

  /*!
    -------------------------------------------------------------------------
    function:     Tools_Template
    -------------------------------------------------------------------------
    description:  Constructor

    arguments:    strName    [IN]  - name of the template
                  pBegin     [IN]  - begin of the template in the buffer
                  pEnd       [IN]  - end of the template in the buffer
    -------------------------------------------------------------------------
   */
   Tools_Template (       Tools_Template * pMaster,
                       const SAPDB_UTF8        * szName,
                       const SAPDB_UTF8        * pBegin, 
                       const SAPDB_UTF8        * pEnd,
                       const SAPDB_Bool          bValueList);

  /*!
    -------------------------------------------------------------------------
    function:     ~Tools_Template
    -------------------------------------------------------------------------
    description:  Destructor
    -------------------------------------------------------------------------
   */
   ~Tools_Template ( );

  /*!
    -------------------------------------------------------------------------
    function:     isValid
    -------------------------------------------------------------------------
    description:  check the template template

    return value: true if successfull otherwise false
    -------------------------------------------------------------------------
   */
   bool  isValid ( ) {return (m_pFileBuffer != NULL);}

  /*!
    -------------------------------------------------------------------------
    function:     writePage
    -------------------------------------------------------------------------
    description:  write the page to writer object

    arguments:    aWriter     [INOUT] - a writer object
                  bWithHeader [IN]    - send the HTTP Header
    -------------------------------------------------------------------------
   */
   void writePage ( const Tools_TemplateWriter & aWriter,
                    bool                      bWithHeader = true);


  /*!
    -------------------------------------------------------------------------
    function:     ASCIIToUTF8
    -------------------------------------------------------------------------
    description:  converts ASCII to UTF8

    arguments:    pSource [INOUT] - the ASCII Source

    return value: the UTF8 String
                  
    -------------------------------------------------------------------------
   */
   static Tools_DynamicUTF8String ASCIIToUTF8 ( const char * pSource);

/*!  
  EndChapter: "normal" member functions
 */

private:

/*!  
  Chapter: virtual member functions
 */

  /*!
    -------------------------------------------------------------------------
    function:     askForWriteCount
    -------------------------------------------------------------------------
    description:  virtual function to determine the write count for a template
    
                  This class will use this function to determine how 
                  many times this element should be written to the page.
                  A derived class should overwrite this function to
                  provide correct values.

    arguments:    szName [IN] - the name of the template

    return value: the write count, if you return -1 this class will call
                  the virtual function askForContinue.
    -------------------------------------------------------------------------
   */
  virtual SAPDB_Int2 askForWriteCount ( const Tools_DynamicUTF8String & szName )  
                                      {return 1;}

  /*!
    -------------------------------------------------------------------------
    function:     askForValue
    -------------------------------------------------------------------------
    description:  virtual function to determine the value  for a template
    
                  This class will use this function to determine the 
                  value for a template. A derived class should overwrite 
                  this function to provide correct values.

    arguments:    szName [IN] - the name of the template

    return value: the value for the HTML page
    -------------------------------------------------------------------------
   */
  virtual Tools_DynamicUTF8String askForValue ( const Tools_DynamicUTF8String & szName )  
                              {return "";}

  /*!
    -------------------------------------------------------------------------
    function:     askForContinue
    -------------------------------------------------------------------------
    description:  virtual function to determine to repeat write a template
    
                  This class will use this function to repeat to write
                  a template. 

    arguments:    szName [IN] - the name of the template

    return value: true, the template will write another time, 
                  otherwise false
    -------------------------------------------------------------------------
   */
  virtual bool askForContinue ( const Tools_DynamicUTF8String & szName ) 
                              {return false;}

  /*!
    -------------------------------------------------------------------------
    function:     initTemplate
    -------------------------------------------------------------------------
    description:  virtual function to initialize a template
    -------------------------------------------------------------------------
   */
  virtual bool initTemplate ( ) { return true; }

/*!  
  EndChapter: virtual member functions
 */

  /*
    -------------------------------------------------------------------------
    private member functions
    -------------------------------------------------------------------------
   */
  void                       readFile          ( Tools_DynamicUTF8String & szFile );

  void                       analyzeTemplate   ( const SAPDB_UTF8       * strName,
                                                 const SAPDB_UTF8       * pBegin, 
                                                 const SAPDB_UTF8       * pEnd,
                                                 const SAPDB_Bool         bValueList);

  const SAPDB_UTF8 *         findNextTemplate  ( const SAPDB_UTF8       * pBuffer,
                                                 const SAPDB_UTF8       * pEnd,
                                                 Tools_DynamicUTF8String& strTemplate,
                                                 SAPDB_Bool             * bTemplValList);

  const SAPDB_UTF8 *         findEndOfTemplate ( const SAPDB_UTF8       * pBuffer,
                                                 const SAPDB_UTF8       * pEnd,
                                                 const SAPDB_UTF8       * strTemplate );

  const SAPDB_UTF8 *         findNextValue     ( const SAPDB_UTF8       * pBuffer,
                                                 const SAPDB_UTF8       * pEnd,
                                                 const SAPDB_UTF8       ** pBeginOfValue,
                                                 const SAPDB_UTF8       ** pEndOfValue,
                                                 Tools_DynamicUTF8String& strValue);

  const SAPDB_UTF8 *         objectBegin       (  ) 
                                               {return m_pBegin; }
  
  const SAPDB_UTF8 *         objectEnd         (  ) 
                                               {return m_pEnd;   }
  
  void                       writeTemplates    ( const Tools_TemplateWriter & aWriter );

  void                       writeValue        ( const Tools_TemplateWriter & aWriter );

  void                       writeValueList    ( const Tools_TemplateWriter & aWriter );

  /*
    -------------------------------------------------------------------------
    private member variables
    -------------------------------------------------------------------------
   */
  SAPDB_UTF8              * m_pFileBuffer;
  Tools_DynamicUTF8String   m_strName;
  const SAPDB_UTF8        * m_pBegin;
  const SAPDB_UTF8        * m_pEnd;
  const SAPDB_UTF8        * m_pBeginOfValue;
  const SAPDB_UTF8        * m_pEndOfValue;

  typedef Tools_Vector<Tools_Template *> HTMLTemplateList;
  HTMLTemplateList m_TemplateList;

  typedef Tools_Vector<Tools_TemplateValue *> HTMLTemplateValueList;
  HTMLTemplateValueList m_TemplateValueList;

  SAPDB_Bool m_bValueList;

  Tools_Template  * m_pMaster;

  bool m_bError;
  Tools_DynamicUTF8String m_strError;

}; 

/*! EndClass: Tools_Template */

#endif // _Tools_Template_HPP_
