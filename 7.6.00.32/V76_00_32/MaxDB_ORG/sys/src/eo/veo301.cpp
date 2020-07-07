/*!
  @file           veo301.cpp
  @author         JoergM
  @special area   
  @brief          
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

#if defined (_WIN32)
    extern "C" int        WinArgc;
    extern "C" char     **WinArgv;
#endif


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

extern "C" {
    int sql__pmain(int);
    int PASCAL_MAIN(void);
    int sql__exit(int);
    extern int     _argc;
    extern char ** _argv;
    extern char ** _envp;
};

int main(int argc,
         char *argv[],
         char *envp[] )
{
#if defined (_WIN32)
    WinArgc = argc;
    WinArgv = argv;
#else
    _argc = argc;
    _argv = argv;
    _envp = envp;
#endif

    sql__pmain(0);

    PASCAL_MAIN();

    sql__exit(0);
    return 0;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/