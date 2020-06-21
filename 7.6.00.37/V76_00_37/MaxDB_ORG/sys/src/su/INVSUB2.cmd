explain select &1 from &2 where &2ka in
(select substr(&3f,2,1) from &3 where &3a = 'Z') !
monitor init !
select &1 from &2 where &2ka in
(select substr(&3f,2,1) from &3 where &3a = 'Z') !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where &2ka = 'Z' and &2kb in
(select &3e - 100 from &3 where &3a = 'Z') !
monitor init !
select &1 from &2 where &2ka = 'Z' and &2kb in
(select &3e - 100 from &3 where &3a = 'Z') !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where &2ka = 'Z' and &2kb = 0 and &2kc in
(select &3c from &3 where &3a = 'Z' and &3b = 0) !
monitor init !
select &1 from &2 where &2ka = 'Z' and &2kb = 0 and &2kc in
(select &3c from &3 where &3a = 'Z' and &3b = 0) !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where &2kb = 9 and &2kc = 'C' and &2ka in
(select &3a from &3) !
monitor init !
select &1 from &2 where &2kb = 9 and &2kc = 'C' and &2ka in
(select &3a from &3) !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where &2kb between 9 and 10 and 
&2kc like 'C%' and &2ka in
(select &3a from &3) !
monitor init !
select &1 from &2 where &2kb between 9 and 10 and 
&2kc like 'C%' and &2ka in
(select &3a from &3) !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where &2kb > 1 and 
&2kc like 'C' and &2ka in
(select &3a from &3 where &3a <= 'Z') !
monitor init !
select &1 from &2 where &2kb > 1 and 
&2kc like 'C' and &2ka in
(select &3a from &3 where &3a <= 'Z') !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where &2ka >= 'Z' and &2kb in
(select trunc(&3d) from &3 where &3a = 'Z') !
monitor init !
select &1 from &2 where &2ka >= 'Z' and &2kb in
(select trunc(&3d) from &3 where &3a = 'Z') !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where 
&2ka in (select decode (&3b, null, 'Z', 0, 'ZZ')  from &3) and 
&2kb in (select decode (&3b, 9, 0, null, 0, 0, 0) from &3) and
&2kc in (select decode (&3b, 0, 'a', 9, 'b', 'C') from &3) !
monitor init !
select &1 from &2 where 
&2ka in (select decode (&3b, null, 'Z', 0, 'ZZ')  from &3) and 
&2kb in (select decode (&3b, 9, 0, null, 0, 0, 0) from &3) and
&2kc in (select decode (&3b, 0, 'a', 9, 'b', 'C') from &3) !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where 
&2ka in ('A   ', 'B  ', 'C ') and 
&2kb in (select &3b from &3 where &3b > 0)     and
&2kc in (select &3c from &3 where &3a='A' and &3b=9) !
monitor init !
select &1 from &2 where 
&2ka in ('A   ', 'B  ', 'C ') and 
&2kb in (select &3b from &3 where &3b > 0)     and
&2kc in (select &3c from &3 where &3a='A' and &3b=9) !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where 
&2ka in ('A                                 ') and 
&2kb in (select &3b from &3 where &3b > 0)     and
&2kc in (select &3c from &3 where &3a='A' and &3b=9) !
monitor init !
select &1 from &2 where 
&2ka in ('A                                 ') and 
&2kb in (select &3b from &3 where &3b > 0)     and
&2kc in (select &3c from &3 where &3a='A' and &3b=9) !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where 
&2ka >= 'A                                 '   and 
&2kb in (select &3b from &3 where &3b > 0)     and
&2kc in (select &3c from &3 where &3a='A' and &3b=9) !
monitor init !
select &1 from &2 where 
&2ka >= 'A                                 '   and 
&2kb in (select &3b from &3 where &3b > 0)     and
&2kc in (select &3c from &3 where &3a='A' and &3b=9) !
file sel_monitor ( load !
fetch !
explain select &1 from &2 where 
&2ka in (select &3c from &3 where &3a in ('Z ', 'M ', 'A ', null)) and 
&2kb between 1 and 9 and
&2kc < 'B' !
monitor init !
select &1 from &2 where 
&2ka in (select &3c from &3 where &3a in ('Z ', 'M ', 'A ', null)) and 
&2kb between 1 and 9 and
&2kc < 'B' !
file sel_monitor ( load !
fetch !
