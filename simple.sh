#!/usr/bin/env bash
# Most minimal build command for mes.c
set -eux
rm -rf out-glibc/
mkdir out-glibc
gcc -ggdb \
	-D WITH_GLIBC=1 \
	mes_vector.c \
	mes_hash.c \
	mes_struct.c \
	mes_math.c \
	mes_strings.c \
	mes_module.c \
	mes_gc.c \
	mes_lib.c \
	mes_printer.c \
	mes_reader.c \
	mes_posix.c \
	mes_builtins.c \
	mes_eval.c \
	mes.c \
	temp.c \
	functions/in_set.c \
	functions/numerate.c \
	functions/file_print.c \
	-o out-glibc/mes

# Simplest of tests
echo '(display "hello\n")' | MES_BOOT=boot-01.scm out-glibc/mes

# Basic test.  Once this works there are ~30 tests in mes/test/*.test
MES_DEBUG=4 MES_PREFIX=mes MES=out-glibc/mes tests/base.test

# GC test
MES_DEBUG=3 MES_ARENA=10000 MES_MAX_ARENA=10000 MES_BOOT=gc-test.scm out-glibc/mes

# MesCC test
MES_DEBUG=2 MES_PREFIX=mes MES=out-glibc/mes sh -x scripts/mescc scaffold/hello.c
set +e
scaffold/hello
r=$?
if [ $r != 42 ]; then
   exit 1
fi

# If we ever need to get into hairy debugging, a scaffold with ~50
# bootstrap tests is available in mes: scaffold/boot/*.scm
