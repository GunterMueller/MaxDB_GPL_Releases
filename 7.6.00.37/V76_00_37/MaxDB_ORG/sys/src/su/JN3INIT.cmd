*   *ID* JN3INIT  DBCMD    changed on 1992-09-28-13.24.10 by ADMIN     *
create table t1 (
s1         char(19),
s2         char(29),
s3         char(39),
s4         char(49),
s5         char(59),
s6         char(69),
s7         fixed(10),
s8         fixed(10),
s9         fixed(10),
sec        char(06),
sen        fixed(04)
) !
insert t1 set
s1 = 't1___s1 nr 1',
s2 = 't1___s2 nr 1',
s3 = 't1___s3 nr 1',
s4 = 't1___s4 nr 1',
s5 = 't1___s5 nr 1',
s6 = 't1___s6 nr 1',
s7 = 1,
s8 = 8811,
sen = 4711,
sec = 'ABCDEF',
s9 = 999
!
insert t1 set
s1 = 't1___s1 nr 2',
s2 = 't1___s2 nr 2',
s3 = 't1___s3 nr 2',
s4 = 't1___s4 nr 2',
s5 = 't1___s5 nr 2',
s6 = 't1___s6 nr 2',
s7 = 2,
s8 = 8822,
sen = 4711,
sec = 'ABCDEF',
s9 = 999
!
create table t2 (
s1         char(19),
s2         char(29),
s3         char(39),
s4         char(49),
s5         char(59),
s6         char(69),
s7         fixed(10),
sec        char(21),
sen        fixed(08)
) !
insert t2 set
s1 = 't2___s1 nr 1',
s2 = 't2___s2 nr 1',
s3 = 't2___s3 nr 1',
s4 = 't2___s4 nr 1',
s5 = 't2___s5 nr 1',
s6 = 't2___s6 nr 1',
sen = 4711,
sec = 'ABCDEF               ',
s7 = 11111
!
insert t2 set
s1 = 't2___s1 nr 2',
s2 = 't2___s2 nr 2',
s3 = 't2___s3 nr 2',
s4 = 't2___s4 nr 2',
s5 = 't2___s5 nr 2',
s6 = 't2___s6 nr 2',
sen = 4711,
sec = 'ABCDEF',
s7 = 44444
!
create table t3 (
s1         char(19),
s2         char(29),
s3         char(39),
s4         char(49),
s5         char(59),
s6         char(69),
s7         fixed(10),
s8         fixed(10),
s9         fixed(10),
sec        char(30),
sen        fixed(12)
) !
insert t3 set
s1 = 't3___s1 nr 1',
s2 = 't3___s2 nr 1',
s3 = 't3___s3 nr 1',
s4 = 't3___s4 nr 1',
s5 = 't3___s5 nr 1',
s6 = 't3___s6 nr 1',
s7 = 31,
s8 = 300,
sen = 4711,
sec = 'ABCDEF                        ',
s9 = 3999
!
insert t3 set
s1 = 't3___s1 nr 2',
s2 = 't3___s2 nr 2',
s3 = 't3___s3 nr 2',
s4 = 't3___s4 nr 2',
s5 = 't3___s5 nr 2',
s6 = 't3___s6 nr 2',
s7 = 32,
s8 = 310,
sen = 4711,
sec = 'ABCDEF',
s9 = 3999
!
commit work !
