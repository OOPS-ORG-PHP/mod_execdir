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

struct quote_value {
	int quote;
	int squote;
	int dquote;
	int bquote;
	int daquote;
};

struct quote_chk_char {
	char mcur;
	char cur;
	char acur;
};

typedef struct cmd_argv {
	char * cmd;
	int    clen;
	char * argv;
	int    alen;
	int    debug;
} CmdArgv;

PHPAPI char * get_jailed_shell_cmd (char *);
static char * php_jailed_shell_cmd (char *, char *);
static int    check_quote_block (struct quote_chk_char, struct quote_value *);
