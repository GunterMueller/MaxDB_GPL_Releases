#! /usr/bin/perl
#
# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/XMLParser.pm#2 $
# $DateTime: 2005/05/25 08:19:10 $
# $Change: 560346 $
#
# Desc: 
#	- parser of pcr_releases.xml
#	- specify XML handlers to generate XML tree
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


package BuildPackage::XMLParser;

use Exporter;
use XML::Parser::Expat;
use GME::XMLElement;

@ISA = ('Exporter');

@EXPORT = ('new', 'ParseFile', 'ParseStream');

my %ParserHash;

sub new {
	my $self = {};
	bless $self;

	$self->{Parser} = new XML::Parser::Expat;
	$self->{Parser}->setHandlers (
	        Start  => \&HandleStart,
       		Char   => \&HandleChar,
	        End    => \&HandleEnd );
	$self->{Current} = 0;
	$self->{Parent} = 0;
	$self->{Tree} = 0;

	$ParserHash{$self->{Parser}} = $self;
	return $self;
}

sub ParseFile {
	my $self = shift;
	my $filename = shift;	
	$self->{Parser}->parsefile ($filename);
	return $self->{Tree};
}

sub ParseStream {
	my $self = shift;
	my $fh_ref = shift;	
	undef $bla;
	XML::Parser::Expat::ParseStream 
		($self->{Parser}->{Parser}, $fh_ref, $bla);
	return $self->{Tree};
}

sub HandleStart {
	my ($ExpatParser, $Type, %Attr) = @_;
	my $Parser = $ParserHash{$ExpatParser};
	$Parser->{Current} = XMLElement->new ($Type);        	
	if ($Parser->{Parent} == 0) {
		# set root of tree
		$Parser->{Tree} = $Parser->{Current};
	}
	else{
		$Parser->{Current}->SetParent ($Parser->{Parent});	
	}
	foreach my $attrib (keys(%Attr)){
		$Parser->{Current}->SetAttribute($attrib, $Attr{$attrib});
	}
	$Parser->{Parent} = $Parser->{Current};
}

sub HandleEnd {
	my ($ExpatParser, $Type) = @_;
	my $Parser = $ParserHash{$ExpatParser};;
	$Parser->{Current}->Insert();
	$Parser->{Parent} = $Parser->{Current}->GetParent();
	$Parser->{Current} = $Parser->{Parent};
}

sub HandleChar {
	my  ($ExpatParser, $Content) = @_;
	my $Parser = $ParserHash{$ExpatParser};;

	# don't care empty Content
	return unless ($Content =~ /\S/);

	$Content =~ /(\S+)/;
	$Parser->{Current}->SetContent ($1);
}

1;

