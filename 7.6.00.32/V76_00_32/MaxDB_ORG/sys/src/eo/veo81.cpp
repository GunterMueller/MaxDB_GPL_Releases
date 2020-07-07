/*!
  @file           veo81.cpp
  @author         RaymondR
  @special area   get options (getopt)
  @brief          Parse and handle program arguments (argc,argv)
  @see            geo80.h

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
#include "geo81.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



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



/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*------------------------ class teo81_EvalProgArgs -------------------------*/
/*---------------------------------------------------------------------------*/

bool teo81_EvalProgArgs::eo80_vNextOptionDesc ( unsigned long        Idx,
                                                teo80_OptionDesc     &Option )
{
  ROUTINE_DBG_MEO60 (_T("teo81_EvalProgArgs::eo80_vNextOptionDesc"));
  bool   OptionAvail;

  if ( Idx < m_NumOfOptionDesc )
  {
    Option.OptionID_eo80           = m_OptionDesc[Idx].OptionIndex;
    Option.ShortOption_eo80        = m_OptionDesc[Idx].ShortOption;
    Option.OptionWithArgument_eo80 = m_OptionDesc[Idx].OptionWithArgument;
    
    _tcsncpy(Option.LongOption_eo80, m_OptionDesc[Idx].LongOptionStr, 
             sizeof(Option.LongOption_eo80) - 1 );

    OptionAvail = true;
  }
  else
    OptionAvail = false;

  return OptionAvail;
}

/*---------------------------------------------------------------------------*/

void teo81_EvalProgArgs::eo80_vHelp ( const TCHAR*         OptionArgument )
{ 
  ROUTINE_DBG_MEO60 (_T("teo81_EvalProgArgs::eo80_vHelp"));

# define MAX_PARAM_HELP_STR  19

  unsigned long                  Curr;
  bool                           LongHelp       = false;
  long                           StrPos         = 0;
  TCHAR                          UsageLine[2048];
  TCHAR                          ParamHelpStr[80];
  bool                           LineSplitt;
  TCHAR const                    *pOptinalOpen;
  TCHAR const                    *pOptinalClose;

  if (( OptionArgument != NULL ) && (!_tcsicmp( OptionArgument, _T("LONG"))))
    LongHelp = true;

  _stprintf ( UsageLine, _T("\nUsage:") );

  StrPos = (long)_tcslen (UsageLine);

  for ( Curr = 0; Curr < m_NumOfOptionDesc; Curr++ ) 
  {
    if (m_OptionDesc[Curr].ShortOption == _T('\0') )
      continue;

    if (m_OptionDesc[Curr].Optional == true )
    {
      pOptinalOpen  = "[";
      pOptinalClose = "]";
    }
    else
    {
      pOptinalOpen  = "";
      pOptinalClose = "";
    }

    do
    {
      if ( LongHelp == true )
      {
        if ( m_OptionDesc[Curr].OptionWithArgument )
          _stprintf ( UsageLine + StrPos, _T(" %s-%s %s%s"),
                      pOptinalOpen,
                      m_OptionDesc[Curr].LongOptionStr, 
                      m_OptionDesc[Curr].ParamHelpStr,
                      pOptinalClose );
        else
          _stprintf ( UsageLine + StrPos, _T(" %s-%s%s"),
                      pOptinalOpen,
                      m_OptionDesc[Curr].LongOptionStr,
                      pOptinalClose );
      }
      else
      {
        if ( m_OptionDesc[Curr].OptionWithArgument )
          _stprintf ( UsageLine + StrPos, _T(" %s-%c %s%s"),
                      pOptinalOpen,
                      m_OptionDesc[Curr].ShortOption, 
                      m_OptionDesc[Curr].ParamHelpStr,
                      pOptinalClose );
        else
          _stprintf ( UsageLine + StrPos, _T(" %s-%c%s"),
                      pOptinalOpen,
                      m_OptionDesc[Curr].ShortOption,
                      pOptinalClose );
      }

      if  ( _tcslen (UsageLine) >= 80 )
      {
        UsageLine[StrPos] = _T('\0');
        printf ( _T("%s\n"), UsageLine );

        _stprintf ( UsageLine, _T("      ") );
        LineSplitt = true;
      }
      else
        LineSplitt = false;

      StrPos = (long)_tcslen (UsageLine);
    }
    while ( LineSplitt == true );
  }

  printf ( _T("%s\n\n"), UsageLine );

  for ( Curr = 0; Curr < m_NumOfOptionDesc; Curr++ ) 
  {
    if ( false == m_OptionDesc[Curr].OptionWithArgument )
      ParamHelpStr[0] = _T('\0');
    else if ( MAX_PARAM_HELP_STR < _tcslen(m_OptionDesc[Curr].ParamHelpStr) )
      _tcscpy ( ParamHelpStr, _T("<...>") );
    else
      _tcscpy ( ParamHelpStr, m_OptionDesc[Curr].ParamHelpStr );

    if ( true == LongHelp )
    {
      printf ( _T(" -%-23.23s %-19s - %s\n"),
               m_OptionDesc[Curr].LongOptionStr,
               ParamHelpStr,
               m_OptionDesc[Curr].OptionHelpStr );
    }
    else if (m_OptionDesc[Curr].ShortOption != _T('\0') )
    {
      printf ( _T(" -%c %-19s - %s\n"), 
               m_OptionDesc[Curr].ShortOption,
               ParamHelpStr,
               m_OptionDesc[Curr].OptionHelpStr );
    }
  }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
