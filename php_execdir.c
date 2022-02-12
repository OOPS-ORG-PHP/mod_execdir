/*
  +----------------------------------------------------------------------+
  | mod_execdir v1                                                       |
  +----------------------------------------------------------------------+
  | Copyright 2022. JoungKyun.Kim All rights reserved.                   |
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
#include "ext/standard/info.h"

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
static const struct execdir_overload_func execdir_list[] = {
	{ "exec",            "exec_re",            "exec_orig"            },
	{ "system",          "system_re",          "system_orig"          },
	{ "passthru",        "passthru_re",        "passthru_orig"        },
	{ "pcntl_exec",      "pcntl_exec_re",      "pcntl_exec_orig"      },
	{ "shell_exec",      "shell_exec_re",      "shell_exec_orig"      },
	{ "popen",           "popen_re",           "popen_orig"           },
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	{ "proc_open",       "proc_open_re",       "proc_open_orig"       },
	{ "proc_close",      "proc_close_re",      "proc_close_orig"      },
	{ "proc_terminate",  "proc_terminate_re",  "proc_terminate_orig"  },
	{ "proc_get_status", "proc_get_status_re", "proc_get_status_orig" },
#endif
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ execdir_functions[]
 */
#include "execdir_arginfo.h"

#if PHP_VERSION_ID >= 50300
const
#endif
zend_function_entry execdir_functions[] = {
	PHP_FE (exec_re,            arginfo_exec_re)
	PHP_FE (system_re,          arginfo_system_re)
	PHP_FE (passthru_re,        arginfo_passthru_re)
	PHP_FE (shell_exec_re,      arginfo_shell_exec_re)
	PHP_FE (popen_re,           arginfo_popen_re)
	PHP_FE (pcntl_exec_re,      arginfo_pcntl_exec_re)
	PHP_FE (jailed_shellcmd,    arginfo_jailed_shellcmd_re)

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	PHP_FE (proc_open_re,       arginfo_proc_open)
	PHP_FE (proc_close_re,      arginfo_proc_close)
	PHP_FE (proc_terminate_re,  arginfo_proc_terminate)
	PHP_FE (proc_get_status_re, arginfo_proc_get_status)
#endif

#if PHP_VERSION_ID >= 60000
#ifdef PHP_EXECDIR_COMPAT
	PHP_FE (exec_orig,           arginfo_exec_re)
	PHP_FE (system_orig,         arginfo_system_re)
	PHP_FE (passthru_orig,       arginfo_passthru_re)
	PHP_FE (shell_exec_orig,     arginfo_shell_exec_re)
	PHP_FE (popen_orig,          arginfo_popen_re)
	PHP_FE (pcntl_exec_orig,     arginfo_pcntl_exec_re)

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	PHP_FE (proc_open_orig,       arginfo_proc_open)
	PHP_FE (proc_close_orig,      arginfo_proc_close)
	PHP_FE (proc_terminate_orig,  arginfo_proc_terminate)
	PHP_FE (proc_get_status_orig, arginfo_proc_get_status)
#endif
#endif
#endif
	{NULL, NULL, NULL}
};
/* }}} */

#if PHP_VERSION_ID < 60000
/* {{{ execdir_hook_functions[]
 */
#if PHP_VERSION_ID >= 50300
const
#endif
zend_function_entry execdir_hook_functions[] = {
	PHP_FALIAS (exec,            exec_re,            arginfo_exec_re)
	PHP_FALIAS (system,          system_re,          arginfo_system_re)
	PHP_FALIAS (passthru,        passthru_re,        arginfo_passthru_re)
	PHP_FALIAS (shell_exec,      shell_exec_re,      arginfo_shell_exec_re)
	PHP_FALIAS (popen,           popen_re,           arginfo_popen_re)
	PHP_FALIAS (pcntl_exec,      pcntl_exec_re,      arginfo_pcntl_exec_re)

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	PHP_FALIAS (proc_open,       proc_open_re,       arginfo_proc_open)
	PHP_FALIAS (proc_close,      proc_close_re,      arginfo_proc_close)
	PHP_FALIAS (proc_terminate,  proc_terminate_re,  arginfo_proc_terminate)
	PHP_FALIAS (proc_get_status, proc_get_status_re, arginfo_proc_get_status)
#endif
	{NULL, NULL, NULL}
};
/* }}} */
#endif

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
	STD_PHP_INI_ENTRY ("exec_dir", PHP_EXECDIR, PHP_INI_SYSTEM, OnUpdateString, exec_dir, zend_execdir_globals, execdir_globals)
PHP_INI_END ()
/* }}} */

#ifdef COMPILE_DL_EXECDIR
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
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
	const struct execdir_overload_func *p;
#if PHP_VERSION_ID >= 60000
	zend_function * func, * nfunc;
#ifdef PHP_EXECDIR_COMPAT
	zend_function * ofunc;
#endif
#endif

#if PHP_VERSION_ID < 80000
	TSRMLS_FETCH ();
#endif

	p = &(execdir_list[0]);
	while ( p->orig_func != NULL ) {
		if ( ! execdir_hash_exists (CG (function_table), p->orig_func) ) {
			p++;
			continue;
		}

#if PHP_VERSION_ID < 60000
  #ifdef PHP_EXECDIR_COMPAT
		/*
		 * Rename original function gets postfile '_orig'
		 */
		zend_hash_find (CG (function_table), p->orig_func, strlen(p->prog_func) + 1, (void **) &func);
		zend_hash_add  (
			CG (function_table), p->ovld_func, strlen(p->prog_func) + 1,
			func, sizeof (zend_function), NULL
		);
  #endif
		/*
		 * Remove original function
		 */
		zend_hash_del (CG (function_table), p->orig_func, strlen(p->orig_func)  + 1);
#else
		if ( (func = zend_hash_str_find_ptr (CG (function_table), EXEC_STRING (p->orig_func))) != NULL ) {
			nfunc = zend_hash_str_find_ptr (CG (function_table), EXEC_STRING (p->ovld_func));
  #ifdef PHP_EXECDIR_COMPAT
			ofunc = zend_hash_str_find_ptr (CG (function_table), EXEC_STRING (p->save_func));
			if ( ofunc->internal_function.handler != func->internal_function.handler )
				ofunc->internal_function.handler = func->internal_function.handler;
  #endif
			if ( func->internal_function.handler != nfunc->internal_function.handler )
				func->internal_function.handler = nfunc->internal_function.handler;
		}
#endif /* end of PHP_VERSION_ID < 60000 */
		p++;
	}


#if PHP_VERSION_ID < 60000
	zend_register_functions (NULL, execdir_hook_functions, NULL, MODULE_PERSISTENT TSRMLS_CC);
#endif
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
		REGISTER_INI_ENTRIES ();
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

	return SUCCESS;
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

	DISPLAY_INI_ENTRIES ();
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
	char        * command, * jcommand;
#if PHP_VERSION_ID < 60000
	size_t        total_readbytes;
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

#if PHP_VERSION_ID < 50400
	if ( PG (safe_mode) ) {
		php_error_docref (NULL TSRMLS_CC, E_WARNING, "Cannot execute using backquotes in Safe Mode");
		RETURN_FALSE;
	}
#endif

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
		char * z = memchr (posix_mode, 'b', mode_len);
		if ( z ) {
			memmove (z, z + 1, mode_len - (z - posix_mode));
		}
	}
#endif

#if PHP_VERSION_ID < 50400
	{
		char *b, *buf = 0, *tmp;

		if ( PG (safe_mode) ) {
			b = strchr (command, ' ');
			if ( ! b ) {
				b = strrchr (command, '/');
			} else {
				char * c;

				c = command;
				while ( (*b != '/') && (b != c) ) {
					b--;
				}
				if ( b == c ) {
					b = NULL;
				}
			}

			if ( b ) {
				spprintf (&buf, 0, "%s%s", PG (safe_mode_exec_dir), b);
			} else {
				spprintf (&buf, 0, "%s/%s", PG (safe_mode_exec_dir), command);
			}

			tmp = php_escape_shell_cmd (buf);
			fp = VCWD_POPEN (tmp, posix_mode);
			efree (tmp);

			if ( ! fp ) {
				php_error_docref2 (NULL TSRMLS_CC, buf, posix_mode, E_WARNING, "%s", strerror (errno));
				efree (posix_mode);
				efree (buf);
				RETURN_FALSE;
			}

			efree (buf);

		} else {
			jcommand = get_jailed_shell_cmd (command);
			fp = VCWD_POPEN (jcommand, posix_mode);
			if ( ! fp ) {
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
 * Executes specified program in current process space as defined by exec(2)
 */
PHP_FUNCTION (pcntl_exec_re)
{
#if PHP_VERSION_ID >= 60000
/* {{{ for PHP 7 */
	zval        * args = NULL, * envs = NULL;
	zval        * element;
	HashTable   * args_hash, *envs_hash;
	int           argc = 0, argi = 0;
	int           envc = 0, envi = 0;
	char       ** argv = NULL, ** envp = NULL;
	char       ** current_arg, ** pair;
	size_t        pair_length;
	zend_string * key;
	char        * path, * jpath;
	size_t        path_len;
	zend_ulong    key_num;

	if ( strcmp (sapi_module.name, "cli") != 0 &&
		 strncmp (sapi_module.name, "cgi", 3) != 0 &&
		 strncmp (sapi_module.name, "fpm", 3) != 0 ) {
		php_error_docref (NULL, E_WARNING, "pcntl_exec_re can only work on CLI/CGI SAPI.");
		RETURN_FALSE;
	}

	if ( zend_parse_parameters (ZEND_NUM_ARGS (), "p|aa", &path, &path_len, &args, &envs) == FAILURE ) {
#if PHP_VERSION_ID >= 80000
		RETURN_THROWS ();
#else
		return;
#endif
	}

	if ( ZEND_NUM_ARGS () > 1 ) {
		/* Build argument list */
#if PHP_VERSION_ID >= 80000
		SEPARATE_ARRAY(args);
#endif
		args_hash = Z_ARRVAL_P (args);
		argc = zend_hash_num_elements (args_hash);

		argv = safe_emalloc ((argc + 2), sizeof (char *), 0);
		*argv = path;
		current_arg = argv + 1;
		ZEND_HASH_FOREACH_VAL (args_hash, element) {
			if ( argi >= argc ) break;
#if PHP_VERSION_ID >= 80000
			if ( ! try_convert_to_string (element) ) {
				efree (argv);
				RETURN_THROWS ();
			}

#else
			convert_to_string_ex (element);
#endif
			*current_arg = Z_STRVAL_P (element);
			argi++;
			current_arg++;
		} ZEND_HASH_FOREACH_END ();
		*(current_arg) = NULL;
	} else {
		argv = emalloc (2 * sizeof (char *));
#if PHP_VERSION_ID >= 80000
		argv[0] = path;
		argv[1] = NULL;
#else
		*argv = path;
		*(argv + 1) = NULL;
#endif
	}

	jpath = get_jailed_shell_cmd(path);

	if ( ZEND_NUM_ARGS () == 3 ) {
		/* Build environment pair list */
		envs_hash = Z_ARRVAL_P (envs);
		envc = zend_hash_num_elements (envs_hash);

		pair = envp = safe_emalloc ((envc + 1), sizeof (char *), 0);
		ZEND_HASH_FOREACH_KEY_VAL (envs_hash, key_num, key, element) {
			if ( envi >= envc ) break;
			if ( ! key ) {
				key = zend_long_to_str (key_num);
			} else {
				zend_string_addref (key);
			}

#if PHP_VERSION_ID >= 80000
			if ( ! try_convert_to_string (element) ) {
				zend_string_release (key);
				efree (argv);
				efree (envp);
				RETURN_THROWS ();
			}
#else
			convert_to_string_ex (element);
#endif

			/* Length of element + equal sign + length of key + null */
			ZEND_ASSERT(Z_STRLEN_P(element) < SIZE_MAX && ZSTR_LEN(key) < SIZE_MAX);
			*pair = safe_emalloc(Z_STRLEN_P(element) + 1, sizeof(char), ZSTR_LEN(key) + 1);
			pair_length = Z_STRLEN_P (element) + ZSTR_LEN (key) + 2;
			strlcpy (*pair, ZSTR_VAL (key), ZSTR_LEN (key) + 1);
			strlcat (*pair, "=", pair_length);
			strlcat (*pair, Z_STRVAL_P (element), pair_length);

			/* Cleanup */
			execdir_string_release (key);
			envi++;
			pair++;
		} ZEND_HASH_FOREACH_END ();
		*(pair) = NULL;

		if ( execve (jpath, argv, envp) == -1 ) {
			//PCNTL_G (last_error) = errno;
			php_error_docref (NULL, E_WARNING, "Error has occurred: (errno %d) %s", errno, strerror (errno));
		}

		/* Cleanup */
		for ( pair = envp; *pair != NULL; pair++ ) efree (*pair);
		efree (envp);
	} else {

		if ( execv (jpath, argv) == -1 ) {
			//PCNTL_G (last_error) = errno;
			php_error_docref (NULL, E_WARNING, "Error has occurred: (errno %d) %s", errno, strerror (errno));
		}
	}

/* }}} */
#else
/* {{{ before PHP 7 */
	zval       * args = NULL, * envs = NULL;
	zval      ** element;
	HashTable  * args_hash, * envs_hash;
	int          argc = 0, argi = 0;
	int          envc = 0, envi = 0;
	int          return_val = 0;
	char      ** argv = NULL, ** envp = NULL;
	char      ** current_arg, ** pair;
	size_t       pair_length;
	char       * key;
	uint         key_length;
	char       * path, * jpath = NULL;
	int          path_len;
	ulong        key_num;

#if PHP_VERSION_ID < 50400
	if ( zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "s|aa", &path, &path_len, &args, &envs) == FAILURE )
#else
	if ( zend_parse_parameters (ZEND_NUM_ARGS () TSRMLS_CC, "p|aa", &path, &path_len, &args, &envs) == FAILURE )
#endif
	{
		return;
	}

#if PHP_VERSION_ID < 50400
	if ( strlen (path) != path_len ) {
		php_error_docref (NULL TSRMLS_CC, E_WARNING, "NULL byte detected. Possible attack");
		RETURN_FALSE;
	}
#endif

	if ( ZEND_NUM_ARGS () > 1 ) {
		/* Build argument list */
		args_hash = HASH_OF (args);
		argc = zend_hash_num_elements (args_hash);

		argv = safe_emalloc ((argc + 2), sizeof (char *), 0);
		*argv = path;
		for ( zend_hash_internal_pointer_reset (args_hash), current_arg = argv + 1;
				(argi < argc && (zend_hash_get_current_data (args_hash, (void **) &element) == SUCCESS));
				(argi++, current_arg++, zend_hash_move_forward (args_hash)) ) {

			convert_to_string_ex (element);
			*current_arg = Z_STRVAL_PP (element);
		}
		*(current_arg) = NULL;
	} else {
		argv = emalloc (2 * sizeof (char *));
		*argv = path;
		*(argv+1) = NULL;
	}

	jpath = get_jailed_shell_cmd (path);

	if ( ZEND_NUM_ARGS () == 3 ) {
		/* Build environment pair list */
		envs_hash = HASH_OF (envs);
		envc = zend_hash_num_elements (envs_hash);

		envp = safe_emalloc ((envc + 1), sizeof (char *), 0);
		for ( zend_hash_internal_pointer_reset (envs_hash), pair = envp;
				(envi < envc && (zend_hash_get_current_data (envs_hash, (void **) &element) == SUCCESS));
				(envi++, pair++, zend_hash_move_forward (envs_hash)) ) {
			switch (return_val = zend_hash_get_current_key_ex (envs_hash, &key, &key_length, &key_num, 0, NULL)) {
				case HASH_KEY_IS_LONG:
					key = emalloc (101);
					snprintf (key, 100, "%ld", key_num);
					key_length = strlen (key);
					break;
#if PHP_VERSION_ID < 50500
				case HASH_KEY_NON_EXISTANT:
#else
				case HASH_KEY_NON_EXISTENT:
#endif
					pair--;
					continue;
			}

			convert_to_string_ex (element);

			/* Length of element + equal sign + length of key + null */
			ZEND_ASSERT(Z_STRLEN_PP(element) < SIZE_MAX &&  key_length < SIZE_MAX);
			if ( Z_STRLEN_PP(element) < SIZE_MAX &&  key_length < SIZE_MAX ) {
#if PHP_VERSION_ID < 50200
				php_error_docref (NULL TSRMLS_CC, E_ERROR, "pcntl_exec(): The element length or key length is greater than %ld.", SIZE_MAX);
#else
				php_error_docref (NULL TSRMLS_CC, E_ERROR, "The element length or key length is greater than %ld.", SIZE_MAX);
#endif
			}
			pair_length = Z_STRLEN_PP (element) + key_length + 2;
			*pair = emalloc (pair_length);
			strlcpy (*pair, key, key_length);
			strlcat (*pair, "=", pair_length);
			strlcat (*pair, Z_STRVAL_PP (element), pair_length);

			/* Cleanup */
			if ( return_val == HASH_KEY_IS_LONG ) efree (key);
		}
		*(pair) = NULL;

		if ( execve (jpath, argv, envp) == -1 ) {
			//PCNTL_G (last_error) = errno;
#if PHP_VERSION_ID < 50200
			php_error_docref (NULL TSRMLS_CC, E_WARNING, "pcntl_exec(): Error has occurred: (errno %d) %s", errno, strerror (errno));
#else
			php_error_docref (NULL TSRMLS_CC, E_WARNING, "Error has occurred: (errno %d) %s", errno, strerror (errno));
#endif
		}

		/* Cleanup */
		for ( pair = envp; *pair != NULL; pair++ ) efree (*pair);
		efree (envp);
	} else {

		if ( execv (jpath, argv) == -1 ) {
			//PCNTL_G(last_error) = errno;
#if PHP_VERSION_ID < 50200
			php_error_docref (NULL TSRMLS_CC, E_WARNING, "pcntl_exec(): Error has occurred: (errno %d) %s", errno, strerror (errno));
#else
			php_error_docref (NULL TSRMLS_CC, E_WARNING, "Error has occurred: (errno %d) %s", errno, strerror (errno));
#endif
		}
	}
/* }}} */
#endif

	efree (jpath);
	efree (argv);

	RETURN_FALSE;
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

/* PHP_EXECDIR_COMPAT functions (*_orig)
 * This functions are only used PHP 7.4 and after
 */
#if PHP_VERSION_ID >= 60000
#ifdef PHP_EXECDIR_COMPAT
PHP_FUNCTION (exec_orig) { RETURN_NULL (); }
PHP_FUNCTION (system_orig) { RETURN_NULL (); }
PHP_FUNCTION (passthru_orig) { RETURN_NULL (); }
PHP_FUNCTION (shell_exec_orig) { RETURN_NULL (); }
PHP_FUNCTION (popen_orig) { RETURN_NULL (); }
PHP_FUNCTION (pcntl_exec_orig) { RETURN_NULL (); }

#ifdef PHP_CAN_SUPPORT_PROC_OPEN
PHP_FUNCTION (proc_open_orig) { RETURN_NULL (); }
PHP_FUNCTION (proc_get_status_orig) { RETURN_NULL (); }
PHP_FUNCTION (proc_close_orig) { RETURN_NULL (); }
PHP_FUNCTION (proc_terminate_orig) { RETURN_NULL (); }
#endif
#endif
#endif
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
