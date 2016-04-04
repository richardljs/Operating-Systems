#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{

	int fd;
	struct stat st;
	if(argc != 2) {
		printf(1, "Incorrect number of arguments. Usage: filestat <file>\n");
		exit();
	}
	if((fd = open(argv[1], 0)) < 0) {
		printf(1, "Can't open file!");
	}
	fstat(fd, &st);
	if(st.type == T_DIR) printf(1, "File type: %s, size: %d, checksum: %d\n", "Directory", st.size, st.checksum);
	else if(st.type == T_FILE) printf(1, "File type: %s, size: %d, checksum: %d\n", "File", st.size, st.checksum);
	else if(st.type == T_DEV) printf(1, "File type: %s, size: %d, checksum: %d\n", "Special Device", st.size, st.checksum);
	else if(st.type == T_CHECKED) printf(1, "File type: %s, size: %d, checksum: %d\n", "File with Checksum", st.size, st.checksum);
	close(fd);
	exit();

}
