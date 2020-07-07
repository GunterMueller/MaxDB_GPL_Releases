
/* some common prolog for every module
 */

#ifndef GSP03_H
#define GSP03_H


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

#define csp3_eo_msg                         50000
    /**/
    /**/
    /* Number range 51000 to 51999 (ak layer) see gsp03_1 */
    /**/
    /**/
    /* Number range 52000 to 52999 (kb layer) see gsp03_2 */
    /**/
    /**/
    /* Number range 53000 to 53999 (bd layer) see gsp03_3 */
    /**/
#define csp3_msg                            54000
#define csp3_bad_page                       54001
#define csp3_sysbuf_storage_exceeded        54002
#define csp3_allocat                        54003
#define csp3_size_check                     54004
#define csp3_xparam_error                   54005
#define csp3_illegal_taskid                 54006
#define csp3_region_check                   54007
#define csp3_no_more_memory                 54008
#define csp3_g08x1_excl_check               54009
#define csp3_g20x1_unifill                  54010
#define csp3_structure_check                54011
#define csp3_move_error                     54111
    /**/
#define csp3_n_akcache                  "AK CACHE"
#define csp3_n_autosave                 "AUTOSAVE"
#define csp3_n_badindex                 "BADINDEX"
#define csp3_n_bufwriter                "BUFWRITE"
#define csp3_n_btree                    "B*TREE  "
#define csp3_n_check                    "CHECK   "
#define csp3_n_checkpoint               "CHKPOINT"
#define csp3_n_config                   "CONFIG  "
#define csp3_n_convcache                "CONVCACH"
#define csp3_n_data                     "DATA    "
#define csp3_n_datacache                "DATACACH"
#define csp3_n_distribution             "DISTRIB "
#define csp3_n_dyndata                  "dynDATA "
#define csp3_n_dynpool                  "dynpool "
#define csp3_n_fieldlist                "FIELDLST"
#define csp3_n_fill                     "FILL    "
#define csp3_n_fbm                      "FBM     "
#define csp3_n_hresult                  "HRESULT "
#define csp3_n_history                  "HISTORY "
#define csp3_n_io                       "I/O     "
#define csp3_n_index                    "INDEX   "
#define csp3_n_join                     "JOIN    "
#define csp3_n_lock                     "LOCK    "
#define csp3_n_log                      "LOG     "
#define csp3_n_memory                   "MEMORY  "
#define csp3_n_migration                "MIGRAT  "
#define csp3_n_move                     "MOVE    "
#define csp3_n_net                      "NET     "
#define csp3_n_obj                      "OBJECT  "
#define csp3_n_objlog                   "OBJLOG  "
#define csp3_n_order                    "ORDER   "
#define csp3_n_packet                   "PACKET  "
#define csp3_n_param                    "PARAM   "
#define csp3_n_region                   "REGION  "
#define csp3_n_restart                  "RESTART "
#define csp3_n_restore                  "RESTORE "
#define csp3_n_server                   "SERVER  "
#define csp3_n_size_check               "SIZECHEC"
#define csp3_n_save                     "SAVE    "
#define csp3_n_savepoint                "SAVPOINT"
#define csp3_n_aksemantic               "SEMANTIC"
#define csp3_n_show                     "SHOW    "
#define csp3_n_shutdown                 "SHUTDOWN"
#define csp3_n_syserror                 "SYSERROR"
#define csp3_n_tempcache                "TEMPCACH"
#define csp3_n_treelock                 "TREELOCK"
#define csp3_n_trigger                  "TRIGGER "
#define csp3_n_unicode                  "UNICODE "
#define csp3_n_view                     "VIEW    "

/******************************************
 *               T Y P E S                *
 ******************************************/


typedef enum tsp3_priority_Enum
{
    sp3p_nil,
    sp3p_syscon,
    sp3p_console,
    sp3p_knldiag
} tsp3_priority_Enum;
typedef tsp3_priority_Enum tsp3_priority_Param;
typedef ENUM_TYPE_MSP00 (          tsp3_priority, tsp3_priority_Enum, sp3p_knldiag, unsigned char);
    /**/

typedef enum tsp3_msg_type_Enum
{
    sp3m_nil,
    sp3m_error,
    sp3m_warning,
    sp3m_info
} tsp3_msg_type_Enum;
typedef tsp3_msg_type_Enum tsp3_msg_type_Param;
typedef ENUM_TYPE_MSP00 (          tsp3_msg_type, tsp3_msg_type_Enum, sp3m_info, unsigned char);

struct tsp03_TemplateDummy {
    tsp3_priority        dummy_tsp3_priority;
    tsp3_msg_type        dummy_tsp3_msg_type;
};


#endif
