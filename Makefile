MM_STATICLIBRARIES:=sharedlibrary-mini
MM_SHAREDLIBRARIES:=sharedlibrarytest
MM_TESTS:=test
MM_RELEASE:=sharedlibrary-mini

sharedlibrary-mini.c:=sharedlibrary_mini.c

sharedlibrarytest.c:=sharedlibrarytest.c
sharedlibrarytest-I:=./

test-:=-Wl,--wrap=malloc,--wrap=free,--wrap=main
test.c:=test.c
test-I:=../test-mini/ ./
test-L:=../test-mini/ ./
test-l:=test-mini sharedlibrary-mini sharedlibrarytest

include makefile_mini.mk
