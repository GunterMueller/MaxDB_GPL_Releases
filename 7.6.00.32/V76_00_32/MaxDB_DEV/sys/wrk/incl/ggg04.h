
/* some common prolog for every module
 */

#ifndef GGG04_H
#define GGG04_H


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

#define MAX_CATCH_LEVEL_GG04                   16
#define SITE_MXGG04                             2
#define RESCNT_MXGG04                           4
#define HASHVAL_MXGG04                          4

#if defined (BIT64)

#define ONE_8BYTE_CNT_GG04                      1    /* PTS 1103743 JA 1999-08-25 */

#else

#define ONE_8BYTE_CNT_GG04             1.0    /* PTS 1103743 JA 1999-08-25 */

#endif

#define NUMBER_MXGG04                  (( csp_fixed + 1 ) / 2 + 2)
#define MB_PART1_LOCKHEAD_MXGG04                4
#define LOCK_MXGG04                            16
#define MAX_USER_TASKS_GG04            MAX_INT2_SP00
#define MAX_TRANS_GG04                 MAX_INT2_SP00
#define MAX_STRATEGY_KEYS_GG04                 20
#define MAX_SHOW_COLUMNS_GG04                  80    /* PTS 1111217 E.Z. */
#define MAX_SERVER_MDESC_GG04          MAX_INT2_SP00
#define MAX_SERVER_TASKS_GG04          MAX_INT2_SP00
#define MAX_JOINS_GG04                        256
#define USERID_MXGG04                           4
    /**/
#define cgg04_languages                         4
#define cgg04_monday                            1
#define cgg04_sunday                            7
    /* constants for demands on result count */
#define cgg04_one_record_at_most_internal (- 4)    /* only one result allowed; SELECT INTO*/
#define cgg04_one_record_at_most_oracle (- 3)    /* result set allowed; SELECT INTO */
#define cgg04_subquery_one_record      (- 2)    /* subquery with one result */
#define cgg04_no_rowno_predicate       (- 1)    /* no ROWNO predicate is given */
#define cgg04_at_least_one_record      (- 5)    /* one record needed EXISTS predicate */
#define cgg04_rowno_given_as_parameter (- 6)    /* rowno not known during parsing */
#define cgg04_valid_rowno                       0    /* real ROWNO */
    /**/
    /***** in part2.buf[ 5 ] together with m_insert_select *****/
#define cgg04_ins_sel_nokey            '\x01'
#define cgg04_ignore_dupl              '\x02'
#define cgg04_reject_dupl              '\x03'
#define cgg04_update_dupl              '\x04'
#define cgg04_cluster_key              '\x05'    /* PTS 1139184 */
    /**/
#define cgg04_param_in_between_expr    '\x03'
#define cgg04_param_in_in_expr         '\x04'
#define cgg04_value_to_be_truncated    '\x05'
    /**/
#define cgg04_del_trigger_link          "\x00\x01"
#define cgg04_ins_trigger_link          "\x00\x03"
#define cgg04_upd_trigger_link          "\x00\x05"
    /**/
#define cgg04_dummy_site                "\xFF\xFE"
#define cgg04_nil_site                  "\xFF\xFF"
    /**/
#define cgg04_truechar                 '\x01'
#define cgg04_falsechar                '\x00'
    /**/
#define cgg04_zero_exponent            0x80
    /**/
    /***** constants used together with g02_codetables ******/
#define cgg04_to_ebcdic                         1    /* ascii_to_ebcdic */
#define cgg04_to_ascii                          2    /* ebcdic_to_ascii */
#define cgg04_up_ebcdic                         3    /* small_ebcdic_to_capital_ebcdic*/
#define cgg04_up_ascii                          4    /* small_ascii_to_capital_ascii  */
#define cgg04_to_up_ebcdic                      5    /* small_ascii_to_capital_ebcdic */
#define cgg04_to_up_ascii                       6    /* small_ebcdic_to_capital_ascii */
#define cgg04_low_ebcdic                        7    /* capital_ebcdic_to_small_ebcdic*/
#define cgg04_low_ascii                         8    /* capital_ascii_to_small_ascii  */
    /**/
#define TriggerKeyLen_cgg04                    14
    /**/
#define cgg04_union_select                    100
#define cgg04_union_append                     10
    /**/
#define cgg04_union_byte                        1
#define cgg04_except_byte                       2
#define cgg04_intersect_byte                    3
    /**/
#define cgg04_is_false                          0
#define cgg04_is_true                           1
#define cgg04_is_undef                          2
#define cgg04_is_not_false                      3
#define cgg04_is_not_true                       4
#define cgg04_is_not_undef                      5
    /**/
#define cgg04_timeout_interval                 30
#define cgg04_lc_timeout_interval               5    /* PTS 1110289 */
#define cgg04_str_col_entry_size                8
#define cgg04_return_stack_entries             13
#define cgg04_msgtext_len                     160    /* Length of column MSGTEXT */
#define cgg04_max_cmd_timeout               32400    /* seconds = 9 h   */
#define cgg04_mapset_entries                  256
    /**/
#define cgg04_oradate_len_to_char              14
#define cgg04_oradate_max_to_char             100
    /**/
#define cgg04_first_and_only_of_all             0    /* 1. + only data-packet             */
#define cgg04_first_and_only_of_this   (- 3)    /* 1. + only data-packet of this msg */
#define cgg04_first_of_all             (- 2)    /* 1.        data-packet             */
#define cgg04_last_of_all              (- 1)    /* last data-packet                  */
#define cgg04_last_of_this                      1    /* last data-packet of this message  */
    /*                                      >1 any other data-packet          */

/******************************************
 *               T Y P E S                *
 ******************************************/


typedef struct                     tgg04_FuncParamInfo
{
    tsp00_ObjAddr       fpi_nil_ptr;
    tgg00_SelectParamPtr fpi_sel_ptr;
} tgg04_FuncParamInfo;

typedef struct                     tgg04_DbFuncInfo
{
    tgg00_Surrogate     dbf_codeid;
    tsp00_Addr          dbf_acv_addr;
    tgg00_ServerdbNo    dbf_site;
    pasbool             dbf_new_code;
    tsp_dcom_co_create_instance_parms dbf_com_params;
    tsp00_Int4          dbf_dispid;
    pasbool             dbf_created;
} tgg04_DbFuncInfo;

typedef enum tgg04_FsUseMode_Enum
{
    fs_read,
    fs_write,
    fs_readwrite
} tgg04_FsUseMode_Enum;
typedef tgg04_FsUseMode_Enum tgg04_FsUseMode_Param;
typedef ENUM_TYPE_MSP00 (          tgg04_FsUseMode, tgg04_FsUseMode_Enum, fs_readwrite, unsigned char);
    /* */

typedef struct                     tgg04_StringFileDescr
{
    tgg00_FileId        str_treeid;
    tgg04_FsUseMode     use_mode;
    tsp00_CodeType      codespec;
    tgg00_ServerdbNo    dhomesite;
} tgg04_StringFileDescr;

typedef enum tgg04_Distinct_Enum
{
    no_distinct,
    full_distinct,
    distinct_without_warning_byte
} tgg04_Distinct_Enum;
typedef tgg04_Distinct_Enum tgg04_Distinct_Param;
typedef ENUM_TYPE_MSP00 (          tgg04_Distinct, tgg04_Distinct_Enum, distinct_without_warning_byte, unsigned char);
    /* */

typedef enum tgg04_CacheType_Enum
{
    cachCatalog_egg04,
    cachConv_egg04,
    cachData_egg04,
    cachFbm_egg04,
    cachProcCode_egg04,
    cachProcData_egg04,
    cachTemp_egg04,
    cachUsm_egg04,
    cachSequence_egg04
} tgg04_CacheType_Enum;
typedef tgg04_CacheType_Enum tgg04_CacheType_Param;
typedef ENUM_TYPE_MSP00 (          tgg04_CacheType, tgg04_CacheType_Enum, cachSequence_egg04, unsigned char);
    /* */

typedef struct                     tgg04_ServerdbIdent
{
    tsp00_DbName        svServerdb_gg04;
    tsp00_NodeId        svServernode_gg04;
} tgg04_ServerdbIdent;

typedef struct                     tgg04_ControlUser
{
    tsp00_CryptPw       gg04cu_password;
    tsp00_KnlIdentifier gg04cu_user;
} tgg04_ControlUser;

typedef struct                     tgg04_OneShortDay
{
    tsp00_Int4          daynumber;
    tsp00_C3            dayname;
    pasbool             filler;
} tgg04_OneShortDay;

typedef struct                     tgg04_OneLanguageShortDaynames
{
    tsp00_C3            language;
    pasbool             filler;
    ARRAY_VAR_MSP00 (         days, tgg04_OneShortDay, cgg04_sunday - cgg04_monday + 1);
} tgg04_OneLanguageShortDaynames;
typedef ARRAY_TYPE_MSP00 (         tgg04_ShortDaynames, tgg04_OneLanguageShortDaynames, cgg04_languages);

typedef struct                     tgg04_CodeTables
{
    ARRAY_VAR_MSP00 (         tables, tsp00_Ctable, MAX_CODE_TABLES_GG00);
} tgg04_CodeTables;

typedef struct                     tgg04_CodeGlobals
{
    tsp00_CodeType      ctype;
    tsp00_CodeType      char_default;
    tgg00_DateTimeFormat date_time;
    tsp00_SwapKind      kernel_swap;
} tgg04_CodeGlobals;

typedef struct                     tgg04_TabIdGlobals
{
    tgg00_FileId        sys_diag_parse;
    tgg00_FileId        obj_hash;
    tgg00_FileId        sys_cmd_analyze;
    tgg00_FileId        sys_data_analyze;
} tgg04_TabIdGlobals;
    /* */

typedef enum tgg04_ErrorText_Enum
{
    errtext_char,
    errtext_stack,
    errtext_surrogate,
    errtext_indexNo
} tgg04_ErrorText_Enum;
typedef tgg04_ErrorText_Enum tgg04_ErrorText_Param;
typedef ENUM_TYPE_MSP00 (          tgg04_ErrorText, tgg04_ErrorText_Enum, errtext_indexNo, unsigned char);
    /* */

typedef struct                     tgg04_Err
{
    union
    {
    /* 1 */
        struct tgg04_Err_1
        {
            tsp00_C256          errt_F;
        } C_1;
    /* 2 */
        struct tgg04_Err_2
        {
            tgg00_Surrogate     errtableid_F;
            tgg00_StackEntry    errstack_F;
            tgg00_ServerdbNo    errtablesite_F;
            tgg00_Tfn           errstacktype_F;
            pasbool             errrest1_F;
            tsp00_C20           errrest2_F;
        } C_2;
    /* 3 */
        struct tgg04_Err_3
        {
            tgg00_Surrogate     errtableid__F;
            tsp00_Int4          errfiller0_F;
            tsp00_Int4          errfiller1_F;
            tsp00_Uint1         errindexno_F;
            tsp00_Int4          errfiller2_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_C256         &errt                   ()  { return this->variant.C_1.errt_F; };
    tgg00_Surrogate    &errtableid             ()  { return this->variant.C_2.errtableid_F; };
    tgg00_StackEntry   &errstack               ()  { return this->variant.C_2.errstack_F; };
    tgg00_ServerdbNo   &errtablesite           ()  { return this->variant.C_2.errtablesite_F; };
    tgg00_Tfn          &errstacktype           ()  { return this->variant.C_2.errstacktype_F; };
    pasbool            &errrest1               ()  { return this->variant.C_2.errrest1_F; };
    tsp00_C20          &errrest2               ()  { return this->variant.C_2.errrest2_F; };
    tgg00_Surrogate    &errtableid_            ()  { return this->variant.C_3.errtableid__F; };
    tsp00_Int4         &errfiller0             ()  { return this->variant.C_3.errfiller0_F; };
    tsp00_Int4         &errfiller1             ()  { return this->variant.C_3.errfiller1_F; };
    tsp00_Uint1        &errindexno             ()  { return this->variant.C_3.errindexno_F; };
    tsp00_Int4         &errfiller2             ()  { return this->variant.C_3.errfiller2_F; };
    /* const accessor functions */
    const tsp00_C256   &errt                   () const  { return this->variant.C_1.errt_F; };
    const tgg00_Surrogate &errtableid             () const  { return this->variant.C_2.errtableid_F; };
    const tgg00_StackEntry &errstack               () const  { return this->variant.C_2.errstack_F; };
    const tgg00_ServerdbNo &errtablesite           () const  { return this->variant.C_2.errtablesite_F; };
    const tgg00_Tfn    &errstacktype           () const  { return this->variant.C_2.errstacktype_F; };
    const pasbool      &errrest1               () const  { return this->variant.C_2.errrest1_F; };
    const tsp00_C20    &errrest2               () const  { return this->variant.C_2.errrest2_F; };
    const tgg00_Surrogate &errtableid_            () const  { return this->variant.C_3.errtableid__F; };
    const tsp00_Int4   &errfiller0             () const  { return this->variant.C_3.errfiller0_F; };
    const tsp00_Int4   &errfiller1             () const  { return this->variant.C_3.errfiller1_F; };
    const tsp00_Uint1  &errindexno             () const  { return this->variant.C_3.errindexno_F; };
    const tsp00_Int4   &errfiller2             () const  { return this->variant.C_3.errfiller2_F; };
#endif    /* defined (__cplusplus) */
} tgg04_Err;
    /**/

typedef struct                     tgg04_subq_context
{
    tgg00_FileId        sc_tree_id;
    tgg00_Lkey          sc_tree_pos;
    tgg00_BasisError    sc_read_state;
} tgg04_subq_context;

struct tgg04_TemplateDummy {
    tgg04_CacheType      dummy_tgg04_CacheType;
    tgg04_Distinct       dummy_tgg04_Distinct;
    tgg04_ErrorText      dummy_tgg04_ErrorText;
    tgg04_FsUseMode      dummy_tgg04_FsUseMode;
};


#endif
