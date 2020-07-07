
/* some common prolog for every module
 */

#ifndef VSP0031_H
#define VSP0031_H


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

#define csp31_min_text_field                    0
#define csp31_max_text_field                  255
    /* */

/******************************************
 *               T Y P E S                *
 ******************************************/


typedef enum tsp31_event_ident_Enum
{
    sp31ei_nil,
    sp31ei_filler1,
    sp31ei_autosave,
    sp31ei_filler3,
    sp31ei_filler4,
    sp31ei_db_filling_above_limit,
    sp31ei_db_filling_below_limit,
    sp31ei_event,
    sp31ei_log_above_limit,
    sp31ei_filler9,
    sp31ei_filler10,
    sp31ei_upd_stat_wanted,
    sp31ei_filler12,
    sp31ei_error,
    sp31ei_admin,
    sp31ei_backupresult,
    sp31ei_checkdata,
    sp31ei_databasefull,
    sp31ei_logfull,
    sp31ei_online,
    sp31ei_outofsessions,
    sp31ei_standby,
    sp31ei_systemerror,
    sp31ei_logsegmentfull,
    sp31ei_last_but_not_used
} tsp31_event_ident_Enum;
typedef tsp31_event_ident_Enum tsp31_event_ident_Param;
typedef ENUM_TYPE_MSP00 (          tsp31_event_ident, tsp31_event_ident_Enum, sp31ei_last_but_not_used, unsigned char);
    /* */

typedef enum tsp31_event_prio_Enum
{
    sp31ep_nil,
    sp31ep_low,
    sp31ep_medium,
    sp31ep_high
} tsp31_event_prio_Enum;
typedef tsp31_event_prio_Enum tsp31_event_prio_Param;
typedef ENUM_TYPE_MSP00 (          tsp31_event_prio, tsp31_event_prio_Enum, sp31ep_high, unsigned char);
    /* */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp31_event_text, csp31_max_text_field - csp31_min_text_field + 1);
typedef CSTRING_TYPE_MSP00      (  tsp31_event_textc, csp31_max_text_field - csp31_min_text_field + 1);
    /* */

typedef struct                     tsp31_event_description
{
    tsp00_Date          sp31ed_date;
    tsp00_Time          sp31ed_time;
    tsp00_Int4          sp31ed_value_1;
    tsp00_Int4          sp31ed_value_2;
    tsp00_Int4          sp31ed_eventcnt;
    tsp31_event_ident   sp31ed_ident;
    tsp31_event_prio    sp31ed_priority;
    tsp00_Int2          sp31ed_text_len;
    tsp31_event_text    sp31ed_text_value;
} tsp31_event_description;

struct tsp0031_TemplateDummy {
    tsp31_event_prio     dummy_tsp31_event_prio;
    tsp31_event_ident    dummy_tsp31_event_ident;
};


#endif
