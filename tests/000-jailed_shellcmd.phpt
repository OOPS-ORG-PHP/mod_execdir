--TEST--
Check for jailed_shellcmd function
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
exec_dir=/var/lib/php71/bin
--FILE--
<?php
if ( version_compare (PHP_VERSION, "5.1.0", "<") )
	dl ('execdir.so');

if ( file_exists ('tests/cmdformat.txt') )
	$target = 'tests/cmdformat.txt';
else
	$target = 'cmdformat.txt';

$fp = fopen ($target, 'rb');
$exec_dir = ini_get ('exec_dir');

while ( ($buf = fgets ($fp, 256)) !== false ) {
	$buf = trim ($buf);
	if ( preg_match ('/#execdir#/', $buf) ) {
		$res[] = preg_replace ('/#execdir#/', $exec_dir, $buf);
	} else {
		$cmds[] = $buf;
	}
}

foreach ( $cmds as $key => $cmd ) {
	$r = jailed_shellcmd ($cmd);

	if ( $r == $res[$key] ) echo "yes\n";
	else echo "no\n";
}
?>
--EXPECT--
yes
yes
yes
yes
yes
yes
yes
yes
yes
yes
yes
yes
yes
yes
yes
yes
yes
yes
