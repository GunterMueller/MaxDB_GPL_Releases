# This file was automatically generated by glue
#
#   ========== licence begin  GPL
#   Copyright (c) 2000-2006 SAP AG
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
#   ========== licence end


package SAP::DBTech::dbmpackage;

$SAP::DBTech::dbmpackage::extensionLoaded = 0;

if(defined(&Win32::BuildNumber)) {
    package SAP::DBTech::dbmaperl;
    require Exporter;
    require DynaLoader;
    @ISA = qw(Exporter DynaLoader);
    package SAP::DBTech::dbmc;
    eval {
        bootstrap SAP::DBTech::dbmaperl;
        @EXPORT = qw( );
        $SAP::DBTech::dbmpackage::extensionLoaded = 1;
    };
}

if (!$SAP::DBTech::dbmpackage::extensionLoaded) {
    package SAP::DBTech::dbmcperl;
    require Exporter;
    require DynaLoader;
    @ISA = qw(Exporter DynaLoader);
    package SAP::DBTech::dbmc;
    bootstrap SAP::DBTech::dbmcperl;
    @EXPORT = qw( );
}



# ------- FUNCTION WRAPPERS --------

package SAP::DBTech::dbm;

my %OWNER = ();


### class DBM ###
package DBM;

use strict;

sub TIEHASH {
    my ($classname,$obj) = @_;
    return bless $obj, $classname;}

sub CLEAR { }

sub new {
    my $clazzName = "DBM";
    my $self = shift;
    $self = SAP::DBTech::dbmc::dbm_DBM(@_);
    return undef if (!defined($self));
    bless $self, $clazzName;
    $OWNER{$self} = 1;
    my %retval;
    tie %retval, $clazzName, $self;
    return bless \%retval, $clazzName;
}


sub DESTROY {
    my ($self) = @_;
    return unless defined $self;

    # Only do something on the inner (non-tied) object
    if (ref($self) ne 'HASH' && exists $OWNER{$self}) {
        SAP::DBTech::dbmc::DBM_destructor ($self);
        delete $OWNER{$self};
    }
}

sub cmd {
    return SAP::DBTech::dbmc::DBM_cmd(@_);
}

sub rawCmd {
    return SAP::DBTech::dbmc::DBM_rawCmd(@_);
}

sub cancelCmd {
    return SAP::DBTech::dbmc::DBM_cancelCmd(@_);
}

sub release {
    return SAP::DBTech::dbmc::DBM_release(@_);
}
### end of class DBM ###

package dbm;

sub DBM {
    my $clazzName = "DBM";
    my $self = SAP::DBTech::dbmc::dbm_DBM(@_);
    return undef if (!defined($self));
    bless $self, $clazzName;
    $OWNER{$self} = 1;
    my %retval;
    tie %retval, $clazzName, $self;
    return bless \%retval, $clazzName;
}


sub saveUser {
    return SAP::DBTech::dbmc::dbm_saveUser(@_);
}

sub checkUser {
    return SAP::DBTech::dbmc::dbm_checkUser(@_);
}

sub _buildInfo {
    return SAP::DBTech::dbmc::dbm__buildInfo(@_);
}

sub deleteUser {
    return SAP::DBTech::dbmc::dbm_deleteUser(@_);
}

# End of module

1;