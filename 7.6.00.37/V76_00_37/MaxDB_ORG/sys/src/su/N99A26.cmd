*   *ID* N99A26   DBCMD    changed on 1992-09-28-13.27.18 by ADMIN     *
file connect ( kern test !
* Kernel error 137 !
create table t ( a char ( 8),
b char (8),
c char (8),
d char (8),
e char (254),
f char (254),
g char (254),
h char (254),
i char (254),
j char (127),
k char (127),
l char (127),
m char (127))!
insert t values ('aa', 'bb', 'cc', 'dd',
'eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee',
'fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff',
'ggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg',
'hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh hhhh',
'iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii',
'jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj',
'kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk',
'lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll',
'mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm')!
create unique index i1 on t (a, b, c, d) !
create table t1 (a1 char (8), b1 char (8))!
create unique index i1 on t1 (a1, b1) !
insert t1 set a1 = 'aaa', b1 = 'dd'!
pars_then_ex !
data !
select * from t, t1 where a = :a and b = :b and c = :c and d > :d and
a1 = :a1 and d = b1 !
c 8 'aa' c 8 'bb' c 8 'cc' c 8 'a' c 8 'aaa'

nodata !
fetch into :a, :b, :c, :d, :e, :f, :g, :h, :i, :j, :k, :l, :m, :a1, :b1 !
pars_execute !
drop table t !
drop table t1 !
commit release !
