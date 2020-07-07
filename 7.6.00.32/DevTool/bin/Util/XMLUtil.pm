package Util::XMLUtil;

use BuildPackage::XMLParser;
use IO::File;

@ISA = ('Exporter');

@EXPORT = ('ParseXMLFile', 'getSubTree', 'getChildren');

sub err{
	my ($text) = @_;
	die($text);
}

# sub parse the XML file with release/file information and return a tree of XMLElemets
sub ParseXMLFile {
	local $xmlfile = shift;
	$XMLFile = $xmlfile if ( defined $xmlfile );
	my ($fh, $rc);
	$^O=~/mswin/i && $XMLFile=~s/\//\\/g;
	$fh = IO::File::new();
	$rc = open ($fh, "< ".$XMLFile);
	unless ($rc) {
		err("cannot open ".$XMLFile."\n");
		err("WARNING: NO XML FILE FOR OTHER RUNTIMES FOUND\n"); 
		return 0;
	}
	my $Parser = BuildPackage::XMLParser::new ();
	my $Tree = $Parser->ParseStream ($fh);
	$fh->close ();
	return ($Tree);
}	


# sub return a subtree (XMLElement) specified by entity name and its content
sub getSubTree{
	my ($tree,$type,$ref_content,$hash_attrib,$hide_errors)=@_;
	my $content = $$ref_content;
	undef $content if ( $ref_content == 0 ) ;
	$tree==0 && return 0;
	while(1){
		if($tree->GetType eq $type){
			while(1){
				local $found_tree  = 1;
				if (defined $content)
				{ $found_tree = 0 unless( $tree->GetContent eq $content ) };
				if ( $found_tree )
				{
					if (defined $hash_attrib && ( ref($hash_attrib) eq 'HASH') )
					{
						$found_tree = 0;
						foreach $key (keys(%$hash_attrib))
						{
						#	$test=$tree->GetAttribute ($key);
						#	$test1=$hash_attrib->{$key};
							if ( $tree->GetAttribute ($key) eq $hash_attrib->{$key})
							{
								$found_tree = 1;
								last;
							}
						}
					}
				}
				return $tree if ($found_tree);	
				$tree=$tree->VisitNeighbor;
				if($tree==0){
					unless ($hide_errors)
					{
						local $error_text = "subtree ";
						$error_text.= "\"$content\" " if (defined $content);
						if (defined $hash_attrib && ( ref($hash_attrib) eq 'HASH') )
						{
							$error_text.= "with attributes ";
							foreach $key (keys(%$hash_attrib))
							{	
								$error_text.= "$key=$hash_attrib->{$key},"
							}
							chop $error_text;
						}
						$error_text.=" not found in type \"$type\" in XML tree\n";
						err ($error_text);
					}
					return 0;
				}
			}
			last;
		}
		$tree=$tree->VisitChild;
		if($tree == 0){
			err("entity \"$type\" not found in tree\n");
			return 0;
		} 
	}
}

# return all child entities (subtrees) as list

sub getChildren{
	my($tree)=@_;
	$tree==0 && return;
	$tree=$tree->VisitChild;
	$tree==0 && return;
	my @treelist;
	while(1){
		push @treelist,$tree unless($tree==0);
		$tree=$tree->VisitNeighbor;
		$tree ==0 && return @treelist;
	}
}

1;


