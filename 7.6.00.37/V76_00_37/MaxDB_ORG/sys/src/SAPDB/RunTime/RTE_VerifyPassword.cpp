/*!
  @file           RTE_VerifyPassword.cpp
  @author         JoergM
  @brief          DBM RunTime: User Authentification

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

#include "RunTime/RTE_UNIXAuthenticate.hpp"
#include "RunTime/RTE_saveUNIXcalls.h"
#include "Messages/Msg_List.hpp"

#include <unistd.h>
#include <grp.h>
#ifndef AIX
/* on AIX this produces a confilct with stdlib.h definition of setkey.... */
#include <crypt.h>
#endif /* AIX */
#include <pwd.h>

#if defined(HPUX) || defined(SUN) || defined(SOLARIS) || defined(LINUX) || defined(NMP)
#include <shadow.h>
#endif /* SUN | SOLARIS | LINUX | NMP */

#if defined(AIX)
#include <userpw.h>
#endif /* AIX */

#if defined(OSF1)
#include <sys/types.h>
#include <sys/security.h>
#include <prot.h>
#endif /* OSF1 */

/*----------------------------------------*/
    #undef MF__
    #define MF__ MOD__"FindCryptedPassword"
static const char *FindCryptedPassword(const char *Username, const struct passwd *pwEntry)
{
    const char *FoundCryptedPassword = pwEntry->pw_passwd;

    if ( FoundCryptedPassword
     && (*FoundCryptedPassword) == '\0' )
    {
        return FoundCryptedPassword;
    }

    if ( !FoundCryptedPassword
      || !FoundCryptedPassword[0]
      || !FoundCryptedPassword[1] ) /* check minimum length = 2, core dump free version of strlen() < 2 ;-) */
    {
        char srchUsername[20];
        strcpy(srchUsername, Username);
        {
#if defined(AIX)
            struct userpw *spwdEntry = getuserpw (srchUsername);
            if (spwdEntry != NULL) 
            {
                FoundCryptedPassword = spwdEntry->upw_passwd;
            }
#elif defined(OSF1)
            struct pr_passwd *spwdEntry = getprpwnam(srchUsername);
            if ( spwdEntry != NULL )
            {
                FoundCryptedPassword = spwdEntry->ufld.fd_encrypt;
            }
#else
            struct spwd *spwdEntry = getspnam (srchUsername);
            if (spwdEntry != NULL) 
            {
                FoundCryptedPassword = spwdEntry->sp_pwdp;
            }
#endif
        }

        if ( !FoundCryptedPassword
          || !FoundCryptedPassword[0]
          || !FoundCryptedPassword[1] ) /* check minimum length = 2, core dump free version of strlen() < 2 ;-) */
        {
            FoundCryptedPassword = "?";
        }
    }

    return FoundCryptedPassword;
}

/*---------------------------------------*/

int main(int argc, char *argv[])
{
    /*
      First check that caller is really 'SAPDB_Owner' and no one else...
     */
    SAPDB_Int4 userId;
    SAPDB_Int4 groupId;
    Msg_List   errList;

#ifdef SDB_PASSWORD_TEST
    printf("%s compiled for password check\n", argv[0]);
#endif

#if defined(OSF1)
    /* Manual page forces this call to be first in main() */
    set_auth_parameters(argc, argv);
#endif

    if ( RTE_GetDefaultSapdbUserAndGroupIds( userId,
                                             groupId,
                                             errList) )
    {
        if ( RTE_save_getuid() != 0 && RTE_save_getuid() != userId )
        {
            printf("Authorization failed\n");
            return -1;
        }
    }
    else
    {
        printf("Failed to get SDB_OWNER and/or SDB_GROUP entry\n");
        return -1;
    }

    /*
      Now we check arguments
     */
    RTE_FileHandle inputPipe = 0;
    RTE_FileHandle outputPipe = 0;
    SAPDB_Char *username;
    SAPDB_Char seedBuffer[16];
    memset(seedBuffer, 0, 16);

#ifdef SDB_PASSWORD_TEST
    if ( argc != 3 )
    {
        printf("Syntax: dbmverifypw username password\n");
        printf("Usage: Verifies the password of given username\n");
        return (1);
    }
#else
    if ( argc != 4 )
    {
        printf("Syntax: dbmverifypw username pipein pipeout\n");
        printf("Usage: Verifies the password of given username\n");
        return (1);
    }
#endif

    username = argv[1];
    SAPDB_Char *pNumber;

    struct passwd *pwEntry = getpwnam (username);
    if (pwEntry == NULL) 
    {
        printf("User '%s' unknown\n", username);
        return 2;
    }

    const char *cryptedPassword = FindCryptedPassword(username, pwEntry);
    if ( !cryptedPassword
      || !cryptedPassword[0]
      || !cryptedPassword[1] ) /* check for minimum length = 2 */
    {
        cryptedPassword = 0;
        seedBuffer[0] = 'X';
        seedBuffer[1] = 'X';
        seedBuffer[2] = 0;
    }
    else
    {
        if ( cryptedPassword[0] != '$' )
        {
            seedBuffer[0] = cryptedPassword[0];
            seedBuffer[1] = cryptedPassword[1];
            seedBuffer[2] = 0;
        }
        else
        {
            SAPDB_Int4 iSalt = 0;

            /* copy salt identifier up to limiting '$' */
            do {
                seedBuffer[iSalt] = cryptedPassword[iSalt];
                ++iSalt;
            } while ( cryptedPassword[iSalt] != '$' && iSalt < (sizeof(seedBuffer)-1) );

            /* copy salt key up to limiting '$' */
            do {
                seedBuffer[iSalt] = cryptedPassword[iSalt];
                ++iSalt;
            } while ( cryptedPassword[iSalt] != '$' && iSalt < (sizeof(seedBuffer)-1) );

            /* add the optional limiting '$' */
            if ( iSalt < (sizeof(seedBuffer)-2) )
            {
                seedBuffer[iSalt++] = '$';
            }
            seedBuffer[iSalt] = 0;
        }
    }

#ifdef SDB_PASSWORD_TEST
    char *cryptBuffer = crypt(argv[2], &seedBuffer[0]);

    if ( !cryptBuffer )
    {
        printf("Got no crypted password\n");
        return 1;
    }
    else
    {
        printf("CryptedPassword %s\n", cryptBuffer);
    }
#else
    pNumber = argv[2];
    while ( *pNumber >= '0' && *pNumber <= '9' )
    {
        inputPipe *= 10;
        inputPipe += (*pNumber) - '0';
        ++pNumber;
    }

    pNumber = argv[3];
    while ( *pNumber >= '0' && *pNumber <= '9' )
    {
        outputPipe *= 10;
        outputPipe += (*pNumber) - '0';
        ++pNumber;
    }

    if ( write(outputPipe, seedBuffer, strlen(seedBuffer)) != strlen(seedBuffer) )
    {
        printf("Writing seed failed\n");
        return 4;
    }

    char cryptBuffer[64];
    int bytesIn = 0;

    memset(cryptBuffer, 0, sizeof(cryptBuffer));

    while ( bytesIn < (sizeof(cryptBuffer)-1) )
    {
        int bytesRead = (int)read(inputPipe, cryptBuffer + bytesIn, (sizeof(cryptBuffer)-1) - bytesIn);
        if ( bytesRead < 0 )
        {
            printf("Read error\n");
            return 5;
        }
        if ( bytesRead == 0 )
        {
            break;
        }
        else
        {
            bytesIn += bytesRead;
        }
    }
    cryptBuffer[bytesIn] = 0;

    if ( !cryptedPassword ) /* delayed to prevent ugly messages... */
    {
        printf("Password not found\n");
        sleep(1); /* slows out brute force attack */
        return 3;
    }
#endif

    if ( strncmp(cryptedPassword, cryptBuffer, strlen(cryptBuffer)) != 0 )
    {
        printf("Password mismatch\n");
        sleep(1); /* slows out brute force attack */
        return 5;
    }
#ifdef SDB_PASSWORD_TEST
    printf("Password matched\n");
#endif

    return 0;
}
