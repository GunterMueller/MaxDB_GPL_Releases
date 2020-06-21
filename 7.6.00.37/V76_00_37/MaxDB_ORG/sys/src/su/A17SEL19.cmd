*   *ID* A17SEL19 DBCMD    changed on 1992-09-28-13.22.45 by ADMIN     *
* Fehlerprotokoll 155 !
file connect ( kern test !
create table t ( a fixed (5),
                 b fixed (5),
                 c fixed (5),
                 d fixed (5))!
insert t set a = 1, b = 0, c = 3, d = 0!
create index i1 on t (a, b)!
rollback release !
