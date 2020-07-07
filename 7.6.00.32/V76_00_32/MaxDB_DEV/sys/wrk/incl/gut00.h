
/* some common prolog for every module
 */

#ifndef GUT00_H
#define GUT00_H


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

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

#define cut_start_of_field             '\x01'    /* virt. terminal only (VTA09) */
#define cut_end_of_field               '\x00'    /* virt. terminal only (VTA09) */
#define cut_protected                  '\x00'
#define cut_unprotected                0x80
#define cut_bright_protected           '\x01'
#define cut_bright_unprotected         0x81
#define cut_invisible                  0x82
#define cut_mask_items                         20
#define cut_max_cmd_lines_term                 20
#define cut_max_cmds_term                      20
#define cut_max_files_term                     25
#define cut_max_lines_term                   1500
#define cut_max_param_term                     10
#define cut_max_prime_cnt                     506
#define cut_nil_block_no               (- 1)
#define cut_nil_entry_pno              (- 1)
#define cut_sql_req_failed             (- 8888)
    /* */
#define cut_pfkey_none                 bsp_c8
#define cut_pfkey_help                  "help    "
#define cut_pfkey_hexint                "hex/int "

/******************************************
 *               T Y P E S                *
 ******************************************/


typedef enum tut_diag_scan__Enum
{
    utds_buffer_output,
    utds_page_header,
    utds_full_info,
    utds_bd_full_info,
    utds_node,
    utds_key,
    utds_key_first_last,
    utds_key_buf,
    utds_rec,
    utds_dump_label,
    utds_fdir,
    utds_logentry_header,
    utds_logentry_full_info,
    utds_logentry_buf,
    utds_display,
    utds_minbuf,
    utds_edit,
    utds_double_numbering
} tut_diag_scan__Enum;
typedef tut_diag_scan__Enum tut_diag_scan__Param;
typedef SET_TYPE_MSP00 (           tut_diag_scan, tut_diag_scan__Enum, utds_double_numbering, 4);

typedef struct                     tut_vf_fileref
{
    tsp00_Int4          utvf_handle;
    tsp00_Int4          utvf_filler;
} tut_vf_fileref;
typedef tut_vf_fileref            *tut_fileref_ptr;

typedef struct                     tut_file_pos
{
    tsp00_Int4          utfp_block_no;
    tsp00_Int2          utfp_page_no;
    tsp00_Int2          utfp_pages_per_block;
} tut_file_pos;

typedef enum tut_dev_type_Enum
{
    utdt_nil,
    utdt_data_perm,
    utdt_data_static,
    utdt_log,
    utdt_mirr_log,
    utdt_restart_rec,
    utdt_loginfo,
    utdt_volume_name,
    utdt_magic_rec_file,
    utdt_binary_file
} tut_dev_type_Enum;
typedef tut_dev_type_Enum tut_dev_type_Param;
typedef ENUM_TYPE_MSP00 (          tut_dev_type, tut_dev_type_Enum, utdt_binary_file, unsigned char);

typedef struct                     tut_file
{
    tut_file_pos        utf_pos;
    tut_file_pos        utf_cycle_lwb;
    tut_file_pos        utf_cycle_upb;
    tut_file_pos        utf_first;
    tut_file_pos        utf_last;
    tsp00_Int4          utf_entry_pno;
    tsp00_Int4          utf_entry_pos;
    tsp00_Int4          utf_cycle_lwb_pno;
    tsp00_Int4          utf_cycle_upb_pno;
    tsp00_Int4          utf_last_pno;
    tsp00_Int4          utf_first_offset;
    tsp00_Int4          utf_page_size;
    tsp00_Int4          utf_page_len;
    tsp00_Int4          utf_block_size;
    tsp00_Int4          utf_block_len;
    tsp00_Int4          utf_fhandle;
    tsp00_Int2          utf_filler1;
    pasbool             utf_rewind;
    tut_dev_type        utf_sql_dev_type;
    tsp00_Int2          utf_sql_devno;
    tsp00_Int2          utf_sql_retcode;
    tsp00_Int4          utf_sql_ref;
    tsp00_Addr          utf_sql_packet;
    tut_fileref_ptr     utf_sql_prot;
    tsp00_MoveObjPtr    utf_block;
    tsp00_MoveObjPtr    utf_page;
    tsp00_VFilename     utf_fn;
} tut_file;
typedef ARRAY_TYPE_MSP00 (         tut_c20_array, tsp00_C20, 20);
    /* */

typedef enum tut_debug_Enum
{
    debug_always,
    debug_ut,
    debug_ta
} tut_debug_Enum;
typedef tut_debug_Enum tut_debug_Param;
typedef ENUM_TYPE_MSP00 (          tut_debug, tut_debug_Enum, debug_ta, unsigned char);
    /* */

typedef enum tut_pfkey_Enum
{
    pf_none,
    pf_cancel,
    pf_down,
    pf_end,
    pf_help,
    pf_left,
    pf_next,
    pf_prev,
    pf_print,
    pf_right,
    pf_run,
    pf_up
} tut_pfkey_Enum;
typedef tut_pfkey_Enum tut_pfkey_Param;
typedef ENUM_TYPE_MSP00 (          tut_pfkey, tut_pfkey_Enum, pf_up, unsigned char);

typedef struct                     tut_mask_item
{
    union
    {
    /* 1 */
        struct tut_mask_item_1
        {
            tsp00_C256          in_c256_F;
            tsp00_C20           msg_F;
            tsp00_C2            msg_in_attr_F;
            pasbool             upper_case_F;
            pasbool             code_input_F;
            tsp00_Int2          in_len_F;
        } C_1;
    /* 2 */
        struct tut_mask_item_2
        {
            tsp00_Name          in_name_F;
        } C_2;
    /* 3 */
        struct tut_mask_item_3
        {
            tsp00_DevName       in_devname_F;
        } C_3;
    /* 4 */
        struct tut_mask_item_4
        {
            tsp00_VFilename     in_fn_F;
        } C_4;
    /* 5 */
        struct tut_mask_item_5
        {
            tsp00_C1            in_num1_F;
        } C_5;
    /* 6 */
        struct tut_mask_item_6
        {
            tsp00_C2            in_num2_F;
        } C_6;
    /* 7 */
        struct tut_mask_item_7
        {
            tsp00_C5            in_num5_F;
        } C_7;
    /* 8 */
        struct tut_mask_item_8
        {
            tsp00_C10           in_num10_F;
        } C_8;
    /* 9 */
        struct tut_mask_item_9
        {
            tsp00_KnlIdentifier in_ident_F;
        } C_9;
    /* 10 */
        struct tut_mask_item_10
        {
            tsp00_DbName        in_db_F;
        } C_10;
    /* 11 */
        struct tut_mask_item_11
        {
            tsp00_NodeId        in_node_F;
        } C_11;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_C256         &in_c256                ()  { return this->variant.C_1.in_c256_F; };
    tsp00_C20          &msg                    ()  { return this->variant.C_1.msg_F; };
    tsp00_C2           &msg_in_attr            ()  { return this->variant.C_1.msg_in_attr_F; };
    pasbool            &upper_case             ()  { return this->variant.C_1.upper_case_F; };
    pasbool            &code_input             ()  { return this->variant.C_1.code_input_F; };
    tsp00_Int2         &in_len                 ()  { return this->variant.C_1.in_len_F; };
    tsp00_Name         &in_name                ()  { return this->variant.C_2.in_name_F; };
    tsp00_DevName      &in_devname             ()  { return this->variant.C_3.in_devname_F; };
    tsp00_VFilename    &in_fn                  ()  { return this->variant.C_4.in_fn_F; };
    tsp00_C1           &in_num1                ()  { return this->variant.C_5.in_num1_F; };
    tsp00_C2           &in_num2                ()  { return this->variant.C_6.in_num2_F; };
    tsp00_C5           &in_num5                ()  { return this->variant.C_7.in_num5_F; };
    tsp00_C10          &in_num10               ()  { return this->variant.C_8.in_num10_F; };
    tsp00_KnlIdentifier &in_ident               ()  { return this->variant.C_9.in_ident_F; };
    tsp00_DbName       &in_db                  ()  { return this->variant.C_10.in_db_F; };
    tsp00_NodeId       &in_node                ()  { return this->variant.C_11.in_node_F; };
    /* const accessor functions */
    const tsp00_C256   &in_c256                () const  { return this->variant.C_1.in_c256_F; };
    const tsp00_C20    &msg                    () const  { return this->variant.C_1.msg_F; };
    const tsp00_C2     &msg_in_attr            () const  { return this->variant.C_1.msg_in_attr_F; };
    const pasbool      &upper_case             () const  { return this->variant.C_1.upper_case_F; };
    const pasbool      &code_input             () const  { return this->variant.C_1.code_input_F; };
    const tsp00_Int2   &in_len                 () const  { return this->variant.C_1.in_len_F; };
    const tsp00_Name   &in_name                () const  { return this->variant.C_2.in_name_F; };
    const tsp00_DevName &in_devname             () const  { return this->variant.C_3.in_devname_F; };
    const tsp00_VFilename &in_fn                  () const  { return this->variant.C_4.in_fn_F; };
    const tsp00_C1     &in_num1                () const  { return this->variant.C_5.in_num1_F; };
    const tsp00_C2     &in_num2                () const  { return this->variant.C_6.in_num2_F; };
    const tsp00_C5     &in_num5                () const  { return this->variant.C_7.in_num5_F; };
    const tsp00_C10    &in_num10               () const  { return this->variant.C_8.in_num10_F; };
    const tsp00_KnlIdentifier &in_ident               () const  { return this->variant.C_9.in_ident_F; };
    const tsp00_DbName &in_db                  () const  { return this->variant.C_10.in_db_F; };
    const tsp00_NodeId &in_node                () const  { return this->variant.C_11.in_node_F; };
#endif    /* defined (__cplusplus) */
} tut_mask_item;

typedef enum tut_termline_type_Enum
{
    lt_normal,
    lt_get1name,
    lt_get2name,
    lt_getc256
} tut_termline_type_Enum;
typedef tut_termline_type_Enum tut_termline_type_Param;
typedef ENUM_TYPE_MSP00 (          tut_termline_type, tut_termline_type_Enum, lt_getc256, unsigned char);

typedef struct                     tut_termline_desc
{
    tut_termline_type   ld_type;
    pasbool             ld_new_page;
    pasbool             ld_in_code1;
    pasbool             ld_in_code2;
    tsp00_C2            ld_attr1;
    tsp00_C2            ld_attr2;
    tsp00_Int2          ld_in_len1;
    tsp00_Int2          ld_in_len2;
} tut_termline_desc;
typedef ARRAY_TYPE_MSP00 (         tut_ok_array, pasbool, cut_mask_items);
typedef ARRAY_TYPE_MSP00 (         tut_term_mask, tut_mask_item, cut_mask_items);
    /* */
typedef ARRAY_TYPE_MSP00 (         tut_term_cmds, tsp00_Line, cut_max_cmd_lines_term);
    /* */
typedef ARRAY_TYPE_MSP00 (         tut_term_param_pos, tsp00_Int2, cut_max_param_term);
typedef tsp00_C512                 tut_param_ln;

typedef struct                     tut_terminal
{
    ARRAY_VAR_MSP00 (         inputfile, tut_vf_fileref, cut_max_files_term);
    ARRAY_VAR_MSP00 (         inputfn, tsp00_VFilename, cut_max_files_term);
    ARRAY_VAR_MSP00 (         inputfilpos, tsp00_Int4, cut_max_files_term);
    ARRAY_VAR_MSP00 (         param_line, tut_param_ln, cut_max_files_term);
    ARRAY_VAR_MSP00 (         param_lwb, tut_term_param_pos, cut_max_files_term);
    ARRAY_VAR_MSP00 (         param_upb, tut_term_param_pos, cut_max_files_term);
    ARRAY_VAR_MSP00 (         count_cmd_lines, tsp00_Int2, cut_max_cmds_term);
    ARRAY_VAR_MSP00 (         last_cmd_lines, tut_term_cmds, cut_max_cmds_term);
    ARRAY_VAR_MSP00 (         lines, tsp00_Line, cut_max_lines_term);
    ARRAY_VAR_MSP00 (         ln_desc, tut_termline_desc, cut_max_lines_term);
    ARRAY_VAR_MSP00 (         prime_number, tsp00_Int4, cut_max_prime_cnt);
    tsp00_HifParms      hif;
    tsp00_Line          blankline;
    tsp00_Line          blank_window_line;
    tsp00_ScreenBufAddr screen_buf;
    tsp00_ScreenBufAddr screen_att;
    tsp00_VFilename     blankfilename;
    tsp00_Int4          bottom_frame_offset;
    tsp00_Int4          help_line_offset;
    tsp00_Int4          msg_line_offset;
    tsp00_Int4          screen_chars;
    tsp00_Int4          term_ref;
    tsp00_Int4          top_frame_offset;
    tsp00_Int2          bottom;
    tsp00_Int2          curr_inputfile;
    tsp00_Int2          currcol;
    tsp00_Int2          currline;
    tsp00_Int2          first_cmd_line;
    tsp00_Int2          header_lines;
    tsp00_Int2          last_cmd_line;
    tsp00_Int2          last_cmd_no;
    tsp00_Int2          put_linecount;
    tsp00_Int2          retcode;
    tsp00_Int2          retcode_external;
    tsp00_Int2          screen_cols;
    tsp00_Int2          screen_lines;
    tsp00_Int2          top;
    tsp00_Int2          window_bottom;
    tsp00_Int2          window_cols;
    tsp00_Int2          window_currline;
    tsp00_Int2          window_lines;
    tsp00_Int2          window_top;
    tsp00_Int2          used_window_lines;
    pasbool             has_sysline;
    pasbool             help_state;
    pasbool             is_batch;
    pasbool             is_cmd_read;
    pasbool             is_display;
    pasbool             is_frame;
    pasbool             is_holding;
    pasbool             is_inputfile;
    pasbool             is_mask;
    pasbool             is_reset_called;
    pasbool             is_virt_term;
    pasbool             screen_available;
    pasbool             screen_changed;
    pasbool             use_pfkeys;
    pasbool             io_ok;
    pasbool             use_script;
} tut_terminal;

struct tut00_TemplateDummy {
    tut_pfkey            dummy_tut_pfkey;
    tut_termline_type    dummy_tut_termline_type;
    tut_debug            dummy_tut_debug;
    tut_dev_type         dummy_tut_dev_type;
    tut_diag_scan        dummy_tut_diag_scan;
};


#endif
