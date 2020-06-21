/*******************************************************************************

  module:       sdbinfo.c

  ------------------------------------------------------------------------------

  responsible:  Alexander Ringhof (D038248 - alexander.ringhof@sap.com)

  special area: DBM Server: exec_sdbinfo

  description:  control module, determination of db host system parameters

  see also:     none

  ------------------------------------------------------------------------------
                         Copyright (c) 2002-2005 SAP AG

  modifications:
  -------------------------------------------------------------------------
  001   who: RA
       what: Peculiar in .NET c++ runtime will be ignored
       when: 20.12.2004
  -------------------------------------------------------------------------
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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*******************************************************************************/

/* INCLUDE SECTION */
#include "cn/sdbinfo/sdbinfo.h"
#include "SAPDBCommon/SAPDB_string.h"

#ifndef WIN32
 #include <unistd.h>
#endif

#if defined(SUN) || defined(SOLARIS)
  #include <sys/processor.h>
#elif HPUX
  /*#include <sys/pset.h>*/
  #include <sys/param.h>
  #include <sys/pstat.h>
#endif

/* DEFINE SECTION */
#define FALSE 0
#define TRUE 1

/* GLOBAL SECTION */
char *SdbinfoVersionMajor = "1";
char *SdbinfoVersionMinor = "001";

char *FullPrtFileName = NULL;     /*<IndepDataPath>/wrk/SDBINFO.PRT*/
char IndepDataPath[1024];
char IndepProgPath[1024];

char *dll_filename_arr[20];       /*array for filenames: find_Files*/

int CmdMode = 0;                  /*call parameter "-c" shows commands*/
char *OsVersion = NULL;           /*OS Version determined will be stored here*/

#ifdef WIN32
  SYSTEM_INFO sys;                /*system information structure*/
  OSVERSIONINFO osvi;             /*os version information structure*/
  char *FullWinSysPath = NULL;
#endif

#ifdef WIN32
  char *slash = "\\";
#else
  char *slash = "/";
#endif


/* HELP functions */
/**************************************************************
  description:
    print usage informations
  arguments:
    none
  return value:
    none
**************************************************************/
void print_HelpUsage ()
{
  printf("usage: SDBINFO [-a[ll] -h[elp] -v]\n");
  printf(" -a[ll] all system informations\n");
  printf(" -h[elp] help information of this tool\n");
  printf(" -v version of this tool\n");
  printf(" no options return all system informations\n");
}

void print_WrongUsage ()
{
  printf ("INFO: Wrong usage !\n");
  print_HelpUsage();
}

void print_HelpText ()
{
  print_HelpUsage ();
  printf ("\nReturncodes: 0 ok\n");
  printf (" -1 no system informations found\n");
  printf (" -2 wrong usage\n");
}


/* check_Usage */
/**************************************************************
  description:
    evaluate which usage
  arguments:
    *
  return value:
    none
**************************************************************/
void check_Usage ( int argc, char *argv[], int *all)
{
  int i = 1;

  *all = 0;

  while (i < argc)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
        case 'c':
        case 'C':
          if (strlen(argv[i]) == 2)
            CmdMode=1;
          else
          {
            print_WrongUsage();
            exit (-2);
          }
          break;
        case 'v':
        case 'V':
          if (strlen(argv[i]) == 2)
					{
            printf("SDBINFO version %s.%s\n", SdbinfoVersionMajor,SdbinfoVersionMinor);
            printf("(001) Peculiar in .NET c++ runtime will be ignored\n");
          }           
          else
          {
            print_WrongUsage();
            exit (-2);
          }
          exit(0);
        case 'h':
        case 'H':
          if (strlen(argv[i]) == 2)
          {
            print_HelpText();
            exit(0);
          }
          else
            if (((argv[i][2] == 'E') || (argv[i][2] == 'e')) &&
                ((argv[i][3] == 'L') || (argv[i][3] == 'l')) &&
                ((argv[i][4] == 'P') || (argv[i][3] == 'P')))
            {
              print_HelpText();
              exit(0);
            }
            else
            {
              print_WrongUsage();
              exit (-2);
            }
            break;
        case 'a':
        case 'A':
          if (strlen(argv[i]) == 2)
            *all = 1;
          else
            if (((argv[i][2] == 'L') || (argv[i][2] == 'l')) &&
                ((argv[i][3] == 'L') || (argv[i][3] == 'l')))
              *all = 1;
            else
            {
              print_WrongUsage();
              exit (-2);
            }
            break;
        default:
          print_WrongUsage();
          exit (-2);
      } /* end switch */
    }
    else
      if ( i + 1 != argc)
      {
        print_WrongUsage();
        exit (-2);
      }
      else
        break;
    i++;
  } /* endwhile */
  return;
}


/* exec_OS_Cmd */
/**************************************************************
  description:
    executes a OS command an writes its output
    to stdout and prt-file
    output appearance is handled by arguments
  arguments:
    (in) os_cmd_out
          command to execute
    (in) str
          parametername
    (in) free
          free name_crs after output
    (in) name_cr
        0 print parametername
        1 print parametername with carriage return
        2 print not parametername
    (in) out
        0 print to stdout
        1 print not to stdout
    (in) save
        0 save output to global variable
        1 do not save output
  return value:
    none
**************************************************************/
void exec_OS_Cmd(char os_cmd_out[], char str[], int free, int name_cr, int out, int save)
{
  int i=0;
  char psBuffer[512];
  FILE *pipe;
  FILE *WriteHandle;
  char na[] = "#";

  /* open PRT-file for write */
  if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: '%s' can't be opened\n", FullPrtFileName );
    exit(1);
  }

  /* print parametername */
  switch(name_cr)
  {
    case 0:
      fprintf( WriteHandle, "%s", str );
      if (out==0) printf(str);
      if (CmdMode==1) printf("(%s)",os_cmd_out);
      break;
    case 1:
      fprintf( WriteHandle, "%s\n", str );
      if (out==0) printf(str);
      if (CmdMode==1) printf("(%s)",os_cmd_out);
      if (out==0) printf("\n");
      break;
    case 2:
      break;
    default:
      break;
  }

  if (strncmp(os_cmd_out,na,1)==0)
  {
    /* no info for parameter determination */
    fprintf( WriteHandle, "%s\n", "not available" );
    if (out==0) printf("not available\n");
  }
  else
  {
    /* command execution */
    #ifdef WIN32
    if( (pipe = _popen( os_cmd_out, "rt" )) == NULL )
    #else
    if( (pipe = popen( os_cmd_out, "r" )) == NULL )
    #endif
    {
      fprintf( WriteHandle, "%s\n", "ERROR: command execution failed" );
      printf( "ERROR: command execution failed\n" );
      fclose(WriteHandle);
      exit( 1 );
  }

  #ifdef WIN32
  /* read pipe until end of file (termination of command) */
  while( !feof( pipe ) )
  {
    if( fgets( psBuffer, 256, pipe ) != NULL )
    {
      /*std output*/
      if (strncmp(psBuffer,"OK",2)!=0)
      {
        if (out==0) printf( psBuffer );
        /*output to file*/
        fprintf( WriteHandle, "%s", psBuffer );
      }
    }
  }
  /* close pipe and print return value */
  _pclose( pipe );
  /*TSTprintf( "\nProcess returned %d\n", _pclose( pipe ) );TST*/
  #else
  /* read pipe until end of file (termination of command) */
  while( fgets( psBuffer, 256, pipe ) != NULL )
  {
    /*std output*/
    if (strncmp(psBuffer,"OK",2)!=0)
    {
      if (out==0) printf( psBuffer );
      if (save==0)
      {
        OsVersion = (char *) malloc((strlen(psBuffer) +1) *sizeof (char));
        SAPDB_strcpy(OsVersion,psBuffer);
      }
      /*output to file*/
      fprintf( WriteHandle, "%s", psBuffer );
    }
  }
  /* close pipe and print return value */
  pclose( pipe );
  /*TSTprintf( "\nProcess returned %d\n", pclose( pipe ) );TST*/
  #endif
  }

  /* set free spaces */
  while (i!=free)
  {
    fprintf( WriteHandle, "%s\n", "" );
    if (out==0) printf("\n");
    i++;
  }
  i=0;

  /* close PRT-file */
  fclose(WriteHandle);
}


/* exec_Cmd */
/**************************************************************
  description:
    execution of commands without output handling
  arguments:
    (in) cmd
          command to execute
  return value:
    none
**************************************************************/
void exec_Cmd(char cmd[])
{
  FILE *pipe;
  FILE *WriteHandle;

  /* open PRT-file for write */
  if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: '%s' can't be opened\n", FullPrtFileName );
    exit(1);
  }

  /* command execution */
  #ifdef WIN32
  if( (pipe = _popen( cmd, "rt" )) == NULL )
  #else
  if( (pipe = popen( cmd, "r" )) == NULL )
  #endif
  {
    fprintf( WriteHandle, "%s\n", "ERROR: command execution failed" );
    printf( "ERROR: command execution failed\n" );
    fclose(WriteHandle);
    exit( 1 );
  }
  else
  {
    fclose(WriteHandle);
  }

  #ifdef WIN32
  _pclose( pipe );
  #else
  pclose( pipe );
  #endif
}


/* get_NT_WinmsdParam (PROTOTYP)*/
/**************************************************************
  description:
    extraction of windows system parameters
    out of file info_nt.txt created by winmsd command
  arguments:
    none
  return value:
    none
**************************************************************/
void get_NT_WinmsdParam()
{
  #ifdef WIN32
  FILE *ReadHandle;
  int i;

  /*char *p; *//*string token pointer*/
  char info_str[MXLINELEN];
  /*char version_string[MXLINELEN]; */

  /* SYSTEM_INFO sys;
  OSVERSIONINFO osvi;
  char *lc_prod_string=NULL; */

  /*int len = UTS_NAME_LENGTH;
  SAP_UC nodename[ UTS_NAME_LENGTH ];
  SAP_UC buf[128];*/

  /*open INFO_NT-file*/
  if( (ReadHandle = fopen( "INFO.TXT", "r" )) == NULL )
  {
    printf( "ERROR: 'INFO_NT.TXT' read failed\n" );
    exit (1);
  }

  /*read INFO_NT-file*/
  i=0;
  while ((fgets(info_str,MXTEXTLEN,ReadHandle) != NULL) && (i==0))
  {
    /* delete cr*/
    /*i=strlen(info_str)-1;
    if(info_str[i]=="\n") info_str[i]="\0";*/

    /* printf(info_str); */

    /*get token 1 from info_str*/
    /*p=strtok(info_str, "' ");
    printf(("|%s"),p); */

    /*check version | get string*/
    /*if (strncmp(info_str,"Version",7)==0)
    {
      SAPDB_strcpy(version_string,p);
      printf(("|%s\n"),version_string);
      i=1;
    } */
  }

  fclose(ReadHandle);

  /*get SID out of lc_prod_string*/
  /*p=strtok(lc_prod_string, "- ");
  lc_prod_host = (SAP_UC *) mallocU(strlenU(p) + 1);
  strcpyU(lc_prod_host,p);
  fprintfU(WriteHandle,cU("|%s\n"),lc_prod_host);*/

  /*get HOST out of lc_prod_string*/
  /*p=strtok(NULL, "- ");
  lc_prod_sid = (SAP_UC *) mallocU(strlenU(p) + 1);
  strcpyU(lc_prod_sid,p);
  fprintfU(WriteHandle,cU("|%s\n"),lc_prod_sid);*/
  #endif
}


/* get_LC_PcrRuntime (PROTOTYP) */
/**************************************************************
  description:
    determination of installed precompiler runtimes (irconf -s)
  arguments:
    none
  return value:
    none
**************************************************************/
void get_LC_PcrRuntime()
{
  char psBuffer[128];
  FILE *pipe;
  FILE *WriteHandle;

  /* open PRT-file for write */
  if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: '%s' can't be opened\n", FullPrtFileName );
    exit (1);
  }

  /* header information */
  fprintf( WriteHandle, "%s\n", "Installed Precompiler Runtimes" );
  fprintf( WriteHandle, "%s\n", "------------------------------" );

  /* command execution */
  #ifdef WIN32
  if( (pipe = _popen( "irconf -s", "rt" )) == NULL )
  #else
  if( (pipe = popen( "irconf -s", "rt" )) == NULL )
  #endif
  {
    fprintf( WriteHandle, "%s\n", "ERROR: command execution failed" );
    fclose(WriteHandle);
    exit( 1 );
  }

  /* read pipe until end of file (termination of command) */
  #ifdef WIN32
  while( !feof( pipe ) )
  {
    if( fgets( psBuffer, 128, pipe ) != NULL )
    {
      /*std output*/
      printf( psBuffer );
      /*output to file*/
      fprintf( WriteHandle, "%s", psBuffer );
    }
  }
  /* close pipe and print return value */
  printf( "\nProcess returned %d\n", _pclose( pipe ) );
  #else
  while( fgets( psBuffer, 128, pipe ) != NULL )
  {
    /*std output*/
    printf( psBuffer );
    /*output to file*/
    fprintf( WriteHandle, "%s", psBuffer );
  }
  /* close pipe and print return value */
  printf( "\nProcess returned %d\n", pclose( pipe ) );
  #endif

  /* close file */
  fclose(WriteHandle);
}


/* set_IndepPath */
/**************************************************************
  description:
    setting of IndepPathes with dbmcli command dbm_getpath
  arguments:
    (in) path
      1 IndepDataPath
      2 IndepProgPath
  return value:
    0 success
    -1 error
**************************************************************/
int set_IndepPath(int path)
{
  int rc=-1;
  char psBuffer[256];
  FILE *pipe;

  char lc_cmd_out[40];

  /* command setting */
  switch(path)
  {
    case 1:      
      SAPDB_strcpy (lc_cmd_out, "dbmcli dbm_getpath IndepDataPath");
      break;
    case 2:      
      SAPDB_strcpy (lc_cmd_out, "dbmcli dbm_getpath IndepProgPath");
      break;
    default:
      printf( "ERROR: undefined call of function set_IndepPath");
      exit (-2);
      break;
  }

  /* Command execution */
#ifdef WIN32
  if( (pipe = _popen( lc_cmd_out, "rt" )) == NULL )
#else
  if( (pipe = popen( lc_cmd_out, "r" )) == NULL )
#endif
  {
    printf( "ERROR: command execution failed" );
    exit( 1 );
  }

#ifdef WIN32
  /* Read pipe until end of file (termination of command) */
  while( !feof( pipe ) )
  {
#endif
    if( fgets( psBuffer, 256, pipe ) != NULL )
    {
      /*std output*/
      if (strncmp(psBuffer,"OK",2)==0)
      {
        if (fgets( psBuffer, 256, pipe ) != NULL)
        {
          psBuffer[strlen(psBuffer)-1]='\0';

          switch(path)
          {
            case 1:              
              SAPDB_strcpy(IndepDataPath,psBuffer);
              break;
            case 2:              
              SAPDB_strcpy(IndepProgPath,psBuffer);
              break;
            default:
              printf( "ERROR: undefined call of function set_IndepPath");
              exit (-2);
            break;
          }
          rc=0;
        }
      }
    }
#ifdef WIN32
  }
#endif

  return(rc);
}


/*get_Dll_info*/
/**************************************************************
  description:
    determination of Dll informations with
    windows functions
    - dll version
    - dll description
  arguments:
    (in) dll_name
        name of dll
  return value:
    0 success
    -1 error
**************************************************************/
int get_Dll_info(char *dll_name)
{
  #ifdef WIN32
  int flag=-1;                    /*function success flag*/

	int length,i,sonderflag;

  FILE *WriteHandle;

  char FullDllFileName[500];      /*<WinSystemPath>/dll_name*/
  char dll_name_str[261];         /*dll_name for stdout*/


  TCHAR tchBuffer[BUFSIZE];       /*buffer for string*/
  DWORD dwResult;                 /*function return value + length of dll version info in byte*/
  LPDWORD lpdwHandle = NULL;

  DWORD winrc;                    /*last error code of a windows system call*/

  DWORD dwHandle = 0;
  LPVOID lpData;

  /* variables for dll info determination*/
  char acFormat[80];              /*format of variable dll info (build with wsprintf)*/

  WORD wCodePage;                 /*code page of dll information*/
  WORD wLangID;                   /*language id of dll information*/
  LPVOID pvInfo;                  /*pointer to code page information*/
  UINT wInfoLen;                  /*length of dll information*/
  VS_FIXEDFILEINFO *pFixedFile;   /*pointer to fixed dll information*/
  LPCTSTR pcData;                 /*pointer to variable dll information*/

  /* PRT-file write */
  if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: Append to '%s' failed\n",FullPrtFileName );
    exit (1);
  }
  else
  {

  /*
  SYSTEM FUNCTION USAGE
  #####################
  DWORD GetFileVersionInfoSize()
  LPTSTR lptstrFilename
  LPDWORD lpdwHandle

  BOOL GetFileVersionInfo()
  LPTSTR lptstrFilename, IN filename of dll (0_str)
  DWORD dwHandle, IN not used
  DWORD dwLen, IN length of getFileVersionInfoSize
  LPVOID lpData OUT pointer to info buffer

  FIXED DLL INFORMATIONS
  printf("Value dwSignature: %d\n",pFixedFile->dwSignature);
  printf("Value dwStrucVersion: %d\n",pFixedFile->dwStrucVersion);
  printf("Value dwFileVersionMS: %d\n",pFixedFile->dwFileVersionMS);
  printf("Value dwFileVersionLS: %d\n",pFixedFile->dwFileVersionLS);
  printf("Value dwFileFlagsMask: %d\n",pFixedFile->dwFileFlagsMask);
  printf("Value dwFileFlags: %d\n",pFixedFile->dwFileFlags);
  printf("Value dwFileOS: %d\n",pFixedFile->dwFileOS);
  printf("Value dwFileType: %d\n",pFixedFile->dwFileType);
  printf("Value dwFileSubtype: %d\n",pFixedFile->dwFileSubtype);

  VARIABLE DLL INFORMATIONS
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\ProductName",
  wCodePage,wLangID);
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\LegalCopyright",
  wCodePage,wLangID);
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\InternalName",
  wCodePage,wLangID);
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\OriginalFilename",
  wCodePage,wLangID);
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\ProductVersion",
  wCodePage,wLangID);
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\CompanyName",
  wCodePage,wLangID);
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\FileVersion",
  wCodePage,wLangID);
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\Description",
  wCodePage,wLangID);
  PROBLEMS
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\Comments",
  wCodePage,wLangID);
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\SpecialBuild",
  wCodePage,wLangID);
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\LegalTrademarks",
  wCodePage,wLangID);
  wsprintf(acFormat,"StringFileInfo\\%04X%04X\\PrivateBuild",
  wCodePage,wLangID);
  #######################
  */

  /*set dll_name_str*/  
  SAPDB_strcpy(dll_name_str,dll_name);

  /*set FullPrtFileName*/  
  SAPDB_strcpy(FullDllFileName,FullWinSysPath);
  strcat(FullDllFileName,slash);
  strcat(FullDllFileName,dll_name);
  /*printf("%s", FullDllFileName );*/

  /*get size in bytes of dll's version information*/
  dwResult = GetFileVersionInfoSize(FullDllFileName,lpdwHandle);

  /*printf("%d bytes\n",dwResult);*/

  if (dwResult==0)
  {
    printf("%s no version info\n", FullDllFileName);
    fprintf(WriteHandle,"%s no version info\n", FullDllFileName);
  }
  else
  {
    /*give buffer to pointer*/
    lpData = tchBuffer;

    /*fill buffer with version information*/
    if( GetFileVersionInfo(FullDllFileName,dwHandle,dwResult,lpData) ) /*0=error*/
    {
      /*get static (\\) informations (fixed)*/
      VerQueryValue(lpData,
                    "\\",
                    (void**)&pFixedFile,
                    &wInfoLen);

      /*print dll version and name*/
      printf(" %d.%d.%d.%d %s",
              HIWORD(pFixedFile->dwProductVersionMS),
              LOWORD(pFixedFile->dwProductVersionMS),
              HIWORD(pFixedFile->dwProductVersionLS),
              LOWORD(pFixedFile->dwProductVersionLS),
              dll_name_str);
      fprintf(WriteHandle," %d.%d.%d.%d %s",
              HIWORD(pFixedFile->dwProductVersionMS),
              LOWORD(pFixedFile->dwProductVersionMS),
              HIWORD(pFixedFile->dwProductVersionLS),
              LOWORD(pFixedFile->dwProductVersionLS),
              dll_name_str);

      /*read CodePage and LanguageID for the determination of string names*/
      VerQueryValue(lpData,"\\VarFileInfo\\Translation",
                    &pvInfo,&wInfoLen);

      /*extract these informations*/
      wCodePage = LOWORD(*(DWORD*)pvInfo);
      wLangID = HIWORD(*(DWORD*)pvInfo);

      /*printf("CodePage: %d | LangID: %d\n", wCodePage, wLangID);*/

      /*get product name (variable)*/
      wsprintf(acFormat,"StringFileInfo\\%04X%04X\\ProductName",
                wCodePage,wLangID);
      VerQueryValue(lpData,acFormat,(void**)&pcData,&wInfoLen);


      /* check first on displayable ascii values (R) */
      length=strlen(pcData);
      /*printf("Laenge pcData: %i\n",length);*/
      sonderflag=-1;

      /* output signs (product name) ignoring peculiar to stdout and prt file */
      printf(" ");
			fprintf(WriteHandle," ");
      for (i=0; i<length; i++)
      {
        if ((pcData[i] > 127) || (pcData[i]<0))
        { 
          /*no output*/
          /*
          printf("Sonderzeichen gefunden an Stelle: %i ",i);
          printf("  %c ", pcData[i]);
          printf("  %i\n", (int) pcData[i]);
          */
          sonderflag=0;               
        }
        else
        {
          /*printf("Stelle: %i",i);*/
          printf("%c", pcData[i]);
					fprintf(WriteHandle,"%c",pcData[i]);
          /*printf("  %i\n", (int) pcData[i]);*/
        }
      }
      printf(" runtime\n");       
			fprintf(WriteHandle," runtime\n");


      flag=0;
    }
    else
    {
      /*error handling*/
      winrc = GetLastError();
      printf("Error %d occured !\n",winrc);
    }
  }

	}/*prt-file*/
  /* close prt-file */
  fclose(WriteHandle);
  return(flag);
  #else
  return(0);
  #endif


}


/*find_Files*/
/**************************************************************
  description:
    determination of filenames in a given directory
  arguments:
    (in) FullDllWildcard
        Pathname followed by filename
  return value:
    0 success (files found)
    -1 error (no files found)
**************************************************************/
int find_Files (char *f_FullDllWildcard)
{
  #ifdef WIN32
  WIN32_FIND_DATA FilesFound;
  HANDLE DirHandle;
  int i=0;
  int flag=-1; /* function success variable */

  /*create list of files in directory*/
  /*sprintfU (SearchPath, cU("%s\\*"), rpath); UNICODE*/
  /*printf ("SearchPath: %s\n", f_FullDllWildcard);*/

  DirHandle = FindFirstFile(f_FullDllWildcard, &FilesFound);

  if (DirHandle != INVALID_HANDLE_VALUE)
  {
    do
    {
      /*check if listed file is a subdirectory*/
      if (FilesFound.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        if ((strcmp(("."), FilesFound.cFileName)) && (strcmp((".."), FilesFound.cFileName)))
        {
          printf("Only Directory has been found !");
        }
      }
      else /* found file is a simple file*/
      {
        flag=0;
        /*printf("File Found: %s\n",FilesFound.cFileName);*/

        /*copy filename to array*/
        dll_filename_arr[i] = (char *) malloc(strlen(FilesFound.cFileName) *sizeof (char));

        SAPDB_strcpy(dll_filename_arr[i],FilesFound.cFileName);
        /*printf("dll_filename_arr: %s Index: %i\n",dll_filename_arr[i],i);*/
        i++;
      }

    } while (FindNextFile(DirHandle, &FilesFound));

    FindClose(DirHandle);
  }
  return(flag);
  #else
  return(0); /*UNIX*/
  #endif
}



/* M A I N */
/**************************************************************
  description:
    main function
  arguments:
    (in) argc
    (in) argv[]
  return value:
  none
**************************************************************/
int main(int argc, char *argv[])
{
  int all;
  int rc;                             /*return code of function calls*/
  int i;                              /*counter*/
  int length;                         /*length of strings (strlen operations)*/
  FILE *WriteHandle;

  int LocalMode;                      /*if no dbmcli is available, sdbinfo runs in localmode*/

  char *PrtFileName = "SDBINFO.PRT";  /*protocol file name*/
  char *WorkDir = "wrk";              /*work directory name*/

  char *dll_wildcard = "msvc*.dll";   /*winnt dll names fo be checked*/
  char *FullDllWildcard = NULL;       /*<FullWinSysPath>/dll_wildcard*/
  char *cmp_str = "D.dll";            /*compare string for uninteresting *D.dll*/

#ifdef WIN32
  LPTSTR lpszSystemPath;              /* pointer to system path string */
  TCHAR tchBuffer[BUFSIZE];           /* buffer for string*/
#endif

#ifdef HPUX
struct pst_static pst;								/*system information for HPUX OS*/
struct pst_dynamic psd;

size_t cnt_online;                    /*counter: processors online*/
size_t cnt_configured;                /*counter: processors configured*/
#endif

  /* Library Suffix */
#ifdef WIN32
  #define SAPDB_DDL_SUFFIX cU(".dll")
#elif defined (HPUX) && !defined (HP_IA64)
  #define SAPDB_DDL_SUFFIX cU(".sl")
#elif AIX
  #define SAPDB_DDL_SUFFIX cU(".o")
#else
  #define SAPDB_DDL_SUFFIX cU(".so")
#endif

  /* OS PARAMETERS */
  /*###############*/

  /* Date & Time */
  char date_str[] = "";
#ifdef OSF1
  char os_date_cmd[] = "date";
#elif WIN32
  char os_date_cmd[] = "echo %date%";
  char os_time_cmd[] = "echo %time%";
#elif defined SUN || defined SOLARIS
  char os_date_cmd[] = "date";
#elif AIX
  char os_date_cmd[] = "date";
#elif HPUX
  char os_date_cmd[] = "date";
#elif LINUX
  char os_date_cmd[] = "date";
#else
  char os_date_cmd[] = "#";
#endif

  /* System name */
  char host_str[] = "[System name]";
#ifdef OSF1
  char os_hostname_cmd[] = "hostname";
#elif WIN32
  char os_hostname_cmd[] = "hostname";
#elif FREEBSD
  char os_hostname_cmd[] = "hostname";
#elif defined SUN || defined SOLARIS
  char os_hostname_cmd[] = "hostname";
#elif AIX
  char os_hostname_cmd[] = "hostname";
#elif HPUX
  char os_hostname_cmd[] = "hostname";
#elif LINUX
  char os_hostname_cmd[] = "hostname";
#else
  char os_hostname_cmd[] = "#";
#endif

  /* OS Name */
  char platform_str[] = "[OS name]";
#ifdef OSF1
  char os_platform_cmd[] = "uname -s";
#elif WIN32
  char os_platform_cmd[] = "echo %os%";
#elif FREEBSD
  char os_platform_cmd[] = "uname -s";
#elif defined SUN || defined SOLARIS
  char os_platform_cmd[] = "uname -s";
#elif AIX
  char os_platform_cmd[] = "uname -s";
#elif HPUX
  char os_platform_cmd[] = "uname -s";
#elif LINUX
  char os_platform_cmd[] = "uname -s";
#else
  char os_platform_cmd[] = "#";
#endif

  /* OS Version */
  char version_str[] = "[Version]";
#ifdef OSF1
  char os_version_cmd[] = "uname -r";
#elif WIN32
  char os_version_cmd[] = "#";/*special implementation*/
#elif FREEBSD
  char os_version_cmd[] = "uname -r";
#elif defined SUN || defined SOLARIS
  char os_version_cmd[] = "uname -r";
#elif AIX
  char os_version_cmd[] = "oslevel";
#elif HPUX
  char os_version_cmd[] = "uname -r";
#elif LINUX
  char os_version_cmd[] = "uname -r";
#else
  char os_version_cmd[] = "#";
#endif

  /* OS Patch level */
  char patch_str[] = "[Patch level]";
#ifdef OSF1
  char os_patch_cmd[] = "uname -v";
#elif WIN32
  char os_patch_cmd[] = "#";/*special implementation*/
#elif FREEBSD
  char os_patch_cmd[] = "uname -v"; // is this too long (~ 70 chars) ??
#elif defined SUN || defined SOLARIS
  char os_patch_cmd[] = "uname -v";
#elif AIX
  char os_patch_cmd[] = "lslpp -Lc bos.mp | grep bos | cut -c 21-22";
#elif HPUX
  char os_patch_cmd[] = "uname -v";
#elif LINUX
  char os_patch_cmd[] = "uname -v";
#else
  char os_patch_cmd[] = "#";
#endif

  /* OS Processor type */
  char proctype_str[] = "[Processor type]";
#ifdef OSF1
  char os_proctype_cmd[] = "uname -p";
#elif WIN32
  char os_proctype_cmd[] = "echo %PROCESSOR_ARCHITECTURE%";
#elif FREEBSD
  char os_proctype_cmd[] = "uname -p";
#elif defined SUN || defined SOLARIS
  char os_proctype_cmd[] = "uname -p";
#elif AIX
  char os_proctype_cmd[] = "uname -p";
#elif HPUX
  char os_proctype_cmd[] = "uname -m";
#elif LINUX
  char os_proctype_cmd[] = "uname -p";
#else
  char os_proctype_cmd[] = "#";
#endif

/*  ifdef _LONG_LONG *//* == AIX 4.1 ff */
   /*else*/       /* no _LONG_LONG == AIX 3.2 */

  /* OS Processor number */
  char procnumber_str[] = "[Processor number]";
#ifdef OSF1
  char os_procnumber_cmd[] = "uname -p";
#elif WIN32
  char os_procnumber_cmd[] = "#";
#elif FREEBSD
  char os_procnumber_cmd[] = "uname -m";
#elif defined SUN || defined SOLARIS
  char os_procnumber_cmd[] = "psrinfo";
#elif AIX
  char os_procnumber_cmd[] = "uname -p";
#elif HPUX
  char os_procnumber_cmd[] = "uname -m";
#elif LINUX
  char os_procnumber_cmd[] = "uname -p";
#else
  char os_procnumber_cmd[] = "#";
#endif

  /* OS Processor info */
  char procinfo_str[] = "[Processor info]";
#ifdef OSF1
  char os_procinfo_cmd[] = "psrinfo";
#elif WIN32
  char os_procinfo_cmd[] = "echo %PROCESSOR_IDENTIFIER%";
#elif defined SUN || defined SOLARIS
  char os_procinfo_cmd[] = "psrinfo -v";
#elif AIX
  char os_procinfo_cmd[] = "bindprocessor -q";
#elif HPUX
  char os_procinfo_cmd[] = "model";
#elif LINUX
  char os_procinfo_cmd[] = "uname -m";
#else
  char os_procinfo_cmd[] = "#";
#endif

  /* C++ Compiler version */
	/* not needed !!*/
/*  char cversion_str[] = "[C++ compiler version]";
#ifdef OSF1
  char os_cversion_cmd[] = "cxx -V";
#elif WIN32
  char os_cversion_cmd[] = "#";
#elif FREEBSD
  char os_cversion_cmd[] = "CC -v | grep version";
#elif defined SUN || defined SOLARIS
  char os_cversion_cmd[] = "CC -V | grep Pa";
#elif AIX
  char os_cversion_cmd[] = "lslpp -L xlC.cpp | grep xlC.cpp";
#elif HPUX
  char os_cversion_cmd[] = "#";
#elif LINUX
  char os_cversion_cmd[] = "gcc -dumpversion";
#else
  char os_cversion_cmd[] = "#";
#endif
*/

  /* C++ Compiler location NOT NEEDED*/
  /* char clocation_str[] = "[C++ compiler location]";
#ifdef OSF1
  char os_clocation_cmd[] = "which cxx";
#elif WIN32
  char os_clocation_cmd[] = "#";
#elif FREEBSD
  char os_clocation_cmd[] = "which CC";
#elif defined SUN || defined SOLARIS
  char os_clocation_cmd[] = "which CC";
#elif AIX
  char os_clocation_cmd[] = "which xlC";
#elif HPUX
  char os_clocation_cmd[] = "which cc";
#elif LINUX
  char os_clocation_cmd[] = "which gcc";
#else
  char os_clocation_cmd[] = "#";
#endif*/

  /* C++ Run-Time Library */
  char cruntimelib_str[] = "[C++ run-time library]";
#ifdef OSF1
  char os_cruntimelib_cmd[] = "setld -i | grep \"C++ Run-Time Library\"";
#elif WIN32
  char os_cruntimelib_cmd[] = "#";
#elif defined SUN || defined SOLARIS
  char os_cruntimelib_cmd[] = "showrev -a | grep libC | awk '{i=NF; print $1,\"\",$2,\"\",$i}'";
#elif AIX
  char os_cruntimelib_cmd[] = "lslpp -L xlC.rte | grep xlC.rte";
#elif HPUX
#if defined (HP_IA64)
  char os_cruntimelib_cmd[] = "what /usr/lib/hpux64/libCsup.so";
  char os_cruntimelib_cmd2[]= "what /usr/lib/hpux64/libc.so";
#else
  char os_cruntimelib_cmd[] = "what /usr/lib/pa20_64/libCsup.sl";
  char os_cruntimelib_cmd2[]= "what /usr/lib/pa20_64/libc.sl";
#endif
#elif LINUX
  char os_cruntimelib_cmd[] = "rpm -qi glibc | grep Version | awk '{print$3}'";
#else
  char os_cruntimelib_cmd[] = "#";
#endif

  /* Process limits */
  char proclimits_str[] = "[Process limits]";
#ifdef OSF1
  char os_proclimits_cmd[] = "ulimit -a";/*< 5.1 no output*/
#elif WIN32
  char os_proclimits_cmd[] = "#";
#elif FREEBSD
  char os_proclimits_cmd[] = "ulimit -u";
#elif defined SUN || defined SOLARIS
  char os_proclimits_cmd[] = "ulimit -a";
#elif AIX
  char os_proclimits_cmd[] = "ulimit -a";
#elif HPUX
  char os_proclimits_cmd[] = "ulimit -a";
#elif LINUX
  char os_proclimits_cmd[] = "ulimit -a";
#else
  char os_proclimits_cmd[] = "#";
#endif

  /* Hard limits */
  char hardlimits_str[] = "[Hard limits]";
#ifdef OSF1
  char os_hardlimits_cmd[] = "ulimit -Ha";/*< 5.1 no output*/
#elif WIN32
  char os_hardlimits_cmd[] = "#";
#elif defined SUN || defined SOLARIS
  char os_hardlimits_cmd[] = "ulimit -Ha";
#elif AIX
  char os_hardlimits_cmd[] = "ulimit -Ha";
#elif HPUX
  char os_hardlimits_cmd[] = "ulimit -Ha";
#elif LINUX
  char os_hardlimits_cmd[] = "ulimit -Ha";
#else
  char os_hardlimits_cmd[] = "#";
#endif

  /* Additional OS Parameters */
#ifdef OSF1
  char realmem_str[] = "[Physical memory (Kb)]";
  char os_realmem_cmd[] = "vmstat -P";
#elif AIX
  char realmem_str[] = "[Physical memory (Kb)]";
  char os_realmem_cmd[] = "lsattr -E -l sys0 -a realmem | awk '{print$2}'";

  char maxuproc_str[] = "[MaxProcUser]";
  char os_maxuproc_cmd[] = "lsattr -E -l sys0 -a maxuproc | awk '{print$2}'";
#elif LINUX
  char realmem_str[] = "[Physical memory (Kb)]";
  char os_realmem_cmd[] = "cat /proc/meminfo | grep MemTotal | awk '{print$2}'";
#else
  char realmem_str[] = "[Physical memory (Kb)]";
  char os_realmem_cmd[] = "#";
#endif

  /* WIN32 */
#ifdef WIN32
  char os_cmd_winmsd[] = "start /wait winmsd.exe /report info_nt.txt /categories+systemsummary";
#endif

  /* LC PARAMETERS */
  /*###############*/

  /* liveCaches */
  char livecaches_str[] = "[liveCaches]";
  char lc_livecaches_cmd[] = "dbmcli db_enum";


  /* PRE SETTINGS */
  /*##############*/

  /* tool usage */
  check_Usage(argc, argv, &all);

  /* init dll_filename_arr */
  for (i=0; i<=20; i++)
  {
    dll_filename_arr[i]="";
  }

  /* set IndepPath: IndepData */
  rc = set_IndepPath(1);

  if (rc != 0)
  {
    LocalMode = 0;
  }
  else
  {
    LocalMode = 1;
  }

  if (rc != 0)
  {
    if (LocalMode == 0)
    {
      printf( "INFO: Running in LocalMode\n" );
    }
    else
    {
      printf( "ERROR: IndepDataPath can't be determined !\n" );
      printf( "--> Please log in as user <sidadm> to run sdbinfo.\n" );
      exit(-1);
    }
  }

  /* set IndepPath: IndepProg */
  rc = set_IndepPath(2);
  if (rc != 0)
  {
    if (LocalMode == 0)
    {
      printf( "INFO: Running in LocalMode\n" );
    }
    else
    {
      printf( "ERROR: IndepProgPath can't be determined !\n" );
      printf( "--> Please log in as user <sidadm> to run sdbinfo.\n" );
      exit(-1);
    }
  }

  /* set FullPrtFileName PRT-file */
  if ((LocalMode == 0) && (rc != 0))
  {
    FullPrtFileName = (char *) malloc((strlen(PrtFileName) +1) *sizeof (char));
    SAPDB_strcpy(FullPrtFileName,PrtFileName);
  }
  else
  {
    FullPrtFileName = (char *) malloc((strlen(IndepDataPath)
                    +strlen(slash)
                    +strlen(WorkDir)
                    +strlen(slash)
                    +strlen(PrtFileName) +1) *sizeof (char));

    SAPDB_strcpy(FullPrtFileName,IndepDataPath);
    strcat(FullPrtFileName,slash);
    strcat(FullPrtFileName,WorkDir);
    strcat(FullPrtFileName,slash);
    strcat(FullPrtFileName,PrtFileName);
  }

  /* create new PRT-file */
  if( (WriteHandle = fopen( FullPrtFileName, "w" )) == NULL )
  {
    printf( "ERROR: File '%s' can't be created\n",FullPrtFileName );
    exit (1);
  }

  /* print tool information to PRT-file*/
  fprintf( WriteHandle, "%s\n", "# S D B I N F O" );
  fprintf( WriteHandle, "%s%s.%s\n","# Version: ",SdbinfoVersionMajor,SdbinfoVersionMinor);
  fprintf( WriteHandle, "%s\n", "# Determination of liveCache operating system parameters" );
  fprintf( WriteHandle, "%s\n", "" );
  fclose(WriteHandle);

  /* print execution date */
  #ifdef WIN32
  exec_OS_Cmd(os_date_cmd, date_str,0,0,1,1);
  exec_OS_Cmd(os_time_cmd, date_str,1,0,1,1);
  #else
  exec_OS_Cmd(os_date_cmd, date_str,1,0,1,1);
  #endif


/* OS PARAMETER DETERMINATION */
/*############################*/

/*PREPARE*/
/*********/
  /* PRT-file write */
  if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: Append to '%s' failed\n",FullPrtFileName );
    exit (1);
  }
  else
  {
    fprintf( WriteHandle, "%s\n", "<Operating system parameters>" );
  }
  /* close prt-file */
  fclose(WriteHandle);

  /* stdout */
  printf( "<Operating system parameters>\n" );
  printf( "\n");


#ifdef WIN32
  /* set system info structure (sys) */
  GetSystemInfo( &sys );

  /* set os version info structure (osvi) */
  osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
  if( 0 == GetVersionEx( &osvi ) )
  {
    printf("ERROR: OS version info structure determination failed !\n");
    exit(-1);
  }

  /* set SystemDirectory */
  lpszSystemPath = tchBuffer;
  if( !GetSystemDirectory(lpszSystemPath, MXPATHLEN+1) )
  {
    printf("ERROR: Windows system directory can't be determined !");
  }
  FullWinSysPath = (char *) malloc(strlen(lpszSystemPath) *sizeof (char));
  SAPDB_strcpy(FullWinSysPath, lpszSystemPath);
  /*printf("System directory: %s\n", FullWinSysPath);*/
#endif

/******************************************************/
/* output control variables: free, name_cr, out, save */
/******************************************************/

/* System name (OS_IND)*/
/***********************/
  exec_OS_Cmd(os_hostname_cmd, host_str,1,1,0,1);


/* OS Name | Version | Patch level (OS_DEP) */
/********************************************/
#ifdef WIN32 /* OS Name | Version | Patch level (NT) */

    /* PRT-file write */
  if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: Append to '%s' failed\n",FullPrtFileName );
    exit (1);
  }
  else
  {

  switch( osvi.dwPlatformId )
  {
    case VER_PLATFORM_WIN32_NT:
      printf("%s\nWindows NT\n",platform_str);
      printf("%s\n%d.%d\n",version_str, osvi.dwMajorVersion,osvi.dwMinorVersion );
      printf("%s\nBuild %d\n",patch_str, osvi.dwBuildNumber );

      fprintf(WriteHandle,"%s\nWindows NT\n",platform_str);
      fprintf(WriteHandle,"%s\n%d.%d\n",version_str, osvi.dwMajorVersion,osvi.dwMinorVersion );
      fprintf(WriteHandle,"%s\nBuild %d\n",patch_str, osvi.dwBuildNumber );
      break;

    case VER_PLATFORM_WIN32_WINDOWS:
      /*strcpyU( uts->sysname, cU("Windows") );*/
      printf("%s\nWindows\n", platform_str);
      printf("%s\n", version_str);

      fprintf(WriteHandle,"%s\nWindows\n", platform_str);
      fprintf(WriteHandle,"%s\n", version_str);
      if( 0 == osvi.dwMinorVersion )
      {
        printf("95");
        fprintf(WriteHandle,"95");
      }
      if( 0 < osvi.dwMinorVersion )
      {
        printf("98");
        fprintf(WriteHandle,"98");
      }
      printf("%s\nnot available",patch_str);
      fprintf(WriteHandle,"%s\nnot available",patch_str);
      break;

    case VER_PLATFORM_WIN32s:
      printf("%s\nWindows\n", platform_str);
      printf("%s\n3.1",version_str);
      printf("%s\nnot available",patch_str);

      fprintf(WriteHandle,"%s\nWindows\n", platform_str);
      fprintf(WriteHandle,"%s\n3.1",version_str);
      fprintf(WriteHandle,"%s\nnot available",patch_str);
      break;

    default:
      printf("%s\nunknown\n", platform_str);
      printf("%s\nnot available",version_str);
      printf("%s\nnot available",patch_str);

      fprintf(WriteHandle,"%s\nunknown\n", platform_str);
      fprintf(WriteHandle,"%s\nnot available",version_str);
      fprintf(WriteHandle,"%s\nnot available",patch_str);
      break;
  }
  }/*prt-file write*/
  printf("\n"); /*free line*/
  fprintf(WriteHandle,"\n");

  /* close prt-file */
  fclose(WriteHandle);
#else
  /* OS Name (UNIX) */
  exec_OS_Cmd(os_platform_cmd, platform_str,0,1,0,1);
  /* OS Version (UNIX) */
  exec_OS_Cmd(os_version_cmd, version_str,0,1,0,0);
  /* OS Patch level (UNIX) */
  exec_OS_Cmd(os_patch_cmd, patch_str,1,1,0,1);
#endif


/* OS Processor type (OS_DEP)*/
/*****************************/
#ifdef WIN32 /* OS Processor type (NT)*/

  /* PRT-file write */
  if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: Append to '%s' failed\n",FullPrtFileName );
    exit (1);
  }
  else
  {

  /* header */
  printf("%s\n", proctype_str);
  fprintf(WriteHandle,"%s\n", proctype_str);

  switch( osvi.dwPlatformId )
  {
    case VER_PLATFORM_WIN32_NT: /*1*/
      switch(sys.wProcessorArchitecture)
      {
        case PROCESSOR_ARCHITECTURE_INTEL:
          printf("Intel ");
          if( 5 > sys.wProcessorLevel )
          {
            printf( "80%d86\n", sys.wProcessorLevel );
            fprintf(WriteHandle,"80%d86\n", sys.wProcessorLevel );
          }
          else
          {
            printf( "80%d86 (Mod %d Step %d)\n", sys.wProcessorLevel,sys.wProcessorRevision/256,sys.wProcessorRevision%256 );
            fprintf(WriteHandle,"80%d86 (Mod %d Step %d)\n", sys.wProcessorLevel,sys.wProcessorRevision/256,sys.wProcessorRevision%256 );
          }
          break;

        case PROCESSOR_ARCHITECTURE_ALPHA:
          printf("Alpha ");
          printf( "%d\n",sys.wProcessorLevel );
          fprintf(WriteHandle,"Alpha ");
          fprintf(WriteHandle,"%d\n",sys.wProcessorLevel );
          break;

        case PROCESSOR_ARCHITECTURE_PPC:
          printf("PPC ");
          printf("%d\n",600+sys.wProcessorLevel );
          fprintf(WriteHandle,"PPC ");
          fprintf(WriteHandle,"%d\n",600+sys.wProcessorLevel );
          break;

        case PROCESSOR_ARCHITECTURE_MIPS:
          printf("MIPS ");
          printf( "R%d\n",1000*sys.wProcessorLevel );
          fprintf(WriteHandle,"MIPS ");
          fprintf(WriteHandle,"R%d\n",1000*sys.wProcessorLevel );
          break;

#ifdef WIN64
        case PROCESSOR_ARCHITECTURE_IA64:
          printf("IA64 ");
          printf(" Level %d (Mod %d Step %d)\n", sys.wProcessorLevel,sys.wProcessorRevision/256,sys.wProcessorRevision%256 );
          fprintf(WriteHandle,"IA64 ");
          fprintf(WriteHandle," Level %d (Mod %d Step %d)\n", sys.wProcessorLevel,sys.wProcessorRevision/256,sys.wProcessorRevision%256 );
          break;
#endif
        default:
          break;
      }
      break;

    case VER_PLATFORM_WIN32_WINDOWS:/*1*/
      switch( sys.dwProcessorType )
      {
        case PROCESSOR_INTEL_386:
          printf("386\n");
          fprintf(WriteHandle,"386\n");
          break;

        case PROCESSOR_INTEL_486:
          printf("486\n");
          fprintf(WriteHandle,"486\n");
          break;

        case PROCESSOR_INTEL_PENTIUM:
          printf("586\n");
          fprintf(WriteHandle,"586\n");
          break;

        default:
          printf("not available\n");
          fprintf(WriteHandle,"not available\n");
          break;
      }
      break;

    default:/*1*/
      printf("not available\n");
      fprintf(WriteHandle,"not available\n");
      break;
  }
  }
  /* close PRT file */
  fclose(WriteHandle);

#else /* OS Processor type (UNIX) */

#ifdef AIX
  /*sets proctype on "not available" because "uname -p" is for AIX 4 not implemented*/
  /*printf("Stored: %s\n",OsVersion);*/
  if (strncmp(OsVersion,"4",1)==0)
  {
    SAPDB_strcpy(os_proctype_cmd,"#");
  }
#endif

  exec_OS_Cmd(os_proctype_cmd, proctype_str,0,1,0,1);

#endif


/* OS Processors configured & online (OS_DEP) */
/**********************************************/

  /* PRT-file write */
  if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: Append to '%s' failed\n",FullPrtFileName );
    exit (1);
  }
  else
  {
    /* header */
    printf("%s\n", "[Processors online]");
    fprintf(WriteHandle,"%s\n", "[Processors online]");

#ifdef WIN32 /* OS Processor type (NT)*/

    /*number of processors online*/
    printf("%u\n", sys.dwNumberOfProcessors);
    fprintf(WriteHandle,"%u\n", sys.dwNumberOfProcessors);

#elif HPUX
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) != -1)
    {
      /*number of processors online*/
      cnt_online = psd.psd_proc_cnt;
      printf("%d\n", cnt_online);
      fprintf(WriteHandle,"%d\n", cnt_online);
    }
    else
    {
      printf("%s\n", "Error: cannot determine number of processors");
    }
#else
    /*number of processors configured*/
    /*i = sysconf(_SC_NPROCESSORS_CONF);
    printf("%i\n", i);
    fprintf(WriteHandle,"%i\n", i);*/

    /*number of processors online*/
    i = sysconf(_SC_NPROCESSORS_ONLN);
    printf("%i\n", i);
    fprintf(WriteHandle,"%i\n", i);
#endif
  }
  /* close PRT file */
  fclose(WriteHandle);


  /*_SC_LIBC_VERSION
 The version of libc that is in use by the application that is requesting this information. See below for details.*/


/* OS Processor info (OS_IND) */
/******************************/
  exec_OS_Cmd(os_procinfo_cmd, procinfo_str,1,1,0,1);


/* OS C++ compiler version (OS_DEP) */
/************************************/
/*#if !(defined HPUX || defined WIN32)
  exec_OS_Cmd(os_cversion_cmd, cversion_str,0,1,0,1);
#endif*/


/* C++ Run-Time Library (OS_DEP) */
/*********************************/
#ifdef WIN32 /* C++ Run-Time Library (NT) */

  /* PRT-file write */
  if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: Append to '%s' failed\n",FullPrtFileName );
    exit (1);
  }
  else
  {
    /* header */
    printf("%s\n",cruntimelib_str);
    fprintf(WriteHandle,"%s\n",cruntimelib_str);
		/* close prt-file */
		fclose(WriteHandle);
	}

		/* examples */
    /* Visual Studio 98 --> msvcrt.dll + msvcp60.dll */
    /* VisualStudio.NET --> msvcr70.dll + msvcp70.dll */

    /* set FullDllWildcard */
    FullDllWildcard = (char *) malloc(strlen(FullWinSysPath)
                      +strlen(slash)
                      +strlen(dll_wildcard) *sizeof (char));

    SAPDB_strcpy(FullDllWildcard,FullWinSysPath);
    strcat(FullDllWildcard,slash);
    strcat(FullDllWildcard,dll_wildcard);
    /*printf("FullDllWildcard: %s\n",FullDllWildcard);*/

    /* search for specified dll filename msvc*.dll and fill dll_filename_arr */
    rc = find_Files(FullDllWildcard);
    i=0;
    if (rc == 0) /*dll files found*/
    {
      for (i; i<=20; i++)
      {
        length=strlen(dll_filename_arr[i]);
        /*printf("strlen: %i\n",length);*/

        if (length != 0)
        {
          /* determination and printing of dll_info */
          rc = get_Dll_info(dll_filename_arr[i]);
          /*printf("RC of get_Dll_info: %i\n", rc);*/
        }
      }
    }
    else
    {
      printf("No installed runtime library found !");
      /*fprintf(WriteHandle,"No installed runtime library found !"); */
    }


	/* PRT-file write */
  if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: Append to '%s' failed\n",FullPrtFileName );
    exit (1);
  }
  else
  {
    printf("\n"); /*free line*/
    fprintf(WriteHandle,"\n"); /*free line*/

		/* close prt-file */
		fclose(WriteHandle);
	}

#else  /* C++ Run-Time Library (UNIX)*/

  exec_OS_Cmd(os_cruntimelib_cmd, cruntimelib_str,1,1,0,1);

#ifdef HPUX
  /* additional command */
  exec_OS_Cmd(os_cruntimelib_cmd2, cruntimelib_str,1,2,0,1);
#endif

#endif


/* Process limits (OS_DEP) */
/***************************/
#ifndef WIN32
  exec_OS_Cmd(os_proclimits_cmd,proclimits_str,0,1,0,1);
#endif

/* Hard limits (OS_DEP) */
/************************/
#ifndef WIN32
  exec_OS_Cmd(os_hardlimits_cmd,hardlimits_str,1,1,0,1);
#endif

/* Additional OS parameters */
/****************************/
  /*exec_OS_Cmd(os_realmem_cmd,realmem_str,1,0,0.1);*/



/* LC PARAMETER DETERMINATION */
/*############################*/

  /*open prt-file*/
  /*if( (WriteHandle = fopen( FullPrtFileName, "a" )) == NULL )
  {
    printf( "ERROR: '%s' can't be created\n",FullPrtFileName );
    exit(1);
  }
  else
  {
    fprintf( WriteHandle, "%s\n", "[liveCache parameters]" );
  }*/
  /*close prt-file*/
  /*fclose(WriteHandle);*/

  /*stdout*/
  /*printf( "[liveCache parameters]\n" );
  printf( "\n" );*/

  /* output control variables: free / name_cr / out */

  /* liveCaches */
  /*exec_OS_Cmd(lc_livecaches_cmd,livecaches_str,1,1,0,1);*/

  /*get_LC_PcrRuntime();*/


  /* POST ACTIVITIES */
  /*#################*/

  printf( "System information saved in %s \n",FullPrtFileName );
  return(0);
}
