/*
 * defined mod_execdir ARG_INFO
 */

/* {{{ php_exec.c */

/* arginfo_exec_re {{{
 */
#if PHP_VERSION_ID >= 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX (arginfo_exec_re, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
    ZEND_ARG_TYPE_INFO (0, command, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE (1, output, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE (1, result_code, IS_LONG, 1, "null")
#elif PHP_VERSION_ID >= 70200
ZEND_BEGIN_ARG_INFO_EX (arginfo_exec_re, 0, 0, 1)
    ZEND_ARG_INFO (0, command)
    ZEND_ARG_TYPE_INFO (1, output, IS_ARRAY, 1)
    ZEND_ARG_TYPE_INFO (1, result_code, IS_LONG, 1)
#else
ZEND_BEGIN_ARG_INFO_EX (arginfo_exec_re, 0, 0, 1)
    ZEND_ARG_INFO (0, command)
    ZEND_ARG_INFO (1, output) /* ARRAY_INFO(1, output, 1) */
    ZEND_ARG_INFO (1, return_value)
#endif
ZEND_END_ARG_INFO ()
/* }}} */

/* arginfo_system_re {{{
 */
#if PHP_VERSION_ID >= 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX (arginfo_system_re, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO (0, command, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE (1, result_code, IS_LONG, 1, "null")
#elif PHP_VERSION_ID >= 70200
ZEND_BEGIN_ARG_INFO_EX (arginfo_system_re, 0, 0, 1)
    ZEND_ARG_INFO (0, command)
    ZEND_ARG_TYPE_INFO (1, result_code, IS_LONG, 1)
#else
ZEND_BEGIN_ARG_INFO_EX (arginfo_system_re, 0, 0, 1)
    ZEND_ARG_INFO (0, command)
    ZEND_ARG_INFO (1, return_value)
#endif
ZEND_END_ARG_INFO ()
/* }}} */

/* arginfo_passthru_re {{{
 */
#if PHP_VERSION_ID >= 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX (arginfo_passthru_re, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO (0, command, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE (1, result_code, IS_LONG, 1, "null")
#elif PHP_VERSION_ID >= 70200
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX (arginfo_passthru_re, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO (0, command, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO (1, result_code, IS_LONG, 1)
#else
ZEND_BEGIN_ARG_INFO_EX (arginfo_passthru_re, 0, 0, 1)
    ZEND_ARG_INFO (0, command)
    ZEND_ARG_INFO (1, return_value)
#endif
ZEND_END_ARG_INFO ()
/* }}} */

/* arginfo_jailed_shellcmd_re {{{
 */
#if PHP_VERSION_ID >= 70200
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX (arginfo_jailed_shellcmd_re, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO (0, cmd, IS_STRING, 0)
#else
ZEND_BEGIN_ARG_INFO (arginfo_jailed_shellcmd_re, 0)
	ZEND_ARG_INFO (0, cmd)
#endif
ZEND_END_ARG_INFO ()
/* }}} */

/* arginfo_shell_exec_re {{{
 */
#if PHP_VERSION_ID >= 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX (arginfo_shell_exec_re, 0, 1, MAY_BE_STRING|MAY_BE_FALSE|MAY_BE_NULL)
	ZEND_ARG_TYPE_INFO (0, command, IS_STRING, 0)
#elif PHP_VERSION_ID >= 70200
ZEND_BEGIN_ARG_INFO_EX (arginfo_shell_exec_re, 0, 0, 1)
	ZEND_ARG_TYPE_INFO (0, command, IS_STRING, 0)
#else
ZEND_BEGIN_ARG_INFO (arginfo_shell_exec_re, 0)
	ZEND_ARG_INFO (0, cmd)
#endif
ZEND_END_ARG_INFO ()
/* }}} */

/* arginfo_popen_re {{{
 */
#if PHP_VERSION_ID >= 70200
ZEND_BEGIN_ARG_INFO_EX (arginfo_popen_re, 0, 0, 2)
    ZEND_ARG_TYPE_INFO (0, command, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO (0, mode, IS_STRING, 0)
#else
ZEND_BEGIN_ARG_INFO (arginfo_popen_re, 0)
    ZEND_ARG_INFO (0, command)
    ZEND_ARG_INFO (0, mode)
#endif
ZEND_END_ARG_INFO ()
/* }}} */
/* }}} */

/* {{{ proc_open.c */
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
/* arginfo_proc_terminate {{{
 */
#if PHP_VERSION_ID >= 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX (arginfo_proc_terminate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO (0, process)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE (0, signal, IS_LONG, 0, "15")
#elif PHP_VERSION_ID >= 70200
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX (arginfo_proc_terminate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO (0, process)
	ZEND_ARG_TYPE_INFO (0, signal, IS_LONG, 0)
#else
ZEND_BEGIN_ARG_INFO_EX (arginfo_proc_terminate, 0, 0, 1)
	ZEND_ARG_INFO (0, process)
	ZEND_ARG_INFO (0, signal)
#endif
ZEND_END_ARG_INFO ()
/* }}} */

/* arginfo_proc_close {{{
 */
#if PHP_VERSION_ID >= 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX (arginfo_proc_close, 0, 1, IS_LONG, 0)
#else
ZEND_BEGIN_ARG_INFO (arginfo_proc_close, 0)
#endif
	ZEND_ARG_INFO (0, process)
ZEND_END_ARG_INFO ()
/* }}} */

/* arginfo_proc_get_status {{{
 */
#if PHP_VERSION_ID >= 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX (arginfo_proc_get_status, 0, 1, IS_ARRAY, 0)
#else
ZEND_BEGIN_ARG_INFO (arginfo_proc_get_status, 0)
#endif
	ZEND_ARG_INFO (0, process)
ZEND_END_ARG_INFO ()
/* }}} */

/* arginfo_proc_open {{{
 */
ZEND_BEGIN_ARG_INFO_EX (arginfo_proc_open, 0, 0, 3)
#if PHP_VERSION_ID >= 80000
	ZEND_ARG_TYPE_MASK (0, command, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO (0, descriptor_spec, IS_ARRAY, 0)
	ZEND_ARG_INFO (1, pipes)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE (0, cwd, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE (0, env_vars, IS_ARRAY, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE (0, options, IS_ARRAY, 1, "null")
#elif PHP_VERSION_ID >= 70200
	ZEND_ARG_INFO (0, command)
	ZEND_ARG_TYPE_INFO (0, descriptor_spec, IS_ARRAY, 0)
	ZEND_ARG_INFO (1, pipes)
	ZEND_ARG_TYPE_INFO (0, cwd, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO (0, env_vars, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO (0, options, IS_ARRAY, 1)
#else
	ZEND_ARG_INFO (0, command)
	ZEND_ARG_INFO (0, descriptorspec) /* ARRAY_INFO(0, descriptorspec, 1) */
	ZEND_ARG_INFO (1, pipes) /* ARRAY_INFO(1, pipes, 1) */
	ZEND_ARG_INFO (0, cwd)
	ZEND_ARG_INFO (0, env) /* ARRAY_INFO(0, env, 1) */
	ZEND_ARG_INFO (0, other_options) /* ARRAY_INFO(0, other_options, 1) */
#endif
ZEND_END_ARG_INFO ()
/* }}} */
#endif
/* }}} */

/* arginfo_pcntl_exec_re {{{
 * located in ext/pcntl.c
 */
#if PHP_VERSION_ID >= 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX (arginfo_pcntl_exec_re, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO (0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE (0, args, IS_ARRAY, 0, "[]")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE (0, env_vars, IS_ARRAY, 0, "[]")
#elif PHP_VERSION_ID >= 70200
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX (arginfo_pcntl_exec_re, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO (0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO (0, args, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO (0, env_vars, IS_ARRAY, 0)
#else
ZEND_BEGIN_ARG_INFO_EX (arginfo_pcntl_exec_re, 0, 0, 1)
    ZEND_ARG_INFO (0, path)
    ZEND_ARG_INFO (0, args)
    ZEND_ARG_INFO (0, envs)
#endif
ZEND_END_ARG_INFO ()
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
