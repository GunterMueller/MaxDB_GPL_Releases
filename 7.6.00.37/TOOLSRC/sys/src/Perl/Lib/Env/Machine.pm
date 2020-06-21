
package Env::Machine;

use strict;
use Env;
use XML::XMLConfig;

1;

sub LoadCfg
{
    my ($mach) = @_;
    
	my $machineCfg = XML::XMLConfig->new("machine", "MachineConfigs");
	
	if ($machineCfg->Load())
	{
	    my $cfg = $machineCfg->ParseSection($mach);
	    return $cfg;
	}
	else
	{
	    return undef;
	}
}
