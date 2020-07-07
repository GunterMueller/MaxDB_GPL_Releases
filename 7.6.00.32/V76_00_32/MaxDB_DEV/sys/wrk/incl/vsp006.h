
/* some common prolog for every module
 */

#ifndef VSP006_H
#define VSP006_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

#ifndef GSP00_H
#include "gsp00.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

#define mxsp6_date_format                     100
#define csp6_languages                          4

/******************************************
 *               T Y P E S                *
 ******************************************/

typedef ARRAY_TYPE_MSP00 (         tsp6_timestamp_array, integer, 7);

typedef enum tsp6_date_fmt_elem_Enum
{
    sp6df_no_correct_format,
    sp6df_fill_mode_toggle,
    sp6df_exact_mode_toggle,
    sp6df_quoted_text,
    sp6df_special_char,
    sp6df_seconds_of_minute,
    sp6df_seconds_of_day,
    sp6df_minute,
    sp6df_hour,
    sp6df_hour_12,
    sp6df_hour_24,
    sp6df_day_of_week_numeric,
    sp6df_day_of_week_long,
    sp6df_day_of_week_short,
    sp6df_day_of_month,
    sp6df_day_of_year,
    sp6df_julian_day,
    sp6df_week_of_month,
    sp6df_week_of_year,
    sp6df_month_numeric,
    sp6df_month_long,
    sp6df_month_short,
    sp6df_month_roman,
    sp6df_quarter,
    sp6df_year,
    sp6df_year_1,
    sp6df_year_10,
    sp6df_year_10_relative,
    sp6df_year_100,
    sp6df_year_comma,
    sp6df_year_short,
    sp6df_century,
    sp6df_meridian_a_m_,
    sp6df_meridian_p_m_,
    sp6df_meridian_am,
    sp6df_meridian_pm,
    sp6df_anno_a_d_,
    sp6df_anno_b_c_,
    sp6df_anno_ad,
    sp6df_anno_bc,
    sp6df_microseconds
} tsp6_date_fmt_elem_Enum;
typedef tsp6_date_fmt_elem_Enum tsp6_date_fmt_elem_Param;
typedef ENUM_TYPE_MSP00 (          tsp6_date_fmt_elem, tsp6_date_fmt_elem_Enum, sp6df_microseconds, unsigned char);

typedef enum tsp6_date_fmt_modifier__Enum
{
    sp6dfm_spelled,
    sp6dfm_ordinal,
    sp6dfm_iso,
    sp6dfm_signed_years
} tsp6_date_fmt_modifier__Enum;
typedef tsp6_date_fmt_modifier__Enum tsp6_date_fmt_modifier__Param;
typedef SMALLSET_TYPE_MSP00 (      tsp6_date_fmt_modifier, tsp6_date_fmt_modifier__Enum, sp6dfm_signed_years);

typedef enum tsp6_date_spell_kind_Enum
{
    sp6dfs_spell_mixed,
    sp6dfs_spell_lower,
    sp6dfs_spell_upper
} tsp6_date_spell_kind_Enum;
typedef tsp6_date_spell_kind_Enum tsp6_date_spell_kind_Param;
typedef ENUM_TYPE_MSP00 (          tsp6_date_spell_kind, tsp6_date_spell_kind_Enum, sp6dfs_spell_upper, unsigned char);

typedef enum tsp6_date_error_Enum
{
    sp6de_ok,
    sp6de_format_not_recognized,
    sp6de_invalid_date,
    sp6de_invalid_time,
    sp6de_invalid_timestamp,
    sp6de_invalid_day,
    sp6de_invalid_timezone,
    sp6de_unknown_day,
    sp6de_unknown_month,
    sp6de_no_letter,
    sp6de_no_number,
    sp6de_no_to_date_format,
    sp6de_unknown_meridian,
    sp6de_too_short_input,
    sp6de_too_long_input,
    sp6de_inconsistent,
    sp6de_duplicate_format,
    sp6de_out_of_range,
    sp6de_overflow,
    sp6de_num_overflow,
    sp6de_end_of_month_adjustment,
    sp6de_num_invalid
} tsp6_date_error_Enum;
typedef tsp6_date_error_Enum tsp6_date_error_Param;
typedef ENUM_TYPE_MSP00 (          tsp6_date_error, tsp6_date_error_Enum, sp6de_num_invalid, unsigned char);
typedef char                       tsp6_language;

typedef struct                     tsp6_dict_languages
{
    integer             count;
    ARRAY_VAR_MSP00 (         lang, tsp00_C3, 4);
    ARRAY_VAR_MSP00 (         max_month_len, tsp00_Uint1, 4);
    ARRAY_VAR_MSP00 (         max_day_len, tsp00_Uint1, 4);
} tsp6_dict_languages;

typedef struct                     tsp6_spell_dict
{
    tsp00_C12           string;
    tsp00_Int2          length;
} tsp6_spell_dict;

typedef enum tsp6_dict_etc_Enum
{
    sp6d_no_etc,
    sp6d_and,
    sp6d_hundred,
    sp6d_thousand,
    sp6d_million,
    sp6d_millions
} tsp6_dict_etc_Enum;
typedef tsp6_dict_etc_Enum tsp6_dict_etc_Param;
typedef ENUM_TYPE_MSP00 (          tsp6_dict_etc, tsp6_dict_etc_Enum, sp6d_millions, unsigned char);
typedef ARRAY2_TYPE_MSP00 (        tsp6_dict_upto19, tsp6_spell_dict, 19 - 0 + 1, true - 0 + 1);
typedef ARRAY2_TYPE_MSP00 (        tsp6_dict_upto90, tsp6_spell_dict, 9 - 2 + 1, true - 0 + 1);
typedef ARRAY2_TYPE_MSP00 (        tsp6_dict_for_etc, tsp6_spell_dict, sp6d_millions - 0 + 1, true - 0 + 1);
typedef ARRAY_TYPE_MSP00 (         tsp6_dict_month, tsp6_spell_dict, 12);
typedef ARRAY_TYPE_MSP00 (         tsp6_dict_weekday, tsp6_spell_dict, 7);
typedef ARRAY_TYPE_MSP00 (         tsp6_dict_roman_month, tsp6_spell_dict, 12);
typedef ARRAY_TYPE_MSP00 (         tsp6_dict_meridian, tsp6_spell_dict, sp6df_anno_bc - sp6df_meridian_a_m_ + 1);

typedef struct                     tsp6_language_spell
{
    tsp00_C24           string;
    tsp00_Int2          length;
} tsp6_language_spell;
typedef ARRAY_TYPE_MSP00 (         tsp6_language_month, tsp6_language_spell, 12);
typedef ARRAY_TYPE_MSP00 (         tsp6_language_weekday, tsp6_language_spell, 7);

typedef struct                     tsp6_dictionary
{
    tsp00_Ctable       *dict_upp_table;
    tsp00_Ctable       *dict_low_table;
    tsp00_C3            lang;
    tsp00_Uint1         max_month_len;
    tsp00_Uint1         max_day_len;
    tsp6_language_month dict_lang_month;
    tsp6_language_weekday dict_lang_day;
    tsp6_dict_upto19    dict_19;
    tsp6_dict_upto90    dict_90;
    tsp6_dict_for_etc   dict_etc;
    tsp6_dict_month     dict_mon;
    tsp6_dict_weekday   dict_day;
    tsp6_dict_roman_month dict_rom;
    tsp6_dict_meridian  dict_mer;
} tsp6_dictionary;

typedef struct                     tsp6_dictionaries
{
    integer             count;
    ARRAY_VAR_MSP00 (         dict, tsp6_dictionary, 4);
} tsp6_dictionaries;

struct tsp006_TemplateDummy {
    tsp6_date_spell_kind dummy_tsp6_date_spell_kind;
    tsp6_date_fmt_elem   dummy_tsp6_date_fmt_elem;
    tsp6_dict_etc        dummy_tsp6_dict_etc;
    tsp6_date_error      dummy_tsp6_date_error;
    tsp6_date_fmt_modifier dummy_tsp6_date_fmt_modifier;
};


#endif
