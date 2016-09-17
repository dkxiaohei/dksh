void my_exit(void)
{
    __asm__ ("mov $1, %eax\n"
             "mov $0, %ebx\n"
             "int $0x80");    // extended asm
}
