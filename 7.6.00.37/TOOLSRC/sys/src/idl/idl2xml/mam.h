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
#ifndef _MAM_H
#define _MAM_H

#define MamAtCount      1  

#define FilName         1            /* file name                  : NoName */
#define FilBktSize      2            /* bucket size                :  32768 */
#define FilRdrSize      3            /* resident area              :   2048 */
#define FilRcdHandle    4            /* max. record handles        :     50 */
#define FilVersion      5            /* file version               :      0 */
#define FilOptResident 11            /* option = resident file     :   TRUE */
#define FilOptSave     12            /* option = save on close     :  FALSE */
#define FilOptNoSave   13            /* option = don't save on close : TRUE */

#define DstName       101
#define DstBlkSize    102
#define DstBktSize    103
                      
#define DixName       201
#define DixFunction   202
#define DixType       203
#define DixIndex      204

/* return codes */
                       
#define RcNil           0            /* no returncode set                   */
#define RcOk            1            /* success: (rc % 4) == 1              */
#define RcWarning       3            /* warning: (rc % 4) == 3              */
#define RcError         4            /* error:   (rc % 4) == 0              */
#define RcNip           8            /* error:   not implemented            */

/* made all SUCCESS indicators '1'  FJH 21-02-95 */

#define RcFilOk         1            /* success: (rc % 4) == 1              */
#define RcFilWrn      103            /* warning: (rc % 4) == 3              */
#define RcFilErr      104            /* error:   (rc % 4) == 0              */
#define RcFilInl      108            /* invalid name length == 0 or > 63    */
#define RcFilIbs      112            /* invalid bucket size != 2**10 - 16   */
#define RcFilIrs      116            /* invalid RDR size, too small         */
#define RcFilNsa      120            /* no space available                  */
#define RcFilNrh      124            /* no record handle available          */
#define RcFilFnf      128            /* file not found                      */

#define RcDstOk         1            /* success: (rc % 4) == 1              */
#define RcDstWrn      203            /* warning: (rc % 4) == 3              */
#define RcDstErr      204            /* error:   (rc % 4) == 0              */
#define RcDstIsz      208            /* invalid name length                 */
#define RcDstIbs      212            /* invalid block size bkt_sz%blk_sz !=0*/
#define RcDstNsa      216            /* no space available                  */
#define RcDstInf      220            /* index not found                     */
#define RcDstFnf      224            /* file not found                      */
#define RcDstDnf      228            /* dataset not found                   */

#define RcDixOk         1            /* success: (rc % 4) == 1              */
#define RcDixWrn      303            /* warning: (rc % 4) == 3              */
#define RcDixErr      304            /* error:   (rc % 4) == 0              */
#define RcDixNin      307            /* no index name supplied              */
#define RcDixInl      308            /* invalid name length == 0 or > 15    */
#define RcDixNif      312            /* no index function supplied          */
#define RcDixNsa      316            /* no space available                  */
#define RcDixKnf      320            /* key not found                       */
#define RcDixKne      323            /* key not equal                       */
#define RcDixEmp      324            /* index_set is empty                  */
#define RcDixDin      328            /* duplicate index name                */


#define RcRcdOk         1            /* success: (rc % 4) == 1              */
#define RcRcdWrn      403            /* warning: (rc % 4) == 3              */
#define RcRcdErr      404            /* error:   (rc % 4) == 0              */
#define RcRcdNmh      408            /* no more handles                     */
#define RcRcdEod      411            /* warning: end of dataset             */
#define RcRcdBod      415            /* warning: begiining of dataset       */
#define RcRcdRnf      416            /* record not found                    */
#define RcRcdKnf      420            /* key not found                       */
#define RcRcdKnx      423            /* key not found, but next found       */
#define RcRcdBnf      424            /* bucket not found                    */
#define RcRcdNsa      428            /* no space available                  */
#define RcRcdRne      432            /* record not empty                    */
#define RcRcdRnx      436            /* record not exists                   */
#define RcRcdRof      440            /* Index Overflow                      */

#define RcHdlOk         1            /* success: (rc % 4) == 1              */
#define RcHdlErr      504            /* error:   (rc % 4) == 0              */

#define RcBlkOk         1            /* success: (rc % 4) == 1              */
#define RcBlkWrn      603            /* warning: (rc % 4) == 3              */
#define RcBlkErr      604            /* error:   (rc % 4) == 0              */
#define RcBlkEnd      607            /* end of block                        */
#define RcBlkNmh      608            /* no more handles                     */
#define RcBlkEod      611            /* warning: end of dataset             */
#define RcBlkBod      615            /* warning: begiining of dataset       */
#define RcBlkInf      620            /* index not found                     */
#define RcBlkDnf      628            /* dataset not found                   */

#define RcRngOk         1            /* success: (rc % 4) == 1              */
#define RcRngWrn      703            /* warning: (rc % 4) == 3              */
#define RcRngErr      704            /* error:   (rc % 4) == 0              */
#define RcRngNsa      708            /* no space available                  */
#define RcRngTlo      712            /* record is too long                  */
#define RcRngBnf      716            /* block not found                     */
#define RcRngEni      720            /* element not inserted                */
#define RcRngEna      724            /* element not appended                */
#define RcRngEnr      728            /* element not read                    */
#define RcRngEnd      732            /* element not deleted                 */
#define RcRngNba      736            /* no block acquired                   */
#define RcRngRer      740            /* read error in replacement dataset   */
#define RcRngRnd      744            /* range not deleted                   */
#define RcRngBns      748            /* block not split                     */


/* function codes for index function */

#define Dix_nil         0
#define Dix_keygen      1            /* generate key                        */
#define Dix_keyrcd      2            /* compare key - record                */
#define Dix_rcdrcd      3            /* compare record - record             */

/* function codes for record operations */

#define Rcd_nil         0
#define Rcd_bod         1            /* read at the beginning of dataset    */
#define Rcd_nxt         2            /* read next record                    */
#define Rcd_prv         3            /* read previous record                */
#define Rcd_eod         4            /* read at the end of dataset          */
#define Rcd_rdi         5            /* key is Record Dataset ID (RDI)      */
#define Rcd_lss         6            /* search key       <        and read  */
#define Rcd_leq         7            /* search key       <=       and read  */
#define Rcd_eqb         8            /* search first key =(begin) and read  */
#define Rcd_eqe         9            /* search last key  =(end)   and read  */
#define Rcd_geq        10            /* search key       >=       and read  */
#define Rcd_gtr        11            /* search key       >        and read  */
#define Rcd_beq        12            /* like Rcd_eqb, warning if unequal    */
#define Rcd_eeq        13            /* like Rcd_eqe, warning if unequal    */
#define Rcd_cur	     25            /* current state must be used          */

/* new function codes for record operations */

#define Rcd_nil_Key     0
#define Rcd_bod_Key     1            /* read at the beginning of dataset    */
#define Rcd_nxt_Key     2            /* read next record                    */
#define Rcd_prv_Key     3            /* read previous record                */
#define Rcd_eod_Key     4            /* read at the end of dataset          */
#define Rcd_rdi_Key     5            /* key is Record Dataset ID (RDI)      */
#define Rcd_lss_Key   110            /* search key       <        and read  */
#define Rcd_leq_Key   111            /* search key       <=       and read  */
#define Rcd_eqb_Key     8            /* search first key =(begin) and read  */
#define Rcd_eqe_Key     9            /* search last key  =(end)   and read  */
#define Rcd_geq_Key   106            /* search key       >=       and read  */
#define Rcd_gtr_Key   107            /* search key       >        and read  */
#define Rcd_beq_Key    12            /* like Rcd_eqb, warning if unequal    */
#define Rcd_eeq_Key    13            /* like Rcd_eqe, warning if unequal    */
#define Rcd_cur_Key    25            /* current state must be used          */

/* function codes for record operations */

#define Blk_nil         0
#define Blk_bod         1            /* read at the beginning of dataset    */
#define Blk_nxt         2            /* read next block                     */
#define Blk_prv         3            /* read previous block                 */
#define Blk_eod         4            /* read at the end of dataset          */
#define Blk_bdi         5            /* key is Block Dataset ID (BDI)       */

#define Dst_nil         0
#define Dst_save        5

typedef unsigned long DixFunc, *DixFuncp;

typedef struct
{
 unsigned char  id;                  /* ID of attribute                     */
 char           nm[15];              /* nam of attribute                    */
 char           vc[64];              /* attribute value in character format */
 unsigned long  vi;                  /* attribute value in integer format   */
 unsigned long  df;                  /* default value for attribute         */
 unsigned long  rc;                  /* return code for attribute           */
} st_MamAtL, *st_MamAtLp;

typedef struct
{
 unsigned short lg;                  /* length of data element              */
 unsigned short ty;                  /* type of data element                */
 unsigned char *pt;                  /* pointer to data element             */
} st_dsc, *st_dsc_p;

typedef struct
{
 unsigned short ct;                  /* number of row in attribute list     */
 unsigned short ty;                  /* type of data element                */
 st_MamAtLp     pt;                  /* pointer to data element             */
} st_AtLdsc, *st_AtLdscp;
                                 
/* Mam Handles */                   

typedef struct
{
 st_dsc_p       table_dp;
 unsigned short ix;
 unsigned short id;
} MamHandle, *MamHandlep;

typedef struct
{
 unsigned short   blkb_ix;           /* beginning of range: block index     */ 
 unsigned short   blkb_off;          /*                     block offset    */
 unsigned short   blke_ix;           /* end of range:       block index     */ 
 unsigned short   blke_off;          /*                     block offset    */
 unsigned long    precursor;
 unsigned long    postcursor;
}st_rng, *st_rng_p;

typedef MamHandle FilHandle, *FilHandlep;
typedef MamHandle DstHandle, *DstHandlep;
typedef MamHandle BlkHandle, *BlkHandlep;
typedef MamHandle DixHandle, *DixHandlep;
typedef MamHandle RcdHandle, *RcdHandlep;

#ifndef Gen00Def
#ifndef rc_t_Declared
#define rc_t_Declared
typedef unsigned short rc_t;
#endif
#endif

#ifndef MamNoFuncDef


/*   File manipulation MAM routines   */
 
rc_t  MamFilCreate
( 
 st_AtLdscp     MamAtLp,             /* pointer to file attribute list      */
 FilHandlep     FilHp                /* pointer to file handle              */
);

rc_t  MamFilOpen
( 
 st_AtLdscp     MamAtLp,             /* pointer to file attribute list      */
 FilHandlep     FilHp                /* pointer to file handle              */
);

rc_t  MamFilClose
(
 st_AtLdscp     MamAtLp,             /* pointer to file attribute list   */
 FilHandlep     FilHp                /* file handle pointer                 */
);                                                                          

rc_t  MamFilDelete
(
 FilHandlep     FilHp                /* file handle pointer                 */
);                                                                          

rc_t MamFilVersion
(
 char           *FileName,           /* file name                           */
 unsigned short *FileVersion
);                                                                          


/*   Dataset manipulation MAM routines   */
 
rc_t  MamDstCreate
(
 st_AtLdscp     MamAtLp,             /* pointer to dataset attribute list   */
 FilHandlep     FilHp,               /* file handle pointer                 */
 DstHandlep     DstHp                /* pointer to dataset handle           */
);

rc_t  MamDstOpen
(
 st_AtLdscp     MamAtLp,             /* pointer to dataset attribute list   */
 FilHandlep     FilHp,               /* file handle pointer                 */
 DstHandlep     DstHp                /* pointer to dataset handle           */
);

rc_t  MamDstClose
(
/* st_AtLdscp     MamAtLp,              pointer to dataset attribute list   */
 DstHandlep     DstHp                /* dataset handle pointer              */
);                                                                          

rc_t  MamDstDelete
(
/* st_AtLdscp     MamAtLp,              pointer to dataset attribute list   */
 DstHandlep     DstHp                /* dataset handle pointer              */
);                                                                          

rc_t  MamDstErase
(
/* st_AtLdscp     MamAtLp,              pointer to dataset attribute list   */
 DstHandlep     DstHp                /* dataset handle pointer              */
);                                                                          

rc_t MamDstInfo
(
 st_AtLdscp     MamAtLp,             /* descriptor of dataset attribute list*/
 DstHandlep     DstHp                /* pointer to dataset handle           */
);

/*   Block manipulation MAM routines   */
             
rc_t  MamBlkOpen
(
 DstHandlep     DstHp,               /* pointer to dataset handle           */
 BlkHandlep     BlkHp                /* pointer to block handle             */
);

rc_t  MamBlkClose
(
/* st_AtLdscp     MamAtLp,              pointer to dataset attribute list   */
 BlkHandlep     BlkHp                /* pointer to block handle             */
);                                                                          
            
/* find a record given a certain key                                        */
rc_t MamBlkFind
(
 unsigned short bdi,                 /* block dataset identifier to be read */          
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 st_dsc_p       w_blk_dp             /* pointer to block descriptor         */
);

/* read a record which position is based on the state of the recordhandler  */
rc_t MamBlkRead
(
 unsigned short r_blk_func,          /* function code for block operation   */
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 st_dsc_p       w_blk_dp             /* pointer to block descriptor         */
);

/* get the current block number                                             */
rc_t MamBlkNo
(
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 unsigned short *blk_no_p            /* function code for block operation   */
);
            
/* get the last block number                                                */
rc_t MamBlkLastNo
(
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 unsigned short *blk_no_p            /* function code for block operation   */
);
            
rc_t MamBlkRecInfo
(
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 unsigned short blk_no,              /* block no. in dataset                */
 unsigned short blk_off,             /* offset in block                     */
 st_dsc_p       blk_dp,              /* block descriptor                    */
 unsigned short *acc_sz_p,           /* accumulated record sizes of block   */
 unsigned short *rcd_ct_p,           /* record count in block               */
 unsigned short *rcd_no_p,           /* record no. in block                 */
 unsigned short *rcd_off_p           /* offset in record                    */
);

rc_t MamBlkInfo
(
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 unsigned short *blk_no_p,           /* number of blocks                    */
 unsigned char  **blktab_p           /* pointer to block table              */
);

/*   Range manipulation MAM routines   */

rc_t  MamRngBlkRead
(
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 unsigned short ix,                  /* beginning of range in block         */
 st_dsc_p       blk_dp
);

rc_t MamRngDelete
(
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 st_rng_p       r_rng_p,   
 st_rng_p       w_newrng_p  
);

rc_t MamRngFldReplace
(
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 st_rng_p       r_rng_p,  
 st_rng_p       w_newrng_p, 
 st_dsc_p       fld_dp               /* block descriptor                    */
);

rc_t MamRngRcdReplace
(
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 st_rng_p       r_rng_p,  
 st_rng_p       w_newrng_p, 
 RcdHandlep     RcdHp                /* Record HANDLE pointer               */
);

rc_t  MamRngDisplay
(
 BlkHandlep     BlkHp,               /* Block HANDLE pointer                */
 unsigned short ix,                  /* block block index                   */
 char           *buf
);

/*   Index manipulation MAM routines   */
             
rc_t  MamDixCreate
(
 st_AtLdscp     MamAtLp,             /* pointer to index attribute list     */
 DstHandlep     DstHp,               /* dataset handle pointer              */
 DixHandlep     DixHp                /* pointer to dataset index handle     */
);

rc_t  MamDixOpen
(
 st_AtLdscp     MamAtLp,             /* pointer to index attribute list     */
 DstHandlep     DstHp,               /* dataset handle pointer              */
 DixHandlep     DixHp                /* pointer to dataset index handle     */
);

rc_t  MamDixDelete
(
 DixHandlep     DixHp                /* dataset index handle pointer        */
);

          
          
/*   Recordhandlers MAM routines   */
           
rc_t  MamRcdOpen
(
 DstHandlep     DstHp,               /* dataset handle pointer              */
 DixHandlep     DixHp,               /* dataset index handle pointer        */
 RcdHandlep     RcdHp                /* pointer to record handle            */
);

rc_t  MamRcdClose
(
 RcdHandlep     RcdHp                /* record handle pointer               */
);
     
     
/*   Actions on Records (i.e. via the above Recordhandlers)                 */
      
/*   Add a record at the end of a dataset */
rc_t MamRcdAdd
(
 unsigned short r_rcd_func,          /* function code for record operation  */
 RcdHandlep     RcdHp,               /* record handle pointer               */
 st_dsc_p       r_rcd_dp             /* pointer to record descriptor        */
);

/* append to a record, i.e. length new record > length old record           */
rc_t MamRcdAppend
(
 unsigned short r_rcd_func,          /* function code for record operation  */
 RcdHandlep     RcdHp,               /* record handle pointer               */
 st_dsc_p       r_rcd_dp             /* pointer to record descriptor        */
);

/* extend a record to a specified length, i.e. new length > current length  */
rc_t MamRcdExtend
(
 unsigned short r_rcd_func,          /* function code for bucket operation  */
 RcdHandlep     RcdHp,               /* record handler pointer              */
 unsigned short r_elt_lg             /* new record length                   */
);

                
/* Clear a record, i.e. record stays in place but length becomes zero       */                
rc_t MamRcdClear
(
 unsigned short r_rcd_func,          /* function code for record operation  */
 RcdHandlep     RcdHp,               /* record handle pointer               */
 st_dsc_p       w_rcd_dp             /* pointer to record descriptor        */
);

/* Clear a record, i.e. record stays in place but length becomes zero       */                
rc_t MamRcdDelete
(
 unsigned short r_rcd_func,          /* function code for record operation  */
 RcdHandlep     RcdHp               /* record handle pointer               */
);


/* reuse an earlier cleared record                                          */
rc_t MamRcdReuse
(
 unsigned short r_rcd_func,          /* function code for record operation  */
 RcdHandlep     RcdHp,               /* record handle pointer               */
 st_dsc_p       w_rcd_dp             /* pointer to record descriptor        */
);


/* find a record given a certain key                                        */
rc_t MamRcdFind
(
 unsigned short r_rcd_func,          /* function code for bucket operation  */
 RcdHandlep     RcdHp,               /* record handle pointer               */
 st_dsc_p       w_rcd_dp,            /* pointer to record descriptor        */
 st_dsc_p       r_key_dp             /* pointer to key descriptpr           */
);

/* read a record which position is based on the state of the recordhandler  */
rc_t MamRcdRead
(
 unsigned short r_rcd_func,          /* function code for record operation  */
 RcdHandlep     RcdHp,               /* record handle pointer               */
 st_dsc_p       w_rcd_dp             /* pointer to record descriptor        */
);


/* write a record which position is based on the state of the recordhandler */
rc_t MamRcdWrite
(
 unsigned short r_rcd_func,          /* function code for record operation  */
 RcdHandlep     RcdHp,               /* record handle pointer               */
 st_dsc_p       w_rcd_dp             /* pointer to record descriptor        */
);

/* replace a record which position is based on the state of the recordhandler*/
rc_t MamRcdReplace
(
 unsigned short r_rcd_func,          /* function code for record operation  */
 RcdHandlep     RcdHp,               /* record handle pointer               */
 st_dsc_p       w_rcd_dp             /* pointer to record descriptor        */
);


/* get the physical position of the record based on the current state       */
rc_t MamRcdNo
(
 RcdHandlep     RcdHp,               /* record handle pointer               */
 unsigned long  *w_no                /* pointer to receive record number    */
);                                                                         

/* get the record count of the dataset where the RcdH is pointing to  */
rc_t MamRcdCnt
(
  RcdHandlep      RcdHp,                /* Rcd Handle                          */
  unsigned long   *w_rcd_ct              /* pointer to record count field   */
);

#endif

#endif  
