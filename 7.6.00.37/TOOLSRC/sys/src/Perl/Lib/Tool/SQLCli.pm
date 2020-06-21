#! /usr/bin/perl
#
#    RequestInput.pm - Asks for user input.
#
#    @(#)RequestInput.pm     2003-04-23
#
#    U. Jansen, SAP AG
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

##############################################################################
# Static includes:

package Tool::SQLCli;

use strict;

##############################################################################
# new() - Registers a new instance.
##############################################################################
sub new
{
    my $proto = shift;
    my $params = shift;
    my $data = {};
    my $class = ref($proto) || $proto;

    foreach my $key (keys %{$params})
    {
        $data->{$key} = $params->{$key};
    }

    bless ($data, $class);
    return $data;
}

##############################################################################
# validate() - Validates all settings and tests DB connection.
##############################################################################
sub validate
{
    my $self = shift;
    
    unless (ref $self) { die "Object method called as class method!\n"; }
    
    unless ($self->{'DBNAME'}) { print "DB name is not set.\n"; return 0; }
    unless ($self->{'USER'}) { $self->{'USER'} = "DBA"; }
    unless ($self->{'PASSWORD'}) { $self->{'PASSWORD'} = "DBA"; }
    $self->{'SQLCLI'} = ($self->{'SQLCLI'} ? $self->{'SQLCLI'} : (-e "$ENV{'OWN'}\\usr\\bin\\sqlcli.exe" ? "$ENV{'OWN'}\\usr\\bin\\sqlcli.exe" : (-e "$ENV{'OWN'}/usr/bin/sqlcli" ? "$ENV{'OWN'}/usr/bin/sqlcli.exe" : "")));
    unless ($self->{'SQLCLI'}) { print "No valid sqlcli command found.\n"; return 0; }
    
    return 1;
}

##############################################################################
# execute() - Executes a SQL command.
##############################################################################
sub execute
{
    my $self = shift;
    unless (ref $self) { die "Object method called as class method!\n"; }

    $self->{'SQL_CMD'} = shift;
    
    unless ($self->{'SQL_CMD'})
    {
        $self->{'RC'} = -1;
        $self->{'ERROR_MSG'} = "Missing SQL statement.";
        return $self;
    }
    
    my $cmd_str = "$self->{'SQLCLI'} -A -x";
    $cmd_str .= " -n $self->{'HOSTNAME'}" if ($self->{'HOSTNAME'});
    $cmd_str .= " -d $self->{'DBNAME'}" if ($self->{'DBNAME'});
    $cmd_str .= " -u $self->{'USER'},$self->{'PASSWORD'}" if ($self->{'USER'});
    $cmd_str .= " $self->{'SQL_CMD'}";
    
    unless (open (CMD_OUT, "$cmd_str |"))
    {
        $self->{'RC'} = -2;
        $self->{'ERROR_MSG'} = "Opening sqlcli command failed.\n";
        return $self;
    }
    
    $self->{'RC'} = 0;
    $self->{'ERROR_MSG'} = "no error";
    my $row = 0;
    my @rows;
    
    while (<CMD_OUT>)
    {
        if (/^\*\s*(-?\d+):(.*)$/)
        {
            $self->{'RC'} = $1;
            $self->{'ERROR_MSG'} = $2;
            if ($self->{'ERROR_MSG'} =~ /^(.*)\sSQLSTATE:\s?(-?\d+)$/)
            {
                $self->{'ERROR_MSG'} = $1;
                $self->{'SQL_STATE'} = $2;
            }
        }
        my @actrow = _splitrow($_);
        my $actptr = \@actrow;
        if ($row == 0) { $self->{'COLUMNS'} = $actptr; }
        else           { push @rows, $actptr; }
        $row = $row + 1;
    }
    close (CMD_OUT);
    
    $self->{'DATA'} = \@rows;
    $self->{'ROWS'} = $row - 1;
    return $self;
}

sub _splitrow
{
    my $row = shift;
    my @tmp = split (/,/, $row);
    my @erg;
    pop @tmp;
    
    foreach my $val (@tmp)
    {
        if ($val =~ /^\"\s*(\S+)\s*\"$/)
        {
            push @erg, $1;
        }
        elsif ($val =~ /^\"(\S+)\"$/)
        {
            push @erg, $1;
        }
        else
        {
            push @erg, $val;
        }
    }
    
    return @erg;
}

1;
