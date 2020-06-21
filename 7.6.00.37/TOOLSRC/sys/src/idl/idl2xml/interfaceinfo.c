/*


    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

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
#include      <stdio.h> 
#include      "idl/idl2xml/pms.h"
#include      "idl/idl2xml/pen00.h" 
#include      "idl/idl2xml/pmincl.h"
#include      "idl/idl2xml/interfaceinfo.h"   /* generated PMS header file */
#include      "idl/idl2xml/interfacetree.h"  
#include      "idl/idl2xml/rte.h"

#define AtL_ct          4
#define MaxNesting      20
#define MaxInclude      10
#define MaxName         300
#define MaxFile         1000
#define MaxFileLen      64
#define MaxObjectLen    64000
#define MaxLineLen      2000
#define MaxCCName       16                      /* FF 2002-04-11 PTS 1115204 */

#ifdef WIN32
#define Pre_Processor    "cl"
#elif  LINUX
#define Pre_Processor    "c++"
#elif  OSF1
#define Pre_Processor    "cxx"
#elif  SUN || SOLARIS
#define Pre_Processor    "CC"
#elif  AIX
#define Pre_Processor    "xlC_r -cpluscmt"
#elif  HPUX
#define Pre_Processor    "aCC"
#else
#define Pre_Processor    ""
#endif

/* Globals */
st_pms	       pms;								/* PMS record */
unsigned long  IncCount;						/* number of default paths */
unsigned long  CurInput;						/* index of current file pointer */
unsigned long  CurFile;						    /* File count */
unsigned long  RecCount;					    /* Record count */
unsigned long  FilCount;					    /* Files read */
unsigned long  SkipCount;					    /* Files skipped */
unsigned long  CurObjectLen;					/* current object length */
unsigned long  CurLineLen;						/* current line length */
file_p         filp[MaxNesting];				/* input file pointer stack */
char           inorim[MaxNesting];				/* include or import */
char           paths[MaxNesting][MaxName];		/* paths */
char           includes[MaxInclude][MaxName];	/* includes */
char           files[MaxFile][MaxFileLen];		/* filess */
char           pm_object[MaxObjectLen];			/* PMS text buffer */
char           pm_line[MaxLineLen];				/* input line */
file_p         xmlp;							/* xml file */
file_p         logp;							/* log file */
file_p         comp;                            /* com file */
bool           error;							

bool           PreProcessFlag;
char           PreProcParms[MaxName];           /* prepreocessor parameters    FF 2002-04-11 PTS 1115204 */
char           PreProc[MaxCCName];              /* name of preprocessor to be used FF 2002-04-11 PTS 1115204 */

InterfaceElementp  InterfaceRoot;        /* root for internal xml tree */

unsigned short IfiFilOpen
(
    char          *filnam
);

IfiMatchTest()
{
	bool		   brc;
	unsigned long lg;
	unsigned long ml;
	
	brc = PmExecInitH (&pms, CbList, 0);
	
	strcpy (pm_line, " virtual HRESULT STDMETHODCALLTYPE SAPAPO_CHANGE_PROP_AREAS( ); ");
	ml         = sizeof (pm_line);
	lg         = strlen (pm_line);
	pms.cursor = 0;
	brc = PmExecPattern (&pms, "p_midl", (unsigned char*) pm_line, &lg, ml);
	pm_line[lg] = 0;
	printf ("%s", pm_line);
	
}

bool IfiArg

(
 int     argc,
 char  **argv,
 char   *filp,
 char   *xmlp,
 char   *logp
 )
{

	unsigned short ix;

	strcpy(filp, "");
	strcpy(xmlp, "");
	strcpy(logp, "");
	strcpy(PreProc,Pre_Processor); /* FF 2002-04-11 PTS 1115204 */
	strcpy(PreProcParms,"");      /* FF 2002-04-11 PTS 1115204 */
    PreProcessFlag = FALSE;

	RecCount = 0;
	IncCount = 0;
	strcpy (includes[IncCount], "");
	
	for (ix = 1; ix < argc; ix++)
	{
		switch (argv[ix][1])
		{
		case 'F':
		case 'f':
			strcpy (filp, &argv[ix][2]);
			break;
		case 'I':
		case 'i':
			IncCount++;
			strcpy (includes[IncCount], &argv[ix][2]);
			includes[IncCount][strlen(includes[IncCount])] = '/';
			break;
		case 'L':
		case 'l':
			strcpy (logp, &argv[ix][2]);
			break;
		case 'X':
		case 'x':
			strcpy (xmlp, &argv[ix][2]);
			break;
		case 'c':                           /* FF 2002-04-11 PTS 1115204 */
		case 'C':
			strcpy (PreProc, &argv[ix][2]);
			break;
		case 'p':                           /* FF 2002-04-11 PTS 1115204 */
		case 'P':
            if ( argv[ix][2] )
            {
			    strcpy (PreProcParms, &argv[ix][2]);
            }
            PreProcessFlag = TRUE;
            break;
        default:
        	printf ("unknown option '%s' ignored\n", &argv[ix][1]); /* FF 2002-04-11 PTS 1115204 */
            break;
		}
	}
	
	return TRUE;
}

IfiInitialization
(
 int    argc,
 char  **argv
 )
{
	bool           brc;
	unsigned short rc;
	unsigned short clg;
	unsigned short cml;
	unsigned long  lg;
	unsigned long  ml;
	char  in[MaxName];
	char  fil[MaxName];
	char  out[MaxName];
	char  log[MaxName];
	char  com[MaxName];
	st_d           out_d;
	
	brc = PmExecInitH (&pms, CbList, 0);
	
	if (!brc)
	{
		printf ("Parser initialization not successful.\n");
		return FALSE;
	}
	
	brc = IfiArg (argc, argv, in, out, log); 
	
	if (!brc)
	{
		printf ("No Arguments found.\n");
		return FALSE;
	}
	
	ml = MaxName;
	lg = strlen(in);
	
	brc = PmExecPattern (&pms, "p_path", (unsigned char*)in, &lg, ml);
	if (brc)
	{
		clg = MaxName;
		cml = MaxName;
		brc = PmExecFetch (&pms, "pth", (unsigned char*)paths[CurInput], &clg, cml);
		paths[CurInput][clg] = 0;
		brc = PmExecFetch (&pms, "fil", (unsigned char*)fil, &clg, cml);
		fil[clg] = 0;
		CurFile = 0;
		strcpy (files[CurFile], fil);
	}
	else
	{
		printf ("Input file '%s' has no path.\n", in);
		return FALSE;
	}


	CurInput  = 0;
	FilCount  = 1;
	SkipCount = 1;

	rc  = IfiFilOpen (in);
/*	rc  = RteFilOpen (fil_get, in, &filp[CurInput]); */
	
	if ((rc % 2) != 1) 
	{
		printf ("Input file '%s' could not be opened.\n", in);
		return FALSE;
	}
	
	/* o=C:\ppd\InterfaceInfo\Beispiel\Beispiel1.xml l=C:\ppd\InterfaceInfo\Beispiel\Beispiel1.log */
	if (strlen(out) == 0)
	{
		strcpy (out, paths[CurInput]);
		strcpy (&out[strlen(out)], fil);
		strcpy (&out[strlen(out)], ".xml");
	}
	
	rc  = RteFilOpen (fil_put, out, &xmlp);
	
	if ((rc % 2) != 1) 
	{
		printf ("Output file '%s' could not be opened.\n", out);
		return FALSE;
	}
	
	if (strlen(log) == 0)
	{
		strcpy (log, paths[CurInput]);
		strcpy (&log[strlen(log)], fil);
		strcpy (&log[strlen(log)], ".log");
	}
	
	rc  = RteFilOpen (fil_put, log, &logp);
	if ((rc % 2) != 1) 
	{
		printf ("Log file '%s' could not be opened.\n", log);
		return FALSE;
	}

	strcpy (com, paths[CurInput]);
	strcpy (&com[strlen(com)], fil);
	strcpy (&com[strlen(com)], ".cmt");
	rc  = RteFilOpen (fil_put, com, &comp);
	if ((rc % 2) != 1) 
	{
		printf ("Comment file '%s' could not be opened.\n", com);
		return FALSE;
	}

	
	strcpy (pm_line, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"yes\" ?>");
	out_d.lg = strlen (pm_line);
	out_d.pt = (unsigned char*)pm_line;
	rc = RteFilRecord (fil_put, xmlp, MaxLineLen, &out_d);

	strcpy (pm_line, "<IDL2XML>");
	out_d.lg = strlen (pm_line);
	out_d.pt = (unsigned char*)pm_line;
	rc = RteFilRecord (fil_put, xmlp, MaxLineLen, &out_d);

	return TRUE;
 }

 /* FF 2002-04-11 PTS 1115204 */
unsigned short IfiFilOpen
(
    char          *filnam
)
{
	unsigned short rc;
    int            rcs;
	bool           brc;
	char  pth[MaxName];
	char  fil[MaxName];
	char  typ[MaxName];
	unsigned long  lg;
	unsigned long  ml;
	unsigned short clg;
	unsigned short cml;
    char     sys[512];

    rc  = RteFilOpen (fil_get, filnam, &filp[CurInput]);
	if ((rc % 2) != 1)
	{
		return rc;
	}

    if (PreProcessFlag  && (0 < strlen( PreProc )) && (!strstr(filnam,"livecachetypes.idl")))
	{
        int ix;

	    lg = strlen(filnam);
	    ml = MaxName;
	    pms.cursor = 0;
	    brc = PmExecPattern (&pms, "p_path", (unsigned char *)filnam, &lg, ml);
    	if (brc)
		{
			clg = MaxName;
			cml = MaxName;
			brc = PmExecFetch (&pms, "pth", (unsigned char*)pth, &clg, cml);
			pth[clg] = 0;

			clg = MaxName;
			cml = MaxName;
			brc = PmExecFetch (&pms, "fil", (unsigned char*)fil, &clg, cml);
			fil[clg] = 0;

			clg = MaxName;
			cml = MaxName;
			brc = PmExecFetch (&pms, "typ", (unsigned char*)typ, &clg, cml);
			typ[clg] = 0;
            
            RteFilClose( &filp[CurInput] );
#if defined(WIN32)
            strcpy(sys,PreProc);
            strcat(sys," -EP ");
            strcat(sys,PreProcParms);
			for (ix = 1; ix <= IncCount; ix++)
			{
                char *p = NULL;
                strcat(sys," -I");
                strcat (sys, includes[ix]);
                if ( sys[strlen(sys)-1] == '/' ) sys[strlen(sys)-1] = '\0';
            }
            strcat(sys," -Tc");
            strcat(sys,filnam);
            strcat(sys," > ");
            strcat(sys,pth);
            strcat(sys,fil);
            strcat(sys,"__idl__pp__.i");
#else
            strcpy(sys,"cd ");
            strcat(sys,pth);
            strcat(sys,";");
            strcat(sys,"cp ");
            strcat(sys,fil);
            strcat(sys,".");
            strcat(sys,typ);
            strcat(sys," ");
            strcat(sys,fil);
            strcat(sys,"__idl__pp__.cpp;");
            strcat(sys,PreProc);
            strcat(sys," ");
            strcat(sys,PreProcParms);
			for (ix = 1; ix <= IncCount; ix++)
			{
                strcat(sys," -I");
                strcat (sys, includes[ix]);
            }
#if defined(LINUX)
            strcat(sys," -E ");
#else
            strcat(sys," -P ");
#endif
            strcat(sys,fil);
            strcat(sys,"__idl__pp__.cpp;");
#if defined(LINUX)
            strcat(sys," > ");
            strcat(sys,fil);
            strcat(sys,"__idl__pp__.i;");
#endif
            strcat(sys,"rm ");
            strcat(sys,fil);
            strcat(sys,"__idl__pp__.cpp");
#endif
            printf("all system(%s)\n",sys);
            rcs = system(sys);
            if ( !rcs )
            {
                strcpy(sys,pth);
                strcat(sys,fil);
                strcat(sys,"__idl__pp__.i");
#if defined(OSF1) 
                strcat(sys,"xx");
#endif
                rc  = RteFilOpen (fil_get, sys, &filp[CurInput]);
            }
            else
            {
				printf ("Preprocessing failed: '%s'\n", sys);
				printf ("Using file '%s' without being preprocessed: \n", filnam);
                rc  = RteFilOpen (fil_get, filnam, &filp[CurInput]);
            }
		}
	}

	return rc;

}

bool IfiOpen
(
	unsigned long  xlg
 )
{
	unsigned short rc;
	bool           brc;
	unsigned long  lg;
	unsigned long  ix;
	unsigned short clg;
	unsigned short cml;
	unsigned long  ml;
	st_d           out_d;
	char  filnam[MaxName];
	char  fil[MaxName];
	char  fname[MaxName];
	char  pname[MaxName];
	char  name[MaxName];
	char           morn;

	
	/* if "oaidl.idl" or "ocidl.idl", skip otherwise stack current file pointer and  */
	/* open new file */
	strcpy (filnam, paths[CurInput]);

	brc = PmExecFetch (&pms, "pth", (unsigned char*)fname, &clg, sizeof(filnam));
	
	if ((0 == strcmp("oaidl.idl", fname)) || (0 == strcmp("ocidl.idl", fname))) return TRUE;


	lg = strlen(fname);
	ml = MaxName;
	pms.cursor = 0;
	brc = PmExecPattern (&pms, "p_path", (unsigned char *)fname, &lg, ml);
	if (brc)
	{
		clg = MaxName;
		cml = MaxName;
		brc = PmExecFetch (&pms, "fil", (unsigned char*)name, &clg, cml);
		
		for (ix = 0; ix < CurFile; ix++)
		{
			if (0 == strcmp(files[ix], name))
			{
				SkipCount++;
				return TRUE;
			}
		}


        CurFile++;
		strcpy (files[CurFile], name);

		morn = pm_line[2];
		CurInput++;
		
		clg = MaxName;
		cml = MaxName;
		brc = PmExecFetch (&pms, "pth", (unsigned char*)pname, &clg, cml);
		if (0 == clg)
		{
			for (ix = 0; ix <= IncCount; ix++)
			{
				strcpy (filnam, includes[ix]);
				strcpy (&filnam[strlen(filnam)], fname);
				rc  = IfiFilOpen (filnam);
                /*rc  = RteFilOpen (fil_get, filnam, &filp[CurInput]);*/
				if ((rc % 2) == 1) 
				{
					FilCount++;
					break; 
				}
			}

			if ((rc % 2) != 1)
			{
				printf ("File not found: '%s'\n", filnam);
				return FALSE;
			}


		}
		else
		{
			strcpy (&filnam[strlen(filnam)], fname);
      /* +++ PTS 1109544   Pattern sollte alle '../' aus Pfad entfernen ! Warum ????
      while (brc)
			{
				lg = strlen(filnam);
				ml = MaxName;
				pms.cursor = 0;
				brc = PmExecPattern (&pms, "p_filecompr", (unsigned char*)filnam, &lg, ml);
				filnam[lg] = 0;
			}
      * --- PTS 1109544 */

			lg = strlen(filnam);
			ml = MaxName;
			pms.cursor = 0;
			brc = PmExecPattern (&pms, "p_path", (unsigned char*)filnam, &lg, ml);
			if (brc)
			{
				clg = MaxName;
				cml = MaxName;
				brc = PmExecFetch (&pms, "pth", (unsigned char*)paths[CurInput], &clg, cml);
				paths[CurInput][clg] = 0;
				brc = PmExecFetch (&pms, "fil", (unsigned char*)fil, &clg, cml);
				fil[clg] = 0;
			}
			else
			{
				printf ("Input file '%s' has no path.\n", paths[CurInput]);
				return FALSE;
			}
			
			rc  = IfiFilOpen (filnam); 
			/* rc  = RteFilOpen (fil_get, filnam, &filp[CurInput]);*/
			if ((rc % 2) != 1) 
			{
				printf ("File not found: '%s'\n", filnam);
				return FALSE;
			}
			FilCount++;
		}
	}
	else
	{
		printf ("Incorrect file format '%s'\n", fname);
		return FALSE;
	}
	
	
	pm_line[xlg] = 0;
	printf ("%s \n", pm_line);
	
	
	inorim[CurInput] = morn;  /* 'N' or 'M' */
	out_d.lg = (unsigned short) xlg; 
	out_d.pt = (unsigned char*)pm_line;
	rc = RteFilRecord (fil_put, xmlp, MaxLineLen, &out_d);
	
	return TRUE;
	
}
 
 bool IfiRead
	 (
	 )
 {
	 unsigned short rc;
	 bool           brc;
	 unsigned long  lg;
	 unsigned short clg;
	 unsigned long  ml;
	 char			cty;
	 st_d           buf_d;
	 st_d           out_d;
	 st_d           com_d;
	 unsigned char  com[MaxLineLen];
	 
	 
	 buf_d.pt = (unsigned char*)pm_line;
	 CurLineLen = 0;
	 
	 /* This read loop eliminates all comments and blank lines. */
	 /* If an include or import statement is detected, the current file pointer is stacked,  */
	 /* the new file is opened and read. On end of current file, the stacked file is reactivated. */
	 /* End of file of the first file is end of input. */
	 /* "oaidl.idl" and "ocidl.idl" are ignored. */
	 
	 for(;;)
	 {
		 rc = RteFilRecord (fil_get, filp[CurInput], MaxLineLen, &buf_d);
		 if ((rc % 2) != 1) 
		 {
			 if (0 == CurInput)
			 {
				 return FALSE;  /* end of input */
			 }
			 else
			 {
				 if (inorim[CurInput] == 'N') strcpy (pm_line, "</INCLUDE>");
				 else                         strcpy (pm_line, "</IMPORT>");
				 
				 out_d.lg = strlen(pm_line); 
				 out_d.pt = (unsigned char*)pm_line;
				 rc = RteFilRecord (fil_put, xmlp, MaxLineLen, &out_d);
				 
				 CurInput--;   /* activate previous file */
				 continue;
			 }
		 }
		 
		 RecCount++;
		 ml         = MaxLineLen;
		 lg         = buf_d.lg;
		 pms.cursor = 0;
		 
		 for (;;)
		 {
             /*if (0 == memcmp (pm_line, "cpp_quote", 9)) break; */
             memcpy (com, pm_line, buf_d.lg);
			 /* Look for comments */
			 brc = PmExecPattern (&pms, "p_combeg", (unsigned char*)pm_line, &lg, ml);
			 if (brc)
			 {
				 com_d.lg = buf_d.lg;
				 com_d.pt = com;

          		 rc = RteFilRecord (fil_put, comp, MaxLineLen, &com_d);
				 /* fetch comment type '/' or '*'  */
				 brc = PmExecFetch (&pms, "cty", (unsigned char*)&cty, &clg, 1);
				 if (cty == '/')  
				 {
					 /* Comment at end of line */
					 lg = pms.cursor;
					 break; 
				 }
				 else
				 {
					 /* look for end of comment, may be multi-line comment */
					 for (;;)
					 {
						 brc = PmExecPattern (&pms, "p_comend", (unsigned char*)pm_line, &lg, ml);
						 if (!brc)
						 {
							 /* end of comment not found, try next line */
							 buf_d.pt = (unsigned char*)(pm_line + pms.cursor);
							 rc = RteFilRecord (fil_get, filp[CurInput], (unsigned short)(MaxLineLen - pms.cursor), &buf_d);
							 if ((rc % 2) != 1) return FALSE;
			          		 rc = RteFilRecord (fil_put, comp, MaxLineLen, &buf_d);
							 lg = buf_d.lg + pms.cursor;
						 }
						 else
						 {
							 /* end of commnent */
							 break;
						 }
					 }
				 }
			 }
			 else
			 {
				 /* No comment on line */
				 break; 
			 }
		 }
		 
		 pms.cursor = 0;
		 
		 brc = PmExecPattern (&pms, "p_emptyline", (unsigned char*)pm_line, &lg, ml);
		 if (brc)
		 {
			 buf_d.pt = (unsigned char*)pm_line;
		 }
		 else 
		 {
			 /* look for 'import' or 'include' */
			 brc = PmExecPattern (&pms, "p_nestedfile", (unsigned char*)pm_line, &lg, ml);
			 if (brc)
			 {
				 brc = IfiOpen(lg);
				 if (!brc)
				 {
					 error = TRUE;
					 return FALSE;
				 }
			 }
			 else
			 {
				 CurLineLen = lg; 
				 break;
			 }
		 }
  }
  return TRUE;
}

bool IfiMain
(
 )
{
	
	bool           brc;
	bool           fin;
	unsigned short rc;
	unsigned short clg;
	unsigned char  typ;
	unsigned long  lg;
	unsigned long  ml;
	char           pat[30];
	st_d           log_d;
	st_d           out_d;
	bool           LibraryIDL;

	
	CurLineLen = 0;
	LibraryIDL = FALSE;

	for (;;)
	{
		/* read an input line without comment */
		brc = IfiRead();
		if (!brc) break;
		
		lg           = CurLineLen;
		ml           = MaxObjectLen;
		CurObjectLen = lg;
		memcpy (pm_object, pm_line, lg);
		strcpy (pat, "p_beg");
		
		fin = FALSE;
		
		for (;;)
		{
			/* look for begin pattern */
			pms.cursor   = 0;
			brc = PmExecPattern (&pms, "p_beg", (unsigned char*)pm_object, &lg, ml);
			if (brc)
			{
				if (fin)  break;
				
				brc = PmExecFetch (&pms, "beg", (unsigned char*)&typ, &clg, 1);
				if (typ == '[')
				{
					/* statement type not yet recognized, append next line */
					brc = IfiRead();
					if (brc)
					{
						memcpy (&pm_object[CurObjectLen], pm_line, CurLineLen);
						CurObjectLen += CurLineLen;
						pm_object[CurObjectLen] = 0;
						lg = CurObjectLen;
					}
				}
				else
				{
					/* if statement type is known */
					switch (typ)
					{
					case 'e': strcpy (pat, "p_end");     break; /* enum */
					case 't': strcpy (pat, "p_end");     break; /* typedef */
					case 's': strcpy (pat, "p_end");     break; /* struct */
					case 'i': strcpy (pat, "p_end");     break; /* interface   */   
					case 'T': strcpy (pat, "p_end");     break; /* HRESULT   */   
					case 'o': strcpy (pat, "p_end");	 break; /* coclass */
					case 'p': strcpy (pat, "p_pragma");  break; /* pragma */
					case 'q': strcpy (pat, "p_cpp");     break; /* cpp quote */
					case '#': strcpy (pat, "p_hash");    break; /* hash */
					case 'l': strcpy (pat, "p_library"); LibraryIDL = TRUE; break; /* library */
					case 'c': strcpy (pat, "p_const");   break; /* const */
					}
					/* search for 'complete' pattern */
					for (;;)
					{
						/* incomplete (p_end) or complete (i.e. p_cpp) pattern */
						pms.cursor   = 0;
						fin = PmExecPattern (&pms, pat, (unsigned char*)pm_object, &lg, ml);
						if (fin) 
						{
							/* execute 'complete' pattern, if incomplete */
							if (0 == strcmp(pat, "p_end"))
							{
								pms.cursor   = 0;
								fin = PmExecPattern (&pms, "p_cont", (unsigned char*)pm_object, &lg, ml);
							}
							break; 
						}
						else
						{
							if (0 == strcmp(pat, "p_end"))
							{
								/* append next line */
								brc = IfiRead();
								if (brc)
								{
									memcpy (&pm_object[CurObjectLen], pm_line, CurLineLen);
									CurObjectLen += CurLineLen;
									pm_line[CurLineLen] = 0;
									lg = CurObjectLen;
								}
							}
							else break;
						}
					}
                    if (fin)
                    {
						/* complete statement is converted to xml and written to the xml file */
						out_d.lg = (unsigned short) lg; 
						out_d.pt = (unsigned char*)pm_object;
						rc = RteFilRecord (fil_put, xmlp, MaxObjectLen, &out_d);
						break;
					}
					else
					{
						/* incorrect statement */
						log_d.lg = (unsigned short) CurLineLen;
						log_d.pt = (unsigned char*)pm_line;
				
						rc = RteFilRecord (fil_put, logp, MaxLineLen, &log_d);
						pm_object[lg] = 0;
						printf ("The following statement is ignored:\n%s\n", pm_object);
						break;
					}
				}
			}
			else
			{
				/* unrecognized input is logged */
				log_d.lg = (unsigned short) CurLineLen;
				log_d.pt = (unsigned char*)pm_line;
				
				rc = RteFilRecord (fil_put, logp, MaxLineLen, &log_d);
				pm_object[lg] = 0;
				printf ("The following statement is ignored:\n%s\n", pm_object);
				break;
			}
		}
	}
	
	/* sorry, I'm too laisy to look for the end of library statement */

	if (LibraryIDL)
	{
	
		strcpy (pm_object, "</INTERFACE>");
		out_d.lg = strlen (pm_object);
		out_d.pt = (unsigned char*)pm_object;
		rc = RteFilRecord (fil_put, xmlp, MaxObjectLen, &out_d);
		strcpy (pm_object, "</LIBRARY>");
		out_d.lg = strlen (pm_object);
		out_d.pt = (unsigned char*)pm_object;
		rc = RteFilRecord (fil_put, xmlp, MaxObjectLen, &out_d);
    }

	strcpy (pm_object, "</IDL2XML>");
	out_d.lg = strlen (pm_object);
	out_d.pt = (unsigned char*)pm_object;
	rc = RteFilRecord (fil_put, xmlp, MaxObjectLen, &out_d);


	
	rc  = RteFilClose (&logp);
	return !error;
}


int main
(
 int    argc,
 char  **argv
 )
{
	bool brc;

	error = FALSE;

	brc = IfiInitialization (argc, argv);
	
	if (brc) brc = IfiMain();

	printf ("%d records in %d files read, %d files skipped\n", RecCount, FilCount, SkipCount);
    if (brc) return 0;
	else return 1;
}

