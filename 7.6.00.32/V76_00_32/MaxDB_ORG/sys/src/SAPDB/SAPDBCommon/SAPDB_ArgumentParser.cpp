/*!========================================================================

  @file         SAPDB_ArgumentParser.cpp
  @ingroup      
  @author       RaymondR

  @brief        Parse and handle program command line arguments.

                SAPDB_ArgumentParser is an abstract base class which provides a
                functionality to parse and validate command line arguments given by
                the argument array (argv) and the argument count (argc) as passed 
                to main().

                The class parses a list of tokens using a format that specifies 
                expected flags (options) and there arguments. A option is a single
                ASCII letter or a string (long option) starting with a dash and might
                be followed by an argument that may or may not be separated from it
                by one or more tabs or spaces. 

  @since        2004-11-15  11:54
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

#include "SAPDBCommon/SAPDB_ArgumentParser.hpp"

#include <string.h>
#include <stdlib.h>
#include <malloc.h>

/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/

#define  INITIAL_OPT_DESC_BUFFER_SIZE   30

#if defined (_WIN32)
# define RTE_ArgCharSLen   _tcslen
# define RTE_ArgCharSCpy   _tcscpy
# define RTE_ArgCharSnCpy  _tcsncpy

# define RTE_ArgCharCmp    _tcscmp
# define RTE_ArgCharSnCmp  _tcsncmp
# define RTE_ArgCharSiCmp  _tcsicmp
# define RTE_ArgCharSniCmp _tcsnicmp
# define RTE_ArgCharSpn    _tcsspn
# define RTE_ArgCharStoul  _tcstoul
#else
# define RTE_ArgCharSLen   strlen
# define RTE_ArgCharSCpy   strcpy
# define RTE_ArgCharSnCpy  strncpy

# define RTE_ArgCharSnCmp  strncmp
# define RTE_ArgCharSiCmp  strcasecmp
# define RTE_ArgCharSniCmp strncasecmp
# define RTE_ArgCharCmp    strcmp
# define RTE_ArgCharSpn    strspn
# define RTE_ArgCharStoul  strtoul
#endif

#define  INT_OP_VAL_OPTION_FOUND  0x0001


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

SAPDB_ArgumentParser::SAPDB_ArgumentParser( const SAPDB_Int4           argc, 
                                            const RTE_ArgChar* const*  argv,
                                            const SAPDB_Int4           helpOptionID,
                                            const SAPDB_Bool           caseSensitiveLongOptions,
                                            const SAPDB_Bool           allowSlashesAsOptionPrefix,
                                            const SAPDB_Bool           handleMissingNoneOptionalOptions )
                                        : m_ArgIndex (1),
                                          m_Argv(argv),
                                          m_Argc(argc),
                                          m_CaseSensitiveLongOptions(caseSensitiveLongOptions),
                                          m_HelpOptionID(helpOptionID),
                                          m_NumOfOptionDesc(INITIAL_OPT_DESC_BUFFER_SIZE),
                                          m_AllowSlashesAsOptionPrefix(allowSlashesAsOptionPrefix),
                                          m_Options(NULL),
                                          m_HandleMissingNoneOptionalOptions( handleMissingNoneOptionalOptions )
{
}

/*---------------------------------------------------------------------------*/

SAPDB_ArgumentParser::~SAPDB_ArgumentParser()
{ 
    if ( m_Options ) free( m_Options );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SAPDB_ArgumentParser::StartParsing ()
{
    OptionDescription*      currentOption;
    SAPDB_UInt4             argStrPos;
    SAPDB_UInt4             optionLen;
    const RTE_ArgChar*      optionArgument; 
    RTE_ArgChar             shortOption[2];
    OptionError             optError;
    SAPDB_Bool              breakOptionFound;
    SAPDB_Int4              pass;
    SAPDB_Bool              ok   = true;

    if ( m_Options == NULL )
        GetOptionDesc (); // - get option Descriptions

    if ( m_Options == NULL )
        return false;

    for ( pass = 1; (pass <= 2) && ok; pass++ )
    {
        optionLen        = 0;
        breakOptionFound = false;
        optionArgument   = NULL;
        argStrPos        = 1;
        m_ArgIndex       = 1;

        do
        {
            if ( 1 == argStrPos )
            {
                if ( m_ArgIndex >= m_Argc ) //  no arguments remaining ?
                    break; // - leave do/while

#               if defined (_WIN32)
                 SAPDB_Bool optionPrefixFound = ( m_Argv[m_ArgIndex][0] == RTE_ArgCharT('-') ) ||
                                                ( m_Argv[m_ArgIndex][0] == RTE_ArgCharT('/') && m_AllowSlashesAsOptionPrefix );
#               else
                 SAPDB_Bool optionPrefixFound = m_Argv[m_ArgIndex][0] == RTE_ArgCharT('-');
#               endif

                // --- option prefix?
                if ( false == optionPrefixFound )
                {
                    if ( pass > 1 )
                    {
                       if ( false == ArgumentWithoutOption( m_ArgIndex, m_Argv[m_ArgIndex] ))
                           break; // - leave do/while
                    }
                    m_ArgIndex++;
                    continue;
                }

                // --- option character missing?
                if ( RTE_ArgCharT('\0') == m_Argv[m_ArgIndex][1] )
                {
                    HelpOnError( MissingOptionSpecifier, NULL );
                    ok = false;
                    break; // - leave do/while
                }

                // --- break option '--' found?
                if ( RTE_ArgCharCmp(m_Argv[m_ArgIndex], RTE_ArgCharT("--")) == 0 )
                {
                    m_ArgIndex++;
                    argStrPos        = 1;
                    breakOptionFound = true;
                    break; // - leave do/while
                }
            }

            FindOption( &m_Argv[m_ArgIndex][argStrPos], currentOption, optionLen, optError );

            // --- option unknown or ambiguous?
            if ( NoError != optError)
            {
                HelpOnError( optError, &m_Argv[m_ArgIndex][argStrPos] );
                ok = false;
                break; // - leave do/while
            }

            if (( optionLen > 1 ) && ( argStrPos > 1 ))  // long options after option prefix only!
            {
                HelpOnError( UnknownOption, &m_Argv[m_ArgIndex][1] );
                ok = false;
                break; // - leave do/while
            }

            argStrPos += optionLen;

            currentOption->internal |= INT_OP_VAL_OPTION_FOUND;

            if ( currentOption->optionProperties & ( OptionHasAnArgument | OptionArgumentIsOptional))
            {
                //  -- option with argument
                if ( m_Argv[m_ArgIndex][argStrPos] != RTE_ArgCharT('\0') )
                {
                    optionArgument = &m_Argv[m_ArgIndex][argStrPos];
                }
                else if ( m_ArgIndex + 1 >= m_Argc ) // --- option argument missing?
                {
                    m_ArgIndex++;
                    argStrPos = 1;

                    if ( 0 == ( currentOption->optionProperties & OptionArgumentIsOptional ))
                    {   // ---- argument missing because its not optional, display help onformation
                        if ( currentOption->optionID == m_HelpOptionID )
                            Help();
                        else
                        {
                            if ( 1 == optionLen )
                            {
                                shortOption[0] = currentOption->shortOption;
                                shortOption[1] = RTE_ArgCharT('\0');
                                HelpOnError( MissingOptionArgument, shortOption );
                            }
                            else
                                HelpOnError( MissingOptionArgument, currentOption->longOption ); 
                        }
                        ok = false;
                        break; // - leave do/while
                    }
                }
                else
                {
                    m_ArgIndex++;
                    optionArgument = m_Argv[m_ArgIndex];
                }
                m_ArgIndex++;
                argStrPos = 1;
            }
            else //  -- option without argument
            { 
                // --- end of argument reached?
                if ( m_Argv[m_ArgIndex][argStrPos] == RTE_ArgCharT('\0') )
                {
                    argStrPos = 1;
                    m_ArgIndex++;
                }
                else if ( optionLen > 1 ) // no following options after a long option
                {
                    HelpOnError( UnknownOption, &m_Argv[m_ArgIndex][1] );
                    ok = false;
                    break; // - leave do/while
                }
                optionArgument = NULL;
            }

            if ( m_HelpOptionID == currentOption->optionID )
            {
                if ( optionArgument )
                    FindOption( optionArgument, currentOption, optionLen, optError );

                if ( optionArgument && ( NoError == optError ))
                {
                    if ( OptionHelpRequest ( currentOption->optionID ) == false )
                        Help ( optionArgument );
                }
                else
                    Help ( optionArgument );

                ok = false;
                break; // - leave do/while
            }
            else if ( 1 == pass )
                ok = HandleNextOptionPass1( currentOption->optionID, optionArgument );
            else if ( pass > 1 )
                ok = HandleNextOption( currentOption->optionID, optionArgument );
        }
        while ( ok );

        if (( 1 == pass ) && ok )
        {
            ok = Pass1ParseFinished ( &(m_Argv[m_ArgIndex]), ArgsRemaining(), 
                                        m_ArgIndex - 1, breakOptionFound );
        }
    }

    if ( ok )
    {
        OptionDescription*   optionInfo;

        if ( m_HandleMissingNoneOptionalOptions &&
             SearchForMissingNoneOptionalOptions( optionInfo ) )
        {
            HelpOnError( MissingOption, optionInfo->longOption );
            ok = false;
        }
        else
        {
            ok = ParseFinished( &(m_Argv[m_ArgIndex]), ArgsRemaining(), 
                                m_ArgIndex - 1, breakOptionFound );
        }
     }
    return ok;
}


/*===========================================================================*
*  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
*===========================================================================*/

void SAPDB_ArgumentParser::GetOptionDesc ()
{
    SAPDB_Bool          optionAvail;
    OptionDescription*  optionDesc;
    SAPDB_UInt4         curr = 0;

    m_Options = (OptionDescription*)malloc( sizeof(OptionDescription) * m_NumOfOptionDesc );

    if ( 0 != m_Options )
    {
        optionAvail = FirstOptionDesc( m_Options[curr]);

        while (( optionAvail ))
        {
           m_Options[curr].internal = 0;
           curr++;     

            if ( curr >= m_NumOfOptionDesc )
            {
                m_NumOfOptionDesc += INITIAL_OPT_DESC_BUFFER_SIZE;

                optionDesc = (OptionDescription*)malloc( sizeof(OptionDescription) * m_NumOfOptionDesc );

                if ( optionDesc )
                {
                    memcpy ( optionDesc, m_Options, sizeof(OptionDescription) * curr );
                    free ( m_Options );
                    m_Options = optionDesc;
                }
                else
                {
                    free ( m_Options );
                    m_Options = NULL;
                    curr      = 0;
                    break;
                }
            }

            optionAvail = NextOptionDesc( m_Options[curr]);
        } 
    }

    m_NumOfOptionDesc = curr;

    return;
}

/*---------------------------------------------------------------------------*/

void SAPDB_ArgumentParser::FindOption( const RTE_ArgChar*   arg,
                                       OptionDescription*   &optionInfo,
                                       SAPDB_UInt4          &optionLen,
                                       OptionError          &optError )  const
{
    SAPDB_UInt4         curr;
    SAPDB_UInt4         argLen;
    SAPDB_UInt4         minLen;
    SAPDB_Bool          optionFound;
    SAPDB_UInt4         optLen;

    optError    = NoError;
    optionInfo  = NULL;
    optionLen   = 0;
    argLen      = (SAPDB_UInt4)RTE_ArgCharSLen(arg);

    if ( argLen > 1 )
    {
        // --- search argument in long options first
        //
        for ( curr = 0; (optError == NoError) && (curr < m_NumOfOptionDesc); curr++ ) 
        {
            optLen = (SAPDB_UInt4)RTE_ArgCharSLen(m_Options[curr].longOption);

            if ( m_CaseSensitiveLongOptions == true )
                optionFound = !RTE_ArgCharSnCmp(arg, m_Options[curr].longOption, optLen);
            else
                optionFound = !RTE_ArgCharSniCmp(arg, m_Options[curr].longOption, optLen);

            if (( optLen > 0 ) && ( optionFound == true ))
            {
                optionInfo = &m_Options[curr];
                optionLen  = optLen;
                return;
            }
        }
    }

    // --- search argumnent in short options next
    //
    for ( curr = 0; (curr < m_NumOfOptionDesc); curr++ ) 
    {
        if ( m_Options[curr].shortOption == arg[0] )
        {
            if (( optionInfo ) && ( optionInfo != &m_Options[curr] ))
            {
                optError = AmbiguousOption;
                return;
            }
            else if ( 0 == optionInfo )
            {
                optionInfo = &m_Options[curr];
                optionLen  = 1;
            }
        }
    }

    if ( argLen > 1 )
    {
        // --- check if next argument is a short options
        //
        for ( curr = 0; ( optionInfo != NULL) && (curr < m_NumOfOptionDesc); curr++ ) 
        {
            if ( m_Options[curr].shortOption == arg[1] )
                return;
        }

        // --- is argument part of a long option?
        //
        for ( curr = 0; (curr < m_NumOfOptionDesc); curr++ ) 
        {
            optLen = (SAPDB_UInt4)RTE_ArgCharSLen(m_Options[curr].longOption);
            minLen = (optLen < argLen) ? optLen : argLen;  

            if ( m_CaseSensitiveLongOptions )
                optionFound = !RTE_ArgCharSnCmp(arg, m_Options[curr].longOption, minLen);
            else
                optionFound = !RTE_ArgCharSniCmp(arg, m_Options[curr].longOption, minLen);

            if (( optLen > 0 ) && optionFound )
            {
                // --- check: if  
                //      1. the argument was found in short options
                //      2. the argument found in short options is not the same like this
                if (( optionInfo != NULL) && 
                    ( optionInfo != &m_Options[curr] ))
                {
                    optError = AmbiguousOption;
                    return;
                }
                else
                {
                    optionInfo = &m_Options[curr];
                    optionLen  = minLen;
                }
            }
        }
    }

    if ( 0 == optionInfo )
        optError = UnknownOption;

    return;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SAPDB_ArgumentParser::SearchForMissingNoneOptionalOptions (  OptionDescription*   &optionInfo ) const
{
    SAPDB_UInt4         curr;

    // --- search for none optional options that are missing
    for ( curr = 0; (curr < m_NumOfOptionDesc); curr++ ) 
    {
        if ( ( false == ( m_Options[curr].optionProperties & OptionIsOptional )) &&
             ( false == ( m_Options[curr].internal & INT_OP_VAL_OPTION_FOUND )))
        {
            optionInfo = &m_Options[curr];
            return true;
        }
    }
    return false;
}

/*---------------------------------------------------------------------------*/

SAPDB_ArgumentParser::SeparateArgs::SeparateArgs ( const RTE_ArgChar*         argument,
                                                   RTE_ArgChar                SeparatorChar )
                                 : m_ArgSeparatorChar (SeparatorChar)
{
    SAPDB_UInt4     optionArgumentStrLen = (SAPDB_UInt4)RTE_ArgCharSLen(argument);

    m_OptionArgumentStr = (RTE_ArgChar*)malloc( sizeof(RTE_ArgChar) * (optionArgumentStrLen + 1) );
    m_NextArg           = m_OptionArgumentStr;

    if ( m_OptionArgumentStr != NULL )
        RTE_ArgCharSCpy ( m_OptionArgumentStr, argument );
}

/*---------------------------------------------------------------------------*/

SAPDB_ArgumentParser::SeparateArgs::~SeparateArgs() 
{ 
    if (m_OptionArgumentStr) free ( m_OptionArgumentStr ); 
}

/*---------------------------------------------------------------------------*/

const RTE_ArgChar* SAPDB_ArgumentParser::SeparateArgs::NextPart()
{
    RTE_ArgChar*     ThisArg = NULL;
    RTE_ArgChar*     TmpPtr  = NULL;
    SAPDB_UInt4      String  = 0;

    if ( m_OptionArgumentStr && m_NextArg )
    {
        ThisArg  = m_NextArg;

        for ( TmpPtr = m_NextArg; ; TmpPtr++ )
        {
            if ( *TmpPtr == RTE_ArgCharT('"') )
                String ^= 1;
            else if ( *TmpPtr == RTE_ArgCharT('\0') )
            {
                m_NextArg = NULL;
                break;
            }
            else if ( (*TmpPtr == m_ArgSeparatorChar) && (0 == String) )
            {
                *TmpPtr    = RTE_ArgCharT('\0');
                m_NextArg  = TmpPtr + 1;
                break;
            }
        }
    }

    return ThisArg;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  SAPDB_ArgumentParser::ArgumentIsValid ( const RTE_ArgChar*   optionArgument,
                                                  const RTE_ArgChar*     allowedChars )
{
  if ( RTE_ArgCharSpn( optionArgument, allowedChars ) != RTE_ArgCharSLen(optionArgument) )
    return false;
  
  return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  SAPDB_ArgumentParser::ArgumentToUInt ( const RTE_ArgChar*   optionArgument,
                                                 SAPDB_UInt4            &value )
{
    RTE_ArgChar*  endPtr;
    SAPDB_UInt8   tmpVal;         

    tmpVal = RTE_ArgCharStoul( optionArgument, &endPtr, 10);

    if ((( *endPtr == RTE_ArgCharT('\0') ) || ( *endPtr == RTE_ArgCharT(' ') )) && ( tmpVal <= 0x0FFFFFFFFUL ))
    {
        value = (SAPDB_UInt4)tmpVal;
        return true;
    }

  return false;
}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/