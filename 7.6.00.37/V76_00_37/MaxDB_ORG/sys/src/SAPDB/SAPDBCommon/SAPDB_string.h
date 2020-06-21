/*!***************************************************************************

  @file           SAPDB_string.h
  @author         JoergM
  @ingroup        Runtime
  @brief          C String and memset,memcpy, memmove definitions (replaces string.h)
  
  C Strings in SAPDB have enforced limited length of maximum 2 GigaByte
  (SAPDB_strlen is casted to return an SAPDB_Int4...) 


  No strdup() equivalent wanted!
  
  -------------------------------------------------------------------------

  copyright:   Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




*****************************************************************************/

#ifndef SAPDB_STRING_H
#define SAPDB_STRING_H

#include <string.h>

/* redefined type definitions */
#ifdef WIN32
/*! compare s1_ and s2_ case insensitive */
#define SAPDB_strcasecmp(s1_,s2_)     stricmp( (s1_), (s2_) )
/*! compare s1_ and s2_ case insensitive but atmost n_ characters */
#define SAPDB_strncasecmp(s1_,s2_,n_) strnicmp( (s1_), (s2_), (n_) )
#else
/*! compare s1_ and s2_ case insensitive */
#define SAPDB_strcasecmp(s1_,s2_)     strcasecmp( (s1_), (s2_) )
/*! compare s1_ and s2_ case insensitive but atmost n_ characters */
#define SAPDB_strncasecmp(s1_,s2_,n_) strncasecmp( (s1_), (s2_), (n_) )
#endif

/*! compare s1_ and s2_ case sensitive */
#define SAPDB_strcmp(s1_,s2_)         strcmp( (s1_), (s2_) )
/*! compare s1_ and s2_ case sensitive but atmost n_ characters */
#define SAPDB_strncmp(s1_,s2_,n_)     strncmp( (s1_), (s2_), (n_) )

/*! copy string src_ into dest_ */
#define SAPDB_strcpy(dest_,src_)      strcpy( (dest_), (src_) )
/*! copy atmost n_ characters from string src_ into dest_ */
#define SAPDB_strncpy(dest_,src_,n_)  strncpy( (dest_), (src_), (n_) )

/*!
  @fn void SAPDB_strncpy_zero_terminated(char *dest, char const *source, size_t n)

  @brief copy atmost n_ characters from string src_ into dest_ with enforced zero termination

  This function is almost identical to 'strncpy' but differes in handling of zero termination.
  The destination buffer is always zero terminated even if that means to ignore the last source
  buffer character at position source[n-1]. 

  This function does not return anything! This allows to implement it as macro!

  @param dest [out] destination buffer to fill
  @param source [in] source buffer
  @param n [in] number of character to copy.
  @return none
 */
void SAPDB_strncpy_zero_terminated(char *dest, char const *source, size_t n);

/*! copy atmost n_ characters from string src_ into dest_ with enforced zero termination
    Since it is a macro definition, we have to copy arguments on local
    variables. Doubled usage of macro expansion produces side effects....
 */
#define SAPDB_strncpy_zero_terminated(dest_,src_,n_) \
 { \
   size_t n = (size_t)(n_); \
   if ( n > 0 ) \
   { \
       char *dst = (dest_); \
       strncpy( dst, (src_), n ); \
       dst[n-1] = 0; \
   } \
 }

/*! append string src_ to dest_ */
#define SAPDB_strcat(dest_,src_)       strcat( (dest_), (src_) )
/*! append atmost n_ character of string src_ to dest_ */
#define SAPDB_strncat(dest_,src_,n_)   strncat( (dest_), (src_), (n_) )

/*!
  @fn void SAPDB_strncat_zero_terminated(char *dest, char const *source, size_t n)

  @brief append atmost n_ characters from string src_ into dest_ with enforced zero termination

  This function is almost identical to 'strncpy' but differes in handling of zero termination.
  The destination buffer is always zero terminated even if that means to ignore the last source
  buffer character at position source[n-1]. 

  This function does not return anything! This allows to implement it as macro!

  @param dest [out] destination buffer to fill
  @param source [in] source buffer
  @param n [in] number of character to copy.
  @return none
 */
void SAPDB_strncat_zero_terminated(char *dest, char const *source, size_t n);

/*! append atmost n_ characters from string src_ into dest_ with enforced zero termination
    Since it is a macro definition, we have to copy arguments on local
    variables. Doubled usage of macro expansion produces side effects....
 */
#define SAPDB_strncat_zero_terminated(dest_,src_,n_) \
 { \
   size_t n = (size_t)(n_); \
   if ( n > 0 ) \
   { \
       char *dst = (dest_); \
       dst += strlen(dst); \
       strncpy( dst, (src_), n ); \
       dst[n-1] = 0; \
   } \
 }

/*! search character 'c_' in string 's_' from start */
#define SAPDB_strchr(s_,c_)           strchr( (s_), (c_) )
/*! reverse search character 'c_' in string 's_' from end */
#define SAPDB_strrchr(s_,c_)          strrchr( (s_), (c_) )

/*! calculate string length of string 's_' explicit downcast to 'SAPDB_Int4' */
#define SAPDB_strlen(s_)              ((SAPDB_Int4)strlen( s_ ))

/* strdup falls into the category malloc/free which is not wanted at all... */

/*! fill memory at 's_' bytewise using 'n_' times fill character 'c_' */
#define SAPDB_memset(s_,c_,n_)        memset((s_),(c_),(n_))

/*! copy memory 'n_' bytes from 'src_' into 'dst_' buffer. 'src_' and 'dest_' may not overlap */
#define SAPDB_memcpy(dst_, src_, n_)  memcpy((dst_), (src_), (n_))

/*! copy memory 'n_' bytes from 'src_' into 'dst_' buffer. 'src_' and 'dest_' may overlap */
#define SAPDB_memmove(dst_, src_, n_) memmove((dst_), (src_), (n_))

/*! compare memory 'n_' bytes from 'src_' into 'dst_' buffer. 'src_' and 'dest_' may not overlap */
#define SAPDB_memcmp(dst_, src_, n_)  memcmp((dst_), (src_), (n_))

#endif /* SAPDB_STRING_H */
