file connect ( kern test !
create table ttt (a char (8) key)!
create table xxx (a char (8), b char (8), c char (8), d char (8),
e char (9), f char (2), g char (8) references ttt)!
file sel_foreignkey ( KERN XXX !
file sel_references ( KERN TTT !
rollback release !
file connect ( kern test !
create table ttt (a char (8) key, ttb char(8), 
    constraint c1_ttt unique(ttb))!
create table xxx (a char (8), b char (8), c char (8), d char (8),
e char (9), f char (2), g char (8) ,
h char(8) references ttt(ttb) )!
file sel_foreignkey ( KERN XXX !
file sel_references ( KERN TTT !
rollback release !
file connect ( kern test !
create table ttt (a char (8) key, ttb char(8), 
    constraint c1_ttt unique(ttb))!
create table xxx (a char (8), b char (8), c char (8), d char (8),
e char (9), f char (2), g char (8) references ttt,
h char(8) references ttt(ttb) )!
file sel_foreignkey ( KERN XXX !
file sel_references ( KERN TTT !
rollback release !
* PTS 1121138-40 !
file connect ( kern test sqlmode oracle !
create table t ( a char (8) primary key)!
create table t1 ( a1 char (8) constraint myfirstfk references t)!
create table t1a ( a1 char (8) not null 
           constraint notnullfk1 references t)!
create table t2 ( a1 char (8), 
constraint mysecondfk foreign key (a1) references t)!

select OWNER,TABLENAME,FKEYNAME from DOMAIN.FOREIGNKEYCOLUMNS
where REFOWNER = 'KERN'
and REFTABLENAME = 'T'!
fetch into :a, :b, :c!
drop table t !
drop table t1 !
drop table t1a !
drop table t2 !
commit release !
