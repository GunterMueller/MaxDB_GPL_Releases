#!/usr/bin/perl
#
#    Automail.pm - A module to send automatic/standarized mails.
#
#    @(#)Automail 2004-03-17
#
#    U. Jansen, SAP AG
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#

require Exporter;
package Mail::Automail;
use Mail::Sendmail;
use strict;
use vars qw(@ISA @EXPORT_OK $debug $log $error);

@ISA        = qw(Exporter);
@EXPORT_OK  = qw(Send);


# Hash elements that can be used (mandatory elements are prepended by an asterix):
#
# From        : Overrides 'automail@maxdb.sap.corp'
#*To          : List of recipients
# Cc          : Additional recipients
# Bcc         : List of blind copy recipients
# Subject     : Overrides automatic Subject generation
# LCVer       : Set LC Version manually, preferred format: X.X.XX.(DEV|COR|RAMP|XX)
#*Category    : Should be one of these: Make, Inst, Test, Check
# Type        : Can be any single word which is added to the Category e.g. 'Info'
# Hostname    : Overrides the automatic hostname determination
# Infotext    : Any additional text which is appended to the subject
#*Message     : The message body.
# Smtp        : Overrides default Smtp server 'mail.sap.corp'
# Content-Type: Overrides the standard 'text' content type

sub Send
{
    my %param = @_;
    my %mail;
    my $hostname;
    
    $mail{'From'} = ($param{'From'} ? $param{'From'} : 'automail@maxdb.sap.corp');
    $mail{'To'}   = $param{'To'};
    $mail{'Cc'}   = $param{'Cc'};
    $mail{'Bcc'}  = $param{'Bcc'};
    $mail{'Message'} = $param{'Message'};
    $mail{'Smtp'} = ($param{'Smtp'} ? $param{'Smtp'} : "mail.sap.corp");
    $mail{'Content-Type'} = $param{'Content-Type'} if($param{'Content-Type'});
    
    if ($param{'Subject'})
    {
        $mail{'Subject'} = $param{'Subject'};
    }
    else
    {
        $mail{'Subject'}  = FixVerStr($param{'LCVer'});
        $mail{'Subject'} .= " " . ($param{'Category'} ? $param{'Category'} : "Unknown") . ($param{'Type'} ? " $param{'Type'}" : "");
        chomp($hostname = `hostname`);
        $mail{'Subject'} .= " " . ($param{'Hostname'} ? $param{'Hostname'} : $hostname );
        $mail{'Subject'} .= ($param{'Infotext'} ? " - $param{'Infotext'}" : "");
    }
    
    my $rc = sendmail(%mail);
    my $mail_str = "";
    $mail_str .= "To     : $mail{'To'}\n";
    $mail_str .= "Cc     : $mail{'Cc'}\n";
    $mail_str .= "Subject: $mail{'Subject'}\n\nContent:\n\n$mail{'Message'}\n\n";
     
    return (wantarray ? ($rc, $mail_str) : $rc);
}

sub FixVerStr
{
    my $verstr = shift;
    my ($may,$min,$corr,$build, $state);
    
    $verstr = "X.X.XX.XX" unless ($verstr);
    
    $may = ($verstr =~ /^(\d).*/ ? $1 : ($ENV{'RELVER'} =~ /^R?(\d)/ ? $1 : "X"));
    $min = ($verstr =~ /^\d\.?(\d).*/ ? $1 : ($ENV{'RELVER'} =~ /^R\d(\d)/ ? $1 : "X"));
	$ENV{'CORRECTION_LEVEL'} = "XX" unless ($ENV{'CORRECTION_LEVEL'});
    $corr = ($verstr =~ /^\d\.?\d\.?(\d\d).*/ ? $1 : ($ENV{'CORRECTION_LEVEL'} =~ /^(\d\d)/ ? $1 : ($ENV{'CORRECTION_LEVEL'} =~ /^(\d)/ ? "0$1" : "XX")));
    $build = ($verstr =~ /^\d\.?\d\.?\d?\d?\.?(DEV|COR|RAMP|\d\d)/i ? uc($1) : ($ENV{'LC_STATE'} ? ($ENV{'LC_STATE'} =~ /^(DEV|COR)/i ? uc($1) : ($ENV{'LC_STATE'} =~ /RAMP/i ? ($ENV{'BUILDPRAEFIX'} =~ /^(\d\d)/ ? $1 : ($ENV{'BUILDPRAEFIX'} =~ /^(\d)/ ? "0$1" : "XX")) : "XX")) : "XX"));
    $state = $ENV{'LC_STATE'};

    if ($may eq "X")
    {
        $may = ((($verstr =~ /^TOOL/i) || ($ENV{'RELVER'} =~ /^R?TOOL/i)) ? "TOOL" : "X");
        if ($may ne "X")
        {
            $min = "";
            $corr = "";
            $build = ($verstr =~ /^TOOL\.?(.+)$/i ? "$1" : $ENV{'TOOLREL'} ? $ENV{'TOOLREL'} : "DEV");
        }
    }
    
    if (($may eq "X" || $min eq "X" || $corr eq "XX" || $build eq "XX") && $ENV{'TOOL'} && $ENV{'OWN'})
    {
        require Buildinfo;
        my $build_new;
        ($may, $min, $corr, $build_new, $state) = Buildinfo::get_buildinfo_from_gsp100("MajorVersion","MinorVersion","FORMATEDCorrectionLevel","FORMATEDBuildPrefix", "LC_STATE");
        $build = $build_new if ($build eq "XX");
    }
    
    my $res = $may . ($min ? ".$min" : "") . ($corr ? ".$corr" : "") . (($state && $state =~ /DEV|COR/) ? ".$state" : ($build ? ".$build" : ""));
	$ENV{'CORRECTION_LEVEL'} = undef if ($ENV{'CORRECTION_LEVEL'} eq "XX");
    return $res;
}

1;

