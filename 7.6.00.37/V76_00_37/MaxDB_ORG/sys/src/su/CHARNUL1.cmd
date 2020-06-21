file connect ( kern test !
select hex(k1), hex(k2), &3 &4 from t !
fetch !
select hex(k1), hex(k2), &3 &4 from t order by &1!
fetch !
explain select k1, k2, &3 &4 from t where &1 = 'aaab'!
select hex(k1), hex(k2), &3 &4 from t where &1 = 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 from t where &1 = 'aaab '!
fetch !
select hex(k1), hex(k2), &3 &4 from t where &1 >= 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 from t where &1 <= 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 <= 'aaab' and &1 >= 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 <= 'aaac' and &1 >= 'aaaa'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 > 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 < 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 > 'aaab' and &1 < 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 > 'aaaa' and &1 < 'aaac'!
fetch !
select hex(k1), hex(k2), &3 &4 from t where &1 <> 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 between 'aaab' and 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 between 'aaa' and 'aab'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 between 'aaab' and 'aaac'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 between 'aaac' and 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 from t where &1 like 'aaab'!
fetch !
select hex(k1), hex(k2), &3 &4 from t where &1 like 'aaa%'!
fetch !
select hex(k1), hex(k2), &3 &4 from t where &1 like 'aaab%'!
fetch !
select hex(k1), hex(k2), &3 &4 
from t where &1 = 'aaab' and &2 like '%2'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 >= 'aaab' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 > 'aaab' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 <= 'aaab' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 < 'aaab' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 between 'aaab' and 'aaab' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 between 'aaaa' and 'aaab' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 between 'aaab' and 'aaaa' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 between 'aaab' and 'aaac' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 between 'aaaa' and 'aaac' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 like 'aaab' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 like 'aaab%' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 like 'aaa%' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 like 'aaa%b' and &2 = '12'!
fetch!
select hex(k1), hex(k2), &3 &4 
from t where &1 like 'aaa%b%' and &2 = '12'!
fetch!

declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.i1), hex(t2.i2)
from t t1, t t2 
where t1.&1 >= t2.&1 and t1.&1 > 'aaab' and t2.&2 = '11' for reuse !
fetch erg!

declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.i1), hex(t2.i2) 
from t t1, t t2 
where t1.&1 = t2.&1 and t2.&2 = '11' for reuse !
fetch erg!

declare erg cursor for 
select hex(k1), hex(k2), hex(i1), hex(i2) from t t1
where t1.&1 in
(select &1 from t where
 t.&1 = t1.&1 and t.&2 = '11') for reuse !
fetch erg!

declare erg cursor for 
select hex(k1), hex(k2), hex(i1), hex(i2) from t t1
where t1.&1 like 'aaa%' and t1.&1 in
(select &1 from t where
 t.&1 = t1.&1 and t.&2 = '12') for reuse !
fetch erg!

* parse_then_ex
pars_then_ex !
!* both order sequences by different indices are right 
!* because of data structure on disc 
explain
select /*+indexaccess("t.i1")*/hex(k1), hex(k2), &3 &4 from t 
 order by i1!

data!
select /*+indexaccess("t.i1")*/hex(k1), hex(k2), &3 &4 from t 
 order by i1!

nodata!
fetch into :a, :b, :c, :d!

explain
select /*+indexaccess(I)*/hex(k1), hex(k2), &3 &4 from t 
 order by i1!

data!
select /*+indexaccess(I)*/hex(k1), hex(k2), &3 &4 from t 
 order by i1!

nodata!
fetch into :a, :b, :c, :d!

data!
select hex(k1), hex(k2), &3 &4 from t where &1 = :a!
c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 from t where &1 = :a!
c 5 'aaab '

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 from t where &1 >= :a!
c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 from t where &1 <= :a!
c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 
from t where &1 <= :a and &1 >= :b!
c 5 'aaab' c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 
from t where &1 <= :a and &1 >= :b!
c 5 'aaac' c 5 'aaaa'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 from t where &1 > :a!
c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 from t where &1 < :a!
c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 
from t where &1 > :a and &1 < :b!
c 5 'aaab' c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 
from t where &1 > :a and &1 < :b!
c 5 'aaaa' c 5 'aaac'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 
from t where &1 <> :a!
c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 
from t where &1 between :a and :b!
c 5 'aaab' c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 
from t where &1 between :a and :b!
c 5 'aaa' c 5 'aab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 
from t where &1 between :a and :b!
c 5 'aaab' c 5 'aaac'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 
from t where &1 between :a and :b!
c 5 'aaac' c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 from t where &1 like :a!
c 5 'aaab'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 from t where &1 like :a!
c 5 'aaa%'

nodata!
fetch into :a, :b, :c, :d!
data!
select hex(k1), hex(k2), &3 &4 from t where &1 like :a!
c 5 'aaab%'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 = :a and &2 like :b !
c 5 'aaab' c 3 '%2'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 >= :a and &2 = :b !
c 5 'aaab' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 > :a and &2 = :b !
c 5 'aaab' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 <= :a and &2 = :b !
c 5 'aaab' c 3 '12'

nodata!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 < :a and &2 = :b !
c 5 'aaab' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 between :a and :b and &2 = :c !
c 5 'aaab' c 5 'aaab' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 between :a and :b and &2 = :c !
c 5 'aaaa' c 5 'aaab' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 between :a and :b and &2 = :c !
c 5 'aaab' c 5 'aaaa' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 between :a and :b and &2 = :c !
c 5 'aaab' c 5 'aaac' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 between :a and :b and &2 = :c !
c 5 'aaaa' c 5 'aaac' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 like :a and &2 = :b !
c 5 'aaab' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 like :a and &2 = :b !
c 5 'aaab%' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 like :a and &2 = :b !
c 5 'aaa%' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
select hex(k1), hex(k2), &3 &4 
from t where &1 like :a and &2 = :b !
c 5 'aaa%b' c 3 '12'

nodata!
fetch into :a, :b, :c, :d!
data !
declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.i1), hex(t2.i2) 
from t t1, t t2 
where t1.&1 >= t2.&1 and t1.&1 > :a and t2.&2 = :b for reuse !
c 5 'aaab' c 3 '11'

nodata!
fetch erg into :a, :b, :c, :d!
data !
declare erg cursor for 
select hex(t1.k1), hex(t1.k2), hex(t2.i1), hex(t2.i2)
from t t1, t t2 
where t1.&1 = t2.&1 and t2.&2 = :a for reuse !
c 3 '11'

nodata!
fetch erg into :a, :b, :c, :d!
data !
declare erg cursor for 
select hex(k1), hex(k2), hex(i1), hex(i2) from t t1
where t1.&1 in
(select &1 from t where
 t.&1 = t1.&1 and t.&2 = :a) for reuse !
c 3 '11'

nodata!
fetch erg into :a, :b, :c, :d!
data !
declare erg cursor for 
select hex(k1), hex(k2), hex(i1), hex(i2) from t t1
where t1.&1 like :a and t1.&1 in
(select &1 from t where
 t.&1 = t1.&1 and t.&2 = :b) for reuse !
c 5 'aaa%' c 3 '12'

nodata!
fetch erg into :a, :b, :c, :d!
pars_execute!
commit release!
