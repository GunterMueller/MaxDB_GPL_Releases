/*!
    @file           DBMCliEvtHndl_Base.hpp
    @author         MarcW
    @brief          abstract base class for event handlers - Specification

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

#ifndef DBMCliEvtHndl_Base_HPP
#define DBMCliEvtHndl_Base_HPP

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "Messages/Msg_List.hpp"
#include "ToolsCommon/Tools_ParameterParser.hpp"
#include "SAPDB/DBM/Cli/DBMCli_Database.hpp"
#include "SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp"

/*!
    @brief     Event Handler base class

    Event handlers, that need a connection to the database, should be derived from this
    class. It offers a database object, through which operations in the DB
    can be performed.
    The handlers don't need to care about connect data (but must make sure that these
    are on command line).
    @see DBMCli_Database
 */
class DBMCliEvtHndl_Base
{
public:
    /*!
        @brief Constructor

        In this constructor, the command line is checked for parameters n, d and u
        that are required to create a database object, also a parameter evt containing
        a serialized event is required.
        @see DBMCli_Event
        @param argc         [IN] same as parameter argc of main function
        @param argv         [IN] same as parameter argv of main function
        @param aMessageList [OUT] message list to report errors
    */
    DBMCliEvtHndl_Base(int argc, char* argv[], Msg_List& aMessageList);

    /*! @brief Destructor */
    virtual ~DBMCliEvtHndl_Base();

    /*!
        @brief event handling method

        This method must be implemented by all derived handlers.
    */
    virtual void HandleEvent() = 0;

protected:
    /*! @brief DB connect */
    Tools_ParameterParser* pConnectParameters;
    /*! @brief the received event */
    DBMCliEvtCm_Event* pEvent;
    /*! reference to message list passed in constructor */
    Msg_List& m_oMessageList;
    /*! @brief could we find an event con command line? */
    SAPDB_Bool eventReceived(){ return eventRec; };
    /*! 
        @brief pointer to DB object
        
        Subclasses can use this pointer to connect, they don't need to check connect
        data themselves.
    */
    DBMCli_Database* pDatabase;

private:
    /*! @brief  true if an event was on command line */
    SAPDB_Bool eventRec;
};

#endif //DBMCliEvtHndl_Base_HPP
