--TEST--
Check for proc_open_re function that return success
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

$datafile = getcwd () . '/tests/data.txt';
if ( ! file_exists ($datafile) )
	$datafile = getcwd () . '/data.txt';

putenv ('LANG=C');
#if ( version_compare (phpversion(), '7.4.0', '>=') ) {
#	$cmd = array ("cat", "${datafile}");
#} else {
	$cmd = "cat ${datafile}";
#}
$proc = proc_open (
	$cmd,
	array(
		array ('pipe', 'r'),
		array ('pipe', 'w'),
		array ('pipe', 'w')
	),
	$pipes
);

var_dump ($proc);
print stream_get_contents ($pipes[1]);
print stream_get_contents ($pipes[2]);
?>
--EXPECT--
resource(7) of type (process)
php_execdir test data
1. exec
2. system
3. passthru
4. shell_exec
5. popen
6. proc_open
