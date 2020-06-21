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
#include "idl/idl2xml/pms00.h"
#include "idl/idl2xml/pmdesc.h"

bool PmdDInit 
(
 st_sdc_p   dp,
 ub_int_p   pt,
 us_int     ml,
 char_p     cp
)
{
  us_int   lg;
  bool     rc;

  lg = strlen (cp);
  rc = lg <= ml;

  if (!rc) lg = ml;

  dp->lg = lg;
  dp->ml = ml;
  dp->pt = pt;

  if (lg > 0) memcpy (pt, cp, lg);
 
  if (lg < ml) memset (pt + lg, 0, ml - lg);
 
  return rc;
}

bool PmdDAppend 
(
 st_sdc_p   tp,
 st_sdc_p   fp
)
{
  us_int   lg;
  bool     rc;

  lg = fp->lg;
  rc = tp->ml >= (tp->lg + lg);

  if (!rc) lg = tp->ml - tp->lg;

  memcpy (tp->pt + tp->lg, fp->pt, lg);
  tp->lg += lg; 

  return rc;
}


bool PmdDCopy
(
 st_sdc_p   tp,
 st_sdc_p   fp
)
{
  us_int   lg;
  bool     rc;

  lg = fp->lg;
  rc = tp->ml >= lg;

  if (!rc) lg = tp->ml;

  memcpy (tp->pt, fp->pt, lg);
  tp->lg = lg; 

  return rc;
}

bool PmdSAppend 
(
 st_sdc_p   tp,
 char_p     cp
)
{
  us_int   lg;
  bool     rc;

  lg = strlen (cp);

  rc = tp->ml >= (tp->lg + lg);

  if (!rc) lg = tp->ml - tp->lg;

  memcpy (tp->pt + tp->lg, cp, lg);
  tp->lg += lg;

  return rc;
}


bool PmdSCopy
(
 st_sdc_p   tp,
 char_p     cp
)
{
  us_int   lg;
  bool     rc;

  lg = strlen (cp);

  rc = tp->ml >= lg;

  if (!rc) lg = tp->ml;

  memcpy (tp->pt, cp, lg);
  tp->lg = lg;

  return rc;
}

bool PmdIAppend 
(
 st_sdc_p   tp,
 us_int     ui
)
{
  us_int   lg;
  bool     rc;
  char     is[10];

  sprintf (is, "%d", ui);

  lg = strlen (is);

  rc = tp->ml >= (tp->lg + lg);

  if (!rc) lg = tp->ml - tp->lg;

  memcpy (tp->pt + tp->lg, is, lg);
  tp->lg += lg; 

  return rc;
}

bool PmdICopy
(
 st_sdc_p   tp,
 us_int     ui
)
{
  us_int   lg;
  bool     rc;
  char     is[10];

  sprintf (is, "%d", ui);

  lg = strlen (is);

  rc = tp->ml >= lg;

  if (!rc) lg = tp->ml - tp->lg;

  memcpy (tp->pt, is, lg);
  tp->lg = lg; 

  return rc;
}

bool PmdPdAppend 
(
 st_sdc_p   tp,
 st_sdc_p   fp,
 us_int     ix,
 us_int     pl
)
{
  us_int   lg;
  bool     rc;

  lg = pl;
  rc = tp->ml >= (tp->lg + lg);

  if (!rc) lg = tp->ml - tp->lg;

  memcpy (tp->pt + tp->lg, fp->pt + ix, lg);
  tp->lg += lg; 

  return rc;
}

bool PmdPsAppend 
(
 st_sdc_p   tp,
 char_p     cp,
 us_int     ix,
 us_int     pl
)
{
  us_int   lg;
  bool     rc;

  lg = pl;
  rc = tp->ml >= (tp->lg + lg);

  if (!rc) lg = tp->ml - tp->lg;

  memcpy (tp->pt + tp->lg, cp + ix, lg);
  tp->lg += lg; 

  return rc;
}


bool PmdPdCopy
(
 st_sdc_p   tp,
 st_sdc_p   fp,
 us_int     ix,
 us_int     pl
)
{
  us_int   lg;
  bool     rc;

  lg = pl;
  rc = tp->ml >= lg;

  if (!rc) lg = tp->lg;

  memcpy (tp->pt, fp->pt + ix, lg);
  tp->lg = lg; 

  return rc;
}

bool PmdPsCopy
(
 st_sdc_p   tp,
 char_p     cp,
 us_int     ix,
 us_int     pl
)
{
  us_int   lg;
  bool     rc;

  lg = pl;
  rc = tp->ml >= lg;

  if (!rc) lg = tp->lg;

  memcpy (tp->pt, cp + ix, lg);
  tp->lg = lg; 

  return rc;
}


bool PmdSLexeq
(
 st_sdc_p   tp,
 char_p     cp
)
{
  us_int   lg;
 
  lg = strlen (cp);

  if (tp->lg != lg) return FALSE;

  return (0 == memcmp (tp->pt, cp, lg));

}

bool PmdDLexeq
(
 st_sdc_p   tp,
 st_sdc_p   fp
)
{

  if (tp->lg != fp->lg) return FALSE;

  return (0 == memcmp (tp->pt, fp->pt, tp->lg));

}

bool PmdPsLexeq
(
 st_sdc_p   tp,
 char_p     cp,
 us_int     ix,
 us_int     pl
)
{
 
  if (tp->lg != pl) return FALSE;

  return (0 == memcmp (tp->pt, cp + ix, pl));

}

bool PmdPdLexeq
(
 st_sdc_p   tp,
 st_sdc_p   fp,
 us_int     ix,
 us_int     pl
)
{
 
  if (tp->lg != pl) return FALSE;

  return (0 == memcmp (tp->pt, fp->pt + ix, pl));

}

