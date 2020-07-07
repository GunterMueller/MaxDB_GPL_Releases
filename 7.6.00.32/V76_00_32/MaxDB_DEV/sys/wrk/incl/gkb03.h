
/* some common prolog for every module
 */

#ifndef GKB03_H
#define GKB03_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

#ifndef GSP00_H
#include "gsp00.h"
#endif

#ifndef GGG00_H
#include "ggg00.h"
#endif

#ifndef GGG04_H
#include "ggg04.h"
#endif

#ifndef GKB00_H
#include "gkb00.h"
#endif

#ifndef VSP0031_H
#include "vsp0031.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

#define ckb3_extra_block                        0
#define ckb3_log_devsp_no                       1
#define ckb3_max_statics                        2
#define ckb3_min_pages_per_block                4
#define ckb3_default_block_count                8
#define ckb3_max_bool_list                    128
#define ckb3_max_data_volumes                4096    /* power(2,12) see cserv.pcf */
#define ckb3_max_tasks                 (ckb3_max_data_volumes * 2)
#define ckb3_nil_devsp_no                       0
#define ckb3_nil_tape_no                        0
#define ckb3_nil_task_no                        0
#define ckb3_region_normal                      1
#define ckb3_region_autosave                    2
#define ckb3_swap_check_no                      1
    /* */
    /* layout of first (info) block */
    /* */
#define ckb3_blockpos_hostinfo                  1    /* first  page of block       */
#define ckb3_blockpos_restartrec                2    /* second page of block;      */
#define ckb3_blockpos_xp_page                   3    /* third page of block;       */
    /* */
#define mxkb3_info_page_body           (PAGE_MXSP00 - PAGE_HEADER_MXGG00 - INT4_MXSP00 - PAGE_TRAILER_MXGG00)
    /* */
#define mxkb3_xparam_page_body         (PAGE_MXSP00 - PAGE_HEADER_MXGG00 - INT4_MXSP00 - INT4_MXSP00 - INT4_MXSP00 - PAGE_TRAILER_MXGG00)

/******************************************
 *               T Y P E S                *
 ******************************************/


typedef enum tkb3_all_states_Enum
{
    sy_allocated,
    sy_bup_working,
    sy_configured,
    sy_error,
    sy_initialized,
    sy_knockoffwork,
    sy_filler6,
    sy_filler7
} tkb3_all_states_Enum;
typedef tkb3_all_states_Enum tkb3_all_states_Param;
typedef ENUM_TYPE_MSP00 (          tkb3_all_states, tkb3_all_states_Enum, sy_filler7, unsigned char);
    /* */
typedef SMALLSET_TYPE_MSP00 (      tkb3_system_state, tkb3_all_states_Enum, sy_filler7);
    /* */

typedef enum tkb3_answer_id_Enum
{
    ans_okay,
    ans_task_ready,
    ans_read_finish,
    ans_error,
    ans_tape_full
} tkb3_answer_id_Enum;
typedef tkb3_answer_id_Enum tkb3_answer_id_Param;
typedef ENUM_TYPE_MSP00 (          tkb3_answer_id, tkb3_answer_id_Enum, ans_tape_full, unsigned char);
    /* */

typedef enum tkb3_savetasks_Enum
{
    task_nil,
    task_drt,
    task_twt,
    task_dwt,
    task_trt
} tkb3_savetasks_Enum;
typedef tkb3_savetasks_Enum tkb3_savetasks_Param;
typedef ENUM_TYPE_MSP00 (          tkb3_savetasks, tkb3_savetasks_Enum, task_trt, unsigned char);
    /* */

typedef enum tkb3_task_state_Enum
{
    ts_none,
    ts_started,
    ts_running,
    ts_stopped,
    ts_waiting
} tkb3_task_state_Enum;
typedef tkb3_task_state_Enum tkb3_task_state_Param;
typedef ENUM_TYPE_MSP00 (          tkb3_task_state, tkb3_task_state_Enum, ts_waiting, unsigned char);
    /* */

typedef enum tkb3_sre_state_Enum
{
    sres_free,
    sres_contains_pages,
    sres_reading_writing
} tkb3_sre_state_Enum;
typedef tkb3_sre_state_Enum tkb3_sre_state_Param;
typedef ENUM_TYPE_MSP00 (          tkb3_sre_state, tkb3_sre_state_Enum, sres_reading_writing, unsigned char);
    /* */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tkb3_info_page_body, mxkb3_info_page_body);
typedef CSTRING_TYPE_MSP00      (  tkb3_info_page_bodyc, mxkb3_info_page_body);
    /* */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tkb3_xparam_page_body, mxkb3_xparam_page_body);
typedef CSTRING_TYPE_MSP00      (  tkb3_xparam_page_bodyc, mxkb3_xparam_page_body);

typedef struct                     tkb3_info_stuff
{
    tsp00_PageNo        inf_pno;
    tgg00_PageType      inf_pt;
    tgg00_PageType2     inf_pt2;
    tgg00_PageCheckType inf_check;
    pasbool             inf_filler1;
    tgg00_MessType      inf_mess_type;
    tgg00_MessType2     inf_mess2_type;
    tsp00_Int2          inf_volume_no;
    tsp00_Int4          inf_swap_check;
    tsp00_Int4          inf_file_version;
    tsp00_Int2          inf_save_blocksize;
    tsp00_Int2          inf_max_volume_no;
    tsp00_Int4          inf_page_count;
    tsp00_Int4          inf_bd_page_count;
    tsp00_Int4          inf_vol_page_cnt;
    tsp00_Int4          inf_tapes_used;
    tsp00_Version       inf_curr_knlvers;
    tsp00_Version       inf_knlvers;
    tsp00_DbName        inf_serverdb;
    tsp00_NodeId        inf_servernode;
    pasbool             inf_is_consistent;
    pasbool             inf_is_clustered;
    tsp00_Uint4         inf_first_iosequence;
    tsp00_Uint4         inf_last_iosequence;
    tsp00_Date          inf_start_date;
    tsp00_Time          inf_start_time;
    tsp00_Date          inf_end_date;
    tsp00_Time          inf_end_time;
    tsp00_Date          inf_dbstamp1_date;
    tsp00_Time          inf_dbstamp1_time;
    tsp00_Date          inf_dbstamp2_date;
    tsp00_Time          inf_dbstamp2_time;
    tsp00_C14           inf_label;
    tsp00_C2            inf_filler3;
    tsp00_Line          inf_db_ident;
    tsp00_Int4          inf_max_used_data_pno;
    tsp00_Int4          inf_conv_page_count;
} tkb3_info_stuff;

typedef struct                     tkb3_info_page
{
    union
    {
    /* 1 */
        struct tkb3_info_page_1
        {
            tkb3_info_stuff     the_info_F;
        } C_1;
    /* 2 */
        struct tkb3_info_page_2
        {
            tgg00_PageHeader    inf_header_F;
            tkb3_info_page_body inf_body_F;
            tsp00_Int4          inf_checksum_F;
            tgg00_PageTrailer   inf_trailer_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tkb3_info_stuff    &the_info               ()  { return this->variant.C_1.the_info_F; };
    tgg00_PageHeader   &inf_header             ()  { return this->variant.C_2.inf_header_F; };
    tkb3_info_page_body &inf_body               ()  { return this->variant.C_2.inf_body_F; };
    tsp00_Int4         &inf_checksum           ()  { return this->variant.C_2.inf_checksum_F; };
    tgg00_PageTrailer  &inf_trailer            ()  { return this->variant.C_2.inf_trailer_F; };
    /* const accessor functions */
    const tkb3_info_stuff &the_info               () const  { return this->variant.C_1.the_info_F; };
    const tgg00_PageHeader &inf_header             () const  { return this->variant.C_2.inf_header_F; };
    const tkb3_info_page_body &inf_body               () const  { return this->variant.C_2.inf_body_F; };
    const tsp00_Int4   &inf_checksum           () const  { return this->variant.C_2.inf_checksum_F; };
    const tgg00_PageTrailer &inf_trailer            () const  { return this->variant.C_2.inf_trailer_F; };
#endif    /* defined (__cplusplus) */
} tkb3_info_page;
typedef tkb3_info_page            *tkb3_info_ptr;

typedef struct                     tkb3_xparam_page
{
    union
    {
    /* 1 */
        struct tkb3_xparam_page_1
        {
            tsp00_PageNo        xp_pno_F;
            tgg00_PageType      xp_pt_F;
            tgg00_PageType2     xp_pt2_F;
            tgg00_PageCheckType xp_check_F;
            pasbool             xp_filler1_F;
        } C_1;
    /* 2 */
        struct tkb3_xparam_page_2
        {
            tgg00_PageHeader    xp_header_F;
            tsp00_Int4          xp_length_F;
            tsp00_Int4          xp_total_length_F;
            tkb3_xparam_page_body xp_data_F;
            tsp00_Int4          xp_checksum_F;
            tgg00_PageTrailer   xp_trailer_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_PageNo       &xp_pno                 ()  { return this->variant.C_1.xp_pno_F; };
    tgg00_PageType     &xp_pt                  ()  { return this->variant.C_1.xp_pt_F; };
    tgg00_PageType2    &xp_pt2                 ()  { return this->variant.C_1.xp_pt2_F; };
    tgg00_PageCheckType &xp_check               ()  { return this->variant.C_1.xp_check_F; };
    pasbool            &xp_filler1             ()  { return this->variant.C_1.xp_filler1_F; };
    tgg00_PageHeader   &xp_header              ()  { return this->variant.C_2.xp_header_F; };
    tsp00_Int4         &xp_length              ()  { return this->variant.C_2.xp_length_F; };
    tsp00_Int4         &xp_total_length        ()  { return this->variant.C_2.xp_total_length_F; };
    tkb3_xparam_page_body &xp_data                ()  { return this->variant.C_2.xp_data_F; };
    tsp00_Int4         &xp_checksum            ()  { return this->variant.C_2.xp_checksum_F; };
    tgg00_PageTrailer  &xp_trailer             ()  { return this->variant.C_2.xp_trailer_F; };
    /* const accessor functions */
    const tsp00_PageNo &xp_pno                 () const  { return this->variant.C_1.xp_pno_F; };
    const tgg00_PageType &xp_pt                  () const  { return this->variant.C_1.xp_pt_F; };
    const tgg00_PageType2 &xp_pt2                 () const  { return this->variant.C_1.xp_pt2_F; };
    const tgg00_PageCheckType &xp_check               () const  { return this->variant.C_1.xp_check_F; };
    const pasbool      &xp_filler1             () const  { return this->variant.C_1.xp_filler1_F; };
    const tgg00_PageHeader &xp_header              () const  { return this->variant.C_2.xp_header_F; };
    const tsp00_Int4   &xp_length              () const  { return this->variant.C_2.xp_length_F; };
    const tsp00_Int4   &xp_total_length        () const  { return this->variant.C_2.xp_total_length_F; };
    const tkb3_xparam_page_body &xp_data                () const  { return this->variant.C_2.xp_data_F; };
    const tsp00_Int4   &xp_checksum            () const  { return this->variant.C_2.xp_checksum_F; };
    const tgg00_PageTrailer &xp_trailer             () const  { return this->variant.C_2.xp_trailer_F; };
#endif    /* defined (__cplusplus) */
} tkb3_xparam_page;

typedef struct                     tkb3_task_desc
{
    tsp00_TaskId        tsd_task_pid;
    tsp00_Int2          tsd_filler2;
    tsp00_Int2          tsd_tape_index;
    tkb3_task_state     tsd_state;
    tkb3_savetasks      tsd_task_kind;
    tsp00_C6            tsd_filler;
} tkb3_task_desc;
typedef ARRAY_TYPE_MSP00 (         tkb3_task_list, tkb3_task_desc, ckb3_max_tasks);
    /* */
typedef tkb3_task_list            *tkb3_task_list_ptr;

typedef struct                     tkb3_tape_desc
{
    pasbool             tpd_is_open;
    pasbool             tpd_is_full;
    tsp00_Int2          tpd_volume_no;
    tsp00_Int4          tpd_fno;
    tsp00_Int4          tpd_max_pages;
    tsp00_Int4          tpd_cnt_pages;
    tsp00_Int4          tpd_xp_size;
    tsp00_Int4          tpd_xp_read;
    tsp00_Int4          tpd_xp_pages;
    tsp00_VfType        tpd_type;
    pasbool             tpd_check_destructive;
    tsp00_Int2          tpd_mirror_index;
    tsp00_Int4          tpd_total_cnt_pages;
    tgg00_BasisError    tpd_err;
    pasbool             tpd_is_replaced;
    pasbool             tpd_is_clustered;
    tsp00_VFilename     tpd_name;
    tsp00_ErrText       tpd_errtext;
} tkb3_tape_desc;
typedef ARRAY_TYPE_MSP00 (         tkb3_tape_list, tkb3_tape_desc, MAX_TAPES_GG00);
    /* */
typedef tkb3_tape_list            *tkb3_tape_list_ptr;
    /* volume description: array of used volumes without deconfigured volumes
         dvd_volno references real volume number */

typedef struct                     tkb3_devsp_desc
{
    pasbool             dvd_is_open;
    pasbool             dvd_filler1;
    tsp00_Int2          dvd_volno;
    tsp00_Int4          dvd_fno;
} tkb3_devsp_desc;
typedef ARRAY_TYPE_MSP00 (         tkb3_devsp_list, tkb3_devsp_desc, ckb3_max_data_volumes);
    /* */
typedef tkb3_devsp_list           *tkb3_devsp_list_ptr;
    /* mapping from real volume number to volume index of tkb3_devsp_list */
typedef ARRAY_TYPE_MSP00 (         tkb3_volume_index, tsp00_Int2, ckb3_max_data_volumes);
    /* */
typedef tkb3_volume_index         *tkb3_volume_index_ptr;
    /* */
typedef ARRAY_TYPE_MSP00 (         tkb3_bool_list, pasbool, ckb3_max_bool_list);
    /* */
typedef ARRAY_TYPE_MSP00 (         tkb3_into_queue_list, tsp00_Int4, MAX_TAPES_GG00);

typedef struct                     tkb3_page
{
    union
    {
    /* 1 */
        struct tkb3_page_1
        {
            tsp00_Page          the_page_F;
        } C_1;
    /* 2 */
        struct tkb3_page_2
        {
            tsp00_PageNo        the_pno_F;
            tgg00_PageType      the_pt_F;
            tgg00_PageType2     the_pt2_F;
            tgg00_PageCheckType the_check_F;
        } C_2;
    /* 3 */
        struct tkb3_page_3
        {
            tkb3_info_page      info_page_F;
        } C_3;
    /* 4 */
        struct tkb3_page_4
        {
            tkb3_xparam_page    xp_page_F;
        } C_4;
    /* 5 */
        struct tkb3_page_5
        {
            tkb00_Page          univ_page_F;
        } C_5;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Page         &the_page               ()  { return this->variant.C_1.the_page_F; };
    tsp00_PageNo       &the_pno                ()  { return this->variant.C_2.the_pno_F; };
    tgg00_PageType     &the_pt                 ()  { return this->variant.C_2.the_pt_F; };
    tgg00_PageType2    &the_pt2                ()  { return this->variant.C_2.the_pt2_F; };
    tgg00_PageCheckType &the_check              ()  { return this->variant.C_2.the_check_F; };
    tkb3_info_page     &info_page              ()  { return this->variant.C_3.info_page_F; };
    tkb3_xparam_page   &xp_page                ()  { return this->variant.C_4.xp_page_F; };
    tkb00_Page         &univ_page              ()  { return this->variant.C_5.univ_page_F; };
    /* const accessor functions */
    const tsp00_Page   &the_page               () const  { return this->variant.C_1.the_page_F; };
    const tsp00_PageNo &the_pno                () const  { return this->variant.C_2.the_pno_F; };
    const tgg00_PageType &the_pt                 () const  { return this->variant.C_2.the_pt_F; };
    const tgg00_PageType2 &the_pt2                () const  { return this->variant.C_2.the_pt2_F; };
    const tgg00_PageCheckType &the_check              () const  { return this->variant.C_2.the_check_F; };
    const tkb3_info_page &info_page              () const  { return this->variant.C_3.info_page_F; };
    const tkb3_xparam_page &xp_page                () const  { return this->variant.C_4.xp_page_F; };
    const tkb00_Page   &univ_page              () const  { return this->variant.C_5.univ_page_F; };
#endif    /* defined (__cplusplus) */
} tkb3_page;
typedef tkb3_page                 *tkb3_page_ptr;
typedef ARRAY_TYPE_MSP00 (         tkb3_block, tkb3_page, 1000);
    /* */
typedef tkb3_block                *tkb3_block_ptr;

typedef struct                     tkb3_srr_elem
{
    tkb3_sre_state      sre_state;
    pasbool             sre_is_clustered;
    tsp00_Int2          sre_pages_per_block;
    tsp00_Int2          sre_next;
    tsp00_Int2          sre_prev;
    tsp00_Int2          sre_task_no_into;
    tsp00_Int2          sre_filler2;
    tsp00_Int4          sre_filler3;
    union
    {
    /* 1 */
        struct tkb3_srr_elem_1
        {
            tkb3_block_ptr      sre_block_F;
        } C_1;
    /* 2 */
        struct tkb3_srr_elem_2
        {
            tsp00_C8            sre_addr_space_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tkb3_block_ptr     &sre_block              ()  { return this->variant.C_1.sre_block_F; };
    tsp00_C8           &sre_addr_space         ()  { return this->variant.C_2.sre_addr_space_F; };
    /* const accessor functions */
    const tkb3_block_ptr &sre_block              () const  { return this->variant.C_1.sre_block_F; };
    const tsp00_C8     &sre_addr_space         () const  { return this->variant.C_2.sre_addr_space_F; };
#endif    /* defined (__cplusplus) */
} tkb3_srr_elem;
typedef ARRAY_TYPE_MSP00 (         tkb3_srr_array, tkb3_srr_elem, 1000 - 0 + 1);
    /*   element '0' is for writing info, config and restart pages */
    /* */
typedef tkb3_srr_array            *tkb3_srr_ptr;

typedef struct                     tkb3_remote_channel
{
    tsp00_Int4          kb3rc_size;
    tsp00_Int4          kb3rc_length;
    tsp00_TaskId        kb3rc_pid;
    tsp00_TaskId        kb3rc_filler1;
    union
    {
    /* true */
        struct tkb3_remote_channel_true
        {
            tsp00_MoveObjPtr    kb3rc_buffer_F;
        } C_true;
    /* false */
        struct tkb3_remote_channel_false
        {
            tsp00_C8            kb3rc_filler2_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_MoveObjPtr   &kb3rc_buffer           ()  { return this->variant.C_true.kb3rc_buffer_F; };
    tsp00_C8           &kb3rc_filler2          ()  { return this->variant.C_false.kb3rc_filler2_F; };
    /* const accessor functions */
    const tsp00_MoveObjPtr &kb3rc_buffer           () const  { return this->variant.C_true.kb3rc_buffer_F; };
    const tsp00_C8     &kb3rc_filler2          () const  { return this->variant.C_false.kb3rc_filler2_F; };
#endif    /* defined (__cplusplus) */
} tkb3_remote_channel;
typedef tkb3_remote_channel       *tkb3_remote_channel_ptr;
typedef ARRAY_TYPE_MSP00 (         tkb3_remote_io, tkb3_remote_channel, MAX_TAPES_GG00);

typedef struct                     tkb3_save_static
{
    tsp00_Int4          sta_pages_transferred;
    tsp00_Int4          sta_into_count;
    tsp00_Int4          sta_out_count;
    tsp00_Int4          sta_queue_into_count;
    tsp00_Int4          sta_queue_out_count;
    tsp00_Int2          sta_blocksize;
    tsp00_Int2          sta_max_tasks;
    tsp00_Int2          sta_num_devsp;
    tsp00_Int2          sta_devsp_ready_cnt;
    tsp00_Int2          sta_num_tapes;
    tsp00_Int2          sta_tapes_ready_cnt;
    tsp00_Int2          sta_tapes_start_cnt;
    tsp00_Int2          sta_conttape;
    tsp00_Int2          sta_task_for_conttape;
    tsp00_Int2          sta_queue_size;
    tsp00_Int2          sta_queue_first_free;
    tsp00_Int2          sta_queue_first_read;
    tsp00_Int2          sta_queue_last_read;
    tsp00_Int2          sta_read_tasks_waiting;
    tsp00_Int2          sta_write_tasks_waiting;
    tkb3_system_state   sta_system_state;
    pasbool             sta_is_auto_load;
    pasbool             sta_is_cold;
    tgg00_MessType      sta_msgtype;
    tgg00_MessType2     sta_msg2type;
    tgg00_VtraceType    sta_vtr_direction;
    tsp00_Date          sta_until_date;
    tsp00_Time          sta_until_time;
    tsp00_Uint4         sta_until_iosequence;
    tsp00_Int4          sta_region;
    tsp00_TaskId        sta_wait_pid;
    pasbool             sta_try_again;
    pasbool             sta_check_save;
    pasbool             sta_write_bitmap_pages;
    pasbool             sta_write_conv_pages;
    tsp00_Int4          sta_file_version;
    tsp00_Int2          sta_locked_queue_index;
    pasbool             sta_is_remote;
    pasbool             sta_lwt_perform_redo;
    tsp00_PageNo        sta_last_lpno_wanted;
    tsp00_TaskId        sta_trt_eof_wait_pid;
    tgg00_MediaName     sta_media_name;
    tsp00_C14           sta_tape_label;
    pasbool             sta_no_release_log;
    pasbool             sta_filler1;
    tsp00_TaskId        sta_coordinator_taskid;
    tsp00_Int4          sta_filler2;
    tgg00_UtilCmdId     sta_utilcmd_id;
    tkb3_bool_list      sta_volume_started;
    tkb3_into_queue_list sta_trt_into_queue;
    tkb3_remote_io      sta_remote;
    tkb3_info_stuff     sta_info;
    union
    {
    /* 1 */
        struct tkb3_save_static_1
        {
            tkb3_task_list_ptr  sta_task_F;
        } C_1;
    /* 2 */
        struct tkb3_save_static_2
        {
            tsp00_C8            sta_adr_space1_F;
            tkb3_tape_list_ptr  sta_tape_F;
        } C_2;
    /* 3 */
        struct tkb3_save_static_3
        {
            tsp00_C8            sta_adr_space2_F;
            tsp00_C8            sta_addr_space3_F;
            tkb3_devsp_list_ptr sta_devsp_F;
        } C_3;
    /* 4 */
        struct tkb3_save_static_4
        {
            tsp00_C8            sta_adr_space4_F;
            tsp00_C8            sta_addr_space5_F;
            tsp00_C8            sta_addr_space6_F;
            tkb00_PagePtr       sta_data_rst_rec_F;
        } C_4;
    /* 5 */
        struct tkb3_save_static_5
        {
            tsp00_C8            sta_adr_space7_F;
            tsp00_C8            sta_addr_space8_F;
            tsp00_C8            sta_addr_space9_F;
            tsp00_C8            sta_addr_spac10_F;
            tkb3_srr_ptr        sta_queue_F;
        } C_5;
    /* 6 */
        struct tkb3_save_static_6
        {
            tsp00_C8            sta_adr_spac11_F;
            tsp00_C8            sta_addr_spac12_F;
            tsp00_C8            sta_addr_spac13_F;
            tsp00_C8            sta_addr_spac14_F;
            tsp00_C8            sta_addr_spac15_F;
            tsp00_BoolAddr      sta_to_cancel_F;
        } C_6;
    /* 7 */
        struct tkb3_save_static_7
        {
            tsp00_C8            sta_adr_spac16_F;
            tsp00_C8            sta_addr_spac17_F;
            tsp00_C8            sta_addr_spac18_F;
            tsp00_C8            sta_addr_spac19_F;
            tsp00_C8            sta_addr_spac20_F;
            tsp00_C8            sta_addr_spac21_F;
            tsp00_Addr          sta_backup_server_F;
        } C_7;
    /* 8 */
        struct tkb3_save_static_8
        {
            tsp00_C8            sta_adr_spac22_F;
            tsp00_C8            sta_addr_spac23_F;
            tsp00_C8            sta_addr_spac24_F;
            tsp00_C8            sta_addr_spac26_F;
            tsp00_C8            sta_addr_spac27_F;
            tsp00_C8            sta_addr_spac28_F;
            tsp00_C8            sta_addr_spac29_F;
            tkb3_volume_index_ptr sta_vol_idx_F;
        } C_8;
    /* 9 */
        struct tkb3_save_static_9
        {
            tsp00_C8            sta_adr_spac30_F;
            tsp00_C8            sta_addr_spac31_F;
            tsp00_C8            sta_addr_spac32_F;
            tsp00_C8            sta_addr_spac33_F;
            tsp00_C8            sta_addr_spac34_F;
            tsp00_C8            sta_addr_spac35_F;
            tsp00_C8            sta_addr_spac36_F;
            tsp00_C8            sta_addr_spac37_F;
        } C_9;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tkb3_task_list_ptr &sta_task               ()  { return this->variant.C_1.sta_task_F; };
    tsp00_C8           &sta_adr_space1         ()  { return this->variant.C_2.sta_adr_space1_F; };
    tkb3_tape_list_ptr &sta_tape               ()  { return this->variant.C_2.sta_tape_F; };
    tsp00_C8           &sta_adr_space2         ()  { return this->variant.C_3.sta_adr_space2_F; };
    tsp00_C8           &sta_addr_space3        ()  { return this->variant.C_3.sta_addr_space3_F; };
    tkb3_devsp_list_ptr &sta_devsp              ()  { return this->variant.C_3.sta_devsp_F; };
    tsp00_C8           &sta_adr_space4         ()  { return this->variant.C_4.sta_adr_space4_F; };
    tsp00_C8           &sta_addr_space5        ()  { return this->variant.C_4.sta_addr_space5_F; };
    tsp00_C8           &sta_addr_space6        ()  { return this->variant.C_4.sta_addr_space6_F; };
    tkb00_PagePtr      &sta_data_rst_rec       ()  { return this->variant.C_4.sta_data_rst_rec_F; };
    tsp00_C8           &sta_adr_space7         ()  { return this->variant.C_5.sta_adr_space7_F; };
    tsp00_C8           &sta_addr_space8        ()  { return this->variant.C_5.sta_addr_space8_F; };
    tsp00_C8           &sta_addr_space9        ()  { return this->variant.C_5.sta_addr_space9_F; };
    tsp00_C8           &sta_addr_spac10        ()  { return this->variant.C_5.sta_addr_spac10_F; };
    tkb3_srr_ptr       &sta_queue              ()  { return this->variant.C_5.sta_queue_F; };
    tsp00_C8           &sta_adr_spac11         ()  { return this->variant.C_6.sta_adr_spac11_F; };
    tsp00_C8           &sta_addr_spac12        ()  { return this->variant.C_6.sta_addr_spac12_F; };
    tsp00_C8           &sta_addr_spac13        ()  { return this->variant.C_6.sta_addr_spac13_F; };
    tsp00_C8           &sta_addr_spac14        ()  { return this->variant.C_6.sta_addr_spac14_F; };
    tsp00_C8           &sta_addr_spac15        ()  { return this->variant.C_6.sta_addr_spac15_F; };
    tsp00_BoolAddr     &sta_to_cancel          ()  { return this->variant.C_6.sta_to_cancel_F; };
    tsp00_C8           &sta_adr_spac16         ()  { return this->variant.C_7.sta_adr_spac16_F; };
    tsp00_C8           &sta_addr_spac17        ()  { return this->variant.C_7.sta_addr_spac17_F; };
    tsp00_C8           &sta_addr_spac18        ()  { return this->variant.C_7.sta_addr_spac18_F; };
    tsp00_C8           &sta_addr_spac19        ()  { return this->variant.C_7.sta_addr_spac19_F; };
    tsp00_C8           &sta_addr_spac20        ()  { return this->variant.C_7.sta_addr_spac20_F; };
    tsp00_C8           &sta_addr_spac21        ()  { return this->variant.C_7.sta_addr_spac21_F; };
    tsp00_Addr         &sta_backup_server      ()  { return this->variant.C_7.sta_backup_server_F; };
    tsp00_C8           &sta_adr_spac22         ()  { return this->variant.C_8.sta_adr_spac22_F; };
    tsp00_C8           &sta_addr_spac23        ()  { return this->variant.C_8.sta_addr_spac23_F; };
    tsp00_C8           &sta_addr_spac24        ()  { return this->variant.C_8.sta_addr_spac24_F; };
    tsp00_C8           &sta_addr_spac26        ()  { return this->variant.C_8.sta_addr_spac26_F; };
    tsp00_C8           &sta_addr_spac27        ()  { return this->variant.C_8.sta_addr_spac27_F; };
    tsp00_C8           &sta_addr_spac28        ()  { return this->variant.C_8.sta_addr_spac28_F; };
    tsp00_C8           &sta_addr_spac29        ()  { return this->variant.C_8.sta_addr_spac29_F; };
    tkb3_volume_index_ptr &sta_vol_idx            ()  { return this->variant.C_8.sta_vol_idx_F; };
    tsp00_C8           &sta_adr_spac30         ()  { return this->variant.C_9.sta_adr_spac30_F; };
    tsp00_C8           &sta_addr_spac31        ()  { return this->variant.C_9.sta_addr_spac31_F; };
    tsp00_C8           &sta_addr_spac32        ()  { return this->variant.C_9.sta_addr_spac32_F; };
    tsp00_C8           &sta_addr_spac33        ()  { return this->variant.C_9.sta_addr_spac33_F; };
    tsp00_C8           &sta_addr_spac34        ()  { return this->variant.C_9.sta_addr_spac34_F; };
    tsp00_C8           &sta_addr_spac35        ()  { return this->variant.C_9.sta_addr_spac35_F; };
    tsp00_C8           &sta_addr_spac36        ()  { return this->variant.C_9.sta_addr_spac36_F; };
    tsp00_C8           &sta_addr_spac37        ()  { return this->variant.C_9.sta_addr_spac37_F; };
    /* const accessor functions */
    const tkb3_task_list_ptr &sta_task               () const  { return this->variant.C_1.sta_task_F; };
    const tsp00_C8     &sta_adr_space1         () const  { return this->variant.C_2.sta_adr_space1_F; };
    const tkb3_tape_list_ptr &sta_tape               () const  { return this->variant.C_2.sta_tape_F; };
    const tsp00_C8     &sta_adr_space2         () const  { return this->variant.C_3.sta_adr_space2_F; };
    const tsp00_C8     &sta_addr_space3        () const  { return this->variant.C_3.sta_addr_space3_F; };
    const tkb3_devsp_list_ptr &sta_devsp              () const  { return this->variant.C_3.sta_devsp_F; };
    const tsp00_C8     &sta_adr_space4         () const  { return this->variant.C_4.sta_adr_space4_F; };
    const tsp00_C8     &sta_addr_space5        () const  { return this->variant.C_4.sta_addr_space5_F; };
    const tsp00_C8     &sta_addr_space6        () const  { return this->variant.C_4.sta_addr_space6_F; };
    const tkb00_PagePtr &sta_data_rst_rec       () const  { return this->variant.C_4.sta_data_rst_rec_F; };
    const tsp00_C8     &sta_adr_space7         () const  { return this->variant.C_5.sta_adr_space7_F; };
    const tsp00_C8     &sta_addr_space8        () const  { return this->variant.C_5.sta_addr_space8_F; };
    const tsp00_C8     &sta_addr_space9        () const  { return this->variant.C_5.sta_addr_space9_F; };
    const tsp00_C8     &sta_addr_spac10        () const  { return this->variant.C_5.sta_addr_spac10_F; };
    const tkb3_srr_ptr &sta_queue              () const  { return this->variant.C_5.sta_queue_F; };
    const tsp00_C8     &sta_adr_spac11         () const  { return this->variant.C_6.sta_adr_spac11_F; };
    const tsp00_C8     &sta_addr_spac12        () const  { return this->variant.C_6.sta_addr_spac12_F; };
    const tsp00_C8     &sta_addr_spac13        () const  { return this->variant.C_6.sta_addr_spac13_F; };
    const tsp00_C8     &sta_addr_spac14        () const  { return this->variant.C_6.sta_addr_spac14_F; };
    const tsp00_C8     &sta_addr_spac15        () const  { return this->variant.C_6.sta_addr_spac15_F; };
    const tsp00_BoolAddr &sta_to_cancel          () const  { return this->variant.C_6.sta_to_cancel_F; };
    const tsp00_C8     &sta_adr_spac16         () const  { return this->variant.C_7.sta_adr_spac16_F; };
    const tsp00_C8     &sta_addr_spac17        () const  { return this->variant.C_7.sta_addr_spac17_F; };
    const tsp00_C8     &sta_addr_spac18        () const  { return this->variant.C_7.sta_addr_spac18_F; };
    const tsp00_C8     &sta_addr_spac19        () const  { return this->variant.C_7.sta_addr_spac19_F; };
    const tsp00_C8     &sta_addr_spac20        () const  { return this->variant.C_7.sta_addr_spac20_F; };
    const tsp00_C8     &sta_addr_spac21        () const  { return this->variant.C_7.sta_addr_spac21_F; };
    const tsp00_Addr   &sta_backup_server      () const  { return this->variant.C_7.sta_backup_server_F; };
    const tsp00_C8     &sta_adr_spac22         () const  { return this->variant.C_8.sta_adr_spac22_F; };
    const tsp00_C8     &sta_addr_spac23        () const  { return this->variant.C_8.sta_addr_spac23_F; };
    const tsp00_C8     &sta_addr_spac24        () const  { return this->variant.C_8.sta_addr_spac24_F; };
    const tsp00_C8     &sta_addr_spac26        () const  { return this->variant.C_8.sta_addr_spac26_F; };
    const tsp00_C8     &sta_addr_spac27        () const  { return this->variant.C_8.sta_addr_spac27_F; };
    const tsp00_C8     &sta_addr_spac28        () const  { return this->variant.C_8.sta_addr_spac28_F; };
    const tsp00_C8     &sta_addr_spac29        () const  { return this->variant.C_8.sta_addr_spac29_F; };
    const tkb3_volume_index_ptr &sta_vol_idx            () const  { return this->variant.C_8.sta_vol_idx_F; };
    const tsp00_C8     &sta_adr_spac30         () const  { return this->variant.C_9.sta_adr_spac30_F; };
    const tsp00_C8     &sta_addr_spac31        () const  { return this->variant.C_9.sta_addr_spac31_F; };
    const tsp00_C8     &sta_addr_spac32        () const  { return this->variant.C_9.sta_addr_spac32_F; };
    const tsp00_C8     &sta_addr_spac33        () const  { return this->variant.C_9.sta_addr_spac33_F; };
    const tsp00_C8     &sta_addr_spac34        () const  { return this->variant.C_9.sta_addr_spac34_F; };
    const tsp00_C8     &sta_addr_spac35        () const  { return this->variant.C_9.sta_addr_spac35_F; };
    const tsp00_C8     &sta_addr_spac36        () const  { return this->variant.C_9.sta_addr_spac36_F; };
    const tsp00_C8     &sta_addr_spac37        () const  { return this->variant.C_9.sta_addr_spac37_F; };
#endif    /* defined (__cplusplus) */
} tkb3_save_static;
typedef ARRAY_TYPE_MSP00 (         tkb3_static_array, tkb3_save_static, ckb3_max_statics);
typedef tkb3_save_static          *tkb3_save_static_ptr;

typedef struct                     tkb3_backup_result
{
    tsp00_C1            bdef_date;
    tsp00_Date          bcol_date;
    tsp00_C2            bcol_extdate_filler1;
    tsp00_C1            bdef_time;
    tsp00_Time          bcol_time;
    tsp00_C1            bdef_serverdb;
    tsp00_DbName        bcol_serverdb;
    tsp00_C1            bdef_servernode;
    tsp00_NodeId        bcol_servernode;
    tsp00_C1            bdef_kernel_version;
    tsp00_Version       bcol_kernel_version;
    tsp00_ResNum        bcol_pages_transferred;
    tsp00_ResNum        bcol_pages_left;
    tsp00_ResNum        bcol_no_of_volumes;
    tsp00_C1            bdef_media_name;
    tsp00_C64           bcol_media_name;
    tsp00_C1            bdef_tape_name;
    tsp00_VFilename     bcol_tape_name;
    tsp00_C1            bdef_tape_errortext;
    tsp00_ErrText       bcol_tape_errortext;
    tsp00_C1            bdef_tape_label;
    tsp00_C14           bcol_tape_label;
    tsp00_C1            bdef_is_consistent;
    char                bcol_is_consistent;
    tsp00_ResNum        bcol_first_iosequence;
    tsp00_ResNum        bcol_last_iosequence;
    tsp00_C1            bdef_dbstamp1_date;
    tsp00_Date          bcol_dbstamp1_date;
    tsp00_C2            bcol_extdate_filler2;
    tsp00_C1            bdef_dbstamp1_time;
    tsp00_Time          bcol_dbstamp1_time;
    tsp00_C1            bdef_dbstamp2_date;
    tsp00_Date          bcol_dbstamp2_date;
    tsp00_C2            bcol_extdate_filler3;
    tsp00_C1            bdef_dbstamp2_time;
    tsp00_Time          bcol_dbstamp2_time;
    tsp00_ResNum        bcol_bd_page_count;
    tsp00_ResNum        bcol_tapes_used;
    tsp00_C1            bdef_db_ident;
    tsp00_Line          bcol_db_ident;
    tsp00_ResNum        bcol_max_used_data_pno;
    tsp00_ResNum        bcol_conv_page_count;
} tkb3_backup_result;

typedef struct                     tkb3_univ_addr
{
    union
    {
    /* 1 */
        struct tkb3_univ_addr_1
        {
            tsp00_ObjAddr       obj_F;
        } C_1;
    /* 2 */
        struct tkb3_univ_addr_2
        {
            tkb3_task_list_ptr  task_F;
        } C_2;
    /* 3 */
        struct tkb3_univ_addr_3
        {
            tkb3_tape_list_ptr  tape_F;
        } C_3;
    /* 4 */
        struct tkb3_univ_addr_4
        {
            tkb3_devsp_list_ptr devsp_F;
        } C_4;
    /* 5 */
        struct tkb3_univ_addr_5
        {
            tkb3_volume_index_ptr volidx_F;
        } C_5;
    /* 6 */
        struct tkb3_univ_addr_6
        {
            tkb3_srr_ptr        srr_F;
        } C_6;
    /* 7 */
        struct tkb3_univ_addr_7
        {
            tsp00_PageAddr      buf_F;
        } C_7;
    /* 8 */
        struct tkb3_univ_addr_8
        {
            tkb3_block_ptr      block_F;
        } C_8;
    /* 9 */
        struct tkb3_univ_addr_9
        {
            tkb00_PagePtr       log_F;
        } C_9;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_ObjAddr      &obj                    ()  { return this->variant.C_1.obj_F; };
    tkb3_task_list_ptr &task                   ()  { return this->variant.C_2.task_F; };
    tkb3_tape_list_ptr &tape                   ()  { return this->variant.C_3.tape_F; };
    tkb3_devsp_list_ptr &devsp                  ()  { return this->variant.C_4.devsp_F; };
    tkb3_volume_index_ptr &volidx                 ()  { return this->variant.C_5.volidx_F; };
    tkb3_srr_ptr       &srr                    ()  { return this->variant.C_6.srr_F; };
    tsp00_PageAddr     &buf                    ()  { return this->variant.C_7.buf_F; };
    tkb3_block_ptr     &block                  ()  { return this->variant.C_8.block_F; };
    tkb00_PagePtr      &log                    ()  { return this->variant.C_9.log_F; };
    /* const accessor functions */
    const tsp00_ObjAddr &obj                    () const  { return this->variant.C_1.obj_F; };
    const tkb3_task_list_ptr &task                   () const  { return this->variant.C_2.task_F; };
    const tkb3_tape_list_ptr &tape                   () const  { return this->variant.C_3.tape_F; };
    const tkb3_devsp_list_ptr &devsp                  () const  { return this->variant.C_4.devsp_F; };
    const tkb3_volume_index_ptr &volidx                 () const  { return this->variant.C_5.volidx_F; };
    const tkb3_srr_ptr &srr                    () const  { return this->variant.C_6.srr_F; };
    const tsp00_PageAddr &buf                    () const  { return this->variant.C_7.buf_F; };
    const tkb3_block_ptr &block                  () const  { return this->variant.C_8.block_F; };
    const tkb00_PagePtr &log                    () const  { return this->variant.C_9.log_F; };
#endif    /* defined (__cplusplus) */
} tkb3_univ_addr;

struct tkb03_TemplateDummy {
    tkb3_savetasks       dummy_tkb3_savetasks;
    tkb3_answer_id       dummy_tkb3_answer_id;
    tkb3_all_states      dummy_tkb3_all_states;
    tkb3_task_state      dummy_tkb3_task_state;
    tkb3_sre_state       dummy_tkb3_sre_state;
    tkb3_system_state    dummy_tkb3_system_state;
};


#endif
