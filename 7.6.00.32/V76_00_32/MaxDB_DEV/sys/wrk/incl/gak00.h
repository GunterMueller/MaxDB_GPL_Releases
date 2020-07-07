
/* some common prolog for every module
 */

#ifndef GAK00_H
#define GAK00_H


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

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

#define cak00_maxsources                      254
#define cak00_maxsubqueries            csp_maxint1
    /* MAX_UINT1_SP00 - 1                                 */
    /* tabno MAX_UINT1_SP00 used for join expression cols */
#define mxak00_ofield                          80    /* &tak_ofield */
#define cak00_maxordercolumns                 128
#define cak00_col_with_inv                    100
    /**/
#define cak00_local_segment_id          "\x00\x00"
#define cak00_public_segment_id         "\x00\x01"
#define cak00_default_mapchar_set       "DEFAULTMAP        "
#define cak00_old_parsid               (- 12)

/******************************************
 *               T Y P E S                *
 ******************************************/

    /*---------------- basic types ---------------------*/

typedef enum tak00_ooption__Enum
{
    is_order_asc,
    is_opt,
    is_possible_oflw_field,
    is_val_expression,
    is_nls_column,
    is_not_definite,
    is_col_only_specified
} tak00_ooption__Enum;
typedef tak00_ooption__Enum tak00_ooption__Param;
typedef SMALLSET_TYPE_MSP00 (      tak00_ooption, tak00_ooption__Enum, is_col_only_specified);

typedef struct                     tak00_ofield
{
    tsp00_KnlIdentifier ofname;
    tsp00_Int2          ofno;
    tsp00_Int2          ofstno;
    tak00_ooption       ofasc;
    tsp00_Uint1         offill;
    tsp00_Int2          offno;
    tsp00_Int4          ofapos;
    tsp00_Int2          ofnode;
    tsp00_Int2          oftabno;
} tak00_ofield;
typedef ARRAY_TYPE_MSP00 (         tak00_ordfield_arr, tak00_ofield, cak00_maxordercolumns);

typedef struct                     tak00_ord_fields
{
    tak00_ordfield_arr  ofield;
    tsp00_Int2          ocntord;
    tsp00_Int2          ofill1;
    tsp00_Int4          ofill2;
} tak00_ord_fields;
typedef tak00_ord_fields          *tak00_ord_fields_ptr;
    /* column catalog representation */

typedef enum tak00_colprop_Enum
{
    ctkey,
    ctopt,
    ctdefault,
    ctcomment,
    ctunused,
    ctmulti,
    ctlink,
    ctdropped,
    ctexpression,
    ctinvisible,
    ctcatalog,
    ctviewname,
    ctjoinviewkey,
    ctdomain,
    ctaltered,
    ctserial
} tak00_colprop_Enum;
typedef tak00_colprop_Enum tak00_colprop_Param;
typedef ENUM_TYPE_MSP00 (          tak00_colprop, tak00_colprop_Enum, ctserial, unsigned char);
typedef SET_TYPE_MSP00 (           tak00_colpropset, tak00_colprop_Enum, ctserial, 2);

typedef struct                     tak00_columninfo
{
    union
    {
    /* 1 */
        struct tak00_columninfo_1
        {
            tgg00_StackEntry    ccolstack_F;
            tak00_colpropset    ccolpropset_F;
            tsp00_Int2          cextcolno_F;
            tsp00_Int2          creccolno_F;
            tsp00_Uint1         ctabno_F;
            tsp00_DataType      cdatatyp_F;
            tsp00_Int2          cdatalen_F;
            tsp00_Int2          cinoutlen_F;
            tsp00_Uint1         cdatafrac_F;
            char                cshiftoutchar_F;
            tsp00_Int2          cnextind_F;
            tsp00_C2            cudtdatatype_F;
            char                cfiller02_F;
            char                ccolumnn_len_F;
            tsp00_KnlIdentifier ccolumnn_F;
        } C_1;
    /* 2 */
        struct tak00_columninfo_2
        {
            tgg00_StackEntry    cfiller1_F;
            tak00_colpropset    cfiller2_F;
            tsp00_Int2          cfiller3_F;
            tsp00_Int2          cfiller4_F;
            tsp00_Uint1         cfiller5_F;
            tsp00_DataType      cfiller6_F;
            tsp00_Int2          cfiller7_F;
            tsp00_Int2          cfiller8_F;
            tsp00_Uint1         cfiller9_F;
            pasbool             cbinary_F;
        } C_2;
    /* 3 */
        struct tak00_columninfo_3
        {
            tgg00_StackEntry    cfiller_1_F;
            tak00_colpropset    cfiller_2_F;
            tsp00_Int2          cfiller_3_F;
            tsp00_Int2          cfiller_4_F;
            tsp00_Uint1         cfiller_5_F;
            tsp00_DataType      cfiller_6_F;
            tsp00_Int2          cfiller_7_F;
            tsp00_Int2          cfiller_8_F;
            tsp00_Uint1         cfiller_9_F;
            pasbool             cvarchar_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_StackEntry   &ccolstack              ()  { return this->variant.C_1.ccolstack_F; };
    tak00_colpropset   &ccolpropset            ()  { return this->variant.C_1.ccolpropset_F; };
    tsp00_Int2         &cextcolno              ()  { return this->variant.C_1.cextcolno_F; };
    tsp00_Int2         &creccolno              ()  { return this->variant.C_1.creccolno_F; };
    tsp00_Uint1        &ctabno                 ()  { return this->variant.C_1.ctabno_F; };
    tsp00_DataType     &cdatatyp               ()  { return this->variant.C_1.cdatatyp_F; };
    tsp00_Int2         &cdatalen               ()  { return this->variant.C_1.cdatalen_F; };
    tsp00_Int2         &cinoutlen              ()  { return this->variant.C_1.cinoutlen_F; };
    tsp00_Uint1        &cdatafrac              ()  { return this->variant.C_1.cdatafrac_F; };
    char               &cshiftoutchar          ()  { return this->variant.C_1.cshiftoutchar_F; };
    tsp00_Int2         &cnextind               ()  { return this->variant.C_1.cnextind_F; };
    tsp00_C2           &cudtdatatype           ()  { return this->variant.C_1.cudtdatatype_F; };
    char               &cfiller02              ()  { return this->variant.C_1.cfiller02_F; };
    char               &ccolumnn_len           ()  { return this->variant.C_1.ccolumnn_len_F; };
    tsp00_KnlIdentifier &ccolumnn               ()  { return this->variant.C_1.ccolumnn_F; };
    tgg00_StackEntry   &cfiller1               ()  { return this->variant.C_2.cfiller1_F; };
    tak00_colpropset   &cfiller2               ()  { return this->variant.C_2.cfiller2_F; };
    tsp00_Int2         &cfiller3               ()  { return this->variant.C_2.cfiller3_F; };
    tsp00_Int2         &cfiller4               ()  { return this->variant.C_2.cfiller4_F; };
    tsp00_Uint1        &cfiller5               ()  { return this->variant.C_2.cfiller5_F; };
    tsp00_DataType     &cfiller6               ()  { return this->variant.C_2.cfiller6_F; };
    tsp00_Int2         &cfiller7               ()  { return this->variant.C_2.cfiller7_F; };
    tsp00_Int2         &cfiller8               ()  { return this->variant.C_2.cfiller8_F; };
    tsp00_Uint1        &cfiller9               ()  { return this->variant.C_2.cfiller9_F; };
    pasbool            &cbinary                ()  { return this->variant.C_2.cbinary_F; };
    tgg00_StackEntry   &cfiller_1              ()  { return this->variant.C_3.cfiller_1_F; };
    tak00_colpropset   &cfiller_2              ()  { return this->variant.C_3.cfiller_2_F; };
    tsp00_Int2         &cfiller_3              ()  { return this->variant.C_3.cfiller_3_F; };
    tsp00_Int2         &cfiller_4              ()  { return this->variant.C_3.cfiller_4_F; };
    tsp00_Uint1        &cfiller_5              ()  { return this->variant.C_3.cfiller_5_F; };
    tsp00_DataType     &cfiller_6              ()  { return this->variant.C_3.cfiller_6_F; };
    tsp00_Int2         &cfiller_7              ()  { return this->variant.C_3.cfiller_7_F; };
    tsp00_Int2         &cfiller_8              ()  { return this->variant.C_3.cfiller_8_F; };
    tsp00_Uint1        &cfiller_9              ()  { return this->variant.C_3.cfiller_9_F; };
    pasbool            &cvarchar               ()  { return this->variant.C_3.cvarchar_F; };
    /* const accessor functions */
    const tgg00_StackEntry &ccolstack              () const  { return this->variant.C_1.ccolstack_F; };
    const tak00_colpropset &ccolpropset            () const  { return this->variant.C_1.ccolpropset_F; };
    const tsp00_Int2   &cextcolno              () const  { return this->variant.C_1.cextcolno_F; };
    const tsp00_Int2   &creccolno              () const  { return this->variant.C_1.creccolno_F; };
    const tsp00_Uint1  &ctabno                 () const  { return this->variant.C_1.ctabno_F; };
    const tsp00_DataType &cdatatyp               () const  { return this->variant.C_1.cdatatyp_F; };
    const tsp00_Int2   &cdatalen               () const  { return this->variant.C_1.cdatalen_F; };
    const tsp00_Int2   &cinoutlen              () const  { return this->variant.C_1.cinoutlen_F; };
    const tsp00_Uint1  &cdatafrac              () const  { return this->variant.C_1.cdatafrac_F; };
    const char         &cshiftoutchar          () const  { return this->variant.C_1.cshiftoutchar_F; };
    const tsp00_Int2   &cnextind               () const  { return this->variant.C_1.cnextind_F; };
    const tsp00_C2     &cudtdatatype           () const  { return this->variant.C_1.cudtdatatype_F; };
    const char         &cfiller02              () const  { return this->variant.C_1.cfiller02_F; };
    const char         &ccolumnn_len           () const  { return this->variant.C_1.ccolumnn_len_F; };
    const tsp00_KnlIdentifier &ccolumnn               () const  { return this->variant.C_1.ccolumnn_F; };
    const tgg00_StackEntry &cfiller1               () const  { return this->variant.C_2.cfiller1_F; };
    const tak00_colpropset &cfiller2               () const  { return this->variant.C_2.cfiller2_F; };
    const tsp00_Int2   &cfiller3               () const  { return this->variant.C_2.cfiller3_F; };
    const tsp00_Int2   &cfiller4               () const  { return this->variant.C_2.cfiller4_F; };
    const tsp00_Uint1  &cfiller5               () const  { return this->variant.C_2.cfiller5_F; };
    const tsp00_DataType &cfiller6               () const  { return this->variant.C_2.cfiller6_F; };
    const tsp00_Int2   &cfiller7               () const  { return this->variant.C_2.cfiller7_F; };
    const tsp00_Int2   &cfiller8               () const  { return this->variant.C_2.cfiller8_F; };
    const tsp00_Uint1  &cfiller9               () const  { return this->variant.C_2.cfiller9_F; };
    const pasbool      &cbinary                () const  { return this->variant.C_2.cbinary_F; };
    const tgg00_StackEntry &cfiller_1              () const  { return this->variant.C_3.cfiller_1_F; };
    const tak00_colpropset &cfiller_2              () const  { return this->variant.C_3.cfiller_2_F; };
    const tsp00_Int2   &cfiller_3              () const  { return this->variant.C_3.cfiller_3_F; };
    const tsp00_Int2   &cfiller_4              () const  { return this->variant.C_3.cfiller_4_F; };
    const tsp00_Uint1  &cfiller_5              () const  { return this->variant.C_3.cfiller_5_F; };
    const tsp00_DataType &cfiller_6              () const  { return this->variant.C_3.cfiller_6_F; };
    const tsp00_Int2   &cfiller_7              () const  { return this->variant.C_3.cfiller_7_F; };
    const tsp00_Int2   &cfiller_8              () const  { return this->variant.C_3.cfiller_8_F; };
    const tsp00_Uint1  &cfiller_9              () const  { return this->variant.C_3.cfiller_9_F; };
    const pasbool      &cvarchar               () const  { return this->variant.C_3.cvarchar_F; };
#endif    /* defined (__cplusplus) */
} tak00_columninfo;
typedef tak00_columninfo          *tak00_colinfo_ptr;

typedef enum tak00_duration_typ_Enum
{
    year,
    month,
    day,
    hour,
    minute,
    second,
    microsecond
} tak00_duration_typ_Enum;
typedef tak00_duration_typ_Enum tak00_duration_typ_Param;
typedef ENUM_TYPE_MSP00 (          tak00_duration_typ, tak00_duration_typ_Enum, microsecond, unsigned char);

typedef struct                     tak00_scolinf
{
    tsp00_Int2          sci_len;
    tsp00_Int2          sci_iolen;
    tsp00_Int2          sci_frac;
    tsp00_DataType      sci_typ;
    tak00_duration_typ  sci_dttyp;
    tak00_colpropset    sci_cprops;
    tsp00_Int2          sci_com_type;
    tsp00_C2            sci_udt_id;
} tak00_scolinf;
    /* switches for one table selects */

typedef enum tak00_access_config_switch_Enum
{
    cs_L1_strategy,
    cs_L2_strategy,
    cs_L2_range_strategy,
    cs_L2_more_strategies,
    cs_L2_strat_no_optim,
    cs_IN_strategy,
    cs_SUBQ_strategy,
    cs_build_result,
    cs_fetch_result,
    cs_disable_inv_only,
    cs_count_records,
    cs_enable_merge,
    cs_keyaccess,
    cs_keyscan,
    cs_keyrange,
    cs_indexaccess,
    cs_indexscan,
    cs_indexrange,
    cs_trace,
    cs_ordered_join,
    cs_coordinator_join,
    cs_operator_join,
    cs_pretend_parameter,
    cs_hashjoin_ratio,
    cs_unused30,
    cs_unused31,
    cs_unused32,
    cs_unused33,
    cs_unused34,
    cs_unused35,
    cs_unused36,
    cs_unused37
} tak00_access_config_switch_Enum;
typedef tak00_access_config_switch_Enum tak00_access_config_switch_Param;
typedef ENUM_TYPE_MSP00 (          tak00_access_config_switch, tak00_access_config_switch_Enum, cs_unused37, unsigned char);
    /* switches for join selects */

typedef enum tak00_join_config_switch_Enum
{
    cj_keyaccess,
    cj_indexaccess,
    cj_noaccesspath,
    cj_predefined_buffer,
    cj_unused2,
    cj_unused3,
    cj_unused4,
    cj_unused5,
    cj_unused6
} tak00_join_config_switch_Enum;
typedef tak00_join_config_switch_Enum tak00_join_config_switch_Param;
typedef ENUM_TYPE_MSP00 (          tak00_join_config_switch, tak00_join_config_switch_Enum, cj_unused6, unsigned char);
typedef SET_TYPE_MSP00 (           tak00_access_config_switches, tak00_access_config_switch_Enum, cs_unused37, 4);

typedef struct                     tak00_access_configuration
{
    tsp00_Int2          cfg_indexno;
    tak00_access_config_switches cfg_switches;
    tsp00_Int2          cfg_hashratio;
} tak00_access_configuration;
typedef SET_TYPE_MSP00 (           tak00_join_config_switches, tak00_join_config_switch_Enum, cj_unused6, 2);

typedef struct                     tak00_join_configuration
{
    tsp00_Int2          cfg_indexno;
    tsp00_Int2          cfg_parallel_server;
    tsp00_Int4          cfg_bufsize;
    tsp00_Int2          cfg_filler2;
    tak00_join_config_switches cfg_join_switches;
    tak00_access_config_switches cfg_access_switches;
} tak00_join_configuration;
typedef ARRAY_TYPE_MSP00 (         tak00_join_configuration_arr, tak00_join_configuration, cak00_maxsources);

typedef enum tak00_PrivR_Enum
{
    r_sel,
    r_upd,
    r_ins,
    r_del,
    r_owner,
    r_alter,
    r_index,
    r_link
} tak00_PrivR_Enum;
typedef tak00_PrivR_Enum tak00_PrivR_Param;
typedef ENUM_TYPE_MSP00 (          tak00_PrivR, tak00_PrivR_Enum, r_link, unsigned char);
    /* */
typedef SMALLSET_TYPE_MSP00 (      tak00_PrivilegeSet, tak00_PrivR_Enum, r_link);

struct tak00_TemplateDummy {
    tak00_PrivR          dummy_tak00_PrivR;
    tak00_colprop        dummy_tak00_colprop;
    tak00_access_config_switch dummy_tak00_access_config_switch;
    tak00_join_config_switch dummy_tak00_join_config_switch;
    tak00_duration_typ   dummy_tak00_duration_typ;
    tak00_ooption        dummy_tak00_ooption;
    tak00_colpropset     dummy_tak00_colpropset;
    tak00_PrivilegeSet   dummy_tak00_PrivilegeSet;
    tak00_access_config_switches dummy_tak00_access_config_switches;
    tak00_join_config_switches dummy_tak00_join_config_switches;
};


#endif
