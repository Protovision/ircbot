#include "irc.h"

strmap_t factoids;
FILE *factoid_file;

void init_factoids() {
	char line[512];

	strmap_init(&factoids);

	factoid_file = fopen("factoids", "r");
	if (factoid_file == NULL) return;
	
	while (fgets(line, 512, factoid_file)) {
		strmap_insert(&factoids, line, line + strlen(line) + 1);
	}
	fclose(factoid_file);
}

int write_factoid(const char *key, const char *value) {
	fwrite(key, 1, strlen(key) + 1, factoid_file);
	fwrite(value, 1, strlen(value) + 1, factoid_file);
	fputc('\n', factoid_file);
	return 0;
}

void destroy_factoids() {
	factoid_file = fopen("factoids", "w");
	if (factoid_file == NULL) return;

	strmap_iterate(&factoids, write_factoid);
	strmap_destroy(&factoids);
	fclose(factoid_file);
}

char *factoid_set(array *args, struct irc_message *msg) {
	size_t i;
	char value[512];
	char *s;

	if (array_count(args) < 2) return NULL;

	join_string(args, 1, value, " ");
/*
	value[0] = 0;
	for (i = 1; i != array_count(args); ++i) {
		s = *(char**)array_at(args, i);
		strcat(value, s);
		strcat(value, " ");
	}
*/
	
	strmap_insert(&factoids, *(char**)array_at(args, 0), value);
	return NULL;
}

char *factoid_get(array *args, struct irc_message *msg) {
	const char *value;

	if (array_count(args) < 1) return NULL;
	value = strmap_find(&factoids, *(char**)array_at(args, 0));
	if (value == NULL) return NULL;
	return new_string(value);
}


char *shell(array *args, struct irc_message *msg) {
	char dope[512];
	int hookah[6];	
	ssize_t readbytes;
	pid_t pothead;
	char *gram;

	if (array_count(args) < 1) return NULL;
	join_string(args, 0, dope, " ");	
	
	pipe(&hookah[0]);
	pipe(&hookah[2]);
	pipe(&hookah[4]);

	pothead = fork();

	if (pothead == 0) {
		dup2(hookah[0], 0); close(hookah[1]);
		dup2(hookah[3], 1); close(hookah[2]);
		dup2(hookah[5], 2); close(hookah[4]);
		execl("/bin/bash", "/bin/bash", "-s", NULL);	
		exit(0);
	}
	close(hookah[0]);
	close(hookah[3]);
	close(hookah[5]);
	
	write(hookah[1], dope, strlen(dope));
	close(hookah[1]);
	memset(dope, 0, 512);
	for (gram = dope; (readbytes = read(hookah[2], gram, dope + 512 - gram)) > 0; gram += readbytes);

	for (gram = dope; *gram; ++gram) {
		if (*gram == '\n' || *gram == '\r') *gram = ' ';
	}

	return new_string(dope);	
}


char *irc_join(array *args, struct irc_message *msg) {
	fprintf(irc, "JOIN %s\r\n", *(char**)array_at(args, 0));
	return NULL;	
}

char *irc_part(array *args, struct irc_message *msg) {
	fprintf(irc, "PART %s :%s\r\n", *(char**)array_at(args, 0), *(char**)array_at(args, 1));
	return NULL;
}

char *irc_say(array *args, struct irc_message *msg) {
	int i;
	if (array_count(args) < 2) return NULL;
	fprintf(irc, "PRIVMSG %s :", *(char**)array_at(args, 0));
	for (i = 1; i < array_count(args); ++i) {
		fprintf(irc, "%s ", *(char**)array_at(args, i));
	}		
	fputs("\r\n", irc);
	return NULL;	
}

char *irc_quit(array *args, struct irc_message *msg) {
	fprintf(irc, "QUIT :%s\r\n", *(char**)array_at(args, 0));
	return NULL;
}





struct bot_command bot_cmds[] = {
	{ "@join", irc_join },
	{ "@part", irc_part },
	{ "@say", irc_say },
	{ "@quit", irc_quit },
	{ "set", factoid_set },
	{ "get", factoid_get },
	{ "@shell", shell },
	{ NULL, NULL }
};

void (*command_init_funcs[])() = {
	init_factoids,
	NULL	
};

void (*command_cleanup_funcs[])() = {
	destroy_factoids,
	NULL
};

void init_bot_commands() {
	size_t i;
	for (i = 0; command_init_funcs[i] != NULL; ++i)
		command_init_funcs[i]();
}

void cleanup_bot_commands() {
	size_t i;
	for (i = 0; command_cleanup_funcs[i] != NULL; ++i)
		command_cleanup_funcs[i]();
}
