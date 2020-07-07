/*! 
  -----------------------------------------------------------------------------
  module: hls17_BufPars.h
  -----------------------------------------------------------------------------
  responsible:  SteffenS and TimW

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

#ifndef HLS17_BUFPARS_H
#define HLS17_BUFPARS_H

/*!
  -----------------------------------------------------------------------------
 
  class:        yy_buffer_state

  description:  Flex used internal class for statement buffering

  -----------------------------------------------------------------------------
*/
class yy_buffer_state
{

public:
    /*!---------------------------------------------------------------------------
        Declaration:  Public types
        Description:  Public types used by public member functions.
    */

    enum BufferStati                // current status
    {
        YY_BUFFER_NEW           = 0,
        YY_BUFFER_NORMAL        = 1,
        YY_BUFFER_EOF_PENDING   = 2
    } yy_buffer_status;

    char        *yy_input_file;
      
    char        *yy_ch_buf;     // input buffer; size is defined in constructor
    char        *yy_buf_pos;    // current position in input buffer
    yy_size_t   yy_buf_size;    // Size of input buffer in bytes, not including room for EOB characters
   
    int yy_n_chars;             // Number of characters read into yy_ch_buf, not including EOB characters.

    int yy_fill_buffer;         // Whether to try to fill the input buffer when we reach the end of it

    /* Whether we're considered to be at the beginning of a line.
     * If so, '^' rules will be active on the next match, otherwise not.
     */
    int yy_at_bol;

  
    /*! EndDeclaration:  Public types */


    /*!
      -----------------------------------------------------------------------------
      function:     yy_buffer_state - constructor
      -----------------------------------------------------------------------------
      description:  create a new buffer object

      arguments:    yy_input_stream  [IN]  - input stream to buffer
                    BufSize          [IN]  - requested buffer size

      returnvalue:  None
      -----------------------------------------------------------------------------
    */
    yy_buffer_state( char *yy_input_stream, int BufSize );

    /*!
      -----------------------------------------------------------------------------
      function:     ~yy_buffer_state - destructor
      -----------------------------------------------------------------------------
      description:  deletebuffer object

      arguments:    None

      returnvalue:  None
      -----------------------------------------------------------------------------
    */
    ~yy_buffer_state();

    
    /*!
      -----------------------------------------------------------------------------
      function:     init
      -----------------------------------------------------------------------------
      description:  Initializes some buffer object variables.

                    Function is called from constructor where the buffer will be
                    filled already. So the status of buffer object is 'NORMAL' and
                    not 'NEW'. This prevents filling the buffer newly in yylex.

      arguments:    InputStream [IN] - input stream

      returnvalue:  None
      -----------------------------------------------------------------------------
    */
    void init( char *InputStream );


}; // yy_buffer_state


#endif  //HLS17_BUFPARS_H
