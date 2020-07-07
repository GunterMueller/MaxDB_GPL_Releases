
#include "vpr01Module.h"

typedef struct {
sqlgatype sqlga;
sqlgaentry sqlgae [sqlgamax];
sqlratype sqlra;
sqltatype sqlta;
sqloatype sqloa;
sqlmatype sqlma;
sqlmftype sqlmf;
sqlcxatype sqlcxa;
struct SQLERROR sqlem;
} p03sqlareas;

void *p03caci (sqlcatype *sqlcar, sqlxatype *sqlxar,
               tpr01_Precompiledfor pcfor);

