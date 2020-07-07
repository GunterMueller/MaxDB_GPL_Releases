#ifndef RC_INVOKED
#include "vpr07Macro.h"
typedef struct
{
  char *szVersion;
  int  cbBuild;
} tpr_pcVersion;

#ifdef __cplusplus
#define externC extern "C"
#else
#define externC
#endif

externC void p07_copyright (void);
externC void p07senderid (tsp00_Int2 * xalang, tsp00_C8 senderid, tsp00_Int2 * cmpkind);
externC void p07version (tsp00_C40 * vers);
externC char *pr07RelNr();
externC char *pr07MinRelNr();

#endif

#define P07_PRODNAME   PROD_NAME_SP100
#if CORRECTION_LEVEL_SP100 < 10
#define P07_RELSTR       MAJOR_VERSION_NO_STR_SP100 "." MINOR_VERSION_NO_STR_SP100 ".0" CORRECTION_LEVEL_STR_SP100
#else
#define P07_RELSTR       MAJOR_VERSION_NO_STR_SP100 "." MINOR_VERSION_NO_STR_SP100 "." CORRECTION_LEVEL_STR_SP100
#endif
#define P07_FILEVERSION   MAJOR_VERSION_NO_SP100,MINOR_VERSION_NO_SP100,CORRECTION_LEVEL_SP100,BUILD_PREFIX_SP100
#define P07_FILEVERSION_STR   MAJOR_VERSION_NO_STR_SP100"."MINOR_VERSION_NO_STR_SP100"."CORRECTION_LEVEL_STR_SP100"."BUILD_PREFIX_STR_SP100

#define P07_BUILD         32

#define P07_FORMATVERSION(szRelNr) sprintf(szRelNr, "%d%02d%02d", MAJOR_VERSION_NO_SP100, MINOR_VERSION_NO_SP100, CORRECTION_LEVEL_SP100)

#define P07_PCR_VERSION COMP_NAME_C_PREC_SP100 " " P07_RELSTR "    Build %s"

#define P07_COMPANY         COMPANY_NAME_SP100
#define P07_PC_RUNTIME      COMP_NAME_C_PREC_SP100" Runtime"
#define P07_PC_PRECOMPILER  "C-PreComp"
#define P07_PC_COPYR_RESERV COPYRIGHT_SP100

#define P07_PC_COPYR_VERSION P07_PC_RUNTIME" "P07_FILEVERSION_STR" "DATE_SP100

#define P07_CMPCALL   "CAL"
#define P07_CMPCPC    "CPC"
#define P07_CMPCOB    "COB"
#define P07_CMPODBC   "ODB"
#define P07_CMPOCI    "OCI"

