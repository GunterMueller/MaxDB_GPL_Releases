/*!========================================================================

  @file         SAPDB_StandardArgumentParser.cpp
  @ingroup      
  @author       RaymondR

  @brief        Parse and handle program command line arguments.

                SAPDB_StandardArgumentParser is an class which provides a 
                functionality to parse and validate command line arguments 
                given by the argument array (argv) and the argument count (argc)
                as passed to main().

                The class parses a list of tokens using a format that specifies 
                expected flags (options) and there arguments. A option is a single
                ASCII letter or a string (long option) starting with a dash and might
                be followed by an argument that may or may not be separated from it
                by one or more tabs or spaces. 

  @since        2004-11-15  11:56
  @sa           

  ==========================================================================

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_StandardArgumentParser.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined (_WIN32)
# define RTE_ArgCharStPrintf      _stprintf
# define RTE_ArgCharSLen          _tcslen
# define RTE_ArgCharSCpy          _tcscpy
# define RTE_ArgCharSnCpy         _tcsncpy

# define RTE_ArgCharCmp           _tcscmp
# define RTE_ArgCharSnCmp         _tcsncmp
# define RTE_ArgCharSiCmp         _tcsicmp
# define RTE_ArgCharSniCmp        _tcsnicmp
#else
# define RTE_ArgCharStPrintf      sprintf
# define RTE_ArgCharSLen          strlen
# define RTE_ArgCharSCpy          strcpy
# define RTE_ArgCharSnCpy         strncpy

# define RTE_ArgCharSnCmp         strncmp
# define RTE_ArgCharSiCmp         strcasecmp
# define RTE_ArgCharSniCmp        strncasecmp
# define RTE_ArgCharCmp           strcmp
#endif


#define DESC_STR_SUBSTITUTION  "<...>"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

SAPDB_Bool SAPDB_StandardArgumentParser::FirstOptionDesc( OptionDescription   &option )
{
    m_Idx = 0;
    return NextOptionDesc( option );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SAPDB_StandardArgumentParser::NextOptionDesc( OptionDescription   &option )
{
  bool   optionAvail;

  if ( m_Idx < m_NumOfOptionDesc )
  {
    option.optionID           = m_OptionDesc[m_Idx].optionID;
    option.shortOption        = m_OptionDesc[m_Idx].shortOption;
    option.optionProperties   = m_OptionDesc[m_Idx].optionProperties;
    
    RTE_ArgCharSnCpy( option.longOption, m_OptionDesc[m_Idx].longOptionStr, sizeof(option.longOption) - 1 );

    optionAvail = true;
    m_Idx++;
  }
  else
    optionAvail = false;

  return optionAvail;
}

/*---------------------------------------------------------------------------*/

void SAPDB_StandardArgumentParser::Help ( const RTE_ArgChar*         optionArgument )
{ 
# define MAX_PARAM_HELP_STR  15
# define MAX_LONG_OPT_STR    MAX_LONG_OPTION + 1

    SAPDB_UInt4                  curr;
    SAPDB_Bool                   longHelp       = false;
    SAPDB_Int4                   StrPos         = 0;
    RTE_ArgChar                  usageLine[4096];
    RTE_ArgChar                  optionArgDescStr[80];
    SAPDB_Bool                   lineSplitt;
    RTE_ArgChar const *          pOptinalOpen;
    RTE_ArgChar const *          pOptinalClose;
    RTE_ArgChar const *          pOptinalArgOpen;
    RTE_ArgChar const *          pOptinalArgClose;
    RTE_ArgChar *                pOptionStr;
    RTE_ArgChar                  shortOptionStr[2];
    SAPDB_UInt4                  strLen;
    SAPDB_UInt4                  maxOptionStrLen = 1;  // short option has one character
    SAPDB_UInt4                  maxDescStrLen   = 0;
    SAPDB_UInt4                  descStrSubstLen;

    descStrSubstLen =(SAPDB_UInt4)RTE_ArgCharSLen( RTE_ArgCharT(DESC_STR_SUBSTITUTION) );

    // Do we have a long option request?
    if (( optionArgument != NULL ) && (!RTE_ArgCharSiCmp( optionArgument, RTE_ArgCharT("LONG"))))
       longHelp = true;

    //
    // First of all we must calculate the maximum length of some option strings.
    //
    for ( curr = 0; curr < m_NumOfOptionDesc; curr++ ) 
    {
        if (( m_OptionDesc[curr].longOptionStr[0] != RTE_ArgCharT('\0') && longHelp ) ||
            ( m_OptionDesc[curr].shortOption      == RTE_ArgCharT('\0') ))
        {
            strLen          = (SAPDB_UInt4)RTE_ArgCharSLen( m_OptionDesc[curr].longOptionStr );
            maxOptionStrLen =  maxOptionStrLen > strLen ? maxOptionStrLen : strLen;
        }

        if ( m_OptionDesc[curr].optionProperties & ( OptionHasAnArgument | OptionArgumentIsOptional ))
        {
            strLen = (SAPDB_UInt4)RTE_ArgCharSLen( m_OptionDesc[curr].optionArgDescStr );
            if ( MAX_PARAM_HELP_STR < strLen ) strLen = descStrSubstLen;
            maxDescStrLen =  maxDescStrLen > strLen ? maxDescStrLen : strLen;
        }
    }

    if ( maxOptionStrLen > MAX_LONG_OPT_STR ) 
        maxOptionStrLen = MAX_LONG_OPT_STR;


    RTE_ArgCharStPrintf ( usageLine, RTE_ArgCharT("\nUsage:") );

    StrPos = (long)RTE_ArgCharSLen(usageLine);

    for ( curr = 0; curr < m_NumOfOptionDesc; curr++ ) 
    {   
        // We need square brackets around the option, if its optional?
        if ( m_OptionDesc[curr].optionProperties & OptionIsOptional )
        {
            pOptinalOpen  = "[";  
            pOptinalClose = "]";
        }
        else
        {
            pOptinalOpen  = "";
            pOptinalClose = "";
        }

        if ( m_OptionDesc[curr].optionProperties & OptionArgumentIsOptional )
        {
            pOptinalArgOpen  = "[";  
            pOptinalArgClose = "]";
        }
        else
        {
            pOptinalArgOpen  = "";
            pOptinalArgClose = "";
        }

        do
        {
            // any option information ?
            if (( m_OptionDesc[curr].shortOption      == RTE_ArgCharT('\0') ) &&
                ( m_OptionDesc[curr].longOptionStr[0] == RTE_ArgCharT('\0') ))
            {
                // we don't have any option information, display argument only
                RTE_ArgCharStPrintf ( usageLine + StrPos, RTE_ArgCharT(" %s%s%s"),
                                       pOptinalOpen,
                                       m_OptionDesc[curr].optionArgDescStr,
                                       pOptinalClose );
            }
            else
            {   // Display the long or short option?
                if (( m_OptionDesc[curr].longOptionStr[0] != RTE_ArgCharT('\0') && longHelp ) ||
                    ( m_OptionDesc[curr].shortOption      == RTE_ArgCharT('\0') ))
                {   // we have to display the long option
                    pOptionStr = const_cast<RTE_ArgChar*>(m_OptionDesc[curr].longOptionStr);
                }
                else
                {   // the short option is the right choice
                    shortOptionStr[0] = m_OptionDesc[curr].shortOption;
                    shortOptionStr[1] = '\0';
                    pOptionStr        = shortOptionStr;
                }

                if ( m_OptionDesc[curr].optionProperties & ( OptionHasAnArgument | OptionArgumentIsOptional ))
                {   // option has an argument!
                    RTE_ArgCharStPrintf ( usageLine + StrPos, RTE_ArgCharT(" %s-%s %s%s%s%s"),
                                           pOptinalOpen,
                                           pOptionStr,
                                           pOptinalArgOpen,
                                           m_OptionDesc[curr].optionArgDescStr,
                                           pOptinalArgClose,
                                           pOptinalClose );
                }
                else
                {   // option has no argument!
                    RTE_ArgCharStPrintf ( usageLine + StrPos, RTE_ArgCharT(" %s-%s%s"),
                                           pOptinalOpen,
                                           pOptionStr,
                                           pOptinalClose );
                }
            }

            if  ( RTE_ArgCharSLen(usageLine) >= 80 )
            {   // splitt the usage line its to long!
                usageLine[StrPos++] = RTE_ArgCharT('\n');
                usageLine[StrPos]   = RTE_ArgCharT('\0');
                Print ( usageLine );

                RTE_ArgCharStPrintf ( usageLine, RTE_ArgCharT("      ") );
                lineSplitt = true;
            }
            else
                lineSplitt = false;

            StrPos = (long)RTE_ArgCharSLen(usageLine);
        }
        while ( lineSplitt == true );
    }

    StrPos = (long)RTE_ArgCharSLen(usageLine);
    usageLine[StrPos++] = RTE_ArgCharT('\n');
    usageLine[StrPos++] = RTE_ArgCharT('\n');
    usageLine[StrPos]   = RTE_ArgCharT('\0');

    Print ( usageLine );

    for ( curr = 0; curr < m_NumOfOptionDesc; curr++ ) 
    {
        if (( m_OptionDesc[curr].shortOption      == RTE_ArgCharT('\0') ) &&
            ( m_OptionDesc[curr].longOptionStr[0] == RTE_ArgCharT('\0') ))
        {
            // we don't have any option information, display argument only
            RTE_ArgCharSCpy ( optionArgDescStr, m_OptionDesc[curr].optionArgDescStr );
            RTE_ArgCharStPrintf ( usageLine, 
                                    RTE_ArgCharT(" %-*.*s %-*.*s - %s\n"),
                                    maxDescStrLen + 1,
                                    maxDescStrLen + 1,
                                    optionArgDescStr,
                                    maxOptionStrLen + 1,
                                    maxOptionStrLen + 1,
                                    "",
                                    m_OptionDesc[curr].optionHelpStr );
        }
        else 
        {
            if ( 0 == ( m_OptionDesc[curr].optionProperties & ( OptionHasAnArgument | OptionArgumentIsOptional )))
            {   // Option has no argument
                optionArgDescStr[0] = RTE_ArgCharT('\0');
            }
            else if ( MAX_PARAM_HELP_STR < RTE_ArgCharSLen(m_OptionDesc[curr].optionArgDescStr) )
            {    // Shrink argument description string!
                RTE_ArgCharSCpy ( optionArgDescStr, RTE_ArgCharT(DESC_STR_SUBSTITUTION) );
            }
            else
            {   // Display the complete a argument description
                RTE_ArgCharSCpy ( optionArgDescStr, m_OptionDesc[curr].optionArgDescStr );
            }

            if (( m_OptionDesc[curr].longOptionStr[0] != RTE_ArgCharT('\0') && longHelp ) ||
                ( m_OptionDesc[curr].shortOption      == RTE_ArgCharT('\0') ))
            {   // we have to display the long option
                pOptionStr = const_cast<RTE_ArgChar*>(m_OptionDesc[curr].longOptionStr);
            }
            else
            {   // take the short option
                shortOptionStr[0] = m_OptionDesc[curr].shortOption;
                shortOptionStr[1] = '\0';
                pOptionStr        = shortOptionStr;
            }

            RTE_ArgCharStPrintf ( usageLine, 
                                    RTE_ArgCharT(" -%-*.*s %-*.*s - %s\n"),
                                    maxOptionStrLen,
                                    maxOptionStrLen,
                                    pOptionStr,
                                    maxDescStrLen + 1,
                                    maxDescStrLen + 1,
                                    optionArgDescStr,
                                    m_OptionDesc[curr].optionHelpStr );
        }
       Print ( usageLine );
    }
}

/*---------------------------------------------------------------------------*/

void SAPDB_StandardArgumentParser::Print( const RTE_ArgChar*    helpLine )
{ 
        printf ( helpLine );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
