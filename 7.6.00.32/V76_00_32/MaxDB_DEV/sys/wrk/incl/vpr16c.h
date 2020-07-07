#define FCALL
#define FLOAT float
/*PRETTY*/
void p16profinit (sqlratype *sqlra, sqlgaentry *gaentry,
                  char *progn,  tsp00_Int2 lang,
                  struct SQLERROR *ret) ;
void p16profexec (sqlratype *sqlra, sqlgaentry *gaentry,
                  char *stmt, char *progn, char *modn,
                  tsp00_Int2 lang, tsp00_Int2 lino,
                  char *pid, struct SQLERROR *ret) ;
void p16delprof (sqlratype *sqlra, sqlgaentry *gaentry,
                 char *user, char *prog, tsp00_Int2 lang,
                 struct SQLERROR *ret) ;
void p16insprof (sqlratype *sqlra, sqlgaentry *gaentry,
                 char *user, char *prog, char *modn,
                 tsp00_Int2 lang, tsp00_Int4 lino, char *pid,
                 char *stmt, tsp00_Int4 deltatime,
                 struct SQLERROR *ret) ;
void p16updprof (sqlratype *sqlra, sqlgaentry *gaentry,
                 tsp00_KnlIdentifier VAR_ARRAY_REF user, tsp00_Name VAR_ARRAY_REF prog, tsp00_Name VAR_ARRAY_REF modn, tsp00_Int2 lang, tsp00_Int4 lino, char *pid,
                 tsp00_Int4 deltatime, struct SQLERROR *ret) ;

