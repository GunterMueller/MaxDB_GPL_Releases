/*



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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
//***************************************************************************
//
//  DATEI        : hls14_FlexDefs.h
//   
//  AUTOR        : Tim Wöhrle, SAP AG, BEB Kern
//   
//  Copyright (c) 1999-2005 SAP AG-2000
//
//  BESCHREIBUNG :
//      von flex definierte und (abhängig von der Grammatik) benötigte 
//      Konstanten und Makros
//
//  ÄNDERUNGEN   : 
//      23.11.98 - Erstellt
//
//****************************************************************************
#ifndef HLS14_FLEXDEFS_H
#define HLS14_FLEXDEFS_H

#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 5
#define SKELETT_VERSION  "1"


/* The following is because we cannot portably get our hands on size_t
* (without autoconf's help, which isn't available because we want
* flex-generated scanners to compile on their own).
*/
typedef unsigned int yy_size_t;

typedef int yy_state_type;


#define yyconst const


/* Returned upon end-of-file. */
#define YY_NULL 0

/* Promotes a possibly negative, possibly signed char to an unsigned
* integer for use as an array index.  If the signed char is negative,
* we want to instead treat it as an 8-bit unsigned char, hence the
* double cast.
*/
#define YY_SC_TO_UI(c) ((unsigned int) (unsigned char) c)

/* Enter a start condition.  This macro really ought to take a parameter,
* but we do it the disgusting crufty way forced on us by the ()-less
* definition of BEGIN.
*/
#define BEGIN yy_start = 1 + 2 *

/* Translate the current start state into a value that can be later handed
* to BEGIN to return to the state.  The YYSTATE alias is for lex
* compatibility.
*/
#define YY_START ((yy_start - 1) / 2)
#define YYSTATE YY_START

/* Action number for EOF rule of a given start state. */
#define YY_STATE_EOF(state) (YY_END_OF_BUFFER + state + 1)

/* Special action meaning "start processing a new file". */
#define YY_NEW_FILE yyrestart( yyin )

#define YY_END_OF_BUFFER_CHAR 0

/* Size of default input buffer. */
#define YY_BUF_SIZE 16384

#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2
#define unput(c) yyunput( c, yytext_ptr )

#define ECHO
#ifndef ECHO
   #define ECHO LexerOutput( yytext, yyleng )
#endif

#ifndef yyterminate
#define yyterminate() return YY_NULL
#endif


/* Number of entries by which start-condition stack grows. */
#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif


/* Code executed at the end of each rule. */
#ifndef YY_BREAK
#define YY_BREAK break;
#endif

#ifndef YY_EXIT_FAILURE
#define YY_EXIT_FAILURE 2
#endif
   


/* Amount of stuff to slurp up with each read. */
#ifndef YY_READ_BUF_SIZE
#define YY_READ_BUF_SIZE 8192
#endif


/* Code executed at the beginning of each rule, after yytext and yyleng
* have been set up.
*/
#ifndef YY_USER_ACTION
#define YY_USER_ACTION
#endif

#define YY_AT_BOL() (yy_current_buffer->yy_at_bol)


#endif  // HLS14_FLEXDEFS_H