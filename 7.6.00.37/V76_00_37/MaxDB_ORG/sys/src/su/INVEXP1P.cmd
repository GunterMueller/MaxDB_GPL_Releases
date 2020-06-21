*   *ID* INVSEL1P.cmd      changed on 1996-02-14-11.00.00 by krischan  *
pars_then_ex!
data!
explain
select &1 &2 uk1, uk2 &3 from u &4!

explain
select &1 &2 uk1, uk2, u5 &3 from u &4!

explain
select &1 &2 uk1, uk2 &3 from u order by u5 &4!

explain
select &1 &2 uk1, uk2 &3 from u order by u5 desc &4!

explain
select &1 &2 uk1, uk2, u5 &3 from u order by u5 &4!

explain
select &1 &2 uk1, uk2, u5 &3 from u order by u5 desc &4!

explain
select &1 &2 uk1, uk2 &3 from u order by u4 &4!

explain
select &1 &2 uk1, uk2 &3 from u order by u4 desc &4!

explain
select &1 &2 uk1, uk2, u5 &3 from u order by u4 &4!

explain
select &1 &2 uk1, uk2, u5 &3 from u order by u4 desc &4!

explain
select &1 &2 uk1, uk2, u4 &3 from u order by u4 &4!

explain
select &1 &2 uk1, uk2, u4 &3 from u order by u4 desc &4!

explain
select &1 &2 uk1, uk2 &3 from u where u5 = :a &4!
c 20 'EEE'

explain
select &1 &2 uk1, uk2, u5 &3 from u where u5 = :a &4!
c 20 'EEE'

explain
select &1 &2 uk1, uk2, u4 &3 from u where u5 = :a &4!
c 20 'EEE'

explain
select &1 &2 uk1, uk2 &3 from u where u5 > :a &4!
c 20 'EEE'

explain
select &1 &2 uk1, uk2, u5 &3 from u where u5 > :a &4!
c 20 'EEE'

explain
select &1 &2 uk1, uk2, u4 &3 from u where u5 > :a &4!
c 20 'EEE'

explain
select &1 &2 uk1, uk2 &3 from u where u5 < :a &4!
c 20 'EEE'

explain
select &1 &2 uk1, uk2, u5 &3 from u where u5 < :a &4!
c 20 'EEE'

explain
select &1 &2 uk1, uk2, u4 &3 from u where u5 < :a &4!
c 20 'EEE'

explain
select &1 &2 uk1, uk2 &3 from u where u5 between :a and :b &4!
c 20 'CCC'
c 20 'KKK'

explain
select &1 &2 uk1, uk2, u5 &3 from u where u5 between :a and :b &4!
c 20 'CCC'
c 20 'KKK'

explain
select &1 &2 uk1, uk2, u4 &3 from u where u5 between :a and :b &4!
c 20 'CCC'
c 20 'KKK'

explain
select &1 &2 uk1, uk2 &3 from u where u5 between :a and :b &4!
c 20 'KKK'
c 20 'CCC'

explain
select &1 &2 uk1, uk2, u5 &3 from u where u5 between :a and :b &4!
c 20 'KKK'
c 20 'CCC'

explain
select &1 &2 uk1, uk2, u4 &3 from u where u5 between :a and :b &4!
c 20 'KKK'
c 20 'CCC'

pars_execute!
commit work release!
file connect ( kern test !
pars_then_ex!

explain
select &1 &2 uk1, uk2 &3 from u where u5 like :a &4!
c 20 'F%'

explain
select &1 &2 uk1, uk2, u5 &3 from u where u5 like :a &4!
c 20 'F%'

explain
select &1 &2 uk1, uk2, u4 &3 from u where u5 like :a &4!
c 20 'F%'

explain
select &1 &2 uk1, uk2 &3 from u where u5 in (:a, :b, :c) &4!
c 20 'LLL'
c 20 'CCC'
c 20 'GGG'

explain
select &1 &2 uk1, uk2, u5 &3 from u
where u5 in (:a, :b, :c) &4!
c 20 'LLL'
c 20 'CCC'
c 20 'GGG'

explain
select &1 &2 uk1, uk2, u4 &3 from u
where u5 in (:a, :b, :c) &4!
c 20 'LLL'
c 20 'CCC'
c 20 'GGG'

explain
select &1 &2 uk1, uk2 &3 from u where u4 = 444 &4!

explain
select &1 &2 uk1, uk2, u4 &3 from u where u4 = :a &4!
n 10 444

explain
select &1 &2 uk1, uk2, u5 &3 from u where u4 = :a &4!
n 10 444

explain
select &1 &2 uk1, uk2 &3 from u where u4 > :a &4!
n 10 444

explain
select &1 &2 uk1, uk2, u4 &3 from u where u4 > :a &4!
n 10 444

explain
select &1 &2 uk1, uk2, u5 &3 from u where u4 > :a &4!
n 10 444

explain
select &1 &2 uk1, uk2 &3 from u where u4 < :a &4!
n 10 444

explain
select &1 &2 uk1, uk2, u4 &3 from u where u4 < :a &4!
n 10 444

explain
select &1 &2 uk1, uk2, u5 &3 from u where u4 < :a &4!
n 10 444

explain
select &1 &2 uk1, uk2 &3 from u where u4 between :a and :b &4!
n 10 222
n 10 555

explain
select &1 &2 uk1, uk2, u4 &3 from u where u4 between :a and :b &4!
n 10 222
n 10 555

explain
select &1 &2 uk1, uk2, u5 &3 from u where u4 between :a and :b &4!
n 10 222
n 10 555

explain
select &1 &2 uk1, uk2 &3 from u where u4 between :a and :b &4!
n 10 555
n 10 222

explain
select &1 &2 uk1, uk2, u4 &3 from u where u4 between :a and :b &4!
n 10 555
n 10 222

explain
select &1 &2 uk1, uk2, u5 &3 from u where u4 between :a and :b &4!
n 10 555
n 10 222

explain
select &1 &2 uk1, uk2 &3 from u where u4 in (:a, :b, :c) &4!
n 10 666
n 10 222
n 10 444

explain
select &1 &2 uk1, uk2, u4 &3 from u where u4 in (:a, :b, :c) &4!
n 10 666
n 10 222
n 10 444

explain
select &1 &2 uk1, uk2, u5 &3 from u where u4 in (:a, :b, :c) &4!
n 10 666
n 10 222
n 10 444

pars_execute!
