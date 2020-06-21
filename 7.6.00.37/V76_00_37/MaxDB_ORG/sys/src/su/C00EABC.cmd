file connect ( kern test !
* Die Laenge der Parameter fuehrt dazu, dasz nur noch eine Strategie
* mit 64 Bytes Platz hat. Es ist aber fe_sequential. Wenn vak505
* die Daten + part1 fuer den Fetch-Messbuffer in Part1 stecken will,
* werden diese Strategie-Bytes nicht abgezogen, sondern mit gemoved.
* Und das gibt dann 4144 Bytes in einen mxsp_buf (4096 Bytes)==>rumms
* bis 6.1.1.Cl 15C 15.12.95 !
create table t ( a char (16) key, b char (36) key, c char (4) key,
d char (3))!
insert t values ('A', 'B', 'C', 'D')!
pars_then_ex!
data !
select *, *, *, *, * from t where
a = :a and
 b in (
:a1,:a2,:a3,:a4,:a5,:a6,:a7,
:c0,:c1,:c2,:c3,:c4,:c5,:c6,:c7,:c8,:c9, 
:c10,:c11,:c12,:c13,:c14,:c15,:c16,:c17,:c18,:c19, 
:c20,:c21,:c22,:c23,:c24,:c25,:c26,:c27,:c28,:c29, 
:c30,:c31,:c32,:c33,:c34,:c35,:c36,:c37,:c38,:c39, 
:c40,:c41,:c42,:c43,:c44,:c45,:c46,:c47,:c48,:c49, 
:c50,:c51,:c52,:c53,:c54,:c55,:c56,:c57,:c58,:c59, 
:c60,:c61,:c62,:c63,:c64,:c65,:c66,:c67,:c68,:c69, 
:c70,:c71,:c72,:c73,:c74,:c75,:c76,:c77,:c78,:c79, 
:c80,:c81,:c82,:c83,:c84,:c85,:c86,:c87,:c88,:c89, 
:c90,:c91,:c92,:c93,:c94,:c95,:c96,:c97,:c98,:c99)
and c = :c
order by 1,2,3!
 c 19 'A'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B'
c 36 'B' c 36 'B' c 36 'B' c 36 'B' c 36 'B' 
c 36 'B' c 36 'B'
 c 4 'C'

nodata!
fetch into :a, :b, :c, :d, :a, :b, :c, :d, :a, :b, :c, :d,
   :a, :b, :c, :d, :a, :b, :c, :d !
pars_execute!
rollback release !
