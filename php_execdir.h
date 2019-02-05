/*
  +----------------------------------------------------------------------+
  | mod_exec v1                                                          |
  +----------------------------------------------------------------------+
  | Copyright (c) 1999-2018 JoungKyun.Kim                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: JoungKyun.Kim <http://oops.org>                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_EXECDIR_H
#define PHP_EXECDIR_H

#ifndef PHP_EXECDIR
	#define PHP_EXECDIR ""
#endif

#define EXECDIR_EXT_VERSION "1.0.4"

extern zend_module_entry execdir_module_entry;
#define execdir_module_ptr &execdir_module_entry
#define phpext_execdir_ptr execdir_module_ptr

#ifdef PHP_WIN32
	#define PHP_EXECDIR_API __declspec(dllexport)
#else
	#define PHP_EXECDIR_API
#endif

#ifdef ZTS
	#include "TSRM.h"
	#define EXECDIR_G(v) TSRMG(execdir_globals_id, zend_execdir_globals *, v)
#else
	#define EXECDIR_G(v) (execdir_globals.v)
#endif

ZEND_BEGIN_MODULE_GLOBALS(execdir)
	char * exec_dir;
ZEND_END_MODULE_GLOBALS(execdir)

PHP_MINIT_FUNCTION (execdir);
PHP_MSHUTDOWN_FUNCTION (execdir);
PHP_RINIT_FUNCTION (execdir);
PHP_RSHUTDOWN_FUNCTION (execdir);
PHP_MINFO_FUNCTION (execdir);

PHP_FUNCTION (exec_re);
PHP_FUNCTION (system_re);
PHP_FUNCTION (passthru_re);
PHP_FUNCTION (shell_exec_re);
PHP_FUNCTION (popen_re);
PHP_FUNCTION (pcntl_exec_re);
PHP_FUNCTION (jailed_shellcmd);

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
PHP_MINIT_FUNCTION (proc_open_re);
PHP_FUNCTION (proc_open_re);
PHP_FUNCTION (proc_get_status_re);
PHP_FUNCTION (proc_close_re);
PHP_FUNCTION (proc_terminate_re);
#endif

#if PHP_VERSION_ID < 60000
	#define RETURN_EXECDIR_STRING(s,i) RETURN_STRING(s,i)
	#define RETVAL_EXECDIR_STRING(s,i) RETVAL_STRINGL(s,i,1)
	#define execdir_hash_exists(ht,key) zend_hash_exists(ht,key,strlen(key)+1)
	#define zend_ini_entry_def zend_ini_entry
	#define zend_hash_str_add_empty_element zend_hash_add_empty_element
#else
	#define RETURN_EXECDIR_STRING(s,i) RETURN_STRING(s)
	#define RETVAL_EXECDIR_STRING(s,i) RETVAL_STRINGL(s,i)
	#define execdir_hash_exists(ht,key) zend_hash_str_exists(ht,key,strlen(key))
#endif

#if PHP_VERSION_D < 70300
	#define execdir_string_release(x) zend_string_release(x)
#else
	#define execdir_string_release(x) zend_string_release_ex(x,0)
#endif

#endif /* PHP_EXECDIR_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
