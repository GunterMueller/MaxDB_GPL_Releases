/*!
    \file    ToolsTime_TimeFormatter.hpp
    \author  TiloH
    \ingroup time handling for Tools
    \brief   declaration of a class for formatting time data into strings

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#if !defined TOOLSTIME_TIMEFORMATTER_HPP
#define TOOLSTIME_TIMEFORMATTER_HPP


class ToolsTime_Time;       //forward declaration to avoid include of ToolsTime_Time.hpp
class ToolsTime_TimeFormat; //forward declaration to avoid include of ToolsTime_TimeFormat.hpp


/*! \brief a class for formatting date and time values into strings

    An object of this class can be used to transfom a time value into
    a string, according to a specified format. */
class ToolsTime_TimeFormatter
{
  public:
    /*! \brief calculate the length of a string representing a certain time in a certain format
        
        Use this function, if you would like to allocate memory for a time string dynamic.
        This function is not counting any needed terminating zero.
        \see writeStringIntoBuffer

        \param time   [IN] the date/time
        \param format [IN] the date/time format
        \return length of the data/time string of time in the specified format */
    size_t calculateResultingStringLength(
        const ToolsTime_Time       & time,
        const ToolsTime_TimeFormat & format) const;

    /*! \brief write a time in a specified format into an existing buffer

        Use function calculateResultingStringLength() to calculate the length of the resulting
        string in advance. The resulting string will be zero terminated, if buffer is not 0 and
        usableBufferSpace is at least 1.
        \see calculateResultingStringLength
        \param buffer            [OUT] pointer to the buffer, were result is created in
        \param usableBufferSpace [IN]  the size of the buffer
        \param time              [IN]  the time written into buffer
        \param format            [IN]  the format used to write time into buffer
        \return true, if resulting string and terminating zero fit in the buffer, false otherwise */
    bool writeStringIntoBuffer(
        char                       * buffer,
        size_t                       usableBufferSpace,
        const ToolsTime_Time       & time,
        const ToolsTime_TimeFormat & format) const;

    /*! \brief create a string representing a time in a specified format

        Space for the resulting string is allocated using operator new. If
        not enough memory can be allocated 0 is returned.
        \param time   [IN] the time
        \param format [IN] the time format used
        \return zero terminated time string, if needed memory can be allocated with new(), 0 otherwise */
    char * createString(
        const ToolsTime_Time       & time,
        const ToolsTime_TimeFormat & format) const;

  private:
    /*! \brief copy a string of size stringLength into a buffer of limited size */
    bool copyStringNWithLimit(char * & buffer, size_t & limit, const char * string, size_t stringLength) const;

    /*! \brief copy a certain number of digits of an integer into a buffer of limited size */
    bool copyIntWithLimit(char * & buffer, size_t & limit, int integer, size_t digits) const;
};

#endif
