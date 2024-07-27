include makefile_mini.mk


$(call mm_start_parameters_t,a)
$(call mm_start,a)

$(call mm_add_library_parameters_t,b)
b.filetypes:=EMMLibraryfiletype_Static
b.c:=sharedlibrary_mini.c
b.h:=sharedlibrary_mini.h
$(call mm_add_library,sharedlibrary-mini,b)

$(call mm_stop_parameters_t,f)
$(call mm_stop,f)