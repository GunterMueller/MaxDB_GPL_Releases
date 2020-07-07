
/* some common prolog for every module
 */

#ifndef GAK40_H
#define GAK40_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

#ifndef GSP00_H
#include "gsp00.h"
#endif

#ifndef VSP001_H
#include "vsp001.h"
#endif

#ifndef GGG00_H
#include "ggg00.h"
#endif

#ifndef GGG04_H
#include "ggg04.h"
#endif

#ifndef GAK00_H
#include "gak00.h"
#endif

#ifndef VAK001_H
#include "vak001.h"
#endif

#ifndef GGG07_H
#include "ggg07.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

    /* max length of SHOW-resultrow */
#define mxak40_shinfo                        5400

/******************************************
 *               T Y P E S                *
 ******************************************/

typedef ARRAY_TYPE_MSP00 (         tak40_int2_arr, tsp00_Int2, mxak40_shinfo);
typedef tak40_int2_arr            *tak40_int2_arr_ptr;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tak40_shinfo, mxak40_shinfo);
typedef CSTRING_TYPE_MSP00      (  tak40_shinfoc, mxak40_shinfo);

typedef struct                     tak40_show_record
{
    tsp00_Int2          shsl;
    tsp00_Int2          shkl;
    tsp00_Int2          shvarcol_offset;
    tsp00_Int2          shvarcol_cnt;
    tsp00_C4            shkey;
    tak40_shinfo        shinfo;
} tak40_show_record;

typedef struct                     tak40_show_glob
{
    tak_sysbufferaddress a4sysbuf;
    tak00_colinfo_ptr   a4col_ptr;
    tak_sysbufferaddress a4dombuf;
    tsp00_Int4          a4commandId;
    tsp00_Int4          a4ti;
    tsp00_Int4          a4ci;
    tsp00_Int4          a4pos;
    tsp00_Int4          a4index;
    tsp00_Int4          a4id_len;
    tsp00_C4            a4sh_key;
    tsp00_Int4          a4LongKey;
    tak_syspointerarr   a4p_arr;
    tsp00_KnlIdentifier a4authname;
    tsp00_KnlIdentifier a4tablen;
    tsp00_KnlIdentifier a4user;
    tsp00_KnlIdentifier a4coln;
    tgg00_SysInfoKey    a4sysk;
    tsp00_KnlIdentifier a4showfile_name;
    tgg00_ShowKind      a4sh_kind;
    pasbool             a4priv_check;
    tak00_PrivilegeSet  a4priv;
    pasbool             a4is_synonym;
    pasbool             a4del_basetab;
    tsp00_Int2          a4tab_count;
    tgg00_FileId        a4treeid;
    tgg00_FileId        a4LongContainer;
    tak40_show_record   a4sh_buf;
    tsp00_Int2          a4col_len;
    tsp00_Int2          a4tab_len;
    tsp00_Int4          a4error_pos;
    tsp00_KnlIdentifier a4synname;
    pasbool             a4systab_select;
    pasbool             a4unique;
    pasbool             a4temp;
    pasbool             a4private;
    pasbool             a4nonprivate;
    pasbool             a4public;
    pasbool             a4syns;
    pasbool             a4all;
    pasbool             a4is_systable;
    pasbool             a4sys_tables;
    pasbool             a4use_base;
    pasbool             a4use_view;
    tsp00_Int2          a4usertab_pos;
    tsp00_Int2          a4usertab_cnt;
    tsp00_Int2          a4mfetch_pos;
    tsp00_Int2          a4mfetch_cnt;
    tgg00_FilePos       a4tree_pos;
    tak_sysbufferaddress a4usertab_buf;
    tgg00_SysInfoKey    a4next_key;
    tgg00_BasisError    a4b_err;
    tsp00_KnlIdentifier a4next_owner;
    tsp00_KnlIdentifier a4next_tablen;
    tgg00_Surrogate     a4next_user;
    tgg00_Surrogate     a4owner_id;
    tsp00_Buf           a4data;
    tgg00_MessBlock     a4mblock;
    tgg07_StrategyInfo  a4strategy;
    tgg00_SelectParamPtr a4sel;
    tgg07_select_context_ptr a4sel_param;
    tgg07_get_param_ptr a4get_param;
    tsp00_Int2          a4long_desc_len;
    tgg00_DataPartPtr   a4res_buf_ptr;
    tsp00_CodeType      a4mess_code;
    tak40_show_record  *a4row_ptr;
    tak_acv_address     a4acv_ptr;
    pasbool             a4all_objects;
    pasbool             a4roles;
    tsp00_Int2          a4role_cnt;
    ARRAY_VAR_MSP00 (         a4role_ids, tgg00_Surrogate, 8);
    tak40_int2_arr_ptr  a4aux_buf_ptr;
} tak40_show_glob;
typedef tak40_show_glob           *tak40_show_glob_ptr;


#endif
