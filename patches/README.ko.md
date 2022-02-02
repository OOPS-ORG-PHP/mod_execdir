# PHP Execdir Patch

Copyright 2022. JoungKyun.Kim All rights reserved.

## 1. 설명

***PHP Execdir Patch*** 는 [***mod_execdir***](https://github.com/OOPS-ORG-PHP/mod_execdir/) 기능을 확장 기능으로 사용하는 것이 아니라, PHP code에 직접 pacth를 하여 PHP의 main 기능으로 사용하는 패치 입니다.

이 패치를 이용하면, PHP의 시스템 함수를 사용시에 특정 디렉토리에 있는 명령만 실행할 수 있도록 제한을 하여, ***web shell*** 이나 ***system shell injection*** 공격을 <u>원천적으로 방어</u>할 수 있으며, 다음의 함수에 영향을 미칩니다.

 * exec
 * system
 * passthru
 * popen
 * proc_open
 * pcntl_exec
 * shell_exec
 * backtick operation


***mod_execdir*** 확장 모듈을 이용하는 것 보다 이 패치를 이용할 경우에는 다음의 ***장단점*** 이 있습니다.

***장점:***

1. 동적 모듈이 아니기 때문에 성능이 조금 더 좋다.
2. ***pcntl_exec*** 함수 사용시에, ***pcntl_strerror*** 함수를 사용할 수 있다.

***단점:***

1. patch를 적용해서 PHP를 다시 빌드해야 한다.
2. 빌드 시간이 오래 걸린다.

이 patch들은 각 버전별로 제공을 합니다. 만약 해당 버전이 없다면, 해당 버전에 가장 가까운 버전을 선택하면 됩니다. patch 파일은 해당 PHP 버전에 patch가 실패할 경우 새로운 버전으로 패치 파일이 생성이 됩니다.

예를 들어, php 7.1.0 patch 파일이 php 7.1.5에 잘 적용이 된다면, php 7.1.0 patch 파일이 실패하는 버전이 나올때 까지 유지가 됩니다.

## 2. 설치

[***mod_execdir/patches***](https://github.com/OOPS-ORG-PHP/mod_execdir/tree/master/patches) 디렉토리에서 빌드할 PHP 버전에 맞는 patch 파일을 다운도르 합니다. 현재 빌드하려는 버전이 없을 경우에는 가장 최신의 버전을 다운로드 받으십시오. 이 의미는 이전 버전 패치가 문제가 없거나 또는 아직 지원을 하지 않을 수도 있음을 의미 합니다. 여기서는 PHP 7.0.7을 예로 듭니다.

```shell
[root@host ~]$ cd php-7.0.7
[root@host php-7.0.7]$ wget https://raw.githubusercontent.com/OOPS-ORG-PHP/mod_execdir/master/patches/php-7.0.7-execdir.patch
[root@host php-7.0.7]$ patch -p1 < ./php-7.0.7-execdir.patch
# configure 파일을 재생성
[root@host php-7.0.7]$ ./buildconf --force
```

패치를 적용한 후에, ***configure*** 시에 ***--with-exec-dir*** 옵션으로 jail을 할 기본 디렉토리를 지정 합니다.

```shell
[root@host php-7.0.7]$ ./configure --prefix=/opt/php-7.0.7 \
                                   --with-exec-dir=/var/lib/php/bin \
                                   ... (and with other options)
[root@host php-7.0.7]$ make && make install
```

## 3. 설정

### 3.1. PHP 5.3 이하

이 패치는 ***Safe Mode*** 의 ***safe_mode_exec_dir*** 기능을 ***Safe Mode*** 가 아닌 환경에서 사용하기 위한 목적을 가졌습니다. 그러므로 PHP 5.3 이하 버전에서는 ***Safe Mode*** 기능이 활성화 되어 있으면, 이 기능은 동작하지 않습니다.

PHP 5.3 이하 버전에서는 system 함수들을 jail 하기 위하여 ***safe_mode_exec_dir*** ini 옵션을 사용합니다.

```ini
; When safe_mode is on, only executables located in the safe_mode_exec_dir
; will be allowed to be executed via the exec family of functions.
; http://php.net/safe-mode-exec-dir
safe_mode_exec_dir = /var/lib/php/bin
```

아래와 같이 ***safe_mode_exec_dir*** 옵션이 설정이 되어 있지 않을 경우에는, ***configure*** 시에 지정한 ***--with-exec-dir*** 값이 사용이 됩니다. ***configure*** 시에 ***--with-exec-dir*** 옵션을 주지 않았다면, 이 경우 ***safe_mode_exec_dir*** 의 값은 빈 값이 됩니다.

```ini
; When safe_mode is on, only executables located in the safe_mode_exec_dir
; will be allowed to be executed via the exec family of functions.
; http://php.net/safe-mode-exec-dir
;safe_mode_exec_dir =
```

만약, jail을 하고 싶지 않다면 다음과 같이 빈 값을 지정해 놓아야 합니다.

```ini
; When safe_mode is on, only executables located in the safe_mode_exec_dir
; will be allowed to be executed via the exec family of functions.
; http://php.net/safe-mode-exec-dir
safe_mode_exec_dir =
```

### 3.2. PHP 5.4 이상

PHP 5.4 부터는 ***Safe Mode*** 기능이 없어졌기 때문에, ***safe_mode_exec_dir*** 옵션 대신 ***exec_dir*** 옵션을 사용 합니다.

```ini
; only executables located in the exec_dir will be allowed to be executed
; via the exec family of functions. This is only AnNyung LInux patch
; see also https://github.com/OOPS-ORG-PHP/mod_execdir/
exec_dir = /var/lib/php/bin
```

아래와 같이 ***exec_dir*** 옵션이 설정이 되어 있지 않을 경우에는, ***configure*** 시에 지정한 ***--with-exec-dir*** 값이 사용이 됩니다. ***configure*** 시에 ***--with-exec-dir*** 옵션을 주지 않았다면, 이 경우 ***exec_dir*** 의 값은 빈 값이 됩니다.

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

### 3.3 Apache 가상 호스트별 설정

***PHP*** 를 ***apache module*** 로 사용할 경우 ***php_admin_value*** 지시자를 이용하여 가상 호스트마다 설정을 다르게 할 수 있습니다.
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

***exec_dir*** 옵션은 ***PHP_INI_SYSTEM*** 으로 할당이 되어 있기 때문에, ***.htaccess*** 에서는 사용이 불가능 합니다.


### 3.4 FPM pool 별 설정

***PHP*** 를 ***fpm*** 모드로 사용할 경우에는, FPM pool 별로 설정이 가능 합니다.

```ini
[www]
php_admin_flag[exec_dir] = /var/php/pool/www/bin

[www1]
php_admin_flag[exec_dir] = /var/php/pool/www1/bin
```

## 4. 동작

[***mod_execdir***](https://github.com/OOPS-ORG-PHP/mod_execdir/blob/master/README.ko.md) 확장 문서를 참조 하십시오.

