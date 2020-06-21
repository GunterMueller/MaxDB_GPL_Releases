 * PAGE 9 !
select city from weather where humidity = 89 !
fetch into :a!
 * PAGE 10 !
select city from weather where degree = 66 !
fetch into :a!
select city, degree from weather
  order by degree !
fetch into :a,:b!
 * PAGE 11 !
select city, degree from weather
 where degree > 80
 order by degree !
fetch into :a,:b!
select city, degree, humidity from weather
 where degree > 80
       and humidity < 70
 order by degree !
fetch into :a,:b,:c!
commit work !
