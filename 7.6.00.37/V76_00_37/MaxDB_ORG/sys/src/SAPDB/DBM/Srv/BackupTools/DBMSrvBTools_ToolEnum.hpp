/*!
  \file    DBMSrvBTools_ToolEnum.hpp
  \author  TiloH
  \ingroup external backup tool handling by the DBMServer
  \brief   declaring an enumeration for directly supported
           external backup tools

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

#if !defined(DBMSrvBTools_ToolEnum_hpp)
#define DBMSrvBTools_ToolEnum_hpp


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include <string.h>


// -----------------------------------------------------------------------------
// class DBMSrvBTools_ToolEnum
// -----------------------------------------------------------------------------

/*! \brief A Class for enumerating external backup tools known by DBMServer.

    \details This class provides an enumeration of supported external backup tools.
    It also supplies the magic strings (ADSM, BACK, NSR, ...) used for marking
    external backup tools in the media defenitions and in the backup history. */

class DBMSrvBTools_ToolEnum
{
  public:
    //! enumeration type for enumerating supported external backup tools
    enum AValue{ADSM=0, //!< used for TSM/ADSM from IBM
                NSR=1,  //!< used for Legato's NetWorker
                BACK=2, //!< used for Backint for SAPDB/MaxDB
                None=3};//!< used for backups without a backup tool
                        // explicit value setting (=0, =1, ...) just to be sure
                        // that None has the highest number 

    /*! constructor
    
       initialising the new object to DBMSrvBTools_ToolEnum::None */
    DBMSrvBTools_ToolEnum();

    /*! copy constructor
    
        new object is a copy of Original
        \param Original [IN] original object that should be copied */
    DBMSrvBTools_ToolEnum(const DBMSrvBTools_ToolEnum & Original);     // not explicit for HP

    /*! constructor

        initialising new object to value TheValue
        \param TheValue [IN] initial value for new object */
    explicit DBMSrvBTools_ToolEnum(const AValue TheValue);

    /*! constructor

        initialising new object from the string ToolString
        \param ToolString [IN] a string starting with one of the magic strings
               representing external backup tools in history and medium files.
               If ToolString does not start with one of the known magic strings,
               the new object gets the value None.*/
    explicit DBMSrvBTools_ToolEnum(const char * ToolString);

    /*! constructor

        initialising new object from the string ToolString by using maximal
        MagicStringLength characters of ToolString (if no terminating zero
        is found before that)
        \param ToolString       [IN] a string starting with one of the magic strings
               representing external backup tools in history and medium files.
               If ToolString does not start with one of the known magic strings,
               the new object gets the value None.
        \param ToolStringLength [IN] Maximal number of characters that are used from ToolString */      
    explicit DBMSrvBTools_ToolEnum(const char * ToolString, size_t ToolStringLength);

    /*! conversion operator
        \return the Value of the object*/
    operator AValue() const;

    /*! test two DBMSrvBTools_ToolEnum's for equality
    
        Two DBMSrvBTools_ToolEnum's are compared based on their AValue's
        \return 1 if both AValue's are equal, 0 otherwise*/
    int operator == (const DBMSrvBTools_ToolEnum &Other) const;

    /*! conversion into a string
    
        \return a string for the object, that can be used for identifying a backup tool */
    const char * AsString() const;

    /*! check if String is identifying a backup tool.

        \return 1, if String is identifying a backup tool or 
        equals the "No-Backup-tool-string", 0 otherwise */
    static int IsAValidToolEnumString(const char * String);

    /*! explicit conversion into an int

        Returns an integer for the object, that can be used for identifying a backup tool.
        \return an integer from 0 to NumberOfBackupToolConnectors()-1*/
    int ToolAsInt() const;

    /*! \return number of known backup tools. */
    static int NumberOfBackupToolConnectors();

  private:
    AValue Value; //!< an enum value for the backup tool represented by the object

    AValue     DetermineToolFromString(const char *String, size_t StringLength) const;
    static int StrNStartsWithPrefix(const char *String, size_t StringLength, const char *Prefix);
};

#endif
