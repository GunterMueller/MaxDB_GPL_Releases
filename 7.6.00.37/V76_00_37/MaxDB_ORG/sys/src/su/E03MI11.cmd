*   *ID* E03MI11  DBCMD    changed on 1992-09-28-13.23.41 by ADMIN     *
 ************************************************************ !
&1 select &2 from &3 
where mi1 = 'b' and mi2 = 'b'
&4 &5!
&6 &7 !
&1 select &2 from &3 where mi1 = 'b'
&4 &5!
&6 &7 !
&1 select &2 from &3 where mi2 = 'b'
&4 &5!
&6 &7 !
&1 select &2 from &3 where mi1 > 'a'
&4 &5!
&6 &7 !
&1 select &2 from &3 
where mi1 > 'b' and mi2 > 'b'
&4 &5!
&6 &7 !
&1 select &2 from &3 
where mi1 < 'b' and mi2 < 'b'
&4 &5!
&6 &7 !
&1 select &2 from &3 
where mi1 between 'b' and 'b'
&4 &5!
&6 &7 !
&1 select &2 from &3 
where mi1 = 'b' and
    mi2 between 'b' and 'b'
&4 &5!
&6 &7 !
&1 select &2 from &3 
where mi1 like 'b%'
  and mi2 between 'b' and 'c'
&4 &5!
&6 &7 !
