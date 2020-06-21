/*
 * main.c


    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

 */

/*
 * general perl preprocessor code
 */
#ifndef HAS_BOOL
typedef char bool;
#endif

#ifdef __cplusplus
#define externC extern "C"
#else
#define externC extern
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
#ifdef __cplusplus
}
#endif

#ifndef WIN32
#define UNIX
#endif

#include "script.h"

externC void xs_init (void);

int main (int argc, char **argv, char **env) {
	int PerlArgc;
	char **PerlArgv;
	PerlInterpreter *pPerl;
	int i;
	char EvalBuff[1024];

	/*
	 * build list of parameters for the perl script 
	 */
	PerlArgv = malloc (4 * sizeof (char *));
	if (PerlArgv == 0)
		return 0;

	/*
	 * tell the perl interpreter to eval a string variable,
	 * no external script is used
	 */
	PerlArgv[0] = "";
	PerlArgv[1] = "-e";
	PerlArgv[2] = (char *) sz_text;
	PerlArgc = 3;

	pPerl = perl_alloc ();
	if (pPerl == 0) {
		free (PerlArgv);
		return 0;
	}

	perl_construct (pPerl);
	perl_parse (pPerl, xs_init, PerlArgc, PerlArgv, env);

	/*
	 * call perl sub main with arguments
	 */
	strcpy (EvalBuff, "main (qw (");
	for (i = 1; i < argc; i++) {
		strcat (EvalBuff, argv[i]);
		strcat (EvalBuff, " ");
	}
	strcat (EvalBuff, "));" );
	
	perl_eval_pv (EvalBuff, 1);
	perl_free (pPerl);

	free (PerlArgv);
	return 0;
}

externC XS (boot_SMB);

externC void
xs_init (void) {
	char *file = __FILE__;
	dXSUB_SYS;
	
	newXS ("SMB::bootstrap", boot_SMB, file);
}
