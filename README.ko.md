PHP execdir extension
===
[![PHP license](https://img.shields.io/badge/license-PHP-blue.svg)](https://raw.githubusercontent.com/php/php-src/master/LICENSE) [![GitHub issues](https://img.shields.io/github/issues/OOPS-ORG-PHP/mod_execdir.svg)](https://github.com/OOPS-ORG-PHP/mod_execdir/issues) [![GitHub forks](https://img.shields.io/github/forks/OOPS-ORG-PHP/mod_execdir.svg)](https://github.com/OOPS-ORG-PHP/mod_execdir/network) [![GitHub stars](https://img.shields.io/github/stars/OOPS-ORG-PHP/mod_execdir.svg)](https://github.com/OOPS-ORG-PHP/mod_execdir/stargazers)

## 1. License

Copyright (c) 2018 JoungKyun.Kim &lt;http://oops.org&gt; All rights reserved.

This program is under PHP License.

## 2. Description

***mod_execdir*** 확장은 특정 디렉토리에 있는 명령만 실행할 수 있도록 제한을 하여, web shell이나 system shell injection 공격을 <u>원천적으로 방어</u>할 수 있습니다.

이 기능은 PHP 5.4 이전의 ***safe_mode_exec_dir*** 기능을 향상 구현한 것입니다.

***safe_mode_exec_dir*** 기능은 오직 ***SAFE MODE***에서만 사용이 가능하며, PHP 5.4 이후 부터는 ***SAFE MODE***가 제거 되면서 더 이상 사용을 할 수가 없습니다. 또한, ***safe_mode_exec_dir***의 parser는 너무나 간단하게 되어 있어 사용을 하기에 제약이 많았습니다.

execdir 기능은 이런 ***safe_mode_exec_dir*** 기능의 단점을 보완하여, ***SAFE MODE***가 아니고, 또한 PHP 5.4 이후 버전에서도 사용할 수 있도록 지원을 합니다.

2005년 5월 PHPBB의 <u>highlight syntax security hole</u> 때문에 만들어 졌으며, 대형 비지니스 사이트와 안녕 리눅스 배포본에 적용이 되어 10년 이상 검증이 되었습니다.

이 기능은 소스 코드에 직접 patch를 하거나, 또는 php dynamic extension으로 사용할 수 있으며, 이 기능은 다음의 PHP 함수에 영향을 미칩니다:

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

PHP 5 와 PHP 7 이상에서 사용이 가능합니다.

PHP 확장 모듈의 경우, PHP 5 호환 코드로 작성 하였지만, 실제 테스트는 PHP 5.1 이후 버전에서만 테스트 되었습니다.

이 기능을 사용하기 위해서는 2가지의 방법이 있습니다. 이 중에서 원하는 방법을 선택 하십시오.
  1. PHP source에 직접 패치
      * 코드에 직접 패치를 하기 때문에 mod_execdir 보다 성능이 좋음.
  2. mod_execdir 확장 사용
      * 원 함수를 hooking 및 alias를 하기 때문에 소스 패치보다는 성능이 미세하게 떨어짐.
      * ***pcntl_exec*** call 실패 시에, pcntl_get_last_error() 함수 사용을 못함. 코드 수정이 필요 함.


### 3.2. PHP source 에 직접 patch를 하는 경우

이 기능은 확장 모듈로서 사용을 할 수도 있고, PHP 소스에 직접 패치를 할 수도 있습니다. 확장 기능으로 사용하는 것 보다 PHP에 직접 패치를 해서 사용하는 것을 선호 한다면, [Patch 파일에 대한 설명](https://github.com/OOPS-ORG-PHP/mod_execdir/blob/master/patches/README.ko.md)을 참조 하십시오.

### 3.3. 동적 확장으로 사용할 경우

```shell
[root@host mod_execdir]$ phpize
[root@host mod_execdir]$ ./configure --with-execdir=/var/lib/php/bin
[root@host mod_execdir]$ make test PHP_EXECUTABLE=/usr/bin/php
[root@host mod_execdir]$ make install
```

***configure***시에 ***--with-execdir*** 옵션을 이용하여 jail을 시킬 기본 디렉토리를 지정할 수 있습니다.

## 4.Usage

### 4.1. 설정

#### 4.1.1 module loading

php.ini에 다음의 설정을 추가 합니다. php 동적 확장으로 빌드를 했을 경우에는 execdir.so 를 php.ini에서 로딩해 줘야 합니다.

```ini
extension = execdir.so
```

이 모듈은 기존의 system 함수들을 바꿔치기 하는 것이므로, 가장 마지막에 로딩되도록 추가해 줍니다.

#### 4.1.2 ini 설정

***exec_dir*** 옵션을 이용하여, jail 시킬 디렉토리를 지정할 수 있습니다.

```ini
; only executables located in the exec_dir will be allowed to be executed
; via the exec family of functions. This is only AnNyung LInux patch
; see also https://github.com/OOPS-ORG-PHP/mod_execdir/
exec_dir = /var/lib/php/bin
```

아래와 같이 ***exec_dir*** 옵션이 설정이 되어 있지 않을 경우에는, ***configure***시에 지정한 ***--with-execdir*** 값이 사용이 됩니다. ***configure*** 시에 ***--with-execdir*** 옵션을 주지 않았다면, 이 경우 ***exec_dir***의 값은 빈 값이 됩니다.

```ini
; only executables located in the exec_dir will be allowed to be executed
; via the exec family of functions. This is only AnNyung LInux patch
; see also https://github.com/OOPS-ORG-PHP/mod_execdir/
;exec_dir =
```

만약, jail을 하고 싶지 않다면 다음과 같이 빈 값을 지정해 놓아야 합니다.

```ini
; only executables located in the exec_dir will be allowed to be executed
; via the exec family of functions. This is only AnNyung LInux patch
; see also https://github.com/OOPS-ORG-PHP/mod_execdir/
exec_dir =
```

#### 4.1.3 Apache VirtualHost

***PHP***를 ***apache module***로 사용할 경우 ***php_admin_value*** 지시자를 이용하여 가상 호스트마다 설정을 다르게 할 수 있습니다.
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

이 설정을 응용하면 ```<Directory>```, ```<Location>``` 등의 블럭에서도 사용이 가능 합니다.

***exec_dir*** 옵션은 ***PHP_INI_SYSTEM***으로 할당이 되어 있기 때문에, ***.htaccess*** 에서는 사용이 불가능 합니다.

#### 4.1.4 PHP FPM pool

***PHP***를 ***fpm*** 모드로 사용할 경우에는, FPM pool 별로 설정이 가능 합니다.

```ini
[www]
php_admin_flag[exec_dir] = /var/php/pool/www/bin

[www1]
php_admin_flag[exec_dir] = /var/php/pool/www1/bin
```

### 4.2. 명령어 파서 지원 형식

paser에서 지원하는 형식은 다음과 같습니다:

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

파서 적용 예는 다음과 같습니다:

```php
exec ('ls -al /etc/hosts', $o, $r);
exec ('ls -al /etc/ | grep hosts', $o, $r);
exec ('/bin/ls /etc/ | grep hosts', $o, $r);
exec ('cat $(echo "/etc/passwd") | grep root; ls -al', $o, $r);
exec ('cat `echo "/etc/passwd"` | grep root; ls -al', $o, $r);
exec ("echo '$(ls -l | grep abc)' | grep abc");
exec ('echo "$(ls -l | grep abc)" | grep abc');
```

execdir 설정이 되어 있을 경우, 위의 코드들은 실제로는 다음과 같이 실행이 됩니다:

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

mod_execdir 확장 패키지로 빌드를 했을 경우에만 해당 됩니다.

#### 4.3.1. Original functions

기존의 함수들은 ***_orig*** 접미사를 이용하여 호출을 할 수 있습니다.

```php
<?php
exec_orig ('ls /etc/hosts', $o, $r);
var_dump ($o);
?>
```

원 함수 목록은 다음과 같습니다.

  * exec_orig
  * system_orig
  * passthru_orig
  * shell_exec_orig
  * popen_orig
  * proc_open_orig
  * proc_close_orig
  * proc_terminate_orig
  * proc_get_status_orig

#### 4.3.2. mod_execdir APIs

  * ***exec_re*** : mapping ***exec*** function
  * ***system_re*** : mapping ***system*** function
  * ***passthru_re*** : mapping ***passthru*** function
  * ***shell_exec_re*** : mapping ***shell_exec*** function
  * ***popen_re*** : mapping ***popen*** function
  * ***proc_open_re*** : mapping ***proc_open*** function
  * ***proc_close_re*** : mapping ***proc_close*** function
  * ***proc_terminate_re*** :  mapping ***proc_terminate*** function
  * ***proc_get_status_re*** : mapping ***proc_get_status*** function
  * ***pcntl_exec_re*** : mapping ***pcntl_exec*** function  
    ***pcntl_exec*** 함수 호출 에러시에, ***pcntl_get_last_error()*** 함수를 사용할 수 없습니다. 그러므로, 다음과 같이 약간의 소스 수정이 필요 합니다.  
    ***이전:***  

  ```php
  <?php
  if ( ($r = @pcntl_exec ('/bin/cat', array ('/etc/hosts')) === false ) {
      echo pcntl_strerror (pcntl_get_last_error ()) . "\n"
  }
  ```
    ***이후:***  
    
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
  # 다음 코드는 system(command) 과 동일하게 동작
  $jcmd = jailed_shellcmd ('/bin/ls');
  system_orig ($jcmd);
  ?>
  ```


## 5.Contributors
JoungKyun.Kim
