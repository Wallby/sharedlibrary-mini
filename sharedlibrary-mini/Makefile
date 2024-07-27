include makefile_mini.mk


$(call mm_start_parameters_t,a)
a.ignoredbinaries:=^test$(MM_EXECUTABLE_EXTENSION)$$ libsharedlibrarytest$(MM_SHAREDLIBRARY_EXTENSION)
$(call mm_start,a)

$(call mm_add_library_parameters_t,b)
b.filetypes:=EMMLibraryfiletype_Static
b.c:=sharedlibrary_mini.c
b.h:=sharedlibrary_mini.h
$(call mm_add_library,sharedlibrary-mini,b)

$(call mm_add_library_parameters_t,c)
c.filetypes:=EMMLibraryfiletype_Shared
c.c:=sharedlibrarytest.c
c.libraries:=sharedlibrary-mini
$(call mm_add_library,sharedlibrarytest,c)

$(call mm_add_executable_parameters_t,d)
d.c:=test.c
#d.libraries:=sharedlibrary-mini sharedlibrarytest test-mini
d.libraries:=sharedlibrary-mini sharedlibrarytest
d.lib:=test-mini
d.libFolders:=../test-mini
d.hFolders:=../test-mini
d.gccOrG++:=-Wl,--wrap=malloc,--wrap=free,--wrap=main
$(call mm_add_executable,test,d)

$(call mm_add_test_parameters_t,e)
e.executables:=test
$(call mm_add_test,test,e)

$(call mm_stop_parameters_t,f)
f.releasetypes:=EMMReleasetype_Zip
$(call mm_stop,f)