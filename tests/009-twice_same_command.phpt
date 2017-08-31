--TEST--
Check for NULL byte detected error
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
$cmd = array (
    'ls -al &> /dev/null',
    'ls -al &> /dev/null',
);

foreach ($cmd as $v ) {
    echo system ($v);
}
?>
--EXPECT--
