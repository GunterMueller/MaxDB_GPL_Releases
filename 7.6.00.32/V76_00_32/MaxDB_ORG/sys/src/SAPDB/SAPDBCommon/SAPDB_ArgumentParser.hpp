/*!========================================================================

  @file         SAPDB_ArgumentParser.hpp
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

  @since        2004-11-15  11:59
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


#ifndef SAPDB_ARGUMENTPARSER_HPP
#define SAPDB_ARGUMENTPARSER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define  UNDEF_OPTION_INDEX             -1
#define  MAX_LONG_OPTION                20

#if defined (_WIN32)
# define RTE_ArgCharT                   _T
#else
# define RTE_ArgCharT(_s)               _s
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


#define NUM_OF_OPTIONS_DESC(_Options)   (sizeof(_Options)/sizeof(_Options[0]))


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

const SAPDB_UInt4 OptionIsOptional         = 0x0001;     // the option is optional (e.g. [-e]  [-x <argument>] ).
const SAPDB_UInt4 OptionHasAnArgument      = 0x0002;     // the option always has an argument
const SAPDB_UInt4 OptionArgumentIsOptional = 0x0004;     // the option argument is optional (e.g. -x[<argument>]).


/*!---------------------------------------------------------------------------
  @class   SAPDB_ArgumentParser
  @brief   Parse and handle program command line arguments.

           SAPDB_ArgumentParser is an abstract base class which provides a
           functionality to parse and validate command line arguments given by
           the argument array (argv) as passed to main().

           The class parses a list of tokens using a format that specifies 
           expected flags (options) and there arguments. A option is a single
           ASCII letter or a string (long option) starting with a dash and might
           be followed by an argument that may or may not be separated from it
           by one or more tabs or spaces. 

           A valid argument line might look like this:

                mygrogramm -d MyDatabase -n PetersMaschine -Q -FastConnect .... 

           The parser completes processing when it has read all arguments or when
           it encounters the special argument -- (double dash, break option). 

 ----------------------------------------------------------------------------*/

class SAPDB_ArgumentParser
{
public:
    /*!
      @brief         Program name

                     Returns the name and path of the program executable

      @return        Name and path of the executable (like argv[0])
    */
    inline const RTE_ArgChar*  ProgramName() const;

    /*!
      @brief         Start the argument parsing.
                     
                     After the object was created, call this methode to start the
                     parse operation.

      @return        False, if an parse error has occurred.
    */
    SAPDB_Bool   StartParsing();

protected:
    /*!
       @Description  Structure describes a single option.
     
                     'OptionDescription' is a structure containing a legitimate
                     option character or long option string. The variable 
                     'optionArgument' specifies if the option requires an argument
                     ( e.g. -x <argument> ). Each option has an numeric value 
                     ('optionID') to identify the option that has to be 
                     handled by the 'HandleNextOptionPass1' and 'HandleNextOption' methodes.

                     Each program usually has more than one option. That means 
                     there has to be a set of option descriptions; one per 
                     legitimate option. To build up this set, the methodes 
                     'FirstOptionDesc' and 'NextOptionDesc' are called until
                     false is returned.
    */
    typedef struct    
    {
        SAPDB_Int4     optionID;                             // option specific ID e.g. 1=Help, 2=SaveAll ...
        RTE_ArgChar    shortOption;                          // single option character e.g.  'u', 't' ...
        RTE_ArgChar    longOption[MAX_LONG_OPTION + 1];      // e.g.  'SaveAll', 'Update'
        SAPDB_Int4     optionProperties;                     // option argument handling (e.g. OptionIsOptional, OptionHasNoArgument ... ).
        SAPDB_Int4     internal;                             // reserved, value is used internally 
    } OptionDescription;

    /*!
       @Description    Argument parse errors. On error its passed to 'HelpOnError'
    */
    typedef enum
    {
        NoError,       
        MissingOptionSpecifier,       // dash ('-') without an option char or option string found
        UnknownOption,                // option char or option string is unknown
        MissingOptionArgument,        // option char or option string without argument
        AmbiguousOption,              // ambiguous option found 
        MissingOption                 // the option is none optional but is missing 
    } OptionError;

  
    /*!
       @brief    Constructor

       @param    argc                            [in] - Its the argument count, usually the argc variable of main()
       @param    argv                            [in] - Its the argument array, usually the argv variable of main()
       @param    helpOptionID                    [in] - Its the identifier of the help option (e.g. -h, -? ),
                                                        if this option has been defined.
       @param    caseSensitiveLongOptions        [in] - If true all long options are case sensitive.
       @param    allowSlashesAsOptionPrefix      [in] - (Windows only) Slash '/' is allowed to be an option prefix
       @param    handleMissingNoneOptionalOptions[in] - If true the usage will be displayed if a none optional
                                                        option is missing
                                                         
    */
    SAPDB_ArgumentParser( const SAPDB_Int4             argc, 
                          const RTE_ArgChar* const*    argv, 
                          const SAPDB_Int4             helpOptionID                      = UNDEF_OPTION_INDEX,
                          const SAPDB_Bool             caseSensitiveLongOptions          = false,
                          const SAPDB_Bool             allowSlashesAsOptionPrefix        = true,
                          const SAPDB_Bool             handleMissingNoneOptionalOptions  = true
                        );


    /*!
       @brief         Destructor
    */
    virtual ~SAPDB_ArgumentParser();

    /*!
       @brief         Get the first option description

                      This methode has to be implemented by the derived class. It's call
                      by the parser to get the first option of a set of option descriptions.

       @param         option [out]  - The first option description from a set of options.

       @return        False, if the are no more options
    */
    virtual SAPDB_Bool   FirstOptionDesc( OptionDescription  &option ) = 0;

    /*!
       @brief         Get the next option description

                      This methode has to be implemented by the derived class. It's call
                      by the parser to get the next option of a set of option descriptions.

       @param         option [out]  - The next option description from a set of options.

       @return        False, if the are no more options
    */
    virtual SAPDB_Bool NextOptionDesc( OptionDescription   &option ) = 0;


    /*!
       @brief         Handle the next option, found during pass 1.
  
                      The parser calls this virtual methode each time an option was found during
                      the first pass of a two pass argument parsing.

                      The two pass parsing operation is helpfull to handle complex option argument 
                      dependencies. In the most cases this methode is NOT reimplemented by the
                      derived class.

                      Example:
                        The argument layout of option '-x <...>' might depend on the existence of 
                        option '-g'. That means, if the '-g' option is given, the argument
                        of '-x <...>' is '<entryname>,<entryvalue>'. In the other case the argument
                        has to be '<entryname>'.
                        Since the option sequence is unrestricted the '-g' option must be detected
                        during the first pass to handle the argument of option '-x' correctly during
                        pass two.

       @param         optionID       [in]     - Option specific identifier of the option that
                                                was found ( e.g. -h = 1 for help, it's part of
                                                the option description 'OptionDescription' ). 
       @param         optionArgument [in]     - Its the argument of the option, if any else NULL!

       @return        False, if the argument could not be handled or is not valid. In that case
                      the parse operation (StartParsing) is terminated with false.
    */
    virtual SAPDB_Bool HandleNextOptionPass1( const SAPDB_Int4       optionID,
                                              const RTE_ArgChar*     optionArgument = 0 )
                                            { return true; };

    /*!
       @brief         The first pass of argument parsing has finised without errors.

                      The parser calls this virtual methode when it has read all arguments 
                      or when it encounters the special argument -- (double dash, break option) 
                      during the first pass of parsing.

       @param         argvRemaining    [in]     - Array of arguments which are unhandled,
                                                  argument might be NULL!
       @param         argsRemaining    [in]     - Number of remaining arguments, might be zero.
       @param         argsParsed       [in]     - Number of arguments/options parsed.
       @param         breakOptionFound [in]     - Special token -- (double dash) has stopped
                                                  parsing.

       @return        False, if the remaining arguments could not be handled.
                      In that case the parse operation (StartParsing) is terminated with false.
    */
    virtual SAPDB_Bool Pass1ParseFinished ( const RTE_ArgChar* const* argvRemaining,
                                            SAPDB_Int4                argsRemaining,
                                            SAPDB_Int4                argsParsed,
                                            SAPDB_Bool                breakOptionFound ) 
                                          { return true; };

    /*!
       @brief         An argument without an option was found.

                      The parser calls this virtual methode if an argument was found that has not 
                      a leading option. Usually, each argument has an option (e.g. -d <database>
                      -U <user> -F <file>) to distinguish different kinds of arguments.
                      One reason not to have an leading option could be, that the first argument
                      is mandatory and always has the same meaning ( e.g. its always the database
                      name).

                      If this methode is not reimplemented, an argument without an option breaks
                      the argument parsing and calls the 'ParseFinished' methode. The 
                      'ParseFinished' methode has to handle all remaining arguments or has to
                      deal with this error situation.

                      This methode is only called during the second pass of parsing. On the 
                      first pass all arguments that does not have an option are scipped.

       @param         argIndex [in]                    - Its the n'th option of the argument list
       @param         argument [in]                    - Its the argument which has no option

       @return        False, if the argument could not be handled or is not valid. In that case
                      the parsing is stopped and the 'ParseFinished' methode is called. If the 
                      return value is 'true' the parsing is going on with the next argument.
    */
    virtual SAPDB_Bool ArgumentWithoutOption( const SAPDB_Int4       argIndex,
                                              const RTE_ArgChar*     argument )        
                                           { return false; };

    /*!
       @brief         The next option found.

                      The parser calls this virtual methode each time an option was found during
                      the second pass of a two pass argument parsing. The derived class has to
                      implement this methode.

       @param         optionID       [in]     - Option specific identifier of the option that
                                                was found ( e.g. -h = 1 for help, it's part of
                                                the option description 'OptionDescription' ). 
       @param         optionArgument [in]     - Its the argument of the option, if any else NULL!.

       @return        False, if the argument could not be handled or is not valid. In that case
                      the parse operation (StartParsing) is terminated with false.
    */
    virtual SAPDB_Bool HandleNextOption            ( const SAPDB_Int4       optionID,
                                                     const RTE_ArgChar*     optionArgument = 0 ) = 0;

    /*!
       @brief         The argument parsing has finised without errors.

                      The parser calls this virtual methode
                   
                        - if it has read and handled all arguments without errors
                        - if it encounters the special token -- (double dash, break option). 
                        - if the 'ArgumentWithoutOption' methode returns false because there is 
                          an argument with no option which could not be handled.

       @param         argvRemaining    [in]     - Array of arguments which are unhandled,
                                                  argument might be NULL!
       @param         argsRemaining    [in]     - Number of remaining arguments.
       @param         argsParsed       [in]     - Number of arguments/options parsed.
       @param         breakOptionFound [in]     - Special token -- (double dash) has stopped
                                                  parsing.

       @return        False, if the remaining arguments could not be handled.
                      In that case the parse operation (StartParsing) is terminated with false.
    */
    virtual SAPDB_Bool ParseFinished( const RTE_ArgChar* const*  argvRemaining,
                                      SAPDB_Int4                 argsRemaining,
                                      SAPDB_Int4                 argsParsed,
                                      SAPDB_Bool                 breakOptionFound ) 
                                   { 
                                       if (( 0 < argsRemaining ) || ( 0 == argsParsed ))
                                       {
                                           Help();
                                           return false;
                                       }
                                       return true;
                                   };
    /*!
       @brief         Display the help information of a requested option.

                      This methode can be reimplemented by the derived class to explain
                      an option in more detail.
                      
                      If this methode returns false the 'Help' methode is called which
                      usually displays a complete usage information. 

       @param         optionID [in]     - Option specific identifier of the option which
                                          has to be explained in more detail.
                                        
       @return        'Help' will be called, if false.
    */
    virtual SAPDB_Bool OptionHelpRequest( const SAPDB_Int4        optionID )
                                        { return false; };

    /*!
       @brief         Display the usage information or the help to a given argument.

                      This methode has to be implemented by the derived class. Usually its
                      called if a special help option was found ( e.g. -h or -? ). An
                      help option followed by an argument can be interpreted as an argument
                      help request ( e.g. -h <argument> ). 

       @param         optionArgument [in]  - The argument that following the help option (might be NULL!).

    */
    virtual void      Help( const RTE_ArgChar*    optionArgument = 0 ) = 0;


    /*!
       @brief         Display help if parse error has been detected.

                      Reimplement this methode to explain parse error in more detail.            

       @param         optError [in]  - Parse error code.
       @param         option   [in]  - Option or argument that caused the error (might be NULL!).
  */
  virtual void       HelpOnError( OptionError            optError,
                                  const RTE_ArgChar*     option = 0 )
                                { Help(); };


    /*!
       @brief         Check if an argument contains valid characters.

                      Returns true if 'optionArgument' consists entirely of characters
                      in 'allowedChars'.

       @param         optionArgument [in]  - The argument that should be checked.
       @param         allowedChars   [in]  - Character string containing all legal characters.

       @return       true if argument contains valid characters
  */
  SAPDB_Bool         ArgumentIsValid ( const RTE_ArgChar*     optionArgument, 
                                       const RTE_ArgChar*     allowedChars );

    /*!
       @brief         Convert argument to a unsigned integer value.


       @param         optionArgument [in]  - The argument that should be checked.
       @param         value          [out] - Unsigned interger value

       @return       true if argument was converted
  */
  SAPDB_Bool         ArgumentToUInt ( const RTE_ArgChar*       optionArgument,
                                      SAPDB_UInt4              &value );

protected:

    /*!---------------------------------------------------------------------------
      @class   SeparateArgs
      @brief   Split an argument into small pieces 

               Use this utility class to split arguments which are a concatenation
               of small arguments pieces speparate by a single separator character
               like a comma or a slash  (e.g. 'MyPC,Peter,MyPassword1').
    ----------------------------------------------------------------------------*/
    class SeparateArgs
    {
    public:
        /*!
           @brief         Constructor.
 
                          Construct an object of 'SeparateArgs'.

           @param         optionArgument [in]  - Argument that has to be splitted (e.g. 'red/green/orange')
           @param         separatorChar  [in]  - A single separator character like comma,
                                             slash etc.
        */
        SeparateArgs( const RTE_ArgChar*      argument, 
                      RTE_ArgChar             separatorChar );
        /*!
           @brief         Destructor.
        */
        ~SeparateArgs();

        /*!
           @brief         Get the next part of the argument
 
                          Call this methode to get the next part of the separated 
                          argument. If its called the first time, it returns
                          the first argument part.

           @return        Pointer to the first/next argument option part.
        */
        const RTE_ArgChar*  NextPart(); 

    private: 
        RTE_ArgChar*         m_OptionArgumentStr;
        RTE_ArgChar          m_ArgSeparatorChar;
        RTE_ArgChar*         m_NextArg;
    };


private:
    inline SAPDB_Int4  ArgsRemaining() const;
    void               FindOption   ( const RTE_ArgChar*       arg,
                                      OptionDescription*       &optionInfo,
                                      SAPDB_UInt4              &optionlen,
                                      OptionError              &optError ) const;
    void               GetOptionDesc();

    SAPDB_Bool         SearchForMissingNoneOptionalOptions( OptionDescription*   &optionInfo ) const;

    SAPDB_Int4                      m_Argc;
    const RTE_ArgChar* const*       m_Argv;
    OptionDescription*              m_Options;
    SAPDB_UInt4                     m_NumOfOptionDesc;
    SAPDB_Int4                      m_ArgIndex;
    SAPDB_Int4                      m_HelpOptionID;
    SAPDB_Bool                      m_CaseSensitiveLongOptions;
    SAPDB_Bool                      m_AllowSlashesAsOptionPrefix;
    SAPDB_Bool                      m_HandleMissingNoneOptionalOptions;

private:  // - copy object not allowed!
    inline                     SAPDB_ArgumentParser ( const SAPDB_ArgumentParser& ) {}; 
    inline const SAPDB_ArgumentParser& operator =   ( const SAPDB_ArgumentParser& ) { return *this; }; 
};

/*---------------------------------------------------------------------------*/

inline SAPDB_Int4   SAPDB_ArgumentParser::ArgsRemaining () const
{
    SAPDB_Int4  argsRemaining = m_Argc - m_ArgIndex;

    return argsRemaining < 0 ? 0 : argsRemaining;
}

/*---------------------------------------------------------------------------*/

inline const RTE_ArgChar*  SAPDB_ArgumentParser::ProgramName () const
{
    return m_Argv[0];
}

#endif  /* SAPDB_ARGUMENTPARSER_HPP */
