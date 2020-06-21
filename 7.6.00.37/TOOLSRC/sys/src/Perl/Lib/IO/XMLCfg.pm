
package IO::XMLCfg;

use strict;
use XML::Simple;
my $CfgPaths = { 'TOOL'    => "$ENV{'TOOL'}/config/",
                 'RELEASE' => "$ENV{'OWN'}/sys/src/config/" };

sub Load
{
    my $cfgname = ResolveCfgPath(shift);
    my $data = XMLin($cfgname) if ($cfgname);
    return $data;
}

sub Save
{
    my $cfgname = ResolveCfgPath(shift);
    my $data = shift;
    my $xml = XMLout($data);
    unless (open (OUTFILE, ">$cfgname"))
    {
        print "Error: Can't open file '$cfgname' for writing!\n$!\n";
        return 0;
    }
    print OUTFILE "$xml\n";
    return (close (OUTFILE));
}

sub ResolveCfgPath
{
    my $cfgname = shift;

    my ($type, $path) = ($cfgname =~ /^([^:]{2,}):([^:]+)$/) ? ($1,$2) : (undef, $cfgname);

    unless ($path)
    {
        print "Error: Config name is empty!\n";
        return undef;
    }

    $path .= '.xml' unless ($path =~ /^.*\.xml$/i);
    
    if ($type)
    {
        unless ($CfgPaths->{$type})
        {
            print "Error: Config type '$type' unknown!\n";
            return undef; 
        }
        $path = $CfgPaths->{$type} . $path;
    }
    return $path;
}