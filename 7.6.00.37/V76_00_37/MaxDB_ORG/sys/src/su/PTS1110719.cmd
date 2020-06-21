
connect kern test !


*//
*////////////////////////////////////////////////////
*//
Create Table Name_Space (
    Id         Char(24)    Byte,
    Name_Space Char(50)      ,
    Primary Key (Name_Space),
    Constraint Name_Space_X Unique (Id)
)!
*//
Insert into Name_Space Set
  Id = X'000000000000000000000000000000000000000000000001',
  Name_Space = 'DAV:'!
*//
*////////////////////////////////////////////////////
*//
Create Table Property_Management (
   Id                Char(24)   Byte,
   Name_Space_Id     Char(24)   Byte,
   Name_Prefix       Char(50)      ,
   Name              Char(50)     ,
   Primary Key (Name_Space_Id, Name_Prefix)
)!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000001',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'displayname'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000002',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'resourcetype'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000003',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'getcontenttype'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000004',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'getcontentlength'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000005',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'getlastmodified'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000006',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'lockdiscovery'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000006',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'lockid'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000007',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'lockowner'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000008',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'locktype'!
*//
Insert into Property_Management Set
  Id = X'000000000000000000000000000000000000000000000009',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'lockscope'!
*//
Insert into Property_Management Set
  Id = X'00000000000000000000000000000000000000000000000A',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'locktimeout'!
*//
Insert into Property_Management Set
  Id = X'00000000000000000000000000000000000000000000000B',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'lockdepth'!
*//
Insert into Property_Management Set
  Id = X'00000000000000000000000000000000000000000000000C',
  Name_Space_Id = X'000000000000000000000000000000000000000000000001',
  Name_Prefix = 'supportedlock'!
*//
*//Select * from Property_Management
*//
*////////////////////////////////////////////////////
*//
Create Table Property (
   CId                  Char(24)  Byte,

   Property_Id          Char(24)  Byte,
   Property_Short_Value Char(900)     ,
   Property_Long_Value  Long      Byte
)!
*//
Create Index Property_X1 On Property (Property_Id, CId)!
*//
Create Index Property_X2 On Property (Cid DESC)!
*//
*///////////////////////////////////////////////////
*// Initial values for ROOT
*///////////////////////////////////////////////////
*//
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000001',
  Property_Short_Value = '/'!
*//
*// resourcetype for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000002',
  Property_Short_Value = 'collection'!
*//
*// getcontentype for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000003',
  Property_Short_Value = 'sapdbwww/directory'!
*//
*// getcontentlength for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000004',
  Property_Short_Value = '0'!
*//
*// getlastmodified for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000005',
  Property_Short_Value = TIMESTAMP!
*//
*// lockdiscovery for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'000000000000000000000000000000000000000000000006',
  Property_Short_Value = '000000000000000000000000000000000000000000000000'!
*//
*// supportedlock for root
Insert into Property Set
  CId = X'000000000000000000000000000000000000000000000000',
  Property_Id = X'00000000000000000000000000000000000000000000000C',
  Property_Short_Value = 'exclusive'!


* before PTS1110719 this produces 8 results, one row with 'DAV' and 'lockid'
SELECT NS.Name_Space, PM.Name_Prefix, 
       P.Property_Short_Value, P.Property_Id   
FROM Property P, Property_Management PM, Name_Space NS         
WHERE 
P.Property_Id = PM.Id AND PM.Name_Space_Id = NS.Id AND
NOT (NS.Name_Space = 'DAV:' AND PM.Name_Prefix = 'lockid') !

* before PTS1110719 this produces the correct result set of 7 rows
SELECT NS.Name_Space, PM.Name_Prefix, 
       P.Property_Short_Value, P.Property_Id   
FROM Property P, Property_Management PM, Name_Space NS         
WHERE 
NOT (NS.Name_Space = 'DAV:' AND PM.Name_Prefix = 'lockid') AND
P.Property_Id = PM.Id AND PM.Name_Space_Id = NS.Id !


commit work release!
