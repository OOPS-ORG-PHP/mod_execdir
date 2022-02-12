--TEST--
Check for popen_re function that return success
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

$pd = popen ("cat ${datafile}", 'r');
var_dump ($pd);
$read = fread ($pd, 1024);
echo $read;
pclose ($pd);
?>
--EXPECT--
resource(4) of type (stream)
php_execdir test data
1. exec
2. system
3. passthru
4. shell_exec
5. popen
6. proc_open
