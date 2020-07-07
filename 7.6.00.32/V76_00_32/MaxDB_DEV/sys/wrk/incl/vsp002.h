
/* some common prolog for every module
 */

#ifndef VSP002_H
#define VSP002_H


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

typedef tsp00_VFilename            tsp2_devname;
    /**/

typedef enum tsp2_dev_type_Enum
{
    sp2dt_sys,
    sp2dt_log,
    sp2dt_mirr_log,
    sp2dt_data,
    sp2dt_trace
} tsp2_dev_type_Enum;
typedef tsp2_dev_type_Enum tsp2_dev_type_Param;
typedef ENUM_TYPE_MSP00 (          tsp2_dev_type, tsp2_dev_type_Enum, sp2dt_trace, unsigned char);
    /**/

typedef enum tsp2_io_op_code_Enum
{
    sp2ioc_read,
    sp2ioc_write
} tsp2_io_op_code_Enum;
typedef tsp2_io_op_code_Enum tsp2_io_op_code_Param;
typedef ENUM_TYPE_MSP00 (          tsp2_io_op_code, tsp2_io_op_code_Enum, sp2ioc_write, unsigned char);
    /* THE FOLLOWING MUST BE REMOVED BY RTE BECAUSE IT IS OBSOLETE */
    /* LOOK INTO RTETask_ITask::TaskType - UH 2004-11-12           */

typedef enum tsp2_process_type_Enum
{
    sp2pt_rte,
    sp2pt_utility,
    sp2pt_timeout,
    sp2pt_user,
    sp2pt_filler1,
    sp2pt_log_writer,
    sp2pt_trace_writer,
    sp2pt_filler2,
    sp2pt_filler3,
    sp2pt_server,
    sp2pt_pager,
    sp2pt_filler4,
    sp2pt_event,
    sp2pt_backup,
    sp2pt_garbage_collector,
    sp2pt_filler5
} tsp2_process_type_Enum;
typedef tsp2_process_type_Enum tsp2_process_type_Param;
typedef ENUM_TYPE_MSP00 (          tsp2_process_type, tsp2_process_type_Enum, sp2pt_filler5, unsigned char);

struct tsp002_TemplateDummy {
    tsp2_io_op_code      dummy_tsp2_io_op_code;
    tsp2_process_type    dummy_tsp2_process_type;
    tsp2_dev_type        dummy_tsp2_dev_type;
};


#endif
