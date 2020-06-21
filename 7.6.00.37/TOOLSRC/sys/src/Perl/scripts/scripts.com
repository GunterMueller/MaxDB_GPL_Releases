
* ascii exec concom=no unpack=no ->$TOOL/bin/
DevStudio.pl dep=PerfSet.pm
get_components.pl dep=:APODev/APOSettings.pm


fix_hashbang.pl execscript=
CheckOptFlags.pl dep=:Perl/Lib/SDB/Make/OptimizationInfo.pm