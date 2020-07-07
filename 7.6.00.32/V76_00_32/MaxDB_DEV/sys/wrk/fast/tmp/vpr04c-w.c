/*!**************************************************************************

  module:    vpr04c.c

  -------------------------------------------------------------------------

  responsible: BurkhardD

  special area:   CPCDrv | Client | Parameter Conversions
  description : Parameter Conversions

  last changed: 2000-09-20  9:05
  see also    :

  -------------------------------------------------------------------------





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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "gpr00.h"
#include "gpr04.h"
#include "livecachetypes.h" /* no check */
#include "vpr01SQL.h"
#include "gmt90.h"
#include "hsp45.h"
#include "vsp46c.h"
#include "gpr03.h"
#include "gpr08.h"
#include "vpr03sc.h"
#include "hsp78_0.h"
#include "vpr03Packet.h"
#include "vpr04Long.h"

/*==================================================================*
 *  DEFINES                                                    *
 *==================================================================*/
#define INT4_DIG   12 /* max. no. of characters for tsp00_Int4 + 1 */
#define INT8_DIG   26 /* max. no. of characters for tsp00_Longint + 1(8byte) */
#define MAX_FLOAT 130 /* max. posible string length for strings which containing floats */

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/* internal dbmode and ansi flag stored by p04mode */
static tsp00_Int2 dbmode, sqlansi;
tpr05_StringEncoding pr04cPacketEncoding;
static tsp00_SwapKind pr04cPacketSwapKind;

const static tsp00_Int2 uninull [1] = {0x0000};

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

void s40check (const char *, int, int, char *);
void s40gsint (const char *, int, int, tsp00_Int2 *, char *);
void s40glint (const char *, int, int, tsp00_Int4 *, char *);
void s40g4int (tsp00_Uint1 *, int, tsp00_Int4 *, char *);
void s40g8int (const char *, int, int, tsp00_Longint *, char *);
void s40gsuns (const char *, int, int, tsp00_Uint2 *, char *);
void s40gluns (const char *, int, int, tsp00_Uint4 *, char *);
void s40gsrel (const char *, int, int, float *, char *);
void s40glrel (const char *, int, int, double *, char *);
void s40gdec (const char *, int, int, char *, int, int, char *);
void s42gstr (const char *, int, int, int, int, char *, int,
    int *, char *);
void s40gzone (const char *, int, int, int, char *, int, int,
    char *);
void s40glzon (const char *, int, int, int, char *, int, int,
    char *);
void s40glszo (const char *, int, int, int, char *, int, int,
    char *);
void s40gtszo (const char *, int, int, int, char *, int, int,
    char *);
void s40gbyte (const char *, int, int, char *, int, int, char *);

void s41p4int (tsp00_Uint1 *, int, int, char *);
void s41psint (char *, int, int, int, int, char *);
void s41plint (char *, int, int, int, tsp00_Int4, char *);
void s41p8int (char *, int, int, int, tsp00_Longint, char *);
void s41psuns (char *, int, int, int, unsigned int, char *);
void s41pluns (char *, int, int, int, tsp00_Uint4, char *);
void s41psrel (char *, int, int, int, double, char *);
void s41plrel (char *, int, int, int, double, char *);
void s41pdec (char *, int, int, int, const char *, int, int, char *);
void s41pbyte (char *, int, int *, const char *, int, int, char *);
void s43pstr (char *, int, int, int, const char *, int, int, char *);
void s41pzone (char *, int, int, int, const char *, int, int,
    char *);
void s47ognum (const char *, tsp00_Int4, int, char *, tsp00_Int4,int, int, char *);
void s47opnum (const char *, tsp00_Int4, tsp00_Int2, char *, tsp00_Int4, tsp00_Int2 *, int, char *);
void s26find_part (tsp1_segment_ptr, int, tsp1_part_ptr*);
void s26new_part_init (tsp1_packet_ptr, tsp1_segment_ptr,
	tsp1_part_ptr*);
void s26finish_part (tsp1_packet_ptr, tsp1_part_ptr);

void s80uni_trans (void*, tsp00_Int2, const char*, tsp00_Int4,
	tsp00_Int2, char*, tsp00_Int4*, tsp00_Int2, const char*, char*, tsp00_Int4*);

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

tpr05_StringEncoding pr04cGetBufEncoding(const struct SQLCOL *this);
static char pr04cGetDefineByte(const struct SQLCOL *this);
static boolean pr04IsUnicode(const struct SQLCOL *this, char *buf);
static tsp00_Int4 pr05cGetPaddedLength(tsp00_Int4  msgEncoding,	const tsp00_Byte *szPaddedStr, tsp00_Uint4 cbSize, const char PadChar);
static int p04unito (const struct SQLCOL *this, char *buf,
	const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	tsp00_Int2 vd, tsp00_Int2 vf);
static int p04unicto (const struct SQLCOL *this, char *buf,
                      const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	              tsp00_Int2 vd, tsp00_Int2 vf);
static int p04gunifrom (const struct SQLCOL *this, const char *buf,
	                void *va, tsp00_Int4 *cl, tsp00_Int4 *vl,
	                tsp00_Int2 vd, tsp00_Int2 vf, boolean terminate);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

void p04mode (sqlcatype *sqlca, sqlgaentry *gae)
{
  p04ocimode (sqlca->sqlrap, sqlca->sqlgap, gae);
}

void p04ocimode (sqlratype *sqlrap, sqlgatype *sqlgap, sqlgaentry *gae)
{
  dbmode  = sqlrap->rakamode;
  sqlansi = sqlrap->rasqlansi;
  pr04cPacketEncoding = pr03PacketGetEncoding((tsp1_packet *)gae->gareqptr);
  pr04cPacketSwapKind = pr03PacketGetSwapKind((tsp1_packet *)gae->gareqptr);
}

void
p04trvinit (const char *vname, int ix, char *line, tsp00_Int2 * ll,
    int inp)
{
  static const char *const s[] = {"INPUT  : ", "OUTPUT : ",
				  "ROWNO****", "LONGVAR: ", "ARR-CNT**"};

  *ll = (tsp00_Int2) strlen (s[inp]);
  memcpy (line, s[inp], *ll);
  *ll += sprintf (&line[*ll], "%4.0d", ix);
  if (inp != CPR_TR_LOOP && inp != CPR_TR_ARRCNT) {
    line[(*ll)++] = ':';
    line[(*ll)++] = ' ';
    memcpy (&line[*ll], vname, sizeof (sqllname));
    *ll += sizeof (sqllname);
    line[(*ll)++] = ':';
    line[(*ll)++] = ' ';
  }
}     /*--------------------------------------------*/

void
p04trloop (sqlratype * sqlrap, int lpind)
{
  sqltatype      *tap = sqlrap->rasqltap;
  tsp00_Int2       *ll = &tap->tastr80l;

  if (tap->tatrout != CPR_TRACE_LONG && tap->tatrout != CPR_TRACE_MODLONG)
    return;

  p04trvinit ("", lpind + 1, tap->tastr80, ll, CPR_TR_LOOP);
  p08vfwritetrace (sqlrap);
}     /*--------------------------------------------*/


#define PR04TLEN(l) (l*pr04cBufEncoding->fixedCharacterSize)

static void
p04trvwrite (sqlratype * sqlrap,
	     const struct SQLCOL *col, char *buf)
{
  tsp1_segment *seg = (tsp1_segment *) sqlrap->rasegptr;
  tsp1_part *datapart;
  sqltatype      *tap = sqlrap->rasqltap;
  tsp00_Int2       *ll = &tap->tastr80l;
  char            res;

  tpr05_StringEncoding pr04cBufEncoding = pr04cGetBufEncoding(col);

  buf++; /* ignore defined byte */
  if (col->colislong == CPR_OLDLONG || *buf == CPR_LONG_DESC_DEF_BYTE) {
    static const char s[] = "LONGVARCHAR-DESC";
    static const int ls = sizeof (s) - 1;

    memcpy (&tap->tastr80[*ll], s, ls);
    *ll += ls;
    return;
  }
  if (col->colisnum) {
    int             ls;
    ls = sizeof(ls);
    if (col->coltype == dboolean) {
      char *s;
      s = (*buf) ? "TRUE" : "FALSE";
      ls = strlen (s);
      memcpy (&tap->tastr80[*ll], s, ls);
    }
    else {
      int    trcollen;
      trcollen = 18;
      if (col->colfrac < 0)
	trcollen = (int) col->collen;
      M90TRACEBUF ("tr_buf ", buf, 1, 20);
      s42gstr (buf, 1, trcollen, (int) col->colfrac,
	       (int) col->collen, tap->tastr80, *ll + 1, &ls, &res);
      M90TRACEBUF ("tr80_buf ",tap->tastr80, *ll, 20);
      M90TRACE (M90_TR_SCHAR, "res    ", &res);
    }
    *ll += ls;
  }
  else {
    BOOLEAN isbyte = p04IsBinaryType(col->coltype);
    tsp00_Uint4   cbWrite;        /* number of bytes written */
    tsp00_Uint4   cbParsed;       /* number of bytes parsed */

    int lt = (isbyte) ? 2 * col->coliolen - 2 : col->coliolen - 1;
    int             ls;
    if (col->coltype == dvarcharuni && col->coliolen == 0) {
      p03find_part (sqlrap, sp1pk_data, &datapart);
      lt=(datapart->sp1p_part_header.sp1p_buf_len-col->colpos-1 >= 80) ? 80 :
	datapart->sp1p_part_header.sp1p_buf_len-col->colpos-1 ;
      M90TRACE (M90_TR_WORD, "buf_len   ",
		&datapart->sp1p_part_header.sp1p_buf_len);
      M90TRACE (M90_TR_SWORD, "colpos   ", &col->colpos);
      M90TRACE (M90_TR_SWORD, "lt   ", &lt);
    }
    if (lt > PR04TLEN(254))
      lt = PR04TLEN(254);
    if (!isbyte) {
      tsp00_Int4 cbPaddedLen; /* length of padded spaces */
      cbPaddedLen = pr05cGetPaddedLength(pr04cBufEncoding->EncodingType,
					 (tsp00_Byte*)buf, lt, ' ');
      lt -= cbPaddedLen;
    }
    ls = (lt + *ll > PR04TLEN(80)) ? (*ll < PR04TLEN(50)) ? PR04TLEN(30) : PR04TLEN(80) - *ll : lt;
    if (isbyte) {
      cbParsed = ls / 2;
      cbWrite = cbParsed*2;
      if (*ll+1+cbWrite > sizeof(tap->tastr80)) {
        pr01TracePrintf(sqlrap, "sqlabort *ll=%d, cbWrite=%d", *ll, cbWrite);
        sqlabort();
      }
      s40gbyte (buf, 1, cbParsed, tap->tastr80, *ll + 1, cbWrite, &res);
      buf += cbParsed;
      *ll += cbWrite;
      lt -= cbWrite;
    }
    else {
      tsp78ConversionResult rc;
      rc = sp78convertString(sp77encodingUTF8, &tap->tastr80[*ll], sizeof(tap->tastr80),
			     &cbWrite, FALSE,
			     pr04cBufEncoding, buf, ls,
			     &cbParsed);

      if (rc != sp78_Ok) {
	cbParsed = cbWrite = ls;
	pr01TracePrintf(sqlrap, "Conversion to UTF8 failed (%s).", sp78errString(rc));
          cbWrite = sp77sprintfUnicode(sp77encodingUTF8, tap->tastr80, sizeof(tap->tastr80)-1, "BUFFER : 0x%.*r", ls, buf);
      }
      buf += cbParsed;
      *ll += cbWrite;
      lt -= cbParsed;
    }
    while (lt > 0) {
      static const char n[] = " ...   : ";
      static const tsp00_Int2 ln = sizeof (n) - 1;

      p08vfwritetrace (sqlrap);
      memcpy (tap->tastr80, n, ln);
      *ll = ln;
      ls = (lt > PR04TLEN(70)) ? PR04TLEN(70) : lt;
      if (isbyte) {
	cbParsed = (ls > 1) ? ls / 2 : ls;
	cbWrite = cbParsed*2;
        if (*ll+1+cbWrite > sizeof(tap->tastr80)) {
	  pr01TracePrintf(sqlrap, "sqlabort *ll=%d, cbWrite=%d", *ll, cbWrite);
          sqlabort();
        }
	s40gbyte (buf, 1, cbParsed, tap->tastr80, *ll + 1, cbWrite, &res);
	buf += cbParsed;
	lt -= cbWrite;
	*ll += cbWrite;
      }
      else {
	tsp78ConversionResult rc;
	rc = sp78convertString(sp77encodingUTF8, &tap->tastr80[*ll], sizeof(tap->tastr80),
			       &cbWrite, FALSE,
			       pr04cBufEncoding, buf, ls,
			       &cbParsed);

	if (rc != sp78_Ok) {
          cbParsed = ls;
	  cbWrite = cbParsed*2;
	  pr01TracePrintf(sqlrap, "Conversion to UTF8 failed (%s).", sp78errString (rc));
          cbWrite = sp77sprintfUnicode(sp77encodingUTF8, tap->tastr80, sizeof(tap->tastr80)-1, "BUFFER : 0x%.*r", ls, buf);
	}
        buf += cbParsed;
	lt -= cbParsed;
	*ll += cbWrite;
      }
    }
  }
}     /*--------------------------------------------*/

void
p04ptrvwrite (sqlratype * sqlrap,
	      const struct SQLCOL *col, char *buf, tsp00_Int4 pos)
{
  p04trvwrite (sqlrap, col, buf + pos - 1);
}     /*--------------------------------------------*/

tsp00_Int2 *p04swap2 (tsp00_Int2 *dbuf, tsp00_Int2 *sbuf, tsp00_Int4 len)
{
  tsp00_Int2 *res = dbuf;
  tsp00_Int4 i;
  unsigned char c;
  union sw2
  {
    tsp00_Int2 i;
    unsigned char c [2];
  } tst;

  tst.i = 1;
  if (tst.c [1])
    return sbuf;

  if (dbuf == 0) {
    res = pr03mAllocat(len * sizeof (tsp00_Int2), "dbuf");
    if (!res)
      sqlabort();
    dbuf = res;
  }

  for (i = 0; i < len; i++) {
    c = ((union sw2*) sbuf)->c [0];
    ((union sw2*) dbuf)->c [0] = ((union sw2*) sbuf++)->c [1];
    ((union sw2*) dbuf++)->c [1] = c;
  }
  return res;
}

static int
p04isnum(const tsp1_param_info *sfi)
{
  return (sfi->sp1i_data_type == dfixed
	  || sfi->sp1i_data_type == dfloat
	  || sfi->sp1i_data_type == dvfloat
	  || sfi->sp1i_data_type == dsmallint
	  || sfi->sp1i_data_type == dinteger
	  || sfi->sp1i_data_type == dboolean);
}

  boolean
  p04isunidata (const tsp00_DataType datatype)
   {
    return ((datatype == dunicode) ||
  	  (datatype == dstruni) ||
  	  (datatype == dlonguni) ||
  	  (datatype == dvarcharuni));
  }

boolean
p04isasciidata (const tsp00_DataType datatype)
{
  return ((datatype == dcha) ||
	  (datatype == dstra) ||
	  (datatype == dlonga) ||
	  (datatype == dvarchara));
}

   static int
   p04isuni(const tsp1_param_info *sfi)
   {
    return p04isunidata (sfi->sp1i_data_type);
   }

static int
p04islong (const tsp1_param_info *sfi)
{
  return ((sfi->sp1i_data_type >= dstra &&
	   sfi->sp1i_data_type <= dstrb)
	  || sfi->sp1i_data_type == dstruni) ? CPR_OLDLONG :
    ((sfi->sp1i_data_type >= dlonga &&
      sfi->sp1i_data_type <= dlongdb) ||
     sfi->sp1i_data_type == dlonguni) ?
    CPR_NEWLONG : CPR_NOLONG;
}

static          tsp00_Int2
p04length (const tsp1_param_info *sfi,
	   tsp00_Int4 * coll, char * colf)
{
  /* PTS 1108496 */
  *coll = sfi->sp1i_length;
  *colf = (sfi->sp1i_data_type == dfloat ||
	   sfi->sp1i_data_type == dvfloat) ?
    (char) -1 : sfi->sp1i_frac;
  return sfi->sp1i_in_out_len;
}     /*--------------------------------------------*/

static int
p04incompto (const struct SQLCOL *this,
	     char *buf, const void *va, tsp00_Int4 cl, tsp00_Int4 vs,
	     tsp00_Int2 vl, tsp00_Int2 vf)
     /* not implemented datatypes */
{
  this, buf, va, cl, vs, vl, vf;
  return CPR_NUM_INCOMPATIBLE;
}     /*--------------------------------------------*/

static int
p04defaultto (const struct SQLCOL *this,
	      char *buf, const void *va, tsp00_Int4 cl, tsp00_Int4 vs,
	      tsp00_Int2 vl, tsp00_Int2 vf)
     /* not implemented datatypes */
{
  this, buf, va, cl, vs, vl, vf;
  return CPR_NUM_INVALID;
}     /*--------------------------------------------*/

static int
p04incompfrom (const struct SQLCOL *this,
	       char *buf, const void *va, tsp00_Int4 cl, tsp00_Int4 vs,
	       tsp00_Int2 vl, tsp00_Int2 vf)
     /* not implemented datatypes */
{
  this, buf, va, cl, vs, vl, vf;
  return CPR_NUM_INCOMPATIBLE;
}     /*--------------------------------------------*/

static int
p04defaultfrom (const struct SQLCOL *this,
		const char *buf, void *va, tsp00_Int4 cl, tsp00_Int4 vs,
		tsp00_Int2 vl, tsp00_Int2 vf)
     /* not implemented datatypes */
{
  this, buf, va, cl, vs, vl, vf;
  return CPR_NUM_INVALID;
}     /*--------------------------------------------*/

static int
p04dftoindi (tsp00_Int4 ival, void *indi,
	     tsp00_Int2 indl, tsp00_Int2 indf)
     /* not implemented indicator datatypes */
{
  ival, indi, indl, indf;
  return CPR_NUM_INVALID;
}     /*--------------------------------------------*/

static int
p04dffromindi (tsp00_Int4 * ival, const void *indi,
	       tsp00_Int2 indl, tsp00_Int2 indf)
     /* not implemented indicator datatypes */
{
  ival, indi, indl, indf;
  return CPR_NUM_INVALID;
}     /*--------------------------------------------*/

void
p04init (struct SQLROW *row)
{
  row->ireclen = row->oreclen = 0;
}     /*--------------------------------------------*/

tsp00_Int2
p04pdecode (const char *buf, tsp00_Int2 pos,
	    struct SQLROW *row, struct SQLCOL *c)
{
  return (p04decode ((tsp1_param_info *) (buf + pos - 1), row, c));
}     /*--------------------------------------------*/


tsp00_Int2
p04decode (const tsp1_param_info *sfi,
	   struct SQLROW *row, struct SQLCOL *c)
{
  tsp00_Int4        l;
  tsp00_Int2        len;

  c->coltype = (sfi->sp1i_data_type == dvfloat) ?
    (char) dfloat : sfi->sp1i_data_type;
  c->colmode = sfi->sp1i_mode[0];
  c->colio = sfi->sp1i_io_type;
  c->colpos = sfi->variant.C_true.sp1i_bufpos_F - (tsp00_Int2) 1;
  len = p04length (sfi, &c->collen, &c->colfrac);
  c->coliolen = len;
  c->colisnum = (char) p04isnum(sfi);
  c->colisuni = (char) p04isuni(sfi);
  c->colislong = (char) p04islong(sfi);
  c->colvartype = c->colindtype = CPR_VNONE;
  c->coltobuf = p04defaultto;
  c->colfrombuf = p04defaultfrom;
  c->coltoindi = p04dftoindi;
  c->colfromindi = p04dffromindi;
  l = c->colpos + len;
  if (c->colio == CPR_INFO_INPUT || c->colio == CPR_INFO_INOUT)
    if (l > row->ireclen)
      row->ireclen = l;
  if (c->colio == CPR_INFO_OUTPUT || c->colio == CPR_INFO_INOUT)
    if (l > row->oreclen)
      row->oreclen = l;
  return len;
}     /*--------------------------------------------*/

/*!
  Function:     pr04ColNames2ore

  see also:

  Description:  gets column names from columnnames part in order packet

  The column names will be stored in the orresnamptr members of the given sqlorentry.
  If the the space for these members is insufficient the memory will be reallocated.

  Arguments:
    sqlca      [in] pointer to sqlca
    sqlxa      [in] pointer to sqlxa
    sqlkaentry [in] pointer to a sqlkaentry
    sqlorentry [in] pointer to a sqlorentry

  return value: void
 */
void pr04ColNames2ore(sqlcatype *sqlca, sqlxatype *sqlxa,sqlkaentry *kae, sqlorentry *ore) {
  tsp1_part *part_ptr;
  tsp1_param_info param_info;
  sqlratype *sqlra = (sqlratype *) sqlca->sqlrap;

  ore->orresnamacc = 0;
  p03find_part(sqlca->sqlrap , sp1pk_columnnames , &part_ptr);
  if (part_ptr) {
    tsp1_part_header *partheader = &part_ptr->sp1p_part_header;

    if (partheader->sp1p_buf_len > 0) {
      ore->orresnamacc = partheader->sp1p_buf_len;
      if (   ore->orresnamptr
          && ore->orresnamlen < ore->orresnamacc) {
        /* insufficient space in ore area - free it*/
        pr03mFree(ore->orresnamptr, "ore->orresnamptr");
        ore->orresnamptr = NULL;
        ore->orrescnt = 0;
        ore->orreslen = 0;
        ore->orressqlrow.ireclen = 0;
        ore->orressqlrow.oreclen = 0;
        p03orfree(sqlca , ore->orresnamptr , 3);
      }
      if (! ore->orresnamptr) {
        ore->orresnamlen = ore->orresnamacc;

        ore->orresnamptr = pr03mAllocat(ore->orresnamlen, "ore->orresnamptr");
        if (!ore->orresnamptr) {
           p08runtimeerror (sqlca, sqlxa, cpr_memory_allocation_faild);
           ore->orcolptr = NULL;
           sqlabort ();
        }
      }
      /*set encoding*/
      ore->orresnamencoding = (void*)pr03cGetPacketEncoding();
      /*copy column names*/
      memcpy(ore->orresnamptr, part_ptr->sp1p_buf, ore->orresnamacc);
    }
  }
}

/*!
  Function:     p04GetSFInfo

  see also:

  Description:  gets short field info from shortinfo part in order packet

  The short field infos will be stored in the oreCol members of the given sqlorentry.
  If the the space for these members is insufficient the memory will be reallocated.

  Arguments:
    sqlca      [in] pointer to sqlca
    sqlxa      [in] pointer to sqlxa
    sqlkaentry [in] pointer to a sqlkaentry
    sqlorentry [in] pointer to a sqlorentry

  return value: void
 */
void p04GetSFInfo(sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *kae, sqlorentry *ore) {
  tsp00_Int4 i;
  tsp1_part *part_ptr;
  tsp1_part_header *partheader;
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  sqlratype *sqlra = (sqlratype *) sqlca->sqlrap;

  p03find_part(sqlra, sp1pk_shortinfo, &part_ptr);
  if (part_ptr) {
    ore->orcolcntacc = 0;
    partheader = &part_ptr->sp1p_part_header;
    if (partheader->sp1p_buf_len > 0) {
      ore->orcolcntacc = partheader->sp1p_arg_count;

      if (   ore->orcolptr
          && ore->orcolcnt < partheader->sp1p_arg_count) {
          /* insufficient space in ore area - free it*/
          pr03mFree(ore->orcolptr, "ore->orcolptr");
          ore->orcolptr = NULL;
          ore->orcolcnt = 0;
          ore->orcollen = 0;
          ore->orsqlrow.ireclen = 0;
          ore->orsqlrow.oreclen = 0;
          p03orfree(sqlca , ore->orcolptr , 1);
      }
      if ( !ore->orcolptr ) {
        ore->orcolcnt = partheader->sp1p_arg_count;
        ore->orcollen = sizeof(struct SQLCOL) * ore->orcolcnt;
        ore->orcolptr = pr03mAllocat(ore->orcollen, "ore->orcolptr");
        if (!ore->orcolptr) {
           p08runtimeerror (sqlca, sqlxa, cpr_memory_allocation_faild);
           ore->orcolptr = NULL;
           sqlabort ();
        }
      }
      p04init (&ore->orsqlrow);
      for (i=0; i< partheader->sp1p_arg_count; i++) {
        tsp1_param_info *sfi = (tsp1_param_info *) &(part_ptr->sp1p_buf[(i*sizeof(tsp1_param_info))]);
        /*get parameter data from sfi part in order packet*/
        p04decode ( sfi, &ore->orsqlrow, &ore->orcolptr[i]);
      }
    }/*end if partheader > 0*/
  }
}

/*!
  Function:     p04SFInfo2oreRes

  see also:

  Description:  gets short field info from shortinfo part in order packet

  The short field infos will be stored in the orRes-members of the given sqlorentry.
  If the the space is insufficient the memory will be reallocated.

  Arguments:
    sqlca      [in] pointer to sqlca
    sqlxa      [in] pointer to sqlxa
    sqlkaentry [in] pointer to a sqlkaentry
    sqlorentry [in] pointer to a sqlorentry

  return value: void
 */
void p04SFInfo2oreRes(sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *kae, sqlorentry *ore) {
  tsp00_Int4 i;
  tsp1_part *part_ptr = NULL;
  tsp1_part_header *partheader;
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  sqlratype *sqlra = (sqlratype *) sqlca->sqlrap;

  p03find_part(sqlra, sp1pk_shortinfo, &part_ptr);
  if (! part_ptr)
    p03find_part(sqlra, sp1pk_output_cols_no_parameter, &part_ptr);
  if (part_ptr) {
    ore->orrescntacc = 0;
    partheader = &part_ptr->sp1p_part_header;
    if (partheader->sp1p_buf_len > 0) {
      ore->orrescntacc = partheader->sp1p_arg_count;

      if (   ore->orresptr
          && ore->orrescnt < partheader->sp1p_arg_count) {
          /* insufficient space in ore area - free it*/
            pr03mFree(ore->orresptr, "ore->orresptr");
            ore->orresptr = NULL;
            ore->orrescnt = 0;
            ore->orreslen = 0;
            ore->orressqlrow.ireclen = 0;
            ore->orressqlrow.oreclen = 0;
            p03orfree(sqlca , ore->orresptr , 1);
      }
      if ( !ore->orresptr ) {
          ore->orrescnt = ore->orrescntacc;
          ore->orreslen = sizeof(struct SQLCOL) * ore->orrescnt;
          ore->orresptr = pr03mAllocat(ore->orreslen, "ore->orresptr");
          if (!ore->orresptr) {
             p08runtimeerror (sqlca, sqlxa, cpr_memory_allocation_faild);
             ore->orresptr = NULL;
             sqlabort ();
          }
      }
      p04init (&ore->orressqlrow);
      for (i=0; i< partheader->sp1p_arg_count; i++) {
        tsp1_param_info *sfi = (tsp1_param_info *) &(part_ptr->sp1p_buf[(i*sizeof(tsp1_param_info))]);
        /*modify datatype for unicode columns*/
        if (seg->sp1s_segm_header.sp1r_function_code == csp1_string_command_fc
            &&((sfi->sp1i_data_type >= dstra && sfi->sp1i_data_type <= dstrdb)
               || sfi->sp1i_data_type == dstruni)) {
	       sfi->sp1i_data_type = dvarcharuni ;
	    }
        /*get parameter data from sfi part in order packet*/
        p04decode ( sfi, &ore->orressqlrow, &ore->orresptr[i]);
      }
    }/*end if partheader > 0*/
  }
}


static int
p04isbyte (const struct SQLCOL *this)
{
  return (this->coltype == dchb
	  || this->coltype == dvarcharb
          || this->coltype == dlongb
          || this->coltype == dlongdb
	  || this->coltype == dstrb
	  || this->coltype == dboolean);
}     /*--------------------------------------------*/

static int
p04compchar (const struct SQLCOL *this, const void *va)
{
  return (!va) ? CPR_NUM_MISSING_ADDR : ((sqlansi == CPR_KIND_ANSI &&
					  dbmode == CPR_KIND_INTERNAL) || dbmode == CPR_KIND_DB2) ?
    (this->colisnum && this->coltype != dboolean) ?
    CPR_NUM_INCOMPATIBLE : CPR_NUM_OK : CPR_NUM_OK;
}     /*--------------------------------------------*/

static int
p04defchar (const struct SQLCOL *this, char b)
{
  this, b;
  return (CPR_NUM_OK);
}     /*--------------------------------------------*/

static int
p04compnum (const struct SQLCOL *this, const void *va)
{
    if (!va)
      return CPR_NUM_MISSING_ADDR;
    else {
      if (  (sqlansi == CPR_KIND_ANSI && dbmode == CPR_KIND_INTERNAL)
          || dbmode == CPR_KIND_DB2)  {
        if (!this->colisnum)
          return CPR_NUM_INCOMPATIBLE;
        else
          return CPR_NUM_OK;
      }
      else {
        if (  !this->colisnum
            && this->coltype != dcha
            && dche
            && this->coltype != dvarchara
            && this->coltype != dvarchare
            && this->coltype != dvarcharuni
            && this->coltype != dunicode
            )
          return CPR_NUM_INCOMPATIBLE;
        else
          return CPR_NUM_OK;
      }
    }

/*  old unreadable version

    return (!va) ? CPR_NUM_MISSING_ADDR : ((sqlansi == CPR_KIND_ANSI &&
					  dbmode == CPR_KIND_INTERNAL) || dbmode == CPR_KIND_DB2) ?
    (!this->colisnum) ? CPR_NUM_INCOMPATIBLE : CPR_NUM_OK :
    (!this->colisnum && this->coltype != dcha &&
     dche && this->coltype != dvarchara &&
     this->coltype != dvarchare) ?
    CPR_NUM_INCOMPATIBLE : CPR_NUM_OK; */
}     /*--------------------------------------------*/

static int
p04defnum (char b)
{
    if ((sqlansi == CPR_KIND_ANSI && dbmode == CPR_KIND_INTERNAL)
	|| dbmode == CPR_KIND_DB2)  {
      if (b != CPR_DEFINED_BYTE)
 	return CPR_NUM_INCOMPATIBLE;
      else
 	return CPR_NUM_OK;
    }
    else {
      if (   b != CPR_DEFINED_BYTE
          && b != ' '
          && b != csp_unicode_def_byte)

        return CPR_NUM_INCOMPATIBLE;
      else
        return CPR_NUM_OK;
    }
/*  old unreadable version
  return ((sqlansi == CPR_KIND_ANSI && dbmode == CPR_KIND_INTERNAL)
	  || dbmode == CPR_KIND_DB2) ?
    (b != CPR_DEFINED_BYTE) ? CPR_NUM_INCOMPATIBLE : CPR_NUM_OK : (b != CPR_DEFINED_BYTE && b != ' ') ?
    CPR_NUM_INCOMPATIBLE : CPR_NUM_OK; */
}     /*--------------------------------------------*/

#define P04MAXDEC 44

static int
p04declen (tsp00_Int4 l, tsp00_Int2 f)
{
  tsp00_Int4        res = (f < 0) ? l + 6 : (f == 0) ? l + 1 :
    ((tsp00_Int4) f == l) ? l + 3 : l + 2;

  return (int) (res);
}     /*--------------------------------------------*/

/* host variable conversions */
/*!
  Function:     pr04AsciitoUCS2Buf

  see also:

  Description:  Converts ASCII string to UCS2 encoded packet buffer

  The given ASCII string will be converted to UCS2 native and filled up
  with blanks. The define byte will set to unicode.

  NOTE that function assumes UCS2 Native as encoding type of buffer

  Arguments:
    buffer     [out] pointer to destination UCS2 buffer
    bufferLen  [in]  length of buffer
    szAscii    [in]  source string encoded in Ascii
    szAsciiLen [in]  length of szAscii

  return value: error code
 */
static int pr04AsciitoUCS2Buf (char *buffer, tsp00_Int4 bufferLen, const char *szAscii,  tsp00_Int4 szAsciiLen, char defByte)
{
    char res = (char) CPR_NUM_OK;
    tsp00_Uint4   cbWrite;        /* number of bytes written */
    tsp00_Uint4   cbParsed;       /* number of bytes parsed */
    tsp00_Uint4   cbCol = bufferLen*2;      /* number of bytes in column */
    tsp00_Uint4 cbColwithoutBlanks = cbCol; /*column lengt without trailing blanks*/
    tsp78ConversionResult rc;

    *buffer++ = defByte; /*pr04cGetDefineByte(this); set define byte to unicode*/

    /*remove trailing blanks*/
    cbColwithoutBlanks -= (sp77encodingAscii->countPadChars((void*) szAscii, (tsp00_Uint4) szAsciiLen, ' '));

    /*convert to unicode*/
    rc = sp78convertString(sp77encodingUCS2Native, buffer, cbCol,&cbWrite, FALSE,
                           sp77encodingAscii, szAscii, szAsciiLen, &cbParsed);
    if (cbCol > cbWrite) {
      /* fill up with blanks */
      void *FillBuf = buffer+cbWrite;
      tsp00_Uint4  targetLength = (cbCol - cbWrite);
      tsp00_Uint4  padLength = targetLength;
      sp77encodingUCS2Native->fillString(&FillBuf, &targetLength, padLength, ' ');
    }

    if (rc == sp78_TargetExhausted)
      res = CPR_NUM_TRUNCCHAR;
    else if (rc != sp78_Ok)
      res = CPR_NUM_INVALID;
    return (res);
}

static int p04charto (const struct SQLCOL *this, char *buf, const char *va,
	tsp00_Int4 cl, tsp00_Int4 vl, tsp00_Int2 vd, tsp00_Int2 vf)
/* all char types */
{
  tsp00_Int4        dl = cl - vl;
  char            res = (char) p04compchar (this, va), fch;

  vd, vf;
  if (res)
    return (res);

  *buf++ = pr04cGetDefineByte(this);

  if (this->colisnum) {
    if (this->coltype == dboolean)
      *buf = (char) ((*va) ? 1 : 0);
    else {
#ifdef SQLODBC
      /* ODBC converts numbers into a string (with length matching the
         col-definition), but the conversion routine s43pstr does not
         process trailing spaces. (copied from p04charcto) */
      /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122478 */
      if (vl == 0)
        vl = (tsp00_Int2) strlen ((const char *) va);
      else {
        char *s = (char *) memchr (va, '\0', (size_t) vl);
        if (s)
          vl = (tsp00_Int2) (s - (char *) va);
      }
#endif
      s43pstr (buf, 1, (int) cl, this->colfrac, va, 1,
	       (int) vl, &res);
    }
  }
  else
    if ((pr03cGetPacketEncoding() != sp77encodingAscii || this->colisuni)
        && !p04isbyte(this)) {
      /*database or current column is unicode and type of column is not char byte*/
      tsp00_Uint4   cbWrite;        /* number of bytes written */
      tsp00_Uint4   cbParsed;       /* number of bytes parsed */
      tsp00_Uint4   cbCol = cl*2;   /* number of bytes in column */
      tsp78ConversionResult rc;
      tpr05_StringEncoding pr04cBufEncoding;
      pr04cBufEncoding = pr04cGetBufEncoding(this);
      rc = sp78convertString(pr04cBufEncoding, (char*)buf, cbCol,
			     &cbWrite, FALSE,
			     sp77encodingAscii, va, vl,
			     &cbParsed);
      if (cbCol > cbWrite) {
	/* fill up with blanks */
	void *FillBuf = buf+cbWrite;
	tsp00_Uint4  targetLength = (cbCol - cbWrite);
	tsp00_Uint4  padLength = targetLength / 2;
	pr04cBufEncoding->fillString(&FillBuf, &targetLength, padLength, ' ');
      }

      if (rc == sp78_TargetExhausted) {
	res = CPR_NUM_TRUNCCHAR;
      }
      else {
	if (rc != sp78_Ok)
	  res = CPR_NUM_INVALID;
      }
    }
    else {
      fch = (char) (p04isbyte(this) ? CPR_DEFINED_BYTE : ' ');
      if (dl < 0 && !this->colisuni) {
	const char     *p = &va[vl-1];

	while (vl > cl
	&& *p-- == fch)
	  vl--;
	dl = cl - vl;
      }
      if (dl < 0) {
	  vl = cl;
	  res = (char) ((this->colmode &
			 1 << CPR_INFO_ESCAPE_CHAR) ?
			CPR_NUM_INVESC : CPR_NUM_TRUNCCHAR);
      }
      else
	if (dl)
	  memset (&buf[vl], fch, (size_t) dl);
      memcpy (buf, va, (size_t) vl);
    }
  return (res);
}     /*--------------------------------------------*/

/*!
Function:     p04beautifyNumberString

see also:

Description: Takes the result of vdn-number to ascii conversation and give them
a special format without exponent. The number will be placed right
aligned with leading blanks and encoded in the given encoding type.

Arguments:
destination  [out]    char pointer to destination buffer
destlength   [in]     max. length of destination buffer
columnlength [in|out] size of database column
source       [in]     char pointer to source
res          [in|out] returncode
encType      [in]     encoding type of destination buffer

return value: void
*/
void p04beautifyNumberString (char *destination, tsp00_Int4 destinationlength, tsp00_Int4 *columnlength,
                 char *source, tsp00_Int4 sourcelength, char *res, tpr05_StringEncoding encType)
{
  tsp00_Uint4 FillLength = destinationlength;  /* fillString needs this parameter*/
  void        *FillBuf   = destination;
  tsp78ConversionResult rc;
  char       TempDest[sizeof (tsp00_C256)];
  char*      dest;
  tsp00_Int4 destlen = sizeof (TempDest);

  /*fillup output buffer with blanks*/
  encType->fillString(&FillBuf, &FillLength, (encType==sp77encodingAscii)?destinationlength:destinationlength/2, ' ');

  /*init temporary Ascii Buffer*/
  memset (TempDest,'0',sizeof (TempDest));
  dest = TempDest;

  while (sourcelength && *source == ' ') {
    /*eat leading blanks in source*/
    sourcelength--;
    source++;
  }
  if (sourcelength != 0) {
    tsp00_Int4 i = 0 ;
    char* matchChar;
    tsp00_Int4 PosOfPoint = 0; /* position of point - zero means not point found*/

    /*retrieve the position of the point, zero means not point found*/
    matchChar = (char*) memchr( source, '.', sourcelength );
    if( matchChar != NULL ) PosOfPoint = (tsp00_Int4)(matchChar - source + 1);

    /*retrieve the exponent*/
    matchChar = (char*)memchr( source, 'E', sourcelength );
    if( matchChar != NULL ){
      /*exponent found assuming a format like E[+|-]06*/
      tsp00_Int4 exp;
      sourcelength -= (source+sourcelength)-(matchChar); matchChar++;

      exp = ( matchChar[0] == '-')?-1:1;  matchChar++;
      if (matchChar[0] != '0'){
        exp *= (10*(matchChar[0]-48) + (matchChar[1]-48));
      } else {
        exp *= (matchChar[1]-48);
      }
      if (exp >= 0) {
        memcpy(dest,source,PosOfPoint-1); /*digits before the point*/
        destlen = PosOfPoint-1;
        if (sourcelength-PosOfPoint >= exp ) {
          memcpy(&dest[destlen],&source[destlen+1],exp); /*right shift of the point*/
          destlen += exp;
          if (sourcelength-PosOfPoint > exp){
            dest[destlen++]='.';
            memcpy(&dest[destlen],&source[destlen],sourcelength-destlen);  /*digits behind the point*/
            destlen+=sourcelength-destlen;
            while (  dest[destlen-1]=='0') { destlen--; } /*delete zeros behind point*/
            if (dest[destlen-1]=='.') destlen--;
          }
        }
        else {
          /*copy the rest and fillup with zeros*/
          memcpy(&dest[destlen],&source[destlen+1],sourcelength-destlen-1);
          destlen += exp;
        }
      }
      else {
      /*exponent < 0*/
        destlen=0;
        exp = -exp;
        if(source[0] == '-') {
          memcpy(&dest[destlen],"-0.",3); destlen+=3;
        } else {
          memcpy(&dest[destlen],"0.",2); destlen+=2;
        }
        dest[destlen+(exp-1)]=source[PosOfPoint-2]; destlen+=exp; /*digit before point*/
        memcpy(&dest[destlen],&source[PosOfPoint],sourcelength-PosOfPoint); /*digits behind point*/
        destlen += (sourcelength-PosOfPoint);
        while (  dest[destlen-1]=='0') { destlen--; } /*delete zeros behind point*/
        if (dest[destlen-1]=='.') destlen--;
     }
    }/*end if (exponent found)*/
    else {
      dest = source; /*leave numbers without exponent unchanged*/
      destlen = sourcelength;
      if (PosOfPoint>0){
        while (  dest[destlen-1]=='0') { destlen--; } /*delete zeros behind point*/
        if (dest[destlen-1]=='.') destlen--;
      }
    }
      /*for convert tempdest to destination*/
    {
      tsp78ConversionResult ConvRes = sp78_Ok;
      tsp00_Uint4   dlen, slen;
      tsp00_Int4 offset, convLen, logicalLen=(encType==sp77encodingAscii)?destinationlength:(destinationlength/2);

      *columnlength=destinationlength;
      if (logicalLen >= destlen){
        offset  = logicalLen-destlen;
        convLen = destlen;
      } else {
         tsp00_Int4 PosOfPoint = 0; /* position of point - zero means not point found*/
         char *matchChar = (char*) memchr( dest, '.', destlen );
         if( matchChar != NULL ) PosOfPoint = (tsp00_Int4)(matchChar - dest + 1);
         if (PosOfPoint && logicalLen >= PosOfPoint-1){
           *res = CPR_NUM_TRUNCCHAR;
           offset=0;
           convLen=logicalLen;
           while (  dest[convLen-1]=='0') { convLen--; offset++;}
           if ( dest[convLen-1]=='.') { convLen--; offset++;} /*delete zeros behind point*/
         } else{
           *res = CPR_NUM_INVALID;
           return;
         }
      }
      if ( dest[0]=='-' && dest[1]=='0' && convLen==2){dest[0]='0';convLen=1;offset++;} /*check for -0*/

      ConvRes = sp78convertString(encType,
                                  (void*) &destination[((encType==sp77encodingAscii)?offset:offset*2)],
                                  (tsp00_Uint4)destinationlength,
                                  &dlen,
                                  FALSE,
                                  sp77encodingAscii,
                                  (void*) dest,
                                  convLen,
                                  &slen);

      if (ConvRes == sp78_TargetExhausted) {
        *res = CPR_NUM_TRUNCCHAR;
      }
      else if (ConvRes != sp78_Ok) {
        *res = CPR_NUM_INVALID;
      }
    }
  }/*end source length is null*/
}
/*--------------------------------------------*/

static int p04gcharfrom (const struct SQLCOL *this, const char *buf,
			 char *va, tsp00_Int4 * cl, tsp00_Int4 * vl)
     /* all char types */
{
  tsp00_Int4        rl = -1;
  tsp00_Int4        dl = *vl - *cl, rcl = *cl;
  tsp00_Uint4     cbCol;       /* number of bytes in column */
  char            res = (char) p04compchar (this, va), fch;

  tpr05_StringEncoding pr04cBufEncoding = pr04cGetBufEncoding(this);

  if (res)
    return (res);

  fch = *buf;
  buf++; /*step over define byte*/

  /*determine column length in bytes*/
  if (pr04cBufEncoding==sp77encodingAscii){
    cbCol = *cl;
    rcl   = *cl;
  } else {
    cbCol = *cl*2;
    rcl   = *cl*2;
  }
  if (*vl==0){
  /*Note: if length of hostvariable is null (TCHAR*) then assuming
    hostvar have enougth space for current column length plus null termination*/
    *vl = *cl;
  }

  if (this->coltype == dboolean){
    *va = *buf;
  }
  else
    if (this->colisnum) {
      int rbl;
      char rb [P04MAXDEC];

      /*vdn-number to char*/
      s42gstr (buf, 1, (int) *cl, this->colfrac, (int) *cl, rb, 1,
	       &rbl, &res);
      /*rigth aligned with leading blanks*/
      p04beautifyNumberString (va, *vl, cl, rb, (tsp00_Int4)rbl, &res, sp77encodingAscii);
      rl = *cl;
    }
    else
      if (this->colisuni || pr04cBufEncoding != sp77encodingAscii ) {
        tsp00_Uint4   cbWrite;         /* number of bytes written */
        tsp00_Uint4   cbParsed;        /* number of bytes parsed */
        tsp78ConversionResult rc;

        /*determine real length of column without padding spaces*/
        cbCol -= (pr04cBufEncoding->countPadChars((void*) buf, cbCol, ' '))
          *((tsp00_Uint4) pr04cBufEncoding->fixedCharacterSize);

        /*convert from column to hostvariable*/
        rc = sp78convertString( sp77encodingAscii, va, *vl,
				&cbWrite, FALSE,
				pr04cBufEncoding, (char*)buf, cbCol,
				&cbParsed);

        rl = cbWrite;
        if (rc == sp78_TargetExhausted) {
	      *cl = cbWrite;
	      res = CPR_NUM_TRUNCCHAR;
	      rl = *vl;
	    }
        else if ( rc != sp78_Ok ) {
          res = CPR_NUM_INVALID;
	  rl = cbWrite;
	}
        else {
          if (cbWrite < (tsp00_Uint4)*vl){
            /*if terminate is false fillup Hostvariable whith blanks*/
            void *FillBuf = ((char*)va)+cbWrite;
            tsp00_Uint4  targetLength = ((tsp00_Uint4)*vl - cbWrite);
            tsp00_Uint4  padLength = targetLength;
            sp77encodingAscii->fillString(&FillBuf, &targetLength, padLength, 0x20);
          }
          *cl = cbCol;
        }
      }
      else {
	/* ADIS 1000915 */
	if (!p04isbyte(this) && !this->colisuni) {
	  const char     *p = &buf[rcl];

	  while (rcl && *--p == fch)
	    rcl--;
	  *cl = rcl;
	  dl = *vl - rcl;
	}
	if (*vl) {
	  if (dl < 0) {
	    *cl = *vl;
	    res = CPR_NUM_TRUNCCHAR;
	  }
	  else if (dl)
	    memset (&va[rcl], fch, (size_t) dl);
	}
	if (!p04isbyte(this) && !this->colisuni) {
	  rl = *cl;
	}
	memcpy (va, buf, (size_t) * cl);
      }
  *vl = rl;
  return (res);
}     /*--------------------------------------------*/

static int
p04charfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vchar, vansichar */
{
  vd, vf;
  return p04gcharfrom (this, buf, (char *) va, &cl, &vl);
}     /*--------------------------------------------*/

static int
p04charcto (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vcharc, vansicharc */
{
  char           *s;

  if (!p04isbyte (this)) {
    if (vl == 0)
      vl = (tsp00_Int2) strlen ((const char *) va);
    else {
      s = (char *) memchr (va, '\0', (size_t) vl);
      if (s)
	vl = (tsp00_Int2) (s - (char *) va);
      else
	return CPR_NUM_UNTERM;
    }
  }
  return p04charto (this, buf, (const char *) va, cl, vl, vd, vf);
}     /*--------------------------------------------*/

static int
p04charcfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vcharc */
{
  int             ret;

  vd, vf;
  if (vl == 0) {
    /*if (dbmode == CPR_KIND_ORACLE || dbmode == CPR_KIND_SAPR3)*/
      vl = (tsp00_Int2) strlen ((char *) va);
  }
  else {
    if (!p04isbyte (this))
      vl--;
  }
  ret = p04gcharfrom (this, buf, (char *) va, &cl, &vl);
  if (vl >= 0)
    ((char *) va)[vl] = '\0';
  return ret;
}     /*--------------------------------------------*/

static int
p04ansicharfrom (const struct SQLCOL *this,
		 const char *buf, void *va, tsp00_Int4 cl, tsp00_Int4 vl,
		 tsp00_Int2 vd, tsp00_Int2 vf)
     /* vansichar */
{
  int             ret;
  tsp00_Int4        ovl;
  tsp00_Int4        ocl;

  vd, vf;
  if (vl == 0)
    vl = cl;
  ovl = vl;
  ocl = cl;
  ret = p04gcharfrom (this, buf, (char *) va, &cl, &ovl);
  if (ret == CPR_NUM_OK) {
    if (ocl > vl) {
      ret = CPR_NUM_TRUNCCHAR;
      cl = ocl;
    }
  }
  return ret;
}     /*--------------------------------------------*/

static int
p04ansicharcfrom (const struct SQLCOL *this,
		  const char *buf, void *va, tsp00_Int4 cl, tsp00_Int4 vl,
		  tsp00_Int2 vd, tsp00_Int2 vf)
     /* vansicharc */
{
  int             ret;
  tsp00_Int4        ovl;
  tsp00_Int4        ocl;

  vd, vf;
  if (vl == 0)
    vl = cl;
  ovl = vl;
  ocl = cl;                   /*BF */
  ret = p04gcharfrom (this, buf, (char *) va, &cl, &ovl);
  if (ovl >= 0)
    ((char *) va)[vl - 1] = '\0';
  if (ret == CPR_NUM_OK) {
    if (ocl + 1 > vl) {
      ret = CPR_NUM_TRUNCCHAR;
      cl = ocl;
    }
  }
  return ret;
}     /*--------------------------------------------*/

static int
p04varunito (const struct SQLCOL *this, char *buf,
	      const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	      tsp00_Int2 vd, tsp00_Int2 vf)
     /* vstring */
{
  tsp00_Int2 lvl;
  memcpy (&lvl, va, sizeof (lvl));
  vl = (tsp00_Int4)lvl;
  return p04unito (this, buf, &((const char *) va)[2], cl, vl, vd, vf);
}     /*--------------------------------------------*/

/* null terminated unicode hostvariables*/
static int p04unicto (const struct SQLCOL *this, char *buf,
	const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	tsp00_Int2 vd, tsp00_Int2 vf)
{
  vd; /* only for suppress compiler warnings*/
  vf; /* only for suppress compiler warnings*/

  if (!p04isbyte (this)) {
    tsp00_Uint4 destLengthBytes, destLengthChar;
    tsp00_Int4  isTerminated= 0, isCorrupted, isExhausted;

    if (vl==0) {
      /*determine length for character pointer hostvariables in bytes including zero terminator*/
      vl = 2*(sp81UCS2strlen((tsp81_UCS2Char*) va));
    }

    sp77encodingUCS2Native->stringInfo( va,
                                      (tsp00_Uint4) vl,
                                       TRUE,
                                       &destLengthChar,
                                       &destLengthBytes,
                                       &isTerminated,
                                       &isCorrupted,
                                       &isExhausted);
    if (     isTerminated
        && ! isCorrupted
        && ! isExhausted )
        vl = destLengthBytes;
  }
  return p04unito (this, buf, &((const char *) va)[0], cl, vl, vd, vf);

}     /*--------------------------------------------*/

static int
p04varunifrom (const struct SQLCOL *this,
		const char *buf, void *va, tsp00_Int4 cl, tsp00_Int4 vl,
		tsp00_Int2 vd, tsp00_Int2 vf)
     /* vstring */
{
  int             ret;
  tsp00_Int2 lcl;

  vd, vf;
  if (vl == 0){
    if (dbmode == CPR_KIND_ORACLE || dbmode == CPR_KIND_SAPR3) {
      tsp00_Int2 lvl;
      memcpy (&lvl, va, sizeof (lvl));
      vl = (tsp00_Int4) lvl;
    }
  }
  if (vl > 0)
    vl -= 2;
  ret = p04gunifrom (this, buf, &((char *) va)[2], &cl, &vl, vd, vf, FALSE);
  lcl = (tsp00_Int2)cl*2;
  memcpy (va, &lcl, sizeof (lcl));
  return ret;
}     /*--------------------------------------------*/


static int
p04varcharto (const struct SQLCOL *this, char *buf,
	      const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	      tsp00_Int2 vd, tsp00_Int2 vf)
     /* vstring */
{
  tsp00_Int2 lvl;
  memcpy (&lvl, va, sizeof (lvl));
  vl = (tsp00_Int4)lvl;
  return p04charto (this, buf, &((const char *) va)[2], cl, vl, vd, vf);
}     /*--------------------------------------------*/

static int
p04varcharfrom (const struct SQLCOL *this,
		const char *buf, void *va, tsp00_Int4 cl, tsp00_Int4 vl,
		tsp00_Int2 vd, tsp00_Int2 vf)
     /* vstring */
{
  int             ret;
  tsp00_Int2 lcl;

  vd, vf;
  if (vl == 0){
    if (dbmode == CPR_KIND_ORACLE || dbmode == CPR_KIND_SAPR3) {
      tsp00_Int2 lvl;
      memcpy (&lvl, va, sizeof (lvl));
      vl = (tsp00_Int4) lvl;
    }
  }
  if (vl > 0)
    vl -= 2;
  ret = p04gcharfrom (this, buf, &((char *) va)[2], &cl, &vl);
  lcl = (tsp00_Int2)cl;
  memcpy (va, &lcl, sizeof (lcl));
  return ret;
}     /*--------------------------------------------*/

static int
p04varchar1to (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vstring1 */
{
  vl = *(const unsigned char *) va;
  return p04charto (this, buf, &((const char *) va)[1], cl, vl, vd, vf);
}     /*--------------------------------------------*/

static int
p04varchar1from (const struct SQLCOL *this,
		 const char *buf, void *va, tsp00_Int4 cl, tsp00_Int4 vl,
		 tsp00_Int2 vd, tsp00_Int2 vf)
     /* vstring1 */
{
  int             ret;

  vd, vf;
  vl--;
  ret = p04gcharfrom (this, buf, &((char *) va)[1], &cl, &vl);
  *(unsigned char *) va = (unsigned char) cl;
  return ret;
}     /*--------------------------------------------*/

static int
p04varuni4to (const struct SQLCOL *this, char *buf,
	       const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	       tsp00_Int2 vd, tsp00_Int2 vf)
     /* vstring4 */
{
  memcpy (&vl, va, sizeof (tsp00_Int4));
  return p04unito (this, buf, &((const char *) va)[4], cl, vl, vd, vf);
}     /*--------------------------------------------*/

static int
p04varuni4from (const struct SQLCOL *this,
		 const char *buf, void *va, tsp00_Int4 cl, tsp00_Int4 vl,
		 tsp00_Int2 vd, tsp00_Int2 vf)
     /* vstring4 */
{
  int             ret;
  tsp00_Int4 lcl;

  vd, vf;
  if (vl == 0){
    if (dbmode == CPR_KIND_ORACLE || dbmode == CPR_KIND_SAPR3)
      memcpy (&vl, va, sizeof (tsp00_Int4));
  }
  if (vl > 0)
    vl -= 4;
  ret = p04gunifrom (this, buf, &((char *) va)[4], &cl, &vl, vd, vf, FALSE);
  lcl = (tsp00_Int4)cl*2;   /*unicode varchar4 contains length in Byte*/
  memcpy (va, &lcl, sizeof (tsp00_Int4));
  return ret;
}     /*--------------------------------------------*/

/********b.f. 10.1.95 ***********/
static int
p04varchar4to (const struct SQLCOL *this, char *buf,
	       const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	       tsp00_Int2 vd, tsp00_Int2 vf)
     /* vstring4 */
{
  memcpy (&vl, va, sizeof (tsp00_Int4));
  return p04charto (this, buf, &((const char *) va)[4], cl, vl, vd, vf);
}     /*--------------------------------------------*/
/********b.f. 10.1.95 ***********/
static int
p04varchar4from (const struct SQLCOL *this,
		 const char *buf, void *va, tsp00_Int4 cl, tsp00_Int4 vl,
		 tsp00_Int2 vd, tsp00_Int2 vf)
     /* vstring4 */
{
  int             ret;

  vd, vf;
  if (vl == 0){
    if (dbmode == CPR_KIND_ORACLE || dbmode == CPR_KIND_SAPR3)
      memcpy (&vl, va, sizeof (tsp00_Int4));
  }
  if (vl > 0)
    vl -= 4;
  ret = p04gcharfrom (this, buf, &((char *) va)[4], &cl, &vl);
  memcpy (va, &cl, sizeof (tsp00_Int4));
  return ret;
}     /*--------------------------------------------*/

static int p04grawfrom (const struct SQLCOL *this, const char *buf,
                        char *va, tsp00_Int4 * cl, tsp00_Int4 * vl);

static int
p04rawfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vraw */
{
  vd, vf;
  return p04grawfrom (this, buf, (char *) va, &cl, &vl);
}     /*--------------------------------------------*/

static int p04grawfrom (const struct SQLCOL *this, const char *buf,
			 char *va, tsp00_Int4 * cl, tsp00_Int4 * vl)
     /* all raw types */
{
  tsp00_Int4        rl = -1;
  tsp00_Int4        dl = *vl - *cl, rcl = *cl;
  tsp00_Uint4     cbCol;       /* number of bytes in column */
  char            res = (char) p04compchar (this, va), fch;

  tpr05_StringEncoding pr04cBufEncoding = pr04cGetBufEncoding(this);

  if (res)
    return (res);

  fch = *buf;
  fch = (fch == csp_unicode_def_byte) ? '\0' : fch;

  buf++; /*step over define byte*/

  /*determine column length in bytes*/
  cbCol = (pr04cBufEncoding==sp77encodingAscii)?*cl :(*cl)*2 ;
  if (*vl==0){
    *vl = cbCol;
  }

  if (this->coltype == dboolean){
    *va = *buf;
  }
  else
    if (this->colisnum) {
      int rbl;
      char rb [P04MAXDEC];

      /*vdn-number to char*/
      s42gstr (buf, 1, (int) *cl, this->colfrac, (int) *cl, rb, 1,
	       &rbl, &res);
      /*rigth aligned with leading blanks*/
      p04beautifyNumberString (va, *vl, cl, rb, (tsp00_Int4)rbl, &res, sp77encodingAscii);
      rl = *cl;
    }
    else {
      if (!p04isbyte(this) && !this->colisuni) {
        const char     *p = &buf[rcl];

        while (rcl && *--p == fch)
          rcl--;
        *cl = rcl;
        dl = *vl - rcl;
      }
      if (*vl) {
        if (dl < 0) {
          *cl = *vl;
          res = CPR_NUM_TRUNCCHAR;
        }
        else if (dl)
          memset (&va[rcl], fch, (size_t) dl);
      }
      if (!p04isbyte(this) && !this->colisuni) {
        rl = *cl;
      }
      memcpy (va, buf, (size_t) * cl);
    }
  *vl = rl;
  return (res);
}     /*--------------------------------------------*/

static int
p04int2to (const struct SQLCOL *this, char *buf,
	   const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	   tsp00_Int2 vd, tsp00_Int2 vf)
     /* vint2, vint1 */
{
  tsp00_Int2        vv;
  int             rlen;
  char            res = (char) p04compnum (this, va);

  vl, vd, vf;
  if (res)
    return (res);

  /* PTS 1104972 */
  if (vl == 1)
    vv = *(const signed char *) va;
  else
    memcpy (&vv, va, sizeof (vv));

  *buf++ = pr04cGetDefineByte(this);
  if (this->coltype == dboolean){
    *buf = (char) ((vv) ? 1 : 0);
  }
  else if (this->colisnum) {
    s41psint (buf, 1, (int) cl, this->colfrac, vv, &res);
  }
  else {
    if (!this->colisuni && pr03cGetPacketEncoding() == sp77encodingAscii ) {
        s45i2tos (vv, buf, 1, (int) cl, &rlen, &res);
    }
    else
      if ( this->colisuni || pr03cGetPacketEncoding() == sp77encodingUCS2
           || pr03cGetPacketEncoding() == sp77encodingUCS2Swapped){
      char AsciiBuf[INT4_DIG];
      /*make Ascii*/
      s45i2tos (vv, &AsciiBuf[0], 1, INT4_DIG, &rlen, &res);
      /*make UCS2 Native*/
      res =(res)?res:pr04AsciitoUCS2Buf (--buf, cl, &AsciiBuf[0], INT4_DIG, pr04cGetDefineByte(this));
    }
    else
      res = CPR_NUM_INCOMPATIBLE;
  }
  return (res);
}     /*--------------------------------------------*/

static int
p04int2from (const struct SQLCOL *this, const char *buf,
	     void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	     tsp00_Int2 vd, tsp00_Int2 vf)
     /* vint2, vint1 */
{
  tsp00_Int2        vv;
  char            res = (char) p04compnum (this, va);

  vl, vd, vf;
  if (!res)
    res = (char) p04defnum (*buf++);
  if (res)
    return (res);

  if (this->coltype == dboolean)
    vv = *buf;
  else {
    if (this->colisnum) {
      s40check (buf, 1, cl, &res);
      if (!res)
	s40gsint (buf, 1, (int) cl, &vv, &res);
    }
    else  {
      tsp00_Int4 cbCol;
      tpr05_StringEncoding pr04cBufEncoding = pr04cGetBufEncoding(this);
      cbCol = (pr04cBufEncoding==sp77encodingAscii)?cl :(cl)*2 ;
      sp45cStringtoShortInt (&vv, buf, 1, cbCol, &res, pr04cBufEncoding);
    }
    /* PTS 1104972 */
    /* PTS 1108153 copy values even if truncated  */
    if (res == num_ok || res == num_trunc) {
      if (vl == 1) {
	if (vv > MAX_SINT1_SP00 || vv < MIN_SINT1_SP00)
	  res = CPR_NUM_OVERFLOW;
	else
	  *(signed char *) va = (signed char) vv;
      }
      else
	memcpy (va, &vv, sizeof (vv));
    }
  }
  return (res);
}     /*--------------------------------------------*/

static int
p04int4to (const struct SQLCOL *this, char *buf,
	   const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	   tsp00_Int2 vd, tsp00_Int2 vf)
     /* vint4 */
{
  tsp00_Int4        vv;
  int             rlen;
  char            res = (char) p04compnum (this, va);

  vl, vd, vf;
  if (res)
    return (res);

  memcpy (&vv, va, sizeof (vv));

  *buf++ = pr04cGetDefineByte(this);
  if (this->coltype == dboolean) {
    *buf = (char) ((vv) ? 1 : 0);
  }
  else if (this->colisnum)  {
    s41plint (buf, 1, (int) cl, this->colfrac, vv, &res);
  }
  else {
    if (!this->colisuni && pr03cGetPacketEncoding() == sp77encodingAscii) {
      s45i4tos (vv, buf, 1, (int) cl, &rlen, &res);
    }
    else if ( this->colisuni || pr03cGetPacketEncoding() == sp77encodingUCS2
             || pr03cGetPacketEncoding() == sp77encodingUCS2Swapped){
      char AsciiBuf[INT4_DIG];
      /*make Ascii*/
      s45i4tos (vv, &AsciiBuf[0], 1, INT4_DIG, &rlen, &res);
      /*make UCS2 Native*/
      res =(res)?res:pr04AsciitoUCS2Buf (--buf, cl, &AsciiBuf[0], INT4_DIG, pr04cGetDefineByte(this));
    }
    else
      res = CPR_NUM_INCOMPATIBLE;
  }
  return (res);
}     /*--------------------------------------------*/

static int
p04int4from (const struct SQLCOL *this, const char *buf,
	     void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	     tsp00_Int2 vd, tsp00_Int2 vf)
     /* vint4 */
{
  tsp00_Int4        vv;
  char            res = (char) p04compnum (this, va);

  vl, vd, vf;
  if (!res)
    res = (char) p04defnum (*buf++);
  if (res)
    return (res);

  if (this->coltype == dboolean)
    vv = *buf;
  else if (this->colisnum) {
    s40check (buf, 1, cl, &res);
    if (!res)
      s40glint (buf, 1, (int) cl, &vv, &res);
  }
  else  {
      tsp00_Int4 cbCol;
      tpr05_StringEncoding pr04cBufEncoding = pr04cGetBufEncoding(this);
      cbCol = (pr04cBufEncoding==sp77encodingAscii)?cl :(cl)*2 ;
      sp45cStringtoInt (&vv, buf, 1, cbCol, &res, pr04cBufEncoding);
  }
  /**	if (!res) **bf 5.1.95 ***/
  memcpy (va, &vv, sizeof (vv));
  return (res);
}     /*--------------------------------------------*/

static int
p04int8to (const struct SQLCOL *this, char *buf,
	   const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	   tsp00_Int2 vd, tsp00_Int2 vf)
     /* vint8 */
{
  tsp00_Longint     vv;
  int             rlen;
  char            res = (char) p04compnum (this, va);

  vl, vd, vf;
  if (res)
    return (res);

  *buf++ = pr04cGetDefineByte(this);
  memcpy (&vv, va, sizeof (vv));
  if (this->coltype == dboolean) {
    *buf = (char) ((vv) ? 1 : 0);
  }
  else if (this->colisnum) {
    s41p8int (buf, 1, (int) cl, this->colfrac, vv, &res);
  }
  else {
    if ( !this->colisuni && pr03cGetPacketEncoding() == sp77encodingAscii) {
      s45i8tos (vv, buf, 1, (int) cl, &rlen, &res);
    }
    else if ( this->colisuni || pr03cGetPacketEncoding() == sp77encodingUCS2
             || pr03cGetPacketEncoding() == sp77encodingUCS2Swapped){
      char AsciiBuf[INT8_DIG];
      /*make Ascii*/
      s45i8tos (vv, &AsciiBuf[0], 1, INT8_DIG, &rlen, &res);
      /*make UCS2 Native*/
      res =(res)?res:pr04AsciitoUCS2Buf (--buf, cl, &AsciiBuf[0], INT8_DIG, pr04cGetDefineByte(this));
    }
    else
      res = CPR_NUM_INCOMPATIBLE;
  }
  return (res);
}     /*--------------------------------------------*/

static int
p04int8from (const struct SQLCOL *this, const char *buf,
	     void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	     tsp00_Int2 vd, tsp00_Int2 vf)
     /* vint8 */
{
  tsp00_Longint     vv;
  char            res = (char) p04compnum (this, va);

  vl, vd, vf;
  if (!res)
    res = (char) p04defnum (*buf++);
  if (res)
    return (res);

  if (this->coltype == dboolean)
    vv = *buf;
  else {
    if (this->colisnum) {
      s40check (buf, 1, cl, &res);
      if (!res)
	s40g8int (buf, 1, (int) cl, &vv, &res);
    }
    else
      s45stoi8 (&vv, buf, 1, (int) cl, &res);
  }
  memcpy (va, &vv, sizeof (vv));
  return (res);
}     /*--------------------------------------------*/


static int
p04uns2to (const struct SQLCOL *this, char *buf,
	   const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	   tsp00_Int2 vd, tsp00_Int2 vf)
     /* vuns2, vuns1 */
{
  tsp00_Uint2           vv;
  int             rlen;
  char            res = (char) p04compnum (this, va);

  vd, vf;
  if (res)
    return (res);

  /* PTS 1104972 */
  if (vl == 1)
    vv = *(const unsigned char *) va;
  else
    memcpy (&vv, va, sizeof (vv));

  *buf++ = pr04cGetDefineByte(this);
  if (this->coltype == dboolean) {
    *buf = (char) ((vv) ? 1 : 0);
  }
  else if (this->colisnum) {
    s41psuns (buf, 1, (int) cl, this->colfrac, vv, &res);
  }
  else {
    if ( !this->colisuni && pr03cGetPacketEncoding() == sp77encodingAscii) {
      s45u2tos (vv, buf, 1, (int) cl, &rlen, &res);
    }
    else if ( this->colisuni || pr03cGetPacketEncoding() == sp77encodingUCS2
             || pr03cGetPacketEncoding() == sp77encodingUCS2Swapped){
      char AsciiBuf[INT4_DIG];
      /*make Ascii*/
      s45u2tos (vv, &AsciiBuf[0], 1, INT4_DIG, &rlen, &res);
      /*make UCS2 Native*/
      res =(res)?res:pr04AsciitoUCS2Buf (--buf, cl, &AsciiBuf[0], INT4_DIG, pr04cGetDefineByte(this));
    }
    else
      res = CPR_NUM_INCOMPATIBLE;
  }
  return (res);
}     /*--------------------------------------------*/

static int
p04uns2from (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
     /* vuns2, vuns1 */
{
    tsp00_Uint2           vv;
    char            res = (char) p04compnum (this, va);

    vd, vf;
    if (!res)
        res = (char) p04defnum (*buf++);
    if (res)
        return (res);

    if (this->coltype == dboolean)
        vv = (tsp00_Uint2) * buf;
    else if (this->colisnum)
        {
	  s40check (buf, 1, cl, &res);
	  if (!res)
		  s40gsuns (buf, 1, (int) cl, &vv, &res);
        }
    else  {
      tsp00_Int4 cbCol;
      tpr05_StringEncoding pr04cBufEncoding = pr04cGetBufEncoding(this);
      cbCol = (pr04cBufEncoding==sp77encodingAscii)?cl :(cl)*2 ;
      sp45cStringtoUShort (&vv, buf, 1, cbCol, &res, pr04cBufEncoding);
    }
    if (res == CPR_NUM_TRUNC)
/**bf 5.1.95 ****/
        memcpy (va, &vv, sizeof (vv));
    if (!res) {
      /* PTS 1104972 */
      if (vl == 1) {
	if (vv > MAX_UINT1_SP00)
	  res = CPR_NUM_OVERFLOW;
	else
	  *(unsigned char *) va = (unsigned char) vv;
      }
      else
	memcpy (va, &vv, sizeof (vv));
    }
    return (res);
}     /*--------------------------------------------*/

static int
p04uns4to (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vuns4 */
{
    tsp00_Uint4           vv;
    int             rlen;
    char            res = (char) p04compnum (this, va);

    vl, vd, vf;
    if (res)
        return (res);

    *buf++ = pr04cGetDefineByte(this);

    memcpy (&vv, va, sizeof (vv));

    if (this->coltype == dboolean) {
      *buf = (char) ((vv) ? 1 : 0);
    }
    else if (this->colisnum)  {
      s41pluns (buf, 1, (int) cl, this->colfrac, vv, &res);
    }
    else {
      if ( !this->colisuni && pr03cGetPacketEncoding() == sp77encodingAscii) {
        s45u4tos (vv, buf, 1, (int) cl, &rlen, &res);
      }
      else if ( this->colisuni || pr03cGetPacketEncoding() == sp77encodingUCS2
               || pr03cGetPacketEncoding() == sp77encodingUCS2Swapped){
        char AsciiBuf[INT4_DIG];
        /*make Ascii*/
        s45u4tos (vv, &AsciiBuf[0], 1, INT4_DIG, &rlen, &res);
        /*make UCS2 Native*/
        res =(res)?res:pr04AsciitoUCS2Buf (--buf, cl, &AsciiBuf[0], INT4_DIG, pr04cGetDefineByte(this));
      }
      else
        res = CPR_NUM_INCOMPATIBLE;
    }
    return (res);
}     /*--------------------------------------------*/

static int
p04uns4from (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vuns4 */
{
    tsp00_Uint4           vv;
    char            res = (char) p04compnum (this, va);

    vl, vd, vf;
    if (!res)
        res = (char) p04defnum (*buf++);
    if (res)
        return (res);

    if (this->coltype == dboolean)
        vv = (tsp00_Uint4) * buf;
    else if (this->colisnum)
        {
	  s40check (buf, 1, cl, &res);
	  if (!res)
		  s40gluns (buf, 1, (int) cl, &vv, &res);
        }
    else  {
      tsp00_Int4 cbCol;
      tpr05_StringEncoding pr04cBufEncoding = pr04cGetBufEncoding(this);
      cbCol = (pr04cBufEncoding==sp77encodingAscii)?cl :(cl)*2 ;
      sp45cStringtoUInt (&vv, buf, 1, cbCol, &res, pr04cBufEncoding);
    }
/**	if (!res)  **bf 5.1.95 ***/
    memcpy (va, &vv, sizeof (vv));
    return (res);
}     /*--------------------------------------------*/

static int
p04decto (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vs,
    tsp00_Int2 vl, tsp00_Int2 vf)
/* vdec */
{
    int             rlen;
    char            res = (char) p04compnum (this, va);

    if (res)
        return (res);

    vs;
    *buf++ = pr04cGetDefineByte(this);
    if (this->coltype == dboolean)
        res = CPR_NUM_INCOMPATIBLE;
    else if (this->colisnum)
        s41pdec (buf, 1, (int) cl, this->colfrac, (const char *) va,
            vl, vf, &res);
    else
        s46dctos ((const char *) va, vl, vf, buf, 1, (int) cl,
            &rlen, &res);
    return (res);
}     /*--------------------------------------------*/

static int
p04decfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vs,
    tsp00_Int2 vl, tsp00_Int2 vf)
/* vdec */
{
    char            res = (char) p04compnum (this, va);

    vs;
    if (!res)
        res = (char) p04defnum (*buf++);
    if (res)
        return (res);

    if (this->coltype == dboolean)
        res = CPR_NUM_INCOMPATIBLE;
    else if (this->colisnum)
        {
	  s40check (buf, 1, cl, &res);
	  if (!res)
		  s40gdec (buf, 1, (int) cl, (char *) va, vl, vf,
			  &res);
        }
    else
        s46stodc ((char *) va, vl, vf, buf, 1, (int) cl, &res);
    return (res);
}     /*--------------------------------------------*/

static int
p04flo4to (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vreal4 */
{
    float           vv;
    int             rlen;
    char            res = (char) p04compnum (this, va);

    vl, vd, vf;

    if (!res && sqlansi == CPR_KIND_ANSI && dbmode == CPR_KIND_INTERNAL)
        if (this->coltype == dfixed)
            res = CPR_NUM_INCOMPATIBLE;

    if (res)
        return (res);

    *buf++ = pr04cGetDefineByte(this);
    memcpy (&vv, va, sizeof (vv));
    if (this->coltype == dboolean) {
      *buf = (char) ((vv) ? 1 : 0);
    }
    else if (this->colisnum) {
      s41psrel (buf, 1, (int) cl, this->colfrac, vv, &res);
    }
    else {
      if (!this->colisuni && pr03cGetPacketEncoding() == sp77encodingAscii) {
        s45r4tos (vv, buf, 1, (int) cl, &rlen, &res);
      }
      else if ( this->colisuni || pr03cGetPacketEncoding() == sp77encodingUCS2
               || pr03cGetPacketEncoding() == sp77encodingUCS2Swapped){
        char AsciiBuf[MAX_FLOAT];
        /*make Ascii*/
        s45r4tos (vv, &AsciiBuf[0], 1, MAX_FLOAT, &rlen, &res);
        /*make UCS2 Native*/
        res =(res)?res:pr04AsciitoUCS2Buf (--buf, cl, &AsciiBuf[0], MAX_FLOAT, pr04cGetDefineByte(this));
      }
      else
        res = CPR_NUM_INCOMPATIBLE;
    }
    return (res);
}     /*--------------------------------------------*/

static int
p04flo4from (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vreal4 */
{
    float           vv;
    char            res = (char) p04compnum (this, va);

    vl, vd, vf;
    if (!res)
        res = (char) p04defnum (*buf++);
    if (res)
        return (res);

    if (this->coltype == dboolean)
        vv = (float) *buf;
    else if (this->colisnum)
	{
	  s40check (buf, 1, cl, &res);
	  if (!res)
	  {
		  s40gsrel (buf, 1, (int) cl, &vv, &res);
		  memcpy (va, &vv, sizeof (float));
	  }
    }
    else {
      tsp00_Int4 cbCol;
      tpr05_StringEncoding pr04cBufEncoding = pr04cGetBufEncoding(this);
      cbCol = (pr04cBufEncoding==sp77encodingAscii)?cl :(cl)*2 ;
      sp45cStringtoFloat (&vv, buf, 1, cbCol, &res, pr04cBufEncoding);
      memcpy (va, &vv, sizeof (float));
    }
    return (res);
}     /*--------------------------------------------*/

static int
p04flo8to (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vreal8 */
{
    double          vv;
    int             rlen;
    char            res = (char) p04compnum (this, va);

    vl, vd, vf;

    if (!res && sqlansi == CPR_KIND_ANSI && dbmode == CPR_KIND_INTERNAL)
        if (this->coltype == dfixed)
            res = CPR_NUM_INCOMPATIBLE;

    if (res)
        return (res);

    *buf++ = pr04cGetDefineByte(this);
    memcpy (&vv, va, sizeof (vv));
    if (this->coltype == dboolean)  {
      *buf = (char) ((vv) ? 1 : 0);
    }
    else if (this->colisnum) {
      s41plrel (buf, 1, (int) cl, this->colfrac, vv, &res);
    }
    else {
      if (!this->colisuni && pr03cGetPacketEncoding() == sp77encodingAscii) {
        s45r8tos (vv, buf, 1, (int) cl, &rlen, &res);
      }
      else if ( this->colisuni || pr03cGetPacketEncoding() == sp77encodingUCS2
               || pr03cGetPacketEncoding() == sp77encodingUCS2Swapped){
        char AsciiBuf[MAX_FLOAT];
        /*make Ascii*/
        s45r8tos (vv, &AsciiBuf[0], 1, MAX_FLOAT, &rlen, &res);
        /*make UCS2 Native*/
        res =(res)?res:pr04AsciitoUCS2Buf (--buf, cl, &AsciiBuf[0], MAX_FLOAT, pr04cGetDefineByte(this));
      }
      else
        res = CPR_NUM_INCOMPATIBLE;
    }
    return (res);
}     /*--------------------------------------------*/

static int
p04flo8from (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* vreal8 */
{
    double          vv;
    char            res = (char) p04compnum (this, va);

    vl, vd, vf;
    if (!res)
        res = (char) p04defnum (*buf++);
    if (res)
        return (res);

    if (this->coltype == dboolean)
        vv = (double) *buf;
    else if (this->colisnum)
         {
	  s40check (buf, 1, cl, &res);
	  if (!res)
		  s40glrel (buf, 1, (int) cl, &vv, &res);
         }
    else {
      tsp00_Int4 cbCol;
      tpr05_StringEncoding pr04cBufEncoding = pr04cGetBufEncoding(this);
      cbCol = (pr04cBufEncoding==sp77encodingAscii)?cl :(cl)*2 ;
      sp45cStringtoDouble (&vv, buf, 1, cbCol, &res, pr04cBufEncoding);
    }
    memcpy (va, &vv, sizeof (vv));
    return (res);
}     /*--------------------------------------------*/

/* no longer needed (Cobol)
static int
p04zonto (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vs,
    tsp00_Int2 vl, tsp00_Int2 vf)*/
/* vzoned, vlzone, vlszon, vtszon */
/*{
    char            res = (char) p04compnum (this, va);

    vs;
    if (res)
        return (res);

    if (this->colisnum && this->coltype != dboolean)
        {
        *buf++ = CPR_DEFINED_BYTE;
        s41pzone (buf, 1, (int) cl, this->colfrac, (const char *) va,
            vl, vf, &res);
        }
    else
        res = CPR_NUM_INCOMPATIBLE;
    return (res);
}     --------------------------------------------*/

/* no longer needed (Cobol)
static int
p04zonfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vs,
    tsp00_Int2 vl, tsp00_Int2 vf)
{
    char            res = (char) p04compnum (this, va);

    vs;
    if (!res)
        res = (char) p04defnum (*buf++);
    if (res)
        return (res);

    if (this->colisnum && this->coltype != dboolean)
         {
	  s40check (buf, 1, cl, &res);
	  if (!res)
		  s40gzone (buf, 1, (int) cl, this->colfrac, (char *) va,
			  vl, vf, &res);
	  }
    else
        res = CPR_NUM_INCOMPATIBLE;
    return (res);
}     --------------------------------------------*/

/* no longer needed (Cobol)
static int
p04lzonfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vs,
    tsp00_Int2 vl, tsp00_Int2 vf)

{
    char            res = (char) p04compnum (this, va);

    vs;
    if (!res)
        res = (char) p04defnum (*buf++);
    if (res)
        return (res);

    if (this->colisnum && this->coltype != dboolean)
         {
	  s40check (buf, 1, cl, &res);
	  if (!res)
		  s40glzon (buf, 1, (int) cl, this->colfrac, (char *) va,
			  vl, vf, &res);
	 }
    else
        res = CPR_NUM_INCOMPATIBLE;
    return (res);
}     --------------------------------------------*/

/* no longer needed (Cobol)
static int
p04lszonfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vs,
    tsp00_Int2 vl, tsp00_Int2 vf)
{
    char            res = (char) p04compnum (this, va);

    vs;
    if (!res)
        res = (char) p04defnum (*buf++);
    if (res)
        return (res);

    if (this->colisnum && this->coltype != dboolean)
        {
	  s40check (buf, 1, cl, &res);
	  if (!res)
		  s40glszo (buf, 1, (int) cl, this->colfrac, (char *) va,
			  vl, vf,  &res);
	}
    else
        res = CPR_NUM_INCOMPATIBLE;
    return (res);
}     --------------------------------------------*/

/* no longer needed (Cobol)
static int
p04tszonfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vs,
    tsp00_Int2 vl, tsp00_Int2 vf)
{
    char            res = (char) p04compnum (this, va);

    vs;
    if (!res)
        res = (char) p04defnum (*buf++);
    if (res)
        return (res);

    if (this->colisnum && this->coltype != dboolean)
         {
	  s40check (buf, 1, cl, &res);
	  if (!res)
		  s40gtszo (buf, 1, (int) cl, this->colfrac, (char *) va,
			  vl, vf, &res);
	 }
    else
        res = CPR_NUM_INCOMPATIBLE;
    return (res);
}     --------------------------------------------*/

/* no longer needed (Cobol)
static int
p04numbto (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
{
    char            res = (char) p04compnum (this, va);

    vd, vf;
    if (res)
        return (res);

    if (this->colisnum && this->coltype != dboolean)
        {
        *buf++ = CPR_DEFINED_BYTE;
        s43pstr (buf, 1, (int) cl, this->colfrac, va, 1, (int) vl,
            &res);
        }
    else
        res = CPR_NUM_INCOMPATIBLE;
    return (res);
}     --------------------------------------------*/

/* no longer needed (Cobol)
static int
p04numbfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
{
  char            res = (char) p04compnum (this, va);

  vd, vf;
  if (!res)
    res = (char) p04defnum (*buf++);
  if (res)
    return (res);

  if (this->colisnum && this->coltype != dboolean) {
    int             rl = p04declen ((int) cl, this->colfrac), dl = (int) (vl - rl);

    if (dl < 0)
      res = CPR_NUM_OVERFLOW;
    else {
      s42gstr (buf, 1, (int) cl, this->colfrac, (int) cl,
	       (char *) va, 1, &rl, &res);
      if (dl)
	memset (&((char *) va)[rl], ' ', dl);
    }
  }
  else
    res = CPR_NUM_INCOMPATIBLE;
  return (res);
}     --------------------------------------------*/

static int
p04onumto (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* oracle number */
{
    char            onum [22] ;
    int             rlen;
    char            res = (char) p04compnum (this, va);

    vd, vf;
    if (res)
        return (res);
    rlen = (vl>21)? 21: vl ;
    memset (onum, '\0', 22) ;
    memcpy (onum, (char *)va, rlen);
    *buf++ = pr04cGetDefineByte(this);
    if (this->colisnum) {
        int i ;
        int vll = strlen (onum);
        for (i=0;i<vll;i++) {
	  if (onum[i] == (unsigned char) 102) {
	    i++ ;
	    break;
	  }
	}
        vll = i ;
        memset (buf, '\0', cl);
        s47ognum ((const char*)onum, 1, (int) vll, buf, 1, (int)cl,
                  this->colfrac, &res);
       }
    return (res);
}     /*--------------------------------------------*/

static int
p04onumfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* oracle number */
{
    tsp00_Int2 vll = vl ;
    char            res = (char) p04compnum (this, va);
    vd, vf;
    if (!res)
        res = (char) p04defnum (*buf++);
    if (res)
        return (res);

    if (this->colisnum) {
      s40check (buf, 1, cl, &res);
      if (!res) {
        s47opnum (buf, 1, (tsp00_Int2) cl, (char *) va, 1,
                 &vll, this->colfrac, &res);
        memset (((char *)va)+vll, '\0', vl-vll);
      }
    }
    return (res);
}     /*--------------------------------------------*/

static int
p04ovnumto (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* varnum */
{
    char *len = (char*) va;
    tsp00_Int4 vll = (tsp00_Int4)(len[0]);
    return p04onumto (this, buf, &((const char *) va)[1], cl, vll, vd, vf);
}     /*--------------------------------------------*/

static int
p04ovnumfrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* varnum */
{
    int len, ret;

    vd, vf;
    vl--;
    ret = p04onumfrom (this, buf, &((char *) va)[1], cl, vl, vd, vf);
    len = strlen ((char *)va);
    *(unsigned char *) va = (unsigned char)((len > vl) ? vl-1 : len-1) ;
    return ret;
}     /*--------------------------------------------*/

static int
p04odateto (const struct SQLCOL *this, char *buf,
    const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* internal date */
{
    int             rlen;
    char            res = (char) p04compnum (this, va);

    vd, vf;
    if (res)
        return (res);

    *buf++ = pr04cGetDefineByte(this);
     if (this->colisnum)
        s47ognum ((const char*)va, 1, (int) vl, buf, 1, (int) cl,
                  (int) this->colfrac, &res);
    return (res);
}     /*--------------------------------------------*/

static int
p04odatefrom (const struct SQLCOL *this, const char *buf,
    void *va, tsp00_Int4 cl, tsp00_Int4 vl,
    tsp00_Int2 vd, tsp00_Int2 vf)
/* internal date */
{
  tsp00_Int2 vll = vl ;
  char            res = (char) p04compnum (this, va);

  vd, vf;
  if (!res)
    res = (char) p04defnum (*buf++);
  if (res)
    return (res);

  if (this->colisnum) {
    s40check (buf, 1, cl, &res);
    if (!res)
      s47opnum (buf, 1, (tsp00_Int2) cl, va, 1, &vll,
		(int) this->colfrac, &res);
  }
  return (res);
}     /*--------------------------------------------*/

static int p04unito (const struct SQLCOL *this, char *buf,
	const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	tsp00_Int2 vd, tsp00_Int2 vf)
/* vunicode */
{
  char res = (char) CPR_NUM_OK;
  char rc;

  tsp00_Uint4   cbWrite;      /* number of bytes written   */
  tsp00_Uint4   cbParsed;     /* number of bytes parsed    */
  tsp00_Uint4   cbCol = cl*2; /* number of bytes in column */

  vd; /* only for suppress compiler warnings*/
  vf; /* only for suppress compiler warnings*/

  *buf++ = pr04cGetDefineByte(this);
  if (this->coltype == dboolean)  {
  /*column is boolean*/
    *buf = (char) ((((char*)va)[((sqlLocalMachineSwapKind==sw_normal)?1:0)]) ? 1 : 0);
  }
  else if (this->colisnum) {
  /*column is number*/
    tsp00_Int4 cbTempBuffer = sizeof (tsp00_C512);/*derived from vak05 kernel means max. 512 bytes are enough*/
    char     TempBuffer[sizeof (tsp00_C512)];
    /*now hostvar buffer to ASCII*/
    rc = sp78convertString( sp77encodingAscii, TempBuffer, cbTempBuffer, &cbWrite, FALSE,
	    		    sp77encodingUCS2Native, (char*)va, vl, &cbParsed);

    if (  rc == sp78_TargetExhausted ) {
      res = CPR_NUM_TRUNCCHAR;
      return (res);
    }
    else if (rc != sp78_Ok &&
             rc != sp78_TargetNotTerminated) {
      res = CPR_NUM_INVALID;
      return (res);
    }

    /*make VDNnumber from Ascii*/
    s43pstr (buf, 1, (int) cl, this->colfrac, TempBuffer, 1,(int) cbWrite, &res);
  }
  else {
    /*column is unicode or char*/
    char fillchar;
    tpr05_StringEncoding pr04cBufEncoding;
    if (p04isbyte(this)) {
      /*memcpy and fill up with hex zero*/
      pr04cBufEncoding = sp77encodingUCS2Native;
      fillchar = 0x00;
    }
    else {
      /*convert from UCS2 and fill up with blanks*/
      pr04cBufEncoding = pr04cGetBufEncoding(this);
      fillchar = 0x20;
    }
     rc = sp78convertString(pr04cBufEncoding, (char*)buf, cbCol,
	                   &cbWrite, FALSE,
                           sp77encodingUCS2Native, va, vl,
                           &cbParsed);
    if (cbCol > cbWrite) {
    /* fill up with blanks/hex zero */
      void *FillBuf = buf+cbWrite;
      tsp00_Uint4  targetLength = (cbCol - cbWrite);
      tsp00_Uint4  padLength = targetLength;
      pr04cBufEncoding->fillString(&FillBuf, &targetLength, padLength, fillchar);
    }
    if (rc == sp78_TargetExhausted) {
      res = CPR_NUM_TRUNCCHAR;
    }
    else {
      if (rc != sp78_Ok)
        res = CPR_NUM_INVALID;
    }
  } /*else*/
  return (res);
}     /*--------------------------------------------*/

static int p04gunifrom (const struct SQLCOL *this, const char *buf,
	void *va, tsp00_Int4 *cl, tsp00_Int4 *vl,
	tsp00_Int2 vd, tsp00_Int2 vf, boolean terminate)
/* vunicode */
{
  char res = CPR_NUM_OK ;
  tsp00_Uint4   cbWrite;     /* number of bytes written */
  tsp00_Uint4   cbParsed;    /* number of bytes parsed */
  tsp00_Uint4   cbCol;       /* number of bytes in column */
  tsp78ConversionResult rc;
  tpr05_StringEncoding pr04cBufEncoding = pr04cGetBufEncoding(this);

  vd; /* only for suppress compiler warnings*/
  vf; /* only for suppress compiler warnings*/
  buf++; /*step over define byte*/

  /*determine column length in bytes*/
  cbCol = (pr04cBufEncoding==sp77encodingAscii)?*cl :(*cl)*2 ;

  if (*vl==0){
  /*Note: if length of hostvariable is null (TCHAR*) then assuming
    hostvar have enougth space for current column length plus null termination*/
    *vl = (terminate) ? cbCol+(sp77encodingUCS2Native->terminatorSize): cbCol;
  }

  if (this->coltype == dboolean) {
  /*kernel type is boolean   */
    if (pr04cBufEncoding == sp77encodingUCS2) {
      ((char*)va)[0]= '\0';
      ((char*)va)[1] = *buf;
    }
    else {
      ((char*)va)[0] = *buf;
      ((char*)va)[1]= '\0';
    }
  }
  else if (this->colisnum) {
    tsp00_Int4 rbl;
    char       rb[P04MAXDEC];

    /*mysterious VDN-number to char conversation*/
    s42gstr (buf, 1, (int) *cl, this->colfrac, (int) *cl, rb, 1, &rbl, &res);
    /*make a rigth aligned number with leading blanks*/
    p04beautifyNumberString (va, *vl, cl, rb, (tsp00_Int4)rbl, &res, sp77encodingUCS2Native);
  }
  else {
  /*colisunicode*/
    char fillchar;
    tsp00_Uint4 cbColwithoutBlanks = cbCol;
    if (p04isbyte(this)) {
      /*memcpy and fill up with hex zero*/
      pr04cBufEncoding = sp77encodingUCS2Native;
      fillchar = 0x00;
    }
    else {
      /*convert to UCS2 and fill up with blanks*/
      fillchar = 0x20;
    }
    /* determine real length (in bytes) of column without padding spaces */
    cbColwithoutBlanks -=
      pr04cBufEncoding->fixedCharacterSize *
      (pr04cBufEncoding->countPadChars((void*) buf, cbCol, fillchar));

    rc = sp78convertString( sp77encodingUCS2Native, va, *vl, &cbWrite, terminate,
	    		    pr04cBufEncoding, (char*)buf,
	    		    (terminate)?cbColwithoutBlanks:cbCol, &cbParsed);

    /*if terminate is false fillup Hostvariable whith blanks*/
    if ( ! terminate
        && *vl > cbWrite)
    {
      /* fill up with blanks */
      void *FillBuf = ((char*)va)+cbWrite;
      tsp00_Uint4  targetLength = ((tsp00_Uint4)*vl - cbWrite);
      tsp00_Uint4  padLength = targetLength / 2;
      sp77encodingUCS2Native->fillString(&FillBuf, &targetLength, padLength, fillchar);
    }

    if ( rc == sp78_TargetExhausted ) {
      res = CPR_NUM_TRUNCCHAR;
    }
    else if (   rc != sp78_Ok
             && rc == sp78_TargetNotTerminated) {
      res = CPR_NUM_INVALID;
    }
  }
  return (res);
}

static int p04unicfrom (const struct SQLCOL *this, const char *buf,
	void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	tsp00_Int2 vd, tsp00_Int2 vf)
/* vunicode */
{
  return(p04gunifrom(this, buf, va, &cl, &vl, vd, vf, TRUE));

}

static int p04unifrom (const struct SQLCOL *this, const char *buf,
	void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	tsp00_Int2 vd, tsp00_Int2 vf)
/* vunicode */
{
  return(p04gunifrom(this, buf, va, &cl, &vl, vd, vf, FALSE));

}

static int p04abapto (const struct SQLCOL *this, char *buf,
	const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	tsp00_Int2 vd, tsp00_Int2 vf)
/* vabaphandle */
{
  SQLABAPDesc* ptr=(SQLABAPDesc*)va;
  char res = (char) ((this->coltype == dabaptabhandle) ? CPR_NUM_OK :
		     CPR_NUM_INCOMPATIBLE);
  int dl = cl - vl;
  if (dl < 0) {
    vl = cl;
    /* ignore the truncation of the variable */
    /* the length depends of the length from the abap tab */
    /* res = CPR_NUM_TRUNCCHAR; */
  }

  *buf++ = pr04cGetDefineByte(this);
  memcpy (buf, ptr->StreamParam, (size_t) vl);
  return (res);
}

static int p04abapfrom (const struct SQLCOL *this, const char *buf,
	void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	tsp00_Int2 vd, tsp00_Int2 vf)
/* vabaphandle */
{
  char res = (char) ((this->coltype == dabaptabhandle) ? CPR_NUM_OK :
		     CPR_NUM_INCOMPATIBLE);
  return (res);
}


static int p04vcharwithoutlento (const struct SQLCOL *this, char *buf,
	const void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	tsp00_Int2 vd, tsp00_Int2 vf)
{
  return CPR_NUM_INCOMPATIBLE;
}

static int p04vcharwithoutlenfrom (const struct SQLCOL *this, const char *buf,
	void *va, tsp00_Int4 cl, tsp00_Int4 vl,
	tsp00_Int2 vd, tsp00_Int2 vf)
/* vvcharwithoutlen */
{
  int             ret;
  tsp00_Int2 cl2;

  ret = p04gcharfrom (this, buf+2, &((char *) va)[2], &cl, &vl);
  cl2 = (tsp00_Int2)cl;
  memcpy (va, &cl2, sizeof (tsp00_Int2));
  return ret;
}

/* indicator variable conversions */

static int
p04toint2 (tsp00_Int4 ival, void *indi, tsp00_Int2 indl, tsp00_Int2 indf)
{
    tsp00_Int2        vv = (tsp00_Int2) ival;

    indl, indf;
    memcpy (indi, &vv, sizeof (vv));
    return CPR_NUM_OK;
}     /*--------------------------------------------*/

static int
p04fromint2 (tsp00_Int4 * ival, const void *indi,
    tsp00_Int2 indl, tsp00_Int2 indf)
{
    tsp00_Int2        vv;

    indl, indf;
    memcpy (&vv, indi, sizeof (vv));
    *ival = vv;
    return CPR_NUM_OK;
}     /*--------------------------------------------*/

static int
p04toint4 (tsp00_Int4 ival, void *indi, tsp00_Int2 indl, tsp00_Int2 indf)
{
    indl, indf;
    memcpy (indi, &ival, sizeof (ival));
    return CPR_NUM_OK;
}     /*--------------------------------------------*/

static int
p04fromint4 (tsp00_Int4 * ival, const void *indi,
    tsp00_Int2 indl, tsp00_Int2 indf)
{
    indl, indf;
    memcpy (ival, indi, sizeof (ival));
    return CPR_NUM_OK;
}     /*--------------------------------------------*/

static int
p04toflo4 (tsp00_Int4 ival, void *indi, tsp00_Int2 indl, tsp00_Int2 indf)
{
    float           vv = (float) ival;

    indl, indf;
    memcpy (indi, &vv, sizeof (vv));
    return CPR_NUM_OK;
}     /*--------------------------------------------*/

static int
p04fromflo4 (tsp00_Int4 * ival, const void *indi,
    tsp00_Int2 indl, tsp00_Int2 indf)
{
    float           vv;

    indl, indf;
    memcpy (&vv, indi, sizeof (vv));
    *ival = (tsp00_Int4) vv;
    return CPR_NUM_OK;
}     /*--------------------------------------------*/

static int
p04toflo8 (tsp00_Int4 ival, void *indi, tsp00_Int2 indl, tsp00_Int2 indf)
{
    double          vv = (double) ival;

    indl, indf;
    memcpy (indi, &vv, sizeof (vv));
    return CPR_NUM_OK;
}     /*--------------------------------------------*/

static int
p04fromflo8 (tsp00_Int4 * ival, const void *indi,
    tsp00_Int2 indl, tsp00_Int2 indf)
{
    double          vv;

    indl, indf;
    memcpy (&vv, indi, sizeof (vv));
    *ival = (tsp00_Int4) vv;
    return CPR_NUM_OK;
}     /*--------------------------------------------*/

static int
p04todec (tsp00_Int4 ival, void *indi, tsp00_Int2 indl, tsp00_Int2 indf)
{
    char            buf[19], res;

    s41plint (buf, 1, 18, 0, ival, &res);
    s40gdec (buf, 1, 18, (char *) indi, indl, indf, &res);
    return res;
}     /*--------------------------------------------*/

static int
p04fromdec (tsp00_Int4 * ival, const void *indi,
    tsp00_Int2 indl, tsp00_Int2 indf)
{
    char            buf[19], res;

    s41pdec (buf, 1, 18, 0, (const char *) indi, indl, indf, &res);
    s40glint (buf, 1, 18, ival, &res);
    return res;
}     /*--------------------------------------------*/

static int
p04tozon (tsp00_Int4 ival, void *indi, tsp00_Int2 indl, tsp00_Int2 indf)
{
    char            buf[19], res;

    s41plint (buf, 1, 18, 0, ival, &res);
    s40gzone (buf, 1, 18, 0, (char *) indi, indl, indf, &res);
    return res;
}     /*--------------------------------------------*/

static int
p04tolzon (tsp00_Int4 ival, void *indi, tsp00_Int2 indl, tsp00_Int2 indf)
{
    char            buf[19], res;

    s41plint (buf, 1, 18, 0, ival, &res);
    if (!res)
        s40glzon (buf, 1, 18, 0, (char *) indi, indl, indf, &res);
    return res;
}     /*--------------------------------------------*/

static int
p04tolszon (tsp00_Int4 ival, void *indi,
    tsp00_Int2 indl, tsp00_Int2 indf)
{
    char            buf[19], res;

    s41plint (buf, 1, 18, 0, ival, &res);
    s40glszo (buf, 1, 18, 0, (char *) indi, indl, indf, &res);
    return res;
}     /*--------------------------------------------*/

static int
p04totszon (tsp00_Int4 ival, void *indi,
    tsp00_Int2 indl, tsp00_Int2 indf)
{
    char            buf[19], res;

    s41plint (buf, 1, 18, 0, ival, &res);
    s40gtszo (buf, 1, 18, 0, (char *) indi, indl, indf, &res);
    return res;
}     /*--------------------------------------------*/

static int
p04fromzon (tsp00_Int4 * ival, const void *indi,
    tsp00_Int2 indl, tsp00_Int2 indf)
{
    char            buf[19], res;

    s41pzone (buf, 1, 18, 0, (const char *) indi, indl, indf, &res);
    s40glint (buf, 1, 18, ival, &res);
    return res;
}     /*--------------------------------------------*/

static void
p04newto (struct SQLCOL *col, tsp00_Int2 vt)
{
    col->colvartype = vt;
    switch (vt)
        {
        case CPR_VCHAR:
        case CPR_VRAW:
        case CPR_VRAWC:
        case CPR_VANSICHAR:
            col->coltobuf = (int (*)(const struct SQLCOL *, char *, const void *, sqlint4, sqlint4, sqlint2, sqlint2))p04charto;
            break;
        case CPR_VCHARC:
        case CPR_VCHARZ:
        case CPR_VANSICHARC:
            col->coltobuf = p04charcto;
            break;
        case CPR_VSTRING:
        case CPR_VSTRINGC:
            col->coltobuf = p04varcharto;
            break;
        case CPR_VSTRING1:
            col->coltobuf = p04varchar1to;
            break;
        case CPR_VSTRING4:
            col->coltobuf = p04varchar4to;
            break;
        case CPR_VINT1:
        case CPR_VINT2:
            col->coltobuf = p04int2to;
            break;
        case CPR_VINT4:
            col->coltobuf = p04int4to;
            break;
        case CPR_VINT8:
            col->coltobuf = p04int8to;
            break;
        case CPR_VUNS1:
        case CPR_VUNS2:
            col->coltobuf = p04uns2to;
            break;
        case CPR_VUNS4:
            col->coltobuf = p04uns4to;
            break;
        case CPR_VDECIMAL:
            col->coltobuf = p04decto;
            break;
        case CPR_VREAL4:
            col->coltobuf = p04flo4to;
            break;
        case CPR_VREAL8:
        case CPR_VREAL8SUN:
            col->coltobuf = p04flo8to;
            break;
/* no longer needed
        case CPR_VZONED:
        case CPR_VLZONE:
        case CPR_VLSZON:
        case CPR_VTSZON:
            col->coltobuf = p04zonto;
            break;
        case CPR_VNUMBER:
            col->coltobuf = p04numbto;
            break; */
        case CPR_VONUMBER:
            col->coltobuf = p04onumto;
            break;
        case CPR_VOVARNUM:
            col->coltobuf = p04ovnumto;
            break;
        case CPR_VODATE:
            col->coltobuf = p04odateto ;
            break;
        case CPR_VUCS2:
        case CPR_VUTF16:
        case CPR_VUNICODE:
            col->coltobuf = p04unito;
            break;
        case CPR_VUNICODEC:
            col->coltobuf = p04unicto;
            break;
        case CPR_VSTRINGUNICODE:
            col->coltobuf = p04varunito;
            break;
        case CPR_VSTRINGUNICODE4:
            col->coltobuf = p04varuni4to;
            break;
        case CPR_VFILE:
        case CPR_VFILEC:
            col->coltobuf = p04incompto;
            break;
	case CPR_VABAPHANDLE:
            col->coltobuf = p04abapto;
            break;
	case CPR_VVARCHAR_WITHOUT_LEN:
            col->coltobuf = p04vcharwithoutlento;
            break;
	case CPR_VLONGDESC:
	    col->coltobuf = p04varcharto;
	    break;
        default:
            col->coltobuf = p04defaultto;
        }
}     /*--------------------------------------------*/

static void
p04newfrom (struct SQLCOL *col, tsp00_Int2 vt)
{
    col->colvartype = vt;
    switch (vt)
        {
        case CPR_VCHAR:
            col->colfrombuf = p04charfrom;
            break;
        case CPR_VRAW:
        case CPR_VRAWC:
            col->colfrombuf = p04rawfrom;
            break;
        case CPR_VANSICHAR:
            col->colfrombuf = p04ansicharfrom;
            break;
        case CPR_VCHARC:
        case CPR_VCHARZ:
            col->colfrombuf = p04charcfrom;
            break;
        case CPR_VANSICHARC:
            col->colfrombuf = p04ansicharcfrom;
            break;
        case CPR_VSTRING:
        case CPR_VSTRINGC:
            col->colfrombuf = p04varcharfrom;
            break;
        case CPR_VSTRING1:
            col->colfrombuf = p04varchar1from;
            break;
        case CPR_VSTRING4:
            col->colfrombuf = p04varchar4from;
            break;
        case CPR_VINT1:
        case CPR_VINT2:
            col->colfrombuf = p04int2from;
            break;
        case CPR_VINT4:
            col->colfrombuf = p04int4from;
            break;
        case CPR_VINT8:
            col->colfrombuf = p04int8from;
            break;
        case CPR_VUNS1:
        case CPR_VUNS2:
            col->colfrombuf = p04uns2from;
            break;
        case CPR_VUNS4:
            col->colfrombuf = p04uns4from;
            break;
        case CPR_VDECIMAL:
            col->colfrombuf = p04decfrom;
            break;
        case CPR_VREAL4:
            col->colfrombuf = p04flo4from;
            break;
        case CPR_VREAL8:
        case CPR_VREAL8SUN:
            col->colfrombuf = p04flo8from;
            break;
/* no longer needed (Cobol)
        case CPR_VZONED:
            col->colfrombuf = p04zonfrom;
            break;
        case CPR_VLZONE:
            col->colfrombuf = p04lzonfrom;
            break;
        case CPR_VLSZON:
            col->colfrombuf = p04lszonfrom;
            break;
        case CPR_VTSZON:
            col->colfrombuf = p04tszonfrom;
            break;
        case CPR_VNUMBER:
            col->colfrombuf = p04numbfrom;
            break; */
        case CPR_VONUMBER:
            col->colfrombuf = p04onumfrom;
            break;
        case CPR_VOVARNUM:
            col->colfrombuf = p04ovnumfrom;
            break;
        case CPR_VODATE:
            col->colfrombuf = p04odatefrom ;
            break;
        case CPR_VUCS2:
        case CPR_VUTF16:
        case CPR_VUNICODE:
	    col->colfrombuf = p04unifrom;
            break;
        case CPR_VUNICODEC:
	    col->colfrombuf = p04unicfrom;
            break;
        case CPR_VSTRINGUNICODE:
	    col->colfrombuf = p04varunifrom;
            break;
        case CPR_VSTRINGUNICODE4:
	    col->colfrombuf = p04varuni4from;
            break;
        case CPR_VFILE:
        case CPR_VFILEC:
            col->colfrombuf = (int (*)(const struct SQLCOL *, const char *, void *, sqlint4, sqlint4, sqlint2, sqlint2))p04incompfrom;
            break;
	case CPR_VABAPHANDLE:
            col->colfrombuf = p04abapfrom;
            break;
	case CPR_VVARCHAR_WITHOUT_LEN:
            col->colfrombuf = p04vcharwithoutlenfrom;
            break;
        default:
            col->colfrombuf = p04defaultfrom;
        }
}     /*--------------------------------------------*/

static void
p04newtoindi (struct SQLCOL *col, tsp00_Int2 it)
{
    col->colindtype = it;
    switch (it)
        {
        case CPR_VINT2:
            col->coltoindi = p04toint2;
            break;
        case CPR_VINT4:
            col->coltoindi = p04toint4;
            break;
        case CPR_VDECIMAL:
            col->coltoindi = p04todec;
            break;
        case CPR_VREAL4:
            col->coltoindi = p04toflo4;
            break;
        case CPR_VREAL8:
        case CPR_VREAL8SUN:
            col->coltoindi = p04toflo8;
            break;
        case CPR_VZONED:
            col->coltoindi = p04tozon;
            break;
        case CPR_VLZONE:
            col->coltoindi = p04tolzon;
            break;
        case CPR_VLSZON:
            col->coltoindi = p04tolszon;
            break;
        case CPR_VTSZON:
            col->coltoindi = p04totszon;
            break;
        default:
            col->coltoindi = p04dftoindi;
        }
}     /*--------------------------------------------*/

static void
p04newfromindi (struct SQLCOL *col, tsp00_Int2 it)
{
    col->colindtype = it;
    switch (it)
        {
        case CPR_VINT2:
            col->colfromindi = p04fromint2;
            break;
        case CPR_VINT4:
            col->colfromindi = p04fromint4;
            break;
        case CPR_VDECIMAL:
            col->colfromindi = p04fromdec;
            break;
        case CPR_VREAL4:
            col->colfromindi = p04fromflo4;
            break;
        case CPR_VREAL8:
        case CPR_VREAL8SUN:
            col->colfromindi = p04fromflo8;
            break;
        case CPR_VZONED:
        case CPR_VLZONE:
        case CPR_VLSZON:
        case CPR_VTSZON:
            col->colfromindi = p04fromzon;
            break;
        default:
            col->colfromindi = p04dffromindi;
        }
}     /*--------------------------------------------*/

void
p04err (sqlratype * sqlrap, struct SQLERROR *sqlemp,  char err)
{
  sqltatype       *tap = sqlrap->rasqltap;
  char             retline[80];
  tsp00_Int2         retll;

  int             trace = (tap->tatrout == CPR_TRACE_LONG
			   || tap->tatrout == CPR_TRACE_MODLONG);

  if (trace) {
    retll = tap->tastr80l;
    memcpy (retline, tap->tastr80, retll);
  }
  p03cseterror (sqlemp,  err);
  p03traceerror (sqlrap, sqlemp);
  if (trace) {
    tap->tastr80l = retll;
    memcpy (tap->tastr80, retline, retll);
  }
}     /*--------------------------------------------*/

void
p04invaliderr (sqlratype * sqlrap, const char *buf, struct SQLCOL *col,
	tsp00_Int2 vt, tsp00_Int4 vl, tsp00_Int2 vf, tsp00_Int2 inout)
{
  sqltatype       *tap = sqlrap->rasqltap;
  char             retline[80];
  tsp00_Int2         retll;

  int             trace = (tap->tatrout == CPR_TRACE_LONG
			   || tap->tatrout == CPR_TRACE_MODLONG);

  if (trace) {
    retll = tap->tastr80l;
    memcpy (retline, tap->tastr80, retll);
  }
  p03invaliderr (sqlrap, 1, inout, buf, vt, vl, vf);
  p03invaliderr (sqlrap, 2, inout, buf, (tsp00_Int2) col->coltype,
		 col->collen, (tsp00_Int2) col->colfrac);
  if (trace) {
    tap->tastr80l = retll;
    memcpy (tap->tastr80, retline, retll);
  }
}     /*--------------------------------------------*/

static void p04ldinit (struct SQLLD *ld)
{
  ld->ldpc_mdesc.descElemPtr = (void**) &ld->ldpc;
  ld->ldsp_mdesc.descElemPtr = (void**) &ld->ldsp;
  if (ld->ldpc == 0)
    ld->ldpc_mdesc.descNelem = ld->ldpc_mdesc.descMaxelem = 0;
  if (ld->ldsp == 0) {
    ld->ldsp_mdesc.descNelem = ld->ldsp_mdesc.descMaxelem = 0;
  }
  ld->ldpcarr_mdesc.descElemPtr = (void**) &ld->ldpcarr;
  ld->ldsparr_mdesc.descElemPtr = (void**) &ld->ldsparr;
  if (ld->ldpcarr == 0)
    ld->ldpcarr_mdesc.descNelem = ld->ldpcarr_mdesc.descMaxelem = 0;
  if (ld->ldsparr == 0) {
    ld->ldsparr_mdesc.descNelem = ld->ldsparr_mdesc.descMaxelem = 0;
  }
}

static int
p04longin (int colislong, struct SQLLD **ld, char *buf)
{
  int             ret = colislong;
  struct SQLSPLD *ldldsp;

  p04ldinit (*ld);
  if ((*ld)->ldpc_mdesc.descNelem == (*ld)->ldsp_mdesc.descNelem) {
    if ((*ld)->ldmaxi == 0)
      (*ld)->ldpc_mdesc.descNelem = (*ld)->ldsp_mdesc.descNelem = 0;
    p03dynalloc (&(*ld)->ldpc_mdesc);
  }
  ldldsp = (struct SQLSPLD*) p03dynalloc (&(*ld)->ldsp_mdesc);
  if (colislong == CPR_OLDLONG) {
    static struct OLDSQLSPLD ldsp;
    static char     ldinit = 1;

    if (ldinit) {
      ldinit = 0;
      memset (&ldsp, 0, sizeof (ldsp));
      ldsp.ld_valmode = vm_nodata;
    }
    ldldsp->ldkind = cpr_is_false;
    ldldsp->ldlength = sizeof (ldsp);
    (*ld)->ldcolkind = cpr_is_true;
    ldsp.ld_valind =  ++(*ld)->ldmaxi;
    *buf++ = CPR_DEFINED_BYTE;
    memcpy (buf, &ldsp, sizeof (ldsp));
  }
  else {
    if (colislong == CPR_NEWLONG && *buf == CPR_LONG_DESC_DEF_BYTE) {
      static struct OLDSQLSPLD ldsp;
      static char     ldinit = 1;

      if (ldinit) {
	ldinit = 0;
	memset (&ldsp, 0, sizeof (ldsp));
	ldsp.ld_valmode = vm_nodata;
      }
      ldldsp->ldkind = cpr_is_true;
      ldldsp->ldlength = sizeof (ldsp);
      (*ld)->ldcolkind = cpr_is_true;
      ldsp.ld_valind =  ++(*ld)->ldmaxi;
      memcpy (++buf, &ldsp, sizeof (ldsp));
    }
    else {
      (*ld)->ldcolkind = cpr_is_false;
      (*ld)->ldpc_mdesc.descNelem--;
      (*ld)->ldsp_mdesc.descNelem--;
      ret = 0;
    }
  }
  return ret;
}     /*--------------------------------------------*/

static int
p04longout (char colislong, struct SQLLD **ld, char *buf)
{
  int             ret = colislong;
  struct SQLSPLD *ldldsp;

  p04ldinit (*ld);
  if ((*ld)->ldmaxo == 0)
    (*ld)->ldpc_mdesc.descNelem = (*ld)->ldsp_mdesc.descNelem = 0;
  ldldsp = (struct SQLSPLD*) p03dynalloc (&(*ld)->ldsp_mdesc);
  if (colislong == CPR_OLDLONG) {
    struct OLDSQLSPLD *ldsp = &(*ld)->ldsp[(*ld)->ldmaxo].ld.o;

    ldldsp->ldkind = cpr_is_false;
    ldldsp->ldlength = sizeof (*ldsp);
    (*ld)->ldcolkind = cpr_is_true;
    memcpy (ldsp, ++buf, sizeof (*ldsp));
    ldsp->ld_valind =  ++(*ld)->ldmaxo;
  }
  else {
    if (colislong == CPR_NEWLONG && *buf == CPR_LONG_DESC_DEF_BYTE) {
      struct OLDSQLSPLD *ldsp = &(*ld)->ldsp[(*ld)->ldmaxo].ld.o;

      ldldsp->ldkind = cpr_is_true;
      ldldsp->ldlength = sizeof (*ldsp);
      (*ld)->ldcolkind = cpr_is_true;
      memcpy (ldsp, ++buf, sizeof (*ldsp));
      ldsp->ld_valind =  ++(*ld)->ldmaxo;
    }
    else {
      (*ld)->ldcolkind = cpr_is_false;
      (*ld)->ldsp_mdesc.descNelem--;
      ret = 0;
    }
  }
  return ret;
}     /*--------------------------------------------*/

void p04coltobuf (sqlratype * sqlrap, char *databuf, tsp00_Int4 *buflen,
		  struct SQLCOL *col, const char *vn, const void *va,
		  tsp00_Int2 vt, tsp00_Int4 vl, tsp00_Int2 vd, tsp00_Int2 vf,
		  tsp00_Int4 ival, tsp00_Int4 offs, int pno,
		  struct SQLERROR *sqlemp)
{
  struct SQLLD   **ld = &sqlrap->rasqlldp;
  tsp00_Int4        pos = col->colpos + offs;
  char           *buf = databuf + pos;
  sqltatype      *tap = sqlrap->rasqltap;
  int             trace =
    (tap->tatrout == CPR_TRACE_LONG ||
     tap->tatrout == CPR_TRACE_MODLONG);
  tsp00_Int2       *ll = &tap->tastr80l;
  char           err;

  M90TRACE (M90_TR_ENTRY, "p04coltobuf ", 0);
  err = cpr_p_ok;
  pos += col->coliolen;
  if (*buflen < pos)
    *buflen = pos;
  if (sqlansi == CPR_KIND_ANSI) {
    if (vt == CPR_VCHARC)
      vt = CPR_VANSICHARC;
    else if (vt == CPR_VCHAR)
      vt = CPR_VANSICHAR;
  }
  else {
    if (dbmode == CPR_KIND_ORACLE || dbmode == CPR_KIND_SAPR3) {
      if (vt == CPR_VCHARC && vl > 0) {
	vt = CPR_VCHAR;
      }
      if (dbmode == CPR_KIND_ORACLE) {
	/* ADIS 1001497 */
	if (p04IsEmptyString((char *)va, col->coltype, vt, vl)) {
	  ival = CPR_NULL_DATA;
	}
      }
    }
  }
  if (trace)
    p04trvinit (vn, pno, tap->tastr80, ll, CPR_TR_INP);
  if (ival == CPR_DEFAULT_PARAM) {  /* PTS 1118562 */
    *buf = CPR_DEFAULT_BYTE;
    if (!(col->colmode & 1 << CPR_INFO_DEFAULT)) {
      if (err == cpr_p_ok) {
	err = cpr_inp_default_not_allowed;
	p04err (sqlrap, sqlemp, err);
      }
    }
    if (trace) {
      char     s[] = " IS DEFAULT";
      int      ls = sizeof (s) - 1;

      memcpy (&tap->tastr80[*ll], s, ls);
      *ll += ls;
      p08vfwritetrace (sqlrap);
    }
    if (p04longin (col->colislong, ld, buf)) {
      tsp00_Int2        i = (*ld)->ldmaxi - (tsp00_Int2) 1;
      struct SQLPCLD *p = &(*ld)->ldpc[i];

      p->ldhostaddr = (void *) va;
      p->ldhosttype = vt;
      p->ldhostlen = vl;
      p->ldhostlensum = vl;  /*bf odbc */
      p->ldcolno = (tsp00_Int2) pno;
      p->ldsfdtype = (char) col->coltype;
      p->ldsfiotype = (char) col->colio;
      p->ldsfbufpos = col->colpos + offs + 1;
    }
  }
  else if (ival == CPR_NULL_DATA) {
    /* Geaenderte if-Abfrage (urspruenglich auf ival < 0
       getestet), da der Wert -6 als SQL_IGNORE beim
       Handling von Longspalten interpretiert wird und in
       diesem Fall keinen Nullwert anzeigt. (TS 13.04.2000) */
    *buf = CPR_UNDEF_BYTE;
    if (col->colmode & 1 << CPR_INFO_MANDATORY) {
      if (err == cpr_p_ok) {
	err = cpr_inp_null_not_allowed;
	p04err (sqlrap, sqlemp, err);
      }
    }
    if (trace) {
      static char     s[] = " IS NULL";
      static int      ls = sizeof (s) - 1;

      memcpy (&tap->tastr80[*ll], s, ls);
      *ll += ls;
      p08vfwritetrace (sqlrap);
    }
  }
  else {
    int             res = CPR_NUM_OK;
    int             ls = 0;
    const char     *s = "";

    if (p04longin (col->colislong, ld, buf)) {
      tsp00_Int2        i = (*ld)->ldmaxi - (tsp00_Int2) 1;
      struct SQLPCLD *p = &(*ld)->ldpc[i];

      p->ldhostaddr = (void *) va;
      p->ldhosttype = vt;
      p->ldhostlen = vl;
      p->ldhostlensum = vl;  /*bf odbc */
      p->ldcolno = (tsp00_Int2) pno;
      p->ldsfdtype = (char) col->coltype;
      p->ldsfiotype = (char) col->colio;
      p->ldsfbufpos = col->colpos + offs + 1;
    }
    else {
      if (vt != col->colvartype) {
	p04newto (col, vt);
	if (col->colio == CPR_INFO_INOUT)
	  p04newfrom (col, vt);
      }
      res = col->coltobuf (col, buf, va, col->collen, vl, vd, vf);
    }
    switch (res) {
    case CPR_NUM_TRUNC: {
      s = " TRUNCATED";
      if ((sqlansi == CPR_KIND_ANSI
	   && (vt == CPR_VREAL4 || vt == CPR_VREAL8 || vt == CPR_VREAL8SUN))
	  || vt == CPR_VCHAR || vt == CPR_VCHARC
	  || vt == CPR_VANSICHAR || vt == CPR_VANSICHARC
	  || vt == CPR_VSTRING || vt == CPR_VSTRINGC
	  || vt == CPR_VSTRING1 || vt == CPR_VSTRING4
	  || vt == CPR_VBUF || vt == CPR_VRAW || vt == CPR_VRAWC) {
	if (err == cpr_p_ok) {
	  res = CPR_NUM_OK;
	  err =cpr_inp_string_truncated;
	  if ((dbmode == CPR_KIND_ORACLE)
	      || (dbmode == CPR_KIND_SAPR3))
	    err =
	      cpr_inp_string_truncated_oracle;
	}
      }
      else if (err == cpr_p_ok)
	err = cpr_inp_number_truncated;
      break;
    }
    case CPR_NUM_INVESC: {
      s = " INVALID ESCAPE";
      if (err == cpr_p_ok)
	err = cpr_invalid_escape;
      break;
    }
    case CPR_NUM_UNTERM: {
      s = " UNTERMINATED";
      if (err == cpr_p_ok)
	err = cpr_unterminated_c_string;
      break;
    }
    case CPR_NUM_OVERFLOW: {
      s = " OVERFLOW";
      *buf = CPR_UNDEF_BYTE;
      if (err == cpr_p_ok)
	err = cpr_inp_number_overflow;
      break;
    }
    case CPR_NUM_INVALID: {
      s = " INVALID";
      *buf = CPR_UNDEF_BYTE;
      if (err == cpr_p_ok)
	err = cpr_inp_number_invalid;
      break;
    }
    case CPR_NUM_INCOMPATIBLE: {
      s = " INCOMPATIBLE";
      if (err == cpr_p_ok)
	err = cpr_incompatible_datatype;
      break;
    }
    case CPR_NUM_MISSING_ADDR: {
      s = " MISSING ADDR";
      if (err == cpr_p_ok)
	err = cpr_missing_variable_addr;
      break;
    }
    }
    if (err != cpr_p_ok) {
      const char * vabuf = va;
      p04err (sqlrap, sqlemp, err);
      if ((err == cpr_incompatible_datatype)
	  || (err == cpr_inp_number_invalid))
	p04invaliderr (sqlrap,  vabuf,
		       col, vt, vl, vf,1);
    }
    if (trace) {
      if (res != CPR_NUM_INVALID
	  && res != CPR_NUM_INCOMPATIBLE
	  && res != CPR_NUM_MISSING_ADDR)
	p04trvwrite (sqlrap, col, buf);
      if (res != CPR_NUM_OK) {
	ls = strlen (s);
	if (*ll + ls > sizeof (tap->tastr80)) {
	  static char     s[] = " ...   : ";

	  p08vfwritetrace (sqlrap);
	  *ll = sizeof (s) - 1;
	  memcpy (tap->tastr80, s, *ll);
	}
	memcpy (&tap->tastr80[*ll], s, ls);
	*ll += ls;
      }
      p08vfwritetrace (sqlrap);
    }
  }
  M90TRACE (M90_TR_EXIT, "p04coltobuf ", 0);
}     /*--------------------------------------------*/

/*!
  Function:     p04colfrombuf

  see also: vpr00c

  Description:  Conversion function for output host variables.

  Arguments:
    sqlrap  [in|out] pointer to a sqlratype structure
    databuf [in|out] pointer to buffer containing data
    buflen  [in|out] length of data buffer
    col     [in|out] pointer to a SQLCOL structure
    vn      [in|out] pointer to a variable name string
    va      [in|out] host variable address
    vt      [in]     host variable type as defined in vpc00 / vpr00c
    vl      [in]     host variable length
    vd      [in]     host variable ???
    vf      [in]     host variable fraction
    ival    [in|out] input indicator value
    offs    [in]     offset in part two of the message buffer
    pno     [in]     parameter number
    sqlemp  [in|out] pointer to struct SQLERROR - status of the conversion operation errors

  return value: void
*/
void p04colfrombuf (sqlratype * sqlrap, char *databuf, tsp00_Int4 *buflen,
	            struct SQLCOL *col, const char *vn, void *va,
	            tsp00_Int2 vt, tsp00_Int4 vl, tsp00_Int2 vd, tsp00_Int2 vf,
	            tsp00_Int4 * ival, int offs, int pno,
	            struct SQLERROR *sqlemp)
{
  struct SQLLD   **ld = &sqlrap->rasqlldp;
  tsp00_Int4        pos = col->colpos + offs;
  char           *buf = databuf + pos;
  sqltatype      *tap = sqlrap->rasqltap;
  int             trace =
    (tap->tatrout == CPR_TRACE_LONG || tap->tatrout == CPR_TRACE_MODLONG);
  tsp00_Int2       *ll = &tap->tastr80l;
  char           err;
  tsp00_Int2 colvartype;
  M90TRACE (M90_TR_ENTRY, "p04colfrombuf ", 0);
  err = cpr_p_ok;
  if (*buflen  < pos + col->coliolen - 1) {
    if (err == cpr_p_ok) {
      err = cpr_output_part_too_short;
      p04err (sqlrap, sqlemp, err);
    }
    return;
  }
  if (sqlansi == CPR_KIND_ANSI) {
    if (vt == CPR_VCHARC)
      vt = CPR_VANSICHARC;
    else if (vt == CPR_VCHAR)
      vt = CPR_VANSICHAR;
  }
  else {
    if (dbmode == CPR_KIND_ORACLE || dbmode == CPR_KIND_SAPR3) {
      if (vt == CPR_VCHARC && vl > 0)
	vt = CPR_VCHAR;
      if (vt == CPR_VUNICODEC && vl > 0)
	vt = CPR_VUNICODE;
    }
  }
  if (trace)
    p04trvinit (vn, pno, tap->tastr80, ll, CPR_TR_OUTP);
  if (*buf == CPR_UNDEF_BYTE) {
    *ival = CPR_NULL_DATA;
    (*ld)->ldcolkind = cpr_is_false;
    if (trace) {
      static char     s[] = " IS NULL";
      static int      ls = sizeof (s) - 1;

      memcpy (&tap->tastr80[*ll], s, ls);
      *ll += ls;
      p08vfwritetrace (sqlrap);
    }
  }
  else {
    if (*buf == CPR_OFLW_BYTE) {
      *ival = -2;
      if (trace) {
	static char     s[] = " IS EXCEPTION";
	static int      ls = sizeof (s) - 1;

	memcpy (&tap->tastr80[*ll], s, ls);
	*ll += ls;
	p08vfwritetrace (sqlrap);
      }
    }
    else {
      int             res = CPR_NUM_OK;
      tsp00_Int4        cl = (col->colisuni) ? col->collen : col->collen;
      int             ls = 0;
      const char     *s = "";

      *ival = 0;
      if (p04longout (col->colislong, ld, buf)) {
	struct SQLPCLD *p = (struct SQLPCLD*) p03dynalloc (&(*ld)->ldpc_mdesc);

	p->ldhostaddr = (void *) va;
	p->ldhosttype = vt;
	p->ldhostlen = vl;
	p->ldhostlensum = vl;  /*bf odbc */
	p->ldcolno = (tsp00_Int2) pno;
	p->ldsfdtype = (char) col->coltype;
	p->ldsfiotype = (char) col->colio;
	p->ldsfbufpos = pos + 1;
	p->ldrestdata = NULL;
      }
      else if ( err == cpr_p_ok ) {
	colvartype = col->colvartype;
	if (vt != col->colvartype)
	  p04newfrom (col, vt);
	res = col->colfrombuf (col, buf, va, cl, vl, vd, vf);
      }
      switch (res) {
      case CPR_NUM_TRUNC: {
	s = " TRUNCATED";
	*ival = cl;
	err = cpr_inp_string_truncated_oracle;
	break;
      }
      case CPR_NUM_OVERFLOW: {
	s = " OVERFLOW";
	*ival = -2;
	if (err == cpr_p_ok)
	  err = cpr_out_number_overflow;
	break;
      }
      case CPR_NUM_INVALID: {
	s = " INVALID";
	*ival = CPR_NULL_DATA;
	if (err == cpr_p_ok)
	  err = cpr_out_number_invalid;
	break;
      }
      case CPR_NUM_INCOMPATIBLE: {
	s = " INCOMPATIBLE";
	if (err == cpr_p_ok)
	  err = cpr_incompatible_datatype;
	break;
      }
      case CPR_NUM_MISSING_ADDR: {
	s = " MISSING ADDR";
	if (err == cpr_p_ok)
	  err = cpr_missing_variable_addr;
	break;
      }
      }
      if (err != cpr_p_ok) {
	p04err (sqlrap, sqlemp, err);
	if ((err == cpr_incompatible_datatype)
	    || (err == cpr_out_number_invalid))
	  p04invaliderr (sqlrap, buf, col, vt, vl, vf, 2);
      }
      if (trace) {
	if (res != CPR_NUM_INVALID && res != CPR_NUM_INCOMPATIBLE &&
	    res != CPR_NUM_MISSING_ADDR)
	  p04trvwrite (sqlrap, col, buf);
	if (res != CPR_NUM_OK) {
	  ls = strlen (s);
	  if (*ll + ls > sizeof (tap->tastr80)) {
	    static char     s[] = " ...   : ";

	    p08vfwritetrace (sqlrap);
	    *ll = sizeof (s) - 1;
	    memcpy (tap->tastr80, s, *ll);
	  }
	  memcpy (&tap->tastr80[*ll], s, ls);
	  *ll += ls;
	}
	p08vfwritetrace (sqlrap);
      }
    }
  }
  M90TRACE (M90_TR_EXIT, "p04colfrombuf ", 0);
}     /*--------------------------------------------*/

void
p04coltoindi (const sqlcatype * sqlca, sqlxatype * sqlxa,
    struct SQLCOL *col, sqlv1pointer va1, char *base,
    tsp00_Int4 ival, char *err)
{
  void           *addr = (va1->cm.va1indi == SQLVACM) ?
    base + va1->cm.va1offs :  base;

  if (sqlca->sqlcode >= 0) {
    int             res;
    sqlv2pointer    va2 = sqlxa->sqlv2p + va1->sc.va1indva2 - 1;

    if (va2->va2typ != col->colindtype) {
      p04newtoindi (col, va2->va2typ);
      if (col->colio == CPR_INFO_INOUT)
	p04newfromindi (col, va2->va2typ);
    }
    if (va2->va2const > 0) {
      if (*err == cpr_p_ok)
	*err = cpr_rt_no_output_variable;
    }
    else {
      res = col->coltoindi (ival, addr, va2->va2digit, va2->va2frac);
      if (res == CPR_NUM_INVALID)
	if (*err == cpr_p_ok)
	  *err = cpr_unknown_datatype;
    }
  }
}     /*--------------------------------------------*/

int
p04col1toindi (struct SQLCOL *col, tsp00_Int4 ival, void *va,
    tsp00_Int2 vt, tsp00_Int2 vl, tsp00_Int2 vf)
{
  if (vt != col->colindtype) {
    p04newtoindi (col, vt);
    if (col->colio == CPR_INFO_INOUT)
      p04newfromindi (col, vt);
  }
  return col->coltoindi (ival, va, vl, vf);
}     /*--------------------------------------------*/

void
p04colfromindi (const sqlcatype * sqlca, sqlxatype * sqlxa,
    struct SQLCOL *col, sqlv1pointer va1, char *base,
    tsp00_Int4 * ival, char *err)
{
    struct SQLLD   **ld = &sqlca->sqlrap->rasqlldp;
    struct SQLPCLD *p;

    p04ldinit (*ld);
    if ((*ld)->ldmaxi == 0)
	    (*ld)->ldpc_mdesc.descNelem = (*ld)->ldsp_mdesc.descNelem = 0;
    p = (struct SQLPCLD*) p03dynalloc (&(*ld)->ldpc_mdesc);
    *ival = 0;
    if (va1) {
      void           *addr = (va1->cm.va1indi == SQLVACM) ? base +
	va1->cm.va1offs : base;
      int             res;
      sqlv2pointer    va2 = sqlxa->sqlv2p + va1->sc.va1indva2 - 1;
      /* struct SQLPCLD *p = (struct SQLPCLD*) p03dynalloc (&(*ld)->ldpc_mdesc); */

      p->ldcoladdr = col;
      p->ldindkaddr = addr;
      p->ldindktype = va2->va2typ;
      p->ldindklen = va2->va2size;
      p->ldindkfrac = va2->va2frac;
      if (va2->va2typ != col->colindtype)
	p04newfromindi (col, va2->va2typ);
      res = col->colfromindi (ival, addr, va2->va2digit, va2->va2frac);
      if (res == CPR_NUM_INVALID) {
	if (*err == cpr_p_ok) {
	  *err = cpr_unknown_datatype;
	}
      }
    }
}     /*--------------------------------------------*/

static char    *
p04addr (sqlxatype * sqlxa, sqlv1pointer va, tsp00_Int4 lpind)
{
    char           *addr = (va->pt.va1indi == SQLVAPT) ?
    *((char **) va->pt.va1addr + lpind) :
    (va->st.va1indi == SQLVAST) ?
    (char *) va->st.va1addr + lpind * va->st.va1size :
    (va->sc.va1indi == SQLVASC) ?
    (char *) va->sc.va1addr +
    lpind * sqlxa->sqlv2p[va->sc.va1indva2 - 1].va2size :
    0;

    return addr;
}     /*--------------------------------------------*/

void
p04input (sqlcatype * sqlca, sqlxatype * sqlxa, sqlgaentry *gae, struct tpr_sqlloop *lp)
{
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  sqlkapointer    ka = sqlxa->xaSQLDesc->ka;
  sqlorentry *ore = sqlxa->xaSQLDesc->ore;
  tsp1_part *pidpart;
  tsp00_Uint1 *pid;

  M90TRACE (M90_TR_ENTRY, "p04input ", 0);
  p03find_part (sqlca->sqlrap, sp1pk_parsid, &pidpart);
  pid = pidpart->sp1p_buf;
  if (ka->kapaindex == 0) {
    if (pid [CPR_PARS_IDIX] == csp1_p_mass_command
	|| pid [CPR_PARS_IDIX] == csp1_p_mselect_found
	|| pid [CPR_PARS_IDIX] == csp1_p_reuse_mselect_found
	|| pid [CPR_PARS_IDIX] == csp1_p_for_upd_mselect_found
	|| pid [CPR_PARS_IDIX] == csp1_p_reuse_upd_mselect_found)
      pid [CPR_PARS_IDIX] -= csp1_p_mass_command;
  }
  M90TRACE (M90_TR_WORD, "kapaindex ", &ka->kapaindex);
  if (ka->kapaindex > 0) {
    tsp1_packet *pck = (tsp1_packet*) gae->gareqptr;
    sqlparpointer   pa0 = sqlxa->sqlpap + ka->kapaindex - 1;
    sqlparpointer   pa = pa0;
    sqlparpointer   pamn = pa0;
    sqlparpointer   pamx = pa0 + ka->kapacount - 1;
    tsp00_Int4        lpind = lp->lpindi;
    tsp00_Int4        lpcnt = lp->lpcnt;
    tsp00_Int4        lppos = 0;
    tsp00_Int4        rowno = 0;
    char            repfirst = 1;
    char            trfirst = 0;
    sqltatype      *tap = sqlca->sqlrap->rasqltap;
    tsp00_Lname      vapar;
    tsp00_Lname      va3name;
    tsp00_Lname      *vaname = (tap->tatrout == CPR_TRACE_MODLONG) ?
      &va3name : &vapar;
    tsp1_part *datapart;
    tsp00_Uint1 *data;
    tsp1_part *cntpart;
    tsp00_Uint1 *cnt;
    tpr00_ParseId   parsid;
    tsp00_Int4        pno, pnofirst;
    tsp00_Int4        ival;
    char            err;

    M90TRACE (M90_TR_PTR, "pa0", &pa0);
    M90TRACE (M90_TR_PTR, "pamx", &pamx);
    memcpy (vapar, "PARAMETER                       ", sizeof (tsp00_Lname));
    s26new_part_init (pck, seg, &cntpart);
    cntpart->sp1p_part_header.sp1p_part_kind = sp1pk_resultcount;
    sqlca->sqlrap->rasegmpartptr [sp1pk_resultcount] = cntpart;
    cntpart->sp1p_part_header.sp1p_buf_len = MXPR_NUMB + 1;
    cnt = cntpart->sp1p_buf;
    *cnt = CPR_UNDEF_BYTE;
    s26finish_part (pck, cntpart);
    s26new_part_init (pck, seg, &datapart);
    sqlca->sqlrap->rasegmpartptr [sp1pk_data] = datapart;
    data = datapart->sp1p_buf;
    p04mode (sqlca, gae);
    if (ka->kasqlrow.oreclen == 0)
      lp->lprecli = ka->kasqlrow.ireclen;
    if (lp->lpfirsti == cpr_is_true) {
      datapart->sp1p_part_header.sp1p_attributes[0] |= 1 << sp1pa_first_packet;
      lp->lpfirsti = cpr_is_false;
      if (pa->lo.pakind == SQLPARLO) {
	if (pa->lo.pava1index == 0)
	  lpcnt = pa->lo.paloopcnt;
	else {
	  sqlv1pointer    va1 = sqlxa->sqlv1p + pa->lo.pava1index - 1;
	  sqlv2pointer    va2 = sqlxa->sqlv2p + va1->sc.va1indva2 - 1;

	  /* SQLSTRUCT fehlt noch */
	  tsp00_Int2        sint;
	  tsp00_Int4        lint;

	  switch (va2->va2typ) {
	  case CPR_VINT2:
	  case CPR_VUNS2:
	    memcpy (&sint, va1->sc.va1addr, sizeof (sint));
	    lpcnt = sint;
	    break;
	  case CPR_VINT4:
	  case CPR_VUNS4:
	    memcpy (&lint, va1->sc.va1addr, sizeof (lint));
	    lpcnt = lint;
	    break;
	  default:
	    p08runtimeerror (sqlca, sqlxa, cpr_unknown_datatype);
	    if (lp->lperri == cpr_p_ok)
	      lp->lperri = cpr_unknown_datatype;
	  }
	}
	if (lpcnt > pa->lo.paloopmin)
	  lpcnt = pa->lo.paloopmin;
	if (lpcnt < 1) {
	  p08runtimeerror (sqlca, sqlxa,
			   cpr_r_loop_init_wrong);
	  lp->lpnext = cpr_is_false;
	}
	else {
	  lp->lpcnt = lpcnt;
	  pamn++;
	  *cnt = CPR_UNDEF_BYTE;
	}
      }
    }
    else {
      datapart->sp1p_part_header.sp1p_attributes[0] |= 1 << sp1pa_next_packet;
      pamn++;
      if (lp->lperrd3 == 0)
	*cnt = CPR_UNDEF_BYTE;
      else {
	char res;

	*cnt = CPR_DEFINED_BYTE;
	/*selct into */
	if (lp->lprescount != 0)
	  s41p4int (cnt, 2, (int) lp->lprescount, &res);
	else
	  s41p4int (cnt, 2, (int) lp->lperrd3, &res);
      }
    }
    pnofirst = lp->lpindi;
    pr04LongInitLD (sqlca, sqlxa);
    if (lpcnt == 0 && pid [CPR_PARS_IDIX] == csp1_p_mass_command) {
	    pid [CPR_PARS_IDIX] = csp1_p_none;
    }
    else
      switch (pid [CPR_PARS_IDIX]) {
      case csp1_p_none:
	if (lpcnt == 1 || lpcnt == 0)
	  lpcnt = lp->lpcnt = 0;
	else
	  p08runtimeerror (sqlca, sqlxa,
			   cpr_r_loop_init_wrong);
	break;
      case csp1_p_mselect_found:
      case csp1_p_reuse_mselect_found:
      case csp1_p_for_upd_mselect_found:
      case csp1_p_reuse_upd_mselect_found:
	if (lpcnt == 1 || lpcnt == 0 ) {
	  lpcnt = lp->lpcnt = 0;
	  pid [CPR_PARS_IDIX] -=
	    csp1_p_mass_command;
	}
	break;
      case csp1_p_mass_command:
	break;
      case csp1_p_mfetch_found:
        if (lpcnt == 0)
          lpcnt = lp->lpcnt = 1;
	break;
      default:
	if (lpcnt == 1 || lpcnt == 0)
	  lpcnt = lp->lpcnt = 0;
	else
	  p08runtimeerror (sqlca, sqlxa,
			   cpr_r_loop_init_wrong);
      }
    memcpy (parsid, pid, sizeof (parsid));

    if (sqlca->sqlcode == 0)
      do {
        struct SQLCOL  *col;
	trfirst = (char) (pamn > pa0);
	pno = 0;
        col = ore->orcolptr;
	for (pa = pamn; pa <= pamx; pa++) {
	  sqlv1pointer va1mn, va1mx, va1, iva1mn, iva1;
	  tsp00_Int2        elcnt;
	  char           *base=NULL;
	  char           *ibase=NULL;

	  M90TRACE (M90_TR_PTR, "pa", &pa);
	  M90TRACE (M90_TR_WORD, "pakind", &pa->st.pakind);
	  if (pa->st.pakind == SQLPARST) {
	    va1 = sqlxa->sqlv1p + pa->st.pavarst - 1;
	    elcnt = pa->st.paelcnt;
	    base = p04addr (sqlxa, va1, lpind);
	    if (pa->st.paindst > 0) {
	      iva1 = sqlxa->sqlv1p + pa->st.paindst - 1;
	      ibase = p04addr (sqlxa, iva1, lpind);
	    }
	    pa++;
	  }
	  if (pa->el.pakind != SQLPAREL) {
	    M90TRACE (M90_TR_WORD, "pakind", &pa->st.pakind);
	    continue;
	  }
	  va1 = sqlxa->sqlv1p + pa->el.pavarno - 1;
	  if (va1->cm.va1indi == SQLVACM) {
	    va1mn = va1;
	    va1mx = va1 + elcnt - 1;
	  }
	  else {
	    base = p04addr (sqlxa, va1, lpind);
	    if (va1->pt.va1indi == SQLVAPT)
	      va1 = sqlxa->sqlv1p + va1->pt.va1ix - 1;
	    if (va1->sc.va1indi== SQLVAST) {
	      va1mn = sqlxa->sqlv1p + va1->st.va1ix - 1;
	      va1mx = va1mn + va1->st.va1cmpcnt - 1;
	    }
	    else
	      va1mn = va1mx = va1;
	  }
	  if (pa->el.paindno > 0) {
	    iva1 = sqlxa->sqlv1p + pa->el.paindno - 1;
	    if (iva1->cm.va1indi == SQLVACM)
	      iva1mn = iva1;
	    else {
	      ibase = p04addr (sqlxa, iva1, lpind);
	      if (iva1->pt.va1indi==SQLVAPT)
		iva1 = sqlxa->sqlv1p + iva1->pt.va1ix - 1;
	      if (iva1->sc.va1indi==SQLVAST)
		iva1mn = sqlxa->sqlv1p + va1->st.va1ix - 1;
	      else
		iva1mn = iva1;
	    }
	  }
	  else
	    iva1mn = 0;
	  for (va1 = va1mn, iva1 = iva1mn; va1 <= va1mx; va1++, (iva1) ? iva1++ : 0) {

	    void *addr = (va1->cm.va1indi == SQLVACM && base)
	      ? base + va1->cm.va1offs : base;
	    sqlv2pointer va2 = sqlxa->sqlv2p + va1->sc.va1indva2 - 1;

	    /* va3name = (char*)((va1->sc.va1indva3 > 0) ? sqlxa->sqlv3p[va1->sc.va1indva3 - 1].va3name : 0); */
	    if (va1->sc.va1indva3 > 0)
	      memcpy (va3name, sqlxa->sqlv3p[va1->sc.va1indva3 - 1].va3name,
		      sizeof (sqlxa->sqlv3p[va1->sc.va1indva3 - 1].va3name));
	    else
	      memcpy (va3name, "\0", 1);

	    pno++;
	    if (col->colio == CPR_INFO_OUTPUT) {
              col++;
	      continue;
            }

	    if (trfirst) {
	      trfirst = 0;
	      if (repfirst) {
		repfirst = 0;
                pr04LongTraceInit (sqlca, CPR_TR_ARRCNT, vaname, (int)lp->lpcnt);
	      }
	      rowno =lpind + lp->lpindi - pnofirst + 1;
	      p04trloop (sqlca->sqlrap, (int) (rowno - 1));
	    }
	    err = cpr_p_ok;
	    p04colfromindi (sqlca, sqlxa, col, iva1, ibase, &ival, &err);
	    p04coltobuf (sqlca->sqlrap, (char *)data, &datapart->sp1p_part_header.
	    sp1p_buf_len, col, (char *)*vaname, addr,
			va2->va2typ, va2->va2size, va2->va2digit, va2->va2frac,
			ival, lppos, (int) pno, sqlca->sqlemp);
      err = (char) sqlca->sqlemp->eprerr;
	    p03csqlemptosqlca(sqlca,sqlca->sqlemp);
	    M90TRACE (M90_TR_SDWORD, "sqlcode   ", &sqlca->sqlcode);
	    if (sqlca->sqlrap->rasqlldp->ldcolkind == cpr_is_true)
	      {
		struct SQLLD *ldp = sqlca->sqlrap->rasqlldp;
		tsp00_Int2 *ldmaxi = &ldp->ldmaxi;
		struct SQLPCLD *ldpc = ldp->ldpc;
		struct SQLPCLD *p = ldpc + *ldmaxi - 1;

		p->ldrowno = (lp->lpcnt == 0) ? (tsp00_Int2) 0 : (tsp00_Int2) (rowno/* + 1 */);
		if (pa->el.paindno==0) {
		  p->ldindkaddr = 0;
		  p->ldcoladdr = 0;
		  p->ldindktype = p->ldindklen = p->ldindkfrac = 0;
		}
	      }
	    if (lp->lperri == cpr_p_ok)
	      lp->lperri = err;
            col++;
	  } /* for */
	} /* for */
	if (lp->lpcnt != 0) {
	  char res;

	  if (sqlca->sqlcode == 0) {
	    if (lp->lprecli == 0)
	      lp->lprecli = datapart->sp1p_part_header.sp1p_buf_len;
	    lp->lpnext = cpr_is_true;
	    lpind++;
	  }
	  else {
	    datapart->sp1p_part_header.sp1p_buf_len = lppos;
	    lp->lpnext = cpr_is_false;
	  }
	  lpcnt = lpind - pnofirst;
	  if ((lpind >= lp->lpcnt) && (lp->lpnext == cpr_is_true)) {
	    datapart->sp1p_part_header.sp1p_attributes[0] |= 1 << sp1pa_last_packet;
	  }
	  datapart->sp1p_part_header.sp1p_arg_count = (tsp00_Int2) lpcnt;
	  if (pid [CPR_PARS_IDIX] != csp1_p_mselect_found &&
	      pid [CPR_PARS_IDIX] != csp1_p_reuse_mselect_found &&
	      pid [CPR_PARS_IDIX] != csp1_p_mass_command &&
	      pid [CPR_PARS_IDIX] != csp1_p_for_upd_mselect_found &&
	      pid [CPR_PARS_IDIX] != csp1_p_reuse_upd_mselect_found) {
	    *cnt = CPR_DEFINED_BYTE;
	    s41p4int (cnt, 2, lpcnt, &res);
	  }
	} else if (lp->lpcnt == 0) {
	    datapart->sp1p_part_header.sp1p_attributes[0] |= 1 << sp1pa_last_packet;
	}
	pr04LongILvcOneRecord (sqlca, sqlxa, gae, &parsid, datapart, cntpart);
	if ((sqlca->sqlemp->ereturncode != 0) && (sqlca->sqlemp->ereturncode != 100))
	  return;
	/* PTS 1102362 */
	/* wenn Longspalten an den Kerngeschickt wurden konnen sich
	   die Partadressen veraendert haben */
	cnt = (cntpart) ? cntpart->sp1p_buf : NULL;
	data = (datapart) ? datapart->sp1p_buf : NULL;
	if (!cnt || !data)
	  p08runtimeerror(sqlca, sqlxa, cpr_memory_allocation_faild);
	p03csqlemptosqlca (sqlca, sqlca->sqlemp);
	if (lp->lpcnt != 0) {
	  lppos = datapart->sp1p_part_header.sp1p_buf_len;
	  pr04LongInitLD (sqlca, sqlxa);
	  if (sqlca->sqlrap->rasqlldp->ldallsend && sqlca->sqlcode == 0) {
	    tsp00_Int2 sint = (tsp00_Int2) sqlca->sqlerrd[2];

	    lppos = 0;
	    datapart->sp1p_part_header.sp1p_buf_len = lppos;
	    if (sint == 0)
	      *cnt = CPR_UNDEF_BYTE;
	    else {
	      char res;

	      datapart->sp1p_part_header.sp1p_arg_count = (tsp00_Int2) lpcnt;
	      *cnt = CPR_DEFINED_BYTE;
	      s41p4int (cnt, 2, sint, &res);
	    }
	  }
	}
      }
      while (sqlca->sqlcode == 0
             && datapart->sp1p_part_header.sp1p_buf_len + lp->lprecli
	     + (lpind+1-pnofirst) * ka->kasqlrow.oreclen
	     <= datapart->sp1p_part_header.sp1p_buf_size
	     && lp->lpnext == cpr_is_true
	     && lpind < lp->lpcnt);
    lp->lpindi = lpind;
    lp->lperrd3i = sqlca->sqlerrd[2];

    if (sqlca->sqlcode == 100)
      lpcnt = 0;

    if ((lpcnt != 0)
	&& ((sqlca->sqlemp->ereturncode == 0) ||(sqlca->sqlemp->ereturncode == 100))) {
      /*select into */
      if (lp->lprescount != 0)
	sqlca->sqlerrd[2] = lp->lprescount;
      else
	sqlca->sqlerrd[2] = lpcnt;
    }
    if (lpind >= lp->lpcnt)
      lp->lpnext = cpr_is_false;
    else if (lp->lperri != cpr_p_ok)
      if (lp->lpcnt != 0 && lpcnt != 0) {
	sqlca->sqlcode = 0;
	sqlca->sqlemp->ereturncode = 0;
	sqlca->sqlemp->eprerr   = cpr_p_ok;
      }
    if (!sqlca->sqlrap->rasqlldp->ldallsend
	&& datapart->sp1p_part_header.sp1p_buf_len > 0)
      s26finish_part (pck, datapart);
    else {
      seg->sp1s_segm_header.variant.C_1.sp1s_no_of_parts_F--;
      seg->sp1s_segm_header.variant.C_1.sp1s_segm_len_F -=
	sizeof (tsp1_part_header);
    }
  }
  M90TRACE (M90_TR_EXIT, "p04input ", 0);
}     /*--------------------------------------------*/

tsp00_Int4
p04rescount (tsp1_segment *seg, tsp00_Int4 datalen, int *countlen)
{
  tsp00_Int4        count = 0;
  char            res;
  tsp1_part *cntpart;

  *countlen = 0;
  s26find_part (seg, sp1pk_resultcount, &cntpart);
  if (datalen > 0) {
    count = 1;
    if (cntpart) {
      tsp00_Uint1 *buf = cntpart->sp1p_buf + 1;
      *countlen = MXPR_NUMB + 1;
      if (memcmp (buf, CPR_NUMB1, MXPR_NUMB)) {
	s40g4int (buf, 1, &count, &res);
	if (res != CPR_NUM_OK)
	  count = -1;
      }
    }
  }
  return count;
}     /*--------------------------------------------*/

void p04output (sqlcatype * sqlca, sqlxatype * sqlxa, sqlgaentry *gae, struct tpr_sqlloop *lp)
{
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  sqlkapointer    ka = sqlxa->xaSQLDesc->ka;
  sqlorentry *ore = sqlxa->xaSQLDesc->ore;
  struct SQLERROR *em = sqlca->sqlemp;
  char            pid = ka->kaParseInfo.ParseId [CPR_PARS_IDIX];
  int             cntlen;
  tsp00_Int4       temppos = 0;
  /* ADIS 1106269   */
  tsp00_Int4       *lppos = &temppos;
  tsp00_Int4       *lpRecpos = &temppos;
  tsp00_Int4        errd3;
  tsp1_part *datapart;
  tsp00_Uint1 *data;

  if (sqlca->sqlcode != 0)
    return;

  M90TRACE (M90_TR_ENTRY, "p04output ", 0);
  if (lp->lpfirsto == cpr_is_true)
    lp->lpmax = (lp->lpcnt == 0) ? 1 : lp->lpcnt;
  errd3 = sqlca->sqlerrd[2];
  /**select into ****/
  if (seg->sp1s_segm_header.variant.C_2.sp1r_function_code_F ==
      csp1_mselect_into_fc)
    lp->lprescount = (tsp00_Int2) sqlca->sqlerrd[2];
  else lp->lprescount = 0;
  M90TRACE (M90_TR_SDWORD, "errd3   ", &errd3);
  p03find_part (sqlca->sqlrap, sp1pk_data, &datapart);
  if (datapart) {
    data = datapart->sp1p_buf;
    sqlca->sqlerrd[2] = p04rescount (seg, datapart->sp1p_part_header.sp1p_buf_len, &cntlen);
    M90TRACE (M90_TR_SDWORD, "sqlerrd[2]", &sqlca->sqlerrd[2]);
    p04mode (sqlca, gae);

    if (ore->orsqlrow.oreclen > 0 && ka->kapacount > 0 &&
	datapart->sp1p_part_header.sp1p_buf_len > 0) {
      sqlparpointer   pa = sqlxa->sqlpap + ka->kapaindex - 1;
      sqltatype      *tap = sqlca->sqlrap->rasqltap;
      sqlparpointer   pa0 = pa;
      sqlparpointer   pamn = pa;
      sqlparpointer   pamx = pa + ka->kapacount - 1;
      tsp00_Lname       vapar;
      tsp00_Lname       va3name;
      tsp00_Lname      *vaname = (tap->tatrout == CPR_TRACE_MODLONG) ?
	&va3name : &vapar;
      tsp00_Int4        lpind = 0;
      tsp00_Int4        lpcnt = 1;
      tsp00_Int4        rowno = 0;
      char            trfirst = 0;
      tsp00_Int4        pnofirst = lp->lpindo;
      int             pno;
      tsp00_Int4        ival;
      char            err, rownotfound;
      /* ADIS 1000696 ==> */
      tsp00_Int2 kafaindex;
      tsp00_Int2 famfindex;
      tsp00_Int4 *lastrecord = NULL;
      /* <== ADIS 1000696 */

      memcpy (vapar, "PARAMETER                       ", sizeof (tsp00_Lname));
      if (cntlen == MXPR_NUMB + 1)
	errd3 = sqlca->sqlerrd[2];
      if (pa->lo.pakind == SQLPARLO) {
	lpcnt = datapart->sp1p_part_header.sp1p_arg_count;
	pamn++;
      }
      M90TRACE (M90_TR_SDWORD, "lpcnt   ", &lpcnt);
      /* ADIS 1000696 ==> */
      kafaindex = ka->kafaindex;
      if (kafaindex > 0)
	famfindex = ka->kafae->famfindex;
      else
	famfindex = 0;
      if (famfindex > 0) {
	sqlmfentry *mf = &sqlca->sqlmfp->mfentry[famfindex-1];
	/* ADIS 1106269   */
	lppos = &mf->mfActualPos;
	lastrecord = &mf->mfAllRecordsRead;
	lpRecpos = &mf->mfRecpos;
	if (*lppos >= datapart->sp1p_part_header.sp1p_buf_len)
	  *lppos = 0;
      }
      /* <== ADIS 1000696 */
      if (lpcnt > 0) {
	lp->lpreclo = (kafaindex > 0) ? ka->kafae->fareclen : datapart->sp1p_part_header.sp1p_buf_len/lpcnt;
      }
      while (sqlca->sqlcode == 0 && lpind < lpcnt) {
        struct SQLCOL  *col;
	int outpfound = 0;
	trfirst = (char) (pamn > pa0);
	if (pamn > pa0)	{
	  if (lp->lpfirsto == cpr_is_true) {
	    lp->lpfirsto = cpr_is_false;
	    if (lp->lpmax > 1)
	      pr04LongTraceInit (sqlca, CPR_TR_ARRCNT, vaname, (int) lp->lpmax);
	  }
	}
	pno = 0;
        col = ore->orcolptr;
	for (pa = pamn; pa <= pamx; pa++) {
	  sqlv1pointer va1mn, va1mx, va1, iva1mn, iva1;
	  tsp00_Int2 elcnt;
	  char *base, *ibase;

	  rowno = lpind + pnofirst;
	  M90TRACE (M90_TR_SDWORD, "pnofirst     ", &pnofirst);
	  M90TRACE (M90_TR_SDWORD, "lpind        ", &lpind   );
	  M90TRACE (M90_TR_SDWORD, "rowno        ", &rowno   );
	  if (pa->st.pakind == SQLPARST) {
	    va1 = sqlxa->sqlv1p + pa->st.pavarst - 1;
	    elcnt = pa->st.paelcnt;
	    base = p04addr (sqlxa, va1, rowno);
	    if (pa->st.paindst > 0) {
	      iva1 = sqlxa->sqlv1p + pa->st.paindst - 1;
	      ibase = p04addr (sqlxa, iva1, rowno);
	    }
	    pa++;
	  }
	  if (pa->el.pakind != SQLPAREL) {
            col++;
	    continue;
          }
	  va1 = sqlxa->sqlv1p + pa->el.pavarno - 1;
	  if (va1->cm.va1indi == SQLVACM) {
	    va1mn = va1;
	    va1mx = va1 + elcnt - 1;
	  }
	  else {
	    base = p04addr (sqlxa, va1, rowno);
	    if (va1->pt.va1indi == SQLVAPT)
	      va1 = sqlxa->sqlv1p + va1->pt.va1ix - 1;
	    if (va1->sc.va1indi == SQLVAST) {
	      va1mn = sqlxa->sqlv1p + va1->st.va1ix - 1;
	      va1mx = va1mn + va1->st.va1cmpcnt - 1;
	    }
	    else
	      va1mn = va1mx = va1;
	  }
	  if (pa->el.paindno > 0) {
	    iva1 = sqlxa->sqlv1p + pa->el.paindno - 1;
	    if (iva1->cm.va1indi == SQLVACM)
	      iva1mn = iva1;
	    else {
	      ibase = p04addr (sqlxa, iva1, rowno);
	      if (iva1->pt.va1indi==SQLVAPT)
		iva1 = sqlxa->sqlv1p + iva1->pt.va1ix - 1;
	      if (iva1->sc.va1indi==SQLVAST)
		iva1mn = sqlxa->sqlv1p + iva1->st.va1ix	- 1;
	      else
		iva1mn = iva1;
	    }
	  }
	  else
	    iva1mn = 0;
	  for (va1 = va1mn, iva1 = iva1mn; va1 <= va1mx;
	       va1++, (iva1) ? iva1++ : 0) {
	    void *addr = (va1->cm.va1indi == SQLVACM) ?
	      base + va1->cm.va1offs : base;
	    sqlv2pointer va2 = sqlxa->sqlv2p + va1->sc.va1indva2 - 1;
	    /* va3name = (char*)((va1->sc.va1indva3 > 0) ?	sqlxa->sqlv3p[va1->sc.va1indva3 - 1].va3name : 0); */
	    if (va1->sc.va1indva3 > 0)
	      memcpy (va3name, sqlxa->sqlv3p[va1->sc.va1indva3 - 1].va3name,
		      sizeof (sqlxa->sqlv3p[va1->sc.va1indva3 - 1].va3name));
	    else
	      memcpy (va3name, "\0", 1);
	    pno++;
	    if (col->colio == CPR_INFO_INPUT) {
              col++;
	      continue;
            }

	    outpfound = 1;
	    if (trfirst) {
	      trfirst = 0;
	      p04trloop (sqlca->sqlrap, (int) rowno);
	    }
	    err = cpr_p_ok;
	    if (va2->va2const > 0) {
	      err = cpr_rt_no_output_variable;
	      p04err (sqlca->sqlrap, sqlca->sqlemp, err);
	    }
	    else {
	      p04colfrombuf(sqlca->sqlrap, (char *)data,
			    &datapart->sp1p_part_header.sp1p_buf_len,
			    col, (char *) *vaname, addr, va2->va2typ, va2->va2size,
			    va2->va2digit, va2->va2frac, &ival, *lppos,
			    pno, sqlca->sqlemp);
	      err = sqlca->sqlemp->eprerr;
	      p03csqlemptosqlca (sqlca, sqlca->sqlemp);
	      if (sqlca->sqlrap->rasqlldp->ldcolkind ==	cpr_is_true) {
		struct SQLLD *ldp = sqlca->sqlrap->rasqlldp;
		tsp00_Int2 *ldmaxo = &ldp->ldmaxo;
		struct SQLPCLD *ldpc = ldp->ldpc;
		struct SQLPCLD *p = ldpc + *ldmaxo -1;
		p->ldrowno = (lp->lpcnt == 0) ? (tsp00_Int2) 0 :
		  (tsp00_Int2) (rowno + 1);
		if (iva1) {
		  sqlv2pointer iva2 = sqlxa->sqlv2p + iva1->sc.va1indva2 -1;
		  p->ldindkaddr = (iva1->cm.va1indi == SQLVACM) ?
		    ibase + iva1->cm.va1offs : ibase;
		  p->ldcoladdr = col;
		  p->ldindktype= iva2->va2typ;
		  p->ldindklen = (tsp00_Int2)iva2->va2size;
		  p->ldindkfrac= iva2->va2frac;
		}
		else {
		  p->ldindkaddr = 0;
		  p->ldcoladdr  = 0;
		  p->ldindktype = p->ldindklen = p->ldindkfrac = 0;
		}
	      }
	      if (iva1) {
		p04coltoindi (sqlca, sqlxa, col, iva1, ibase, ival, &err);
	      }
	      else
		if (ival == CPR_NULL_DATA)	{
		  p04err (sqlca->sqlrap, sqlca->sqlemp, cpr_out_null_not_allowed);
		  if (lp->lperro == cpr_p_ok)
		    lp->lperro = cpr_out_null_not_allowed;
		}
	    }
	    if (lp->lperro == cpr_p_ok)
	      lp->lperro = err;
            col++;
	  } /* for */
	} /* for */
	if (outpfound) {
	  p03csqlemptosqlca (sqlca, sqlca->sqlemp);
	  if (lppos)
	    *lppos += lp->lpreclo;
	  lpind++;
	}
	else
	  return;
      }
      rownotfound = 0;
      M90TRACE (M90_TR_SDWORD, "sqlerrd[2]  1", &sqlca->sqlerrd[2]);
      M90TRACE (M90_TR_SDWORD, "errd3        ", &errd3);
      M90TRACE (M90_TR_SDWORD, "lp->lperrd3  ", &lp->lperrd3);
      M90TRACE (M90_TR_SDWORD, "lpind        ", &lpind);

      if (errd3 == lp->lperrd3 &&
	  (dbmode == CPR_KIND_ORACLE || dbmode==CPR_KIND_SAPR3)) {
	rownotfound = 1;
	lp->lpnext = cpr_is_false;
      }
      else {
	sqlca->sqlerrd[2] = (sqlca->sqlcode == 0) ? lpind : lpind - 1;
	lp->lperrd3 += (tsp00_Int2) sqlca->sqlerrd[2];
	lp->lpindo = (tsp00_Int2) (sqlca->sqlerrd[2] + pnofirst);
	errd3 = lp->lperrd3 % lp->lpmax;
	M90TRACE (M90_TR_SDWORD, "errd3  333   ", &errd3);
	M90TRACE (M90_TR_SDWORD, "lp->lperrd3  ", &lp->lperrd3);
	M90TRACE (M90_TR_SDWORD, "lp->lpmax    ", &lp->lpmax);
	if  (datapart->sp1p_part_header.sp1p_attributes[0] & 1 << sp1pa_last_packet) {
	  lp->lpnext = cpr_is_false;
	  /* ADIS 1000696 ==> */
	  if (lastrecord)
	    *lastrecord = cpr_is_true;
	  /* <== ADIS 1000696 */
	}
	else if (errd3 == 0) {
	  lp->lpnext = cpr_is_false;
	  M90TRACE (M90_TR_SDWORD, "lp->lpnext 2 ", &lp->lpnext);
	}
	else if (lp->lpcnt >= errd3) {
	  if ( (seg->sp1s_segm_header.variant.C_2.sp1r_function_code_F != csp1_mselect_into_fc)
	       && ((seg->sp1s_segm_header.variant.C_2.sp1r_function_code_F < csp1_mfetch_first_fc)
		   || (seg->sp1s_segm_header.variant.C_2.sp1r_function_code_F > csp1_mfetch_same_fc)) ) {
	    lp->lpfirsti = cpr_is_true;
	    lp->lpnext = cpr_is_true;
	    lp->lpindi = 0;
	  }
	}
	else if (lp->lpcnt != errd3) {
	  lp->lpcnt -= (tsp00_Int2) errd3;
	  lp->lpnext = cpr_is_true;
	  lp->lpindi = 0;
	}
	else
	  lp->lpnext = cpr_is_false;
      }
      if (lpind == 0 || (rownotfound && sqlca->sqlcode == 0)) {
	static const char s[] = "ROW NOT FOUND";
	static const tsp00_Int2 ls = sizeof (s) - 1;

	sqlca->sqlerrd[2] = (dbmode == CPR_KIND_ORACLE || dbmode == CPR_KIND_SAPR3) ? lp->lperrd3 : 0;
	em->ereturncode = 100;
	em->eerrorpos = 0;
	memcpy (em->etext, s, ls);
	em->etextlen = ls;
	p03returncodeget (sqlca, sqlxa);
      }
    }
    else {
      if (errd3 != 0) {
	sqlca->sqlerrd[2] = errd3;
	if  (datapart->sp1p_part_header.sp1p_attributes[0] & 1 << sp1pa_last_packet) {
	  static const char s[] = "ROW NOT FOUND";
	  static const tsp00_Int2 ls = sizeof (s) - 1;

	  sqlca->sqlerrd[2] = (dbmode == CPR_KIND_ORACLE || dbmode == CPR_KIND_SAPR3) ? lp->lperrd3 : 0;
	  em->ereturncode = 100;
	  em->eerrorpos = 0;
	  memcpy (em->etext, s, ls);
	  em->etextlen = ls;
	  p03returncodeget (sqlca, sqlxa);
	}
      }
    }
  }   /* datapart == null */

  M90TRACE (M90_TR_SDWORD, "sqlerrd[2]  6", &sqlca->sqlerrd[2]);
  M90TRACE (M90_TR_SDWORD, "errd3        ", &errd3);
  M90TRACE (M90_TR_SDWORD, "lp->lperrd3  ", &lp->lperrd3);
  M90TRACE (M90_TR_SDWORD, "lp->lpcnt    ", &lp->lpcnt);
  M90TRACE (M90_TR_EXIT, "p04output ", 0);
}     /*--------------------------------------------*/


void p04sqldin (sqlcatype * sqlca, sqlxatype * sqlxa, sqlgaentry * gae,
void *sqlda, struct tpr_sqlloop *lp)
{
  tsp1_packet *pck = (tsp1_packet*) gae->gareqptr;
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  sqldatype      *da = (sqldatype *) sqlda;
  sqlvartype     *var, *var0 = da->sqlvar;
  sqlvartype     *varmx = var0 + da->sqln - 1;
  tsp00_Int4        lppos = 0;
  tsp00_Int4        lpcnt = lp->lpcnt;
  tsp00_Int4        lpind = lp->lpindi;
  tsp00_Int4        rowno = 0;
  char            trfirst = 0;
  tsp00_Lname       blname;
  tsp00_KnlIdentifier name;
  tpr00_ParseId   parsid;
  tsp00_Int4        pnofirst;
  int             i;
  char            err;
  tsp1_part *pidpart;
  tsp00_Uint1 *pid;
  tsp1_part *datapart;
  tsp00_Uint1 *data;
  tsp1_part *cntpart;
  tsp00_Uint1 *cnt;

  M90TRACE (M90_TR_ENTRY, "p04sqldin       ", 0);
  memcpy (blname, "                                ", sizeof (tsp00_Lname));
  p03find_part (sqlca->sqlrap, sp1pk_parsid, &pidpart);
  pid = &pidpart->sp1p_buf[CPR_PARS_IDIX];
  s26new_part_init (pck, seg, &cntpart);
  cntpart->sp1p_part_header.sp1p_part_kind = sp1pk_resultcount;
  cntpart->sp1p_part_header.sp1p_buf_len = MXPR_NUMB + 1;
  sqlca->sqlrap->rasegmpartptr [sp1pk_resultcount] = cntpart;
  cnt = cntpart->sp1p_buf;
  *cnt = CPR_UNDEF_BYTE;
  s26finish_part (pck, cntpart);
  s26new_part_init (pck, seg, &datapart);
  sqlca->sqlrap->rasegmpartptr [sp1pk_data] = datapart;
  data = datapart->sp1p_buf;
  if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
    sqlkapointer    ka = sqlxa->xaSQLDesc->ka;

    da->sqloffset = 0;
    da->sqlloop = (ka->kapaindex == 0) ? 0 : (ka->kapaindex < 0) ?
      2 : (p08runtimeerror (sqlca, sqlxa, cpr_not_implemented),0);
  }
  p04mode (sqlca, gae);
  if (lp->lpfirsti == cpr_is_true) {
    lp->lpfirsti = cpr_is_false;
    if (da->sqlloop != 0) {
      if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
	da->sqloffset = lp->lpindi;
	da->sqlloop = pr04GetLoopCnt(sqlca, sqlxa, lp);
      }
      else
	lp->lpindi = da->sqloffset;
    }
    lpcnt = da->sqlloop + da->sqloffset;
    lpind = da->sqloffset;
    if (da->sqlloop != 0) {
      if (lpcnt < 1) {
	p08runtimeerror (sqlca, sqlxa, cpr_r_loop_init_wrong);
	lp->lpnext = cpr_is_false;
      }
      else {
	trfirst = 1;
	lp->lpcnt = lpcnt;
	*cnt = CPR_UNDEF_BYTE;
      }
    }
  }
  else {
    trfirst = 1;
    if (lp->lperrd3 == 0)
      *cnt = CPR_UNDEF_BYTE;
    else {
      char res;

      *cnt = CPR_DEFINED_BYTE;
      s41p4int (cnt, 2, (int) lp->lperrd3, &res);
    }
  }
  pnofirst = lp->lpindi;
  if (lpcnt == 0) {
    if (*pid == csp1_p_mass_command ||
	*pid == csp1_p_mselect_found ||
	*pid == csp1_p_reuse_mselect_found ||
	*pid == csp1_p_for_upd_mselect_found ||
	*pid == csp1_p_reuse_upd_mselect_found)
      *pid -= csp1_p_mass_command;

    else {
      if (sqlca->sqlrap->ralang != CPR_LA_CALL)
        if (*pid == csp1_p_mfetch_found)
          lpcnt = 1;
    }
    if (sqlca->sqlrap->ralang == CPR_LA_CALL) {
      if ((*pid == csp1_p_mfetch_found)
	  && (sqlxa->xakano != cpr_com_mfetch)
	  && (sqlxa->xakano != cpr_com_mfetch_describe)) {
	*pid = csp1_p_fetch_found;
      }
    }
  }
  else {
    switch (*pid) {
    case csp1_p_none: {
      if (lpcnt == 1)
	lpcnt = lp->lpcnt = 0;
      else
	p08runtimeerror (sqlca, sqlxa, cpr_r_loop_init_wrong);
      break;
    }
    case csp1_p_mselect_found: {}
    case csp1_p_for_upd_mselect_found: {}
    case csp1_p_reuse_mselect_found: {}
    case csp1_p_reuse_upd_mselect_found: {
      if (lpcnt == 1) {
	lpcnt = lp->lpcnt = 0;
	*pid -= csp1_p_mass_command;
      }
      break;
    }
    case csp1_p_mass_command: {}
    case csp1_p_mfetch_found: {
      break;
    }
    default: {
      if (lpcnt == 1)
	lpcnt = lp->lpcnt = 0;
      else
	p08runtimeerror (sqlca, sqlxa,
			 cpr_r_loop_init_wrong);
      break;
    }
    }
  }
  memcpy (parsid, pidpart->sp1p_buf, sizeof (parsid));
  if (*pid == csp1_p_mfetch_found) {
    pr04oPutResultCount(cnt, lpcnt);
  }
  else {
  if (sqlca->sqlcode == 0 && da->sqln > 0) {
    do {
      for (i = 1, var = var0; var <= varmx; var++, i++) {
	tsp00_Int4 ival = 0;
	void *varaddr = (char *) var->hostvaraddr + lpind * var->hostcolsize;

	tsp00_Int4 *indaddr = (var->hostindaddr)
	  ? (tsp00_Int4*)(var->hostindaddr + lpind) : &ival;
	if (var->hostindaddr)
	  /* in case of allignment */
	  memcpy (&ival, indaddr, sizeof (tsp00_Int4));
	if (var->col.colio == CPR_INFO_OUTPUT)
	  continue;
	if (trfirst) {
	  trfirst = 0;
	  if (lpind == 0) {
	    pr04LongTraceInit (sqlca, CPR_TR_ARRCNT, &blname,
			       (int) da->sqlloop);
            p04traceda (sqlca->sqlrap, (char*)da, NULL, cpr_kind_internal);
	  }
	  rowno = lpind;
	  p04trloop (sqlca->sqlrap, (int) rowno);
	}
	err = cpr_p_ok;
	memcpy (name, var->colname, sizeof (name));
	p04coltobuf (sqlca->sqlrap, (char*)data, &datapart->sp1p_part_header.
		     sp1p_buf_len, &var->col, (char*)name, varaddr, var->hostvartype,
		     var->collength, (tsp00_Int2) var->collength, var->colfrac,
		     ival, lppos, i, sqlca->sqlemp);
	err = (char) sqlca->sqlemp->eprerr;
	p03csqlemptosqlca (sqlca, sqlca->sqlemp);
	if (sqlca->sqlrap->rasqlldp->ldcolkind ==
	    cpr_is_true) {
	  struct SQLLD *ldp = sqlca->sqlrap->rasqlldp;
	  tsp00_Int2 *ldmaxi = &ldp->ldmaxi;
	  struct SQLPCLD *ldpc = ldp->ldpc;
	  struct SQLPCLD *p = ldpc + *ldmaxi- 1;

	  p->ldrowno = (lp->lpcnt == 0) ? (sqlint2) 0 : (sqlint2) (rowno+ 1);
	  p->ldindkaddr = (indaddr == &ival) ? 0 : (char*)indaddr;
	  p->ldcoladdr = &var->col;
	  p->ldindktype = CPR_VINT4;
	  p->ldindklen = 4;
	  p->ldindkfrac = 0;
	}
	if (lp->lperri == cpr_p_ok)
	  lp->lperri = err;
      }
      if (lp->lpcnt != 0) {
	if (sqlca->sqlcode == 0) {
	  if (lp->lprecli == 0)
	    lp->lprecli = datapart->sp1p_part_header.sp1p_buf_len;
	  lp->lpnext = cpr_is_true;
	  lpind++;
	}
	else {
	  datapart->sp1p_part_header.sp1p_buf_len = lppos;
	  lp->lpnext = cpr_is_false;
	}
	lpcnt = lpind - pnofirst;
	if (lpind >= lp->lpcnt && lp->lpnext == cpr_is_true)
	  datapart->sp1p_part_header.sp1p_attributes[0] = 1 << sp1pa_last_packet;
	datapart->sp1p_part_header.sp1p_arg_count = (tsp00_Int2) lpcnt;
	if (*pid!= csp1_p_mselect_found
	    && *pid != csp1_p_reuse_mselect_found
	    && *pid != csp1_p_mass_command
	    && *pid != csp1_p_for_upd_mselect_found
	    && *pid != csp1_p_reuse_upd_mselect_found) {
	  char res;
	  *cnt = CPR_DEFINED_BYTE;
	  s41p4int (cnt, 2, lpcnt, &res);
	}
      }
      pr04LongILvcOneRecord (sqlca, sqlxa, gae, &parsid, datapart, cntpart);
      if ((sqlca->sqlemp->ereturncode != 0) && (sqlca->sqlemp->ereturncode != 100))
	return;
      cnt = cntpart->sp1p_buf;
      p03csqlemptosqlca (sqlca, sqlca->sqlemp);
      if (da->sqlloop != 0) {
	lppos = datapart->sp1p_part_header.sp1p_buf_len;
	pr04LongInitLD (sqlca, sqlxa);
	if (sqlca->sqlrap->rasqlldp->ldallsend && sqlca->sqlcode == 0) {
	  tsp00_Int2 sint = (tsp00_Int2) sqlca->sqlerrd[2];

	  lppos = 0;
	  datapart->sp1p_part_header.sp1p_buf_len = 0;
	  if (sint == 0)
	    *cnt = CPR_UNDEF_BYTE;
	  else {
	    char res;

	    *cnt = CPR_DEFINED_BYTE;
	    s41p4int (cnt, 2, sint, &res);
	  }
	}
      }
      trfirst = 1;
    }
    while (sqlca->sqlcode == 0
           && datapart->sp1p_part_header.sp1p_buf_len + lp->lprecli
	   <= datapart->sp1p_part_header.sp1p_buf_size
	   && lp->lpnext == cpr_is_true
	   && lpind < lp->lpcnt
	   && lppos != 0);
  }
  }
  lp->lpindi = lpind;
  lp->lperrd3i = sqlca->sqlerrd[2];
  if ((lpcnt != 0)
      && ((sqlca->sqlemp->ereturncode == 0)
	  ||(sqlca->sqlemp->ereturncode == 100))) {
    datapart->sp1p_part_header.sp1p_arg_count = (tsp00_Int2) lpcnt;
    sqlca->sqlerrd[2] = lpcnt;
  }
  if (lpind >= lp->lpcnt)
    lp->lpnext = cpr_is_false;
  else if (lp->lperri != cpr_p_ok)
    if (lp->lpcnt != 0 && lpcnt != 0) {
      sqlca->sqlcode = 0;
      sqlca->sqlemp->ereturncode = 0;
      sqlca->sqlemp->eprerr   = cpr_p_ok;
    }
  if (!sqlca->sqlrap->rasqlldp->ldallsend
      && datapart->sp1p_part_header.sp1p_buf_len > 0)
    s26finish_part (pck, datapart);
  else {
    seg->sp1s_segm_header.variant.C_1.sp1s_no_of_parts_F--;
    seg->sp1s_segm_header.variant.C_1.sp1s_segm_len_F -= sizeof (tsp1_part_header);
  }
  M90TRACE (M90_TR_EXIT, "p04sqldin       ", 0);
}     /*--------------------------------------------*/

void
p04sqldout (sqlcatype * sqlca, sqlxatype * sqlxa, sqlgaentry *gae, void *sqlda,
    struct tpr_sqlloop *lp)
{
  tsp1_packet *pck = (tsp1_packet*) gae->gareqptr;
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  struct SQLERROR *em = sqlca->sqlemp;
  sqldatype      *da = (sqldatype *) sqlda;
  sqlvartype     *var, *var0 = da->sqlvar;
  sqlvartype     *varmx = var0 + da->sqln - 1;
  tsp00_Int2        dbmode = sqlca->sqlrap->rakamode;
  tsp00_Int2       kafaindex;
  tsp00_Int2       famfindex;
  tsp00_Int4       temppos = 0;
  tsp00_Int4       *lppos = &temppos;
  /* ADIS 1001733	*/
  tsp00_Int4       *lpRecpos = &temppos;
  tsp00_Int4        lpcnt = 1;
  tsp00_Int4        lpind = 0;
  tsp00_Int4        rowno = 0;
  char            trfirst = 0;
  tsp00_Lname       blname;
  static const char errs[] = "ROW NOT FOUND";
  static const tsp00_Int2 ls = sizeof (errs) - 1;
  tsp00_KnlIdentifier name;
  tsp00_Int4        pnofirst, pno;
  char            err, rownotfound;
  int             cntlen;
  tsp00_Int4        errd3;
  tsp1_part *datapart;
  tsp00_Uint1 *data;
  tsp00_Int4 *nextlpcnt = NULL;
  tsp00_Int4 *lastrecord = NULL;

  if (da->sqlrow.oreclen == 0 || sqlca->sqlcode != 0)
    return;

  M90TRACE (M90_TR_ENTRY, "p04sqldout  ", 0);
  memcpy (blname, "                                ", sizeof (tsp00_Lname));
  if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
    sqlkapointer    ka = sqlxa->xaSQLDesc->ka;

    if (ka->kapaindex == 0) {
      da->sqloffset = 0;
      da->sqlloop = 0;
    }
    else
      if (ka->kapaindex < 0) {
	da->sqloffset = lp->lpindo;
	da->sqlloop = lp->lpcnt;
      }
      else {
	p04err (sqlca->sqlrap, sqlca->sqlemp, cpr_not_implemented);
      }
    p03csqlemptosqlca (sqlca, sqlca->sqlemp);
  }
  else  {
    if (da->sqlloop == 0)
      da->sqloffset = 0;
  }
  p04mode (sqlca, gae);
  if (lp->lpfirsto == cpr_is_true) {
    lp->lpfirsto = cpr_is_false;
    lp->lpmax = (tsp00_Int2) ((lp->lpcnt == 0) ? 1 : da->sqlloop);
    if (da->sqlloop != 0)
      pr04LongTraceInit (sqlca, CPR_TR_ARRCNT, &blname,
			(int) lp->lpmax);
    p04traceda (sqlca->sqlrap, (char*)da, NULL, cpr_kind_internal);
  }
  errd3 = sqlca->sqlerrd[2];
  p03find_part (sqlca->sqlrap, sp1pk_data, &datapart);
  sqlca->sqlerrd[2] = p04rescount (seg,
				   datapart->sp1p_part_header.sp1p_buf_len, &cntlen);
  if (cntlen == MXPR_NUMB + 1)
    errd3 = sqlca->sqlerrd[2];
  if (sqlca->sqlerrd[2] == 0) {
    em->ereturncode = 100;
    em->eerrorpos = 0;
    memcpy (em->etext, errs, ls);
    em->etextlen = ls;
    p03returncodeget (sqlca, sqlxa);
    M90TRACE (M90_TR_EXIT, "p04sqldout  ", 0);
    return;
  }
  pnofirst = da->sqloffset;
  rowno = lpind + pnofirst;
  lp->lpreclo = da->sqlrow.oreclen;
  data = datapart->sp1p_buf;
  if (da->sqlloop != 0) {
    lpcnt = datapart->sp1p_part_header.sp1p_arg_count;
    *lppos = 0;
    trfirst = 1;
  }
  if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
    kafaindex = sqlxa->xaSQLDesc->ka->kafaindex;
    if (kafaindex > 0)
      famfindex = sqlxa->xaSQLDesc->ka->kafae->famfindex;
    else
      famfindex = 0;
  }
  else
    famfindex = da->sqlkano;
  if (famfindex > 0) {
    sqlmfentry *mf = &sqlca->sqlmfp->mfentry[famfindex-1];
    nextlpcnt = &mf->mfNextLoopCnt;
    lppos = &mf->mfActualPos;
    lastrecord = &mf->mfAllRecordsRead;
    /* ADIS 1001733	*/
    lpRecpos = &mf->mfRecpos;
    if (*nextlpcnt == -1) {
      *nextlpcnt = datapart->sp1p_part_header.sp1p_arg_count;
      if (*nextlpcnt >= da->sqlloop) {
	*nextlpcnt -= da->sqlloop;
      }
      else {
	lpcnt = *nextlpcnt;
	*nextlpcnt = 0;
      }
    }
    else {
      if ( *nextlpcnt >= da->sqlloop) {
	*nextlpcnt -= da->sqlloop;
      }
      else {
	lpcnt = *nextlpcnt;
	*nextlpcnt = 0;
	lp->lpnext = cpr_is_true;
	/* ADIS 1001733	*/
	*lpRecpos = 0;
      }
    }
    if (da->sqlloop != 0) {
      lpcnt = datapart->sp1p_part_header.sp1p_arg_count;
      *lppos = 0;
      trfirst = 1;
    }
  }
  while (sqlca->sqlcode == 0 && lpind < lpcnt) {
    for (pno = 1, var = var0; var <= varmx; var++, pno++) {
      tsp00_Int4 ival;
      void *varaddr = (char *) var->hostvaraddr
	+ rowno * var->hostcolsize;
      tsp00_Int4 *indaddr = (var->hostindaddr) ?
	var->hostindaddr + rowno : 0;

      if (var->col.colio == CPR_INFO_INPUT)
	continue;

      if (trfirst) {
	trfirst = 0;
	p04trloop (sqlca->sqlrap, (int) rowno);
      }
      err = cpr_p_ok;
      memcpy (name, var->colname, sizeof (name));
      p04colfrombuf (sqlca->sqlrap, (char*)data,
		     &datapart->sp1p_part_header.sp1p_buf_len,
		     &var->col, (char*)name, varaddr, var->hostvartype,
		     var->collength, (tsp00_Int2) var->collength,
		     var->colfrac, &ival, *lppos, (int) pno,
		     sqlca->sqlemp);
      err = (char) sqlca->sqlemp->eprerr;
      p03csqlemptosqlca (sqlca, sqlca->sqlemp);
      if (sqlca->sqlrap->rasqlldp->ldcolkind == cpr_is_true) {
	struct SQLLD   *ldp = sqlca->sqlrap->rasqlldp;
	tsp00_Int2       *ldmaxo = &ldp->ldmaxo;
	struct SQLPCLD *ldpc = ldp->ldpc;
	struct SQLPCLD *p = ldpc + *ldmaxo - 1;

	p->ldrowno = (lp->lpcnt == 0) ?
	  (tsp00_Int2) 1 : (tsp00_Int2) (rowno + 1);
	p->ldindkaddr = indaddr;
	p->ldcoladdr = &var->col;
	p->ldindktype = CPR_VINT4;
	p->ldindklen = 4;
	p->ldindkfrac = 0;
      }
      if (indaddr)
	memcpy (indaddr, &ival, sizeof (tsp00_Int4));
      else if (ival == CPR_NULL_DATA) {
	p04err (sqlca->sqlrap, sqlca->sqlemp,
		cpr_out_null_not_allowed);
	if (lp->lperro == cpr_p_ok)
	  lp->lperro = cpr_out_null_not_allowed;
      }
      if (lp->lperro == cpr_p_ok)
	lp->lperro = err;
    }
    p03csqlemptosqlca (sqlca, sqlca->sqlemp);
    *lppos += lp->lpreclo;
    lpind++;
    rowno++;
    trfirst = 1;
  }
  rownotfound = 0;
  if (lpcnt == 0)	{
    rownotfound = 1;
    lp->lpnext = cpr_is_false;
  }
  else {
    sqlca->sqlerrd[2] = (sqlca->sqlcode == 0) ? lpind :
      lpind - (tsp00_Int2) 1;
    lp->lperrd3 += sqlca->sqlerrd[2];
    lp->lpindo = sqlca->sqlerrd[2] + pnofirst;
    errd3 = lp->lperrd3 % lp->lpmax;
    if (datapart->sp1p_part_header.sp1p_attributes[0]
	& 1 << sp1pa_last_packet) {
      lp->lpnext = cpr_is_false;
      if (lastrecord)
	*lastrecord = cpr_is_true;
    }
    else {
      if (errd3 == 0) {
	lp->lpnext = cpr_is_false;
      }
      else {
	if (lp->lpcnt >= errd3 + lpcnt) {
	  lp->lpfirsti = cpr_is_true;
	  lp->lpnext = cpr_is_true;
	  lp->lpindi = 0;
	}
	else {
	  if (lp->lpcnt != errd3) {
	    lp->lpcnt -= (tsp00_Int2) errd3;
	    lp->lpnext = cpr_is_true;
	    lp->lpindi = 0;
	  }
	  else
	    lp->lpnext = cpr_is_false;
	}
      }
    }
  }
  if (lpind == 0 || (rownotfound && sqlca->sqlcode == 0)) {
    sqlca->sqlerrd[2] = (dbmode == CPR_KIND_ORACLE ||
			 dbmode == CPR_KIND_SAPR3) ? lp->lperrd3 : 0;
    em->ereturncode = 100;
    em->eerrorpos = 0;
    memcpy (em->etext, errs, ls);
    em->etextlen = ls;
    p03returncodeget (sqlca, sqlxa);
  }
  M90TRACE (M90_TR_EXIT, "p04sqldout  ", 0);
}     /*--------------------------------------------*/

void p04sftosql (sqlvartype * var, struct SQLROW *row,
    const tsp1_param_info *sfi, char init)
{
  if (sfi)
    p04decode (sfi, row, &var->col);
  var->colio = var->col.colio;
  var->colmode = var->col.colmode;
  var->coltype = var->col.coltype;
  var->collength = var->col.collen;
  var->colfrac = var->col.colfrac;
  if (init) {
    var->hostcolsize = 0;
    var->hostindicator = 0;
    var->hostvartype = -1;
    var->hostvaraddr = 0;
    var->hostindaddr = &var->hostindicator;
  }
}     /*--------------------------------------------*/

tsp00_Int2 p04or2sqlda (sqlcatype * sqlca, sqlxatype * sqlxa, sqlorentry *ore, tpr00_DescribeKindEnum DescribeKind) {
  sqldatype      *da = sqlca->sqlcxap->xasqldap;
  sqlvartype     *var = da->sqlvar;
  sqlvartype     *coln_var = da->sqlvar;
  struct SQLCOL  *col;
  tsp1_part *coln_part;
  tsp00_Uint1 *buf, *ebuf;
  char            first = 1;
  int ln, i;

  if (DescribeKind == Describe_Params_epr00) {
    da->sqln = ore->orcolcntacc;
    col = ore->orcolptr;
  }
  else {
    da->sqln = ore->orrescntacc;
    col = ore->orresptr;
  }

  if ((tsp00_Int4) da->sqln > da->sqlmax) {
    da->sqld = 0;
    if (sqlca->sqlrap->ralang != CPR_LA_CALL)
      p08runtimeerror (sqlca, sqlxa, cpr_too_many_hostvar);
    else
      p08runtimeerror (sqlca, sqlxa, cpr_too_many_hostvar_callif);
    return 0;
  }

  da->sqld = 0;
  for (i=0; i < da->sqln;i++) {
      var->col = col[i];
      p04sftosql (var, &da->sqlrow, NULL, (char) 0x01);

    if (var->colio != CPR_INFO_INPUT)
      da->sqld++;
    var++;
  }

  var = coln_var;
  /*handle Columnnames*/
  if (   DescribeKind == Describe_Columns_epr00
      && ore->orresnamacc != 0
      && da->sqln == da->sqld){
    tsp00_Uint4 cbWrite, cbParsed;
    sqlvartype     *evar = da->sqlvar + da->sqln;
    int i = 1, pos = 0, len;
    while (var < evar) {
      memset (var->colname,' ',sizeof (var->colname));
      len = (int) ((char*)ore->orresnamptr)[pos++];

      sp78convertString(sp77encodingUTF8,
                        var->colname,
                        sizeof (var->colname),
                        &cbWrite,
                        FALSE,
                        (tsp77encoding *)ore->orresnamencoding,
                        &(((char*)ore->orresnamptr)[pos]),
                        (tsp00_Uint4)len,
                        &cbParsed);

      pos += len;
      i ++;
      var++;
    }
  }
  else
  {
    static const char s[] = "COLUMN";
    static const int ls = sizeof (s) - 1;
    int i = 1;
    sqlvartype     *evar = da->sqlvar + da->sqln;
    while (var < evar) {
      memcpy (var->colname, s, ls);
      ln = ls + sprintf ((char*)&var->colname[ls], "%d", i++);
      if (ln < sizeof (var->colname))
        memset (&var->colname [ln], ' ',
                sizeof (var->colname) - ln);
        var++;
    }
  }
  return da->sqlrow.oreclen;
}     /*--------------------------------------------*/

tsp00_Int2
p04sqldsfi (sqlcatype * sqlca, sqlxatype * sqlxa, sqlorentry *ore, void *sqlda,
    char fetchdesc, char again, tpr00_DescribeKindEnum DescribeKind)
{
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  sqldatype      *da = (sqldatype *) sqlda;
  sqlvartype     *var = da->sqlvar;
  sqlvartype     *coln_var = da->sqlvar;
  struct SQLCOL  *col;
  tsp1_part *part;
  tsp1_part *coln_part;
  tsp00_Uint1 *buf, *ebuf;
  char            first = 1;
  int ln, i;
  tsp1_param_info sfi;

  M90TRACE (M90_TR_ENTRY, "p04sqlsfi   ", 0);
  p03find_part (sqlca->sqlrap, sp1pk_shortinfo, &part);
  if (!part) {
    if (!ore || (ore->orcolcntacc == 0 && ore->orrescntacc == 0)) {
      da->sqln = 0;
      return 0;
    }
  }
  if (part) {
    da->sqln = part->sp1p_part_header.sp1p_arg_count;
  }
  else {
    if (DescribeKind == Describe_Params_epr00) {
      da->sqln = ore->orcolcntacc;
      col = ore->orcolptr;
    }
    else {
      da->sqln = ore->orrescntacc;
      col = ore->orresptr;
    }
  }
  if ((tsp00_Int4) da->sqln > da->sqlmax) {
    da->sqld = 0;
    if (sqlca->sqlrap->ralang != CPR_LA_CALL)
      p08runtimeerror (sqlca, sqlxa, cpr_too_many_hostvar);
    else
      p08runtimeerror (sqlca, sqlxa, cpr_too_many_hostvar_callif);
    return 0;
  }
  p03find_part (sqlca->sqlrap, sp1pk_columnnames, &coln_part);
  if (part) {
    buf = part->sp1p_buf;
    ebuf = buf + part->sp1p_part_header.sp1p_buf_len;
  }
  da->sqld = 0;
  if (first)
    p04init (&da->sqlrow);
  for (i=0; i < da->sqln;i++) {
    if (part) {
      memcpy (&sfi, buf, sizeof (tsp1_param_info));
      buf += sizeof (tsp1_param_info);
      if (seg->sp1s_segm_header.variant.C_2.sp1r_function_code_F
          == csp1_string_command_fc &&
          ((sfi.sp1i_data_type >= dstra &&
            sfi.sp1i_data_type <= dstrb)
           || sfi.sp1i_data_type == dstruni))
        sfi.sp1i_data_type = dvarcharuni ;
      M90TRACEBUF ("sfi_info_string ", &sfi, 0, 11);
      p04sftosql (var, &da->sqlrow, &sfi, (char) !again);
      M90TRACEBUF ("sfi_info ", &sfi, 0, 11);
      M90TRACEBUF ("colinfo  ", &var->col, 0, 19);
    }
    else {
      var->col = col[i];
      p04sftosql (var, &da->sqlrow, NULL, (char) !again);
    }
    if (var->colio != CPR_INFO_INPUT)
      da->sqld++;
    var++;
  }
  var = coln_var;
  p03find_part (sqlca->sqlrap, sp1pk_columnnames, &part);
  if (part) {
    tsp00_Uint4   cbWrite;     /* number of bytes written */
    tsp00_Uint4   cbParsed;    /* number of bytes parsed */
    tsp78ConversionResult ConvRes;
    buf = part->sp1p_buf;
    ebuf = buf + part->sp1p_part_header.sp1p_buf_len;
    while (buf < ebuf) {
      ln = *buf++;
      ConvRes = sp78convertString(sp77encodingUTF8, var->colname, sizeof(var->colname), &cbWrite, FALSE, pr03cGetPacketEncoding(), buf, ln, &cbParsed);
      if (ConvRes != sp78_Ok) {
	sp77sprintf((char*)var->colname, sizeof(var->colname)-1, "Columnname corrupted (%s)", sp78errString(ConvRes));
      }
      else {
	void *FillBuf = var->colname+cbWrite;
	tsp00_Uint4  targetLength = sizeof(var->colname)-cbWrite;
	tsp00_Uint4  padLength = targetLength;
	sp77encodingUTF8->fillString(&FillBuf, &targetLength, padLength, ' ');
      }
      buf += ln;
      var++;
    }
  }
  else {
    if (! fetchdesc) {
      static const char s[] = "COLUMN";
      static const int ls = sizeof (s) - 1;
      int i = 1;
      sqlvartype     *evar = da->sqlvar + da->sqln;
      while (var < evar) {
	memcpy (var->colname, s, ls);
	ln = ls + sprintf ((char*)&var->colname[ls], "%d", i++);
	if (ln < sizeof (var->colname))
	  memset (&var->colname [ln], ' ',
		  sizeof (var->colname) - ln);
	var++;
      }
    }
  }
  M90TRACE (M90_TR_EXIT, "p04sqlsfi   ", 0);
  return da->sqlrow.oreclen;
}     /*--------------------------------------------*/

BOOLEAN p04IsEmptyString(char *va, tsp00_Int2 coltype, tsp00_Int2 vt, tsp00_Int4 vl)
{
  char *v=NULL;
  boolean unicode = false;

  if (!p04IsBinaryType(coltype)) {
    switch(vt) {
    case CPR_VCHAR:
    case CPR_VRAW:
    case CPR_VRAWC:
    case CPR_VANSICHAR:
    case CPR_VCHARC:
    case CPR_VCHARZ:
    case CPR_VANSICHARC:
      v = &va[0];
      break;
    case CPR_VSTRING:
    case CPR_VSTRINGC:
      v = &va[2];
      break;
    case CPR_VSTRING1:
      v = &va[1];
      break;
    case CPR_VSTRING4:
      v = &va[4];
      break;
    case CPR_VUNICODE:
    case CPR_VUNICODEC:
    case CPR_VUCS2:
    case CPR_VUTF16:
      v = &va[0];
      unicode = true;
      break;
    case CPR_VSTRINGUNICODE:
      v = &va[2];
      unicode = true;
      break;
    case CPR_VSTRINGUNICODE4:
      v = &va[4];
      unicode = true;
      break;
    }
  }
  if ( v && *v == '\0') {
    if (!unicode)
      return(TRUE);
    if (v[1] == '\0')
      return(TRUE);
    else
      return(FALSE);
  }
  else
    return(FALSE);
}

tsp00_Int2 p04IsFillUpHostvar( tsp00_Int2 vt )
{
  switch(vt) {
  case (CPR_VSTRING):
  case (CPR_VSTRINGC):
  case (CPR_VSTRING1):
  case (CPR_VSTRING4):
  case (CPR_VSTRINGUNICODE):
  case (CPR_VSTRINGUNICODE4):
    return(cpr_is_false);
  }
  return(cpr_is_true);
}

BOOLEAN p04IsBinaryType(tsp00_Int2 coltype)
{
  switch(coltype) {
  case (dcha):
  case (dunicode):
  case (dstra):
  case (dstruni):
  case (dlonga):
  case (dlonguni):
  case (dvarchara):
  case (dvarcharuni):
  case (ddate):
  case (dtime):
  case (dtimestamp):
    return(FALSE);
  default:
    return(TRUE);
  }
}

static tsp00_Int4 pr05cGetPaddedLength(tsp00_Int4  msgEncoding,
			const tsp00_Byte *szPaddedStr,
			tsp00_Uint4 cbSize,
			const char PadChar)
{
  register const tsp00_Byte *padCharPtr;
  tsp00_Int4 stepWidth, cbPaddedBytes;
  switch( msgEncoding )
  {
  case csp_unicode:
    stepWidth = 2;
    padCharPtr = szPaddedStr + cbSize - 1;
    break;
  case csp_unicode_swap:
    stepWidth = 2;
    padCharPtr = szPaddedStr + cbSize - stepWidth;
    break;
  default:
    stepWidth = 1;
    padCharPtr = szPaddedStr + cbSize - 1;
    break;
  }

  cbPaddedBytes = 0;

  /* Loop until begin of string or no more spaces found */
  while ( (padCharPtr >= szPaddedStr)
       && (*padCharPtr == PadChar) )
  {
    padCharPtr -= stepWidth;
    cbPaddedBytes += stepWidth;
  }
  return cbPaddedBytes;
}

static char pr04cGetDefineByte(const struct SQLCOL *this){
  char defByte;

  if( this->colisuni ) {
    /*unicode columns*/
    defByte = csp_unicode_def_byte;
  }
  else if (! p04IsBinaryType(this->coltype)){
    /*character columns but not unicode columns*/
    defByte = csp_ascii_blank;
  }
  else {
    /*binary and numeric columns*/
    defByte = csp_defined_byte;
  }
  return defByte;
}

tpr05_StringEncoding pr04cGetBufEncoding(const struct SQLCOL *this)
{
  tpr05_StringEncoding Encoding = pr03cGetPacketEncoding();
  if (p04isbyte(this)) /* bynary data always unconverted */
    return sp77encodingAscii;
  if (this->colisuni ) { /* unicode data */
    if (Encoding == sp77encodingAscii) { /* on an ascii request ensure swapping */
      /* ASCII Connection with UNICODE Data */
      Encoding = (pr04cPacketSwapKind == sw_full_swapped) ?
	sp77encodingUCS2Swapped : sp77encodingUCS2;
    }
  }
  return Encoding;
}

static char ident_sr[] = "@(#)vpr04c-w.c dated 2006-07-17 05:56:31 used 2008-05-09 01:05:40";
extern char *vpr04c_w_force_data () { return( ident_sr ); }
static char ident_mf[] = "@(#)vpr04c-w FAST I386 UNIX LINUX PTOC -REL30=1 -RELVER=R76 -LC_STATE=RAMP -COMPILER=UNDEF -MACHINE=UNDEF -SQLODBC=1";
static char ident_cc[] = "@(#)vpr04c-w cc -DREL30 -DLINUX -DI386 -DSAG -DSDB_RELEASE_070600 -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/usr/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl/SAPDB -DSYSV -D_SVID -I/usr/include/ncurses -w -Wall -D_FILE_OFFSET_BITS=64 -fPIC -DDYNAHASH -DHAS_NPTL -DSQLODBC -Dsql_sqldb -fno-strict-aliasing -DSAPDB_FAST -D_REENTRANT -O3 -march=pentium -mcpu=pentiumpro -I/home/gunter/SANDBOX/MaxDB/DevTool/incl ";
extern char *vpr04c_w_force_whatinfo () { return( ident_cc ); }
