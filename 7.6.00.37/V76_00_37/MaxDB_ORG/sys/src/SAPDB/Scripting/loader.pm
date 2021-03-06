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


package SAP::DBTech::loaderpackage;

$SAP::DBTech::loaderpackage::extensionLoaded = 0;

if(defined(&Win32::BuildNumber)) {
    package SAP::DBTech::loaderaperl;
    require Exporter;
    require DynaLoader;
    @ISA = qw(Exporter DynaLoader);
    package SAP::DBTech::loaderc;
    eval {
        bootstrap SAP::DBTech::loaderaperl;
        @EXPORT = qw( );
        $SAP::DBTech::loaderpackage::extensionLoaded = 1;
    };
}

if (!$SAP::DBTech::loaderpackage::extensionLoaded) {
    package SAP::DBTech::loadercperl;
    require Exporter;
    require DynaLoader;
    @ISA = qw(Exporter DynaLoader);
    package SAP::DBTech::loaderc;
    bootstrap SAP::DBTech::loadercperl;
    @EXPORT = qw( );
}



# ------- FUNCTION WRAPPERS --------

package SAP::DBTech::loader;

my %OWNER = ();


### class Loader ###
package Loader;

use strict;

sub TIEHASH {
    my ($classname,$obj) = @_;
    return bless $obj, $classname;}

sub CLEAR { }

sub new {
    my $clazzName = "Loader";
    my $self = shift;
    $self = SAP::DBTech::loaderc::loader_Loader(@_);
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
        SAP::DBTech::loaderc::Loader_destructor ($self);
        delete $OWNER{$self};
    }
}

sub cmd {
    return SAP::DBTech::loaderc::Loader_cmd(@_);
}

sub rawCmd {
    return SAP::DBTech::loaderc::Loader_rawCmd(@_);
}

sub sql {
    return SAP::DBTech::loaderc::Loader_sql(@_);
}

sub cancelCmd {
    return SAP::DBTech::loaderc::Loader_cancelCmd(@_);
}

sub release {
    return SAP::DBTech::loaderc::Loader_release(@_);
}
### end of class Loader ###

package loader;

sub Loader {
    my $clazzName = "Loader";
    my $self = SAP::DBTech::loaderc::loader_Loader(@_);
    return undef if (!defined($self));
    bless $self, $clazzName;
    $OWNER{$self} = 1;
    my %retval;
    tie %retval, $clazzName, $self;
    return bless \%retval, $clazzName;
}


sub _buildInfo {
    return SAP::DBTech::loaderc::loader__buildInfo(@_);
}

sub _createGUID {
    return SAP::DBTech::loaderc::loader__createGUID(@_);
}

# End of module

1;
