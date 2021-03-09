--TEST--
Check for pcntl_exec_re function that return success
--SKIPIF--
<?php
if ( ! extension_loaded ('execdir') ) {
    print 'skip';
}
?>
--POST--
--GET--
--INI--
exec_dir=
--FILE--
<?php
ini_set('track_errors', true);
$datafile = getcwd () . '/tests/data.txt';
if ( ! file_exists ($datafile) )
	$datafile = getcwd () . '/data.txt';

$r = @pcntl_exec ("/bin/cat", array ($datafile));

if ( $r === false ) {
	var_dump ($r);
	if ( version_compare (PHP_VERSION, '8.0.0') >= 0 ) {
		var_dump (error_get_last ()['message']);
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
