
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
package Tag;
use strict;

use vars qw(%expandChar $specialChar);
BEGIN {
    $specialChar = "&><\"";
    %expandChar = ( "&", "&amp;", ">", "&gt;", "<", "&lt;", "\"", "&quot;");
}

sub mapString($) {
    my $value = $_[0];
    $value =~ s/([$specialChar])/$expandChar{$1}/g;
    return $value;
}

sub new() {
    my ($class, $name, $value) = @_;
    my $self = {};
    $self->{tagname} = "$name";
    if (defined $value) {
	$self->addValue($value);
    }
    return bless $self, $class;
}

sub addAttr()
{
    my ($self, $attr, $value) = @_;
    if (defined $self->{tagname}) {
        $value = mapString($value);
        $self->{attr}{"$attr"} = "$value";
    }
}


sub addValue()
{
    my ($self, $value) = @_;
    if (defined $self->{tagname}) {
        $value = mapString($value);
	$self->{value} = $value;
    }
}

sub addTag()
{
    my ($self, $name, $value) = @_;
    if (defined $self->{tagname}) {
        my $tag = new Tag ( $name );
        $tag->addValue("$value");
        push @{$self->{list}} , $tag;
        return $tag;
    }
    return undef;
}

sub addList()
{
    my ($self, $name, @list)  = @_;
    if (defined $self->{tagname}) {
        foreach (@list) {
            my $tag = new Tag ( $name );
            $tag->addValue("$_");
            push @{$self->{list}} , $tag;
        }
    }
}

sub flush
{
    my $self = shift;
    if (defined $self->{tagname} && $main::displayTags{"$self->{tagname}"}) {
	push @main::XMLFILE, "\<$self->{tagname}";
	if (defined $self->{attr}) {
	    foreach (keys %{$self->{attr}}) {
		push @main::XMLFILE, " $_=\"$self->{attr}{$_}\"";
	    }
            push @main::XMLFILE, "\>\n";
	}
        else {
            push @main::XMLFILE, "\>";
        }
	if ( defined $self->{value} ) {
	    push @main::XMLFILE, "$self->{value}";
	}
        if ( defined $self->{list} ) {
            push @main::XMLFILE, "\n";
	    foreach (@{$self->{list}}) {
		$_->flush;
	    }
	}
        push @main::XMLFILE, "\</$self->{tagname}\>\n";
    }
}

package List;

sub new($$){
 my $file=shift;
 my $name=shift;
 my (@list);
 my $self={ };
 $self->{"name"}=$name;
 LINE:
 while(<$file>) {
     chop;
     if ( /'(.*)'\t.*/ ) {
	 print "$1\n";
	 push @list, "$1";
     }
     else {
	 last LINE;
     }
 }
 @{$self->{"list"}}=@list;
 return bless $self;
}

1; # module return code
