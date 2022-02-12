--TEST--
Check for exec_re function that has command not found
--SKIPIF--
<?php
if ( ! extension_loaded ('execdir') ) {
	if ( version_compare(PHP_VERSION, "5.1.0", "<") ) {
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
if ( version_compare(PHP_VERSION, "5.1.0", "<") )
	dl ('execdir.so');

$o = null;
$r = null;
$ret = exec ('ls /etc/hosts 2> /dev/null', $o, $r);
var_dump ($ret);
var_dump ($o);
var_dump ($r);
?>
--EXPECT--
string(0) ""
array(0) {
}
int(127)
