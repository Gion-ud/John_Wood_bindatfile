#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "bindatfile/bindatfile.h"
#include <libc_chked.h>
#include <stddef.h>

const int idx = 22;
int main(void) {
	int fd = open("data.bin", O_RDONLY);
	if (fd < 0) {
		perror("open failed");
		return errno;
	}
	struct stat sb = {0};
	fstat(fd, &sb);

	void *f_mem = mmap(
		NULL,
		sb.st_size,
		PROT_READ,
		MAP_PRIVATE,
		fd,
		0
	);
	if (f_mem == MAP_FAILED) {
		perror("mmap failed");
		close(fd);
		return errno;
	}

	DAT_FILE_HEADER header = *(DAT_FILE_HEADER*)((byte_t*)f_mem);
	uoff32_t *offsettable = (uoff32_t*)((byte_t*)f_mem + header.offtableoff);
	DAT_ENTRY_HEADER entry_header = *(DAT_ENTRY_HEADER*)(
		(byte_t*)f_mem + offsettable[idx]
	);
	int ret = write(
		STDOUT_FILENO,
		(byte_t*)f_mem + offsettable[idx] + sizeof(DAT_ENTRY_HEADER),
		entry_header.len
	);
	if (ret < 0) {
		perror("write failed");
	}
	ret = write(STDOUT_FILENO, "\n", 1);
	if (ret < 0) {
		perror("write failed");
	}

	munmap(f_mem, sb.st_size);
	close(fd);
	return 0;
}
