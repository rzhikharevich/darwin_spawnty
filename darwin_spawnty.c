#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <spawn.h>
#include <sys/wait.h>

typedef struct {
	const char *key;
	int value;
} str2int_t;

const str2int_t str2int[] = {
	{"normal",             0x00000000},
	{"default",            0x00000000},
	{"app_default",        0x00000100},
	{"app_tal",            0x00000200},
	{"daemon_standard",    0x00000300},
	{"daemon_interactive", 0x00000400},
	{"daemon_background",  0x00000500},
	{"daemon_adaptive",    0x00000600}
};

int posix_spawnattr_setprocesstype_np(posix_spawnattr_t *spattr, int type);

int main(int argc, char *argv[], char *env[]) {
	if (argc < 3) {
		fputs(
			"Usage: darwin_spawnty <proctype> <argv...>\n"
			"\n"
			"Available process types: \n"
			"  normal, default,\n"
			"  app_{default,tal},\n"
			"  daemon_{standard,interactive,background,adaptive}\n"
			"\n"
			"See launchd documentation for more information.\n",
			stderr);
			
		return 1;
	}
	
	int err;
	
	const char *type_key = argv[1];
	const char *path = argv[2];
	
	posix_spawnattr_t attr;
	posix_spawnattr_init(&attr);
	
	int type = -1;
	
	for (size_t i = 0; i < sizeof(str2int) / sizeof(str2int[0]); i++) {
		if (!strcmp(type_key, str2int[i].key)) {
			type = str2int[i].value;
		}
	}
	
	if (type < 0) {
		fputs("Error: Unrecognized process type.\n", stderr);
		return 1;
	}
	
	if ((err = posix_spawnattr_setprocesstype_np(&attr, type))) {
		fprintf(
			stderr,
			"Error: posix_spawnattr_setprocesstype_np failed: %s\n",
			strerror(err));
			
		return 1;
	}
	
	if ((err = posix_spawnp(NULL, path, NULL, &attr, argv + 2, env))) {
		fprintf(
			stderr,
			"Error: posix_spawn failed: %s\n",
			strerror(err));
			
		return 1;
	}
	
	wait(NULL);
}
