/*
  +----------------------------------------------------------------------+
  | mod_execdir v1                                                       |
  +----------------------------------------------------------------------+
  | Copyright (c) JoungKyun.Kim                                          |
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
#include "ext/standard/php_string.h"

#include "execdirapi.h"
#include "php_execdir.h"

extern ZEND_DECLARE_MODULE_GLOBALS (execdir)

/* {{{ +-- CmdArgv * cmdargv_init (void)
 *
 * memory allocation CmdArgv structure
 */
CmdArgv * cmdargv_init (void) {
	CmdArgv * ret;

	ret = emalloc (sizeof (CmdArgv) + 1);
	ret->cmd  = NULL;
	ret->clen = 0;
	ret->argv = NULL;
	ret->alen = 0;
	ret->debug = 0;

	return ret;
}
/* }}} */

/* {{{ +-- void cmdargv_free (CmdArgv * v)
 *
 * memory free CmdArgv structure
 */
void cmdargv_free (CmdArgv * v) {
	if ( v == NULL )
		return;

	if ( v->cmd != NULL )
		efree (v->cmd);
	if ( v->argv != NULL )
		efree (v->argv);

	efree (v);
	v = NULL;
}
/* }}} */

/* {{{ +-- CmdArgv * make_cmdargv (char * cmd)
 *
 * saperate command and command arguments
 */
CmdArgv * make_cmdargv (char * cmd) {
	int       len  = strlen (cmd);
	int       i    = 0;
	char    * path = cmd;
	CmdArgv * ret  = NULL;

	//ret = cmdargv_init ();
	ret = emalloc (sizeof (CmdArgv));
	ret->cmd = NULL;
	ret->clen = 0;
	ret->argv = NULL;
	ret->alen = 0;
	ret->debug = 0;

	// trim path
	for ( i=0; i<len; i++ ) {
		// 32 -> space, 9 -> tab
		if ( cmd[i] == 32 || cmd[i] == 9 ) {
			path = cmd + i + 1;
			continue;
		}
		break;
	}
	len = strlen (path);

	if ( strncmp ("DEBUG:", path, 6) == 0 ) {
		ret->debug = 1;
		path += 6;
	} else if ( strncmp ("DDEBUG:", path, 7) == 0 ) {
		ret->debug = 2;
		path += 7;
	}

	// trim path again for DEBUG:
	for ( i=0; i<len; i++ ) {
		// 32 -> space, 9 -> tab
		if ( path[i] == 32 || path[i] == 9 ) {
			path += (i + 1);
			continue;
		}
		break;
	}
	len = strlen (path);

	if ( path[0] == '`' || (path[0] == '$' && path[1] == '(') ) {
		ret->cmd  = estrdup (path);
		ret->clen = strlen (path);

		if ( ret->debug > 0 ) {
			php_printf ("-- make_cmdargv ------\n");
			php_printf ("Origianl : %s\n", cmd);
			php_printf ("cmd      : %s (%d)\n", ret->cmd, ret->clen);
			php_printf ("argv     : %s (%d)\n", ret->argv, ret->alen);
			php_printf ("----------------------\n\n");
		}

		return ret;
	}

	for ( i=0; i<len; i++ ) {
		// 32 -> space, 9 -> tab
		if ( path[i] == 32 || path[i] == 9 || path[i] == ';' || path [i] == '|' || path[i] == '&' || path[i] == '>' ) {
			// 92 -> back slash
			if ( i == 0 || path[i-1] == 92 )
				continue;

			break;
		}
	}

	ret->argv = estrdup (path + i);
	{
		char * npath = estrdup (path);
		*(npath + i) = 0;
		ret->alen = strlen (ret->argv);
		ret->cmd  = estrdup (npath);
		ret->clen = strlen (npath);
		efree (npath);
	}

	if ( ret->debug > 0 ) {
		php_printf ("-- make_cmdargv ------\n");
		php_printf ("Origianl : %s\n", cmd);
		php_printf ("cmd      : %s (%d)\n", ret->cmd, ret->clen);
		php_printf ("argv     : %s (%d)\n", ret->argv, ret->alen);
		php_printf ("----------------------\n\n");
	}

	return ret;
}
/* }}} */

/* {{{ +-- PHPAPI char * get_jailed_shell_cmd (char * cmd)
 *
 * return value is needed to call efree
 */
PHPAPI char * get_jailed_shell_cmd (char * cmd) {
	size_t   exec_len;
	char   * exec_dir;
	char   * _cmd;

#if PHP_VERSION_ID < 50400
	if ( EXECDIR_G (exec_dir) )
		exec_dir = EXECDIR_G (exec_dir);
	else if ( PG (safe_mode_exec_dir) )
		exec_dir = PG (safe_mode_exec_dir);
	else
		exec_dir = "";
#else
	//exec_dir = PG (exec_dir);
	exec_dir = EXECDIR_G (exec_dir);
#endif
	exec_len = strlen (exec_dir);

	if ( exec_len ) {
		CmdArgv * cp;
		char    * c = NULL;
		char    * vcmd;
		char    * __cmd;

		cp = make_cmdargv (cmd);

		if ( cp->cmd[0] != '`' && cp->cmd[0] != '$' && (c = strrchr (cp->cmd, '/')) )
			vcmd = c + 1;
		else
			vcmd = cp->cmd;

		__cmd = emalloc (sizeof (char) * (cp->clen + cp->alen + 7 + 1));
		memset (__cmd, 0, sizeof (char) * (cp->clen + cp->alen + 7 + 1));

		sprintf (
				__cmd, "%s%s%s",
				cp->debug == 1 ? "DEBUG:" : (cp->debug == 2 ? "DDEBUG:" : ""),
				vcmd,
				cp->alen ? cp->argv : ""
				);

		cmdargv_free (cp);
		_cmd = php_jailed_shell_cmd (__cmd, exec_dir);
		efree (__cmd);
	} else {
		_cmd = estrdup (cmd);
	}

	return _cmd;
}
/* }}} */

/* {{{ +-- static int check_quote_block (struct quote_chk_char qc, struct quote_value * _qv)
 *
 * case by single quote or double quote
 * single quote (')  is digit 39
 * double quote (")  is digit 34
 * reverse slash (\) is digit 92
 */
static int check_quote_block (struct quote_chk_char qc, struct quote_value * _qv) {
	if ( qc.cur != 39 && qc.cur != 34 ) {
		switch (qc.cur) {
			case ';' :
			case '|' :
			case '`' :
			case '$' :
			case '&' :
				if ( qc.mcur == 92 )
					return 0;

				if ( _qv->quote > 0 ) {
					if ( _qv->dquote ) {
						if ( _qv->bquote )
							return 0;
						else if ( _qv->daquote )
							return 0;
					}

					return 1;
				}
		}
		return 0;
	}

	if ( qc.mcur == 92 )
		return 0;

	if ( _qv->quote > 0 ) {
		if ( qc.cur == 34 ) {
			if ( _qv->squote ) return 0;
		} else {
			if ( _qv->dquote ) return 0;
		}

		_qv->quote  = 0;
		_qv->squote = 0;
		_qv->dquote = 0;
	} else {
		_qv->quote = 1;
		if ( qc.cur == 34 ) {
			_qv->dquote = 1;
			_qv->squote = 0;
		} else {
			_qv->dquote = 0;
			_qv->squote = 1;
		}
	}

	return 0;
}
/* }}} */

/* {{{ static char * php_jailed_shell_cmd (char * cmd, char * path)
 */
static char * php_jailed_shell_cmd (char * cmd, char * path) {
	int      cmd_len  = 0;
	int      path_len = 0;
	int      buf_len  = 0;
	int      ep = 0;
	int      i, j, b, _len = 0;
	char   * buf;
	char   * _cmd, * x_cmd, * debug_cmd;
	char   * _path, * _tpath;
	int      _start;
	int      debug = 0;
	int      bskip = 0;
	struct quote_value qv = { 0, 0, 0, 0, 0 };
	struct quote_chk_char qc;

	cmd_len = strlen (cmd);
	debug_cmd = emalloc (sizeof (char *) * cmd_len + 1);
	memset (debug_cmd, 0, cmd_len);
	if ( ! strncmp ("DEBUG:", cmd, 6) ) {
		debug = 1;
		memcpy (debug_cmd, cmd + 6, cmd_len - 6);
	} else if ( ! strncmp ("DDEBUG:", cmd, 7) ) {
		debug = 2;
		memcpy (debug_cmd, cmd + 7, cmd_len - 7);
	} else {
		debug_cmd = estrdup (cmd);
	}

	{
#if PHP_VERSION_ID < 60000
		_tpath = php_trim (path, strlen (path), NULL, 0, NULL, 3 TSRMLS_CC);
		x_cmd = php_trim (debug_cmd, strlen (debug_cmd), NULL, 0, NULL, 3 TSRMLS_CC);
		_cmd = ( x_cmd[0] == '/' ) ? estrdup (x_cmd + 1) : estrdup (x_cmd);
#else
		zend_string * zbuf, * zbuf_r;

		zbuf = zend_string_init (path, strlen (path), 0);
		zbuf_r = php_trim (zbuf, NULL, 0, 3);

		_tpath = estrdup (ZSTR_VAL (zbuf_r));
		execdir_string_release (zbuf);
		execdir_string_release (zbuf_r);

		zbuf = zend_string_init (debug_cmd, strlen (debug_cmd), 0);
		zbuf_r = php_trim (zbuf, NULL, 0, 3);

		x_cmd = estrdup (ZSTR_VAL (zbuf_r));
		execdir_string_release (zbuf);
		execdir_string_release (zbuf_r);

		_cmd = ( x_cmd[0] == '/' ) ? estrdup (x_cmd + 1) : estrdup (x_cmd);
#endif
	}

	efree (x_cmd);
	efree (debug_cmd);

	cmd_len  = strlen (_cmd);
	path_len = strlen (_tpath);

	if ( path_len == 1 )
		b = !strcmp (_tpath, "/") ? 0 : 1;
	else if ( _tpath[path_len - 1] != '/' )
		b = 1;
	else
		b = 0;

	_path = emalloc (sizeof (char) * (cmd_len + path_len + 4));
	memset (_path, 0, sizeof (char) * (cmd_len + path_len + 4));
	sprintf (_path, "%s%s", _tpath, !b ? "" : "/");
	path_len = strlen (_path);
	efree (_tpath);

	for ( i=0; i<cmd_len; i++ ) {
		switch (_cmd[i]) {
			case ';' :
			case '|' :
			case '`' :
			case '$' :
			case '&' :
				_len++;
				break;
		}
	}

	_len++;
	buf_len = sizeof (char *) * (cmd_len + ((path_len + 2) * _len));
	buf = emalloc (buf_len);
	memset (buf, 0, buf_len);

	if ( _cmd[0] != ';' )
		memcpy (buf, _path, path_len);
	ep = strlen (buf);

	_len   = 0;
	_start = 0;

	if ( debug )
		php_printf ("Orig Cmd ==> %s\n", _cmd);

	for ( i=0; i<cmd_len; i++ ) {
roopstart:
		if ( debug > 1 ) {
			php_printf ("#### "
						"_cmd[i] => %c : "
						"i => %2d : "
						"cmd_len => %2d : "
						"_start => %2d : "
						"quote => %2d : "
						"squote => %2d : "
						"dquote => %2d : "
						"bquote => %2d : "
						"daquote => %2d\n",
						_cmd[i], i, cmd_len, _start,
						qv.quote, qv.squote, qv.dquote, qv.bquote, qv.daquote);
		}

		qc.mcur = _cmd[i-1];
		qc.cur  = _cmd[i];
		qc.acur = _cmd[i+1];

		if ( qc.mcur != '\\' && qc.cur == '`' ) {
			if ( ! qv.bquote ) qv.bquote = 1;
			else qv.bquote = 0;
		}

		if ( qc.mcur != '\\' && qc.cur == '$' && qc.acur == '(' ) {
			qv.daquote = 1;
		} else if ( qv.daquote && qc.mcur != '\\' && qc.cur == ')' ) {
			qv.daquote = 0;
		}

		if ( i == cmd_len - 1 && _start <= i ) {
			memcpy (buf + ep, _cmd + _start, i - _start + 1);
			break;
		}

		if ( check_quote_block (qc, &qv) ) {
			i++;
			goto roopstart;
		}

		switch (_cmd[i]) {
			case ';' :
			case '|' :
			case '`' :
			case '$' :
			case '&' :
				// if escape sor redirection(>& or &>) status, don't apply
				if ( _cmd[i-1] == '\\' || (_cmd[i] == '&' && (_cmd[i-1] == '>' || _cmd[i+1] == '>')) ) {
					i++;
					goto roopstart;
				}

				if ( ! bskip )
					memcpy (buf + ep, _cmd + _start, i - _start + 1);
				else {
					memset (buf + ep, _cmd[i], 1);
					bskip = 0;
				}
				_start = i + 1;
				ep = strlen (buf);

				/*
				if ( _cmd[i] == '`' && ! qv.bquote ) qv.bquote = 1;
				else if ( _cmd[i] == '`' && qv.bquote ) {
					qv.bquote = 0;
					i++;

					goto roopstart;
				}
				*/
				if ( _cmd[i] == '`' && ! qv.bquote ) {
					i++;
					goto roopstart;
				}

				// if not case of $() or && or ||, skip
				if ( _cmd[i] == '$' && _cmd[i+1] != '(' && ! qv.daquote ) {
					i += 2;
					goto roopstart;
				}

				// if case of `` or ;; that include any charactors, skip
				if ( (_cmd[i] == ';' && _cmd[i+1] == ';') || (_cmd[i] == '`' && _cmd[i+1] == '`') ) {
					i++;
					goto roopstart;
				}

				i++;
				if ( (_cmd[i] == '(' && _cmd[i-1] == '$') || (_cmd[i] == '&' && _cmd[i-1] == '&') ||
					 (_cmd[i] == '|' && _cmd[i-1] == '|') ) {
					memset (buf + ep, _cmd[i], 1);
					_start++;
					ep++;
					i++;
				}

				for ( j=i; j<cmd_len; j++ ) {
					// blank ( ) => 32
					// tab (\t)  => 9
					if ( _cmd[j] == 32 || _cmd[j] == 9 ) {
						memset (buf + ep, _cmd[j], 1);
						_start++;
						ep++;
						i++;
					} else {
						char   vcmd[256] = { 0, };
						char * _vcmd     = NULL;
						char * _tvcmd    = NULL;
						int    _vcmd_len = 0, y;

						for ( y=j; y<cmd_len; y++ ) {
							// if path include backticks (/bin/`echo /bin/ls`)
							if ( (! qv.bquote && _cmd[y] == '`') || (! qv.daquote && _cmd[y] == '$' && _cmd[y+1] == '(') ) {
								memcpy (buf + ep, path, path_len);
								ep = strlen (buf);
								memset (buf + ep, '/', 1);
								ep++;
								i = y;
								bskip++;
								goto roopstart;
							}

							// blank ( ) => 32
							// tab (\t)  => 9
							if ( _cmd[y] == 32 || _cmd[y] == 9 || (qv.bquote && _cmd[y] == '`') ) {
									memcpy (vcmd, _cmd + j, y - j );
									_tvcmd = strrchr (vcmd, '/');
									_vcmd = _tvcmd ? _tvcmd + 1 : vcmd;
									_vcmd_len = strlen (_vcmd);
									i = y;

									if ( _cmd[y] == '`' )
										 qv.bquote = 0;

									break;
							}
						}

						memcpy (buf + ep, _path, path_len);
						memcpy (buf + ep + path_len, _vcmd, _vcmd_len);
						ep += (path_len + _vcmd_len);
						_start = i;
						break;
					}
				}
				break;
		}
	}

	if ( debug )
		 php_printf ("Conv Cmd ==> %s\n--\n", buf);

	efree (_cmd);
	efree (_path);

	return buf; 
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
