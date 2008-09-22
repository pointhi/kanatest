#!/bin/sh
AUTOCONF_VERSION=2.61
AUTOMAKE_VERSION=1.10

echo -n "checking for autoconf >= $AUTOCONF_VERSION... "
if (autoconf --version) < /dev/null > /dev/null 2>&1; then
    V=`autoconf --version | grep -iw autoconf | sed -n 's/.* \([0-9.]*\)[-a-z0-9]*$/\1/p'`
    MA1=`echo "$V" | cut -d "." -f1`;
    MI1=`echo "$V" | cut -s -d "." -f2`;
    MA2=`echo "$AUTOCONF_VERSION" | cut -d "." -f1`;
    MI2=`echo "$AUTOCONF_VERSION" | cut -d "." -f2;`
    test -z "$MINOR1" && MINOR1="0";

    if [ "$MA1" -gt "$MA2" ] || [ "$MA1" -eq "$MA2" -a "$MI1" -ge "$MI2" ]; then
        echo "found"
    else
        echo "not found"
        exit -1
    fi
else
    echo
    echo "not found"
    exit -1
fi

echo -n "checking for automake >= $AUTOMAKE_VERSION... "
if (automake --version) < /dev/null > /dev/null 2>&1; then
    V=`automake --version | grep automake | sed -n 's/.* \([0-9.]*\)[-a-z0-9]*$/\1/p'`
    MA1=`echo "$V" | cut -d "." -f1`;
    MI1=`echo "$V" | cut -s -d "." -f2`;
    MA2=`echo "$AUTOMAKE_VERSION" | cut -d "." -f1`;
    MI2=`echo "$AUTOMAKE_VERSION" | cut -d "." -f2;`
    test -z "$MINOR1" && MINOR1="0";

    if [ "$MA1" -gt "$MA2" ] || [ "$MA1" -eq "$MA2" -a "$MI1" -ge "$MI2" ]; then
        echo "found"
    else
        echo "not found"
        exit -1
    fi
else
    echo
    echo "not found"
    exit -1
fi

echo -n "checking for configure.ac... "
if [ ! -e configure.ac ]; then
    echo "not found"
    exit -1
else
    echo "found"
fi

echo -n "checking for Makefile.am... "
if [ ! -e Makefile.am ]; then
    echo "not found"
    exit -1
else
    echo "found"
fi

echo -n "generate configure script... "
aclocal 
autoheader 
autoconf 
automake --add-missing --copy --force
echo "done"

rm -rf ./autom4te.cache

