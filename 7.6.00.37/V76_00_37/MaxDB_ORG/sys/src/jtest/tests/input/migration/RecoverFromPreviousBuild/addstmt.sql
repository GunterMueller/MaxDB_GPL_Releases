//
// This file contains statements to be executed in addition to the contents of schema.sql 
// before creating a backup for use with RecoverFromPreviousBuild.
//
// All statements are to be executed as user testUser+"2", i.e. MIGRTST2.
//

//
// updatable join views
//
create table test1 ( k1 int, k2 int, c3 int, c4 int, primary key(k1,k2))
//
create index i1 on test1(k2,k1)
//
create table test2 ( k1 int, k2 int, c3 int, c4 int, primary key(k1,k2), foreign key(k1, k2) references test1(k1,k2))
//
create index i2 on test2(k2,k1)
//
create view testv( k1, k2, c3, c4, c5 ) as select t1.k1, t1.k2, t1.c3, t1.c4, t2.c4 from test1 t1, test2 t2 where t1.c4 = t2.c4 and t1.k1=t2.k1 and t1.k2=t2.k2 and t2.k1 = 1 with check option
//
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,1)
//
insert into testv(k1,k2,c3,c4,c5) values (1,3,1,2,2)

