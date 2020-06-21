/*!
    @file           DBMSrvShM_InitialisationSynchroniser.hpp
    @author         MarcW
    @brief          sync processes when creating the shared memory

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

#if !defined(DBMSrvShM_InitialisationSynchroniser_HPP)
#define DBMSrvShM_InitialisationSynchroniser_HPP

/*!
    @brief class for synchronising shared memory initalisation
*/
class DBMSrvShM_InitialisationSynchroniser {
public:

    /*!
        @brief synchronize shared memory initalisation
    */
    static bool permissionToInitialize(const char* syncFile);
private:
    DBMSrvShM_InitialisationSynchroniser(){}
};

#endif //DBMSrvShM_InitialisationSynchroniser_HPP
