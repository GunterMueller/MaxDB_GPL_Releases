/*!
  @file           geo81.h
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



#ifndef GEO81_H
#define GEO81_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo80.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class teo81_EvalProgArgs : public teo80_GetOpt
{
public:
  typedef struct   // - Option description
  {
    int                     OptionIndex;
    TCHAR                   ShortOption;
    const TCHAR*            LongOptionStr;
    bool                    OptionWithArgument;
    bool                    Optional;
    const TCHAR*            ParamHelpStr;
    const TCHAR*            OptionHelpStr;
  } teo81_OptionDesc; 


protected:
  inline             teo81_EvalProgArgs    ( int                  argc, 
                                             TCHAR**              argv,
                                             teo81_OptionDesc *   OptionDesc,
                                             unsigned long        NumOfOptionDesc,
                                             const int            HelpOptionID )
                                           : m_OptionDesc       ( OptionDesc ),
                                             m_NumOfOptionDesc  ( NumOfOptionDesc ),
                                             teo80_GetOpt       ( argc, argv, HelpOptionID ) {;};
                     ~teo81_EvalProgArgs   () {;};

  virtual void       eo80_vHelp            ( const TCHAR*         OptionArgument );

private:
  bool               eo80_vNextOptionDesc  ( unsigned long        Idx,
                                             teo80_OptionDesc     &Option );

private:
  teo81_OptionDesc *   m_OptionDesc;
  unsigned long        m_NumOfOptionDesc;
};


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO81_H */
