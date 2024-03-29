dnl config.m4 for extension execdir

dnl build support C99
AC_PROG_CC_C99

PHP_ARG_WITH(
	execdir, for execdir support,
	[AC_HELP_STRING(
		[--with-execdir[=DIR]      Jailed shell command to the specified directory],
		[[default=/var/lib/php/bin]]
	)], [], []
)
PHP_ARG_WITH(
	execdir-compat, for original system functions support,
	[AC_HELP_STRING(
		[--with-execdir-compat   Support original system functions with '_orig' suffix],
		[[default=no]]
	)], [], [with_execdir_compat=no]
)

if test "$PHP_EXECDIR" != "no"; then
	AC_DEFINE(HAVE_EXECDIR, 1, [ ])
	CFLAGS="${CFLAGS} -Wall"

	PHP_SUBST(LDFLAGS)
	PHP_SUBST(CPPFLAGS)

	AC_MSG_CHECKING([for path of exec dir])
	if test "$PHP_EXECDIR" != "yes" -a "$PHP_EXECDIR" != "yes, shared"; then
		AC_DEFINE_UNQUOTED(PHP_EXECDIR, "$PHP_EXECDIR", [Support jailed system functions])
		AC_MSG_RESULT([$PHP_EXECDIR])
	else
		AC_DEFINE(PHP_EXECDIR, "/var/lib/php/bin", [ ])
		AC_MSG_RESULT([/var/lib/php/bin])
	fi

	AC_MSG_CHECKING([for original system functions])
	if test "$PHP_EXECDIR_COMPAT" = "yes"; then
		AC_DEFINE([PHP_EXECDIR_COMPAT], [], [Support original system function with _orig suffix])
	fi

	sources="php_execdir.c execdirapi.c proc_open.c proc_open8.c proc_open74.c proc_open7.c proc_open5.c proc_open53.c"

	PHP_NEW_EXTENSION(execdir, $sources, $ext_shared,, \\$(CFALGS))
fi
