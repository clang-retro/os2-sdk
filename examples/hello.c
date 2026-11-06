/* Hello, world for OS/2, 16-bit (1.x) or 32-bit (2.x and later):
 *   clang --target=i8086-pc-os2 -Os hello.c -o hello.exe
 *   clang --target=i386-pc-os2 -Os hello.c -o hello.exe
 */
#include <stdio.h>

int
main(int argc, char **argv)
{
#ifdef __32BIT__
    puts("Hello from clang on 32-bit OS/2!");
#else
    puts("Hello from clang on 16-bit OS/2!");
#endif
    for (int i = 0; i < argc; i++)
        printf("argv[%d] = \"%s\"\n", i, argv[i]);
    return 0;
}
