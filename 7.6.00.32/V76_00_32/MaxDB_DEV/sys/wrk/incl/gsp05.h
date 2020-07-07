
/* some common prolog for every module
 */

#ifndef GSP05_H
#define GSP05_H


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


/******************************************
 *               T Y P E S                *
 ******************************************/


typedef enum tsp05_RteFileWarning_Enum
{
    sp5vfw_no_warning,
    sp5vfw_no_eol_found,
    sp5vfw_no_next_tape,
    sp5vfw_already_locked
} tsp05_RteFileWarning_Enum;
typedef tsp05_RteFileWarning_Enum tsp05_RteFileWarning_Param;
typedef ENUM_TYPE_MSP00 (          tsp05_RteFileWarning, tsp05_RteFileWarning_Enum, sp5vfw_already_locked, unsigned char);

typedef struct                     tsp05_RteFileError
{
    tsp00_VfReturn      sp5fe_result;
    tsp05_RteFileWarning sp5fe_warning;
    tsp00_C2            filler;
    tsp00_C40           sp5fe_text;
} tsp05_RteFileError;

typedef enum tsp05_RteDataKind_Enum
{
    sp5vf_text,
    sp5vf_binary,
    sp5vf_encoded
} tsp05_RteDataKind_Enum;
typedef tsp05_RteDataKind_Enum tsp05_RteDataKind_Param;
typedef ENUM_TYPE_MSP00 (          tsp05_RteDataKind, tsp05_RteDataKind_Enum, sp5vf_encoded, unsigned char);

typedef enum tsp05_RteFileMode_Enum
{
    sp5vf_read,
    sp5vf_write,
    sp5vf_readwrite,
    sp5vf_append
} tsp05_RteFileMode_Enum;
typedef tsp05_RteFileMode_Enum tsp05_RteFileMode_Param;
typedef ENUM_TYPE_MSP00 (          tsp05_RteFileMode, tsp05_RteFileMode_Enum, sp5vf_append, unsigned char);

typedef enum tsp05_RteBufferingKind_Enum
{
    sp5bk_buffered,
    sp5bk_unbuffered,
    sp5bk_disksynced
} tsp05_RteBufferingKind_Enum;
typedef tsp05_RteBufferingKind_Enum tsp05_RteBufferingKind_Param;
typedef ENUM_TYPE_MSP00 (          tsp05_RteBufferingKind, tsp05_RteBufferingKind_Enum, sp5bk_disksynced, unsigned char);

typedef enum tsp05_RteCloseOption_Enum
{
    sp5vf_close_normal,
    sp5vf_close_rewind,
    sp5vf_close_next_tape
} tsp05_RteCloseOption_Enum;
typedef tsp05_RteCloseOption_Enum tsp05_RteCloseOption_Param;
typedef ENUM_TYPE_MSP00 (          tsp05_RteCloseOption, tsp05_RteCloseOption_Enum, sp5vf_close_next_tape, unsigned char);

typedef enum tsp05_RteSeekKind_Enum
{
    sp5vf_seek_begin,
    sp5vf_seek_cur,
    sp5vf_seek_end
} tsp05_RteSeekKind_Enum;
typedef tsp05_RteSeekKind_Enum tsp05_RteSeekKind_Param;
typedef ENUM_TYPE_MSP00 (          tsp05_RteSeekKind, tsp05_RteSeekKind_Enum, sp5vf_seek_end, unsigned char);
/* tsp05_RteSize64 ignored */
     typedef teo00_Uint8 tsp05_RteSize64;

typedef struct                     tsp05_RteFileInfo
{
    pasbool             sp5fi_exists;
    pasbool             sp5fi_readable;
    pasbool             sp5fi_writeable;
    pasbool             sp5fi_is_link;
    tsp00_Longint       sp5fi_size;
    tsp00_Longint       sp5fi_size_x;
    tsp05_RteSize64     sp5fi_size64;
    tsp00_Date          sp5fi_date_modified;
    tsp00_Time          sp5fi_time_modified;
    tsp00_VfType        sp5fi_media_kind;
} tsp05_RteFileInfo;
     typedef struct tsp05_SpecialEnumeration tsp05_SpecialEnumeration;

typedef enum tsp05_SpecialdevKind_Enum
{
    sdk_none_sp05,
    sdk_tapes_sp05,
    sdk_drives_sp05
} tsp05_SpecialdevKind_Enum;
typedef tsp05_SpecialdevKind_Enum tsp05_SpecialdevKind_Param;
typedef ENUM_TYPE_MSP00 (          tsp05_SpecialdevKind, tsp05_SpecialdevKind_Enum, sdk_drives_sp05, unsigned char);

struct tsp05_TemplateDummy {
    tsp05_RteFileWarning dummy_tsp05_RteFileWarning;
    tsp05_RteSeekKind    dummy_tsp05_RteSeekKind;
    tsp05_SpecialdevKind dummy_tsp05_SpecialdevKind;
    tsp05_RteFileMode    dummy_tsp05_RteFileMode;
    tsp05_RteDataKind    dummy_tsp05_RteDataKind;
    tsp05_RteBufferingKind dummy_tsp05_RteBufferingKind;
    tsp05_RteCloseOption dummy_tsp05_RteCloseOption;
};


#endif
