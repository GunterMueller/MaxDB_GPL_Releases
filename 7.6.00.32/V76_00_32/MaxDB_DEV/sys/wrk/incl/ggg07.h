
/* some common prolog for every module
 */

#ifndef GGG07_H
#define GGG07_H


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

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

#define POS_OFFS_GG07                           1
#define TAB_OFFS_GG07                           1
#define STRINVINRANGE_START_MXGG07             36    /* header of tgg07_StrInvInRange without keys */
#define STRATEGY_START_MXGG07                  80    /* StrategyInf - str_catalog */
    /*! STRATEGY_START_MXGG07 MOD ALLIGNMENT_GG00 = 0 !*/
    /**/
#define INV_DISTINCT_OPTIM_GG07        (- 2)
#define AUTO_DISTINCT_OPTIM_GG07                1
#define NO_DISTINCT_OPTIM_GG07                  0
#define KEYSEQLEN_OFFSET_GG07          (PERM_KEY_MXSP00 + 1)    /* will not be used for resultset-'key's */
    /**/
    /* biggest size of all strategy structures */
#define MAX_STRATETGY_LEN_GG07                240
    /**/
#define MAX_COLPOSARR_IDX_GG07         (MAX_STRATEGY_KEYS_GG04 - 1)
    /**/
#define NOT_SEQUENCED_GG07             (- 5)
#define IS_UNDEFINED_GG07              (- 9999)
#define VARKEY_GG07                    (( 255 ))
    /**/
#define cgg07_stratpos_offs                     8
    /**/
#define cgg07_left_len_eq_right_len             0
#define cgg07_left_len_le_right_len             1
#define cgg07_left_len_gt_right_len             2
    /**/
#define cgg07_maxnewpos                (MAX_COL_PER_TAB_GG00 + MAX_JOINS_GG04)
#define cgg07_maxdistinctfiles                 16
    /* offsets to mstrat_pos */
#define cgg07_ResultShiftInfo_Offset            1    /* legacy join   */
#define cgg07_OutputDesc_Offset                 1    /* operator join */
#define cgg07_JoinInfo_Offset                   2
#define cgg07_OneTabStrat_Offset                3

/******************************************
 *               T Y P E S                *
 ******************************************/

typedef ARRAY_TYPE_MSP00 (         tgg07_ColPosArr, tsp00_Int2, MAX_COLPOSARR_IDX_GG07 - 0 + 1);
    /**/

typedef enum tgg07_SubqKind_Enum
{
    no_subquery,
    query_in_subq,
    normal_subquery
} tgg07_SubqKind_Enum;
typedef tgg07_SubqKind_Enum tgg07_SubqKind_Param;
typedef ENUM_TYPE_MSP00 (          tgg07_SubqKind, tgg07_SubqKind_Enum, normal_subquery, unsigned char);
    /**/
typedef ARRAY_TYPE_MSP00 (         tgg07_StrRaw, tsp00_C1, MAX_STRATETGY_LEN_GG07);
    /**/
typedef ARRAY_TYPE_MSP00 (         tgg07_CatRecColPosArr, pasbool, MAX_SHOW_COLUMNS_GG04);
    /**/
typedef SET_TYPE_MSP00 (           tgg07_InvColSet, int, MAX_COL_SEQUENCE_GG00, 2);
    /**/

typedef enum tgg07_KeyStratProp_Enum
{
    ksp_exact_match,
    ksp_exact_IN_SUBQ_match,
    ksp_order_support,
    ksp_order_support_reverse,
    ksp_aggr_MIN_opt,
    ksp_aggr_MAX_opt,
    ksp_auto_distinct,
    ksp_first_row_start,
    ksp_first_row_stop,
    ksp_unused11,
    ksp_unused12,
    ksp_unused13,
    ksp_unused14,
    ksp_unused15,
    ksp_unused16,
    ksp_unused17,
    ksp_unused20,
    ksp_unused21,
    ksp_unused22,
    ksp_unused23,
    ksp_unused24,
    ksp_unused25,
    ksp_unused26,
    ksp_unused27,
    ksp_unused30,
    ksp_unused31,
    ksp_unused32,
    ksp_unused33,
    ksp_unused34,
    ksp_unused35,
    ksp_unused36,
    ksp_unused37
} tgg07_KeyStratProp_Enum;
typedef tgg07_KeyStratProp_Enum tgg07_KeyStratProp_Param;
typedef ENUM_TYPE_MSP00 (          tgg07_KeyStratProp, tgg07_KeyStratProp_Enum, ksp_unused37, unsigned char);
typedef SET_TYPE_MSP00 (           tgg07_KeyStratProps, tgg07_KeyStratProp_Enum, ksp_unused37, 4);

typedef struct                     tgg07_StrKeyInRange
{
    tgg07_ColPosArr     skir_keystart;
    tgg07_ColPosArr     skir_keystop;
    tsp00_Int2          skir_IN_SUBQ_stpos;
    tsp00_Int2          skir_inelemcnt;
    tgg07_KeyStratProps skir_strat_props;
} tgg07_StrKeyInRange;

typedef enum tgg07_InvStratProp_Enum
{
    isp_exact_match,
    isp_exact_IN_SUBQ_match,
    isp_unique_idx,
    isp_inv_only,
    isp_order_support,
    isp_order_support_reverse,
    isp_listmerge_allowed,
    isp_aggr_MIN_opt,
    isp_aggr_MIN_opt_reverse,
    isp_aggr_MAX_opt,
    isp_aggr_MAX_opt_reverse,
    isp_auto_distinct,
    isp_aggr_optim,
    isp_pure_inv_output,
    isp_aggr_key_MIN_opt,
    isp_aggr_key_MAX_opt,
    isp_first_row_start,
    isp_first_row_stop,
    isp_keysubset,
    isp_unused23,
    isp_unused24,
    isp_unused25,
    isp_unused26,
    isp_unused27,
    isp_unused30,
    isp_unused31,
    isp_unused32,
    isp_unused33,
    isp_unused34,
    isp_unused35,
    isp_unused36,
    isp_unused37
} tgg07_InvStratProp_Enum;
typedef tgg07_InvStratProp_Enum tgg07_InvStratProp_Param;
typedef ENUM_TYPE_MSP00 (          tgg07_InvStratProp, tgg07_InvStratProp_Enum, isp_unused37, unsigned char);
typedef SET_TYPE_MSP00 (           tgg07_InvStratProps, tgg07_InvStratProp_Enum, isp_unused37, 4);

typedef struct                     tgg07_StrInvInRange
{
    tgg07_ColPosArr     siir_keystart;
    tgg07_ColPosArr     siir_keystop;
    tgg07_ColPosArr     siir_invstart;
    tgg07_ColPosArr     siir_invstop;
    tsp00_Int2          siir_startcnt;
    tsp00_Int2          siir_stopcnt;
    tsp00_PageNo        siir_invroot;
    tsp00_Int2          siir_indexno;
    tsp00_Int2          siir_icount;
    tgg07_InvColSet     siir_invcoldesc;
    tsp00_Int2          siir_invlen;
    tsp00_Int2          siir_inelemcnt;
    tsp00_Int2          siir_subq_idx;
    tsp00_Int2          siir_out_invseqlen;
    tsp00_Int2          siir_IN_SUBQ_stpos;
    tsp00_Int2          siir_MIN_recpos;
    tsp00_Int2          siir_MAX_recpos;
    tsp00_Int4          siir_ExpIdxPages;
    tgg07_InvStratProps siir_strat_props;
} tgg07_StrInvInRange;

typedef struct                     tgg07_StrJoinMultFields
{
    tgg07_ColPosArr     sjmf_keystart;
    tgg07_ColPosArr     sjmf_keystop;
    tsp00_PageNo        sjmf_invroot;
    tsp00_Int4          sjmf_invlen;
    tsp00_Int2          sjmf_index_no;
    tsp00_Int2          sjmf_cntfields;
    tsp00_Int4          sjmf_filler;
} tgg07_StrJoinMultFields;

typedef struct                     tgg07_StrCatalog
{
    tgg00_ShowKind      scat_show_kind;
    pasbool             scat_upd_strat;
    tsp00_Int2          scat_authname;
    tsp00_Int2          scat_tablename;
    tsp00_Int2          scat_username;
    tsp00_Int2          scat_columnname;
    tsp00_Int2          scat_fill2;
    tgg07_CatRecColPosArr scat_col_needed;
    tgg07_CatRecColPosArr scat_qual_cols;
    tsp00_KnlIdentifier scat_dblink;
    tsp00_Int4          scat_filler;
} tgg07_StrCatalog;

typedef enum tgg07_StratEnum_Enum
{
    strat_viewkey,
    strat_key_equal,
    strat_key_in,
    strat_key_subq,
    strat_key_range,
    strat_inv_range,
    strat_inv_in,
    strat_inv_subq,
    strat_undecided,
    strat_key_equal_fetch,
    strat_key_range_fetch,
    strat_inv_range_fetch,
    strat_inv_range_merge_fetch,
    strat_join_key_equal,
    strat_join_key_next,
    strat_join_all_keys_equal,
    strat_join_key_range,
    strat_join_viewkey,
    strat_join_inv,
    strat_join_all_inv_equal,
    strat_join_inv_range,
    strat_more_than_one,
    strat_no_result,
    strat_catalog
} tgg07_StratEnum_Enum;
typedef tgg07_StratEnum_Enum tgg07_StratEnum_Param;
typedef ENUM_TYPE_MSP00 (          tgg07_StratEnum, tgg07_StratEnum_Enum, strat_catalog, unsigned char);
    /**/
typedef SET_TYPE_MSP00 (           tgg07_StratEnumSet, tgg07_StratEnum_Enum, strat_catalog, 4);

typedef enum tgg07_StratAccessModEnum_Enum
{
    strmod_hash,
    strmod_parallel,
    strmod_sorted,
    strmod_synchronized,
    strmod_unused04,
    strmod_unused05,
    strmod_unused06,
    strmod_unused07
} tgg07_StratAccessModEnum_Enum;
typedef tgg07_StratAccessModEnum_Enum tgg07_StratAccessModEnum_Param;
typedef ENUM_TYPE_MSP00 (          tgg07_StratAccessModEnum, tgg07_StratAccessModEnum_Enum, strmod_unused07, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tgg07_StratAccessModSet, tgg07_StratAccessModEnum_Enum, strmod_unused07);

typedef enum tgg07_KeyOrderSupport_Enum
{
    kos_no_support,
    kos_asc,
    kos_desc
} tgg07_KeyOrderSupport_Enum;
typedef tgg07_KeyOrderSupport_Enum tgg07_KeyOrderSupport_Param;
typedef ENUM_TYPE_MSP00 (          tgg07_KeyOrderSupport, tgg07_KeyOrderSupport_Enum, kos_desc, unsigned char);

typedef struct                     tgg07_StrategyInfo
{
    tgg00_FileId        str_result_id;
    tgg07_StratEnum     str_strategy;
    pasbool             str_build_result;
    tgg04_Distinct      str_distinc;
    pasbool             str_ordering;
    pasbool             str_use_rowno;
    pasbool             str_use_clusterIO;
    pasbool             str_corr_single;
    tsp00_Uint1         str_union_kind;
    pasbool             str_res_possible;
    pasbool             str_use_order;
    tsp00_Int2          str_out_keyseqlen;
    tsp00_Int2          str_cnt_strat;
    tsp00_C2            str_filler;
    tsp00_Int4          str_rowno;
    tsp00_Int4          str_foundresults;
    tsp00_Int2          str_key_len;
    tsp00_Int2          str_rec_len;
    tsp00_Int4          str_union_key_cnt_offs;
    tsp00_Int2          str_stack_output_offs;
    pasbool             str_search_first;
    pasbool             str_all_files;
    tgg07_KeyOrderSupport str_key_order_support;
    pasbool             str_selinto;
    tgg07_StratAccessModSet str_access_mod;
    tgg00_QualKind      str_qual_kind;
    union
    {
    /* strat_key_equal */
        struct tgg07_StrategyInfo_strat_key_equal
        {
            tgg07_StrKeyInRange str_key_in_range_F;
        } C_strat_key_equal;
    /* strat_inv_range */
        struct tgg07_StrategyInfo_strat_inv_range
        {
            tgg07_StrInvInRange str_inv_in_range_F;
        } C_strat_inv_range;
    /* strat_join_inv */
        struct tgg07_StrategyInfo_strat_join_inv
        {
            tgg07_StrJoinMultFields str_join_multfields_F;
        } C_strat_join_inv;
    /* strat_catalog */
        struct tgg07_StrategyInfo_strat_catalog
        {
            tgg07_StrCatalog    str_catalog_F;
        } C_strat_catalog;
    /* strat_viewkey */
        struct tgg07_StrategyInfo_strat_viewkey
        {
            tgg07_StrRaw        str_raw_F;
        } C_strat_viewkey;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg07_StrKeyInRange &str_key_in_range       ()  { return this->variant.C_strat_key_equal.str_key_in_range_F; };
    tgg07_StrInvInRange &str_inv_in_range       ()  { return this->variant.C_strat_inv_range.str_inv_in_range_F; };
    tgg07_StrJoinMultFields &str_join_multfields    ()  { return this->variant.C_strat_join_inv.str_join_multfields_F; };
    tgg07_StrCatalog   &str_catalog            ()  { return this->variant.C_strat_catalog.str_catalog_F; };
    tgg07_StrRaw       &str_raw                ()  { return this->variant.C_strat_viewkey.str_raw_F; };
    /* const accessor functions */
    const tgg07_StrKeyInRange &str_key_in_range       () const  { return this->variant.C_strat_key_equal.str_key_in_range_F; };
    const tgg07_StrInvInRange &str_inv_in_range       () const  { return this->variant.C_strat_inv_range.str_inv_in_range_F; };
    const tgg07_StrJoinMultFields &str_join_multfields    () const  { return this->variant.C_strat_join_inv.str_join_multfields_F; };
    const tgg07_StrCatalog &str_catalog            () const  { return this->variant.C_strat_catalog.str_catalog_F; };
    const tgg07_StrRaw &str_raw                () const  { return this->variant.C_strat_viewkey.str_raw_F; };
#endif    /* defined (__cplusplus) */
} tgg07_StrategyInfo;
typedef tgg07_StrategyInfo        *tgg07_StrategyInfoPtr;

typedef struct                     tgg07_KbOneJoinInfo
{
    tsp00_Int2          kboj_recpos;
    tsp00_Int2          kboj_len;
    tgg00_StackOpType   kboj_op;
    pasbool             kboj_reverse_operands;
    tsp00_Int2          kboj_fill2;
} tgg07_KbOneJoinInfo;

typedef struct                     tgg07_KbJoinInfo
{
    ARRAY_VAR_MSP00 (         kbji_parts, tgg07_KbOneJoinInfo, 2);
} tgg07_KbJoinInfo;

typedef struct                     tgg07_KbJoinRec
{
    tsp00_Int2          kbjr_jointrans_cnt;
    pasbool             kbjr_left_oj;
    pasbool             kbjr_right_oj;
    tsp00_Int2          kbjr_jpath;
    tsp00_Int2          kbjr_filler;
    ARRAY_VAR_MSP00 (         kbjr_jarr, tgg07_KbJoinInfo, MAX_JOINS_GG04);
} tgg07_KbJoinRec;

typedef struct                     tgg07_JoinColCopy
{
    tsp00_Int2          jfrom;
    tsp00_Int2          jto;
    tsp00_Int2          jlen;
    tsp00_Int2          jfill1;
} tgg07_JoinColCopy;
typedef ARRAY_TYPE_MSP00 (         tgg07_JoinColCopyArr, tgg07_JoinColCopy, MAX_JOINS_GG04);

typedef struct                     tgg07_JoinColCopyInfo
{
    tsp00_Int2          n_j_cnt;
    tsp00_Int2          n_j_fill;
    tgg07_JoinColCopyArr n_j_arr;
} tgg07_JoinColCopyInfo;

typedef struct                     tgg07_JoinResInfo
{
    tgg00_FileId        o_tree;
    tsp00_Int2          n_rec_len;
    tsp00_Int2          n_key_len;
    tsp00_Int2          n_res_rec_len;
    tsp00_Int2          n_filler1;
} tgg07_JoinResInfo;

typedef struct                     tgg07_one_new_pos
{
    tsp00_Int2          onfrom;
    tsp00_Int2          onto;
    tsp00_Int2          onlen;
    tsp00_Int2          on_fill;
} tgg07_one_new_pos;

typedef struct                     tgg07_new_positions
{
    ARRAY_VAR_MSP00 (         posarr, tgg07_one_new_pos, cgg07_maxnewpos);
    tsp00_Int2          lposcnt;
    tsp00_Int2          rposcnt;
} tgg07_new_positions;

typedef struct                     tgg07_get_param
{
    tgg07_new_positions gi_n_pos_s;
    tgg07_KbJoinRec     gi_linkrec;
    tgg07_JoinColCopyInfo gi_copy_info;
    tgg07_JoinResInfo   gi_result_info;
} tgg07_get_param;
typedef tgg07_get_param           *tgg07_get_param_ptr;

typedef struct                     tgg07_distinct_file_info
{
    tsp00_PageNo        dfi_root;
    tsp00_Int4          dfi_root_check;
} tgg07_distinct_file_info;
typedef ARRAY_TYPE_MSP00 (         tgg07_distinct_roots, tgg07_distinct_file_info, cgg07_maxdistinctfiles);

typedef enum tgg07_setfunc_handling_Enum
{
    sfh_none,
    sfh_stddev_variance,
    sfh_all
} tgg07_setfunc_handling_Enum;
typedef tgg07_setfunc_handling_Enum tgg07_setfunc_handling_Param;
typedef ENUM_TYPE_MSP00 (          tgg07_setfunc_handling, tgg07_setfunc_handling_Enum, sfh_all, unsigned char);

typedef enum tgg07_set_operators_Enum
{
    so_UNION,
    so_EXCEPT,
    so_INTERSECT,
    so_ALL_mod
} tgg07_set_operators_Enum;
typedef tgg07_set_operators_Enum tgg07_set_operators_Param;
typedef ENUM_TYPE_MSP00 (          tgg07_set_operators, tgg07_set_operators_Enum, so_ALL_mod, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tgg07_set_operator, tgg07_set_operators_Enum, so_ALL_mod);

typedef enum tgg07_key_properties_Enum
{
    kp_key,
    kp_nokey,
    kp_clusterKey
} tgg07_key_properties_Enum;
typedef tgg07_key_properties_Enum tgg07_key_properties_Param;
typedef ENUM_TYPE_MSP00 (          tgg07_key_properties, tgg07_key_properties_Enum, kp_clusterKey, unsigned char);

typedef struct                     tgg07_select_context
{
    tgg00_FileId        selr_resf_id;
    tgg00_FileId        selr_inv_id;
    tgg00_FileId        selr_distinct_id;
    tgg00_FileId        selr_intersect_id;
    tgg00_FilePos       selr_tree_pos;
    tgg00_FileId        selr_id_outer_join;
    tgg00_FileId        selr_tree;
    tgg07_distinct_roots selr_disrootarr;
    tgg00_Lkey          selr_startkey;
    tgg00_Lkey          selr_stopkey;
    tgg00_Lkey          selr_invkey;
    tsp00_Int4          selr_rowno;
    tsp00_Int4          selr_key_cnt_offs;
    tsp00_Int4          selr_countresult;
    tsp00_Int4          selr_act_cntresult;
    tsp00_Int4          selr_resrec_maxlen;
    tsp00_Int4          selr_maxresult;
    tsp00_Int4          selr_group_count;
    tsp00_Int2          selr_recl;
    tsp00_Int2          selr_dis_file_cnt;
    tsp00_Int2          selr_res_keyl;
    tsp00_Int2          selr_l_currpos;
    tsp00_Int2          selr_l_endpos;
    tsp00_Int2          selr_first_jlen;
    tsp00_Int2          selr_l_defbyte_pos;
    tsp00_Int2          selr_keyl;
    tgg00_MessType      selr_mbtype;
    pasbool             selr_distinct_bytes;
    tgg04_Distinct      selr_distinct;
    pasbool             selr_union;
    pasbool             selr_one_result;
    pasbool             selr_subquery;
    pasbool             selr_intern_dist;
    tgg07_key_properties selr_key_props;
    pasbool             selr_count_all;
    pasbool             selr_count_bd;
    tsp00_C1            selr_dupl;
    pasbool             selr_write_rownum;
    pasbool             selr_UNION_ALL_append;
    tgg07_set_operator  selr_set_operator;
    tgg07_setfunc_handling selr_call_avg;
    pasbool             selr_call_later_out;
    pasbool             selr_trunc_part2;
    pasbool             selr_file_to_drop;
    pasbool             selr_no_warn8_sort;
    pasbool             selr_oracle_selinto;
    pasbool             selr_join_with_func;
    pasbool             selr_all_recursive;
    pasbool             selr_last_join;
    pasbool             selr_finding_poss;
    pasbool             selr_reverse_fetch;
} tgg07_select_context;
typedef tgg07_select_context      *tgg07_select_context_ptr;

typedef struct                     tgg07_select_param
{
    union
    {
    /* true */
        struct tgg07_select_param_true
        {
            tgg00_SelectBuffer  selr_selectbuffer_F;
            tgg07_select_context selr_F;
        } C_true;
    /* false */
        struct tgg07_select_param_false
        {
            tsp00_MoveObjPtr    selr_ptrallignment_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_SelectBuffer &selr_selectbuffer      ()  { return this->variant.C_true.selr_selectbuffer_F; };
    tgg07_select_context &selr                   ()  { return this->variant.C_true.selr_F; };
    tsp00_MoveObjPtr   &selr_ptrallignment     ()  { return this->variant.C_false.selr_ptrallignment_F; };
    /* const accessor functions */
    const tgg00_SelectBuffer &selr_selectbuffer      () const  { return this->variant.C_true.selr_selectbuffer_F; };
    const tgg07_select_context &selr                   () const  { return this->variant.C_true.selr_F; };
    const tsp00_MoveObjPtr &selr_ptrallignment     () const  { return this->variant.C_false.selr_ptrallignment_F; };
#endif    /* defined (__cplusplus) */
} tgg07_select_param;
typedef tgg07_select_param        *tgg07_SelectParamPtr;

typedef struct                     tgg07_stack_backup_info
{
    tgg00_StackDesc     bi_stackdesc;
    union
    {
    /* true */
        struct tgg07_stack_backup_info_true
        {
            tsp00_Addr          bi_backup_addr_F;
        } C_true;
    /* false */
        struct tgg07_stack_backup_info_false
        {
            tsp00_MoveObjPtr    bi_backup_ptr_F;
        } C_false;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Addr         &bi_backup_addr         ()  { return this->variant.C_true.bi_backup_addr_F; };
    tsp00_MoveObjPtr   &bi_backup_ptr          ()  { return this->variant.C_false.bi_backup_ptr_F; };
    /* const accessor functions */
    const tsp00_Addr   &bi_backup_addr         () const  { return this->variant.C_true.bi_backup_addr_F; };
    const tsp00_MoveObjPtr &bi_backup_ptr          () const  { return this->variant.C_false.bi_backup_ptr_F; };
#endif    /* defined (__cplusplus) */
} tgg07_stack_backup_info;

struct tgg07_TemplateDummy {
    tgg07_setfunc_handling dummy_tgg07_setfunc_handling;
    tgg07_StratEnum      dummy_tgg07_StratEnum;
    tgg07_StratAccessModEnum dummy_tgg07_StratAccessModEnum;
    tgg07_set_operators  dummy_tgg07_set_operators;
    tgg07_KeyOrderSupport dummy_tgg07_KeyOrderSupport;
    tgg07_InvStratProp   dummy_tgg07_InvStratProp;
    tgg07_KeyStratProp   dummy_tgg07_KeyStratProp;
    tgg07_key_properties dummy_tgg07_key_properties;
    tgg07_SubqKind       dummy_tgg07_SubqKind;
    tgg07_StratAccessModSet dummy_tgg07_StratAccessModSet;
    tgg07_InvStratProps  dummy_tgg07_InvStratProps;
    tgg07_StratEnumSet   dummy_tgg07_StratEnumSet;
    tgg07_KeyStratProps  dummy_tgg07_KeyStratProps;
    tgg07_InvColSet      dummy_tgg07_InvColSet;
    tgg07_set_operator   dummy_tgg07_set_operator;
};


#endif
