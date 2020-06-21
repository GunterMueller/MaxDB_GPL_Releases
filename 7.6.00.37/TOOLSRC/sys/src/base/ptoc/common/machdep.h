/* @(#)common/machdep.h

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

/* Machine dependent front end stuff */

/* Patterns for building special names */

#  define GLOBALLAB_NAME "L%d"
#  define BUILDNAME_CHAR '.'

#if PYRAMYD
#  define FUNCVAR_NAME "$%s"
#  define STRUCTFUNCVAR_NAME "$$%s"
#else
#  define FUNCVAR_NAME ".%s"
#  define STRUCTFUNCVAR_NAME "..%s"
#endif


/* special register numbers */

#if PYRAMYD             /* frame pointer has different register numbers */
#  define FRAMEPOINTER_REG 13
#else
#  define FRAMEPOINTER_REG 6
#endif

/* front end relevant machine properties */

#ifdef PYRAMID_DOUBTFUL
#  define NO_AUTONR		/* storage class AUTONR (auto, no register) not used */ 	
#  define NO_ELIM_CONVERT	/* too m68k dependant to use here		     */
#endif

