connect flater identified by flater sqlmode ansi !
DELETE FROM BASE_VS1 !
INSERT INTO BASE_VS1 VALUES (0,1) !
INSERT INTO BASE_VS1 VALUES (1,0) !
INSERT INTO BASE_VS1 VALUES (0,0) !
INSERT INTO BASE_VS1 VALUES (1,1) !
SELECT COUNT(*) INTO :i FROM BASE_VS1 !
DELETE FROM USIG !
INSERT INTO USIG VALUES (0,2) !
INSERT INTO USIG VALUES (1,3) !
DELETE FROM U_SIG !
INSERT INTO U_SIG VALUES (4,6) !
INSERT INTO U_SIG VALUES (5,7) !
SELECT COUNT(*) INTO :i FROM USIG !
SELECT COUNT(*) INTO :ii FROM U_SIG !
COMMIT WORK RELEASE !
