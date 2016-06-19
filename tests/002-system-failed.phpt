--TEST--
Check for system_re function that has command not found
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
$ret = system ('ls /etc/hosts 2> /dev/null', $r);
var_dump ($ret);
var_dump ($r);
?>
--EXPECT--
string(0) ""
int(127)
