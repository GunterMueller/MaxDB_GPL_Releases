/*
  -----------------------------------------------------------------------------
   module: vls17_BufPars.cpp
   -----------------------------------------------------------------------------
   responsible:  TimW and SteffenS

  special area: Replication Manager
 
  description:  Buffer class for lex scanner
                
  version:      7.6.
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

#include <string.h>
#include "gls00.h"
#include "hls14_FlexDefs.h"
#include "hls17_BufPars.h"

   
/*
  -----------------------------------------------------------------------------
  function:     yy_buffer_state Constructor
  -----------------------------------------------------------------------------
*/
yy_buffer_state::yy_buffer_state( char *yy_input_stream, int BufSize )
{
    yy_buf_size = BufSize + 2;              // buffer contains command to parse and additional 2 EOB chars
    
    yy_ch_buf   = new char[yy_buf_size];
    SAPDB_memcpy(yy_ch_buf, yy_input_stream, BufSize);

    // We always need two end-of-buffer characters.  The first causes a transition to
    // the end-of-buffer state.  The second causes a jam in that state.
    yy_ch_buf[BufSize]      = YY_END_OF_BUFFER_CHAR;
    yy_ch_buf[BufSize + 1]  = YY_END_OF_BUFFER_CHAR;

    yy_fill_buffer = 0;         // Because buffer is ready here it needs not to be filled again later on
    yy_n_chars     = BufSize;   // ATTENTION: variable is size without EOB chars
    yy_input_file  = NULL;      // no more file or stream no fun (no more read from yy_input_file)

    init( yy_input_stream );
}
// yy_buffer_state()



/*
  -----------------------------------------------------------------------------
  function:     ~yy_buffer_state - destructor
  -----------------------------------------------------------------------------
*/
yy_buffer_state::~yy_buffer_state()
{
    delete yy_ch_buf;
}



/*
  -----------------------------------------------------------------------------
  function:     init
  -----------------------------------------------------------------------------
*/
void yy_buffer_state::init( char *InputStream )
{
    yy_buf_pos       = &yy_ch_buf[0];
    yy_buffer_status = YY_BUFFER_NORMAL;
    yy_at_bol        = 1;
}


