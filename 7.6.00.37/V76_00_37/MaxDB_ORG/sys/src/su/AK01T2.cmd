*   *ID* AK01T2   DBCMD    changed on 1992-09-28-13.22.04 by ADMIN     *
CREATE TABLE kern.T1 ( F1 CHAr (4)  key,
F2 FIXED (3,1 ) not null RANGE BETWEEN -1 AND + 3 ) !
create table kern.t2 like kern.t1 !
CREATE TABLE kern.T3 (F1 float(4) DEFAULT 4.55  RANGE IN (1,2,3,4 ) ,
F2 FIXED (3,1 ) RANGE BETWEEN 1 AND 3 ) !
rollback  work !
