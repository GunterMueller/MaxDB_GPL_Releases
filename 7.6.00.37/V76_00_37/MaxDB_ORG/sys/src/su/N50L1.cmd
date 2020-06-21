*   *ID* N50L1    DBCMD    changed on 1992-09-28-13.26.48 by ADMIN     *
 *** getestet wird alter table foreign key ... mit SYNONYMEN
 *** getestet wird Katalog-Select mit und ohne SYNONYME
 ****************************************************** !
file connect ( kern test !
create table t1 ( t1xc1 fixed(5) key,
                  t1xc2 char(5) key,
                  t1xc3 fixed(5),
                  t1xc4 char(5),
                  t1Xc5 fixed(5) not null,
                  t1xc6 char(5) not null,
                  t1xc7 fixed(5) default 1,
                  t1xc8 char(5) default 'AAAA' ) !
create table t2 ( t2xc1 fixed(5) key,
                  t2xc2 char(5) key,
                  t2xc3 fixed(5),
                  t2xc4 char(5),
                  t2Xc5 fixed(5) not null,
                  t2xc6 char(5) not null,
                  t2xc7 fixed(5) default 1,
                  t2xc8 char(5) default 'AAAA' ) !
create table t3 ( t3xc1 fixed(5) key,
                  t3xc2 char(5) key,
                  t3xc3 fixed(5),
                  t3xc4 char(5),
                  t3Xc5 fixed(5) not null,
                  t3xc6 char(5) not null,
                  t3xc7 fixed(5) default 1,
                  t3xc8 char(5) default 'AAAA' ) !
create table t4 ( t4xc1 fixed(5) key,
                  t4xc2 char(5) key,
                  t4xc3 fixed(5),
                  t4xc4 char(5),
                  t4Xc5 fixed(5) not null,
                  t4xc6 char(5) not null,
                  t4xc7 fixed(5) default 1,
                  t4xc8 char(5) default 'AAAA' ) !
create table t5 ( t5xc1 fixed(5) key,
                  t5xc2 char(5) key,
                  t5xc3 fixed(5),
                  t5xc4 char(5),
                  t5Xc5 fixed(5) not null,
                  t5xc6 char(5) not null,
                  t5xc7 fixed(5) default 1,
                  t5xc8 char(5) default 'AAAA' ) !
create table t6 ( t6xc1 fixed(5) key,
                  t6xc2 char(5) key,
                  t6xc3 fixed(5),
                  t6xc4 char(5),
                  t6Xc5 fixed(5) not null,
                  t6xc6 char(5) not null,
                  t6xc7 fixed(5) default 1,
                  t6xc8 char(5) default 'AAAA' ) !
create table t7 ( t7xc1 fixed(5) key,
                  t7xc2 char(5) key,
                  t7xc3 fixed(5),
                  t7xc4 char(5),
                  t7Xc5 fixed(5) not null,
                  t7xc6 char(5) not null,
                  t7xc7 fixed(5) default 7,
                  t7xc8 char(5) default 'BBBB' ) !
create table t8 ( t8xc1 fixed(5) key,
                  t8xc2 char(5) key,
                  t8xc3 fixed(5),
                  t8xc4 char(5),
                  t8Xc5 fixed(5) not null,
                  t8xc6 char(5) not null,
                  t8xc7 fixed(5) default 1,
                  t8xc8 char(5) default 'AAAA' ) !
commit work !
alter table t1 foreign key l11d1 ( t1xc7, t1xc8 )
   references t1 on delete set null !
alter table kern.t2 foreign key l12r1 ( t2xc3,t2xc4 )
   references t1 (t1xc1, t1xc4)!
create index iii on t1 (t1xc1,t1xc4)!
alter table kern.t2 foreign key l12r1 ( t2xc3,t2xc4 )
   references t1 (t1xc1, t1xc4)!
drop index iii on t1 !
create unique index iii on t1 (t1xc1,t1xc4)!
alter table kern.t2 foreign key l12r1 ( t2xc3,t2xc4 )
   references t1 (t1xc1, t1xc4)!
drop index iii on t1 !
alter table kern.t2 drop foreign key l12r1 !
drop index iii on t1 !
create index iii on t2 ( t2xc3,t2xc4 ) !
alter table kern.t2 foreign key l12r1 ( t2xc3,t2xc4 )
   references t1 (t1xc1, t1xc4)!
create unique index iii on t1 (t1xc1,t1xc4)!
alter table kern.t2 foreign key l12r1 ( t2xc3,t2xc4 )
   references t1 (t1xc1, t1xc4)!
alter table kern.t2 drop foreign key l12r1 !
drop index iii on t2 !
rollback !
alter table kern.t2 foreign key l12r1 ( t2xc3 )
   references t1 (t1xc1)!
alter table kern.t2 foreign key l12r1 ( t2xc3, t2xc4 )
   references t1 !
alter table t3 foreign key l23c1 ( t3xc1, t3xc2 )
   references kern.t2 on delete cascade !
alter table t4 foreign key l24n1 ( t4xc3, t4xc4 )
   references t2 on delete set null !
alter table t5 foreign key l25d1 ( t5xc7, t5xc8 )
   references t2 on delete set default !
alter table t3 foreign key l13r1 ( t3xc3, t3xc4 )
   references t1 on delete restrict !
commit work !
create synonym synxt2 for t2 !
create synonym synxt7 for t7 !
create synonym synxt8 for t8 !
alter table t6 foreign key l26c1 ( t6xc1, t6xc2 )
   references synxt2 on delete cascade !
alter table synxt7 foreign key l27d1 ( t7xc7, t7xc8 )
   references t2 on delete set default !
alter table synxt8 foreign key l28r1 ( t8xc3, t8xc4 )
   references synxt2 on delete restrict !
alter table t8 foreign key l28n1 ( t8.t8xc3, t8xc4 )
   references t2 on delete set null !
commit work !
file sel_references ( KERN T2 !
drop synonym synxt7 !
file sel_foreignkey ( KERN T7 !
file sel_foreignkey ( KERN T1 !
file sel_foreignkey ( KERN T2 !
file sel_foreignkey ( KERN T3 !
file sel_references ( KERN T1 !
file sel_references ( KERN T2 !
file sel_references ( KERN T3 !
file sel_references ( KERN T4 !
commit work !
drop table t1 !
file sel_references ( KERN T1 !
file sel_foreignkey ( KERN T1 !
alter table t2 drop foreign key l12r1 !
drop table t3 !
file sel_foreignkey ( KERN T2 !
alter table t3 drop foreign key l23c1 !
drop table t2 !
drop table t4 !
commit work release !
