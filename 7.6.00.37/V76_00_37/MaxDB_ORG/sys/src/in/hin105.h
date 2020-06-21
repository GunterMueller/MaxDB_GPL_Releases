/*



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
#if defined(_WIN32)
  #line 3 "d:\\v72\\sys\\src\\in\\hin105.h"
#endif 

/*! 
  -----------------------------------------------------------------------------
 
  module: hin105.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: log file class
 
  description:  specification of a log file class

  version:      min 7.2.

  -----------------------------------------------------------------------------
 
                          Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------
*/
 

#ifndef _HIN105_H_
#define _HIN105_H_

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "gsp00.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

/*!  
  -----------------------------------------------------------------------------
  Chapter: Macros and Constants
  -----------------------------------------------------------------------------
 */

/*! Declaration: Event types */

#define LOG_INF_MIN105    1
#define LOG_WRN_MIN105    2
#define LOG_ERR_MIN105    4

#define LOG_ALL_MIN105     LOG_INF_MIN105 + LOG_WRN_MIN105 + LOG_ERR_MIN105

/*! EndDeclaration: Event types */

/*!  
  -----------------------------------------------------------------------------
  EndChapter: Macros and Constants
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  Class:        tin105_LogFile
  -----------------------------------------------------------------------------

  description:  "virtual" Log File Class
   
                This is only a virtual base class. You can never create an 
                instance of this class because there is a pure virtual function.

                This class is the abstract log file used by the log class
                <a href="hin101.html">tin100_Log </a>.

                You must derive an own class for implementing a real log file.

                In a pure virtual member function you will got back the
                SAPDBErr_MessageList object, that you passed to tin100_Log.

  -----------------------------------------------------------------------------
*/
class tin105_LogFile {

  public:
    /*!  
      Chapter: Public functions
     */

    /*!  
      Chapter: Enable/Disable log file
     */

    /*!
      -------------------------------------------------------------------------
      function:     tin105_LogFile :: enable
      -------------------------------------------------------------------------
      description:  enable log file 

      arguments:    bEnable     [IN] - set the state

      -------------------------------------------------------------------------
     */
    void enable     (const bool bEnable = true );

    /*!
      -------------------------------------------------------------------------
      function:     tin105_LogFile :: disable
      -------------------------------------------------------------------------
      description:  disable log file

      -------------------------------------------------------------------------
     */
    void disable     ( );

    /*!
      -------------------------------------------------------------------------
      function:     tin105_LogFile :: isEnabled
      -------------------------------------------------------------------------
      description:  get state of log file 

      return value: the current state
      -------------------------------------------------------------------------
     */
    bool isEnabled     ( ) const;
    
    /*!  
      EndChapter: Enable/Disable log file
     */

    /*!  
      EndChapter: Public functions
     */

  protected:

    /*!  
      Chapter: Protected functions
     */

    /*!
      -------------------------------------------------------------------------
      function:     tin105_LogFile :: tin105_LogFile
      -------------------------------------------------------------------------
      description:  constructor

                    This constructor initializes the object an store the value
                    nEventsToLog in a private member.
                    This only these "Events-To-Log" will passed via the 
                    virtual function writeOneEventList() to derived classes.

      arguments:    nEventsToLog [IN] - eventypes (Info, Warning, Error)
                                        to log
      -------------------------------------------------------------------------
     */
    tin105_LogFile(const int nEventsToLog);

    /*! @brief desctructor, for subclassing */
    virtual ~tin105_LogFile() {};

    // write Events to file
    bool writeEvents(const SAPDBErr_MessageList & EvtLst);
    
    // to maintain list of log files by tin100_Log
    void setNext(tin105_LogFile * pLogFile);

    // to retrieve list of log files by tin100_Log
    tin105_LogFile * next() const;

    // to maintain destruction of log files by tin100_Log
    void setToDestroy(const bool bDestroy = true);

    // to retrieve destruction settings by tin100_Log
    bool isToDestroy() const;
    
    /*!
      -------------------------------------------------------------------------
      function:     tin105_LogFile :: writeOneEventList
      -------------------------------------------------------------------------
      description:  write event list to file

                    This pure virtual function have to overwritten by all
                    derived classes implementing real log files

      arguments:    EventList [IN] - EventList to write


      -------------------------------------------------------------------------
     */
    virtual bool writeOneEventList( const SAPDBErr_MessageList & EvtLst) = 0;

    // forbid copy and assignment
    tin105_LogFile(const tin105_LogFile& aFile) {}
    tin105_LogFile& operator = (const tin105_LogFile& aFile) {return *this;}

    /*!  
      EndChapter: Protected functions
     */

  private: 
    tin105_LogFile * m_pNextFile;
    int              m_nEventsToLog;
    bool             m_bEnabled;
    bool             m_bDestruct;


  friend class tin100_Log;

};

/*! EndClass: tin105_LogFile */

/*!
  -----------------------------------------------------------------------------
  Class:        tin105_SequentialLogFile
  -----------------------------------------------------------------------------

  description:  sequential Log File Class

                This is the first real log file. You can you use this as an example 
                for implementing your own log file. However if you like it feel free
                to use this class.

                This is a sequential file. If this file grows to more then 800 KB
                it will shrinked appr. to the half.

  -----------------------------------------------------------------------------
*/
class tin105_SequentialLogFile :  public tin105_LogFile {

  public:
    // CONSTRUCTOR
    tin105_SequentialLogFile  ( const tsp00_Pathc  szFilename,  
                                SAPDBErr_MessageList * EvtList = NULL );
    tin105_SequentialLogFile  ( const tsp00_Pathc  szFilename, 
                                const int          nEventsToLog, 
                                SAPDBErr_MessageList * EvtList = NULL );
    tin105_SequentialLogFile  ( const char*        szFilename,  
                                SAPDBErr_MessageList * EvtList = NULL );
    tin105_SequentialLogFile  ( const char*        szFilename, 
                                const int          nEventsToLog, 
                                SAPDBErr_MessageList * EvtList = NULL );


    virtual ~tin105_SequentialLogFile() {};

    bool checkFile            ( );

  protected:
    // initialize file
    bool openFile             ( const tsp00_Pathc  szFilename,
                                tsp00_Int4       & hFile ) const;

    void closeFile            ( const tsp00_Int4   hFile ) const ;

    void shrinkFile           ( const tsp00_Pathc  szFilename ) const ;

      // overwritten 
    virtual bool writeOneEventList    ( const SAPDBErr_MessageList & EvtLst);

    // forbid copy and assignment
    tin105_SequentialLogFile(const tin105_SequentialLogFile& aFile) : tin105_LogFile(aFile) {}
    tin105_SequentialLogFile& operator = (const tin105_SequentialLogFile& aFile) {return *this;}

  private:
    tsp00_Pathc           m_szFile;
};

/*! EndClass: tin105_SequentialLogFile */

class tin105_ToShortTextConverter {
public:
    /*! @brief returns a short text for a message type */
    static const char* getShortText( const SAPDBErr_MessageList::MessageType msgType );

    /*! @brief returns lengths of short text without terminator */
    static size_t getShortTextLength();
private:
    tin105_ToShortTextConverter() {}
};

#endif  // _HIN105_H_
