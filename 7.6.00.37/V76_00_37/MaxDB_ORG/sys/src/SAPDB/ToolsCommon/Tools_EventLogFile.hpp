/*
    @file           Tools_EventLogFile.hpp
    @author         MarcW
    @brief          logging into Windows Event Log - Specification

\if EMIT_LICENCE

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


\endif
*/
#ifndef Tools_EventLogFile_HPP
#define Tools_EventLogFile_HPP

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include <windows.h>

#include "hin105.h"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

/*!
    @brief     Logger for logging in to Windows Event Log

    This class can be used standalone for making entries in
    the windows event log and also together with the tools
    global log class tin100_GlobalLog.
    This class is only useful in Microsoft Windows environment.
    Windows requires a resource library, that contains the message
    text templates, for us it's Tools_WinEvtLog.dll.
    The path to this library must be given in the constructors.
*/
class Tools_EventLogFile :  public tin105_LogFile {

    public:

        /*!
            @brief type of entry

            Type of entry to be added to the windows event log
        */
        typedef enum {
            ErrorEntry,   /*!< error entry */
            InfoEntry,    /*!< information entry */
            WarningEntry  /*!< warning entry */
        } EntryType;

        /*!
            @brief Constructor

            When using this constructor, messages of all 3 types are
            written to the Windows Event Log.

            @param aHeaderString1 [IN] first line of event text in
                event viewer
            @param aHeaderString2 [IN] first line of event text in
                event viewer (after this line, there is a blank line
                in event viewer). The real informative event text follows
                after the blank line.
            @param aResourceLibrary [IN] path to and including the name of
                the library (DLL), that contains the message IDs. See
                descrption of this class and Microsoft's description for
                making event log entries. If an empty string is passed,
                no registry entry is made.
            @param aSourceName [IN] string to appear in source column of
                event viewer
            @param EvtList [IN] list of events to be written to event log
                immediately after successful creation of the object.
            @see checkFile()
        */
        Tools_EventLogFile( const Tools_DynamicUTF8String& aHeaderString1,
                            const Tools_DynamicUTF8String& aHeaderString2,
                            const Tools_DynamicUTF8String& aSourceName,
                            const Tools_DynamicUTF8String& aResourceLibrary = "",
                            Msg_List* EvtList = NULL );
        /*!
            @brief Constructor

            @param aHeaderString1 [IN] first line of event text in
                event viewer
            @param aHeaderString2 [IN] first line of event text in
                event viewer (after this line, there is a blank line
                in event viewer). The real informative event text follows
                after the blank line.
            @param aResourceLibrary [IN] path to and including the name of
                the library (DLL), that contains the message IDs. See
                descrption of this class and Microsoft's description for
                making event log entries.
            @param aSourceName [IN] string to appear in source column of
                event viewer
            @param nEventsToLog [IN] types of events to log (possible
                values: LOG_INF_MIN105, LOG_WRN_MIN105, LOG_ERR_MIN105,
                LOG_ALL_MIN105 (see hin105.h)). Defaults to LOG_ALL_MIN105.
            @param EvtList [IN] list of events to be written to event log
                immediately after successful creation of the object.
            @see checkFile()
        */
        Tools_EventLogFile( const int nEventsToLog, 
                            const Tools_DynamicUTF8String& aHeaderString1,
                            const Tools_DynamicUTF8String& aHeaderString2,
                            const Tools_DynamicUTF8String& aSourceName,
                            const Tools_DynamicUTF8String& aResourceLibrary = "", 
                            Msg_List* EvtList = NULL );

        /*!
            @brief Constructor

            Just for making necessary registry entry.
            @param aResourceLibrary [IN] path to and including the name of
                the library (DLL), that contains the message IDs. See
                descrption of this class and Microsoft's description for
                making event log entries.
            @param aSourceName [IN] string to appear in source column of
                event viewer
        */
        Tools_EventLogFile( const Tools_DynamicUTF8String& aSourceName,
                            const Tools_DynamicUTF8String& aResourceLibrary ); 

        /*! @brief Destructor */
        ~Tools_EventLogFile();

        /*!
            @brief check status

            After creation of the object, caller should call this method
            for checking, if logging is possible.
            @return true if logging is possible, false otherwise
        */
        bool checkFile() {return bConnected;};

        /*!
            @brief write a simple text to Event Log

            The text will follow after the 2 header lines and the blank line.
            @param text [IN] text to be written
            @param entryType [IN] type of message
            @return true if text could be written
        */
        bool writeRawText(const Tools_DynamicUTF8String& text, EntryType entryType = WarningEntry);

    protected:

        /*!
            @brief write event list to event log
            
            Writes event list to event log if possible.
            @param EvtLst [IN] event list to be written
            @return true if list could be written
        */
        bool writeOneEventList( const Msg_List& EvtLst );

        /*! @brief forbid copy */
        Tools_EventLogFile(const Tools_EventLogFile& aFile) : tin105_LogFile(aFile) {}
        
        /*! @brief forbid assignment */
        Tools_EventLogFile& operator= (const Tools_EventLogFile& aFile) {return *this;}

    private:
        /*!
            @brief makes registry entries

            Registers the resource library. See Microsoft's descrption for making
            event log entries. Sets status returned by checkFile().
            @param aResourceLibrary [IN] path to and including the library
            @see checkFile()
        */
        bool modifyRegistry( const Tools_DynamicUTF8String& aResourceLibrary);

        /*! @brief path to resource dll */
        Tools_DynamicUTF8String sSourceName;
        /*! @brief path in registry where to make entries */
        Tools_DynamicUTF8String sRegistryPath;
        /*! @brief status if entries can be made */
        bool                    bConnected;
        /*! @brief handle to event log */
        HANDLE                  hEventLog;

        /*! @brief header text1 */
        Tools_DynamicUTF8String sHeaderText1;
        /*! @brief header text2 */
        Tools_DynamicUTF8String sHeaderText2;

        /*! @brief location for registry entries */
        static const Tools_DynamicUTF8String sRegistryRoot;
};

#endif  // Tools_EventLogFile_HPP
