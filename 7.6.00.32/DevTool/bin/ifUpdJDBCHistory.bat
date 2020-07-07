@echo off
echo Parameter 1 - own-pfad :
echo %1
echo Parameter 2 - Release :
echo %2
perl.exe %TOOL%\bin\devstudio.pl  -x \\p59962\sapdevelop\V74\develop 7.4 "ifGenJDBCHistory.pl"





