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
exec_dir=/var/lib/php/bin
--FILE--
<?php
$r = shell_exec ('ls /etc/hosts 2> /dev/null');
var_dump ($r);
?>
--EXPECT--
NULL
