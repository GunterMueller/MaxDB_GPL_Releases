#! /usr/bin/perl
#
# Desc: XMLConfig - an XML config reader.
#
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

package XML::XMLConfig;

use strict;

use XML::DOM;
use POSIX;

1;

################################################################################
# new() - creates a new instance of the config parser.
################################################################################

sub new
{
    my ($proto, $type, $name)   = @_;
    my $class                   = ref($proto) || $proto;
    my $data                    = {};

    $data->{'_TYPE'}    = $type;
    $data->{'_NAME'}    = $name;
    $data->{'_LOADED'}  = 0;

    bless($data, $class);

    return $data;
}

################################################################################
# Load() - Loads the XML file.
################################################################################

sub Load
{
    my ($self) = @_;
    unless (ref $self) { die "Error: Object method 'Load' called as class method!\n"; }
    my $parser           = new XML::DOM::Parser or return 0;

    $self->{'_FILE'} = $self->GetCfgFileName();

    if ($self->FileExists())
    {
        $self->{'_DOC'} = $parser->parsefile($self->{'_FILE'}) or undef;
        if ($self->{'_DOC'})
        {
            $self->{'_LOADED'} = 1;
            return 1;
        }
    }

    return 0;
}

################################################################################
# FileExists() - Checks whether a XML file already exists or not.
################################################################################

sub ParseSection
{
    my ($self, $secname) = @_;
    unless (ref $self) { die "Error: Object method 'ParseSection' called as class method!\n"; }

    $self->Load() unless $self->{'_LOADED'};

    return undef unless ($self->{'_DOC'});
    return $self->_parse_section($self->{'_DOC'}, $secname);
}

################################################################################
# FileExists() - Checks whether a XML file already exists or not.
################################################################################

sub FileExists
{
    my $self = shift;
    unless (ref $self) { die "Error: Object method 'FileExists' called as class method!\n"; }

    return (-e $self->{'_FILE'}) ? 1 : 0;
}

################################################################################
# GetCfgFileName() - Returns the complete path/name of an XML file.
################################################################################

sub GetCfgFileName
{
    my $self = shift;
    unless (ref $self) { die "Error: Object method 'GetCfgFileName' called as class method!\n"; }

    $self->{'_CFG_PATHS'} = $self->ReadCfgPaths() unless ($self->{'_CFG_PATHS'});

    unless ($self->{'_CFG_PATHS'}->{$self->{'_TYPE'}}) { die "Error: Config type '$self->{'_TYPE'}' unknown!"}

    if ($self->{'_NAME'} =~ /.*\.XML$/i)
    {
        return "$self->{'_CFG_PATHS'}->{$self->{'_TYPE'}}/$self->{'_NAME'}";
    }
    else
    {
        return "$self->{'_CFG_PATHS'}->{$self->{'_TYPE'}}/$self->{'_NAME'}.XML";
    }
}

################################################################################
# ReadCfgPaths() - Reads the initial XMLConfigPaths.XML file.
################################################################################

sub ReadCfgPaths
{
    my $self = shift;
    unless (ref $self) { die "Error: Object method 'ReadCfgPaths' called as class method!\n"; }

    unless ($ENV{'TOOL'})   { die "Error: Environment 'TOOL' is not set!\n"; }

    my $CfgPathFile = "$ENV{'TOOL'}/config/XMLConfigPaths.XML";
    my $parser      = new XML::DOM::Parser or return "";
    my $doc         = $parser->parsefile($CfgPathFile) or return "";

    my $cfgpath     = $self->_parse_section($doc, "XMLConfigPaths") or die "Error: Can't parse initial configuration paths!\n";

    return $cfgpath;
}

################################################################################
# _parse_section() - Internal function: Parses a section and returns a hash.
################################################################################

sub _parse_section
{
    my ($self, $doc, $secname) = @_;
    my $node;
    my $href = {};

    foreach my $elem ($doc->getElementsByTagName("section"))
    {
        if ($elem->getAttribute("name") eq $secname) { $node = $elem; last; }
    }

    return unless ($node);

    $self->_parse_subnodes($node, $href);

    return $href;
}

################################################################################
# _parse_subnodes() - Internal function: parses all subnodes of a node.
################################################################################

sub _parse_subnodes
{
    my ($self, $node, $href) = @_;

    foreach my $elem ($node->getChildNodes())
    {
        if (($elem->getNodeType() == 1) and ($elem->getNodeName() eq "entry"))
        {
            my $name = $elem->getAttribute("name");
            my $type = $elem->getAttribute("type");
            my $val  = $elem->getFirstChild()->getData();

            if      ($type eq "scalar") { $$href{$name} = $elem->getFirstChild()->getData() }
            elsif   ($type eq "array")
            {
                my @tmparr = split(/,/, $elem->getFirstChild()->getData());
                $href->{$name} = \@tmparr;
            }
            elsif   ($type eq "hash")
            {
                my %tmphash     = split(/,/, $elem->getFirstChild()->getData());
                $href->{$name}  = \%tmphash;
            }
            elsif ($type eq "eval")
            {
                eval '$href->{$name} = ' . $elem->getFirstChild()->getData() . ';';
                if ($@) { print STDERR "XMLparse eval error: $@\n" }
            }
            else { print STDERR "XMLparse type error: Unknown option format found ($name, $type, $val)\n" }
        }
        elsif (($elem->getNodeType() == 1) and ($elem->getNodeName() eq "subsection"))
        {
            $self->_parse_subnodes($elem, $href->{$elem->getAttribute("name")});
        }
    }
}