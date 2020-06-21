 * PAGE 46 !
select feature, section, page from newspaper !
fetch into :a,:b,:c !
 * PAGE 51 !
select feature, section, page from newspaper  
 where section = 'F'!
fetch into :a,:b,:c!
select feature, section, page from newspaper
 where section = 'F'
 order by feature !
fetch into :a,:b,:c !
select feature, section, page from newspaper
 where section = 'F'
 order by page !
fetch into :a,:b,:c !
 * PAGE 52 !
select feature, section, page from newspaper
 where section = 'F'
 order by page, feature !
fetch into :a,:b,:c !
select feature, section, page from newspaper
 where section = 'F'
 order by page desc, feature !
fetch into :a,:b,:c !
 * PAGE 53 !
select feature, section, page
  from newspaper
 where page = 6 !
fetch into :a,:b,:c !
 * PAGE 54 !
select feature, section, page
  from newspaper
 where section = 'B'!
fetch into :a,:b,:c !
select feature, section, page
  from newspaper
 where page > 4 !
fetch into :a,:b,:c !
 * PAGE 56 !
select feature, section, page
  from newspaper
 where section > 'B'!
fetch into :a,:b,:c !
select feature, section, page
  from newspaper
 where page < 8 !
fetch into :a,:b,:c !
 * PAGE 57 !
select feature, section, page
  from newspaper
 where page != 1 !
fetch into :a,:b,:c !
select * from newspaper
 where feature like 'Mo%'!
fetch into :a,:b,:c !
 * PAGE 58 !
select * from newspaper
 where feature like '__i%'!
fetch into :a,:b,:c!
select * from newspaper
 where feature like '%o%o%' !
fetch into :a,:b,:c!
select * from newspaper
 where feature like '%i%i%' !
fetch into :a,:b,:c!
 * PAGE 59 !
select city, day, rain
  from comfort !
fetch into :a,:b,:c!
select city, day, rain
  from comfort 
 where rain is null !
fetch into :a,:b,:c!
 * PAGE 60 !
select city, day, rain
  from comfort 
 where rain is not null !
fetch into :a,:b,:c!
 * PAGE 61 !
select feature, section, page
  from newspaper
 where section in ('A','B','F')!
fetch into :a,:b,:c!
select feature, section, page
  from newspaper
 where section not in ('A','B','F')!
fetch into :a,:b,:c!
select feature, section, page
  from newspaper
 where page between 7 and 10 !
fetch into :a,:b,:c!
 * PAGE 62 !
select feature, section, page
  from newspaper
 where section = 'F'
   and page > 7 !
fetch into :a,:b,:c!
select feature, section, page
  from newspaper
 where section = 'F'
    or page > 7 !
fetch into :a,:b,:c!
select feature, section, page
  from newspaper
 where section = 'F'
   and page between 7 and 10 !
fetch into :a,:b,:c!
 * PAGE 63 !
select feature, section, page
  from newspaper
 where section = 'A'
    or section = 'B'
   and page > 2 !
fetch into :a,:b,:c!
select feature, section, page
  from newspaper
 where section = 'A'
   and page > 2 
    or section = 'B' !
fetch into :a,:b,:c!
 * PAGE 64 !
select feature, section, page
  from newspaper
 where page > 2 
   and section = 'A'
    or section = 'B' !
fetch into :a,:b,:c!
 * PAGE 65 !
select feature, section, page
  from newspaper
 where page > 2 
   and (section = 'A'
    or section = 'B') !
fetch into :a,:b,:c!
select feature, section, page
  from newspaper
 where (section = 'A'
    or section = 'B')
   and page > 2  !
fetch into :a,:b,:c!
 * PAGE 66 !
select section from newspaper
 where feature = 'Doctor Is In'!
fetch into :a!
select feature from newspaper
 where section = 'F' !
fetch into :a!
select feature from newspaper
 where section = ( select section from newspaper
                    where feature = 'Doctor Is In') !
fetch into :a!
 * PAGE 67 !
select * from newspaper
 where section = ( select section from newspaper
                    where page = 1 ) !
select * from newspaper
 where section < ( select section from newspaper
                    where feature = 'Doctor Is In') !
fetch into :a,:b,:c!
 * PAGE 68 !
select city, country from location !
fetch into :a,:b !
select city, condition from weather !
fetch into :a,:b!
 * PAGE 69 ! 
select city from weather
 where condition = 'CLOUDY'!
fetch into :a! 
select city, country from location
 where city in ('PARIS', 'SPARTA')!
fetch into :a,:b!
select city, country from location
 where city in (select city from weather
                 where condition = 'CLOUDY') !
fetch into :a,:b!
 * PAGE 70 ! 
select * from weather
 where degree between 60 and
                 (select degree from weather
                   where city = 'PARIS') !
fetch into :a,:b,:c,:d,:e,:f,:g!
select city, condition, degree from weather !
fetch into :a,:b,:c!
 * PAGE 71 !
select city, longitude, eastwest, latitude, nor_south
  from location !
fetch into :a,:b,:c,:d,:e!
select weather.city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from weather, location
 where weather.city = location.city 
 order by weather.city!
fetch into :a,:b,:c,:d,:e,:f,:g!
 * PAGE 72 !
create view invasion as
select weather.city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from weather, location
 where weather.city = location.city !
 * PAGE 73 !
select city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from invasion !
fetch into :a,:b,:c,:d,:e,:f,:g!
 * PAGE 74 !
select city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from invasion 
 where country = 'GREECE'!
fetch into :a,:b,:c,:d,:e,:f,:g!
drop view invasion !
create view invasion as
select weather.city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from weather, location
 where weather.city = location.city
   and country = 'GREECE'!
 * PAGE 75 !
select city, condition, degree, latitude,
       nor_south, longitude, eastwest
  from invasion !
fetch into :a,:b,:c,:d,:e,:f,:g!
commit work !
