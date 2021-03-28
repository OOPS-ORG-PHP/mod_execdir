#!/bin/bash

source /usr/share/annyung-release/functions.d/bash/functions
setAnsi
tcolor="${bblack}${bgwhite}"

errmsg () {
	echo "$*" 1>&2
}

usage () {
	echo "${bwhite}Usage:${normal} $0 [clean|pack|test [php-version]]"
	exit 1
}

mod_name="$( grep "^ZEND_GET_MODULE" *.c | grep -Po '(?<=\()[a-z]+(?=\))' )"

opts=$(getopt -u -o h -l help -- "$@")
[ $? != 0 ] && usage

set -- ${opts}
for i
do
	case "$i" in
		-h|--help)
			usage
			shift
			;;
		--)
			shift
			break
			;;
	esac
done

mode="${1}"

case "${mode}" in
	clean)
		cat <<-EOL
			${bwhite}[ -f Makefile ] && make distclean
			rm -rf autom4te.cache build include modules
			rm -f .deps Makefile* ac*.m4 dyn*.m4 compile
			rm -f config.h* config.nice configure* config.sub config.guess
			rm -f install-sh ltmain.sh missing mkinstalldirs run-tests.php

			rm -f package.xml
			rm -f tests/*.{diff,exp,log,out,php,sh,mem}
			rm -f ${mod_name}_arginfo.h && git checkout -- ${mod_name}_arginfo.h
			${normal}----->
		EOL

		[ -f Makefile ] && make distclean
		rm -rf autom4te.cache build include modules
		rm -f .deps Makefile* ac*.m4 compile
		rm -f config.h* config.nice configure* config.sub config.guess
		rm -f install-sh ltmain.sh missing mkinstalldirs run-tests.php

		rm -f package.xml
		rm -f tests/*.{diff,exp,log,out,php,sh,mem}
		rm -f ${mod_name}_arginfo.h && git checkout -- ${mod_name}_arginfo.h
		;;
	pack)
		cp -af package.xml.tmpl package.xml
		list=$(grep "md5sum" ./package.xml | sed -r 's/.*"@|@".*//g')

		for i in ${list}
		do
			md5s=$(md5sum "$i" | awk '{print $1}')
			perl -pi -e "s!\@${i}\@!${md5s}!g" ./package.xml
		done

		curdate=$(date +'%Y-%m-%d')
		curtime=$(date +'%H:%M:%S')

		perl -pi -e "s!\@curdate\@!${curdate}!g" ./package.xml
		perl -pi -e "s!\@curtime\@!${curtime}!g" ./package.xml

		pecl package
		rm -f package.xml
		;;
	test)
		# support PHP 4.3 and after

		PHPBIN=/opt/php-qa/php${2}/bin/php
		PHPIZE=/opt/php-qa/php${2}/bin/phpize
		PHPCONFIG=/opt/php-qa/php${2}/bin/php-config
		PHP_OPT=""
		LEGACY_TEST=0

		if [[ $# == 2 ]]; then
			./manage.sh clean
			echo "${PHPIZE} && ./configure"
			${PHPIZE} && ./configure && make -j8 || exit 0

			if [[ ! -f ./run-tests.php ]]; then
				LEGACY_TEST=1
				PHP_OPT="-n -d 'enable_dl=1' -d 'safe_mode=0' -d 'disable_error=0'"
			fi
		fi
		PHP_OPT+=" -d 'track_errors=1' -d 'extension_dir=./modules/' -d 'extension=${mod_name}.so'"

		if [[ -f tests/${3}.php ]]; then
			cat <<-EOL
				${bgreen}------------------------------------------------------------------------
				Sample code execution:

				${bcyan}${PHPBIN} ${PHP_OPT} test/${3}.php
				${bgreen}------------------------------------------------------------------------${normal}

			EOL

			${PHPBIN} ${PHP_OPT} test/${3}.php
			exit $?
		elif [[ -f ${3} ]]; then
			cat <<-EOL
				${bgreen}------------------------------------------------------------------------
				Sample code execution:

				${bcyan}${PHPBIN} ${PHP_OPT} ${3}
				${bgreen}------------------------------------------------------------------------${normal}

			EOL

			eval "${PHPBIN} ${PHP_OPT} ${3}"
			exit $?
		fi

		grep "^PHP_DEPRECATED_DIRECTIVES_REGEX =" Makefile | grep -q "safe_mode"
		[[ $? == 0 ]] && {
			perl -pi -e 's/safe_mode\|//g' Makefile
		}

		cat <<-EOL
			${bwhite}~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			${tcolor}** MAKE test::                                                       ${normal}
			${bwhite}~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			${bcyan}make test PHP_EXECUTABLE=${PHPBIN}
			${bwhite}~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~${normal}

		EOL

		(( LEGACY_TEST == 1 ))                                                   \
			&& PHP_EXECUTABLE=${PHPBIN} bash /opt/php-qa/TEST-ENV/legacy-test.sh \
			|| make test PHP_EXECUTABLE=${PHPBIN} <<< n

		exit $?
		;;
	*)
		errmsg "Unsupport mode '${1}'"
		exit 1
esac

exit 0
