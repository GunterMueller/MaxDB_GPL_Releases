/*


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



*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/SAPDB_Names.h"

/*
  Show program options
 */
int Usage(char const *programName)
{
      fprintf(stderr, "Syntax: %s [-remove] [-verbose] [-%s|-%s|-%s|-%s|-fFILE -sSECTION] [<name>[=<value>]\n",
                programName
                ,SAPDB_DATABASES_SECTION
                ,SAPDB_INSTALLATIONS_SECTION
                ,SAPDB_RUNTIME_SECTION
                ,SAPDB_GLOBALS_SECTION);
      fprintf(stderr, "Usage: list, request or set global configuration parameter name\n");
      fprintf(stderr, "If no <name> is given section content is listed\n");
      fprintf(stderr, "Example parameters: IndepPrograms,IndepData,SapdbUser,SapdbGroup\n");
      return 1;
}

/*
  Remove leading whitespaces in cString and if wanted also remove first equal sign
 */
char *trimTop(char *cString, int ignoreFirstEqual)
{
    int ignoreEqual = ignoreFirstEqual;
    char *pTop = cString;
    while ( *pTop )
    {
        if ( isspace(*pTop) )
        {
            ++pTop;
        }
        else if ( ignoreEqual && *pTop == '=' )
        {
            ++pTop;
            ignoreEqual = 0;
        }
        else
        {
            break;
        }
    }
    return pTop;
}

/*
  Remove trailing whitespaces at end of cString and if wanted also remove first equal sign found at end of cString
 */
void trimTail(char *cString, int ignoreFirstEqual)
{
    int ignoreEqual = ignoreFirstEqual;
    char *pTail = cString + strlen(cString);
    while ( pTail-- != cString )
    {
        if ( isspace(*pTail) )
        {
            *pTail = 0;
        }
        else if ( ignoreEqual && *pTail == '=' )
        {
            *pTail = 0;
            ignoreEqual = 0;
        }
        else
        {
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    char *name = 0;
    char *value = 0;
    int   valueArgc = 0;
    char *valueArgv[3];
    char const *iniFile = 0;
    char const *iniSection = 0;
    int  wantToRemove = 0;
    int  verbose = 0;

    /*
      Argument Parsing
     */
    for ( int iArg = 1; iArg < argc; iArg++ )
    {
        if ( argv[iArg][0] == '-' )
        {
            if ( !strcasecmp(SAPDB_DATABASES_SECTION, &argv[iArg][1]) )
            {
                iniFile = SAPDB_DATABASES_INI_FILE;
                iniSection = SAPDB_DATABASES_SECTION;
            }
            else if ( !strcasecmp(SAPDB_INSTALLATIONS_SECTION, &argv[iArg][1]) )
            {
                iniFile = SAPDB_INSTALLATIONS_INI_FILE;
                iniSection = SAPDB_INSTALLATIONS_SECTION;
            }
            else if ( !strcasecmp(SAPDB_RUNTIME_SECTION, &argv[iArg][1]) )
            {
                iniFile = SAPDB_RUNTIMES_INI_FILE;
                iniSection = SAPDB_RUNTIME_SECTION;
            }
            else if ( !strcasecmp(SAPDB_GLOBALS_SECTION, &argv[iArg][1]) )
            {
                iniFile = 0;
                iniSection = 0;
            }
            else if ( argv[iArg][1] == 'f' )
            {
                iniFile = &argv[iArg][2];
            }
            else if ( argv[iArg][1] == 's' )
            {
                iniSection = &argv[iArg][2];
                if ( !iniFile )
                {
                    iniFile = SAPDB_GLOBAL_CONFIG_FILE;
                }
            }
            else if ( !strcasecmp("remove", &argv[iArg][1]) )
            {
                wantToRemove = 1;
            }
            else if ( !strcasecmp("verbose", &argv[iArg][1]) )
            {
                verbose = 1;
            }
            else
            {
                return Usage(argv[0]);
            }
        }
        else
        {
            if ( valueArgc == 3 )
            {
                fprintf(stderr, "Too may parameters\n");
                return Usage(argv[0]);
            }
            valueArgv[valueArgc++] = argv[iArg];
        }
    }

    tsp00_ErrTextc errText;
    RTE_IniFileResult ok = 0;

    /*
      If no name/value given, list content of section
     */
    if ( valueArgc == 0 )
    {
        RTE_RegistryHandle handle;
        RTE_RegistryLocation location;
        value = new char [4096];
        name = new char [4096];
        if ( !iniFile )
        {
            iniFile = SAPDB_GLOBAL_CONFIG_FILE;
            iniSection = SAPDB_GLOBALS_SECTION;
        }
        
        if ( verbose )
        {
            fprintf(stderr, "Enumerating all values from %s:[%s]\n", iniFile, iniSection);
        }

        handle = RTE_OpenGlobalConfigEnum ( iniFile,
                                            iniSection,
                                            errText,
                                            ok );
        if ( ok )
        {
            fprintf(stderr, 
                "Error opening %s:[%s] %.40s\n",
                iniFile, 
                iniSection, 
                errText.asCharp ());
            return 1;
        }

        int nextEnumResult;
        do {
            memset(name, 0, 4096);
            memset(value, 0, 4096);
            nextEnumResult = RTE_NextConfigEnum ( handle,
                                                  name,
                                                  4095,
                                                  value,
                                                  4095,
                                                  location,
                                                  errText,
                                                  ok );
            if ( nextEnumResult == 1 )
            {
                printf("%s=%s\n", name, value);
            }
            else if ( ok != SAPDB_INIFILE_RESULT_EOF )
            {
                fprintf(stderr, "Error parsing %s:[%s]: %.40s\n",
                    iniFile, 
                    iniSection, 
                    errText.asCharp ());
                return 1;
            }
        } while ( nextEnumResult != 0 );

        if ( 0 == RTE_CloseConfigEnum ( handle,
                                        errText,
                                        ok ) )
        {
            fprintf(stderr, "Closing %s:[%s] failed: %.40s\n",
                iniFile, 
                iniSection, 
                errText.asCharp ());
            return 1;
        }

        return 0;
    }
    
    /*
      Split and Trim name [ = value] parameter...
     */
    if ( valueArgc == 1 )
    {
        name = strdup(valueArgv[0]);
        char *delimiter = strchr(name, '=');
        if ( delimiter )
        {
            *delimiter = 0;
            value = delimiter+1;
            trimTail(name, 0);
            value = trimTop(value,0);
        }

    } else if ( valueArgc == 2 )
    {
        name = strdup(valueArgv[0]);
        value = strdup(valueArgv[1]);
        if ( strchr(name, '=') )
        {
            trimTail(name,1);
            value = trimTop(value,0);
        }
        else
        {
            trimTail(name,0);
            value = trimTop(value,1);
        }
    } else if ( valueArgc == 3 )
    {
        if ( strcmp(valueArgv[1], "=") )
        {
            fprintf(stderr, "Too many parameters\n");
            return Usage(argv[0]);
        }
        name = strdup(valueArgv[0]);
        value = strdup(valueArgv[2]);
    }

    /*
      If value given setting a name = value is selected
     */
    if ( value )
    {
        if ( iniFile )
        {
            if ( verbose )
            {
                fprintf(stderr, "Setting %s:[%s] Parameter '%s' value '%s'\n", iniFile, iniSection, name, value);
            }

            RTE_PutConfigString( iniFile,
                                 iniSection,
                                 name,
                                 value,
                                 errText,
                                 ok );
        }
        else
        {
            iniFile = SAPDB_GLOBAL_CONFIG_FILE;
            iniSection = SAPDB_GLOBALS_SECTION;
            if ( verbose )
            {
                fprintf(stderr, "Setting global %s:[%s] Parameter '%s' value '%s'\n",
                    iniFile, 
                    iniSection, 
                    name, 
                    value);
            }
            RTE_PutInstallationConfigString( name,
                                             value, 
                                             errText,
                                             ok);
        }

        if ( ok )
        {
	        fprintf(stderr, "Setting %s:[%s] parameter '%s' = '%s' failed:%.40s\n", 
                iniFile,
                iniSection, 
                name,
                value,
                errText.asCharp ());
	        return 1;
        }
    }
    else
    {
        /*
          If explicit remove is wanted, do it
         */
        if ( wantToRemove )
        {
            if ( !iniFile )
            {
                iniFile = SAPDB_GLOBAL_CONFIG_FILE;
                iniSection = SAPDB_GLOBALS_SECTION;
                if ( verbose )
                {
                    fprintf(stderr, "Removing global %s:[%s] Parameter '%s'\n",
                        iniFile,
                        iniSection,
                        name);
                }
            }
            else if ( verbose )
            {
                fprintf(stderr, "Removing %s:[%s] Parameter '%s'\n", iniFile, iniSection, name);
            }

            RTE_RemoveConfigString( iniFile,
                                    iniSection,
                                    name,
                                    errText,
                                    ok );
            if ( ok )
            {
	            fprintf(stderr, "Removing %s:[%s] parameter '%s' failed:%.40s\n", 
                    iniFile,
                    iniSection, 
                    name,
                    errText.asCharp ());
	            return 1;
            }
        }
        else
        /*
          Otherwise show value for given name
         */
        {
            value = new char [4096];
            memset(value, 0, 4096);
            if ( iniFile )
            {
                if ( verbose )
                {
                    fprintf(stderr, "Getting %s:[%s] Parameter '%s'\n",
                        iniFile,
                        iniSection,
                        name);
                }
                RTE_GetConfigString( iniFile,
                                     iniSection,
                                     name,
                                     value,
                                     4095,
                                     errText,
                                     ok );
            }
            else
            {
                iniFile = SAPDB_GLOBAL_CONFIG_FILE;
                iniSection = SAPDB_GLOBALS_SECTION;
                if ( verbose )
                {
                    fprintf(stderr, "Getting global %s:[%s] Parameter '%s'\n", iniFile, iniSection, name);
                }
                RTE_GetInstallationConfigString( name,
                                                 value,
                                                 4095,
                                                 errText,
                                                 ok);
            }

            if ( !ok )
            {
	            printf("%s\n", value);
            }
            else
            {
	            fprintf(stderr, "Getting %s:[%s] parameter '%s' failed:%.40s\n", 
                    iniFile,
                    iniSection, 
                    name,
                    errText.asCharp ());
	            return 1;
            }
        }
    }

    return 0;
}
