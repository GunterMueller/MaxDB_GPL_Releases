/*!
    \file    ToolsParsers_ProgramParameterParser.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaring a parser for program parameters

    ========== licence begin  GPL
    Copyright (c) 1998-2006 SAP AG

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

#if !defined(TOOLSPARSERS_PROGRAMPARAMETERPARSER_HPP)
#define TOOLSPARSERS_PROGRAMPARAMETERPARSER_HPP

#include "ToolsCommon/Parsers/Syntax/ToolsParsersSyntax_Syntax.hpp"
#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterBase.hpp"

/*! \brief a class for parsing program parameters
<ul>
  <li>
    Currently only program syntaxes are parsable, that are regular languages
    with a right-linear grammar, as the parser implements a deterministic
    automaton, that starts parsing at the first argument (see also google,
    keywords: Chomsky, hierarchy).
    <p></p>
  </li>
  <li>
    To parse program parameters, you have to specify the programs syntax using
    an extended BNF like syntax:
    <p></p>
    <code>
    ...\n
    int main (unsigned int argc, char ** argv)\n
    ...\n
    ToolsParsers_ProgramParameterParser parser(argc, argv, "x [y] | \<z\>");\n
    ...\n
    </code>
    <p></p>
  </li>
  <li>
    The result is stored in an object of class ToolsParsers_ParseResult that is
    returned by the parse() function of the parser:
    <p></p>
    <code>
    ...\n
    const ToolsParsers_ParseResult & parseResult=parser.parse();\n
    ...\n
    </code>
    <p></p>
  </li>
  <li>
    The result object contains information about the validity of the parser, the
    syntactical correctness of the actual program parameters with respect to
    programs parameter syntax and information about the parsed parameter values
    themselfs:
    <p></p>
    <code>
    ...\n
    if(ToolsParsers_ParseResult::NoError!=parseResult.getResultCode())\n
    ...\n
    if(parseResult.foundSyntaxError())\n
    &nbsp;&nbsp;&nbsp;&nbsp;printf("usage: ...");\n
    ...\n
    if(parseResult.getKeyword("x").isSet())\n
    ...\n
    const char * valueZ=parseResult.getVariable("\<z\>").getValue();\n
    ...\n
    </code>
    <p></p>
  </li>
  <li>
    The BNF like syntax string used to describe the programs syntax can 
    currently have the following features:
    <p></p>
    <ol>
      <li>Keywords
        <p></p>
        Keywords or flags within a programs parameters are represented in the
        syntax string by themselfs. The syntax of a program expecting the two
        parameters "hello" and "world" would have the syntax string:
        \n
        \n
        <code>... "hello world" ...</code>
        <p></p>
      </li>
      <li>String Variables
        <p></p>
        String variables within a programs syntax are represented by thier name
        enclosed in angle brackets. A program with the keyword "-d"
        followed by a variable parsing a database name would have the syntax string:
        \n
        \n
        <code>... "-d \<dbname\>" ...</code>
        <p></p>
        The default for a string variable can be changed by appending the string
        "DEF=" followed by the new default. E.g. <code>"\<size\>DEF=0"</code>.
        Note that there must not be any space between the closing angle bracket
        and "DEF=" and between "DEF=" and the default value. The default for the
        default is the empty string (<code>""</code>). If the default contains a
        space it must be enclosed within double quotes (e.g. <code>
        \<path\>DEF="C:\\Program Files"</code>). Default values starting with
        double quotes or containing spaces and double qoutes are not supported
        yet.
      </li>
      <li>Optional Parameters
        <p></p>
        To make a parameter or group of parameters optional enclose them into
        squared brackets. E.g.:
        \n
        \n
        <code>... "[-d \<dbname\>]" ...</code>
        <p></p>
      </li>
      <li>Alternatives
        <p></p>
        The pipe sign '|' can be used in the syntax string to declare parts
        of the program syntax as alternative for each other. A program accepting
        either "x" or "y" has the syntax string:
        \n
        \n
        <code>... "x | y" ...</code>
        <p></p>
        Note that an alternative has a lower precedence than simple
        concatenation by spaces. Therefore <code>"hello world | hello x"</code>
        means, that the program accepts either the two parameters "hello world"
        or the two parameters "hello x". You have to use round brackets (a
        parameter group, see also below), if another semantic is needed. 
        <p></p>
        Note further, that during parsing the first alternative matching the
        actual program parameters is used. Syntax <code>"\<x\> | y"</code> will
        always parse the first program parameter into variable
        <code>\<x\></code> and keyword "y" will never be found.
        <p></p>
      </li>
      <li>Repetition of Parameters
        <p></p>
        To allow multiple occurrences of a parameter or a parameter group,
        enclose them in curly brackets. E.g.:
        \n
        \n
        <code>... "{hello world}" ...</code>
        \n
        \n
        This syntax string allows the actual parameters "hello world",
        "hello world hello world", "hello world hello world hello world", etc.
        <p></p>
        Note, that by default at least one instance of the parameter or
        parameter group enclosed within the curly brackets must be found during
        the parse run. To allow a parameter or parameter group to be found 0 or
        more times, combine the curly brackets for repetition with the squared
        brackets for optional parameters. E.g.
        <code>"{[hello world]}"</code> or
        <code>"[{hello world}]"</code>.
        <p></p>
        Note, that if a variable is part of a repetition (e.g.
        <code>"{-d \<dbname\>}"</code>) only the last parsed value (e.g. "y" of
        the parameters "-d x -d y") will currently be stored within the
        variable. This will be changed in the future.
        <p></p>
      </li>
      <li>Permutations of Parameters
        <p></p>
        If all permutation of a group of parameters are valid program parameters
        they can be enclosed in a parenthesis followed by an exclamation mark:
        \n
        \n
        <code>... "(a b c)!" ...</code>
        <p></p>
        Note that O(n*n) time is needed to parse such a permutation of
        parameters. This is however much more efficient than the alternative of
        including all n! permutations (e.g. <code>"(a b c)|(a c b)|(b a c)|
        (b c a)|(c a b)|(c b a)"</code>) directly into the syntax string.
        <p></p>
      </li>
      <li>Parameter Groups
        <p></p>
        You can group parameters by enclosing them into round brackets:
        \n
        \n
        <code>... "(x y)" ...</code>
        \n
        \n
        As usual this can be used to override operator precedence. E.g.
        <code>"x|(y z)"</code> means that the program expects either the
        argument "x" or the arguments "y z". 
      </li>
    </ol>
  </li>
</ul>
\see enum ToolsParsers_ParseResult::ResultCode
\see ToolsParsersSyntEl_Keyword
\see ToolsParsersSyntEl_Variable

\todo support Msg_List for parse results in 7.6
\todo use multiple value variables instead of simple variables
\todo correct and integrate class for integer arguments
\todo create and integrate a class for comma separated variable lists
\todo simplify simple syntax output by introducing the concept of operator
      precedence
\todo think about parameter grouping such as "-x -y" -> "-xy"
*/
class ToolsParsers_ProgramParameterParser
{
  public:
    /*! constructor

        The resulting object is working only with pointers of objects, so the
        argv array and the syntax object should be available as long as
        the object is used.

        \param argc   [IN] number of actual program arguments
        \param argv   [IN] array of zero terminated strings representing the
            program arguments
        \param syntax [IN] a pointer to a string representing the syntax to
            parse as a rule of an exetended BNF */
    ToolsParsers_ProgramParameterParser(
        int                  argc,
        const char * const * argv,
        const char *         syntax);

    /*! function for parsing

        parsing the array of program parameters provided in the constructor
        using the program syntax provided in the constructor and storing the
        result into a ToolsParsers_ParseResult object.

        \see class ToolsParsers_ParseResult

        \return an object containing information about the parse result */
    const ToolsParsers_ParseResult & parse(
        );

    /*! function allowing const access to the syntax used by the parser */
    const ToolsParsersSyntax_Syntax & getSyntax(
        ) const;

  private:
    ToolsParsersSyntax_Syntax  m_Syntax; //!< the program syntax used by the parser during the parse run

    const int                  m_ArgCount;   //!< number of actual program arguments
    const char * const * const m_ArgVector;  //!< array of actual program parameters

    bool m_notParsed;   //!< flag preventing multiple parsing

    const char * m_DummyVector[2]; //!< dummy argument vector, used if no arguments are given
};

#endif
