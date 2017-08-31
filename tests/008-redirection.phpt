--TEST--
Check for shell redirection
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
    'ls -al asdf 2> /dev/null',
    'ls -al &> /dev/null',
    'ls -al >& /dev/null',
    'ls -al > /dev/null 2>&1',
    'ls -al &> /dev/null &'
);

foreach ($cmd as $v ) {
    echo system ($v);
}
?>
--EXPECT--
