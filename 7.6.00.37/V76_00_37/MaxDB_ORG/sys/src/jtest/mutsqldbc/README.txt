
Multi User Test Release 1
Alexander Küchler, Nov 2004
=========================

1. Compile the targets
----------------------

    $ imf mutsqldbc

2. Prepare the test
-------------------

    $ cd $OWN/jtest/mutsqldbc
    $ mutparam.pl
        create the databasae configuration
        
3. Run the test
---------------

    $ muti.pl 10
        create the database and execute 10 test runs
    



