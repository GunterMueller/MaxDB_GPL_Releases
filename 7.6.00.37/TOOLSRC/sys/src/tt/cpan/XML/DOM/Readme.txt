			Perl module: XML::DOM

Copyright (c) 1998 Enno Derksen and Clark Cooper.
All rights reserved.
This program is free software; you can redistribute it and/or modify it
under the same terms as Perl itself. 

This is a Perl extension to XML::Parser. It adds a new 'Style' to XML::Parser,
called 'Dom', that allows XML::Parser to build an Object Oriented datastructure 
with a DOM Level 1 compliant interface.
For a description of the DOM (Document Object Model), see http://www.w3.org/DOM/

XML::Parser is a Perl extension interface to James Clark's XML parser, expat.
It requires at least version 5.004 of perl and can be found on CPAN.

The documentation for the XML::DOM extension can be found in pod format at 
the end of the file DOM.pm.

This is a beta version and although there will not be any major API changes,
minor changes may occur as we get feedback
from the people on the perl-xml mailing list. [You can subscribe to
this list by sending a message to subscribe-perl-xml@lyris.activestate.com.]

For bug reports or suggestions send email to enno@att.com (NOT to Clark Cooper!)

To configure this module, cd to the directory that contains this README file
and type the following.

	perl Makefile.PL

Alternatively, if you plan to install XML::Parser somewhere other than
your system's perl library directory. You can type something like this:

	perl Makefile.PL PREFIX=/home/me/perl INSTALLDIRS=perl

Then to build you run make.

	make

You can then test the module by typing:

	make test

If you have write access to the perl library directories, you may then
install by typing:

	make install

Enno Derksen
enno@att.com
