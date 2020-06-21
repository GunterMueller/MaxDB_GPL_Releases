*   *ID* JOINTES2 DBCMD    changed on 1992-09-28-13.24.13 by ADMIN     *
file CONNECT ( KERN TEST !
create table t1 (a1 char(5) key, am char(5) key,
                 a2 char(5) key, a3 char(5)) !
create table t2 (b1 char(5) key, bm char(5) key,
                 b2 char(5) key, b3 char(5) key,
b4 char(254), b5 char(254), b6 char(254), b7 char(254),
b8 char(254), b9 char(254), b10 char(254))!
insert t1 values ('aaaaa', 'multi', 'bbbbb', 'sssss') !
insert t1 values ('aaaaa', 'multi', 'ccccc', 'bbbbb') !
insert t1 values ('xxxxx', 'multi', 'ccccc', 'zzzzz') !
insert t2 values ('aaaaa', 'multi', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaa', 'multi', 'yyyyy', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaa', 'multi', 'yyyyz', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaa', 'multi', 'yyyxz', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('a',     'multi', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('ab',    'multi', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('ac',    'multi', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('b',     'multi', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('c',     'multi', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('xxxxx', 'multi', 'aaaaa', 'zzzzz',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('xxxxx', 'multi', 'aaaab', 'zzzzz',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('zzzzz', 'multi', 'aaaab', 'zzzzz',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('zzzzz', 'multi', 'aaaac', 'zzzzz',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('zzzzz', 'multi', 'aaaad', 'zzzzz',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
explain select a1, t2.* from <t2, t1>
where a1 = b1 and am = bm and a3 = b3 !
select a1, t2.* from <t2, t1> where a1 = b1 and am = bm and a3 = b3 !
fetch !
rollback !
commit release !
