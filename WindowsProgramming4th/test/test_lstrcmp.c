/*
 * https://baike.baidu.com/item/lstrcmpi/0?fromModule=lemma_inlink
 */
#define _UNICODE
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
//#include <string>


int _tmain(int argc, LPTSTR argv[])
{
//    lstrcmp_ex1();

    char * str1 = "coop";
    char * str2 = "co-op";
    int cmp = lstrcmp(str1, str2);
    printf("cmp = %d\n", cmp);

	return 0;
}

