#ifndef DEBUG
#line 20 "vpa09tc"
#endif
#ifndef WIN32
typedef void *LPVOID;
#endif

#include "heo07.h"

#include "vpa00global.h"
#include "vpa40DBC.h"
#include "vpa60Stmt.h"

LPVOID pa09GetTLS(DWORD index);
VOID pa09FreeTLS();

#define PA07TLS_TRACE (0)
LPVOID pa07InitTraceValues();

#define PA09TLS_ASYNC (1)
typedef struct {
  DWORD recursive;
  tpa60Stmt *stmt_block_ptr;
  tpa40DBC  *dbc_block_ptr;
} pa09AsyncLocals;

LPVOID pa09InitAsyncValues();

#define PA09TLS_INDEXLAST PA09TLS_ASYNC

