--TEST--
Check for shell_exec_re function that return success
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
$datafile = getcwd () . '/tests/data.txt';
if ( ! file_exists ($datafile) )
	$datafile = getcwd () . '/data.txt';

$r = shell_exec ("cat $datafile");
var_dump ($r);
?>
--EXPECT--
string(88) "php_execdir test data
1. exec
2. system
3. passthru
4. shell_exec
5. popen
6. proc_open
"
