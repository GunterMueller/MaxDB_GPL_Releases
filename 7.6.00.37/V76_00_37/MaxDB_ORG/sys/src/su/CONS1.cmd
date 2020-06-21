*   *ID* CONS1    DBCMD    changed on 1992-09-28-13.22.58 by ADMIN     *
create table a
 (a1 fixed  (10) key
                 constraint a1 > a2 and a2 > a3 or a1=1 or a3=a2,
  a2 fixed   (4) default 2
                 constraint a2 > a3 and (a2 = a1 or a1 > a3 and
                 (a1 <> 0 or a1 = 2)),
  a3 fixed   (4) default 1
                 constraint a3 = a1 or a3 is null or a3 = 1,
  a4 char  (200) constraint a4 = 'ein' & chr(a3, 1) & chr(a2, 1)
                 & chr(a1, 1) & 'aus' or a4 is null,
  a5 char  (200) check
                 a5 = substr(a4, 6, 4) and substr(a4, 4, 3) > '012'
                 or substr(a6, 1) > 'm' or a5 is null,
  a6 char  (200) default 'm'
                 range between 'e' and 'o' )!
insert a values (6, default, default, 'ein126aus', '6aus', default) !
insert a values (6, default, default, 'ein126aus', '6aus', 'p') !
insert a values (6, default, default, 'ein126aus', '26aus', default) !
insert a values (6, default, default, 'ein621aus', '6aus', default) !
insert a values (6, default, 2      , 'ein126aus', '6aus', default) !
insert a values (6, 1      , default, 'ein126aus', '6aus', default) !
insert a values (1, default, default, 'ein126aus', '6aus', default) !
select * from a!
fetch!
create table b
 (b1 int            key
                    constraint b1 > b2 and b2 > b3 or b1 = 1 or b3 = b2,
  b2 int            key
                    default 2
                    constraint b2 > b3 and (b2 = b1 or b1 > b3 and
                    (b1 <> 0 or b1 = 2)),
  b3 int            key
                    default 1
                    constraint b3 = b1 or b3 is null or b3 > 0,
  b31 varchar (254) default 'c',
  b4  varchar (254) ascii default 'c'
                    constraint b4 > 'a' or b4 is null,
  b5  varchar (254) check b5 = substr(b31, 2, 3) and
                    substr(b4, 2, 3) = 'abc' or
                    substr(b6, 2, 1) > 'a' or b5 is null,
  b6  varchar (254) default 'n'
                    range between 'f' and 'p' )!
insert b values (6, 3, 2, ' abc', 'xabcx', 'abc', default) !
insert b values (6, 3, 2, ' abc', 'xabcx', 'abc', 'q') !
insert b values (6, 3, 2, ' abc', 'xabcx', 'ab', default) !
insert b values (6, 3, 2, ' abc', ' abc ', 'abc', default) !
insert b values (6, 3, 2, 'abc', 'xabcx', 'abc', default) !
insert b values (6, 3, 0, ' abc', 'xabcx', 'abc', default) !
insert b values (6, 2, 2, ' abc', 'xabcx', 'abc', default) !
insert b values (3, 3, 2, ' abc', 'xabcx', 'abc', default) !
select * from b!
fetch!
alter table b add constraint b31 check b31 > 'd'!
alter table b foreign key l (b1) references a!
create view c (c1, c2, c3, c4, c5) as select a.a1, b.b2, b.b3, a.a4,
               a.a5 from a, b where a.a1 = b.b1 with check option!
select * from c !
fetch!
insert c values (8, 4, 2, 'ein128aus', '8aus')!
select * from a!
fetch!
select * from b!
fetch!
select * from c!
fetch!
file sel_table1 ( A!
file sel_table1 ( B!
file sel_table1 ( C!
file sel_view ( C !
file sel_cols_tab ( A !
file sel_cols_tab ( B !
file sel_cols_tab ( C !
file sel_column ( C C1 !
file sel_column ( C C2 !
file sel_column ( C C3 !
file sel_column ( C C4 !
file sel_column ( C C5 !
 *---------!
file sel_checkdef ( BUBU A A1 !
file sel_checkdef ( BUBU A A2 !
file sel_checkdef ( BUBU A A3 !
file sel_checkdef ( BUBU A A4 !
file sel_checkdef ( BUBU A A5 !
file sel_checkdef ( BUBU C A1 !
file sel_checkdef ( BUBU C A2 !
file sel_checkdef ( BUBU C A3 !
file sel_checkdef ( BUBU C A4 !
file sel_checkdef ( BUBU C A5 !
 *---------!
file sel_checkdef ( BUBU C C1 !
file sel_checkdef ( BUBU C C2 !
file sel_checkdef ( BUBU C C3 !
file sel_checkdef ( BUBU C C4 !
file sel_checkdef ( BUBU C C5 !
alter table c alter constraint c3 check b3 = b2 or b3 is null or b3 > 0!
alter table b alter constraint b3 check b3 = b2 or b3 is null or b3 > 0!
file sel_checkdef ( BUBU B B3 !
file sel_checkdef ( BUBU C C3 !
update b set b4 = rpad(b4, 254 - LENGTH(b4), '=')!
select * from b!
fetch!
alter table b column b4 varchar (255)!
update b set b4 = rpad(b4, 1, 'z')!
select * from b!
fetch!
file sel_checkdef ( BUBU A A1 !
file sel_checkdef ( BUBU A A2 !
file sel_checkdef ( BUBU A A3 !
file sel_checkdef ( BUBU A A4 !
file sel_checkdef ( BUBU A A5 !
file sel_checkdef ( BUBU A A6 !
alter table a alter constraint a6 check a6 between 'b' and 'y' !
file sel_checkdef ( BUBU A A6 !
drop table a !
drop table b !
commit!
create table a (
   bg  char (1) constraint  bg  check  bg  in ('a','b'),
  "bh" char (1) constraint "bh" check "bh" in ('a','b'),
   bi  char (1) constraint             bi  in ('a','b'),
  "bj" char (1) constraint            "bj" in ('a','b')
)!
alter table a add ( cg  char (1) constraint  cg  check  cg  in ('a','b'))!
alter table a add ("ch" char (1) constraint "ch" check "ch" in ('a','b'))!
alter table a add ( ci  char (1) constraint             ci  in ('a','b'))!
alter table a add ("cj" char (1) constraint            "cj" in ('a','b'))!
file sel_checkdef ( BUBU A CG !
file sel_checkdef ( BUBU A ch !
file sel_checkdef ( BUBU A CI !
file sel_checkdef ( BUBU A cj !
drop table a !
commit!
