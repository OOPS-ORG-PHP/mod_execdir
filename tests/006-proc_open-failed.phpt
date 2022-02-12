--TEST--
Check for proc_open_re function that has command not found
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
sh: /var/lib/php/bin/cat: No such file or directory
