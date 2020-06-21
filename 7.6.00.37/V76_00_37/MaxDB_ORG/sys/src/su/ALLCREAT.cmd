*   *ID* ALLCREAT DBCMD    changed on 1992-09-28-13.21.26 by ADMIN     *
      create table t1 (s1 int key)!
      create table t2 (s3 int, s4 int, constraint c1 check s3 < s4)!
      create table t3 (s3 int, s4 int, constraint c1 check s3 < s4)
          with replication !
      create index "t1.s1"
	on t1( s1 ) !
      alter table t2 foreign key l1 (s3) references t1!
      analyze appl.proc !
          select * from sysdba.t1 !
      store procedure !
      store procedure all !
      create procedure appl.proc !
      commit!
