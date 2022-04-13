void my_exit(void)
{
#ifdef __APPLE__
    __asm__ ("mov $0x2000001, %rax\n"
             "mov $0, %rdi\n"
             "syscall");
#else
    __asm__ ("mov $1, %eax\n"
             "mov $0, %ebx\n"
             "int $0x80");
#endif  /* __APPLE__ */
}
