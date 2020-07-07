/*!
  -----------------------------------------------------------------------------

  module: hls14_scan.h

  -----------------------------------------------------------------------------

  responsible:  SteffenS, d033893

  special area: Loader

  description:  Definition of (flex generated) class which implements the scanner

  version:      7.4.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1999-2005 SAP AG-2000
 
  -----------------------------------------------------------------------------



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

#ifndef HLS14_SCAN_H
#define HLS14_SCAN_H

#include "gls00.h"
#include "hls14_global.h"
#include "hls14_FlexDefs.h"
#include "hls17_BufPars.h"
#include "hls98msg.h"

//***************************************************************************
// KLASSE        : ExternalScanner
//
// Klasse, die die Funktionalität des externen Scanners 
// implementiert
//***************************************************************************
class LoaderScanner
{
public:
     LoaderScanner( tls00_Command*  SvrCmd, ErrorNumbers& Error);
    ~LoaderScanner();
    
// Operations

    // The variable is used in the scanner grammar file for returning values.
    void    ls14ExecUnscan();       // backs up a complete token
    int     yylex(ScanReturn& ReturnStructure);
    
    // Rücksetzen von Puffer und Eingabestrom
    void    yyrestart( char *input_source );
    void    yy_load_buffer_state();
    
    // Input- and output streams
    void    switch_streams( char *new_in );
    
    // Methods for information and debugging
    inline int          lineno() const          { return yylineno; }
    inline int          debug() const           { return yy_flex_debug; }
    inline const char*  YYText()                { return yytext; }
    inline int          YYLeng()                { return yyleng; }
    inline void         set_debug( int flag )   { yy_flex_debug = flag; }
    inline char*        ls14GetToken()          { return (char*) pszTokenName; }

    
protected:
// Attributes
    char                *yyin;      // input stream
    
    yy_buffer_state     *yy_current_buffer; // input buffer (special class)
    
    ErrorNumbers        ScanError;  // Error variable
   
    // vom Scanner benutzte Variablen (durch flex vordefiniert)
    char*   yytext;
    char*   yy_c_buf_p;
    int     yyleng;
    int     yylineno;               // only maintained if you use %option yylineno
    int     yy_flex_debug;          // only has effect with -d or "%option debug"
    int     yy_start_stack_ptr;
    int     yy_start_stack_depth;
    int*    yy_start_stack;
    char    yy_hold_char;           // holds the character lost when yytext is formed.
    int     yy_n_chars;             // Number of characters read into yy_ch_buf.
    int     yy_start;               // start state number
   
    // The following are not always needed, but may be depending
    // on use of certain flex features (like REJECT or yymore()).
    yy_state_type   yy_last_accepting_state;
    char*           yy_last_accepting_cpos;
    yy_state_type*  yy_state_buf;
    yy_state_type*  yy_state_ptr;
    char*           yy_full_match;
    int*            yy_full_state;
    int             yy_full_lp;
    int             yy_lp;
    int             yy_looking_for_trail_begin;
    int             yy_more_flag;
    int             yy_more_len;
    int             yy_more_offset;
    int             yy_prev_more_offset;
   
   
    // Flag which is used to allow yywrap()'s to do buffer switches
    // instead of setting up a fresh yyin.  A bit of a hack ...
    int             yy_did_buffer_switch_on_eof;

    // Flag fuer die Tokengenerierung
    int             loader_command_found;

// Operations
    // Ein-/Ausgaberoutinen
    int LexerInput( char* buf, int max_size );

    // Methode mit Code, der nach Abarbeitung eines Satzes ausgeführt werden soll
    int wrap() { return 1; }

private:
// Attributes
    char            pszTokenName[32];
    tls00_Command   *pLoaderCommand;
	int				yylex_return;		// return value used in user actions

// Operations

    inline void IncreasePosition()  // Increments the pos within the command given in the constructor
    {
        pLoaderCommand->cmdReqPos_ls00      += yyleng;
    }

    inline void CopyToken(const char *pszToken)
    {
        strcpy(pszTokenName, pszToken);
    }


    //***************************************************************************
    // Implementierung der Methoden, die von flex benötigt werden, aber die
    // spezifisch für diesen Scanner sind
    //***************************************************************************
    void            yyunput( int c, char* buf_ptr );
    int             yyinput();

    void            yy_push_state( int new_state );
    void            yy_pop_state();
    int             yy_top_state();

    yy_state_type   yy_get_previous_state();
    yy_state_type   yy_try_NUL_trans( yy_state_type current_state );
    int             yy_get_next_buffer();


}; 

#endif // HLS14_SCAN_H

