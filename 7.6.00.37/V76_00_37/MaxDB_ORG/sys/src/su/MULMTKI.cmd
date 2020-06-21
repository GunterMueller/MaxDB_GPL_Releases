*   *ID* MULMTKI  DBCMD    changed on 1992-09-28-13.24.40 by ADMIN     *
create table tab1 (a1 fixed(3) key, a2 char(3) ascii key) !
create table tab2 (b1 fixed(3), b2 char(3) ascii, 
b3 char(3) ascii, b4 char(4)) !
create table tab3 (c1 fixed(3), c2 char(3) ascii, 
c3 char(3) ascii, c4 char(4)) !
create table tab4 (d1 fixed(3), d2 char(3) ascii, 
d3 char(3) ascii, d4 char(4)) !
insert into tab1 values ( 1, 'abc') !
insert into tab1 values ( 2, 'abc') !
insert into tab1 values ( 3, 'def') !
insert into tab1 values ( 4, 'def') !
insert into tab1 values ( 5, 'qwe') !
insert into tab2 values ( 1, 'abc', 'ccc', 'ddd') !
insert into tab2 values ( 2, 'def', 'ccc', 'ddd') !
insert into tab2 values ( 3, 'def', 'ccc', 'ddd') !
insert into tab2 values ( 4, 'hij', 'ccc', 'ddd') !
insert into tab2 values ( 6, 'hij', 'ccc', 'ddd') !
insert into tab3 values ( 1, null,  'ccc', 'ddd') !
insert into tab3 values ( 1, 'abc', 'ccc', 'ddd') !
insert into tab3 values ( 2, 'hij', 'ccc', 'ddd') !
insert into tab3 values ( 3, 'abc', 'ccc', 'ddd') !
insert into tab3 values ( 4, 'fed', 'ccc', 'ddd') !
insert into tab3 values ( 8, 'fed', 'ccc', 'ddd') !
insert into tab4 values ( 1, 'abc', 'ccc', 'ddd') !
insert into tab4 values ( 2, 'hij', 'ccc', 'ddd') !
insert into tab4 values ( 3, 'abc', 'ccc', 'ddd') !
insert into tab4 values ( 4, 'fed', 'ccc', 'ddd') !
insert into tab4 values ( 9, 'fed', 'ccc', 'ddd') !
update statistics kern.* !
commit work !
explain select * from tab1, tab2, tab3
where a1 = b1 and
      a2 = c2 !
select * from tab1, tab2, tab3
where a1 = b1 and
      a2 = c2 !
fetch !
commit work !
explain select * from tab1, tab2, tab3, tab4
where a1 = b1 and
      a2 = c2 and
      c4 = d4 and
      b3 = d3    !
select tab2.*  from tab1, tab2, tab3, tab4
where a1 = b1 and
      a2 = c2 and
      c4 = d4 and
      b3 = d3    !
fetch !
create index mindex1 on tab4 (d4, d3) !
explain select * from tab1, tab2, tab3, tab4
where a1 = b1 and
      a2 = c2 and
      c4 = d4 and
      b3 = d3    !
commit work !
select * from tab1, tab2, tab3, tab4
where a1 = b1 and
      a2 = c2 and
      c4 = d4 and
      b3 = d3    !
fetch !
create index mindex2 on tab4 (d3 desc, d4 asc) !
explain select * from tab1, tab2, tab3, tab4
where a1 = b1 and
      a2 = c2 and
      c4 = d4 and
      b3 = d3    !
select a1, b1, a2, c2, c4, d4, d3, b3 from tab1, tab2, tab3, tab4
where a1 = b1 and
      a2 = c2 and
      c4 = d4 and
      b3 = d3    !
fetch !
