
if [ "$1" = "package" ]; then
	list="README.md README.ko.md config.m4 execdirapi.c execdirapi.h php_execdir.c php_execdir.h proc_open.c proc_open5.c proc_open53.c"

	for i in ${list}
	do
		md5v="$(md5sum ${i} | awk '{print $1}')"
		perl -pi -e \
			"s/(<file md5sum=)\"[^\"]*\" (name=\"${i}\" role=)/\1\"${md5v}\" \2/g" \
			package.xml
	done

	for i in $(ls tests/*.*)
	do
		fname=$(basename $i)
		md5v="$(md5sum ${i} | awk '{print $1}')"
		perl -pi -e \
			"s/(<file md5sum=)\"[^\"]*\" (name=\"${fname}\" role=)/\1\"${md5v}\" \2/g" \
			package.xml
	done

	pecl package
	exit 0
fi



[ -f Makefile ] && make distclean
rm -rf modules/ build/ include/ auto*
rm -f tests/*.diff tests/*.exp tests/*.log tests/*.out tests/*.php tests/*.sh
mv config.m4 tests/config.m4
rm -f .deps Makefile* ac* config.* configure* install-sh ltmain.sh missing mkinstalldirs run-tests.php
mv tests/config.m4 ./config.m4
