
/* some common prolog for every module
 */

#ifndef VSP0011_H
#define VSP0011_H


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

#define csp11_AUTO_RECREATE_BAD_INDEXES  "AUTO_RECREATE_BAD_INDEXES       "

/******************************************
 *               T Y P E S                *
 ******************************************/

typedef tsp00_XpKeyType            tsp11_ConfParamName;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tsp11_ConfParamValue, 256);
typedef CSTRING_TYPE_MSP00      (  tsp11_ConfParamValuec, 256);

typedef enum tsp11_ConfParamReturnValue_Enum
{
    ok_sp11,
    key_not_found_sp11,
    cannot_change_online_sp11,
    no_change_permission_sp11,
    type_mismatch_sp11,
    not_ok_sp11
} tsp11_ConfParamReturnValue_Enum;
typedef tsp11_ConfParamReturnValue_Enum tsp11_ConfParamReturnValue_Param;
typedef ENUM_TYPE_MSP00 (          tsp11_ConfParamReturnValue, tsp11_ConfParamReturnValue_Enum, not_ok_sp11, unsigned char);

struct tsp0011_TemplateDummy {
    tsp11_ConfParamReturnValue dummy_tsp11_ConfParamReturnValue;
};


#endif
