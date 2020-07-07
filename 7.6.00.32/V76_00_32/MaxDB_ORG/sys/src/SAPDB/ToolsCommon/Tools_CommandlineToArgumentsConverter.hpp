/*!
    \file    Tools_CommandlineToArgumentsConverter.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaration of a class for converting a commandline into
             an array of zero terminated C strings

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

#if !defined(Tools_CommandlineToArgumentsConverter_HPP)
#define Tools_CommandlineToArgumentsConverter_HPP

/*! \brief a base class converting a command line into an array of zero terminated
    C strings as known from main() */

class Tools_CommandlineToArgumentsConverter
{
  public:
    /*! \enum EscapingRules an enumeration type for forcing different escaping rules during
        commandline conversion */
    enum EscapingRules{WINDOWS_STYLE, UNIX_STYLE};
    /*! \var Tools_CommandlineToArgumentsConverter::EscapingRules Tools_CommandlineToArgumentsConverter::WINDOWS_STYLE
        Enclosing double quotes can be used for masking spaces. Backslashes can be used to mask double quotes.
        Backslashes mask backslashes only, if the actual sequence of backslashes is followed by a double quote.
        A double quote directly following the second of a pair of space masking double quotes is masked
        itself (I just said how it is.)*/
       
    /*! \var Tools_CommandlineToArgumentsConverter::EscapingRules Tools_CommandlineToArgumentsConverter::UNIX_STYLE
       Enclosing single quotes mask everything (even backslashes in front of single quotes).
       Enclosing double quotes mask everything, but a backslash followed by $, `, ", \ or '\\n' will be used to mask that following symbol.
       A backslash escapes everything (also a following backslash) unless it is enclosed by unmasked single quotes (see above),
       or unless it is enclosed by a unmasked pair of double quotes and non of the special characters (see above) is following. */

    /*! \brief A constructor */
    Tools_CommandlineToArgumentsConverter();

    /*! \brief A destructor, invalidating any pointers obtained via getArgumentArray() */
    ~Tools_CommandlineToArgumentsConverter();

    /*! \brief function for converting a command line into an array of arguments*/
    bool setTo(const char * commandLine, EscapingRules escapingRules=WINDOWS_STYLE);

    /*! \brief function for accessing the result of the conversion*/
    size_t getNumberOfArguments() const {return size;}

    /*! \brief function for accessing the result of the conversion*/
    const char * const * getArgumentArray() const {return values;}

  private:
    bool addValue(const char *newArgument);
    bool increaseCapacity(size_t newCapacity);

    unsigned    size;
    char     ** values;
    size_t capacity;
};

#endif
