*   *ID* JOININV  DBCMD    changed on 1992-09-28-13.24.11 by ADMIN     *
file CONNECT ( KERN TEST !
create table t1 (a1 char(120) key, a2 char(5) key, a3 char(5)) !
create index "t1.a3"
	on t1( a3 ) !
create table t2 (b1 char(5) key, b2 char(5), b3 char(5),
b4 char(254), b5 char(254), b6 char(254), b7 char(254),
b8 char(254), b9 char(254), b10 char(254))!
insert t1 values ('aaaaa', 'ccccc', 'bbbbb') !
insert t1 values ('aaaab', 'ccccc', 'bbbbb') !
insert t1 values ('aaaac', 'ccccc', 'bbbbb') !
insert t1 values ('aaaad', 'ccccc', 'bbbbb') !
insert t1 values ('aaaae', 'ccccc', 'bbbbb') !
insert t1 values ('aaaaf', 'ccccc', 'bbbbb') !
insert t1 values ('aaaag', 'ccccc', 'bbbbb') !
insert t1 values ('aaaah', 'ccccc', 'bbbbb') !
insert t1 values ('aaaai', 'ccccc', 'bbbbb') !
insert t1 values ('aaaaj', 'ccccc', 'bbbbb') !
insert t1 values ('aaaak', 'ccccc', 'bbbbb') !
insert t1 values ('aaaal', 'ccccc', 'bbbbb') !
insert t1 values ('aaaam', 'ccccc', 'bbbbb') !
insert t1 values ('aaaan', 'ccccc', 'bbbbb') !
insert t1 values ('aaaao', 'ccccc', 'bbbbb') !
insert t1 values ('aaaap', 'ccccc', 'bbbbb') !
insert t1 values ('aaaaq', 'ccccc', 'bbbbb') !
insert t1 values ('aaaar', 'ccccc', 'bbbbb') !
insert t1 values ('aaaas', 'ccccc', 'bbbbb') !
insert t1 values ('aaaat', 'ccccc', 'bbbbb') !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('bbbbb', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
explain select a1, t2.* from <t2, t1> where a3 = b3 !
select a1, t2.* from <t2, t1> where a3 = b3 !
fetch !
commit work release !
