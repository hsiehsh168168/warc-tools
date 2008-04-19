#include <stdio.h>
#include <wmisc.h>

WPUBLIC int menu (void)
{
    int choix = 0;
   
    while (choix < 1 || choix > 3)
    {
        printf(" Running Tests :\n");
        printf("1 : Interactive Console Mode  \n");
        printf("2 : Basic Mode  \n");
        printf("3 : Automated Mode\n");  
        printf("Select a number ? ");
        scanf("%d", & choix);
    }   
   
    return choix;
}   
