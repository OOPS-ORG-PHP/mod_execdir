[ -f Makefile ] && make distclean
rm -rf modules/ build/ include/ auto*
rm -f tests/*.diff tests/*.exp tests/*.log tests/*.out tests/*.php tests/*.sh
mv config.m4 tests/config.m4
rm -f .deps Makefile* ac* config.* configure* install-sh ltmain.sh missing mkinstalldirs run-tests.php
mv tests/config.m4 ./config.m4
