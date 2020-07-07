/*!
  \file    DBMSrvCmdTrc_TraceProt.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command trace_prot

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "hcn40.h"
#include "SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_TraceProt.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdTrc_TraceProt
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdTrc_TraceProt::DBMSrvCmdTrc_TraceProt
//-----------------------------------------------------------------------------
DBMSrvCmdTrc_TraceProt::DBMSrvCmdTrc_TraceProt()
    : DBMSrv_Command( DBMSrv_Command::KeyTraceProt, false, DBMSrvCmdTrc_TraceProt::m_LongHelp ) 
{
} // end DBMSrvCmdTrc_TraceProt::DBMSrvCmdTrc_TraceProt

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdTrc_TraceProt::m_LongHelp
//-----------------------------------------------------------------------------
const char * const DBMSrvCmdTrc_TraceProt::m_LongHelp=
"@command trace_prot You transfer the binary version of the database trace to the "
                    "text version. You can use all values and combinations of values "
                    "for option command options that you received as a reply when "
                    "displaying the option for the text version of the database trace."
                    "The binary version of the database trace has the file name "
                    "knltrace and is stored in the run directory of the database "
                    "instance."

"@preconditions You have the server authorization DBInfoRead or DBFileRead<NL>"
                "You have generated the database trace.<NL>"
                "You have determined the file ID of the database trace file:"

"@syntax trace_prot <optionlist>"

"@param  <optionlist> The optionlist contains letters which restrict the trace"
                      "output of the internal database components.<NL>"
                      "a - The trace output of the internal database component order interface is copied to the text file<NL>"
                      "b - The trace output of the internal database component record interface is copied to the text file<NL>"
                      "k - The trace output of the internal database component table interface is copied to the text file<NL>"
                      "m - The internal data structure with the name message block is transferred to the text file<NL>"
                      "e - The numbers of the trace page and the buffer's position in the page are copied to the text file for the order interface and the message block<NL>"
                      "s - Information about the processing strategy is copied to the text file<NL>"
                      "t - The results of the internal time measurement are copied to the text file<NL>"
                      "x - Additional trace output is copied to the text file"
"@reply OK<NL>"
      "<output>"

"@replyvalue <output>  output of the tool generating the text version of the trace";

//-----------------------------------------------------------------------------
// public static member function DBMSrvCmdTrc_TraceProt::runCommand
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceProt::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
  DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
  DBMSrvCmdTrc_TraceProt oCommand;

  return oCommand.run(vcontrol, command, oReply);
} // end if
//-----------------------------------------------------------------------------
// public member function DBMSrvCmdTrc_TraceProt::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdTrc_TraceProt::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply) 
{
  tcn00_Error             nFuncReturn = OK_CN00; 

  nFuncReturn =  cn40ExecXKernprot(vcontrol, command, Reply.giveData(), Reply.giveLength(), Reply.giveMaximalLength());

  Reply.adaptLength();

  return nFuncReturn;
} // end DBMSrvCmdTrc_TraceProt::run
