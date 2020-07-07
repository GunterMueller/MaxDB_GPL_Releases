/*!
  \file    DBMSrvProc_ServerProcessCommandLineConverter.hpp
  \author  MarcW
  \brief   convert enum to program name

\if EMIT_LICENCE

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


\endif
*/

#if !defined(DBMSrvProc_ServerProcessCommandLineConverter_hpp)
#define DBMSrvProc_ServerProcessCommandLineConverter_hpp

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/*!
    @brief maps numbers to program names

    This class is used as a security means. When creating a server process through 
    a DBM server command, not the executable name is passed, but an internal number.
    The mapping between this number and the executable name is implemented in this class.
*/
class DBMSrvProc_ServerProcessCommandLineConverter {
public:
    /*!
        @brief internal numbers for executables

        Each executable has its unique number
    */
    enum ProgramKey {
        KeySchedulerProgram = 1,      /*!< DBM command scheduler, "dbmsrvscd" */
        KeyEventDispatcherProgram,    /*!< DBM event dispatcher, "dbmevtdisp" */
        KeyDbanalyzer,                /*!< DBAnalyzer,  "dbanalyzer"          */
        KeyDbmcli,                    /*!< DBM Command Line Interface, "dbmcli" */
        KeyInvalidEntry               /*!< always last to check invalid keys    */
    };

    /*!
        @brief constructor

        @param [IN] aCommandLine commandline containing the executable's ProgramKey
        @see ProgramKey
    */
    DBMSrvProc_ServerProcessCommandLineConverter( const char* aCommandLine );

    /*!
        @brief destructor
    */
    ~DBMSrvProc_ServerProcessCommandLineConverter();

    /*!
        @brief get the converted command line

        @return reference to the converted command line. Memory is managed internally.
    */
    const char* getConvertedCommandLineReference() const {return (const char*)m_ConvertedCommandLine;}

    /*!
        @brief check whether a known ProgramKey was passed in constructor

        @return true if a known ProgramKey was passed in constructor, false otherwise
    */
    bool isValid() const {return m_IsValid;}

    /*!
        @brief check if desired program is to be called with pipe call class

        @see Tools_PipeCall
    */
    bool isPipeCall() const {
        // if there are more, add their indexes with OR in the brackets
        return m_IsValid && m_Programs[m_ProgramIndex].bPipe;
    }

    /*! memory is allocated if returncode is true, must be deleted by caller */
    static bool removePasswordFromCommandLine( const char* lineIn, char*& lineOut, DBMSrvMsg_Error& errOut );

    /*!
        @brief internal numbers for directories of executables

        Each directory has its unique number
    */
    enum DirectoryKey {
      DirNothing,
      DirDepBin,
      DirDepPgm,
      DirIndepBin,
      DirIndepPgm,
      DirUnknown
    };

private:
    bool m_IsValid;
    int m_ProgramIndex;
    char* m_ConvertedCommandLine;

    typedef struct Programs {
      ProgramKey    nKey;
      bool          bPipe;
      DirectoryKey  nDir;
      const char *  szProgram;

    } Programs ;
    static Programs  m_Programs[];

};

#endif //DBMSrvProc_ServerProcessCommandLineConverter_hpp
