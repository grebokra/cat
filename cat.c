#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

void usage(void);
void do_cat(int fd);
int main(int argc, char **argv) {
	int opt;
	while ((opt= getopt(argc, argv, "uh")) != -1) {
		switch (opt) {
		case 'h':
			usage();
			break;
		case 'u':
			setbuf(stdout, NULL);
			break;
		default:
			usage();
		}
	}	
	argv+= optind;
	
	int fd;
	if (*argv) {
		while (*argv) {
			if (strcmp(*argv, "-") == 0) {
				fd= fileno(stdin);
				if (fd < 0) {
					err(1, "stdin");
				}
			}
			else {
				fd= open(*argv, O_RDONLY);
				if (fd < 0) {
					err(1, "%s", *argv);
				}
			}
			do_cat(fd);
			argv++;
		}
	}
	else {
		fd= fileno(stdin);
		if (fd < 0) {
			err(1, "stdin");
		}
		do_cat(fd);
	}
	return 0;
}

void do_cat(int rfd) {
	int wfd;
	char *buf;
	struct stat fstats;
	size_t blksize;

	wfd= fileno(stdout);
	if (wfd < 0) {
		err(1, "stdout");
	}

	if (fstat(wfd, &fstats)) {
		err(1, "stdout");	
	}

	blksize= fstats.st_blksize;
	buf= malloc(blksize);

	ssize_t rc, wc;
	int offset= 0;
	rc= read(rfd, buf, blksize);
	while (rc > 0) {
		while (rc > 0) {
			wc= write(wfd, buf + offset, rc);
			if (wc < 0) {
				err(1, "stdout");
			}

			rc-= wc;
			offset+= wc;
		}

		offset= 0;
		rc = read(rfd, buf, blksize);
	}

}

void usage(void) {
	fprintf(stderr, "usage: cat [-u] [file ...]\n\"cat -h\" for help\n");
	exit(1);
}
