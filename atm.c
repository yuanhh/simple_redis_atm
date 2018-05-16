#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <hiredis/hiredis.h>

#include "include/process.h"
#include "include/str.h"

void init();
void save();
void load();
void remit();
int process_cmd(char *cmd);
void redis_connect();
void redis_free();

redisContext *c;

int main()
{
	char *cmd = NULL;
	size_t len = 0;
    ssize_t nread;
    int status = 0;

    redis_connect();
    if (!c)
        return -1;

    for (;;) {
		nread = getline(&cmd, &len, stdin);

        status = process_cmd(cmd);
        if (status) break;
    }
	free(cmd);
    redis_free();

    return 0;
}

int process_cmd(char *cmd)
{
    char **cmd_split;
    int tok_count = 0;
    int exit = 0;

    cmd_split = strtok_bychar(cmd, ' ', &tok_count);

    if (strcmp(cmd_split[0], "init") == 0) {
        if (tok_count == 3)
            init(cmd_split[1], cmd_split[2]);
    } else if (strcmp(cmd_split[0], "save") == 0) {
        if (tok_count == 3)
            save(cmd_split[1], cmd_split[2]);
    } else if (strcmp(cmd_split[0], "load") == 0) {
        if (tok_count == 3)
            save(cmd_split[1], cmd_split[2]);
    }
    /*else if (strcmp(cmd_split[0], "remit") == 0) {
        if (tok_count == 4)
            remit(cmd_split[1], cmd_split[2]);
    }*/
    else if (strcmp(cmd_split[0], "end") == 0) {
        exit = 1;
    }

    free_2Darray(&cmd_split, tok_count);
    return exit;
}

void redis_connect(void)
{
    c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
    }
}

void redis_free(void)
{
    redisFree(c);
}

void init(char *account, int money)
{
    redisReply *reply;

    reply = redisCommand(c,"SET %s %d", account, money);
    freeReplyObject(reply);

    freeReplyObject(reply);
}

void save(char *account, int money)
{
    redisReply *reply;

    reply = redisCommand(c,"INCRBY %s %d", account, money);
    freeReplyObject(reply);

    freeReplyObject(reply);
}

void load(char *account, int money)
{
    redisReply *reply;

    reply = redisCommand(c,"DECRBY %s %d", account, money);
    freeReplyObject(reply);

    freeReplyObject(reply);
}

/*void remit(char *src_account, char *dst_account, int money)
{
    redisReply *reply;

    reply = redisCommand(c,"SET %s %d", key, value);
    freeReplyObject(reply);

    freeReplyObject(reply);
} */