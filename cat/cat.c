#include <unistd.h> 

char buf[255];
int main() {
	int read_cnt = 0;
	int write_cnt = 0;
	do {
		write_cnt = 0;
		read_cnt = read(STDIN_FILENO, buf, 255);
		if (read_cnt == -1) {
			return 0;
		}
		while (read_cnt - write_cnt != 0) {
			int tmp_write_cnt = 0; 
			tmp_write_cnt = write(STDOUT_FILENO, buf + write_cnt, read_cnt - write_cnt);
			if (tmp_write_cnt == -1) {
				return 0;
			}
			write_cnt += tmp_write_cnt;
		}
	} while (read_cnt != 0);
	return 0;
} 
