#include <unistd.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<errno.h>

char buf[255];
int main(int argc, char** argv) {
	int read_cnt = 0;
	int write_cnt = 0;
	int fd = STDOUT_FILENO;
	if (argc > 1) {
		fd = open(argv[1], O_RDONLY);
		if (fd == -1) {
			perror("CAN'T OPEN FILE");
			return 0;
		}	
	}
	do {
		write_cnt = 0;
		int flag = 1;
		while (flag) {
			read_cnt = read(fd, buf, 255);
			if (read_cnt == -1 && errno != EINTR) {
				perror("READING_ERROR");
				return 0;
			} else if (read_cnt != -1) {
				flag = 0;
			}
		}
		
		while (read_cnt - write_cnt != 0) {
			int tmp_write_cnt = 0; 
			tmp_write_cnt = write(STDOUT_FILENO, buf + write_cnt, read_cnt - write_cnt);
			if (tmp_write_cnt == -1 && errno != EINTR ) {
				perror("WRITINGDING_ERROR");
				return 0;
			} else if (tmp_write_cnt != -1) {
				write_cnt += tmp_write_cnt;
			}
		}
	} while (read_cnt != 0);
	return 0;
} 
