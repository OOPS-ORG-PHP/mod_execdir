--TEST--
Check for execdir extension
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
sh: /var/lib/php/bin/cat: No such file or directory
