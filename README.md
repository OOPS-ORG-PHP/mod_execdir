PHP execdir extension
===

## License

Copyright (c) 2016 JoungKyun.Kim &lt;http://oops.org&gt; All rights reserved.

This program is under ***PHP License***.

## Description

If you use ***mod_execdir*** extension, you can run only the shell command in the specified directory. And this means that possible to ***fundamentally protect*** the web shell and system shell injection attacks.

This feature is an implementation to improve ***safe_mode_exec_dir*** before PHP 5.5.

***safe_mode_exec_dir*** function can be used only in ***SAFE MODE*** state, and it was removed from PHP 5.5. Also, the parser of ***safe_mode_exec_dir*** was written too simple and it has many restrictions in order to use.

The execdir exntesion is to complement the such disadvantages of ***safe_mode_exec_dir***, and support non ***SAFE MODE*** and over PHP 5.5.

This featurhe was made for <u>highlight syntax security hole of PHPBB</u> in May 2005. Also, it has been applied to large business site and AnNyung LInux distribution and verified more than 15 years.

This feature is provided as the source patch or php dynamic extension, and affects follow PHP functions:

  * exec
  * system
  * passthru
  * shell_exec
  * popen
  * proc_open
  * pcntl_exec
  * backtick operator

## Installation

### 1. Requirement

Over PHP 5 and PHP 7. But, it is tested on PHP 5.5 and after.

For this features, we support 2 building method.

  1. build with direct patch on PHP source code
    * support from PHP 4.3 and later
    * better performance than mod_execdir.
    * support ***pcntl_open***
  2. build with mod_execdir extension
    * support PHP 5 and later
    * verified test on PHP 5.5 and later
    * don't support ***pcntl_open***. Instead of using mod_jailed_pcntl extension (Not yet)


### 2. PHP source patch

First, download patch file that matches PHP version. If it does not exsits, download latest version of the patch file. This means that no problem to use previous version of the patch file or does not yet support.

And execute configure with ***--with-exec-dir*** option.

```shell
[root@host ~]$ cd php-7.0.7
[root@host php-7.0.7]$ wget https://raw.githubusercontent.com/OOPS-ORG-PHP/mod_execdir/master/patches/php-7.0.7-execdir.patch
[root@host php-7.0.7]$ patch -p1 < ./php-7.0.7-execdir.patch
[root@host php-7.0.7]$ ./configure --with-exec-dir=/var/lib/php/bin ... (and with other options)
[root@host php-7.0.7]$ make && make install
```


### 3. mod_execdir dynamic extension

```shell
[root@host mod_execdir]$ phpize
[root@host mod_execdir]$ ./configure --with-execdir=/var/lib/php/bin
[root@host mod_execdir]$ make test PHP_EXECUTABLE=/usr/bin/php
[root@host mod_execdir]$ make install
```

## Usage

### 1. Configuraions

Add follow configuration in th php.ini.

If you as with php extension(***mod_execdir***), you must regist execdir extension.

```ini
extension = execdir.so
```

This extension replaces the existing functions, so it is recommended that you last loaded.


Add directory for restricted shell command.

```ini
exec_dir   = /var/lib/php/bin
```

If you patched source code PHP 5.3 and before, you must use "***safe_mode_exec_dir***" option instead of "***exec_dir***"

```ini
; This case, PHP 5.3 and before with patched PHP source code
safe_mode_exec_dir = /var/lib/php/bin
```

### 2. Command parser format

Supported formats are as follows:

```
command
$(command)
`command`
command; command
command $(command)
command $(command $(command))
command $(command `command`)
command `command`
command | command
command && command
command || command
```

Example of applied parser is follows:

```php
exec ('ls -al /etc/hosts', $o, $r);
exec ('ls -al /etc/ | grep hosts', $o, $r);
exec ('/bin/ls /etc/ | grep hosts', $o, $r);
exec ('cat $(echo "/etc/passwd") | grep root; ls -al', $o, $r);
exec ('cat `echo "/etc/passwd"` | grep root; ls -al', $o, $r);
exec ("echo '$(ls -l | grep abc)' | grep abc");
exec ('echo "$(ls -l | grep abc)" | grep abc');
```

If execdir is set, The above code will actually be run in the following manner:

```php
exec ('/var/lib/php/bin/ls -al /etc/hosts', $o, $r);
exec ('/var/lib/php/bin/ls -al /etc/ | /var/lib/php/bin/grep hosts', $o, $r);
exec ('/var/lib/php/bin/ls -al /etc/ | /var/lib/php/bin/grep hosts', $o, $r);
exec ('/var/lib/php/bin/cat $(/var/lib/php/bin/echo "/etc/passwd") | /var/lib/php/bin/grep root; /var/lib/php/bin/ls -al', $o, $r);
exec ('/var/lib/php/bin/cat $(/var/lib/php/bin/echo "/etc/passwd") | /var/lib/php/bin/grep root; /var/lib/php/bin/ls -al', $o, $r);
exec ("/var/lib/php/bin/echo '$(ls -l | grep abc)' | /var/lib/php/bin/grep abc");
exec ('/var/lib/php/bin/echo "$(/var/lib/php/bin/ls -l | /var/lib/php/bin/grep abc)" | /var/lib/php/bin/grep abc');
```

### 3. APIs

The following case is applied with building in mod_execdir as PHP dynamic extension.

#### 1. Original functions

You can call original functions with "***_orig***" postfix.

```php
<?php
exec_orig ('ls /etc/hosts', $o, $r);
var_dump ($o);
?>
```

The list of original functions is follow:

  * exec_orig
  * system_orig
  * passthru_orig
  * shell_exec_orig
  * popen_orig
  * proc_open_orig
  * proc_close_orig
  * proc_terminate_orig
  * proc_get_status_orig

### 2. mod_execdir APIs

  * ***exec_re*** : mapping ***exec*** function
  * ***system_re*** : mapping ***system*** function
  * ***passthru_re*** : mapping ***passthru*** function
  * ***shell_exec_re*** : mapping ***shell_exec*** function
  * ***popen_re*** : mapping ***popen*** function
  * ***popen_re*** : mapping ***popen*** function
  * ***proc_open_re*** : mapping ***proc_open*** function
  * ***proc_close_re*** : mapping ***proc_close*** function
  * ***proc_terminate_re*** :  mapping ***proc_terminate*** function
  * ***proc_get_status_re*** : mapping ***proc_get_status*** function
  * ***jailed_shellcmd*** : return jailed shell command strings
  ```
  Prototype: (string) jailed_shellcmd (string path)
  ```
  
  ```php
  <?php
  # Follow codes has same operation with "system(command)"
  $jcmd = jailed_shellcmd ('/bin/ls');
  system_orig ($jcmd);
  ?>
  ```

## Contributors
JoungKyun.Kim
