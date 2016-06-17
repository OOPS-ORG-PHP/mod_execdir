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

This feature is provided as the source patch or php extension, and affects follow PHP functions:

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


### 3. mod_execdir extension

```shell
[root@host mod_execdir]$ phpize
[root@host mod_execdir]$ ./configure --with-execdir=/var/lib/php/bin
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

### Contributors
JoungKyun.Kim
