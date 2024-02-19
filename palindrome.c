#include <stdio.h>

int main()
{
    char str[100];
    int i, length=0, flag=0, start, end;

    printf("Input a string: ");

    fgets(str, 100, stdin);
    for (i=0; str[i]!='\0'; i++)
    {
        length++;
    }
    printf("length %d\n \n", length);
    for (i=0; length-i>=i; i++){
        printf("%s start.\n", str[i]);
        printf("%s end .\n", str[length-i]);
        if (str[i]!=str[length-i]){
            flag=0;
        }
    }
    // Read in input from the command line

    // Find the length of the string.
    // Hint: How do you know when a string ends in C?

    // Check if str is a palindrome.


    if(flag==1)
        printf("%s is not a palindrome.\n", str);
    else
        printf("%s is a palindrome.\n", str);

    return 0;
}
