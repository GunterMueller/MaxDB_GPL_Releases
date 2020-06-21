*   *ID* N23SEL13 DBCMD    changed on 1992-09-28-13.25.37 by ADMIN     *
file connect ( kern test !
* Fehlerprotokoll 207 !
create table tabelle1 (sp1 char (5), sp2 char (5), sp3 char (5))!
insert tabelle1 values ('aaa', 'kkk', 'bbb')!
insert tabelle1 values ('aaa', 'bbb', 'aaa')!
insert tabelle1 values ('ddd', 'zzz', 'lll')!
insert tabelle1 values ('aaa', 'aaa', 'fff')!
select * from tabelle1 where sp3
 between tabelle1.sp1 and tabelle1.sp2 !
fetch !
select * from tabelle1 where sp3
 between sp1 and sp2 !
fetch !
rollback release !
