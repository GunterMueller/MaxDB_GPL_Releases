/*!
  @file           veo80.cpp
  @author         RaymondR
  @special area   get options (getopt)
  @brief          
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo80.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	inline
#endif



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


teo80_GetOpt::teo80_GetOpt( const int             Argc, 
                            const _TCHAR* const*  Argv,
                            const int             HelpOptionID,
                            const bool            CaseSensitiveLongOptions )
             : ArgIndex_eo80 (1),
               Argv_eo80(Argv),
               Argc_eo80(Argc),
               CaseSensitiveLongOptions_eo80(CaseSensitiveLongOptions),
               HelpOptionID_eo80(HelpOptionID),
               NumOfOptionDesc_eo80(0),
               Options_eo80(NULL)
{
  ROUTINE_DBG_MEO60 (_T("teo80_GetOpt::teo80_GetOpt"));
}

/*---------------------------------------------------------------------------*/


bool teo80_GetOpt::eo80_ParseArguments ()
{
  ROUTINE_DBG_MEO60 (_T("teo80_GetOpt::eo80_ParseArguments"));

  teo80_OptionDesc*       CurrentOption;
  unsigned long           ArgStrPos;
  unsigned long           Optionlen;
  const _TCHAR*           OptionArgument; 
  _TCHAR                  ShortOption[2];
  teo80_OptError          OptError;
  bool                    BreakOptionFound;
  int                     Pass;
  bool                    Ok   = true;

  if ( Options_eo80 == NULL )
    eo80_GetOptionDesc (); // - get option Descriptions

  for ( Pass = 1; (Pass <= 2) && Ok; Pass++ )
  {
    Optionlen        = 0;
    BreakOptionFound = false;
    OptionArgument   = NULL;
    ArgStrPos        = 1;
    ArgIndex_eo80    = 1;
    
    do
    {
      if ( ArgStrPos == 1 )
      {
        if ( ArgIndex_eo80 >= Argc_eo80 ) //  no arguments remaining ?
          break; // - leave do/while
        
        // --- option prefix or option character missing?
        if (( Argv_eo80[ArgIndex_eo80][0] != _T('-') ) && ( Argv_eo80[ArgIndex_eo80][0] != _T('/') )) 
          break; // - leave do/while
        
        if ( Argv_eo80[ArgIndex_eo80][1] == _T('\0') )
        {
          eo80_vHelpOnError( MissingOptionSpecifier_eeo80, NULL );
          Ok = false;
          break; // - leave do/while
        }
        
        // --- break option '--' found?
        if ( _tcscmp(Argv_eo80[ArgIndex_eo80], _T("--")) == 0 )
        {
          ArgIndex_eo80++;
          ArgStrPos        = 1;
          BreakOptionFound = true;
          break; // - leave do/while
        }
      }
      
      eo80_FindOption( &Argv_eo80[ArgIndex_eo80][ArgStrPos],
                       CurrentOption, Optionlen, OptError );
      
      // --- option unknown or ambiguous?
      if ( OptError != NoError_eeo80 )
      {
        eo80_vHelpOnError( OptError, &Argv_eo80[ArgIndex_eo80][ArgStrPos] );
        Ok = false;
        break; // - leave do/while
      }
      
      if (( Optionlen > 1 ) && ( ArgStrPos > 1 ))  // long options after option prefix only!
      {
        eo80_vHelpOnError( UnknownOption_eeo80, &Argv_eo80[ArgIndex_eo80][1] );
        Ok = false;
        break; // - leave do/while
      }
      
      ArgStrPos += Optionlen;
      
      if ( CurrentOption->OptionWithArgument_eo80 == true )
      {
        //  -- option with argument
        if ( Argv_eo80[ArgIndex_eo80][ArgStrPos] != _T('\0') )
        {
          OptionArgument = &Argv_eo80[ArgIndex_eo80][ArgStrPos];
        }
        else if ( ArgIndex_eo80 + 1 >= Argc_eo80 ) // --- option argument missing?
        {
          ArgIndex_eo80++;
          ArgStrPos = 1;
          
          if ( CurrentOption->OptionID_eo80 == HelpOptionID_eo80 )
            eo80_vHelp ( NULL );
          else
          {
            if ( Optionlen == 1 )
            {
              ShortOption[0] = CurrentOption->ShortOption_eo80;
              ShortOption[1] = _T('\0');
              eo80_vHelpOnError( MissingOptionArgument_eeo80, ShortOption );
            }
            else
              eo80_vHelpOnError( MissingOptionArgument_eeo80, CurrentOption->LongOption_eo80 ); 
          }
          Ok = false;
          break; // - leave do/while
        }
        else
        {
          ArgIndex_eo80++;
          OptionArgument = Argv_eo80[ArgIndex_eo80];
        }
        ArgIndex_eo80++;
        ArgStrPos = 1;
      }
      else //  -- option without argument
      { 
        // --- end of argument reached?
        if ( Argv_eo80[ArgIndex_eo80][ArgStrPos] == _T('\0') )
        {
          ArgStrPos = 1;
          ArgIndex_eo80++;
        }
        else if ( Optionlen > 1 ) // no following options after a long option
        {
          eo80_vHelpOnError( UnknownOption_eeo80, &Argv_eo80[ArgIndex_eo80][1] );
          Ok = false;
          break; // - leave do/while
        }
        OptionArgument = NULL;
      }
      
      if ( HelpOptionID_eo80 == CurrentOption->OptionID_eo80 )
      {
        if ( OptionArgument != NULL )
          eo80_FindOption( OptionArgument, CurrentOption, Optionlen, OptError );
        
        if (( OptionArgument != NULL ) && ( OptError == NoError_eeo80 ))
        {
          if ( eo80_vOptionHelpRequest ( CurrentOption->OptionID_eo80 ) == false )
            eo80_vHelp ( OptionArgument );
        }
        else
          eo80_vHelp ( OptionArgument );
        
        Ok = false;
        break; // - leave do/while
      }
      else if ( Pass == 1 )
        Ok = eo80_vNextOptionPass1( CurrentOption->OptionID_eo80, OptionArgument );
      else if ( Pass > 1 )
        Ok = eo80_vNextValidOption( CurrentOption->OptionID_eo80, OptionArgument );
    }
    while ( Ok );

    if (( Pass == 1 ) && Ok )
    {
      Ok = eo80_vPass1Finished ( &(Argv_eo80[ArgIndex_eo80]), eo80_ArgsRemaining(), 
                                 ArgIndex_eo80 - 1, BreakOptionFound );
    }
  }

  if ( Ok  )
  {
    Ok = eo80_vParseArgumentsFinished( &(Argv_eo80[ArgIndex_eo80]), eo80_ArgsRemaining(), 
                                       ArgIndex_eo80 - 1, BreakOptionFound );
  }
  return Ok;
}


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

void teo80_GetOpt::eo80_GetOptionDesc ()
{
  ROUTINE_DBG_MEO60 (_T("teo80_GetOpt::eo80_GetOptionDesc"));

# define DEFAULT_OPT_DESC_BUFFER_SIZE       30

  bool                NextOptionAvail;
  teo80_OptionDesc*   OptionDesc;
  unsigned long       Curr = 0;

  NumOfOptionDesc_eo80 = DEFAULT_OPT_DESC_BUFFER_SIZE;

  Options_eo80 = new teo80_OptionDesc[NumOfOptionDesc_eo80];

  if ( Options_eo80 != NULL )
  {
    do
    {
      NextOptionAvail = eo80_vNextOptionDesc( Curr, Options_eo80[Curr]);
      
      if ( NextOptionAvail == true )
        Curr++;     
      
      if (( NextOptionAvail == false ) || ( Curr == NumOfOptionDesc_eo80 ))
      {
        if (( Curr == NumOfOptionDesc_eo80 ) && ( NextOptionAvail == true ))
        {
          NumOfOptionDesc_eo80 += DEFAULT_OPT_DESC_BUFFER_SIZE;
        }
        else if ( NextOptionAvail == false )
        {
          NumOfOptionDesc_eo80 = Curr;
        }

        OptionDesc = new teo80_OptionDesc[NumOfOptionDesc_eo80];
        
        if ( Options_eo80 )
        {
          SAPDB_MemCopyNoCheck ( OptionDesc, Options_eo80, sizeof(teo80_OptionDesc) * Curr );
          delete Options_eo80;
          Options_eo80 = OptionDesc;
        }
        else
        {
          delete Options_eo80;
          Options_eo80 = NULL;
          break;
        }
      }
    } 
    while (( NextOptionAvail == true ));
  }
    
  return;
}

/*---------------------------------------------------------------------------*/

void teo80_GetOpt::eo80_FindOption( const _TCHAR*      Arg,
                                    teo80_OptionDesc*  &OptionInfo,
                                    unsigned long      &Optionlen,
                                    teo80_OptError     &OptError )  const
{
  ROUTINE_DBG_MEO60 (_T("teo80_GetOpt::eo80_FindOption"));
  unsigned long       Curr;
  unsigned long       ArgLen;
  unsigned long       OptionLen;
  unsigned long       MinLen;
  bool                OptionFound;

  OptError    = NoError_eeo80;
  OptionInfo  = NULL;
  Optionlen   = 0;
  ArgLen      = (unsigned long)_tcslen(Arg);

  if ( ArgLen > 1 )
  {
    // --- search argument in long options first
    //
    for ( Curr = 0; (OptError == NoError_eeo80) && (Curr < NumOfOptionDesc_eo80); Curr++ ) 
    {
      OptionLen = (unsigned long)_tcslen(Options_eo80[Curr].LongOption_eo80);

      if ( CaseSensitiveLongOptions_eo80 == true )
        OptionFound = !_tcsncmp(Arg, Options_eo80[Curr].LongOption_eo80, OptionLen);
      else
        OptionFound = !_tcsnicmp(Arg, Options_eo80[Curr].LongOption_eo80, OptionLen);

      if (( OptionLen > 0 ) && ( OptionFound == true ))
      {
        OptionInfo = &Options_eo80[Curr];
        Optionlen  = OptionLen;
        return;
      }
    }
  }

  // --- search argumnent in short options next
  //
  for ( Curr = 0; (Curr < NumOfOptionDesc_eo80); Curr++ ) 
  {
    if ( Options_eo80[Curr].ShortOption_eo80 == Arg[0] )
    {
      if (( OptionInfo != NULL ) && 
          ( OptionInfo != &Options_eo80[Curr] ))
      {
        OptError = AmbiguousOption_eeo80;
        return;
      }
      else if ( OptionInfo == NULL )
      {
        OptionInfo = &Options_eo80[Curr];
        Optionlen  = 1;
      }
    }
  }

  if ( ArgLen > 1 )
  {
    // --- check if next argument is a short options
    //
    for ( Curr = 0; ( OptionInfo != NULL) && (Curr < NumOfOptionDesc_eo80); Curr++ ) 
    {
      if ( Options_eo80[Curr].ShortOption_eo80 == Arg[1] )
        return;
    }

    // --- is argument part of a long option?
    //
    for ( Curr = 0; (Curr < NumOfOptionDesc_eo80); Curr++ ) 
    {
      OptionLen = (unsigned long)_tcslen(Options_eo80[Curr].LongOption_eo80);
      MinLen    = (OptionLen < ArgLen) ? OptionLen : ArgLen;  

      if ( CaseSensitiveLongOptions_eo80 == true )
        OptionFound = !_tcsncmp(Arg, Options_eo80[Curr].LongOption_eo80, MinLen);
      else
        OptionFound = !_tcsnicmp(Arg, Options_eo80[Curr].LongOption_eo80, MinLen);

      if (( OptionLen > 0 ) && ( OptionFound == true ))
      {
        // --- check: if  
        //      1. the argument was found in short options
        //      2. the argument found in short options is not the same like this
        if (( OptionInfo != NULL) && 
            ( OptionInfo != &Options_eo80[Curr] ))
        {
          OptError = AmbiguousOption_eeo80;
          return;
        }
        else
        {
          OptionInfo = &Options_eo80[Curr];
          Optionlen  = MinLen;
        }
      }
    }
  }

  if ( OptionInfo == NULL )
    OptError = UnknownOption_eeo80;

  return;
}

/*---------------------------------------------------------------------------*/

teo80_GetOpt::teo80_SeparateArgs::teo80_SeparateArgs ( const _TCHAR*         OptionArgument,
                                                       _TCHAR                SeparatorChar )
                     : ArgSeparatorChar (SeparatorChar)
{
  ROUTINE_DBG_MEO60 (_T("teo80_GetOpt::teo80_SeparateArgs::teo80_SeparateArgs"));
  unsigned long     OptionArgumentStrLen = (unsigned long)_tcslen(OptionArgument);

  OptionArgumentStr = new _TCHAR[OptionArgumentStrLen + 1];
  NextArg           = OptionArgumentStr;

  if ( OptionArgumentStr != NULL )
    _tcscpy ( OptionArgumentStr, OptionArgument );
}

/*---------------------------------------------------------------------------*/

const _TCHAR* teo80_GetOpt::teo80_SeparateArgs::NextPart()
{
  ROUTINE_DBG_MEO60 (_T("teo80_GetOpt::teo80_SeparateArgs::NextPart"));
  _TCHAR*        ThisArg = NULL;
  _TCHAR*        TmpPtr  = NULL;
  unsigned long  String  = 0;


  if (( OptionArgumentStr != NULL ) && ( NextArg != NULL ))
  {
    ThisArg  = NextArg;

    for ( TmpPtr = NextArg; ; TmpPtr++ )
    {
      if ( *TmpPtr == _T('"') )
        String ^= 1;
      else if ( *TmpPtr == _T('\0') )
      {
        NextArg = NULL;
        break;
      }
      else if ( (*TmpPtr == ArgSeparatorChar) && (String == 0) )
      {
        *TmpPtr    = _T('\0');
        NextArg    = TmpPtr + 1;
        break;
      }
    }
  }

  return ThisArg;
}



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
