
      tsp6_timestamp_array = ARRAY [ 1..7 ] OF integer;
      tsp6_date_fmt_elem = (
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
            sp6df_microseconds (* sapr3 only *)
            );
      tsp6_date_fmt_modifier = SET OF (
            sp6dfm_spelled,
            sp6dfm_ordinal,
            sp6dfm_iso,
            sp6dfm_signed_years);
      tsp6_date_spell_kind = (
            sp6dfs_spell_mixed,
            sp6dfs_spell_lower,
            sp6dfs_spell_upper);
      tsp6_date_error = (
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
            sp6de_num_invalid);
      tsp6_language       = 1 .. csp6_languages;
 
      tsp6_dict_languages = RECORD
            count         : integer;
            lang          : ARRAY [ tsp6_language ] OF tsp00_C3;
            max_month_len : ARRAY [ tsp6_language ] OF tsp00_Uint1;
            max_day_len   : ARRAY [ tsp6_language ] OF tsp00_Uint1;
      END;
 
 
      tsp6_spell_dict = RECORD
            string : tsp00_C12;
            length : tsp00_Int2;
      END;
 
      tsp6_dict_etc = (
            sp6d_no_etc,
            sp6d_and,
            sp6d_hundred,
            sp6d_thousand,
            sp6d_million,
            sp6d_millions);
      tsp6_dict_upto19      = ARRAY [ 0 .. 19,       boolean ]
            OF tsp6_spell_dict;
      tsp6_dict_upto90      = ARRAY [ 2 .. 9,        boolean ]
            OF tsp6_spell_dict;
      tsp6_dict_for_etc     = ARRAY [ tsp6_dict_etc, boolean ]
            OF tsp6_spell_dict;
      tsp6_dict_month       = ARRAY [ 1 .. 12 ] OF tsp6_spell_dict;
      tsp6_dict_weekday     = ARRAY [ 1 .. 7  ] OF tsp6_spell_dict;
      tsp6_dict_roman_month = ARRAY [ 1 .. 12 ] OF tsp6_spell_dict;
      tsp6_dict_meridian    = ARRAY [ sp6df_meridian_a_m_ .. sp6df_anno_bc  ]
            OF tsp6_spell_dict;
 
      tsp6_language_spell = RECORD
            string : tsp00_C24;
            length : tsp00_Int2;
      END;
 
      tsp6_language_month   = ARRAY [ 1 .. 12 ] OF tsp6_language_spell;
      tsp6_language_weekday = ARRAY [ 1 ..  7 ] OF tsp6_language_spell;
 
      tsp6_dictionary = RECORD
            dict_upp_table  : ^tsp00_Ctable;
            dict_low_table  : ^tsp00_Ctable;
            (* *)
            lang            : tsp00_C3;
            max_month_len   : tsp00_Uint1;
            max_day_len     : tsp00_Uint1;
            dict_lang_month : tsp6_language_month;
            dict_lang_day   : tsp6_language_weekday;
            (* *)
            dict_19         : tsp6_dict_upto19;
            dict_90         : tsp6_dict_upto90;
            dict_etc        : tsp6_dict_for_etc;
            dict_mon        : tsp6_dict_month;
            dict_day        : tsp6_dict_weekday;
            dict_rom        : tsp6_dict_roman_month;
            dict_mer        : tsp6_dict_meridian;
      END;
 
 
      tsp6_dictionaries = RECORD
            count : integer;
            dict  : ARRAY [ tsp6_language ] OF tsp6_dictionary;
      END;
 
