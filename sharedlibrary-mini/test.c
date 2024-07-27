#include <sharedlibrary_mini.h>
#include <test_mini.h>

#include <stdio.h>


SM_IMPORT int sharedlibrarytest1();

int test_1()
{
	if(sharedlibrarytest1() != 1234567890)
	{
		fputs("error: sharedlibrarytest() != 1234567890\n", stderr);
		return 0;
	}
	
	return 1;
}

int test_2()
{
	int sharedlibrary;
	if(sm_load2("libsharedlibrarytest", &sharedlibrary) != 1)
	{
		fputs("error: sm_load_library != 1\n", stderr);
		return 0;
	}
	
	float(*sharedlibrarytest2)();
	if(sm_import2(sharedlibrary, "sharedlibrarytest2", &sharedlibrarytest2) != 1)
	{
		fputs("error: sm_import2 != 1\n", stderr);
		return 0;
	}
	
	if(sharedlibrarytest2() != 1.234567890f)
	{
		fputs("error: sharedlibrarytest2() != 0246802468\n", stderr);
		return 0;
	}
	
	if(sm_unload(sharedlibrary) != 1)
	{
		fputs("error: sm_unload_library != 1\n", stderr);
		return 0;
	}
	
	return 1;
}

void my_on_print(char* a, FILE* b)
{
	fputs(a, b);
}

int main(int argc, char** argv)
{
	sm_set_on_print(&my_on_print);
	
	TM_TEST2(1)
	TM_TEST(2, 9)
	
	sm_unset_on_print();
}
