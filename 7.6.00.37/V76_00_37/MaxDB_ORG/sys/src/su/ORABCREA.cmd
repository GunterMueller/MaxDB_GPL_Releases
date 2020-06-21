file connect ( kern test sqlmode oracle!
create table ADDRESS ( LastName  char(25),
                       FirstName char(25),
                       Street    char(50),
                       City      char(25),
                       State     char(2),
                       Zip       number,
                       Phone     char(12),
                       Ext       char(5))!
create table ASOF ( AsOf date ) !
create table BIRTHDAY ( FirstName char(15),
                        LastName  char(15),
                        BirthDAte date,        
                        Age       number) !
create table BREEDING ( Offstring char(10),
                        Sex       char(1),
                        Cow       char(10),
                        Bull      char(10),
                        Birthdate date)!
create table CD       ( Account      number not null,
                        Amount       number not null,
                        MaturityDate date not null) !
create table COMFORT  ( City          char(14) not null,
                        Day           date not null,
                        Noon          number(3,1),
                        Midnight      number(3,1),
                        Rain          number) !
create table COMMA    ( Amount     number,
                        AmountChar char(20))!
create table EMPTY    ( Nothing char(25),
                        Less    number)! 
create table HOCKEY   ( Team char(20),
                        Won  number,
                        Lost number,
                        Tied number)!
create table HOLIDAY  ( Holiday       char(25),
                        act_date      date,
                        Celebrate     date)!
create table LEDGER   ( ActionDate    date,
                        Action        char(8),
                        Item          char(30),
                        Quantity      number,
                        QuantityType  char(10),
                        Rate          number,
                        Amount        number(9,2),
                        Person        char(25))!
create table LOCATION ( City       char(25),
                        Country    char(25),
                        Continent  char(25),
                        Latitude   number,
                        Nor_South  char(1),
                        Longitude  number,
                        EastWest   char(1))!
create table LODGING  ( Lodging  char(15) not null,
                        LongName char(40),
                        Manager  char(25),
                        Address  char(30))!
create table LONGTIME ( Name    char(25) not null,
                        Lodging char(15),
                        Age     number)!
create table MAGAZINE ( Name      char(16),
                        Title     char(37),
                        author    char(25),
                        issuedate date,
                        page      number)!
create table MATH     ( Name  char(12),
                        Above number,
                        Below number,
                        Empty number)!
create table NAME     ( Name char(25))!
create table NEWSPAPER( Feature char(15) not null,
                        Section char(1),
                        Page    number)!
create table NUMBERTEST(Value1 number,
                        Value2 number,
                        Value3 number(10,2))!
create table PAY      ( Name      char(25) not null,
                        DailyRate number)!
create table PAYDAY   ( CycleDate date)!
create table PROSPECT ( Name    char(25) not null,
                        Address char(35))!
create table ROSE     ( Lodging char(12))!
create table SHIPPING ( Client char(14),
                        Weight number)!
create table SKILL    ( Skill       char(25) not null,
                        Description char(80))!
create table STOCK    ( Company     char(20),
                        Symbol      char(6),
                        Industry    char(15),
                        CloseYesterday number(6,2),
                        CloseToday     number(6,2),
                        Volume         number)!
create table TROUBLE  ( City          char(14) not null,
                        Day           date not null,
                        Noon          number(4,1),
                        Midnight      number(4,1),
                        Rain          number)!
create table TWONAME  ( FirstName char(25),
                        LastName  char(25))!
create table WEATHER  ( City        char(11),
                        degree      number,
                        Humidity    number,
                        Condition   char(9))!
create table WORKER   ( Name    char(25) not null,
                        Age     number,
                        Lodging char(15))!
create table WORKERSKILL(Name   char(25) not null,
                        Skill   char(25) not null,
                        Ability char(15))!
commit work release !
