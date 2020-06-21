/*!
    \file    ToolsTime_TimeFormat.hpp
    \author  TiloH
    \ingroup time handling for Tools
    \brief   declaration of a class for handling time formats

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

#if !defined TOOLSTIME_TIMEFORMAT_HPP
#define TOOLSTIME_TIMEFORMAT_HPP

/*! \brief a class for storing date-time formats

    An object of this class is initialized by a time format string like
    "yyyy.mm.dd HH:MM". The objects stores information about the time format
    string it was initialized with. Such an object can be used to convert
    between time strings and time values represented by class ToolsTime_Time.
    As each object maintains a copy of the format string it is representing,
    the function isAValidObject() should be used to check the validity of the
    object, whenever the represented format string was changed. If the object
    is invalid not enough memory was available.

    \see ToolsTime_Time

    The following time format features are supported:

    1.) The following parts can be part of a time format: year, month, day, hour,
        minute, second, AM/PM

    2.) Character strings not containing any time information can be part of a time
        format. Such strings can be placed in front of the format, inbetween parts
        and at the end of the format.

        \see getNumberOfGaps
        \see getPartPositionFromGap

    3.) Format parts can be in arbitrary order. The function getPartType() can be used to
        determine the order of parts. The function getPartPosition() can be used to
        find a part within the format string that was used to initialize the object.

        \see getPartType
        \see getPartPosition

    4.) Different parts of a format are either separated by separator characters
        (e.g. "yyyy.mm" is separated by '.') or by the length of the parts (e.g.
        "yyyymm", a four digit year is followed by a two digit month). This is
        seen as a property of the left side part (e.g. the year part "yyyy"). The
        function getPartDelimiter() can be used to inquire the right side delimiter
        of a part. If a part has no delimiter getPartDigits() should be used to
        determine the number of digits for that part.

        \see getPartDelimiter
        \see getPartDigits


    Features that are known to be still missing:

    1.) Suppport for formats with non-digit representations of parts other than
        AM/PM, e.g. "2004-Sep-16".

    2.) no support of milliseconds or even smaller timescales, decades and
        bigger timescales are missing too */
class ToolsTime_TimeFormat
{
  public:
    /*! \brief enumeration type for addressing information about
        the different parts of a time format string */
    enum PartType
    {
        Year=0,    //!< for addressing the year
        Month=1,   //!< for addressing the month
        Day=2,     //!< for addressing the day
        Hour=3,    //!< for addressing the hour
        Minute=4,  //!< for addressing the minute
        Second=5,  //!< for addressing the second
        AMPM=6,    //!< for addressing the am/pm part
        FixedEnd=7,//!< for addressing fixed characters following the time string
        Nothing=8  //!< has an internal meaning only
    };

    /*! \brief enumeration type for talking about right side part delimitation */
    enum DelimiterType
    {
        NoDelimiter, //!< a part is not delimited -> only number of digits can be used to find the end of a part
        NonDigit,    //!< the part is delimited to the right by a non digit (e.g. ':' or '-')
        Space        //!< the part is delimited to the right by a space
    };

    /*! \brief default constructor, initialising with the empty format ""

        The default format "" is not useful, so such a time format object should be
        assigned a useful format using operator=(), before the object itself is used.
        Use isAFaultyObject() to check that all the needed memory was allocated.
        \see isAFaultyObject() */
    ToolsTime_TimeFormat();

    /*! \brief constructor, initialising object with a certain time format 

        Use isAFaultyObject() to check that all the needed memory was allocated.

        \param theFormatString [IN] the format string the object is initialised to.

        \see isAFaultyObject() */
    ToolsTime_TimeFormat(const char * theFormatString);

    /*! \brief destructor */
    ~ToolsTime_TimeFormat();

    /*! \brief check for faultlessness of the object

        A faulty object can only be achieved by out of memory situations
        \return true, if object is faulty, false otherwise */
    bool isAFaultyObject() const {return m_IsFaultyObjectFlag;}

    /*! \brief check for invalidity of the time format

         A non valid time format is any format, that can not be handeled correctly by this class.
         \return true, if object represents an invalid format, false otherwise */
    bool isInvalidFormat() const {return !m_IsValidFormatFlag;}


    /*! \brief get position of a part within the original format string
        \param searchedPart [IN] part we are looking for
        \return start position of the searched part within the original format string */
    size_t getPartPosition(PartType searchedPart) const {return (searchedPart<ToolsTime_TimeFormat::Nothing)?m_Position[searchedPart]:0;}

    /*! \brief get number of digits/characters for one part within the original format string
        \param searchedPart [IN] part we are looking for
        \return digits/characters of the searched part within the original format string */
    size_t getPartDigits(PartType searchedPart) const {return (searchedPart<ToolsTime_TimeFormat::Nothing)?m_Digits[searchedPart]:0;}


    /*! \brief get part type of a certain part in the format string
        \param part [IN] number of the part
        \return type of the part */
    PartType getPartType(int part) const {return (part<ToolsTime_TimeFormat::Nothing)?m_Type[part]:ToolsTime_TimeFormat::Nothing;}

    /*! \brief get number of gaps (created by spaces) in front of one part

        The number of gaps is counted from any previous part, not from the start
        of the format string.

        \param part [IN] number of the part
        \return number of space gaps in front of the part */
    int getNumberOfGapsInFrontOf(int part) const {return (part<ToolsTime_TimeFormat::Nothing)?m_GapsInFront[part]:0;}

    /*! \brief get position of a part from the last gap, that is in front of it

        \param part [IN] number of the part
        \return position after the last trailing gap */
    size_t getPartPositionFromGap(int part) const {return (part<ToolsTime_TimeFormat::Nothing)?m_RelativePositionToGap[part]:0;}

    /*! \brief get right side delimiter of a part
        \param part [IN] number of the part
        \return right side delimiter of a part */
    DelimiterType getPartDelimiter(int part) const {return (part<ToolsTime_TimeFormat::Nothing)?m_Delimiter[part]:ToolsTime_TimeFormat::NoDelimiter;}

    /*! \brief set a object to a new time format string
        \param newFormatString [IN] the new time format string for (*this) */
    void operator=(const char * newFormatString);

    /*! \brief get start of basic format section of the format string

        Use function getLengthOfBasicFormatSection (\see getLengthOfBasicFormatSection) to determine
        the end of the basic format section.

        \return start of format section */
    const char * getStartOfFormatSection() const;

    /*! \brief get length of original format section (without "AM=...", "PM=..." etc.) */
    size_t getLengthOfBasicFormatSection() const;

    /*! \brief get maximal possible length of time strings in the format represented by *this
        \return maximal length of time strings in current format */
    size_t getMaximalLengthOfTimeString() const;

    /*! \brief read access to the managed format string
        \return pointer to the objects time format string */
    const char * getFormatString() const;

    /*! \brief enumeration type for AM/PM state in a time string  
        enum DelimiterType */
    enum AMPMType
    {
        NoAMOrPM, //!< no AM or PM found
        AM,       //!< AM found
        PM        //!< PM found
    };

    /*! \brief check whether a string starts with AM or PM or something completely different

        This function uses the AM and PM strings that where configured in the format, if none where
        configured it checks for "AM", "am", "A.M.", "a.m.", "PM", "pm", "P.M." and "p.m.".
        \param string          [IN]  string to check (zero terminated)
        \param charsUsedByAMPM [OUT] number of characters used by found AM/PM
        \return ToolsTime_TimeFormat::NoAMOrPM, if string does not start with AM or PM
                ToolsTime_TimeFormat::AM, if string starts with AM
                ToolsTime_TimeFormat::PM, if string starts with PM */
    AMPMType parseAMPMTypeFrom(const char *string, size_t & charsUsedByAMPM) const;

    /* \brief get read access to AM string of format */
    const char * getAMString() const;

    /* \brief get read access to PM string of format */
    const char * getPMString() const;

  private:
    char * m_FormatString;         //!< a copy of the format string, the object is set to. Something like "SEPARATOR=, , yyyy-mm-dd AM, AM=morgens, PM=abends"

    size_t m_StartOfFormatSection; //!< start of the real format section in the format string (e.g. "yyyy-mmm..."
    size_t m_EndOfFormatSection;   

    size_t        m_Position[ToolsTime_TimeFormat::Nothing];
    size_t        m_Digits[ToolsTime_TimeFormat::Nothing];
    PartType      m_Type[ToolsTime_TimeFormat::Nothing];
    int           m_GapsInFront[ToolsTime_TimeFormat::Nothing];
    size_t        m_RelativePositionToGap[ToolsTime_TimeFormat::Nothing];
    DelimiterType m_Delimiter[ToolsTime_TimeFormat::Nothing];

    char * m_AMString;
    char * m_PMString;

    char m_FormatSectionSeparator;

    bool m_IsFaultyObjectFlag;
    bool m_IsValidFormatFlag;

    static AMPMType startsWithAMPMPlaceHolder(const char *string, size_t & charsUsedByAMPMPlaceHolder);

    enum AnalysisState
    {
        FoundNormalCharacter=0, //!< analyzed character is not a placeholder and no space
        FoundSpace=1,           //!< analyzed character is a space
        FoundPlaceholder=2      //!< analyzed character is part of a placeholder (yyyy, mm, ...)
    };

    void parseForAMPMSection(ToolsTime_TimeFormat::AMPMType    typeDesired,
                             const char                      *  formatString,
                             const char                      *& parsedUpTo,
                             bool                             & parsedSomething,
                             char                            *& newString);
    void parseForSeparatorSection(const char * formatString, const char *& parsedUpTo, bool & parsedSomething);
    void parseForFormatSection(const char * formatString, const char *& parsedUpTo, bool & parsedSomething);

    void adjustEmptyAMPMString(ToolsTime_TimeFormat::AMPMType     typeDesired,
                               char                            * & string);
};

#endif
