
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
package p4infolib;
@EXPORT_OK = qw (GetMaxChangeNr GetNotAtHeadList GetLocalSource);

die "depot not in environment\n" unless ( defined $ENV{DEPOT} );

my %FstatInfo = GetFstatInfo();

#######################################################################
sub GetFstatInfo ()
{
 
    my ($File);
    
    open (PIPE, "p4 -s fstat $ENV{DEPOT}/... |") or die "can't open pipe: $@\n";
    while (<PIPE>) {
        
        last if ( /^exit:\s+(.*)/ );
        
        die "Error in GetFstatInfo: $_\n" if ( /^error/ );

        SWITCH: {
            /^info1:\s+clientFile\s+(.*)/  && 
                do { 
                    # previous file is not on client
                    $FstatInfo{$File}{headChange} = 0 if ($FstatInfo{$File}{haveRev} == 0);
                    $File  = $1;
                    # init for new file
                    $FstatInfo{$File}{haveRev}    = 0;
                    last SWITCH; 
                }; 
            /^info1:\s+headRev\s+(.*)/    && 
                do { 
                    $FstatInfo{$File}{headRev} = $1; 
                    last SWITCH; 
                }; 
            /^info1:\s+headChange\s+(.*)/ && 
                do { 
                    $FstatInfo{$File}{headChange} = $1; 
                    last SWITCH; 
                }; 
            /^info1:\s+haveRev\s+(.*)/    && 
                do { 
                    $FstatInfo{$File}{haveRev} = $1;
                    if ( $FstatInfo{$File}{haveRev} != $FstatInfo{$File}{headRev} ) {
                        $FstatInfo{$File}{headChange} = 0;
                    }
                    last SWITCH; 
                }; 
            /^info1:\s+action\s+(.*)/    && 
                do { 
                    $FstatInfo{$File}{action} = $1;
                    last SWITCH; 
                }; 
        }
                                            
    }
    close (PIPE);

    return %FstatInfo;
}


#######################################################################
sub GetTimestamp ()
{
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
    return sprintf("%0.2d:%0.2d:%0.2d", $hour,$min,$sec);
}

#######################################################################
sub GetMaxChangeNr ()
{

    my ($MaxFile) = sort {$FstatInfo{$b}{headChange} <=> $FstatInfo{$a}{headChange}} keys %FstatInfo;

    return $FstatInfo{$MaxFile}{headChange};
}

#######################################################################
sub GetFileCount ()
{

    return scalar keys %FstatInfo;
}

#######################################################################
sub GetNotAtHeadList ()
{

    my @NotAtHead;
    while ( my ($key, $val) = each %FstatInfo ) {
        push @NotAtHead, $key if ($$val{haveRev} != $$val{headRev});
    }
    
    return @NotHead;
}

#######################################################################
sub GetLocalSource ()
{

    my @LocalSource;
    while ( my ($key, $val) = each %FstatInfo ) {
        push @LocalSource, $key if (defined $$val{action});
    }
    
    return @LocalSource;
}

