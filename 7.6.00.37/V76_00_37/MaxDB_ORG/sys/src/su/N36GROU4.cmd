*   *ID* N36GROU4 DBCMD    changed on 1992-09-28-13.26.25 by ADMIN     *
file connect ( kern test !
create table t ( a char (195) ascii, c char (30) ascii, 
                 d char (20)  ascii, b fixed (8),
                 e char (10)  ascii )!
insert t values ('aaa', 'ccc', 'ddd', 1, 'eee')!
insert t values ('aaa', 'ccc', 'ddd', 1, 'eee')!
insert t values ('aaa', 'ccc', 'ddd', 2, 'eee')!
insert t values ('bbb', 'ccc', 'ddd', 1, 'eee')!
insert t values ('bbb', 'ccc', 'ddd', 3, 'eee')!
select sum(distinct b), a from t group by a,c !
fetch!
select sum(distinct b), a from t group by a,c,d !
fetch!
select sum(distinct b), count(distinct c), a from t group by a,d,e !
fetch!
select sum(distinct b),                    a from t group by a,d,e !
fetch!
drop table t !
* PTS 1117636 / 1118056 !
select dummy, TIMESTAMP from dual group by dummy!
fetch!
commit work release !
* PTS 1122141-3 !
file connect ( kern test !
create table MyTable(TableID varchar(100) not null,
IndexID int not null DEFAULT 0, primary key (TableID) )!
insert into MyTable Values ('Test1',1)!
insert into MyTable Values ('Test2',1)!
insert into MyTable Values ('Test3',2)!
insert into MyTable Values ('Test4',2)!
SELECT TableID FROM MyTable GROUP BY TableID
ORDER BY MAX(IndexID) DESC, TableID ASC!
fetch!
rollback release !

