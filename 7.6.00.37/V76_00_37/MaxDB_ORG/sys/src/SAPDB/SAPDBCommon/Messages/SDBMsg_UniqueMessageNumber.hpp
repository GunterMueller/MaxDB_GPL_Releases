/*!
    \file    SDBMsg_UniqueMessageNumber.hpp
    \author  MarcW
    \ingroup SAPDB common, messages
    \brief   enumeration of unique message numbers

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


*/

#if !defined(SDBMsg_UniqueMessageNumber_hpp)
#define SDBMsg_UniqueMessageNumber_hpp

/*!
	@brief enumeration of all system and error messages as described in the menual

	The content of this class should be generated (from the manual). For each number range
    that is listed in the manual, one enumeration should be declared.

    No instance of this class can be created.
*/
class SDBMsg_UniqueMessageNumber {
public:

	/*!
		@brief error ids of the DBMServer
	*/
	enum DatabaseManagerError {
		ERR                 = -24999,   /*!< common error */
		ERR_NOTIMPL,                    /*!< sorry, command not yet implemented */
		ERR_MEM             = -24995,   /*!< memory allocation error */
        ERR_RTE             = -24994,   /*!< runtime environment error */
        ERR_DBNORUN         = -24936,   /*!< database is not running */
        ERR_SHUTDOWN        = -24895,   /*!< shutdown of database occured */
        ERR_EVENTNONEAVAIL  = -24866,   /*!< no event available */
        ERR_SESSIONLIMIT    = -24755    /*!< session limit reached */
	};

    /*!
        @brief Error messages of the ODBC program
    */
    enum ODBCError {
        NULL_ON_LONG        = -22000,   /*!< Null values not allowed on more than one long column */
        CONN_TIMEOUT        = -22005    /*!< Connection timeout occured. Session reconnected */
    };

private:
	SDBMsg_UniqueMessageNumber() {}
};

#endif //SDBMsg_UniqueMessageNumber_hpp
