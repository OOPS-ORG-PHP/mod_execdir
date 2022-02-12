--TEST--
Check for NULL byte detected error
--SKIPIF--
<?php
if ( ! extension_loaded ('execdir') ) {
	if ( version_compare (PHP_VERSION, "5.1.0", "<") ) {
		dl ('execdir.so');
		if ( ! extension_loaded ('execdir') )
			print 'skip';
	} else {
	    print 'skip';
	}
}
?>
--POST--
--GET--
--INI--
exec_dir=/var/lib/php/bin
--FILE--
<?php
if ( version_compare (PHP_VERSION, "5.1.0", "<") )
	dl ('execdir.so');

$cmd = array (
    'ls -al &> /dev/null',
    'ls -al &> /dev/null',
);

foreach ($cmd as $v ) {
    echo system ($v);
}
?>
--EXPECT--
