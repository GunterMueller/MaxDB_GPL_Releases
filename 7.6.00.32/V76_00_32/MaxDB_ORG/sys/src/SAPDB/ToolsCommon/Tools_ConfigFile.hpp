/*!
  -----------------------------------------------------------------------------
  module: Tools_ConfigFile.hpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: config file handling
  description:  some classes to handle config files

  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#ifndef _Tools_ConfigFile_HPP_
#define _Tools_ConfigFile_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdio.h>

#include "Messages/Msg_List.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "ToolsCommon/Tools_Object.hpp"

/*!
----------------------------------------------------------------------------
Class:        Tools_ConfigObject
-----------------------------------------------------------------------------
description:  config object
-----------------------------------------------------------------------------
*/
class Tools_ConfigObject : public Tools_Object {
public:

protected:
  virtual SAPDB_Bool CheckKey
    (const Tools_DynamicUTF8String & sKey,
           Tools_DynamicUTF8String & sEndKey ) = 0;
  virtual SAPDB_Bool AddKey
    (const Tools_DynamicUTF8String & sKey,
     const Tools_DynamicUTF8String & sValue ) = 0;

  friend class Tools_ConfigFile;

}; /*! EndClass: Tools_ConfigObject */

/*!
----------------------------------------------------------------------------
Class:        Tools_ConfigFile
-----------------------------------------------------------------------------
description:  config file object
-----------------------------------------------------------------------------
*/
class Tools_ConfigFile : public Tools_Object {

public:

  /*!
    -------------------------------------------------------------------------
    function:     Tools_ConfigFile
    -------------------------------------------------------------------------
    description:  Constructor
    -------------------------------------------------------------------------
   */
   Tools_ConfigFile
     ( );

  /*!
    -------------------------------------------------------------------------
    function:     Open
    -------------------------------------------------------------------------
    description:  Opens the configuration file

    arguments:    sFile     [IN] - name of the config file

    return value: success (true) or not (false)
    -------------------------------------------------------------------------
   */
   SAPDB_Bool Open
     (  const Tools_DynamicUTF8String & sFile  );

  /*!
    -------------------------------------------------------------------------
    function:     Analyze
    -------------------------------------------------------------------------
    description:  analyzes the configuration file

    return value: success (true) or not (false)
    -------------------------------------------------------------------------
   */
   SAPDB_Bool Analyze
     (  );

  /*!
    -------------------------------------------------------------------------
    function:     Close
    -------------------------------------------------------------------------
    description:  closes the configuration file
    -------------------------------------------------------------------------
   */
   void Close
     (  );

protected:

   virtual Tools_ConfigObject * CreateObject
     ( const Tools_DynamicUTF8String & sSection ) = 0;
   SAPDB_Bool GetNextLine
     ( Tools_DynamicUTF8String & sLine );
   SAPDB_Bool IsSectionBegin
     ( const Tools_DynamicUTF8String & sLine,
             Tools_DynamicUTF8String & sSection );
   void SplitKeyValue
     ( const Tools_DynamicUTF8String & sLine,
             Tools_DynamicUTF8String & sName,
             Tools_DynamicUTF8String & sValue );
   SAPDB_Bool ReadMultiLineValue
     ( const Tools_DynamicUTF8String & sEndKey,
             Tools_DynamicUTF8String & sValue );

   FILE                    * m_hFile;
   int                       m_nLine;

}; /*! EndClass: Tools_ConfigFile */

#endif // _Tools_ConfigFile_HPP_
