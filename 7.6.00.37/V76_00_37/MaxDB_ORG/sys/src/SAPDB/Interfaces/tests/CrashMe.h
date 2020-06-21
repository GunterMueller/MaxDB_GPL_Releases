/*!
  @file           CrashMe.h
  @author         D025086
  @ingroup        Tests
  @brief          Stress test utility.
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#ifndef IFR_CRASH_ME_H
#define IFR_CRASH_ME_H

class IFR_Connection;
class CrashMe_Check;
/**
 * @brief Stress test utility. Throws away database connections
 * without attaching the interface runtime or shutting down the 
 * database.
 * With this class it is also possible to test if a connection is astablished
 * and the databe is online.
 */

class CrashMe {
public:
  /**
   * @brief Constructor. 
   * @param connection [in] The connection object to control by this class.
   */
  CrashMe(IFR_Connection *connection);

    ~CrashMe();  
    
  /**
   * @brief Terminates the session immediatly by calling sqlarelease directly
   * no code of the interface runtime is attached by this class.
   */
  void BreakConnection();

  /**
   * @brief Reconnects the session to the given database.
   */
  IFR_Connection *Reconnect(char *hostname, char *database, char *username, char *password);

  /**
   * @brief Checks whether the connection is up an running
   * by sending a 'select timestamp into ? from dual' to the database.
   * If the value of the timestamp has been increased the session is active
   * @return <code>true</code> on success, <code>false</code> otherwise.
   */
  bool CheckConnection();
  
private:
  CrashMe_Check *m_chk;
};
#endif
