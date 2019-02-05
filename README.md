PHP execdir extension
===
[![PHP license](https://img.shields.io/badge/license-PHP-blue.svg)](https://raw.githubusercontent.com/php/php-src/master/LICENSE)
![GitHub download](https://img.shields.io/github/downloads/OOPS-ORG-PHP/mod_execdir/total.svg)
[![GitHub last release](https://img.shields.io/github/release/OOPS-ORG-PHP/mod_execdir.svg)](https://github.com/OOPS-ORG-PHP/mod_execdir/releases)
[![GitHub closed issues](https://img.shields.io/github/issues-closed-raw/OOPS-ORG-PHP/mod_execdir.svg)](https://github.com/OOPS-ORG-PHP/mod_execdir/issues?q=is%3Aissue+is%3Aclosed)
[![GitHub closed pull requests](https://img.shields.io/github/issues-pr-closed-raw/OOPS-ORG-PHP/mod_execdir.svg)](https://github.com/OOPS-ORG-PHP/mod_execdir/pulls?q=is%3Apr+is%3Aclosed)

## 1. License

Copyright (c) 2018 JoungKyun.Kim &lt;http://oops.org&gt; All rights reserved.

This program is under ***PHP License***.

## 2. Description

If you use ***mod_execdir*** extension, you can run only the shell command in the specified directory. And this means that possible to ***fundamentally protect*** the web shell and system shell injection attacks.

This feature is an implementation to improve ***safe_mode_exec_dir*** before PHP 5.4.

***safe_mode_exec_dir*** function can be used only in ***SAFE MODE*** state, and it was removed from PHP 5.4. Also, the parser of ***safe_mode_exec_dir*** was written too simple and it has many restrictions in order to use.

The execdir exntesion is to complement the such disadvantages of ***safe_mode_exec_dir***, and support non ***SAFE MODE*** and over PHP 5.4.

This featurhe was made for <u>highlight syntax security hole of PHPBB</u> in May 2005. Also, it has been applied to large business site and AnNyung LInux distribution and verified more than 10 years.

This feature is provided as the source patch or php dynamic extension, and affects follow PHP functions:

  * exec
  * system
  * passthru
  * shell_exec
  * popen
  * proc_open
  * pcntl_exec
  * shell_exec
  * backtick operator

## 3. Installation

### 3.1. Requirement

Over PHP 5 and PHP 7. But, it is tested on PHP 5.1 and after.

For this features, we support 2 building method.

  1. build with direct patch on PHP source code
      * support from PHP 4.3 and later
      * better performance than mod_execdir.
  2. build with mod_execdir extension
      * support PHP 5 and later
      * verified test on PHP 5.5 and later
      * if failed call ***pcntl_exec***, can not use ***pcntl_get_last_error*** function. Need to modify your code.


### 3.2. PHP source patch

This feature can be used as a PHP extension, or you can patch it directly to the PHP source. If you prefer to patch your PHP directly rather than using it as an extension, see the [description of the patch file](https://github.com/OOPS-ORG-PHP/mod_execdir/blob/master/patches/).


### 3.3. mod_execdir dynamic extension

```shell
[root@host mod_execdir]$ phpize
[root@host mod_execdir]$ ./configure --with-execdir=/var/lib/php/bin
[root@host mod_execdir]$ make test PHP_EXECUTABLE=/usr/bin/php
[root@host mod_execdir]$ make install
```

At ***configure*** time, you can use the ***--with-execdir*** option to specify the default directory for jail

## 4. Usage

### 4.1. Configuraions

#### 4.1.1 module loading

If build with PHP dynamic extension, you will need to load ***execdir.so*** file in ***php.ini***. Add follow configuration in th php.ini.

```ini
; for php 7.1 and before
extension = execdir.so

; for php 7.2 and after
; default extension dir
extension = execdir
; use absolute path
extension = /path/execdir.so
```

This extension replaces the existing functions, so it is recommended that you last loaded.

#### 4.1.2 ini settings

Add directory for restricted shell command.

```ini
; only executables located in the exec_dir will be allowed to be executed
; via the exec family of functions. This is only AnNyung LInux patch
; see also https://github.com/OOPS-ORG-PHP/mod_execdir/
exec_dir = /var/lib/php/bin
```

If the ***exec_dir*** option is not set as follows, the ***--with-execdir*** value that specified at the ***configure*** time is used. If you did not provied the ***--with-execdir*** option at the ***configure*** time, then tme value of ***exec_dir*** will be empty.

```ini
; only executables located in the exec_dir will be allowed to be executed
; via the exec family of functions. This is only AnNyung LInux patch
; see also https://github.com/OOPS-ORG-PHP/mod_execdir/
;exec_dir =
```

If you do not want to restrict, you have to specify an empty value like this:

```ini
; only executables located in the exec_dir will be allowed to be executed
; via the exec family of functions. This is only AnNyung LInux patch
; see also https://github.com/OOPS-ORG-PHP/mod_execdir/
exec_dir =
```

#### 4.1.3 Settings per Apache VirtualHost

If you use PHP as an ***apache module***, you can use the ***php_admin_value*** directive to make different settings for each ***virtual host***.

```apache
<VirtualHost *:80>
    ServerName domain.com
    DocumentRoot /var/www/domain.com

    <IfModule php7_module>
        php_admin_flag exec_dir /var/php/domain.com/bin
    </IfModule>
</VirtualHost>

<VirtualHost *:80>
    ServerName domain-other.com
    DocumentRoot /var/www/domain-other.com

    <IfModule php7_module>
        php_admin_flag exec_dir /var/php/domain-other.com/bin
    </IfModule>
</VirtualHost>
```

This setting can be applied in ```<Directory>```, ```<Location>``` and other blocks.

The ***exec_dir*** option is assigned to ***PHP_INI_SYSTEM***, so it can not be used with ***.htaccess***.


#### 4.1.4 Settgins per PHP FPM pool

If you use PHP as fpm mode, you can set different for each FPM pool.

```ini
[www]
php_admin_flag[exec_dir] = /var/php/pool/www/bin

[www1]
php_admin_flag[exec_dir] = /var/php/pool/www1/bin
```


### 4.2. Command parser format

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

### 4.3. APIs

The following case is applied with building in mod_execdir as PHP dynamic extension.

#### 4.3.1. Original functions

You can call original functions with ***_orig*** postfix.

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

These functions can be called from ```1.0.2``` only by giving ***--enable-execdir-addon*** option at ***configure***.

#### 4.3.2. mod_execdir APIs

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
  * ***pcntl_exec_re*** : mapping ***pcntl_exec*** function  
    Can not use ***pcntl_get_last_error()*** fucntion, if call failed ***pcntl_exec***, so Need to modify your code as follows:  
    ***Before:***  

    ```php
    <?php
    if ( ($r = @pcntl_exec ('/bin/cat', array ('/etc/hosts')) === false ) {
        echo pcntl_strerror (pcntl_get_last_error ()) . "\n"
    }
    ?>
    ```
    ***After:***  
    
    ```php
    ini_set ('track_errors', true);
    if ( ($r = @pcntl_exec ('/bin/cat', array ('/etc/hosts')) === false ) {
        echo $php_errormsg . "\n";
    }
    ```
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

## 5. Contributors
JoungKyun.Kim
