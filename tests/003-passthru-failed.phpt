--TEST--
Check for passthru_re function that has command not found
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
passthru ('ls /etc/hosts 2> /dev/null', $r);
var_dump ($r);
?>
--EXPECT--
int(127)
