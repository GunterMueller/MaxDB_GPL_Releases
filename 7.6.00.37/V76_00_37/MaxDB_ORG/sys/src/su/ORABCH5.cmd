 * PAGE 111 !
select city, lower(city), lower('City') from weather !
fetch into :a,:b,:c !
 * PAGE 112 !
select city||country from location !
fetch into :a!
select city||', '||country from location !
fetch into :a!
 * PAGE 114 !
select rpad(city, 35, '.'), degree from weather !
fetch into :a,:b!
 * PAGE 115 !
select lpad(city, 11), degree from weather !
fetch into :a,:b !
select title from magazine !
fetch into :a!
 * PAGE 116 !
select rtrim(title, '."') from magazine !
fetch into :a !
select rtrim(rtrim(title, '"'),'.') from magazine !
fetch into :a !
 * PAGE 117 !
select ltrim(rtrim(title,'."'),'"') from magazine !
fetch into :a !
select ltrim(rtrim(rtrim(title, '"'),'.'),'"') from magazine !
fetch into :a !
 * PAGE 118 !
select ltrim(rtrim(title,'."'),'"THE ')
  from magazine !
fetch into :a !
 * PAGE 120 !
select name, rpad(rtrim(ltrim(title,'"'),'."'),47,'-^'),
  to_char (page, '999') Page  from magazine !
fetch into :a,:b,:c !
 * PAGE 121 !
select city from weather !
fetch into :a!
select city, upper(city), lower(city), initcap(lower(city))
  from weather !
fetch into :a,:b,:c,:d!
 * PAGE 122 !
select name from magazine !
fetch into :a!
select initcap(lower(name)) from magazine !
fetch into :a!
 * folgendes koennen wir (noch) nicht !
select initcap(lower(name)) name,
       initcap(lower(rtrim(ltrim(title,'"'),'."'))) title,
       page
  from magazine !
 * PAGE 123 !
select name, length(name) from magazine !
fetch into :a,:b!
select substr(name, 6, 4) from magazine !
fetch into :a!
 * PAGE 124 !
select lastname, firstname, phone from address !
fetch into :a,:b,:c !
 * PAGE 125 !
select lastname, firstname, phone from address
 where phone like '415-%'!
fetch into :a,:b,:c!
select lastname, firstname, substr(phone,5) from address 
 where phone like '415-%'!
fetch into :a,:b,:c!
 * PAGE 126 !
select lastname||', '||firstname name, substr(phone, 5) phone
  from address
 where phone like '415-%'!
fetch into :a,:b !
select rpad(lastname||', '||firstname,25,'.') name, 
       substr(phone, 5) phone
  from address
 where phone like '415-%'!
fetch into :a,:b !
 * PAGE 127 !
select author from magazine !
fetch into :a !
select author, instr(author, 'O') from magazine !
fetch into :a,:b!
 * PAGE 128 !
select author, instr(author,'O',1,2) from magazine !
fetch into :a,:b !
select author, instr(author, 'WILLIAM') from magazine !
fetch into :1,:b!
 * PAGE 129 !
select author, instr(author,',') from magazine !
fetch into :a,:b!
 * PAGE 130 !
select author, substr(author, 1, instr(author,',')-1)
  from magazine !
fetch into :a,:b !
select author, substr(author, instr(author,',')+2)
  from magazine !
fetch into :a,:b !
select author, substr(author, instr(author,',')+2)
      ||' '||
               substr(author, 1, instr(author,',')-1)
      "By 1.Name"
  from magazine !
fetch into :a,:b!
 * PAGE 132 !
select city
  from weather
 where length(city) < 7 !
fetch into :a!
 * PAGE 133 !
select city
  from weather
 order by length(city) !
fetch into :a !
select author from magazine
 where instr(author,'O',1,2) > 0 !
fetch into :a !
 * PAGE 134 !
select city, degree, condition from weather
 where soundex(city) = soundex('Sidney')!
fetch into :a,:b,:c!
 * PAGE 135 !
select city, degree, condition from weather
 where soundex(city) = soundex('menncestr')!
fetch into :a,:b,:c!
select author from magazine
 where soundex(author) = soundex('Banheffer')!
fetch into :a!
select lastname, firstname, phone
  from address !
fetch into :a,:b,:c!
 * PAGE 136 !
select a.lastname, a.firstname, a.phone
  from address a, address b
 where a.lastname != b.lastname
   and soundex(a.lastname) = soundex(b.lastname) !
fetch into :a,:b,:c !
commit work !
