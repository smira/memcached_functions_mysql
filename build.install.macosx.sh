#!/bin/sh

make
cd src/
gcc -bundle -bundle_loader /opt/local/libexec/mysqld -o libmemcached_functions_mysql.dylib .libs/args.o .libs/add.o .libs/get.o \
    .libs/set.o .libs/prepend.o .libs/replace.o .libs/increment.o .libs/decrement.o .libs/append.o .libs/delete.o .libs/servers.o \
    .libs/version.o .libs/stats.o  -L/opt/local/lib/ -L/opt/local/lib /opt/local/lib/libmemcached.dylib

sudo cp libmemcached_functions_mysql.dylib /usr/lib/libmemcached_functions_mysql.so
sudo /opt/local/etc/LaunchDaemons/org.macports.mysql5/mysql5.wrapper restart
