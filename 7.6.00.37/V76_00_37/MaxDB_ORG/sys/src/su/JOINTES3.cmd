*   *ID* JOINTES3 DBCMD    changed on 1992-09-28-13.24.14 by ADMIN     *
file CONNECT ( KERN TEST !
create table t1 (a1 char(5) key, a2 char(5) key, a3 char(5)) !
create table t2 (b1 char(5) key, b2 char(5) key, b3 char(5) key,
b4 char(254), b5 char(254), b6 char(254), b7 char(254),
b8 char(254), b9 char(254), b10 char(254))!
insert t1 values ('aaaaa', 'bbbbb', 'sssss') !
insert t1 values ('aaaaa', 'ccccc', 'bbbbb') !
insert t1 values ('xxxxx', 'ccccc', 'zzzzz') !
insert t2 values ('aaaaa', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaa', 'yyyyy', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaa', 'yyyyz', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaa', 'yyyxz', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('a', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('ab', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('ac', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('b', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('c', 'fffff', 'xxxxx',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('xxxxx', 'aaaaa', 'zzzzz',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('xxxxx', 'aaaab', 'zzzzz',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('zzzzz', 'aaaab', 'zzzzz',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('zzzzz', 'aaaac', 'zzzzz',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('zzzzz', 'aaaad', 'zzzzz',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 and a3 = b3 !
fetch !
commit release !
