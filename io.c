#include <io.h>
#include <fcntl.h>

int main() {

int fd = open("file.bin", O_RDONLY);

char buf[1024];
read(fd, buf, sizeof(buf));

close(fd);
}