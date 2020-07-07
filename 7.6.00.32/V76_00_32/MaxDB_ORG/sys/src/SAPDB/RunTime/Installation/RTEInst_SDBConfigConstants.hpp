/*!
  \file    RTEInst_SDBConfigConstants.hpp
  \author  MarcW
  \ingroup RTE Installation
  \brief   returncodes

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

#if !defined(RTEInst_SDBConfigConstants_hpp)
#define RTEInst_SDBConfigConstants_hpp

/*!
    @brief collection of constants used in the windows version of sdbconfig
*/
class RTEInst_SDBConfigConstants {
public:
    /*!
        @brief a value representing success (zero)
    */
    static const int OK;

    /*!
        @brief a value representing a syntax error
    */
    static const int SYNTAXERROR;

    /*!
        @brief a value representing an internal error
    */
    static const int INTERNALERROR;

    /*!
        @brief a value representing an error in registry access
    */
    static const int WORKERROR;

    /*!
        @brief a value that is never returned
    */
    static const int NEVERRETURNEDRETURNCODE;

    /*!
        @brief registry key for the independent data path
    */
    static const char* const KEY_INDEPDATA;

    /*!
        @brief registry key for the independent programs path
    */
    static const char* const KEY_INDEPPROGRAMS;

    /*!
        @brief command line argument for the user to request help
    */
    static const char* const HELPREQUEST;


private:
    RTEInst_SDBConfigConstants() {}
};

#endif //RTEInst_SDBConfigConstants_hpp
