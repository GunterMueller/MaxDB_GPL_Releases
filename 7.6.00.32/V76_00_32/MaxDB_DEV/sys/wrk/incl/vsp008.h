
/* some common prolog for every module
 */

#ifndef VSP008_H
#define VSP008_H


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

#define csp8_unicode_ix                         1    /* basic encoding set, param */
    /* */
#define csp8_encodings                          5
#define csp8_subcodes                          20
#define csp8_subspaces                          4
#define csp8_ranges                             3
#define csp8_mapvalues                      65535
    /*  */
#define mxsp8_vendor_str                       60    /* twice the length in table */
#define mxsp8_descr_str                        60    /* twice the length in table */

/******************************************
 *               T Y P E S                *
 ******************************************/

    /* UNI_TRANS return-codes */
    /* */

typedef enum tsp8_uni_error_Enum
{
    uni_ok,
    uni_no_such_encoding,
    uni_not_translatable,
    uni_dest_too_short,
    uni_src_too_short,
    uni_is_singlebyte,
    uni_is_multibyte,
    uni_is_incomplete,
    uni_translate_warning
} tsp8_uni_error_Enum;
typedef tsp8_uni_error_Enum tsp8_uni_error_Param;
typedef ENUM_TYPE_MSP00 (          tsp8_uni_error, tsp8_uni_error_Enum, uni_translate_warning, unsigned char);

typedef enum tsp8_uni_load_error_Enum
{
    uni_load_ok,
    uni_encoding_not_found,
    uni_no_dblang_found,
    uni_cannot_open_infofile,
    uni_cannot_open_localefile,
    uni_cannot_open_longcolfile,
    uni_file_error_localefile,
    uni_file_error_longcolfile,
    uni_memory_alloc_failed,
    uni_encoding_already_loaded,
    uni_bad_localefile,
    uni_bad_infofile
} tsp8_uni_load_error_Enum;
typedef tsp8_uni_load_error_Enum tsp8_uni_load_error_Param;
typedef ENUM_TYPE_MSP00 (          tsp8_uni_load_error, tsp8_uni_load_error_Enum, uni_bad_infofile, unsigned char);
    /* */

typedef enum tsp8_encoding_type_Enum
{
    uni_fixed,
    uni_modal
} tsp8_encoding_type_Enum;
typedef tsp8_encoding_type_Enum tsp8_encoding_type_Param;
typedef ENUM_TYPE_MSP00 (          tsp8_encoding_type, tsp8_encoding_type_Enum, uni_modal, unsigned char);

typedef enum tsp8_value_typ_Enum
{
    uni_value,
    uni_index
} tsp8_value_typ_Enum;
typedef tsp8_value_typ_Enum tsp8_value_typ_Param;
typedef ENUM_TYPE_MSP00 (          tsp8_value_typ, tsp8_value_typ_Enum, uni_index, unsigned char);
    /* */

typedef enum tsp8_uni_flag_Enum
{
    uni_upper
} tsp8_uni_flag_Enum;
typedef tsp8_uni_flag_Enum tsp8_uni_flag_Param;
typedef ENUM_TYPE_MSP00 (          tsp8_uni_flag, tsp8_uni_flag_Enum, uni_upper, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tsp8_flags, tsp8_uni_flag_Enum, uni_upper);
    /* */
    /* UNI_TRANS options                                                */
    /*   uni_change_to_upper:     map character-strings enclosed in     */
    /*                            single-quotes to uppercase.           */
    /*                                                                  */
    /*   uni_fillup_field:        fill the result-string of the trans-  */
    /*                            lation up to the length specified in_ */
    /*                            "uni_trans"-parameter "dest_len". Use */
    /*                            fillchar (see "encoding-record" below */
    /*                                                                  */
    /*   uni_del_trailing_blanks: delete trailing blanks of the result  */
    /*                                                                  */
    /* */

typedef enum tsp8_uni_trans_option_Enum
{
    uni_change_to_upper,
    uni_fillup_field,
    uni_del_trailing_blanks
} tsp8_uni_trans_option_Enum;
typedef tsp8_uni_trans_option_Enum tsp8_uni_trans_option_Param;
typedef ENUM_TYPE_MSP00 (          tsp8_uni_trans_option, tsp8_uni_trans_option_Enum, uni_del_trailing_blanks, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tsp8_uni_opt_set, tsp8_uni_trans_option_Enum, uni_del_trailing_blanks);
    /* */
    /* UNI_LOAD options                                             */
    /*    uni_read_dblang:    tells "uni_fload" to load the charset */
    /*                        specified in environm. variable DBLANG*/
    /*                        and to ignore "iso_locale_str"        */
    /* */

typedef enum tsp8_uni_load_option_Enum
{
    uni_read_dblang
} tsp8_uni_load_option_Enum;
typedef tsp8_uni_load_option_Enum tsp8_uni_load_option_Param;
typedef ENUM_TYPE_MSP00 (          tsp8_uni_load_option, tsp8_uni_load_option_Enum, uni_read_dblang, unsigned char);
typedef SMALLSET_TYPE_MSP00 (      tsp8_uni_load_opt_set, tsp8_uni_load_option_Enum, uni_read_dblang);
    /* */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp8_descr_string, mxsp8_descr_str);
typedef CSTRING_TYPE_MSP00      (  tsp8_descr_stringc, mxsp8_descr_str);
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp8_vendor_string, mxsp8_vendor_str);
typedef CSTRING_TYPE_MSP00      (  tsp8_vendor_stringc, mxsp8_vendor_str);
    /* */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp8_mapc1values, csp8_mapvalues - 0 + 1);
typedef CSTRING_TYPE_MSP00      (  tsp8_mapc1valuesc, csp8_mapvalues - 0 + 1);
typedef ARRAY_TYPE_MSP00 (         tsp8_mapvalues, tsp00_C2, csp8_mapvalues - 0 + 1);
typedef ARRAY_TYPE_MSP00 (         tsp8_mapindexes, tsp00_Int2, csp8_mapvalues - 0 + 1);
    /* */
typedef tsp8_mapc1values          *tsp8_c1_values_table_ptr;
typedef tsp8_mapvalues            *tsp8_values_table_ptr;
typedef tsp8_mapindexes           *tsp8_index_table_ptr;

typedef struct                     tsp8_mapping_table
{
    union
    {
    /* 1 */
        struct tsp8_mapping_table_1
        {
            tsp8_values_table_ptr values_F;
            tsp8_value_typ      value_typ_F;
            tsp00_Uint1         value_len_F;
            tsp00_Int2          mt_fill2_F;
            tsp00_Int4          mt_fill3_F;
        } C_1;
    /* 2 */
        struct tsp8_mapping_table_2
        {
            tsp8_index_table_ptr indexes_F;
            tsp8_value_typ      imt_fill0_F;
            tsp00_Uint1         imt_fill1_F;
            tsp00_Int2          imt_fill2_F;
            tsp00_Int4          imt_fill3_F;
        } C_2;
    /* 3 */
        struct tsp8_mapping_table_3
        {
            tsp8_c1_values_table_ptr c1_values_F;
            tsp8_value_typ      ic1_fill0_F;
            tsp00_Uint1         ic1_fill1_F;
            tsp00_Int2          ic1_fill2_F;
            tsp00_Int4          ic1_fill3_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp8_values_table_ptr &values                 ()  { return this->variant.C_1.values_F; };
    tsp8_value_typ     &value_typ              ()  { return this->variant.C_1.value_typ_F; };
    tsp00_Uint1        &value_len              ()  { return this->variant.C_1.value_len_F; };
    tsp00_Int2         &mt_fill2               ()  { return this->variant.C_1.mt_fill2_F; };
    tsp00_Int4         &mt_fill3               ()  { return this->variant.C_1.mt_fill3_F; };
    tsp8_index_table_ptr &indexes                ()  { return this->variant.C_2.indexes_F; };
    tsp8_value_typ     &imt_fill0              ()  { return this->variant.C_2.imt_fill0_F; };
    tsp00_Uint1        &imt_fill1              ()  { return this->variant.C_2.imt_fill1_F; };
    tsp00_Int2         &imt_fill2              ()  { return this->variant.C_2.imt_fill2_F; };
    tsp00_Int4         &imt_fill3              ()  { return this->variant.C_2.imt_fill3_F; };
    tsp8_c1_values_table_ptr &c1_values              ()  { return this->variant.C_3.c1_values_F; };
    tsp8_value_typ     &ic1_fill0              ()  { return this->variant.C_3.ic1_fill0_F; };
    tsp00_Uint1        &ic1_fill1              ()  { return this->variant.C_3.ic1_fill1_F; };
    tsp00_Int2         &ic1_fill2              ()  { return this->variant.C_3.ic1_fill2_F; };
    tsp00_Int4         &ic1_fill3              ()  { return this->variant.C_3.ic1_fill3_F; };
    /* const accessor functions */
    const tsp8_values_table_ptr &values                 () const  { return this->variant.C_1.values_F; };
    const tsp8_value_typ &value_typ              () const  { return this->variant.C_1.value_typ_F; };
    const tsp00_Uint1  &value_len              () const  { return this->variant.C_1.value_len_F; };
    const tsp00_Int2   &mt_fill2               () const  { return this->variant.C_1.mt_fill2_F; };
    const tsp00_Int4   &mt_fill3               () const  { return this->variant.C_1.mt_fill3_F; };
    const tsp8_index_table_ptr &indexes                () const  { return this->variant.C_2.indexes_F; };
    const tsp8_value_typ &imt_fill0              () const  { return this->variant.C_2.imt_fill0_F; };
    const tsp00_Uint1  &imt_fill1              () const  { return this->variant.C_2.imt_fill1_F; };
    const tsp00_Int2   &imt_fill2              () const  { return this->variant.C_2.imt_fill2_F; };
    const tsp00_Int4   &imt_fill3              () const  { return this->variant.C_2.imt_fill3_F; };
    const tsp8_c1_values_table_ptr &c1_values              () const  { return this->variant.C_3.c1_values_F; };
    const tsp8_value_typ &ic1_fill0              () const  { return this->variant.C_3.ic1_fill0_F; };
    const tsp00_Uint1  &ic1_fill1              () const  { return this->variant.C_3.ic1_fill1_F; };
    const tsp00_Int2   &ic1_fill2              () const  { return this->variant.C_3.ic1_fill2_F; };
    const tsp00_Int4   &ic1_fill3              () const  { return this->variant.C_3.ic1_fill3_F; };
#endif    /* defined (__cplusplus) */
} tsp8_mapping_table;
typedef tsp8_mapping_table        *tsp8_mapping_table_ptr;

typedef struct                     tsp8_byterange
{
    char                low;
    char                high;
    tsp00_Int2          delta;
    tsp00_Int4          br_fill2;
} tsp8_byterange;

typedef struct                     tsp8_subspace
{
    tsp00_Int4          start_ix;
    tsp00_Int4          end_ix;
    tsp00_Int2          dimension;
    tsp00_Int2          ss_filler1;
    tsp00_Int4          ss_filler2;
    ARRAY_VAR_MSP00 (         byteranges, tsp8_byterange, csp8_ranges);
} tsp8_subspace;

typedef struct                     tsp8_subcode
{
    tsp00_KnlIdentifier descr;
    tsp00_Int2          subspace_cnt;
    tsp8_flags          flags;
    pasbool             sub_c_filler;
    ARRAY_VAR_MSP00 (         subspace, tsp8_subspace, csp8_subspaces);
} tsp8_subcode;
typedef tsp8_subcode              *tsp8_subcode_ptr;
typedef ARRAY_TYPE_MSP00 (         tsp8_subcodes, tsp8_subcode_ptr, csp8_subcodes);
typedef tsp8_subcodes             *tsp8_subcodes_ptr;
    /* ENCODING-Record                                                    */
    /*                                                                    */
    /* iso_locale_str = national characterset, specified as a string      */
    /*                  like 'JA_JPN.SJIS' or 'ISO8859'                   */
    /*                                                                    */
    /* descr          = free-style description-text, describes the        */
    /*                  characterset (eg. 'Shift-JIS Japan')              */
    /*                                                                    */
    /* subcode_cnt    = number of subcodes of a national character-       */
    /*                  set (eg. ISO8859 has only one subcode (ASCII),    */
    /*                  but Shift-JIS has 5 subcodes (ASCII, Kana, Kanji  */
    /*                  etc.). This number is generated.                  */
    /*                                                                    */
    /* vendor         = short vendor description, free-style              */
    /*                                                                    */
    /* typ            = Classification of characterset, values are:       */
    /*                  MODAL (Characterset using Shift-In, Shift-Out     */
    /*                  FIXED (Shift-JIS-type of Characterset             */
    /*                  Since MODAL-codesets are not implemented, this    */
    /*                  field is currently not used by s80uni_trans       */
    /*                                                                    */
    /* enc_filler     = fill-character used by s80uni_trans to fill       */
    /*                  fields when requested by option "uni_fill_char"   */

typedef struct                     tsp8_enc_record
{
    tsp8_mapping_table_ptr mapping1;
    tsp8_mapping_table_ptr mapping2;
    tsp8_subcodes       subcode1;
    tsp8_subcodes       subcode2;
    tsp8_vendor_string  vendor;
    tsp8_descr_string   descr;
    tsp00_KnlIdentifier iso_locale_str;
    tsp00_Int2          subcode_cnt1;
    tsp00_Int2          subcode_cnt2;
    tsp00_Int2          enc_filler1;
    tsp00_C4            fillcharacter;
    tsp00_Uint1         fillchar_len;
    tsp00_Uint1         max_codewidth;
    tsp8_encoding_type  typ;
    tsp00_Uint1         enc_filler2;
} tsp8_enc_record;
typedef ARRAY_TYPE_MSP00 (         tsp8_encodings, tsp8_enc_record, csp8_encodings);
typedef tsp8_encodings            *tsp8_encodings_ptr;

struct tsp008_TemplateDummy {
    tsp8_encoding_type   dummy_tsp8_encoding_type;
    tsp8_uni_load_error  dummy_tsp8_uni_load_error;
    tsp8_uni_trans_option dummy_tsp8_uni_trans_option;
    tsp8_uni_load_option dummy_tsp8_uni_load_option;
    tsp8_uni_flag        dummy_tsp8_uni_flag;
    tsp8_value_typ       dummy_tsp8_value_typ;
    tsp8_uni_error       dummy_tsp8_uni_error;
    tsp8_flags           dummy_tsp8_flags;
    tsp8_uni_opt_set     dummy_tsp8_uni_opt_set;
    tsp8_uni_load_opt_set dummy_tsp8_uni_load_opt_set;
};


#endif
