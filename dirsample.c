#define _SVID_SOURCE
#define _GNU_SOURCE
#define __USE_BSD
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>


/**
 *  String for the filter function (see below).
**/
char *filter_string;


/**
 *  Match filter_string case sensitive.
**/
char case_sensitive;


/**
 *  Filter function using a previously set string.
 *  Returns 1 if the name of the directory entry contains the string at
 *  char *filter_string.  If this is not the case or if filter_string is NULL,
 *  0 is returned.
**/
int filter_by_string(const struct dirent *entry)
{
	extern char *filter_string;
	extern char case_sensitive;
	if (filter_string != NULL) {
		if (case_sensitive) {
			if (strstr(entry->d_name, filter_string) == NULL) {
				return 0;
			}
		} else {
			if (strcasestr(entry->d_name, filter_string) == NULL) {
				return 0;
			}
		}
	}
	return 1;
}


/**
 *  Returns 1 if entry is a directory, 0 otherwise.
**/
int filter_dir(const struct dirent *entry)
{
	if (entry->d_type == DT_DIR) {
		return 1;
	}
	return 0;
}


/**
 *  Prints the names of directory entries in the a specified directory, filtered
 *  with a given function.
 *  The directory name is given by dir_name.  filter is a pointer to a filtering
 *  function that takes a directory entry struct and returns 1 or 0, according 
 *  to whether the entry should be printed or not.
**/
void print_sample_entry(char *dir_name, int (*filter)(const struct dirent*))
{
	struct dirent **namelist;
	int num = scandir(dir_name, &namelist, filter, alphasort);

	// if dir_name could not be read there are no matching entries
	if (num <= 0) {
		return;
	}

	int o_index = 0;
	for (int i = 0; i < num; i++) {
		if (i % 7 == 0) {
			o_index = i;
		}
	}

	printf("%s/%s\n", dir_name, namelist[o_index]->d_name);

	// clean up
	for (int i = 0; i < num; i++) {
		free(namelist[i]);
	}
	free(namelist);
}


/**
 * Just print general usage information.
 */
void usage(void) {
	printf("\n");
	printf("Usage: dirsample [OPTIONS] DIRECTORIES\n\n");

	printf("  Print the path of one item from each directory in DIRECTORIES.\n");
	printf("  How samples are taken can be changed with OPTIONS.\n\n");

	printf("\
Options:\n\
  -p [PATTERN]    Print only items including PATTERN in their names.\n\
  -i              Make pattern matching case insensitive.\n\
  -h              Print this help message.\n\n\
");
}


int main(int argc, char** argv)
{
	extern char *filter_string;
	extern char case_sensitive;

    //
	// Command line options
	//
	int opt = 0;

	// Turn case sensitivity on by default.
	case_sensitive = 1;

	while ((opt = getopt(argc, argv, "hip:")) != -1) {
		switch (opt) {
		case 'i':
			case_sensitive = 0;
			break;
		case 'p':
			filter_string = optarg;
			break;
		case 'h':
			usage();
			exit(0);			
		default:
			break;
		}
	}

	//
	// Loop over all remaining command line tokens.
	//
	for (int i = optind; i<argc; i++) {
		print_sample_entry(argv[i], filter_by_string);
	}

	return 0;
}