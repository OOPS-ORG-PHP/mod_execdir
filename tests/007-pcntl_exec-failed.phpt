--TEST--
Check for pcntl_exec_re function that has command not found
--SKIPIF--
<?php
if ( ! extension_loaded ('execdir') ) {
    print 'skip';
}
?>
--POST--
--GET--
--INI--
exec_dir=/var/lib/php/bin
--FILE--
<?php
___ini_set('track_errors', true);
$datafile = getcwd () . '/tests/data.txt';
if ( ! file_exists ($datafile) )
	$datafile = getcwd () . '/data.txt';

$r = @pcntl_exec ("cat", array ($datafile));

if ( $r === false ) {
	var_dump ($r);
	var_dump ($php_errormsg);
}

?>
--EXPECT--
bool(false)
string(69) "pcntl_exec(): Error has occurred: (errno 2) No such file or directory"
