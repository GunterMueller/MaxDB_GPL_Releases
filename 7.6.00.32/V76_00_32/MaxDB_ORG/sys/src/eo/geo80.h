/*!
  @file           geo80.h
  @author         RaymondR
  @special area   get options (getopt)
  @brief          Parse and handle program arguments (argc,argv)
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



#ifndef GEO80_H
#define GEO80_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

# include <stdio.h>
# include "geo47.h"
# include "geo60.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define  UNDEF_OPTION_INDEX_EO80        -1
#define  MAX_LONG_OPTION_EO80           30


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


#define NUM_OF_OPTIONS_EO80(_Options)   (sizeof(_Options)/sizeof(_Options[0]))

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined( __cplusplus )

/*---------------------------------------------------------------------------*/
/*--------------------------- class teo80_GetOpt ----------------------------*/
/*---------------------------------------------------------------------------*/

/*!
  @class          teo80_GetOpt
   @Description    Parse and handle program arguments (argc,argv).


                The teo80_GetOpt is an abstract base class which provides a
                functionality to parse validate and handle program arguments, 
                given by 'argc', 'argv'.

 */

class teo80_GetOpt
{
public:
    inline const _TCHAR*       eo80_GetProgramName    () const;
    bool                       eo80_ParseArguments    ();

protected:
  /*!
     @Description    Protected types used by virtual member functions.

   */

  typedef struct    // - used to discribe a single option
  {
    int          OptionID_eo80;                             // option specific ID e.g. 1=Help, 2=SaveAll ..
    _TCHAR       ShortOption_eo80;                          // single character e.g.  's'
    _TCHAR       LongOption_eo80[MAX_LONG_OPTION_EO80 + 1]; // e.g.  'SaveAll'
    bool         OptionWithArgument_eo80;                   // true, if an argument is following.
  } teo80_OptionDesc;

  typedef enum      // - used to discribe the parse errors
  {
    NoError_eeo80,       
    MissingOptionSpecifier_eeo80,       // '-' or '/' without option char or option string
    UnknownOption_eeo80,                // option char or option string is unknown
    MissingOptionArgument_eeo80,        // option char or option string without parameter
    AmbiguousOption_eeo80               // ambiguous option found 
  } teo80_OptError;

  


  teo80_GetOpt( const int                                 Argc, 
                const _TCHAR* const*                      Argv, 
                const int                                 HelpOptionID                = UNDEF_OPTION_INDEX_EO80,
                const bool                                CaseSensitiveLongOptions    = false );

  ~teo80_GetOpt() { delete Options_eo80;}


  virtual bool       eo80_vNextOptionDesc        ( unsigned long        Idx,
                                                   teo80_OptionDesc     &Option )          = 0;

  virtual bool       eo80_vNextOptionPass1       ( const int            OptionID,
                                                   const _TCHAR*        OptionArgument )  { return true;};

  virtual bool       eo80_vNextValidOption       ( const int            OptionID,
                                                   const _TCHAR*        OptionArgument )   = 0;

  virtual void       eo80_vHelp                  ( const _TCHAR*        OptionArgument )   = 0;

  virtual bool       eo80_vPass1Finished         ( const _TCHAR* const* ArgvRemaining,
                                                   int                  ArgsRemaining,
                                                   int                  ArgsParsed,
                                                   bool                 BreakOptionFound ) 
                                                 { return true; };

  virtual bool       eo80_vParseArgumentsFinished( const _TCHAR* const* ArgvRemaining,
                                                   int                  ArgsRemaining,
                                                   int                  ArgsParsed,
                                                   bool                 BreakOptionFound ) 
                                                 { 
                                                   bool Ok = true;

                                                   if (( ArgsRemaining > 0 ) || ( ArgsParsed == 0 ))
                                                   {
                                                      eo80_vHelp(NULL);
                                                      Ok = false;
                                                   }
                                                   return Ok;
                                                 };

  virtual bool       eo80_vOptionHelpRequest     ( const int            OptionID )    
                                                 { return false;};

  virtual void       eo80_vHelpOnError           ( teo80_OptError       OptError,
                                                   const _TCHAR*        Option )
                                                 { eo80_vHelp(NULL); };
protected:
  class teo80_SeparateArgs
  {
  public:
    teo80_SeparateArgs            ( const _TCHAR*         OptionArgument, 
                                    _TCHAR                SeparatorChar );
    ~teo80_SeparateArgs           () { delete OptionArgumentStr; }

    const _TCHAR*  NextPart       (); 

  private: 
    _TCHAR*         OptionArgumentStr;
    _TCHAR          ArgSeparatorChar;
    _TCHAR*         NextArg;
  };


private:
  inline int         eo80_ArgsRemaining     () const;
  void               eo80_FindOption        ( const _TCHAR*            Arg,
                                              teo80_OptionDesc*        &OptionInfo,
                                              unsigned long            &Optionlen,
                                              teo80_OptError           &OptError ) const;
  void               eo80_GetOptionDesc     ();

  int                             Argc_eo80;
  const _TCHAR* const*            Argv_eo80;
  teo80_OptionDesc*               Options_eo80;
  unsigned long                   NumOfOptionDesc_eo80;
  int                             ArgIndex_eo80;
  int                             HelpOptionID_eo80;
  bool                            CaseSensitiveLongOptions_eo80;
  
private:  // - copy object not allowed!
  inline                     teo80_GetOpt ( const teo80_GetOpt& ) {}; 
  inline const teo80_GetOpt& operator =   ( const teo80_GetOpt& ) { return *this; }; 
};

/*---------------------------------------------------------------------------*/

inline int teo80_GetOpt::eo80_ArgsRemaining () const
{
  ROUTINE_DBG_MEO60 (_T("teo80_GetOpt::eo80_ArgsRemaining"));

  int ArgsRemaining = Argc_eo80 - ArgIndex_eo80;

  return ArgsRemaining < 0 ? 0 : ArgsRemaining;
}

/*---------------------------------------------------------------------------*/

inline const _TCHAR*  teo80_GetOpt::eo80_GetProgramName () const
{
  ROUTINE_DBG_MEO60 (_T("teo80_GetOpt::eo80_GetProgramName"));

  return Argv_eo80[0];
}

#endif /* __cplusplus */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO80_H */
