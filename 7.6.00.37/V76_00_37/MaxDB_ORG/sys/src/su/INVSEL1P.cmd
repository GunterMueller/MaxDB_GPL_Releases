*   *ID* INVSEL1P.cmd      changed on 1996-02-14-11.00.00 by krischan  *
parsing!
declare erg cursor for select uk1, uk2  from u &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for select uk1, uk2, u5  from u &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for select uk1, uk2  from u order by u5 &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2  from u order by u5 desc &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u order by u5 &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u order by u5 desc &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for select uk1, uk2  from u order by u4 &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2  from u order by u4 desc &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for select uk1, uk2, u5  from u order by u4 &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u order by u4 desc &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for select uk1, uk2, u4  from u order by u4 &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u order by u4 desc &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for select uk1, uk2  from u where u5 = :a &1!
data!
executing!
SELECT !
1
c 20 'EEE'

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 = :a &1!
data!
executing!
SELECT !
1
c 20 'EEE'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 = :a &1!
data!
executing!
SELECT !
1
c 20 'EEE'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for select uk1, uk2  from u where u5 > :a &1!
data!
executing!
SELECT !
1
c 20 'EEE'

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 > :a &1!
data!
executing!
SELECT !
1
c 20 'EEE'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 > :a &1!
data!
executing!
SELECT !
1
c 20 'EEE'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2  from u where u5 < :a &1!
data!
executing!
SELECT !
1
c 20 'EEE'

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 < :a &1!
data!
executing!
SELECT !
1
c 20 'EEE'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 < :a &1!
data!
executing!
SELECT !
1
c 20 'EEE'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2  from u where u5 between :a and :b &1!
data!
executing!
SELECT !
1
c 20 'CCC'
c 20 'KKK'

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 between :a and :b &1!
data!
executing!
SELECT !
1
c 20 'CCC'
c 20 'KKK'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 between :a and :b &1!
data!
executing!
SELECT !
1
c 20 'CCC'
c 20 'KKK'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2  from u where u5 between :a and :b &1!
data!
executing!
SELECT !
1
c 20 'KKK'
c 20 'CCC'

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 between :a and :b &1!
data!
executing!
SELECT !
1
c 20 'KKK'
c 20 'CCC'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 between :a and :b &1!
data!
executing!
SELECT !
1
c 20 'KKK'
c 20 'CCC'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
pars_execute!
commit work release!
file connect ( kern test !
parsing!
declare erg cursor for 
select uk1, uk2  from u where u5 like :a &1!
data!
executing!
SELECT !
1
c 20 'F%'

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 like :a &1!
data!
executing!
SELECT !
1
c 20 'F%'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 like :a &1!
data!
executing!
SELECT !
1
c 20 'F%'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2  from u where u5 in (:a, :b, :c) &1!
data!
executing!
SELECT !
1
c 20 'LLL'
c 20 'CCC'
c 20 'GGG'

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for select uk1, uk2, u5  from u
where u5 in (:a, :b, :c) &1!
data!
executing!
SELECT !
1
c 20 'LLL'
c 20 'CCC'
c 20 'GGG'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for select uk1, uk2, u4  from u
where u5 in (:a, :b, :c) &1!
data!
executing!
SELECT !
1
c 20 'LLL'
c 20 'CCC'
c 20 'GGG'

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for select uk1, uk2  from u where u4 = 444 &1!
data!
executing!
SELECT !
1

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 = :a &1!
data!
executing!
SELECT !
1
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 = :a &1!
data!
executing!
SELECT !
1
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2  from u where u4 > :a &1!
data!
executing!
SELECT !
1
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 > :a &1!
data!
executing!
SELECT !
1
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 > :a &1!
data!
executing!
SELECT !
1
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2  from u where u4 < :a &1!
data!
executing!
SELECT !
1
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 < :a &1!
data!
executing!
SELECT !
1
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 < :a &1!
data!
executing!
SELECT !
1
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2  from u where u4 between :a and :b &1!
data!
executing!
SELECT !
1
n 10 222
n 10 555

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 between :a and :b &1!
data!
executing!
SELECT !
1
n 10 222
n 10 555

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 between :a and :b &1!
data!
executing!
SELECT !
1
n 10 222
n 10 555

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2  from u where u4 between :a and :b &1!
data!
executing!
SELECT !
1
n 10 555
n 10 222

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 between :a and :b &1!
data!
executing!
SELECT !
1
n 10 555
n 10 222

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 between :a and :b &1!
data!
executing!
SELECT !
1
n 10 555
n 10 222

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2  from u where u4 in (:a, :b, :c) &1!
data!
executing!
SELECT !
1
n 10 666
n 10 222
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x!
parsing!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 in (:a, :b, :c) &1!
data!
executing!
SELECT !
1
n 10 666
n 10 222
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
parsing!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 in (:a, :b, :c) &1!
data!
executing!
SELECT !
1
n 10 666
n 10 222
n 10 444

nodata!
pars_then_ex!
fetch erg into :w,:x,:y!
pars_execute!
