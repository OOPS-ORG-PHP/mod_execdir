PHP execdir extension
===
[![PHP license](https://img.shields.io/badge/license-PHP-blue.svg)](https://raw.githubusercontent.com/php/php-src/master/LICENSE) [![GitHub issues](https://img.shields.io/github/issues/OOPS-ORG-PHP/mod_execdir.svg)](https://github.com/OOPS-ORG-PHP/mod_execdir/issues) [![GitHub forks](https://img.shields.io/github/forks/OOPS-ORG-PHP/mod_execdir.svg)](https://github.com/OOPS-ORG-PHP/mod_execdir/network) [![GitHub stars](https://img.shields.io/github/stars/OOPS-ORG-PHP/mod_execdir.svg)](https://github.com/OOPS-ORG-PHP/mod_execdir/stargazers)

## License

Copyright (c) 2016 JoungKyun.Kim &lt;http://oops.org&gt; All rights reserved.

This program is under PHP License.

##Description

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
  * backtick operator

## Installation

### 1. Requirement

PHP 5 와 PHP 7 이상에서 사용이 가능합니다.

소스 patch의 경우 php 4.3 부터 지원을 하며, PHP 5.3 까지는 safe_mode_exec_dir ini 옵션을 이용 합니다. 5.4 부터는 exec_dir ini 옵션을 이용 하십시오.

PHP 확장 모듈의 경우, PHP 5 호환 코드로 작성 하였지만, 실제 테스트는 PHP 5.1 이후 버전에서만 테스트 되었습니다.

이 기능을 사용하기 위해서는 2가지의 방법이 있습니다. 이 중에서 원하는 방법을 선택 하십시오.
  1. PHP source에 직접 패치
    * 코드에 직접 패치를 하기 때문에 mod_execdir 보다 성능이 좋음.
  2. mod_execdir 확장 사용
    * 원 함수를 hooking 및 alias를 하기 때문에 소스 패치보다는 성능이 미세하게 떨어짐.
    * ***pcntl_exec*** call 실패 시에, pcntl_get_last_error() 함수 사용을 못함. 코드 수정이 필요 함.


### 2. PHP source 에 직접 patch를 하는 경우

mod_execdir/patches 디렉토리에서 빌드할 PHP 버전에 맞는 patch 파일을 다운도르 합니다. 현재 빌드하려는 버전이 없을 경우에는 가장 최신의 버전을 다운로드 받으십시오. 이 의미는 이전 버전 패치가 문제가 없거나 또는 아직 지원을 하지 않을 수도 있음을 의미 합니다. 여기서는 PHP 7.0.7을 예로 듭니다.

```shell
[root@host ~]$ cd php-7.0.7
[root@host php-7.0.7]$ wget https://raw.githubusercontent.com/OOPS-ORG-PHP/mod_execdir/master/patches/php-7.0.7-execdir.patch
[root@host php-7.0.7]$ patch -p1 < ./php-7.0.7-execdir.patch
[root@host php-7.0.7]$ ./configure --with-exec-dir=/var/lib/php/bin ... (and with other options)
[root@host php-7.0.7]$ make && make install
```

### 3. 동적 확장으로 사용할 경우

```shell
[root@host mod_execdir]$ phpize
[root@host mod_execdir]$ ./configure --with-execdir=/var/lib/php/bin
[root@host mod_execdir]$ make test PHP_EXECUTABLE=/usr/bin/php
[root@host mod_execdir]$ make install
```

## Usage

### 1. 설정

php.ini에 다음의 설정을 추가 합니다.

php 동적 확장으로 빌드를 했을 경우에는 execdir.so 를 php.ini에서 로딩해 줘야 합니다.

```ini
extension = execdir.so
```

이 모듈은 기존의 system 함수들을 바꿔치기 하는 것이므로, 가장 마지막에 로딩되도록 추가해 줍니다.

실행할 수 있는 쉘 실행 파일이 있는 디렉토리를 지정합니다.

```ini
exe_cdir   = /var/lib/php/bin
```

PHP 5.3 이하 버전에서 소스 패치를 했을 경우에는, "***exec_dir***" 대신에 "***safe_mode_exec_dir***" ini 옵션을 사용해야 합니다.

```ini
safe_mode_exec_dir = /var/lib/php/bin
```


### 2. 명령어 파서 지원 형식

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

### 3. APIs

mod_execdir 확장 패키지로 빌드를 했을 경우에만 해당 됩니다.

#### 1. Original functions

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

### 2. mod_execdir APIs

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


## Contributors
JoungKyun.Kim
