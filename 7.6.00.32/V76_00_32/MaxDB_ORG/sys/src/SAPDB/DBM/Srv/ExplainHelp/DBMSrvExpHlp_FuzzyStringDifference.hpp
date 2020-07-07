/*!
  \file    DBMSrvExpHlp_FuzzyStringDifference.hpp
  \author  TiloH
  \ingroup Basics for DBMServer commands explain and help
  \brief   declaration of a class for calculating fuzzy differences
           between strings

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

#if !defined(DBMSrvExpHlp_FuzzyStringDifference_hpp)
#define DBMSrvExpHlp_FuzzyStringDifference_hpp


//! \brief a class calculating minimal fuzzy difference between strings
class DBMSrvExpHlp_FuzzyStringDifference
{
  public:
      /*! \brief function for calculating the minimal fuzzy difference of one string
           to all sub strings of another string

           The fuzzy difference between sring s1 and string s2 is hereby the minimal
           number of charcters that must be left out or inserted into s1 such that
           the resulting s1' equals s2. E.g. s1="Hose" has fuzzy difference 3 to string
           s2="Hasen", because 'o' must be left out, 'a' must be inserted instead and 'n'
           has to be appended at the end to transform s1 into s2.
           
           The minimal fuzzy difference to all substrings of a second string can be used,
           if one is looking for the best matching substring.

           The function calculates all possible distributions of s2 into s1,
            via simple cut and branch. All partial distributions, where characters are
            missing from s2, are calculated. The difference is calculated along the
            way and is used to speedup calculation by cutting complete branches of
            distributions.
           \param s1   [IN]  string we are looking for
           \param s2   [IN]  string whose sub strings are investigated
           \difference [OUT] minimal fuzzy difference of s1 to all substrings of s2
           \return true if needed temporary memory could be allocated, false otherwise */
    static bool calculateMinimalDifferenceToAllSubstrings(
        const char * string1,
        const char * string2,
        size_t     & difference);
};

#endif
