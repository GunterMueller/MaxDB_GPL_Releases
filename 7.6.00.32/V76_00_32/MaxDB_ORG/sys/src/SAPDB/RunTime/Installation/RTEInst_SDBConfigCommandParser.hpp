/*!
  \file    RTEInst_SDBConfigCommandParser.hpp
  \author  MarcW
  \ingroup RTE Installation
  \brief   parse command line of sdbconfig

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

#if !defined(RTEInst_SDBConfigCommandParser_hpp)
#define RTEInst_SDBConfigCommandParser_hpp

/*!
    @brief parse the command line of sdbconfig in windows
*/
class RTEInst_SDBConfigCommandParser {
public:
    /*!
        @brief create a parser for the parsed arguments

        The command line is parsed in the constructor.

        @param argc [IN] number of arguments as provided to main method of sdbconfig
        @param argv [IN] argument vector as provided to main method of sdbconfig
    */
    RTEInst_SDBConfigCommandParser( int argc, char* argv[] );
    
    /*! @brief delete allocated memory */
    ~RTEInst_SDBConfigCommandParser();

    /*!
        @brief check if the command line made sense to sdbconfig
        @return true if the command line made sense to sdbconfig, false otherwise
    */
    bool parseError() {
        return m_ParseError;
    }

    /*!
        @brief check if all parameters are to be listed
        @return true if all parameters are to be listed, false otherwise
    */
    bool listAll();
    
    /*!
        @brief check if the independent data path is to be set
        @return true if the independent data path is to be set, false otherwise
    */
    bool setIndepDataPath();

    /*!
        @brief check if the independent data path is to be read
        @return true if the independent data path is to be read, false otherwise
    */
    bool getIndepDataPath();

    /*!
        @brief check if the independent programs path is to be set
        @return true if the independent programs path is to be set, false otherwise
    */
    bool setIndepProgramsPath();

    /*!
        @brief check if the independent programs path is to be read
        @return true if the independent programs path is to be read, false otherwise
    */
    bool getIndepProgramsPath();

    /*!
        @brief return the path that was passed on the command line
        @return the path that was passed on the command line, NULL if no path was passed
    */
    const char* getPath();

    /*!
        @brief print a usage string
        @param asErrorResponse [IN] if true, the usage string is printed to STDOUT, otherwise to STDOUT
    */
    void printUsage( bool asErrorResponse );

private:
    enum Command {
        None,
        ListAll,
        SetIndepData,
        GetIndepData,
        SetIndepPrograms,
        GetIndepPrograms
   };

    char* m_CommandLine;
    char* m_Path;
    bool m_ParseError;
    Command m_Command;

    static const size_t m_PROGLEN;
    static const size_t m_DATALEN;
};

#endif //RTEInst_SDBConfigCommandParser_hpp
