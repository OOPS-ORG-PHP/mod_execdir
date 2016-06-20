/*
  +----------------------------------------------------------------------+
  | mod_execdir v1                                                       |
  +----------------------------------------------------------------------+
  | Copyright (c) 1999-2016 JoungKyun.Kim                                |
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

  $Id: $
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"
#include "ext/standard/exec.h"

#include "php_execdir.h"

char * get_jailed_shell_cmd (char *);

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
static HashTable execdir_submodules;
#endif

// If you declare any globals in php_exdcdir.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS (execdir)

/* {{{ execdir_list[]
 *
 * system function list to apply exec_dir
 */
char * execdir_list[] = {
	"exec",
	"system",
	"passthru",
	"pcntl_exec",
	"shell_exec",
	"popen",
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	"proc_open",
	"proc_close",
	"proc_terminate",
	"proc_get_status",
#endif
};
/* }}} */

/* {{{ execdir_functions[]
 */

/* {{{ php_exec.c */
ZEND_BEGIN_ARG_INFO_EX (arginfo_exec_re, 0, 0, 1)
    ZEND_ARG_INFO(0, command)
    ZEND_ARG_INFO(1, output) /* ARRAY_INFO(1, output, 1) */
    ZEND_ARG_INFO(1, return_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX (arginfo_system_re, 0, 0, 1)
    ZEND_ARG_INFO(0, command)
    ZEND_ARG_INFO(1, return_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX (arginfo_passthru_re, 0, 0, 1)
    ZEND_ARG_INFO(0, command)
    ZEND_ARG_INFO(1, return_value)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ proc_open.c */
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
ZEND_BEGIN_ARG_INFO_EX(arginfo_proc_terminate, 0, 0, 1)
	ZEND_ARG_INFO(0, process)
	ZEND_ARG_INFO(0, signal)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_proc_close, 0)
	ZEND_ARG_INFO(0, process)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_proc_get_status, 0)
	ZEND_ARG_INFO(0, process)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_proc_open, 0, 0, 3)
	ZEND_ARG_INFO(0, command)
	ZEND_ARG_INFO(0, descriptorspec) /* ARRAY_INFO(0, descriptorspec, 1) */
	ZEND_ARG_INFO(1, pipes) /* ARRAY_INFO(1, pipes, 1) */
	ZEND_ARG_INFO(0, cwd)
	ZEND_ARG_INFO(0, env) /* ARRAY_INFO(0, env, 1) */
	ZEND_ARG_INFO(0, other_options) /* ARRAY_INFO(0, other_options, 1) */
ZEND_END_ARG_INFO()
#endif
/* }}} */

const zend_function_entry execdir_functions[] = {
	PHP_FE (exec_re,            arginfo_exec_re)
	PHP_FE (system_re,          arginfo_system_re)
	PHP_FE (passthru_re,        arginfo_passthru_re)
	PHP_FE (shell_exec_re,      NULL)
	PHP_FE (popen_re,           NULL)
	PHP_FE (jailed_shellcmd,    NULL)

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	PHP_FE (proc_open_re,       arginfo_proc_open)
	PHP_FE (proc_close_re,      arginfo_proc_close)
	PHP_FE (proc_terminate_re,  arginfo_proc_terminate)
	PHP_FE (proc_get_status_re, arginfo_proc_get_status)
#endif
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ execdir_hook_functions[]
 */
const zend_function_entry execdir_hook_functions[] = {
	PHP_FALIAS (exec,            exec_re,            arginfo_exec_re)
	PHP_FALIAS (system,          system_re,          arginfo_system_re)
	PHP_FALIAS (passthru,        passthru_re,        arginfo_passthru_re)
	PHP_FALIAS (shell_exec,      shell_exec_re,      NULL)
	PHP_FALIAS (popen,           popen_re,           NULL)

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	PHP_FALIAS (proc_open,       proc_open_re,       arginfo_proc_open)
	PHP_FALIAS (proc_close,      proc_close_re,      arginfo_proc_close)
	PHP_FALIAS (proc_terminate,  proc_terminate_re,  arginfo_proc_terminate)
	PHP_FALIAS (proc_get_status, proc_get_status_re, arginfo_proc_get_status)
#endif
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ execdir_module_entry
 */
zend_module_entry execdir_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"execdir",
	execdir_functions,
	PHP_MINIT (execdir),
	PHP_MSHUTDOWN (execdir),
	PHP_RINIT (execdir),
	PHP_RSHUTDOWN (execdir),
	PHP_MINFO (execdir),
#if ZEND_MODULE_API_NO >= 20010901
	EXECDIR_EXT_VERSION, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ ini_entries[]
 */
PHP_INI_BEGIN ()
	STD_PHP_INI_ENTRY ("exec_dir", PHP_EXECDIR, ZEND_INI_SYSTEM, OnUpdateString, exec_dir, zend_execdir_globals, execdir_globals)
PHP_INI_END ()
/* }}} */

#ifdef COMPILE_DL_EXECDIR
ZEND_GET_MODULE (execdir)
#endif

/* {{{ +-- void php_execdir_init_globals (zend_execdir_globals * execdir_globals)
 */
void php_execdir_init_globals (zend_execdir_globals * execdir_globals) {
	memset (execdir_globals, 0, sizeof (zend_execdir_globals));
}
/* }}} */

/*{{{ +-- static int safe_hook_execdir (void)
 */
static int safe_hook_execdir (void) {
	int    entno = sizeof (execdir_list) / sizeof (char *);
	int    i, funclen;
	char * func;
	zend_function * zf;
	TSRMLS_FETCH ();

	for ( i=0; i<entno; i++ ) {
		func = execdir_list[i];
		funclen = strlen (func);

		if ( ! execdir_hash_exists (CG (function_table), func) )
			continue;

#if 0
		/*
		 * Remove original function
		 */
#if PHP_VERSION_ID < 60000
		zend_hash_del (CG (function_table), func, funclen + 1);
#else
		zend_hash_str_del (CG (function_table), func, funclen);
#endif
#else
		/*
		 * Rename original function gets postfie '_orig'
		 */
		{
			char            func_re[32] = { 0 };
			int             func_relen;
			zend_function * zf;

			sprintf (func_re, "%s_orig", func);
			func_relen = funclen + 5;

			if ( execdir_hash_exists (CG (function_table), func_re) )
				continue;

#if PHP_VERSION_ID < 60000
			zend_hash_find (CG (function_table), func, funclen + 1, (void **) &zf);
			zend_hash_add (CG (function_table), func_re, func_relen + 1, zf, sizeof (zend_function), NULL);
			zend_hash_del (CG (function_table), func, funclen + 1);
#else
			zf = zend_hash_str_find_ptr (CG (function_table), func, funclen);
			zend_hash_str_add_mem (CG (function_table), func_re, func_relen, zf, sizeof (zend_function));
			zend_hash_str_del (CG (function_table), func, funclen);
#endif
		}
#endif
	}

	zend_register_functions (NULL, execdir_hook_functions, NULL, MODULE_PERSISTENT TSRMLS_CC);
	return 0;
}
/* }}} */

/* {{{ +-- static void php_exec_ex (INTERNAL_FUNCTION_PARAMETERS, int mode)
 */
static void php_exec_ex (INTERNAL_FUNCTION_PARAMETERS, int mode) {
	char   * cmd, * jcmd;
#if PHP_VERSION_ID < 60000
	int      cmd_len;
#else
	size_t   cmd_len;
#endif
	zval   * ret_code = NULL, * ret_array = NULL;
	int      ret;

	if ( mode ) {
		if ( zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "s|z/", &cmd, &cmd_len, &ret_code) == FAILURE ) {
			RETURN_FALSE;
		}
	} else {
		if ( zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "s|z/z/", &cmd, &cmd_len, &ret_array, &ret_code) == FAILURE ) {
			RETURN_FALSE;
		}
	}
	if ( ! cmd_len ) {
		php_error_docref (NULL TSRMLS_CC, E_WARNING, "Cannot execute a blank command");
		RETURN_FALSE;
	}
	if ( strlen (cmd) != cmd_len ) {
		php_error_docref (NULL TSRMLS_CC, E_WARNING, "NULL byte detected. Possible attack");
		RETURN_FALSE;
	}

	jcmd = get_jailed_shell_cmd (cmd);

	if ( ! ret_array ) {
		ret = php_exec (mode, jcmd, NULL, return_value TSRMLS_CC);
	} else {
		if ( Z_TYPE_P (ret_array) != IS_ARRAY ) {
			zval_dtor (ret_array);
			array_init (ret_array);
		}
		ret = php_exec (2, jcmd, ret_array, return_value TSRMLS_CC);
	}
	efree (jcmd);

	if ( ret_code ) {
		zval_dtor (ret_code);
		ZVAL_LONG (ret_code, ret);
	}
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION (execdir)
{
	ZEND_INIT_MODULE_GLOBALS (execdir, php_execdir_init_globals, NULL);

	// get suhosin (https://suhosin.org/)
	if ( execdir_hash_exists (EG (ini_directives), "exec_dir") ) {
		/* update */
		 zend_ini_entry_def * p = (zend_ini_entry_def *) &ini_entries;

		while ( p->name ) {
			zend_ini_entry * e;

#if PHP_VERSION_ID < 60000
			if ( zend_hash_find (EG (ini_directives), p->name, p->name_length, (void **) &e) == FAILURE )
#else
			if ( (e = zend_hash_str_find_ptr (EG (ini_directives), p->name, p->name_length)) == NULL )
#endif
			{
				zend_register_ini_entries (p, module_number TSRMLS_CC);
				break;
			}

			e->modifiable = p->modifiable;
			e->module_number = module_number;
			e->on_modify = p->on_modify;
			e->mh_arg1 = p->mh_arg1;
			e->mh_arg2 = p->mh_arg2;
			e->mh_arg3 = p->mh_arg3;
#if PHP_VERSION_ID < 60000
			e->on_modify (e, e->value, e->value_length, e->mh_arg1, e->mh_arg2, e->mh_arg3, ZEND_INI_STAGE_STARTUP TSRMLS_CC);
#else
			e->on_modify (e, e->value, e->mh_arg1, e->mh_arg2, e->mh_arg3, ZEND_INI_STAGE_STARTUP);
#endif
			p++;
		}
	} else {
		/* not registered yet, then simply use the API */
		zend_register_ini_entries (
			(zend_ini_entry_def *) &ini_entries,
			module_number TSRMLS_CC
		);
	}

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	zend_hash_init (&execdir_submodules, 0, NULL, NULL, 1);

	if ( PHP_MINIT(proc_open_re)(INIT_FUNC_ARGS_PASSTHRU) == SUCCESS) {
		zend_hash_str_add_empty_element (&execdir_submodules, "proc_open_re", strlen ("proc_open_re"));
	}
#endif

	// On cli mode, jailed_shell_cmd does nothing to do.
	//if ( strcmp (sapi_module.name, "cli") != 0 )
	safe_hook_execdir ();
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION (execdir)
{
	UNREGISTER_INI_ENTRIES ();
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	zend_hash_destroy (&execdir_submodules);
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION (execdir)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION (execdir)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION (execdir)
{
	php_info_print_table_start ();
	php_info_print_table_colspan_header (2, "execdir extension support");
	php_info_print_table_row (2, "Summary", "jailed system function");
	php_info_print_table_row (2, "URL", "http://github.com/OOPS-ORG-PHP/mod_execdir");
	php_info_print_table_row (2, "Build version", EXECDIR_EXT_VERSION);
	php_info_print_table_row (2, "Default Exec DIR", PHP_EXECDIR);
	php_info_print_table_end ();
}
/* }}} */

/* {{{ PHP_FUNCTION (string) exec_re (string command [, array &output [, int &return_var]])
 */
PHP_FUNCTION (exec_re)
{
	php_exec_ex (INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ PHP_FUNCTION (string) system_re (string command [, int &return_var])
 */
PHP_FUNCTION (system_re)
{
	php_exec_ex (INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ PHP_FUNCTION (void) passthru_re (string command [, int &return_var])
 */
PHP_FUNCTION (passthru_re)
{
	php_exec_ex (INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ PHP_FUNCTION (string) shell_exec_re (string command)
 * Execute command via shell and return complete output as string
 * from php 5.6.22 && php 7.0.7
 */
PHP_FUNCTION (shell_exec_re)
{
	FILE        * in;
	size_t        total_readbytes;
	char        * command, * jcommand;
#if PHP_VERSION_ID < 60000
	int           command_len;
	char        * ret;
#else
	size_t        command_len;
	zend_string * ret;
#endif
	php_stream  * stream;

	if ( zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "s", &command, &command_len) == FAILURE ) {
		return;
	}

	if ( ! command_len ) {
		php_error_docref (NULL TSRMLS_CC, E_WARNING, "Cannot execute a blank command");
		RETURN_FALSE;
	}
	if ( strlen (command) != command_len ) {
		php_error_docref (NULL TSRMLS_CC, E_WARNING, "NULL byte detected. Possible attack");
		RETURN_FALSE;
	}

	jcommand = get_jailed_shell_cmd (command);

#ifdef PHP_WIN32
	if ( (in = VCWD_POPEN (jcommand, "rt")) == NULL ) {
#else
	if ( (in = VCWD_POPEN (jcommand, "r")) == NULL ) {
#endif
		php_error_docref (NULL TSRMLS_CC, E_WARNING, "Unable to execute '%s'", jcommand);
		efree (jcommand);
		RETURN_FALSE;
	}
	efree (jcommand);

	stream = php_stream_fopen_from_pipe (in, "rb");
#if PHP_VERSION_ID < 60000
	total_readbytes = php_stream_copy_to_mem (stream, &ret, PHP_STREAM_COPY_ALL, 0);
#else
	ret = php_stream_copy_to_mem (stream, PHP_STREAM_COPY_ALL, 0);
#endif
	php_stream_close (stream);

#if PHP_VERSION_ID < 60000
	if ( total_readbytes > 0 ) {
		RETVAL_STRINGL (ret, (int) total_readbytes, 0);
	}
#else
	if ( ret && ZSTR_LEN (ret) > 0 ) {
		RETVAL_STR (ret);
	}
#endif
}
/* }}} */

/* {{{ PHP_FUNCTION (resource) popen (string command, string mode)
 * Execute a command and open either a read or a write pipe to it 
 * From php 5.6.22 && php 7.0.7
 */
PHP_FUNCTION (popen_re)
{
	char       * command, * mode, * jcommand;
#if PHP_VERSION_ID < 60000
	int          command_len, mode_len;
#else
	size_t       command_len, mode_len;
#endif
	FILE       * fp;
	php_stream * stream;
	char       * posix_mode;

#if PHP_VERSION_ID < 50400
	if ( zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "ss", &command, &command_len, &mode, &mode_len) == FAILURE ) {
#else
	if ( zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "ps", &command, &command_len, &mode, &mode_len) == FAILURE ) {
#endif
		return;
	}

	if ( ! command_len ) {
		php_error_docref (NULL TSRMLS_CC, E_WARNING, "Cannot execute a blank command");
		RETURN_FALSE;
	}
	if ( strlen (command) != command_len ) {
		php_error_docref (NULL TSRMLS_CC, E_WARNING, "NULL byte detected. Possible attack");
		RETURN_FALSE;
	}

	posix_mode = estrndup (mode, mode_len);
#ifndef PHP_WIN32
	{
		char *z = memchr (posix_mode, 'b', mode_len);
		if (z) {
			memmove (z, z + 1, mode_len - (z - posix_mode));
		}
	}
#endif

#if PHP_VERSION_ID < 50400
	{
		char *b, *buf = 0, *tmp;

		if (PG (safe_mode)){
			b = strchr (command, ' ');
			if (!b) {
				b = strrchr (command, '/');
			} else {
				char *c;

				c = command;
				while ((*b != '/') && (b != c)) {
					b--;
				}
				if (b == c) {
					b = NULL;
				}
			}

			if (b) {
				spprintf (&buf, 0, "%s%s", PG (safe_mode_exec_dir), b);
			} else {
				spprintf (&buf, 0, "%s/%s", PG (safe_mode_exec_dir), command);
			}

			tmp = php_escape_shell_cmd (buf);
			fp = VCWD_POPEN (tmp, posix_mode);
			efree (tmp);

			if (!fp) {
				php_error_docref2 (NULL TSRMLS_CC, buf, posix_mode, E_WARNING, "%s", strerror (errno));
				efree (posix_mode);
				efree (buf);
				RETURN_FALSE;
			}

			efree (buf);

		} else {
			jcommand = get_jailed_shell_cmd (command);
			fp = VCWD_POPEN (jcommand, posix_mode);
			if (!fp) {
				php_error_docref2 (NULL TSRMLS_CC, command, posix_mode, E_WARNING, "%s", strerror (errno));
				efree (posix_mode);
				RETURN_FALSE;
			}
		}
	}
#else
	jcommand = get_jailed_shell_cmd (command);
	fp = VCWD_POPEN (jcommand, posix_mode);
	efree (jcommand);

	if ( ! fp ) {
		php_error_docref2 (NULL TSRMLS_CC, command, posix_mode, E_WARNING, "%s", strerror (errno));
		efree (posix_mode);
		RETURN_FALSE;
	}
#endif

	stream = php_stream_fopen_from_pipe (fp, mode);

	if ( stream == NULL ) {
		php_error_docref2 (NULL TSRMLS_CC, command, mode, E_WARNING, "%s", strerror (errno));
		RETVAL_FALSE;
	} else {
		php_stream_to_zval (stream, return_value);
	}

	efree (posix_mode);
}
/* }}} */

/* {{{ PHP_FUNCTION (bool) pcntl_exec_re (string $path [, array args [, array envs]])
 */
PHP_FUNCTION (pcntl_exec_re)
{
	RETURN_EXECDIR_STRING ("pcntl_exec", 1);
}
/* }}} */

/* {{{ PHP_FUNCTION (string) jailed_shellcmd (string $path)
 */
PHP_FUNCTION (jailed_shellcmd)
{
	char        * cmd;
	char        * jcmd;
	size_t        cmdlen = 0;

	if ( zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "s", &cmd, &cmdlen) == FAILURE ) {
		return;
	}

	if ( ! cmdlen )
		RETURN_EMPTY_STRING ();

	if ( strlen (cmd) != cmdlen ) {
		php_error_docref (NULL TSRMLS_CC, E_WARNING, "NULL byte detected. Possible attack");
		RETURN_EMPTY_STRING ();
	}

	jcmd = get_jailed_shell_cmd (cmd);

	RETVAL_EXECDIR_STRING (jcmd, strlen (jcmd));

	efree (jcmd);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
