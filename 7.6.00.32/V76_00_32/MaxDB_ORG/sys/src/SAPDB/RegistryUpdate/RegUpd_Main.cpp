/*!**********************************************************************

  module: RegUpd_Main.cpp

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: Registry

  description:  This component updates registry / ini file entries
                in a managed way.
                http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1122610

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2002-2005 SAP AG







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





*/

/***********************************************************************

	Includes

 ***********************************************************************/

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <time.h>

#ifdef WIN32
    #include <direct.h>
#else
    #include <unistd.h>
#endif

#include "heo06.h"
#include "heo670.h"

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"
#include "SAPDB/Registry/Reg_Registry.h"
#include "SAPDB/RegistryUpdate/RegUpd_Log.h"

#include "geo572.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define WITH_TRACE              0
#if WITH_TRACE
    #define WriteTraceMethod(a) printf("\n\n%s\n",a)

    #define WriteTrace(a)       printf(a)
    #define WriteTrace1(a,b)    printf(a,b)
    #define WriteTrace2(a,b,c)  printf(a,b,c)
#else
    #define WriteTraceMethod(a)

    #define WriteTrace(a)
    #define WriteTrace1(a,b)
    #define WriteTrace2(a,b,c)
#endif

/*---------------------------------------------------------------------------*/

#define OPTERRCOLON (1)
#define OPTERRNF (2)
#define OPTERRARG (3)

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef struct st_regupd_argument   RegUpd_Argument;

struct st_regupd_argument {
    char             argName[MAX_ARGUMENT_NAME_LEN];
    char             argValue[MAX_ARGUMENT_VALUE_LEN];
    RegUpd_Argument *nextArgument;
};

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool UpgradeRegistry( char            *sourceFile,
                            char            *destFile,
                            RegUpd_Argument *argumentList,
                            SAPDB_Bool       withRegistryCheck,
                            SAPDB_Bool       sourceIsRegistry );

SAPDB_Bool UpgradeSections( Reg_RegistryP    sourceRegistry,
                            Reg_RegistryP    destRegistry,
                            RegUpd_Argument *argumentList,
                            Reg_SectionNodeP sectionNode,
                            SAPDB_Bool       sourceIsRegistry );

SAPDB_Bool UpgradeKey( Reg_RegistryP     destRegistry,
                       RegUpd_Argument  *argumentList,
                       char             *sectionName,
                       Reg_KeyNodeP      keys );

SAPDB_Bool GetValue( RegUpd_Argument    *argumentList,
                     char               *srcValue,
                     char               *value );
void PrintHelp();

int GetOpt( int           argc,
		    char* const  *argv,
			const char   *optstr,
			char        **optarg );

int OptIsErr( int           argc, 
              char * const *argv,
              int           oint,
              const char   *optstr,
              int           optchr,
              int           err    );

SAPDB_Bool  AddNewArgument( char              *argLine,
                            RegUpd_Argument  **argumentList );

void  FreeArgumentList( RegUpd_Argument  *argumentList );

/***********************************************************************

	Implementation

 ***********************************************************************/

int     optind = 1;
int     opterr = 1;
int     optopt;

int main(int argc, char *argv[])
{

    int              i = 0;
    char             errorText[MAX_ERR_TEXT_LEN] = "";
    char             sourceFile[MAX_FILENAME_LEN] = "";
    char             destFile[MAX_FILENAME_LEN] = "";
    RegUpd_Argument *argumentList = NULL;
    SAPDB_Bool       withRegistryCheck = SAPDB_TRUE;
    SAPDB_Bool       sourceIsRegistry = SAPDB_FALSE;
    int              argPtr = 0;

    WriteTraceMethod("main\n");

#ifndef WIN32
    /* Set signal handler */
    SetSignalHandler();
#endif

    REGUPD_TRY {
	    /* Get current directory	*/
	    if ( !GetExecDirectory( argv[0], MAX_FILENAME_LEN ) ) {
		    exit( EXIT_FAILURE );
	    }

	    /* Check command line arguments	*/
        argPtr = 1;
        if (argv[argPtr]) {
#ifdef WIN32
            if (strcmp(argv[argPtr], "x") == 0) {
                withRegistryCheck = SAPDB_FALSE;
                argPtr++;
            }

            if (argv[argPtr] && strcmp(argv[argPtr], "-r") == 0) {
                sourceIsRegistry = SAPDB_TRUE;
                argPtr++;
            }
#endif
        } else {
			PrintHelp();

		    exit( EXIT_FAILURE );
        }

        if (argc < argPtr + 2) {
            printf("REGUPD[Error]: Too few parameters!\n");
            printf("REGUPD[Info]:  Call regupd to get options info.\n");

		    exit( EXIT_FAILURE );
        } else {
			/* get source file */
            if (argv[argPtr]) {
                strncpy(sourceFile, argv[argPtr], MAX_FILENAME_LEN);
                WriteTrace1("Source file: %s\n", sourceFile);
            }

			/* get destination registry */
            if (argv[argPtr+1]) {
                strncpy(destFile, argv[argPtr+1], MAX_FILENAME_LEN);
                WriteTrace1("Dest file: %s\n", destFile);
            }

            for (i = argPtr + 2; i < argc; i++) {
                if (!AddNewArgument(argv[i], &argumentList)) {
        		    exit( EXIT_FAILURE );
                }
            }
	    }

        if (!UpgradeRegistry(sourceFile, destFile, argumentList, withRegistryCheck, sourceIsRegistry)) {
            FreeArgumentList(argumentList);
		    exit( EXIT_FAILURE );
        }

        FreeArgumentList(argumentList);
    } REGUPD_CATCH;

	return EXIT_SUCCESS;

}

/*===========================================================================*/

SAPDB_Bool UpgradeRegistry( char            *sourceFile,
                            char            *destFile,
                            RegUpd_Argument *argumentList,
                            SAPDB_Bool       withRegistryCheck,
                            SAPDB_Bool       sourceIsRegistry )
{

    Reg_RegistryP    sourceRegistry = NULL;
    Reg_RegistryP    destRegistry = NULL;
    RegUpd_Argument *currentArg = argumentList;

    WriteTraceMethod("UpgradeRegistry>>\n");

    while(currentArg) {
        WriteTrace2("VAR: %s\t%s\n", currentArg->argName, currentArg->argValue);
        currentArg = currentArg->nextArgument;
    }

    WriteTrace("UpgradeRegistry::Reg_OpenRegistry>>\n");
    if (sourceIsRegistry == SAPDB_FALSE) {
        WriteTrace("UpgradeRegistry::Reg_OpenRegistryFile>>\n");
        if (!Reg_OpenRegistryFile(&sourceRegistry, sourceFile)) {
            printf("REGUPD[Error]: Open source failed!\n");
            printf("REGUPD[Info]: %s\n", sourceFile);

            return SAPDB_FALSE;
        }
    } else {
        WriteTrace("UpgradeRegistry::Reg_OpenRegistry>>\n");
        if (!Reg_OpenRegistry(&sourceRegistry, sourceFile)) {
            printf("REGUPD[Error]: Open source failed!\n");
            printf("REGUPD[Info]: %s\n", sourceFile);

            return SAPDB_FALSE;
        }
    }

    WriteTrace("UpgradeRegistry::Reg_OpenRegistryForUpdate>>\n");
    if (!Reg_OpenRegistryForUpdate(&destRegistry, destFile)) {
        Reg_CloseRegistry(sourceRegistry);

        printf("REGUPD[Error]: Open destination failed!\n");
        printf("REGUPD[Info]: %s\n", destFile);


        return SAPDB_FALSE;
    }

    WriteTrace("UpgradeRegistry::UpgradeSections>>\n");
    if (!UpgradeSections(sourceRegistry, destRegistry, argumentList, sourceRegistry->sections, sourceIsRegistry)) {
        Reg_CloseRegistry(sourceRegistry);
        Reg_CloseRegistry(destRegistry);

        printf("REGUPD[Error]: Upgrade sections failed!\n");
        printf("REGUPD[Info]: %s\n", sourceFile);
        printf("REGUPD[Info]: %s\n", destFile);

        return SAPDB_FALSE;
    }

    WriteTrace("UpgradeRegistry::Reg_WriteRegistry>>\n");
    if (!Reg_WriteRegistry(destRegistry, withRegistryCheck)) {
        Reg_CloseRegistry(sourceRegistry);
        Reg_CloseRegistry(destRegistry);

        printf("REGUPD[Error]: Write registry failed!\n");
        printf("REGUPD[Info]: %s\n", sourceFile);
        printf("REGUPD[Info]: %s\n", destFile);

        return SAPDB_FALSE;
    }

    WriteTrace("UpgradeRegistry::Reg_CloseRegistry>>\n");
    Reg_CloseRegistry(sourceRegistry);
    Reg_CloseRegistry(destRegistry);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool UpgradeSections( Reg_RegistryP    sourceRegistry,
                            Reg_RegistryP    destRegistry,
                            RegUpd_Argument *argumentList,
                            Reg_SectionNodeP sectionNode,
                            SAPDB_Bool       sourceIsRegistry )
{

    char    *sectionName = NULL;
    char     destSectionName[REG_MAX_PATH_LEN+1] = "";
    size_t   sourceRegistryFileLen = strlen(sourceRegistry->file);
    size_t   destRegistryFileLen = strlen(destRegistry->file);

    while (sectionNode) {
        if (!sectionNode->section || !sectionNode->section->sectionName) {
            return SAPDB_TRUE;
        }
        sectionName = sectionNode->section->sectionPath;

        if (sourceIsRegistry == SAPDB_TRUE && strlen(sectionName) > sourceRegistryFileLen && sectionNode->section->keys) {
            sectionName = sectionNode->section->sectionPath + sourceRegistryFileLen;
            if (destRegistryFileLen > 0 && destRegistry->file[destRegistryFileLen-1] == '\\') {
                if (sectionName[0] == '\\') {
                    sp77sprintf(destSectionName, REG_MAX_PATH_LEN, "%s%s", destRegistry->file, sectionName + 1);
                } else {
                    sp77sprintf(destSectionName, REG_MAX_PATH_LEN, "%s%s", destRegistry->file, sectionName);
                }
            } else {
                if (sectionName[0] == '\\') {
                    sp77sprintf(destSectionName, REG_MAX_PATH_LEN, "%s%s", destRegistry->file, sectionName);
                } else {
                    sp77sprintf(destSectionName, REG_MAX_PATH_LEN, "%s\\%s", destRegistry->file, sectionName);
                }
            }

            if (!UpgradeKey(destRegistry, argumentList, destSectionName, sectionNode->section->keys)) {
                return SAPDB_FALSE;
            }
        } else {
            if (sectionNode->section->keys) {
                if (!UpgradeKey(destRegistry, argumentList, sectionName, sectionNode->section->keys)) {
                    return SAPDB_FALSE;
                }
            }
        }

        if (sectionNode->section->childSections) {
            if (!UpgradeSections(sourceRegistry, destRegistry, argumentList, sectionNode->section->childSections, sourceIsRegistry)) {
                return SAPDB_FALSE;
            }
        }

        sectionNode = sectionNode->nextSectionNode;
    };

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool UpgradeKey( Reg_RegistryP     destRegistry,
                       RegUpd_Argument  *argumentList,
                       char             *sectionName,
                       Reg_KeyNodeP      keys )
{

    Reg_KeyNodeP     currentKey = keys; 
    char            *keyName = NULL;
    char             value[MAX_VALUE_LEN];
    SAPDB_Bool       found;

    while (currentKey) {
        if (currentKey->key) {
            keyName = currentKey->key->key;

            if (!Reg_KeyExists(destRegistry, sectionName, keyName, &found)) {
                return SAPDB_FALSE;
            }

            if (found == SAPDB_FALSE) {
                GetValue(argumentList, currentKey->key->value, value);
                if (!Reg_SetRegistryKeyInMemory(destRegistry, sectionName, keyName, value)) {
                    return SAPDB_FALSE;
                }
            }
        }

        currentKey = currentKey->nextKeyNode;
    };

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool GetValue( RegUpd_Argument    *argumentList,
                     char               *srcValue,
                     char               *value )
{

    RegUpd_Argument *arg = argumentList;
    char            *varPosStart;
    char            *varPosEnd;
    char             var[MAX_VALUE_LEN];
    size_t           srcValueLen;

    value[0] = '\0';

    if (!arg) {
        sp77sprintf(value, MAX_VALUE_LEN, "%s", srcValue);

        return SAPDB_TRUE;
    }

    srcValueLen = strlen(srcValue);
    varPosStart = strchr(srcValue, '$');
    if (!varPosStart || (size_t)(varPosStart - srcValue) == srcValueLen) {
        sp77sprintf(value, MAX_VALUE_LEN, "%s", srcValue);

        return SAPDB_TRUE;
    }

    varPosEnd = strchr(varPosStart + 1, '$');
    if (!varPosEnd) {
        sp77sprintf(value, MAX_VALUE_LEN, "%s", srcValue);

        return SAPDB_TRUE;
    }

    strncpy(var, varPosStart + 1, varPosEnd - varPosStart - 1);
    var[varPosEnd - varPosStart - 1] = '\0';

    while(arg) {
        if (strcasecmp(arg->argName, var) == 0) {
            sp77sprintf(value, MAX_VALUE_LEN, "%s%s", arg->argValue, varPosEnd + 1);

            return SAPDB_TRUE;
        }

        arg = arg->nextArgument;
    }

    return SAPDB_TRUE;
}

/*===========================================================================*/

void PrintHelp()
{

#ifdef WIN32
	printf("Usage: regupd [-x] [-r] <source registry> <destination registry>\n\n");

	printf("Options:\n");
	printf("  -x      : no check of destination registry (use it at your own risk!)\n");
	printf("            without this option destination registry path will be\n");
	printf("            compared with \\SOFTWARE\\SAP\\SAP DBTech at HKEY_LOCAL_MACHINE\n");
	printf("  -r      : source is not file, source is a registry path\n");
#else
	printf("Usage: regupd <source registry> <destination registry>\n\n");
#endif

}

/*===========================================================================*/

int GetOpt( int           argc,
		    char* const  *argv,
			const char   *optstr,
			char        **optarg )
{

    static int optchr = 0;
    static int dash = 0; /* have already seen the - */

    char *cp;

    if (optind >= argc)
        return(EOF);

    if (!dash && (argv[optind][0] !=  '-'))
        return(EOF);

    if (!dash && (argv[optind][0] ==  '-') && !argv[optind][1]) {
        /*
         * use to specify stdin. Need to let pgm process this and
         * the following args
         */
        return(EOF);
    }

    if ((argv[optind][0] == '-') && (argv[optind][1] == '-')) {
        /* -- indicates end of args */
        optind++;
        return(EOF);
    }

    if (!dash) {
        assert((argv[optind][0] == '-') && argv[optind][1]);
        dash = 1;
        optchr = 1;
    }

    /* Check if the guy tries to do a -: kind of flag */
    assert(dash);
    if (argv[optind][optchr] == ':') {
        dash = 0;
        optind++;
        return(OptIsErr(argc, argv, optind-1, optstr, optchr, OPTERRCOLON));
    }

    if (!(cp = (char*)strchr(optstr, argv[optind][optchr]))) {
        int errind = optind;
        int errchr = optchr;

        if (!argv[optind][optchr+1]) {
            dash = 0;
            optind++;
        }
        else
            optchr++;
        return(OptIsErr(argc, argv, errind, optstr, errchr, OPTERRNF));
    }

    if (cp[1] == ':') {
        dash = 0;
        optind++;

        if(optind == argc)
            return(OptIsErr(argc, argv, optind-1, optstr, optchr, OPTERRARG));
        *optarg = argv[optind++];
        return(*cp);
    } else {
        if (!argv[optind][optchr+1]) {
            dash = 0;
            optind++;
        }
        else
            optchr++;
        return(*cp);
    }
/* Statement will be never reached MaO
    assert(0);
    return(0);
*/
}

/*===========================================================================*/

int OptIsErr( int           argc, 
              char * const *argv,
              int           oint,
              const char   *optstr,
              int           optchr,
              int           err    )
{

    if (opterr) {
        fprintf(stderr, "Error in argument %d, char %d: ", oint, optchr + 1);

        switch (err) {
        case OPTERRCOLON:
            fprintf(stderr, ": in flags\n");
            break;
        case OPTERRNF:
            fprintf(stderr, "option not found %c\n", argv[oint][optchr]);
            break;
        case OPTERRARG:
            fprintf(stderr, "no argument for option %c\n", argv[oint][optchr]);
            break;
        default:
            fprintf(stderr, "unknown\n");
            break;
        }
    }

    optopt = argv[oint][optchr];
    return('?');

}

/*===========================================================================*/

SAPDB_Bool  AddNewArgument( char              *argLine,
                            RegUpd_Argument  **argumentList )
{

    char            *value = NULL;
    tsp00_Bool       allocatOk = SAPDB_FALSE;
    RegUpd_Argument *lastArgument = NULL;
    RegUpd_Argument *newArgument = NULL;
    size_t           argNameLen = 0;

    WriteTraceMethod("AddNewArgument\n");

    if (!argumentList || !argLine) {
        return SAPDB_FALSE;
    }

    if (*argumentList) {
        lastArgument = *argumentList;
        while(lastArgument->nextArgument) {
            lastArgument = lastArgument->nextArgument;
        }
    }

    value = strchr(argLine, '=');
    if (value) {
        sqlallocat(sizeof(struct st_regupd_argument), (SAPDB_UInt1**) &newArgument, &allocatOk);
        if (!allocatOk) {
            return SAPDB_FALSE;
        }

        argNameLen = (value-argLine) >= MAX_ARGUMENT_NAME_LEN ? MAX_ARGUMENT_NAME_LEN : (value-argLine);

        strncpy(newArgument->argName, argLine, argNameLen);
        newArgument->argName[argNameLen] = '\0';
        WriteTrace1("Argument name: %s\n", newArgument->argName);

        sp77sprintf(newArgument->argValue, MAX_ARGUMENT_VALUE_LEN, "%s", value+1);
        WriteTrace1("Argument value: %s\n", newArgument->argValue);

        newArgument->nextArgument = NULL;

        if (!*argumentList) {
            *argumentList = newArgument;
        } else {
            lastArgument->nextArgument = newArgument;
        }
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

void FreeArgumentList( RegUpd_Argument  *argumentList )
{
    RegUpd_Argument *argumentToDelete = argumentList;

    while(argumentToDelete) {
        argumentList = argumentToDelete->nextArgument;
        sqlfree((SAPDB_UInt1*) argumentToDelete);
        argumentToDelete = argumentList;
    }

}

/***********************************************************************

	End

 ***********************************************************************/
