/*!========================================================================
  @ingroup      Sec
  @file         RTESec_SCRAMTest.c
  @author       ChristophB

  @brief

  @since        2004-08-18  16:34
  @sa

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/

#include "RunTime/Security/RTESec_SCRAMMD5.h"
#include "RunTime/Security/RTESec_HMACMD5.h"
#include "RunTime/Security/RTESec_MD5.h"
#include "RunTime/Security/RTESec_MCF.h"

void hexdump (const void *buff, int want) {
	int i;

	for (i = 0; i < want; i++)
		printf ("%02x ", ((unsigned char *)buff)[i]);
}

int genverifier () {
	char buff[1024];
	char salt_ii[1024];
	char *ptr;
	int size;

	SCRAMMD5_VRFY veri[1];
	char salt[SCRAMMD5_SALTSIZE];
	char pass[] = "secret";
	int passlen;

	passlen = strlen (pass);
	memset (salt, 0, SCRAMMD5_SALTSIZE);

	printf ("salt: ");
	hexdump (salt, SCRAMMD5_SALTSIZE);
	printf ("\n");

	printf ("pass: %s\n", pass);

	SCRAMMD5GenVerifier (veri, (unsigned char *) salt, pass, passlen, 0);

	memset (buff, 0, 1024);
	strcat (buff, "$1$");
	MCFEnc (veri->salt, buff + strlen (buff), SCRAMMD5_SALTSIZE);
	strcat (buff, "$");
	MCFEnc (veri->clidata, buff + strlen (buff), SCRAMMD5_DATASIZE);
	strcat (buff, "$");

	printf ("cliv: ");
	hexdump (veri->clidata, SCRAMMD5_DATASIZE);
	printf ("\n");

	printf ("veri: %s\n", buff);
	
	size = MCFDec (&buff[3], salt_ii, SCRAMMD5_SALTSIZE);
	printf ("salt: ");
	hexdump (salt_ii, size);
	printf ("\n");

	ptr = strchr (&buff[4], '$');
	size = MCFDec (ptr + 1, salt_ii, SCRAMMD5_DATASIZE);
	printf ("cliv: ");
	hexdump (salt_ii, size);
	printf ("\n");

	return 0;
}

int doit () {
	char client_challenge[] = "eine UserId und eine Zufallszahl";
	char server_challenge[1024] = "der Salt-und eine andere Zufallszahl";
	int client_challenge_len;
	int server_challenge_len;
	char server_proof[1024];
	char pass[] = "secret";
        int pwdlen = strlen (pass);
	char salt[SCRAMMD5_SALTSIZE];
	SCRAMMD5_VRFY veri[1];
	SCRAMMD5_CLIENT client_data[1];
	int rc;

	memset (salt, 0, SCRAMMD5_SALTSIZE);
	client_challenge_len = strlen (client_challenge) + 1;
	server_challenge_len = strlen (server_challenge) + 1;

	rc = SCRAMMD5GenClient (
            client_data,
            client_challenge, client_challenge_len,
            salt, SCRAMMD5_SALTSIZE,
            server_challenge, server_challenge_len,
            pass, pwdlen, (unsigned char *) server_proof);

	printf ("client:\n");

	printf ("client proof :");
	hexdump (client_data->cproof, SCRAMMD5_DATASIZE);
	printf ("\n");

	printf ("server proof :");
	hexdump (server_proof, SCRAMMD5_DATASIZE);
	printf ("\n");
	
	printf ("server:\n");

	memset (salt, 0, SCRAMMD5_SALTSIZE);
	memcpy (salt, "der Salt-Value", SCRAMMD5_SALTSIZE);

	printf ("salt: ");
	hexdump (salt, SCRAMMD5_SALTSIZE);
	printf ("\n");

	SCRAMMD5GenVerifier (veri, (unsigned char *) salt, pass, pwdlen, 0);

	printf ("client verifier: ");
	hexdump (veri->clidata, SCRAMMD5_DATASIZE);
	printf ("\n");

	rc = SCRAMMD5Gen (
            client_data,
            client_challenge, client_challenge_len,
            salt, SCRAMMD5_SALTSIZE,
            server_challenge, server_challenge_len,
            (const char * )veri, sizeof (SCRAMMD5_VRFY), SCRAM_VERIFY,
            (unsigned char *) server_proof);

	printf ("verified: %d\n", rc);

	printf ("server proof :");
	hexdump (server_proof, SCRAMMD5_DATASIZE);
	printf ("\n");

	return 0;
}

int md5test () {
	struct MD5Context context;
	unsigned char digest[16];
	char buffer[1024];
	
	memset (buffer, 0, 1024);
	strcpy (buffer, "secret");

	RTESec_MD5Init (&context);
	RTESec_MD5Update (&context, (MD5_UINT1 *) buffer, 6);
	RTESec_MD5Final (digest, &context);

	printf ("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
	digest[0], digest[1], digest[2], digest[3],
	digest[4], digest[5], digest[6], digest[7],
	digest[8], digest[9], digest[10], digest[11],
	digest[12], digest[13], digest[14], digest[15]);

	return 0;
}

int main () {
#if 0
	md5test ();
#endif
#if 0
	genverifier ();
#endif
#if 1
	doit ();	
#endif

	return 0;
}

