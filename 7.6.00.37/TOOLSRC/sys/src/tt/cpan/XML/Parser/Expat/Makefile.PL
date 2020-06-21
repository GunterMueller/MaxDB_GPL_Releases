use ExtUtils::MakeMaker;
use Config;
use English;

sub MY::postamble {
  my $self = shift;
  my $ret = '';

  unless ($Config{d_memmove}) {
    $ret = "expat/xmlparse/xmlparse.o:\n\t";
    $ret .= '$(CCCMD) $(CCCDLFLAGS) -I$(PERL_INC) ';
    $ret .= '-D"memmove(d,s,l)=bcopy(s,d,l)" $(DEFINE) ';
    $ret .= 'expat/xmlparse/xmlparse.c';
  }

  $ret;
}

$define = '-DXML_NS -DXML_DTD -DXML_CONTEXT_BYTES=512 ';

$order = ($Config{byteorder} =~ /^12/) ? '12' : '21';

$define .= "-DXML_BYTE_ORDER=$order ";

$define .= ($Config{cc} =~ /cl(\.exe)?$/i) ? '/Fo$@' : '-o $@';

@expat_files = qw(expat/xmltok/xmltok.c expat/xmltok/xmlrole.c
                  expat/xmlparse/xmlparse.c expat/xmlparse/hashtable.c);

@extras = ();

push(@extras,
     CAPI => 'TRUE')
    if ($PERL_VERSION >= 5.005 and $OSNAME eq 'MSWin32'
	and $Config{archname} =~ /-object\b/i);

push(@extras,
     ABSTRACT => "Lowlevel access to James Clark's expat XML parser",
     AUTHOR        => 'Clark Cooper (coopercc@netheaven.com)')
    if ($ExtUtils::MakeMaker::Version >= 5.4301);
     
WriteMakefile(
              NAME	=> 'XML::Parser::Expat',
	      C       => ['Expat.c', @expat_files],
              OBJECT    => '$(O_FILES)',
              DEFINE    => $define,
	      clean     => {FILES => 'expat/xml*/*.o'},
              INC       => "-Iexpat/xmltok -Iexpat/xmlparse",
              XSPROTOARG => '-noprototypes',
              VERSION_FROM => 'Expat.pm',
	      @extras
             );

