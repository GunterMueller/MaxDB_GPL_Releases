/*!========================================================================

  @file         SAPDB_StandardArgumentParser.hpp
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

  @since        2004-11-15  11:55
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


#ifndef SAPDB_STANDARDARGUMENTPARSER_HPP
#define SAPDB_STANDARDARGUMENTPARSER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_ArgumentParser.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  @class   SAPDB_StandardArgumentParser
  @brief   Parse and handle program command line arguments.

           SAPDB_StandardArgumentParser is an class which provides a 
           functionality to parse and validate command line arguments 
           given by the argument array (argv) and the argument count (argc)
           as passed to main().

           The class parses a list of tokens using a format that specifies 
           expected flags (options) and there arguments. A option is a single
           ASCII letter or a string (long option) starting with a dash and might
           be followed by an argument that may or may not be separated from it
           by one or more tabs or spaces. 

           A valid argument line might look like this:

                mygrogramm -d MyDatabase -n PetersMaschine -Q -FastConnect .... 

           The 'SAPDB_StandardArgumentParser' extends the functionality of   
           'SAPDB_ArgumentParser' by adding a standard way to handle option
           descriptions and the parse error situations by writing a usage
           information to the console screen.

           The parser completes processing when it has read all arguments or when
           it encounters the special argument -- (double dash, break option). 

 ----------------------------------------------------------------------------*/

class SAPDB_StandardArgumentParser : public SAPDB_ArgumentParser
{
public:
    /*!
       @Description  Structure describes a single option.
     
                     'OptionDesc' is a structure containing a legitimate
                     option character or long option string. The variable 
                     'optionProperties' specifies if the option properties
                     like the requires an argument or the option is optional
                     ( e.g. -x <argument> or [-r] ). Each option has an numeric 
                     value  ('optionID') to identify the option that has to be 
                     handled by the 'HandleNextOptionPass1' and 'HandleNextOption'
                     methodes.

                     Each program usually has more than one option. That means 
                     there has to be a set of option descriptions; one per 
                     legitimate option. The option descriptons have to be an array
                     which are passed as 'optionDesc' argument to the constructor.
    */
    typedef struct   // - Option description
    {
        SAPDB_Int4            optionID;            // option specific ID e.g. 1=Help, 2=SaveAll ...
        RTE_ArgChar           shortOption;         // single option character e.g.  'u', 't' ...
        const RTE_ArgChar*    longOptionStr;       // e.g.  'SaveAll', 'Update'
        SAPDB_Int4            optionProperties;    // option argument handling (e.g. OptionIsOptional, OptionHasNoArgument ... ).
        const RTE_ArgChar*    optionArgDescStr;    // Option argument description ( e.g. "<Value> | LONG" )
        const RTE_ArgChar*    optionHelpStr;       // Option help string ( e.g. "Its the executable path and name")
    } OptionDesc; 


protected:
    /*!
       @brief         Constructor

       @param   argc                            [in] - Its the argument count, usually the argc variable of main()
       @param   argv                            [in] - Its the argument array, usually the argv variable of main()
       @param   helpOptionID                    [in] - Its the identifier of the help option (e.g. -h, -? ),
                                                       if this option has been defined. The help option 
                                                       should be defined as an option with argument. So that
                                                       the argument 'LONG' could be used to display the long
                                                       help options.
       @param   optionDesc                      [in] - Pointer to the option description array what has to be a
                                                       static array or a member variable of the derived class.
                                                       This is because the constructor only stores the pointer 
                                                       to the array which is access during parse operation.
       @param   numOfOptionDesc                 [in] - Number of arguments in the option array.
       @param   caseSensitiveLongOptions        [in] - If true all long options are case sensitive.
       @param   allowSlashesAsOptionPrefix      [in] - (Windows only) Slash '/' is allowed to be an option prefix
       @param   handleMissingNoneOptionalOptions[in] - If true the usage will be displayed if a none optional
                                                        option is missing
    */
    inline             SAPDB_StandardArgumentParser( const SAPDB_Int4          argc,
                                                   const RTE_ArgChar* const*   argv,
                                                   const SAPDB_Int4            helpOptionID,
                                                   OptionDesc * const          optionDesc,
                                                   const SAPDB_UInt4           numOfOptionDesc,
                                                   const SAPDB_Bool            caseSensitiveLongOptions         = false,
                                                   const SAPDB_Bool            allowSlashesAsOptionPrefix       = true,
                                                   const SAPDB_Bool            handleMissingNoneOptionalOptions = true )
                                             : m_OptionDesc       ( optionDesc ),
                                               m_NumOfOptionDesc  ( numOfOptionDesc ),
                                               m_Idx              (0),
                                               SAPDB_ArgumentParser ( argc, argv, helpOptionID,
                                                                      caseSensitiveLongOptions,
                                                                      allowSlashesAsOptionPrefix,
                                                                      handleMissingNoneOptionalOptions ) {;}

    /*!
       @brief         Destructor
    */
    virtual            ~SAPDB_StandardArgumentParser   () {;}

    /*!
       @brief         Display the usage information or the help to a given argument.

                      This methode has to be implemented by the derived class. Usually its
                      called if a special help option was found ( e.g. -h or -? ). An
                      help option followed by an argument can be interpreted as an argument
                      help request ( e.g. -h <argument> ). 

       @param         optionArgument [in]  - The argument that following the help option (might be NULL!).

    */
    virtual void      Help( const RTE_ArgChar*    optionArgument = 0 );


    /*!
       @brief         Prints the usage information on screen.

                      If this methode is not reimplemented a 'printf' is used 
                      to write the strings to the standard output device.

       @param         helpLine [in]  - The string that has to be displayed

    */
    virtual void      Print( const RTE_ArgChar*    helpLine );

private:
    /*!
       @brief         Get the first option description

                      This methode has to be implemented by the derived class. It's call
                      by the parser to get the first option of a set of option descriptions.

       @param         option [out]  - The first option description from a set of options.

       @return        False, if the are no more options
    */
    virtual SAPDB_Bool   FirstOptionDesc( OptionDescription  &option );

    /*!
       @brief         Get the next option description

                      This methode has to be implemented by the derived class. It's call
                      by the parser to get the next option of a set of option descriptions.

       @param         option [out]  - The next option description from a set of options.

       @return        False, if the are no more options
    */
    virtual SAPDB_Bool NextOptionDesc( OptionDescription   &option );


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

private:
    OptionDesc *         m_OptionDesc;
    SAPDB_UInt4          m_NumOfOptionDesc;
    SAPDB_UInt4          m_Idx;
};

#endif  /* SAPDB_STANDARDARGUMENTPARSER_HPP */
