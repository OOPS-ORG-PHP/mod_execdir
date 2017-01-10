# PHP Execdir Patches

## 1. Description

***PHP Execdir Patches*** are built by patching it directly into PHP source code. This is not function as a PHP extension.

If you build PHP using this patch, you can run only the shell command in the specified directory. And this means that possible to fundamentally protect the web shell and system shell injection attacks. And the following functions are affected:

 * exec
 * system
 * passthru
 * popen
 * proc_open
 * pcntl_exec
 * shell_exec
 * backtick operation
 
There are advantages and disadvantages to using this patch than using the mod_execdir extension module.

***Advantages:***
 1. Performance is better than extension.
 2. There is no restriction when using the ***pcntl_exec*** function.

***Disadvantages:***
 1. You need to rebuild PHP with patch.
 2. It takes a long time to build.

## 2. Installation

First, download patch file that matches PHP version from [mod_execdir/patches](https://github.com/OOPS-ORG-PHP/mod_execdir/blob/master/patches). If it does not exsits, download latest version of the patch file. This means that no problem to use previous version of the patch file or does not yet support.

```shell
[root@host ~]$ cd php-7.0.7
[root@host php-7.0.7]$ wget https://raw.githubusercontent.com/OOPS-ORG-PHP/mod_execdir/master/patches/php-7.0.7-execdir.patch
[root@host php-7.0.7]$ patch -p1 < ./php-7.0.7-execdir.patch
```

And execute configure with --with-exec-dir option for jailed system commands.

```shell
[root@host php-7.0.7]$ ./configure --prefix=/opt/php-7.0.7 \
                                   --with-exec-dir=/var/lib/php/bin \
                                   ... (and with other options)
[root@host php-7.0.7]$ make && make install
```

## 3. Configuration

### 3.1 PHP 5.3 and before

This patch enables the safe_mode_exec_dir feature if ***SAFE MODE*** is disabled. Therefore, in PHP 5.3 and before, this function will not work if the ***SAFE MODE*** is enabled!

In PHP 5.3 and before, use the ***safe_mode_exec_dir*** ini option to jail the sytsem commands.

```ini
; When safe_mode is on, only executables located in the safe_mode_exec_dir
; will be allowed to be executed via the exec family of functions.
; http://php.net/safe-mode-exec-dir
safe_mode_exec_dir = /var/lib/php/bin
```

If the ***safe_mode_exec_dir*** option is not set as follows, the ***--with-exec-dir*** value that specified at the ***configure*** time is used. If you did not provied the ***--with-exec-dir*** option at the ***configure*** time, then tme value of ***safe_mode_exec_dir*** will be empty.

```ini
; When safe_mode is on, only executables located in the safe_mode_exec_dir
; will be allowed to be executed via the exec family of functions.
; http://php.net/safe-mode-exec-dir
;safe_mode_exec_dir =
```

If you do not want to jail, you have to specify an empty value like this:

```ini
; When safe_mode is on, only executables located in the safe_mode_exec_dir
; will be allowed to be executed via the exec family of functions.
; http://php.net/safe-mode-exec-dir
safe_mode_exec_dir =
```

### 3.2 PHP 5.4 and after

In PHP 5.4 and after, ***SAFE MODE*** function has been removed from PHP, so use the ***exec_dir*** option instead of the ***safe_mode_exec_dir*** option. 

```ini
; only executables located in the exec_dir will be allowed to be executed
; via the exec family of functions. This is only AnNyung LInux patch
; see also https://github.com/OOPS-ORG-PHP/mod_execdir/
exec_dir = /var/lib/php/bin
```

If the ***safe_mode_exec_dir*** option is not set as follows, the ***--with-exec-dir*** value that specified at the ***configure*** time is used. If you did not provied the ***--with-exec-dir*** option at the ***configure*** time, then tme value of ***safe_mode_exec_dir*** will be empty.

```ini
; only executables located in the exec_dir will be allowed to be executed
; via the exec family of functions. This is only AnNyung LInux patch
; see also https://github.com/OOPS-ORG-PHP/mod_execdir/
;exec_dir =
```

If you do not want to jail, you have to specify an empty value like this:

```ini
; only executables located in the exec_dir will be allowed to be executed
; via the exec family of functions. This is only AnNyung LInux patch
; see also https://github.com/OOPS-ORG-PHP/mod_execdir/
exec_dir =
```

### 3.3 Settings per Apache VirtualHost

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

### 3.4 Settgins per PHP FPM pool

If you use PHP as fpm mode, you can set different for each FPM pool.

```ini
[www]
php_admin_flag[exec_dir] = /var/php/pool/www/bin

[www1]
php_admin_flag[exec_dir] = /var/php/pool/www1/bin
```

## 4. Operation

See also documents of [mod_execdir](https://github.com/OOPS-ORG-PHP/mod_execdir/blob/master/README.md) extension
