file connect ( kern test !
 * In Rel. 3.0 wurden Wechsel von NOT NULL und NULL-Feldern
 * bei a61_fill_change nicht korrekt behandelt !
create table e (e1 char (8) key,
                e2 char (8) key,
                e3 char (8) key,
                e4 char (8) not null,
                e5 char (8) not null,
                e6 char (8) ,
                e7 char (8) ,
                e8 char (8) not null,
                e9 char (8) )!
insert e set e1 = '1', e2 = 'b', e3 = 'c', e4 = 'd', e5 = 'e',
         e8 = 'h'!
insert e set e1 = '2', e2 = 'b', e3 = 'c', e4 = 'd', e5 = 'e',
             e6 = 'f', e7 = 'g', e8 = 'h', e9 = 'i'!
select * from e !
fetch!
create table a (a1 char (8) ascii key,
                a2 char (8) ascii key,
                a3 char (8) ascii key,
                a4 char (8) ascii not null,
                a5 char (8) ascii not null,
                a6 char (8) ascii,
                a7 char (8) ascii,
                a8 char (8) ascii not null,
                a9 char (8) ascii)!
insert a set a1 = '1', a2 = 'b', a3 = 'c', a4 = 'd', a5 = 'e',
         a8 = 'h'!
insert a set a1 = '2', a2 = 'b', a3 = 'c', a4 = 'd', a5 = 'e',
             a6 = 'f', a7 = 'g', a8 = 'h', a9 = 'i'!
select * from a !
fetch!
rollback release !
file connect ( kern test !
create table d ( a fixed (10,0))!
insert d set a = 5846 !
insert d set a = 5866 !
insert d set a = 5876 !
insert d set a = 5886 !
select * from d where a between (5886-20) and (5886-1)!
fetch!
pars_then_ex !
select * from d where a between (5886-20) and (5886-1)!
fetch!
data !
select * from d where a between (:a - 20) and (5886-1)!
n 38 5886

nodata !
fetch!
data !
* will be seen as two parameter names, no minus (part of paramname) !
select * from d where a between (:a-20) and (:b-1)!
n 38 5886
n 38 5886

nodata !
fetch!
data !
select * from d where a between (:a - 20) and (:b - 1)!
n 38 5886
n 38 5886

nodata !
fetch!
pars_execute!
drop table d !
commit release !
