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
$cmd = "/bin/ls";
var_dump (jailed_shellcmd ($cmd));
?>
--EXPECT--
string(19) "/var/lib/php/bin/ls"
