/*!*********************************************************************

    @file         Proc_JavaUDE.cpp

    @author       DanielD

    @brief        start Java UDE server

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

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>

#if defined (_WIN32)
#include <process.h>
#else
#include <unistd.h>
#endif

#include "gsp01.h"

const char * True = "True";
const char * False = NULL;

#if defined (_WIN32)
#define PATH_DELIMITER ';'
#else
#define PATH_DELIMITER ':'
#endif

/*----------------------------------------*/

static const char * logfileName = "c:/SAPDevelop/V76/develop/ude.log";

static bool
logFileExists ()
{
    return true;
    static bool hasBeenChecked = false;
    static bool fileExists = false;

    struct stat statInfo;

    if (hasBeenChecked) {
        return fileExists;
    }
    int rc = stat (logfileName, &statInfo);
    if (rc == 0) {
        fileExists = true;
    }
    else {
        fileExists = false;
    }
    hasBeenChecked = true;
    return fileExists;
}

/*----------------------------------------*/

static void
logf (
    const char * format,
    ...)
{
    if (!logFileExists ()) {
        va_list argptr;
        va_start (argptr, format);
        va_end (argptr);
        return;
    }
    FILE * logstream = fopen (logfileName, "a");
    if (logstream) {
        va_list argptr;
        va_start (argptr, format);
        fprintf (logstream, "%08s %04d-%02d-%02d %02d:%02d:%02d: ",
            "jstarter", 0, 0, 0, 0, 0, 0);
        vfprintf (logstream, format, argptr);
        fputc ('\n', logstream);
        va_end (argptr);
        fclose (logstream);
    }
}

/*----------------------------------------*/

typedef struct OptionsT {
    const char * java;
    const char * classpath;
    const char * mainclass;
    const char * configfile;
    const char * instroot;
    const char * strict;
    const char * dummy;
} OptionsT;

/*----------------------------------------*/

static const OptionsT defaultOptionsC = {
    NULL,   // java
    NULL,   // classpath
    "com.sap.dbtech.procserver.ProcServer", // mainclass
    NULL,   // configfile
    NULL,   // instroot
    False,  // strict
    NULL
};

/*----------------------------------------*/

typedef struct OptionDefT {
    const char * text;
    const char * help;
    int          offset;
    bool         hasArg;
    char         id;
} OptionDefT;


static const OptionDefT optionsMap [] = {
    {"h", "help", -1, false, 'h'},
    {"?", "help", -1, false, 'h'},
    {"help", "help", -1, false, 'h'},
    {"java", "path to java executable",
        offsetof (OptionsT, java), true, 'J'},
    {"classpath", "class search path of directories and zip/jar files",
        offsetof (OptionsT, classpath), true, 'P'},
    {"mainclass", "main java class of UDE server",
        offsetof (OptionsT, mainclass), true, 'M'},
    {"configfile", "configuration file containing additional options",
        offsetof (OptionsT, configfile), true, 'C'},
    {"instroot", "<dependent_path>",
        offsetof (OptionsT, instroot), true, 'R'},
    {"strict", "show errors when parsing configuration files",
        offsetof (OptionsT, strict), false, 's'},
    {NULL, NULL, -1, false, '\0'}
};

/*----------------------------------------*/

static void
optionHelp (
    const char * unknownOption = NULL)
{
    int rc = 0;
    if (unknownOption != NULL) {

        logf ("unknown Option -%s", unknownOption);
        rc = 2;
    }
    logf ("help for javaude");
    exit (rc);
}

/*----------------------------------------*/

static const char **
optionValueAddr (
    OptionsT * options,
    const OptionDefT * optionDef)
{
    char * raw = (char *) options;
    const char ** valueAddr = (const char **) (raw + optionDef->offset);
    return valueAddr;
}

/*----------------------------------------*/

static void
dumpOptions (
    OptionsT * options)
{
    char * raw = (char *) options;
    const OptionDefT * current = &optionsMap [0];
    while (current->text != NULL) {
        if (current->offset >= 0) {
            const char * value = *optionValueAddr (options, current);
            if (value == NULL) {
                value = "<null>";
            }
            logf ("%s: '%s'", current->text, value);
        }
        ++current;
    }
}

/*----------------------------------------*/

static const OptionDefT *
findOptionDef (
    const char * option)
{
    const OptionDefT * current = &optionsMap [0];
    while (current->text != NULL) {
        if (strcmp (current->text, option) == 0) {
            return current;
        }
        ++current;
    }
    return NULL;
}

/*----------------------------------------*/

static void
blockComments (
    char * line)
{
    char * commentPos = strchr (line, '#');
    if (commentPos != NULL) {
        *commentPos = '\0';
    }
}

/*----------------------------------------*/


static void
stripWhitespace (
    const char * intext,
    int inlen,
    const char ** outtext,
    int * outlen)
{
    const char * eotext = intext + inlen - 1;
    while ((eotext > intext) && isspace (*eotext)) {
        --eotext;
    }
    while (isspace (*intext)) {
        ++intext;
    }
    *outtext = intext;
    *outlen = eotext - intext + 1;
}

/*----------------------------------------*/

static const OptionDefT *
findOptionDefFromLine (
    const char * line,
    bool strict)
{
    /*
     * search for : as delimiter of identifier and data
     */
    const char * colonPos = strchr (line, ':');
    if (colonPos == NULL) {
        if (strict) {
            const char * lineAddr;
            int lineLen;
            stripWhitespace (line, strlen (line), &lineAddr, &lineLen);
            if (lineLen > 0) {
                logf ("invalid configuration line: '%.*s'",
                    lineLen, lineAddr);
            }
        }
        return NULL;
    }
    /*
     * parse identifier
     */
    char identifier [40];
    const char * identifierAddr;
    int identifierLen;
    stripWhitespace (line, colonPos - line, &identifierAddr, &identifierLen);
    if (identifierLen > sizeof (identifier)) {
        return NULL;
    }
    memcpy (identifier, identifierAddr, identifierLen);
    identifier [identifierLen] = '\0';
    /*
     * search for identifier in option definitions
     */
    const OptionDefT * result = findOptionDef (identifier);
    if (strict && (result == NULL)) {
        logf ("configuration variable %s not valid", identifier);
    }
    return result;
}

/*----------------------------------------*/

const char *
extractConfigValue (
    const char * line)
{
    const char * valueStart = strchr (line, ':') + 1;
    const char * valueAddr;
    int valueLen;
    stripWhitespace (valueStart, strlen (valueStart), &valueAddr, &valueLen);
    char * result = (char *)malloc (valueLen + 1);
    memcpy (result, valueAddr, valueLen);
    result [valueLen] = '\0';
    return result;
}

/*----------------------------------------*/

static void
readConfigFile (
    OptionsT * options)
{
    bool strict = (options->strict == True);
    FILE * instream = fopen (options->configfile, "r");
    if (instream == NULL) {
        if (strict) {
            logf ("configfile '%s' not found", options->configfile);
        }
        return;
    }
    char line [1000];
    while (fgets (line, sizeof (line) - 1, instream)) {
        line [sizeof (line) - 1] = '\0';
        blockComments (line);
        const OptionDefT * optionDef = findOptionDefFromLine (line, strict);
        if (optionDef != NULL) {
            *optionValueAddr (options, optionDef) = extractConfigValue (line);
        }
    }
    fclose (instream);
}

/*----------------------------------------*/

static int
parseOptions (
    int argc,
    const char * argv [],
    OptionsT * options)
{
    bool inOptions = true;
    int argi = 1;
    while (inOptions && (argi < argc)) {
        const char * current = argv [argi];
        /*
         * detect end of options
         */
        if (current [0] != '-') {
            inOptions = false;
            break;
        }
        // -- ends options as well
        if (current [1] == '-') {
            inOptions = false;
            ++argi;
            break;
        }
        /*
         * find option description
         */
        ++argi;
        const OptionDefT * optionDef = findOptionDef (current + 1);
        if (optionDef == NULL) {
            optionHelp (current + 1);
            return argi;
        }
        /*
         * default handling of arguments
         */
        const char * argument = NULL;
        if (optionDef->hasArg) {
            *optionValueAddr (options, optionDef) = argv [argi];
            ++argi;
        }
        else if (optionDef->offset >= 0) {
            *optionValueAddr (options, optionDef) = True;
        }
        /*
         * handle option
         */
        switch (optionDef->id) {
            case 'h':
                optionHelp ();
                return argi;
                break;
            case 'C':
                readConfigFile (options);
                break;
        }
    }
    return argi;
}

/*----------------------------------------*/

static void
checkOptions (
    OptionsT * options)
{
    bool hasErrors = false;
    if (options->java == NULL) {
        logf ("option -java must be specified");
        hasErrors = true;
    }
    FILE * javaexecutable = fopen (options->java, "rb");
    if (javaexecutable == NULL) {
        logf ("option -java must point to an executable");
        hasErrors = true;
    }
    else {
        fclose (javaexecutable);
        // TODO: UNIX: check file is executable
    }
    if (options->instroot == NULL) {
        logf ("option -instroot must be specified");
        hasErrors = true;
    }
    if (hasErrors) {
        exit (2);
    }
}

/*----------------------------------------*/

static char *
incCopy (
    char * target,
    const char * data,
    int datalen)
{
    memcpy (target, data, datalen);
    return target + datalen;
}

/*----------------------------------------*/

static void
patchPathEnvironment (
    OptionsT * options)
{
#if defined (_WIN32)
    const char * varname = "PATH";
#else
    const char * varname = "LD_LIBRARY_PATH";
#endif
    int varnameLen = strlen (varname);
    const char * path = getenv (varname);
    int pathlen;
    char * newpath;
    int newpathlen;
    int instrootLen = strlen (options->instroot);

    /*
     * calculate length
     */
    newpathlen = varnameLen + 1 + instrootLen + 4 + 1; // "PATH=%s/pgm"
    if (path != NULL) {
        pathlen = strlen (path);
        newpathlen += pathlen + 1;
    }
    newpath = new char [newpathlen];
    /*
     * copy parts
     */
    char * writer = incCopy (newpath, varname, varnameLen);
    writer [0] = '=';
    writer = incCopy (writer + 1, options->instroot, instrootLen);
    writer = incCopy (writer, "/pgm", 4);
    if (path != NULL) {
        writer [0] = PATH_DELIMITER;
        writer = incCopy (writer + 1, path, pathlen);
    }
    writer [0] = '\0';
    /*
     *
     */
    putenv (newpath);
}

/*----------------------------------------*/

static void
patchClasspathEnvironment (
    OptionsT * options)
{
    char * newpath;
    int newpathLen;
    int userpathLen;
    int instrootLen = strlen (options->instroot);
    static const char * udejar = "/runtime/jar/javaudeserver.jar";
    int udejarLen = strlen (udejar);
    static const char * jdbcjar = "/runtime/jar/sapdbc.jar";
    int jdbcjarLen = strlen (jdbcjar);
    /*
     * calculate length
     */
    newpathLen = 10 + instrootLen + udejarLen + 1 + instrootLen + jdbcjarLen + 1;
    if (options->classpath != NULL) {
        userpathLen = strlen (options->classpath);
        newpathLen += 1 + userpathLen;
    }
    newpath = new char [newpathLen];
    /*
     * copy
     */
    char * writer = incCopy (newpath, "CLASSPATH=", 10);
#if 1
    if (options->classpath != NULL) {
        writer = incCopy (writer, options->classpath, userpathLen);
        writer [0] = PATH_DELIMITER;
        ++writer;
    }
#endif
    writer = incCopy (writer, options->instroot, instrootLen);
    writer = incCopy (writer, udejar, udejarLen);
    writer [0] = PATH_DELIMITER;
    writer = incCopy (writer + 1, options->instroot, instrootLen);
    writer = incCopy (writer, jdbcjar, jdbcjarLen);
#if 0
    if (options->classpath != NULL) {
        writer [0] = PATH_DELIMITER;
        writer = incCopy (writer + 1, options->classpath, userpathLen);
    }
#endif
    writer [0] = '\0';
    /*
     *
     */
    putenv (newpath);
}

/*----------------------------------------*/

static const char **
buildArgv (
    OptionsT * options,
    int argi,
    int argc,
    const char ** argv)
{
    int resultCount = argc - argi + 3; /* add
                                            - argv [0]
                                            - main class
                                            - closing NULL */
    const char ** result = (const char **) calloc (resultCount, sizeof (char *));
    result [0] = options->java;
    result [1] = options->mainclass;
    int resulti;

    for (resulti = 2; argi < argc; ++resulti, ++argi) {
        result [resulti] = argv [argi];
    }
    result [resulti] = NULL;
    //for (int i =0; result [i] != NULL; ++i) {
    //    fprintf (logfile (), "new argv %d: %s\n", i, result [i]);
    //}
    return result;
}

/*----------------------------------------*/

static void
startUdeServer (
    OptionsT * options,
    int argi,
    int argc,
    const char ** argv)
{
    /*
     * patch environment
     */
    patchPathEnvironment (options);
    patchClasspathEnvironment (options);
    /*
     * create command line args
     */
    const char ** execArgv = buildArgv (options, argi, argc, argv);
    /*
     * start program
     */
    execv (options->java, (char * const *) execArgv);
    logf ("execv failed");
}

/*----------------------------------------*/

int
main (
    int argc,
    const char * argv [])
{
    logf ("javaude started");
    OptionsT options;
    memcpy (&options, &defaultOptionsC, sizeof (options));
    int argi = parseOptions (argc, argv, &options);
    logf ("options read");
    dumpOptions (&options);
    checkOptions (&options);
    logf ("options checked");
    startUdeServer (&options, argi, argc, argv);
    logf ("java started");
    return 0;
}

/*----------------------------------------*/

#if defined (_WIN32)

#include <windows.h>

INT APIENTRY
WinMain (
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpszCmdLine,
    INT       nCmdShow )
{
    int argc = 1;
    int maxCount = 32;
    const char ** argv = (const char **) calloc (32, sizeof (char*));
    argv [0] = "javaude";
    const char * line = lpszCmdLine;
    const char * eoline = line + strlen (line);
    /*
     * break into arguments
     */
    const char * current = line;
    while (current < eoline) {
        const char * argstart;
        int arglen;
        /*
         * skip space
         */
        while (isspace (*current)) {
            ++current;
        }
        if (*current == '"') {
            /*
             * handle quoted args
             */
            ++current;
            argstart = current;
            while (*current && (*current != '"')) {
                ++current;
            }
            arglen = current - argstart;
            ++current; // skip closing quote
        }
        else {
            /*
             * handle space delimited args
             */
            argstart = current;
            while (*current && !isspace (*current)) {
                ++current;
            }
            arglen = current - argstart;
        }
        /*
         * grow argv
         */
        if (argc == maxCount) {
            argv = (const char **) realloc (argv, maxCount * 2);
            maxCount *= 2;
        }
        /*
         * strdup current argument
         */
        char * newarg = (char *) malloc (arglen + 1);
        memcpy (newarg, argstart, arglen);
        newarg [arglen] = '\0';
        argv [argc] = newarg;
        ++argc;
    }
    /*
     * call main
     */
    return main (argc, argv);
}

#endif

