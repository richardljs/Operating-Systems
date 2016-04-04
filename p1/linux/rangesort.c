#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <stdint.h>

void
usage() 
{
    fprintf(stderr, "Usage: rangesort -i inputfile -o outputfile -l lowvalue -h highvalue\n");
    exit(1);
}

void
rangeErr()
{
	fprintf(stderr, "Error: Invalid range value\n");
    exit(1);
}

int compareKey (const void * a, const void * b)
{
   return ( (*(rec_t*)a).key - (*(rec_t*)b).key );
}

int
main(int argc, char *argv[])
{
	// input variables
	char *inFile = "/no/such/file";
	char *outFile   = "/no/such/file";
	long low = 0;
	long high = 0;
    int c;

    if (argc != 9)
    	usage();

    while ((c = getopt(argc, argv, "i:o:l:h:")) != -1) {
		switch (c) {
		case 'i':
			inFile 		= strdup(optarg);
			break;
		case 'o':
		    outFile     = strdup(optarg);
		    break;
		case 'l':
		    low			= strtoul(optarg, NULL, 10);
		    int i, lengthi;
		    lengthi = strlen(optarg);
            for (i = 0; i < lengthi; i++){
                if (optarg[i] < 48 || optarg[i] > 57){
                    rangeErr();
                    break;
                }
            }
		    break;
		case 'h':
		    high  		= strtoul(optarg, NULL, 10);
		    int j, lengthj;
		    lengthj = strlen(optarg);
		    for (j = 0; j < lengthj; j++){
                if (optarg[j] < 48 || optarg[j] > 57){
                    rangeErr();
                    break;
                }
            }
		    break;
		default:
		    usage();
		}
    }

    if((high > pow(2, 32) - 1) || (low > pow(2, 32) - 1)) {
    	rangeErr();
    }

    if(low > high) {
    	rangeErr();
    }

    if(low < 0 || high < 0) {
    	rangeErr();
    }

    int fdInput = open(inFile, O_RDONLY);
    if(fdInput < 0) {
    	fprintf(stderr, "Error: Cannot open file %s\n", inFile);
		exit(1);
    }

    int fdOutput = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    if (fdOutput < 0) {
    	fprintf(stderr, "Error: Cannot open file %s\n", outFile);
		exit(1);
    }

    struct stat fileStat;
    fstat(fdInput, &fileStat);
    int size = fileStat.st_size;
    // int numByteRecords = size/(sizeof(rec_t));
    rec_t *collection = (rec_t *)malloc(size);

    // reading file
    int count = 0;
    while (1) {	
		int rc;
		rc = read(fdInput, &(collection[count]), sizeof(rec_t));
		// if EOF
		if (rc == 0)
	    	break;
		if (rc < 0) {
	    	fprintf(stderr, "Failed reading file!\n");
	    	free(collection);
	    	close(fdInput);
	    	exit(1);
		}
		count ++;
	}

	qsort(collection, count, sizeof(rec_t), compareKey);

	int countOut = 0;
	for(countOut = 0; countOut < count; countOut ++) {
		if(collection[countOut].key <= high && collection[countOut].key >= low)
			write(fdOutput, &collection[countOut], sizeof(rec_t));
	}

    free(collection);
    close(fdInput);
    close(fdOutput);
    return 0;
}

