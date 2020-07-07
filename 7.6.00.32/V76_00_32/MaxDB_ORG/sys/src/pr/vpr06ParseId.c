/*!================================================================

  module:       vpr06ParseId.c

 -------------------------------------------------------------------

  responsible:  BurkhardD

  special area:

  description:

  see also:

  change history :

        2001-05-17 D025086 Use pr03PacketReqRec instead of p03reqrecpacket
        to protect unexpected communication errors.
        D025086 03-12-2001 Add pr06ParseIdDropDirect
        D025086 03-12-2001 Add pr06ParseIdAddDropParseIdCmd

  -------------------------------------------------------------------------





    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




 ===================================================================*/


/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vpr06ParseId.h"
#include "vpr01Con.h"
#include "gpr03m.h"
#include "gpr01.h"
#include "gpr03.h"
#include "vpr03Segment.h"
#include "vpr03Packet.h"
#include "vpr03Part.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

#define PR06_CMD_DROP_PARSID "DROP PARSEID"

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static tpr_runtime_errors_Enum pr06ParseIdAddDropParseIdCmd(tpr03_SegmDesc *SegmDesc, tpr00_ParseId *ParseID);
static BOOLEAN pr06ParseIdCheckFackedFetchID(tpr00_ParseId *aparseid);
/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

BOOLEAN pr06ParseIdIsEqual(tpr00_ParseId *p1, tpr00_ParseId *p2)
{
  return ( memcmp(p1, p2, ParseId_mxpr00 ) ? FALSE : TRUE );
}

void pr06ParseIdCopy(tpr00_ParseId *p1, tpr00_ParseId *p2)
{
  memcpy(p1, p2, ParseId_mxpr00);
}

void pr06ParseInfoCopy(tpr00_ParseInfo *p1, tpr00_ParseInfo *p2)
{
  memmove(p1, p2, sizeof(tpr00_ParseInfo));
}

tpr06_ParseIDCache *pr06ParseIDNewCache(struct tpr01_ConDesc *ConDesc, tsp00_Int4 Size)
{
  tpr06_ParseIDCache *ParseIDCache = pr03mAllocat(sizeof(tpr06_ParseIDCache), "ParseIDCache");
  if (ParseIDCache) {
    ParseIDCache->ConDesc = ConDesc;
    ParseIDCache->MaxParseIDs = Size;
    ParseIDCache->ParseIDDesc = pr09NewDescriptor(10, sizeof(tpr00_ParseId));
  }
  return ParseIDCache;
}

/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127862 */
void pr06ParseIDDeleteCache (tpr06_ParseIDCache *Cache)
{
  if (Cache) {
    if (Cache->ParseIDDesc)
      pr09DeleteDescriptor (Cache->ParseIDDesc);
    pr03mFree((tsp00_Uint1*)Cache, "ParseIDCache");
  }
}

externC void pr06ParseIdDropIntoCache(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, tpr06_ParseIDCache *ParseIDCache, tpr00_ParseId *ParseID)
{
  if (ParseIDCache && ParseIDCache->ConDesc)  {
    	tpr01_ConDesc *ConDesc = ParseIDCache->ConDesc;
    	tpr01_ConContainer *Connection = ConDesc->Connection;
    	if (!Connection->IsConnected(ConDesc)) {
    	/* connection down => delete parseid drop cache */
      		pr09CloseAllItems(ParseIDCache->ParseIDDesc);
    	}
    	else if( ! pr06ParseIdIsEqual(ParseID, (tpr00_ParseId*)CPR_PARSIDNULL)
    	       && ((char*)*ParseID)[CPR_PARS_IDIX] != csp1_p_command_executed
    	       && ((char*)*ParseID)[CPR_PARS_IDIX] != csp1_p_use_adbs
    	       &&  pr06ParseIdCheckFackedFetchID(ParseID) ) {
    	        /* put parseid in cache */
                tpr00_ParseId *NewParseID = pr09AddItem(ParseIDCache->ParseIDDesc);
                pr06ParseIdCopy(NewParseID, ParseID);
                p01pparsidtrace (sqlca, sqlxa, NULL, *ParseID, TraceParseIDDrop_epr00);
        } /* end if IsConnected*/
  }/* end if (ParseIDCache->ConDesc) */
  /* set ParseID null */
  pr06ParseIdCopy(ParseID,(tpr00_ParseId*) CPR_PARSIDNULL);
}

externC void pr06ParseIdDrop(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, tpr06_ParseIDCache *ParseIDCache, tpr00_ParseId *ParseID)
{
  if (ParseIDCache && ParseIDCache->ConDesc)
  {
    	tpr01_ConDesc *ConDesc = ParseIDCache->ConDesc;
    	tpr01_ConContainer *Connection = ConDesc->Connection;
    	if (!Connection->IsConnected(ConDesc))
    	/* connection down => delete parseid drop cache */
    	{
      		pr09CloseAllItems(ParseIDCache->ParseIDDesc);
    	}
    	else
    	{
          if (sqlca->sqlrap->raparse_again == FALSE) {
    	    if( ! pr06ParseIdIsEqual(ParseID, (tpr00_ParseId*)CPR_PARSIDNULL)
    	       && ((char*)*ParseID)[CPR_PARS_IDIX] != csp1_p_command_executed
    	       && ((char*)*ParseID)[CPR_PARS_IDIX] != csp1_p_use_adbs
    	       &&  pr06ParseIdCheckFackedFetchID(ParseID) )
    	    {
    	        /* put parseid in cache */
                tpr00_ParseId *NewParseID = pr09AddItem(ParseIDCache->ParseIDDesc);
                pr06ParseIdCopy(NewParseID, ParseID);
            }
            if (pr09GetItemCount(ParseIDCache->ParseIDDesc) > ParseIDCache->MaxParseIDs-1)
                /* cache full => send drop parseid order*/
      	    {
              tpr03_SegmDesc *SegmDesc;
              tpr00_ParseId *ParseID;
              tpr00_ParseId *lastParseID; /* last ParseId poped from cache */
              BOOLEAN firstParseid = TRUE; /*is true for first parseid put to order*/
              /* set cache cursor on first element*/
              pr09SetIndex(ParseIDCache->ParseIDDesc, Start_epr09);

              /* reset returncodes of sqlca an init cmd packet */
              p03csqlcaareainit(sqlca);
              SegmDesc = Connection->InitPacket(ConDesc, sp77encodingAscii, sp1m_dbs);
              /*for all parseid`s in Cache built order*/
              while(ParseID = pr09GetNextItem(ParseIDCache->ParseIDDesc)) {
                if(  pr06ParseIdCheckKnlSessionID(&ConDesc->ga->gaKnlSessionID, ParseID)){
                  if (! firstParseid) {
                    pr03SegmentAdd(SegmDesc);
                  }
                  else {
                    firstParseid = FALSE;
                  }
                  pr06ParseIdAddDropParseIdCmd(SegmDesc, ParseID);
                  lastParseID = ParseID;
                }
              }  
              /* send-recieve part*/
              /* trace only the last ParseID from Cache */
              p01pparsidtrace(sqlca, sqlxa, NULL, *lastParseID, TraceParseIDDrop_epr00);
              pr03PacketReqRec (ConDesc, sqlca->sqlemp);   /*send dbs message to kernel*/
              p01xtimetrace(sqlca, sqlxa, ga);
              /* no error handling
                 if (sqlca->sqlemp->ereturncode != 0)
                 {
                 p03returncodeget (sqlca, sqlxa);
                 }*/

              /* delete parseid drop cache */
              pr09CloseAllItems(ParseIDCache->ParseIDDesc);
            }
            else
              p01pparsidtrace (sqlca, sqlxa, NULL, *ParseID, TraceParseIDDrop_epr00);
          }
    	} /* end if IsConnected*/
  }/* end if (ParseIDCache->ConDesc) */
  /* set ParseID null */
  pr06ParseIdCopy(ParseID,(tpr00_ParseId*) CPR_PARSIDNULL);
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr06ParseIdAddDropParseId

  see also:

  Description:  Adds drop parsid request to the segement.


  Arguments:
    sqlca [in]  -- no comment --
    sqxa  [in]  -- no comment --
    ConDesc [in] descrptor to current connection
    ParseID [in] ParseID to drop

  return value: nothing

  change history :

        D025086 03-12-2001 Initial version.

*/
static tpr_runtime_errors_Enum pr06ParseIdAddDropParseIdCmd(tpr03_SegmDesc *SegmDesc, tpr00_ParseId *ParseID)
{
  tpr_runtime_errors_Enum Err = cpr_p_ok;
  tpr05_StringEncoding PacketEncoding = pr03PacketGetEncoding(SegmDesc->packetPtr);
  tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_command);
  tsp00_Int4 cbPartLen = pr03PartGetFreePartSpace(partPtr);
  tsp00_Int4 Offset = 0;
  Err = pr03PartConverttoPart(partPtr, &Offset, &cbPartLen, PacketEncoding, PR06_CMD_DROP_PARSID, sizeof(PR06_CMD_DROP_PARSID)-1, sp77encodingAscii);
  pr03SegmentFinishPart(SegmDesc);
  if (Err == cpr_p_ok) {
    partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_parsid);
    pr03PartPut(partPtr, 0,ParseID, ParseIdKnl_mxpr00);
    pr03SegmentFinishPart(SegmDesc);
  }
  return Err;
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr06ParseIdDropDirect

  see also:

  Description:  Send a drop parsid request to the kernel.


  Arguments:
    ConDesc [in] descrptor to current connection
    ParseID [in] ParseID to drop

  return value: nothing

  change history :

        D025086 03-12-2001 Initial version.

*/
externC void pr06ParseIdDropDirect(struct tpr01_ConDesc *ConDesc, tpr00_ParseId *ParseID)
{
  static bool recurs = false;
  tpr01_ConContainer *Connection = ConDesc->Connection;
  if(  pr06ParseIdCheckKnlSessionID(&ConDesc->ga->gaKnlSessionID, ParseID)
     &&pr06ParseIdCheckFackedFetchID(ParseID)){
    sqlcatype *sqlca = ConDesc->sqlca;
    sqlxatype *sqlxa = ConDesc->sqlxa;
    tpr03_SegmDesc *SegmDesc;
    /* reset returncodes of sqlca an init cmd packet */
    p03csqlcaareainit(sqlca);
    SegmDesc = Connection->InitPacket(ConDesc, sp77encodingAscii, sp1m_dbs);
    if (SegmDesc) {
      /* build dropparsid order */
      pr06ParseIdAddDropParseIdCmd(SegmDesc, ParseID);
      /* send the drop parsid request */
      p01pparsidtrace (sqlca, sqlxa, NULL, *ParseID, TraceParseIDDrop_epr00);
      pr03PacketReqRec (ConDesc, sqlca->sqlemp);
      p01xtimetrace(sqlca, sqlxa, ConDesc->ga);
    }
  }
  else {
    tpr01_ConDesc *ConDescDrop = Connection->GetConDescByParseID(Connection, ParseID);
    if (ConDescDrop && !recurs) {
      recurs = true; /* do not call recusive sure is sure */
      pr06ParseIdDropDirect(ConDescDrop, ParseID);
    }
  }
  recurs = false;
}

/*!
  Function:     pr06ParseIdCheckKnlSessionID

  see also:

  Description:  Validates the KnlSessionID of given parseid

  A part of every parseid is the session-id defined by database kernel. This function
  checks if the given parseid is the same as stored in gaentry->KnlSessionID.

  Arguments:
    ConDesc    [in] pointer to connect descriptor
    laparseid  [in] a parseid

  return value: TRUE  if KnlSessionID of parseid is the same as stored in gaentry
                FALSE if KnlSessionID of parseid is not the same as stored in gaentry
 */
externC BOOLEAN pr06ParseIdCheckKnlSessionID(tpr00_SessionID *SessionID, tpr00_ParseId *aparseid)
{
  BOOLEAN ret = FALSE;
  /*check knlSessionID*/
  ret = ! ( memcmp (SessionID->KnlSession, &((char*)aparseid)[0], sizeof(SessionID->KnlSession)) );
  if (ret) 
    ret = ! ( memcmp (&SessionID->ClientSession, &((char*)aparseid)[ParseIdKnl_mxpr00], sizeof(SessionID->ClientSession)) );
  return ret;
}

/*!
  Function:     pr06ParseIdCheckFackedFetchID

  see also:

  Description:  Checks if the given parseid was a real parseid or a fake

  Sometimes the mfetch functions need facked parseid. This parseid was generated 
  by the precompiler and should not be droped.

  Arguments:
    laparseid  [in] a parseid

  return value: TRUE  if aparseid is a real kernel parseid
                FALSE if aparseid is a fake
 */
static BOOLEAN pr06ParseIdCheckFackedFetchID(tpr00_ParseId *aparseid)
{
  /*check faked fetch parseid*/
  return ((BOOLEAN) memcmp(&((char*)aparseid)[sizeof(tpr00_KnlSessionID)], (tpr00_ParseId*)CPR_PARSIDNULL, 6));
}
/*!
  Function:     pr06ParseIdKnlSessionIDCompare

  see also:

  Description:  Compares two KnlSessionID`s

  A part of every parseid is the session-id defined by database kernel. This function
  compares two KnlSessionID`s.

  Arguments:
    KnlSessionID_1  [in] const tpr00_KnlSessionID
    KnlSessionID_2  [in] const tpr00_KnlSessionID

  return value:    0 if KnlSessionID_1 identical to KnlSessionID_2 
                != 0 if KnlSessionID_1 not identical to KnlSessionID_2  
 */
externC tsp00_Int4 pr06ParseIdKnlSessionIDCompare(tpr00_SessionID *SessionID_1, tpr00_SessionID *SessionID_2)
{
  tsp00_Int4 ret = -1;
  ret = ( memcmp (SessionID_1, SessionID_2, sizeof(tpr00_SessionID)) );
  return ret;
}

/*!
  Function:     pr06ParseIdKnlSessionIDCopy

  see also:

  Description:  Copies a KnlSessionID into another KnlSessionID


  Arguments:
    KnlSessionID_Dest  [in] const tpr00_KnlSessionID
    KnlSessionID_Src   [in] const tpr00_KnlSessionID

  return value: void 
 */
externC void pr06ParseIdKnlSessionIDCopy(tpr00_SessionID *Dest, tpr00_SessionID *Src)
{
  memcpy (Dest, Src, sizeof(tpr00_SessionID));
}

/*!
  Function:     pr06ParseIdSessionID2ParseID

  see also:

  Description:  Copies a KnlSessionID into another KnlSessionID


  Arguments:
    KnlSessionID_Dest  [in] const tpr00_KnlSessionID
    KnlSessionID_Src   [in] const tpr00_KnlSessionID

  return value: void 
 */
externC void pr06ParseIdSessionID2ParseID(tpr00_ParseId *ParseID, tpr00_SessionID *SessionID)
{
  memcpy (ParseID, SessionID->KnlSession, sizeof(SessionID->KnlSession));
  memcpy (&((char*)ParseID)[ParseIdKnl_mxpr00], &SessionID->ClientSession, sizeof(SessionID->ClientSession));
}

