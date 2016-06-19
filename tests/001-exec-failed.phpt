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
$o = null;
$r = null;
$ret = exec ('ls /etc/hosts 2> /dev/null', $o, $r);
var_dump ($ret);
var_dump ($o);
var_dump ($r);
?>
--EXPECT--
string(0) ""
array(0) {
}
int(127)
