// Minimal newlib syscall stubs (_write lives in swo.c, _sbrk comes from nosys.specs).
#include <errno.h>
#include <sys/stat.h>

int _close(int fd)                   { (void)fd; return -1; }
int _fstat(int fd, struct stat *st)  { (void)fd; st->st_mode = S_IFCHR; return 0; }
int _isatty(int fd)                  { (void)fd; return 1; }
int _lseek(int fd, int p, int dir)   { (void)fd; (void)p; (void)dir; return 0; }
int _read(int fd, char *b, int n)    { (void)fd; (void)b; (void)n; errno = ENOSYS; return -1; }
