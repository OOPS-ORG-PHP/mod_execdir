--TEST--
Check for pcntl_exec_re function that return success
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
exec_dir=
--FILE--
<?php
if ( version_compare (PHP_VERSION, "5.1.0", "<") )
	dl ('execdir.so');

ini_set('track_errors', true);
$datafile = getcwd () . '/tests/data.txt';
if ( ! file_exists ($datafile) )
	$datafile = getcwd () . '/data.txt';

$r = @pcntl_exec ("/bin/cat", array ($datafile));

if ( $r === false ) {
	var_dump ($r);
	if ( version_compare (PHP_VERSION, '8.0.0') >= 0 ) {
		$buf = error_get_last ();
		var_dump ($buf['message']);
	} else {
		var_dump ($php_errormsg);
	}
}

?>
--EXPECT--
php_execdir test data
1. exec
2. system
3. passthru
4. shell_exec
5. popen
6. proc_open
