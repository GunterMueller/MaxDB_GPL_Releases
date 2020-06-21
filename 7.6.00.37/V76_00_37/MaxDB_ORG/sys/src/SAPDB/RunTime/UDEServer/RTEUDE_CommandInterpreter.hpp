/*!*********************************************************************

    @file         RTEUDE_CommandInterpreter.hpp

    @author       DanielD

    @brief

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
***********************************************************************/

#ifndef RTEUDE_COMMANDINTERPRETER_HPP
#define RTEUDE_COMMANDINTERPRETER_HPP

class RTEUDE_IPC;
class RTEUDE_CommandReply;
class RTEUDE_TokenIterator;

typedef bool RTEUDE_cmdFunction_F (
        void                 * context,
        RTEUDE_TokenIterator & tokeni,
        RTEUDE_CommandReply  & reply);

/*!
   Command lookup structure
 */
typedef struct RTEUDE_CommandLookup_ {
    const char * cmdName;
    int          cmdLength;
    RTEUDE_cmdFunction_F * cmdFunction;
} RTEUDE_CommandLookup;

/*!

  @class RTEUDE_CommandInterpreter

 */
class RTEUDE_CommandInterpreter
{
public:
    RTEUDE_CommandInterpreter (RTEUDE_CommandLookup * lookup_init);
    ~RTEUDE_CommandInterpreter ();
    bool run (void * context, RTEUDE_IPC * ipc);
    bool runOne (void * context, const char * buf, int buflen,
            RTEUDE_CommandReply & reply);
protected:
    RTEUDE_CommandLookup * lookup;
};

/*!

  @class RTEUDE_CommandReply

 */
class RTEUDE_CommandReply
{
public:
    bool stopLoop;
    bool sendReply;
    int  length;
    char buf [255];
    RTEUDE_CommandReply ();
    bool set (const char * text, int textlen = -1);
    bool error (const char * text, int textlen = -1);
};


/*!

  @class RTEUDE_TokenIterator

 */
class RTEUDE_TokenIterator
{
public:
    RTEUDE_TokenIterator (const char * cmdline, int len);
    ~RTEUDE_TokenIterator ();
    bool next (const char *& token, int & tokenLen);
    void skipSpace ();
    const char * current () const;
    int length () const;

private:
    const char * readpos;
    const char * endpos;
};

#endif
