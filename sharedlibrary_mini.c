#include "sharedlibrary_mini.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if defined(_WIN32)
#include <windows.h>
#else //< #elif defined(__linux__)
#include <dlfcn.h>
#endif


static void append_or_add_one_element(int elementSize, int* numElements, void** elements)
{
	void* a = *elements;
	//*elements = (void*)new char[elementSize * ((*numElements) + 1)];
	*elements = malloc(elementSize * ((*numElements) + 1));	
	if(*numElements > 0)
	{
		memcpy(*elements, a, elementSize * (*numElements));
		//delete a;
		free(a);		
	}
	++*numElements;
}
#define append_or_add_one_element2(numElements, elements) append_or_add_one_element(sizeof **elements, numElements, (void**)elements)

static void remove_last_num_elements(int elementSize, int* numElements, void** elements, int lastNumElementsToRemove)
{
	if(*numElements == lastNumElementsToRemove)
	{
		//delete *elements;
		free(*elements);
	}
	else
	{
		void* a = *elements;
		//*elements = (void*)new char[elementSize * ((*numElements) - lastNumElementsToRemove)];
		*elements = malloc(elementSize * ((*numElements) - lastNumElementsToRemove));
		memcpy(*elements, a, elementSize * ((*numElements) - lastNumElementsToRemove));
		//delete a;
		free(a);
	}
	*numElements -= lastNumElementsToRemove;
}
#define remove_last_num_elements2(numElements, elements, lastNumElementsToRemove) remove_last_num_elements(sizeof **elements, numElements, (void**)elements, lastNumElementsToRemove)

#ifdef _WIN32
static void getlasterror_to_string(int* a, char* b)
{
	DWORD c = GetLastError();

	char* d;
	// NOTE: FORMAT_MESSAGE_MAX_WIDTH_MASK such that "[FormatMessageA]..
	//       .. ignores regular line breaks in the message definition..
	//       .. text"
	//       ^
	//       https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-formatmessagea
	// NOTE: currently assuming US English is always available (I have no..
	//       .. proof whether or not this is so)
	//       v
	if(FormatMessageA(FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, c, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPTSTR)&d, 0, NULL) != 0)
	{
		if(b == NULL)
		{
			*a = strlen(d);
		}
		else
		{
			strncpy(b, d, *a);
		}
		LocalFree(d);
	}
	else
	{
		if(b == NULL)
		{
			// https://stackoverflow.com/questions/29087129/how-to-calculate-the-length-of-output-that-sprintf-will-generate
			*a = snprintf(NULL, 0, "%u", c);
		}
		else
		{
			snprintf(b, *a, "%u", c);			
		}
	}	
}
#endif

//*****************************************************************************

void(*on_print)(char* a, FILE* b) = NULL;
static void on_printf(FILE* a, char* b, ...)
{
	va_list c;
	va_start(c, b);
	
	int d = vsnprintf(NULL, 0, b, c);
	
	char e[d + 1];
	
	vsprintf(e, b, c);
	
	va_end(c);
	
	on_print(e, a);
}
#define on_print_warning(a) on_print(stdout, "warning: " a)
#define on_print_warning2(a) if(on_print != NULL) { on_print_warning(a); }
#define on_print_warningf(format, ...) on_printf(stdout, "warning: " format __VA_OPT__(,) __VA_ARGS__)
#define on_print_warningf2(format, ...) if(on_print != NULL) { on_print_warningf(format __VA_OPT__(,) __VA_ARGS__); }
#define on_print_error(a) on_print(stderr, "error: " a)
#define on_print_error2(a) if(on_print != NULL) { on_print_error(a); }
#define on_print_errorf(format, ...) on_printf(stderr, "error: " format __VA_OPT__(,) __VA_ARGS__)
#define on_print_errorf2(format, ...) if(on_print != NULL) { on_print_errorf(format __VA_OPT__(,) __VA_ARGS__); }

struct info_about_sharedlibrary_t
{
	int bIsSharedlibrary;
	char* pathToFile;
#if defined(_WIN32)
	struct
	{
		struct
		{
			HANDLE a;
		} handle;
	} win32;
#else //< #elif defined(__linux__)
	struct
	{
		void* a;
	} linux;
#endif
};
struct info_about_sharedlibrary_t info_about_sharedlibrary_default = { .bIsSharedlibrary = 1 };

int numSharedlibrariesTheresRoomFor = 0;
int numSharedlibraries = 0;
struct info_about_sharedlibrary_t* infoPerSharedlibrary;

static int is_sharedlibrary(int sharedlibrary)
{
	if((sharedlibrary < 0) | (sharedlibrary >= numSharedlibraries))
	{
		return -1;
	}
	if(infoPerSharedlibrary[sharedlibrary].bIsSharedlibrary == 0)
	{
		return -1;
	}
}

//*****************************************************************************

void sm_set_on_print(void(*a)(char*, FILE*))
{
	on_print = a;
}
void sm_unset_on_print()
{
	on_print = NULL;
}

int sm_load(char* pathToFile, int* sharedlibrary)
{
	for(int i = 0; i < numSharedlibrariesTheresRoomFor; ++i)
	{
		if(infoPerSharedlibrary[i].bIsSharedlibrary == 0)
		{
			continue;
		}
		if(strcmp(infoPerSharedlibrary[i].pathToFile, pathToFile) == 0)
		{
			return -1;
		}
	}

#if defined(_WIN32)
	HANDLE a = LoadLibraryA(pathToFile);
	if(a == NULL)
	{
		if(on_print != NULL)
		{
			int b;
			getlasterror_to_string(&b, NULL);
			char c[b + 1];
			getlasterror_to_string(&b, c);
			
			on_print_errorf("%s in %s\n", __FUNCTION__);
		}
		
		return 0;
	}
	
#else //< #elif defined(__linux__)
	void* a = dlopen(filename, RTLD_NOW);
	if(a == NULL)
	{
		on_print_errorf2("%s in %s\n", dlerror(), __FUNCTION__);
		return 0;
	}
#endif

	int indexToSharedlibrary = -1;
	for(int i = 0; i < numSharedlibraries; ++i)
	{
		if(infoPerSharedlibrary[i].bIsSharedlibrary == 0)
		{
			indexToSharedlibrary = i;
		}
	}
	if(indexToSharedlibrary == -1)
	{
		append_or_add_one_element2(&numSharedlibrariesTheresRoomFor, &infoPerSharedlibrary);
		indexToSharedlibrary = numSharedlibrariesTheresRoomFor - 1;
	}
	
	struct info_about_sharedlibrary_t* infoAboutSharedlibrary = &infoPerSharedlibrary[indexToSharedlibrary];
	*infoAboutSharedlibrary = info_about_sharedlibrary_default;

#if defined(_WIN32)
	infoAboutSharedlibrary->win32.handle.a = a;
#else //< #elif defined(__linux__)
	infoAboutSharedlibrary->linux.a = a;
#endif

	++numSharedlibraries;

	return 1;
}
int sm_unload(int sharedlibrary)
{
	if(is_sharedlibrary(sharedlibrary) == 0)
	{
		return -1;
	}
	
	struct info_about_sharedlibrary_t* a = &infoPerSharedlibrary[sharedlibrary];
	
#if defined(_WIN32)
	if(FreeLibrary(a->win32.handle.a) == 0)
	{
		if(on_print != NULL)
		{
			int b;
			getlasterror_to_string(&b, NULL);
			char c[b + 1];
			getlasterror_to_string(&b, c);
			
			on_print_warningf("%s in %s\n", c, __FUNCTION__);
		}
	}
#else //< #elif defined(__linux__)
	if(dlclose(infoAboutSharedlibrary->linux.a) != 0)
	{
		on_print_warningf2("%s in %s\n", dlerror(), __FUNCTION__);
	}
#endif
	
	int numElementsAfter = (numSharedlibrariesTheresRoomFor - 1) - sharedlibrary;
	if(numElementsAfter == 0)
	{
		int indexToPreviousSharedlibrary = -1;
		for(int i = sharedlibrary - 1; i >= 0; --i)
		{
			if(infoPerSharedlibrary[i].bIsSharedlibrary == 1)
			{
				indexToPreviousSharedlibrary = i;
				break;
			}
		}
		
		int lastNumElementsToRemove = (numSharedlibrariesTheresRoomFor - 1) - indexToPreviousSharedlibrary;
		
		remove_last_num_elements2(&numSharedlibrariesTheresRoomFor, &infoPerSharedlibrary, lastNumElementsToRemove);
	}
	else
	{
		a->bIsSharedlibrary = 0;
	}
	
	--numSharedlibraries;
	
	return 1;
}

int sm_import(int sharedlibrary, char* functionname, void(**function)())
{
	if(is_sharedlibrary(sharedlibrary) == 0)
	{
		return -1;
	}
	
	struct info_about_sharedlibrary_t* infoAboutSharedlibrary = &infoPerSharedlibrary[sharedlibrary];

#if defined(_WIN32)
	FARPROC a = GetProcAddress(infoAboutSharedlibrary->win32.handle.a, functionname);
	if(a == NULL)
	{
		if(on_print != NULL)
		{
			int b;
			getlasterror_to_string(&b, NULL);
			char c[b + 1];
			getlasterror_to_string(&b, c);
			
			on_print_errorf("%s in %s\n", c, __FUNCTION__);
		}
		
		return 0;
	}
	
	*function = (void(*)())a;
#else //< #elif defined(__linux__)
	// NOTE: "The correct way to distinguish an error from a symbol whose..
	//       .. value is NULL is to call dlerror(3) to clear any old error..
	//       .. conditions, then call dlsym(), and then call dlerror(3)..
	//       .. again, saving its return value into a variable, and check..
	//       .. whether this saved value is not NULL.",..
	//       .. https://man7.org/linux/man-pages/man3/dlsym.3.html
	dlerror();
	void* a = dlsym(infoAboutSharedlibrary->linux.a, functionname);
	char* b = dlerror();
	if(b == NULL)
	{
		on_print_errorf2("%s in %s\n", b, __FUNCTION__);
		
		return 0;
	}
	
	function = (void(*)())a;
#endif

	return 1;
}
