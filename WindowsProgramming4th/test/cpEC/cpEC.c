#include <stdio.h>
#include <errno.h>
#define BUF_SIZE 256

int main(int argc, char *argv[])
{
	FILE *in_file, *out_file;
	char rec[BUF_SIZE];
	size_t bytes_in, bytes_out;

	if (argc != 3) {
		printf("Usage: cpEC file1 file2\n");
		return 1;
	}
	in_file = fopen(argv[1], "rb");
	if (in_file == NULL) {
//		printf("argv[1]= %s", argv[1]);
		perror(argv[1]);
		return 2;
	}
	out_file = fopen(argv[2], "wb");
	if (out_file == NULL) {
//		printf("argv[2]= %s", argv[2]);
		perror(argv[2]);
		return 3;
	}

	/* 一次處理一個檔案 */
	while ((bytes_in = fread(rec, 1, BUF_SIZE, in_file)) > 0) {
		bytes_out = fwrite(rec, 1, bytes_in, out_file);
		if (bytes_out != bytes_in) {
			perror("Fatal write error.");
			return 4;
		}
	}

	fclose(in_file);
	fclose(out_file);
	return 0;
}
