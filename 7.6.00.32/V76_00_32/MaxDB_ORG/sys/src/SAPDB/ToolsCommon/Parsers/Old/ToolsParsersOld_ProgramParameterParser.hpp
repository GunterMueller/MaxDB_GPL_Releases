/*!
    \file    ToolsParsersOld_ProgramParameterParser.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaring a parser for parameters

\if EMIT_LICENCE

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

\endif
*/

#if !defined(ToolsParsersOld_ProgramParameterParser_HPP)
#define ToolsParsersOld_ProgramParameterParser_HPP

// include all syntax elements (convinience for users)
#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Headers.hpp"

/*! \brief a class for parsing parameters
<ul>
  <li> Currently only program synatxes are parsable, that are regular languages with a right-linear grammar,
       as the parser implements a deterministic automaton, that starts parsing at the first argument
       (see also google, keywords: Chomsky, hierarchy).
       <p></p>
  </li>
  <li>
    <p>
    You can parse parameters into the parameter objects and check for syntax errors with help of
    the parse() function:
    </p>
    <p>
    <code>
    ...\n
    p.parse(syntaxError);\n
    \n
    if(syntaxError)\n
    &nbsp;&nbsp;&nbsp;&nbsp;printf("usage: ...");\n
    ...\n
    \n
    </code>
    </p>
  </li>
  <li>
    The following types of program parameters can currently be parsed:
    <ol>
    <li>Simple Parameters
    <p>
    The simplest parameters supported are non-optional parameters representing keywords (or flags) and variable parameters.
    The syntax <code>"program -A B \<var\>"</code> can be parsed by creating a object for keyword A, another one for
    keyword B and one for variable <code>\<var\></code>, adding those objects to a parameter group and parsing for
    that parameter group. The result of the parsing is stored within the
    keyword and variable objects and can be used for further program operations:</p>
    <p>
    <code>
    <A NAME="number1">...</a>\n
    ToolsParsersOldSyntEl_Keyword  a("-A");\n
    ToolsParsersOldSyntEl_Keyword  b("B");\n
    ToolsParsersOldSyntEl_Variable var;\n
    ToolsParsersOldSyntEl_ParameterGroup syntax;\n
    \n
    syntax.push_back(a);\n
    syntax.push_back(b);\n
    syntax.push_back(var);\n
    \n
    ToolsParsersOld_ProgramParameterParser parser(argc, argv, &syntax);\n
    parser.parse(...);\n
    \n
    printf("You \%s the parameter -A\\n", a.isSet()?"used":"didn't use");\n
    printf("You \%s the parameter B\\n", b.isSet()?"used":"didn't use");\n
    printf("You \%s the parameter \<var\>, it had the value \%s\\n", var.isSet()?"used":"didn't use", var.getValue());\n
    ...\n
    \n
    </code>
    </p>
    </li>
    <li>
    Optional Parameters:
    <p>
    Too change the example from <A HREF="#number1">1.)</A> into a program with optional parameters, e.g. "program [-A] [B \<var\>]", simply do not add
    the parameter objects a,b and var directly to syntax, but add them to ToolsParsersOldSyntEl_OptionalList's as needed (e.g. -A to one option list of
    its own and B and \<var\> to a second option list) and add those option lists then to syntax:
    </p>
    <p>
    <code>
    <A NAME="number2">...</a>\n
    ToolsParsersOldSyntEl_OptionalList ol1;\n
    ToolsParsersOldSyntEl_OptionalList ol2;\n
    \n
    ol1.push_back(a);\n
    ol2.push_back(b);\n
    ol2.push_back(var2);\n
    \n
    syntax.push_back(ol1);\n
    syntax.push_back(ol2);\n
    ...\n
    \n
    </code>
    </p>
    </li>
    <li>
    Alternativ Parameters:
    <p>
    To have a synatx with a group of alternativ arguments, use objects of type ToolsParsersOldSyntEl_AlternativeList.
    E.g., the syntax "program [-A] | [B \<var\>]" is achieved, by adding the two option lists of <A HREF="#number2">2.)</A>,
    which represent [-A] and [B \<var\>], to a ToolsParsersOldSyntEl_AlternativeList and by adding this alternative list
    to the syntax (note that as the alternative list is describing the whole program syntax, the alternative list
    itself could also have been used as syntax):
    </p>
    <p>
    <code>
    <A NAME="number3">...</a>\n
    ToolsParsersOldSyntEl_AlternativeList al;\n
    \n
    al.push_back(ol1);\n
    al.push_back(ol2);\n
    \n
    syntax.push_back(al);\n
    ...\n
    \n
    </code>
    </p>
    </li>
    <li>
    Repetition of parameters:
    <p>
    To allow multiple occurrences of one parameter and to allow changing sequences of parameters, use the class ToolsParsersOldSyntEl_RepetitionList.
    To allow in our example something like "program -A -A" or "program B \<var\> -A", in other words to have "program {[-A] | [B \<var\>]}"
    add the alternative list introduced in <A HREF="#number3">3.)</A> to a repetition list:
    </p>
    <p>
    <code>
    <A NAME="number4">...</a>\n
    ToolsParsersOldSyntEl_RepetitionList rl;\n
    \n
    rl.push_back(al);\n
    syntax.push_back(rl);\n
    ...\n
    \n
    </code>
    </p>
    <p>
    Note, that by default at least one element of the repetition list must be found, but as
    [-A] and [B \<var\>] were optional, a program call without any arguments would have been syntactically
    correct too.
    </p>
    <p>
    Note, that if "program B x B y" is called, the ToolsParsersOldSyntEl_Variable var will have the value y.
    If you need both values use a ToolsParsersOldSyntEl_MultipleValueVariable instead of a ToolsParsersOldSyntEl_Variable (see <A HREF="#number5">5.)</A>)
    </p>
    <p>
    \n
    </p>
    </li>
    <li>
    <A NAME="number5">Variables with Multiple Values</a>
    <p>
    If you need a variable with multiple values use class ToolsParsersOldSyntEl_MultipleValueVariable. It can be used, when one
    variable parameter can be specified more than once on the command line and all different values are to be processed in
    the program. But keep in mind, that your synatx must be a regular language, with a right-linear syntax. Therefore the
    syntax of the copy command ("copy \<source<sub>1</sub>\> \<source<sub>2</sub>\> ... \<source<sub>n</sub>\> \<destination\>")
    - a regular language with a left-linear grammar - cannot be parsed by this parser.
    </p>
    <p>
    To parse the program syntax "program {[-A] | [B \<var\>]}" and to be able to access all specified values of \<var\>, substitute
    in the code from <A HREF="#number4">4.)</A> the class ToolsParsersOldSyntEl_Variable by the class ToolsParsersOldSyntEl_MultipleValueVariable and
    access the values by using the getNumberOfValues() and getValue(i) functions:
    </p>
    <p>
    <code>
    ...\n
    ToolsParsersOldSyntEl_MultipleValueVariable var;\n
    ...\n
    parse(...)\n
    ...\n
    for(i=0; i<var.getNumberOfValues(); i++)\n
    &nbsp;&nbsp;&nbsp;&nbsp;printf("\%d. value of \<var\> was \%s.\\n", i, var.getValue(i));\n
    ...\n
    \n
    </code>
    </p>
    <p>
    Note, that currently no information about the context of each single value is available.   
    If you need information about the context of the values (e.g. you have
    "program {[-A \<var\>] [-B \<var\>]}", which is called as "program -A x -B y -A z" and you need to know if
    value z was paired with -B or -A), you have to create an appropriate multiple value class for that purpose (e.g.
    one handling lists of pairs of ToolsParsersOldSyntEl_Keyword and a ToolsParsersOldSyntEl_Variable).
    </p>
    </li>
    </ol>
  </li>
  <li>
    You can change the parameters and their syntax used during parsing with the setParametersToParse() function.
    <p></p>
  </li>
  <li>
    As there is no reset() function, parsing twice with the same set of parameters and different program parameters will
    currently fail in most cases.
    <p></p>
  </li>
</ul>
*/
class ToolsParsersOld_ProgramParameterParser
{
  public:
    /*! constructor

       The object is working only with pointers of objects, so the argv array should be available,
       as long as the object is used.

       \param argc          [IN] number of actual program arguments
       \param argv          [IN] array of zero terminated strings representing the program arguments
       \param parameterTree [IN] a pointer to the group of parameter representing the program syntax
                                 and holding the detailed parse result after function parse() was called */
    ToolsParsersOld_ProgramParameterParser(int                               argc,
                            const char * const              * argv,
                            ToolsParsersOldSyntEl_ParameterBase * syntaxTree);

    /*! function for changing the program syntax that will be parsed

        \param parameterTree [IN] a pointer to the group of parameter representing the program syntax and holding
                                  the detailed parse result after function parse() was called */
    void setParametersToParse(ToolsParsersOldSyntEl_ParameterBase * syntaxTree);
    
    /*! function for parsing

        parsing the array of program parameters provided in the constructor using the parameter syntax
        provided by the setParametersToParse() function and storing the result into the tree of parameter
        objects provided by the same function

        \param syntaxErrorFound [OUT] true, if the actual program parameters matched the syntax, false otherwise
        \return true, if no problems occured
                false otherwise (if arguments of constructor and setParametersToParse() were and are still
                OK, only out of memory errors during parsing can occure)*/
    bool parse(bool & syntaxErrorFound);

  private:
    ToolsParsersOldSyntEl_ParameterBase * m_SyntaxTree; //!< the parameter group defining the syntax and holding the result
    const int                         argC;     //!< number of actual program arguments
    const char * const * const        argV;     //!<array of actual program parameters
};

/*

    

    

    

    
    

    */
#endif
