--TEST--
Check for exec_re function that return success
--SKIPIF--
<?php
if ( ! extension_loaded ('execdir') ) {
	if ( version_compare(PHP_VERSION, "5.1.0", "<") ) {
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
exec_dir=
--FILE--
<?php
if ( version_compare(PHP_VERSION, "5.1.0", "<") )
	dl ('execdir.so');

$datafile = getcwd () . '/tests/data.txt';
if ( ! file_exists ($datafile) )
	$datafile = getcwd () . '/data.txt';

$ret = exec ("cat $datafile", $o, $r);
var_dump ($ret);
var_dump ($o);
var_dump ($r);
?>
--EXPECT--
string(12) "6. proc_open"
array(7) {
  [0]=>
  string(21) "php_execdir test data"
  [1]=>
  string(7) "1. exec"
  [2]=>
  string(9) "2. system"
  [3]=>
  string(11) "3. passthru"
  [4]=>
  string(13) "4. shell_exec"
  [5]=>
  string(8) "5. popen"
  [6]=>
  string(12) "6. proc_open"
}
int(0)
