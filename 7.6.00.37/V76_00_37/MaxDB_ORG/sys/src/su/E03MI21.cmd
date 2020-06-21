*   *ID* E03MI21  DBCMD    changed on 1992-09-28-13.23.42 by ADMIN     *
 ************************************************************ !
file connect ( kern test !
data!
pars_then_exec!
&1 select &2 from &3 
where mi1 = :p1 and mi2 = :p2
&4 &5!
c 20 'b'
c 20 'b'

nodata!
&6 &7 into :a,:b,:c,:d,:e !
data!
&1 select &2 from &3 where mi1 = :p
&4 &5!
c 20 'b'

nodata!
&6 &7 into :a,:b,:c,:d,:e !
data!
&1 select &2 from &3 where mi2 = :p
&4 &5!
c 20 'b'

nodata!
&6 &7 into :a,:b,:c,:d,:e !
data!
&1 select &2 from &3 where mi1 > :p
&4 &5!
c 20 'a'

nodata!
&6 &7 into :a,:b,:c,:d,:e !
data!
&1 select &2 from &3 
where mi1 > :p1 and mi2 > :p2
&4 &5!
c 20 'b'
c 20 'b'

nodata!
&6 &7 into :a,:b,:c,:d,:e !
data!
&1 select &2 from &3 
where mi1 < :p1 and mi2 < :p2
&4 &5!
c 20 'b'
c 20 'b'

nodata!
&6 &7 into :a,:b,:c,:d,:e !
data!
&1 select &2 from &3 
where mi1 between :p1 and :p2
&4 &5!
c 20 'b'
c 20 'b'

nodata!
&6 &7 into :a,:b,:c,:d,:e !
data!
&1 select &2 from &3 
where mi1 like :p1
  and mi2 between :p2 and :p3
&4 &5!
c 20 'b%'
c 20 'b'
c 20 'c'

nodata!
&6 &7 into :a,:b,:c,:d,:e !
pars_execute!
commit release!
