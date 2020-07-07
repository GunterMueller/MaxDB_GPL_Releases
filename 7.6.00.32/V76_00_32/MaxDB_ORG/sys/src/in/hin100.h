/*! 
  -----------------------------------------------------------------------------
 
  module: hin100.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: log class
 
  description:  specification of a log class

  version:      min 7.2.
 
  Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------


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




  -----------------------------------------------------------------------------
*/
 
#ifndef _HIN100_H_
#define _HIN100_H_

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/

#include "hin105.h"  // tin105_LogFile

/*!
  -----------------------------------------------------------------------------
  Class:        tin100_Log
  -----------------------------------------------------------------------------

  description:  Log Object

                To use a intance of this class to write a log or protocol.
                
                You must set at least one file (via addFile()) to receive the 
                log entries.
                
  -----------------------------------------------------------------------------
*/
class tin100_Log {

 public:
    /*!
      -------------------------------------------------------------------------
      function:     tin100_Log :: tin100_Log
      -------------------------------------------------------------------------
      description:  Constructor

                    This constructor initializes the object and starts the 
                    writer thread.

      arguments:    EvtList [INOUT] - specify this pointer to receive
                                      any problems during contruction. 
      -------------------------------------------------------------------------
   */
   tin100_Log(SAPDBErr_MessageList * EvtList = NULL);

   /*!
      -------------------------------------------------------------------------
      function:     tin100_Log :: ~tin100_Log
      -------------------------------------------------------------------------
      description:  Destructor

                    Cleanup everything. 

      -------------------------------------------------------------------------
   */
   virtual ~tin100_Log();

   /*!
      -------------------------------------------------------------------------
      function:     tin100_Log :: writeEntry
      -------------------------------------------------------------------------
      description:  write one entry

                    The specified EventList will be written to all
                    former added files. 

      arguments:    EventList [IN] - EventList to write
                    szFormat  [IN] - messagetext or formatstring with %s
                    p*        [IN] - %s arguments for the formatstring
      -------------------------------------------------------------------------
   */
   bool writeEntry(const SAPDBErr_MessageList & EventList);
   bool writeEntry(const char             * szFormat,
                   const char             * p0 = NULL,
                   const char             * p1 = NULL,
                   const char             * p2 = NULL,
                   const char             * p3 = NULL,
                   const char             * p4 = NULL,
                   const char             * p5 = NULL,
                   const char             * p6 = NULL,
                   const char             * p7 = NULL,
                   const char             * p8 = NULL,
                   const char             * p9 = NULL);

   
   /*!
      -------------------------------------------------------------------------
      function:     tin100_Log :: addFile
      -------------------------------------------------------------------------
      description:  add one output file

                    Use this function to specify one or more output files.

      arguments:    pLogFile    [IN] - pointer to file object
                    bSelfDetroy [IN] - flag wether or not the file is to 
                                       destruct on destruction of Log object.

      -------------------------------------------------------------------------
   */
   void addFile(tin105_LogFile * pLogFile, const bool bSelfDestroy);

  private:

    // members
    tin105_LogFile * m_pFileAnchor;

    // forbid copy and assignment
    tin100_Log(const tin100_Log& aFile) {}
    tin100_Log& operator = (const tin100_Log& aFile) {return *this;}

}; 

/*! EndClass: tin100_Log */

/*!
  -----------------------------------------------------------------------------
  Class:        tin100_GlobalLog
  -----------------------------------------------------------------------------

  description:  process global log Object

               
  -----------------------------------------------------------------------------
*/
class tin100_GlobalLog : public tin100_Log
{

  public:
    /*!
       -------------------------------------------------------------------------
       function:     tin100_GlobalLog :: createObject
       -------------------------------------------------------------------------
       description:  create object

                     This static function creates a object and save the reference
                     in a static private member. The functions deleteObject and
                     getReference wil use this member. If there already an existing
                     object the function will only return it's reference.

       arguments:    pEvtList [INOUT] - EventList-Object to hold any erros during
                                        object creation (optional)

       return value: reference to global log object

       -------------------------------------------------------------------------
    */
    static tin100_GlobalLog * createObject(SAPDBErr_MessageList * pEvtList = NULL);

   /*!
      -------------------------------------------------------------------------
      function:     tin100_GlobalLog :: deleteObject
      -------------------------------------------------------------------------
      description:  destroy object

                    This static function destroys the object created by createObject 
                    (or implicit created by getReference)
 
      -------------------------------------------------------------------------
    */
    static void               deleteObject();

   /*!
      -------------------------------------------------------------------------
      function:     tin100_GlobalLog :: getReference
      -------------------------------------------------------------------------
      description:  provide object

                    This static function provides a reference to the object
                    created by createObject (or implicit created by a
                    former getReference). Check the reference always against
                    null pointer.
 
      arguments:    bCreateIt [IN] - Flag for creation of object if not exists
                                     (optional)

      return value: reference to global log object
      -------------------------------------------------------------------------
    */
    static tin100_GlobalLog * getReference(const bool bCreateIt = false);

   /*!
      -------------------------------------------------------------------------
      function:     tin100_GlobalLog :: writeEntry
      -------------------------------------------------------------------------
      description:  write one entry

                    The specified EventList will be written to all
                    former added files. 

      arguments:    EventList [IN] - EventList to write
                    szMsg     [IN] - Messagetext

      -------------------------------------------------------------------------
    */
    static bool writeEntry(const SAPDBErr_MessageList & EventList);

  protected:

    static tin100_GlobalLog * m_pMySelf;

    // protected constructor
    tin100_GlobalLog(SAPDBErr_MessageList * pEvtList);
};

/*! EndClass: tin100_GlobalLog */

#endif  // _HIN100_H_
