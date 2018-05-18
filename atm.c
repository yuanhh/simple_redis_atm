#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <hiredis/hiredis.h>

#include "include/process.h"
#include "include/str.h"

#define BUFFERSIZE 1024

#define OK "OK\n"
#define NO "NO\n"
#define END "END\n"

struct transection {
    char key[BUFFERSIZE];
    char val[BUFFERSIZE];
};

int init(char *account, char *money);
int save(char *account, char *money);
int load(char *account, char *money);
int remit(char *src_account, char *dst_account, char *money);
int end(void);
int process_cmd(char *cmd);
void redis_connect();
void redis_free();
int redis_reply(redisReply *reply);
void string_tolower(char *str);
void print_transection(struct transection *table, size_t len);

redisContext *c;

int main()
{
	char buf[BUFFERSIZE];
    int status = 0;

    redis_connect();

    if (!c)
        return -1;

    for (;;) {
        memset(buf, 0, BUFFERSIZE);
		status = readLine(STDIN_FILENO, buf, BUFFERSIZE);
        if (status <= 0)
            break;

        status = process_cmd(buf);
        if (status)
            break;
    }
    redis_free();

    return 0;
}

int process_cmd(char *cmd)
{
    char **cmd_split;
    int tok_count = 0;
    int status = -1;

    cmd_split = strtok_bychar(cmd, ' ', &tok_count);

    if (strcmp(cmd_split[0], "init") == 0) {
        if (tok_count == 3)
            status = init(cmd_split[1], cmd_split[2]);
    } else if (strcmp(cmd_split[0], "save") == 0) {
        if (tok_count == 3)
            status = save(cmd_split[1], cmd_split[2]);
    } else if (strcmp(cmd_split[0], "load") == 0) {
        if (tok_count == 3)
            status = load(cmd_split[1], cmd_split[2]);
    } else if (strcmp(cmd_split[0], "remit") == 0) {
        if (tok_count == 4)
            status = remit(cmd_split[1], cmd_split[2], cmd_split[3]);
    } else if (strcmp(cmd_split[0], "end") == 0) {
        write(STDOUT_FILENO, END, strlen(END));
        end();
        return 1;
    }

    if (status == -1)
        write(STDOUT_FILENO, NO, strlen(NO));
    else
        write(STDOUT_FILENO, OK, strlen(OK));

    free_2Darray(&cmd_split, tok_count);
    return 0;
}

void redis_connect(void)
{
    c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            fprintf(stderr, "Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            fprintf(stderr, "Connection error: can't allocate redis context\n");
        }
    }
}

void redis_free(void)
{
    redisFree(c);
}

int redis_reply(redisReply *reply)
{
    if (!reply) {
        exit(EXIT_FAILURE);
    } else {
        switch (reply->type) {
            case REDIS_REPLY_ERROR:
                freeReplyObject(reply);
                return -1;
                break;
            case REDIS_REPLY_NIL:
                freeReplyObject(reply);
                return -1;
                break;
            default:
                break;
        }
    }
    return 0;
}

int init(char *account, char *money)
{
    redisReply *reply;
    int cash;

    cash = atoi(money);
    if (cash < 0)
        return -1;

    string_tolower(account);

    reply = redisCommand(c,"GET %s", account);
    if (reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(reply);
        reply = redisCommand(c,"SET %s %d", account, cash);
        if (redis_reply(reply) == -1) {
            return -1;
        }
    } else {
        return -1;
    }

    freeReplyObject(reply);
    return 0;
}

int save(char *account, char *money)
{
    redisReply *reply;
    int deposit, cash;

    cash = atoi(money);
    if (cash < 0)
        return -1;

    string_tolower(account);

    reply = redisCommand(c,"GET %s", account);
    if (redis_reply(reply) == -1) {
        return -1;
    }

    deposit = atoi(reply->str);
    freeReplyObject(reply);

    reply = redisCommand(c,"SET %s %d", account, deposit + cash);
    if (redis_reply(reply) == -1) {
        return -1;
    }
    freeReplyObject(reply);
    
    return 0;
}

int load(char *account, char *money)
{
    redisReply *reply;
    int deposit, cash;

    cash = atoi(money);
    if (cash < 0)
        return -1;

    string_tolower(account);

    reply = redisCommand(c,"GET %s", account);
    if (redis_reply(reply) == -1) {
        return -1;
    }

    deposit = atoi(reply->str);
    if (cash > deposit) {
        return -1;
    }
    freeReplyObject(reply);
    
    reply = redisCommand(c,"SET %s %d", account, deposit - cash);
    if (redis_reply(reply) == -1) {
        return -1;
    }
    freeReplyObject(reply);

    return 0;
}

int remit(char *src_account, char *dst_account, char *money)
{
    redisReply *src_reply, *dst_reply;
    int src_deposit, dst_deposit, cash;

    cash = atoi(money);
    if (cash < 0)
        return -1;

    string_tolower(src_account);
    string_tolower(dst_account);

    if (!strcmp(src_account, dst_account))
        return -1;

    src_reply = redisCommand(c,"GET %s", src_account);
    if (redis_reply(src_reply) == -1) {
        return -1;
    }
    dst_reply = redisCommand(c,"GET %s", dst_account);
    if (redis_reply(dst_reply) == -1) {
        return -1;
    }

    dst_deposit = atoi(dst_reply->str);
    src_deposit = atoi(src_reply->str);
    if (cash > src_deposit) {
        return -1;
    }
    freeReplyObject(src_reply);
    freeReplyObject(dst_reply);

    src_reply = redisCommand(c,"SET %s %d", src_account, src_deposit - cash);
    if (redis_reply(src_reply) == -1) {
        return -1;
    }
    freeReplyObject(src_reply);

    dst_reply = redisCommand(c,"SET %s %d", dst_account, dst_deposit + cash);
    if (redis_reply(dst_reply) == -1) {
        return -1;
    }
    freeReplyObject(dst_reply);

    return 0;
}

static int cmpstringp(const void *p1, const void *p2)
{
    const struct transection *a = p1;
    const struct transection *b = p2;

    char const *c1, *c2;
    int seq1, seq2;

    c1 = a->key;
    c2 = b->key;

    while (*c1 && *c2) {
        if (isalpha(*c1) && isalpha(*c2)) {
            if (*c1 > *c2)
                return 1;
            else if (*c1 < *c2)
                return -1;
            else {
                c1++;
                c2++;
            }
        } else if (isalpha(*c1)) {
            return 1;
        } else if (isalpha(*c2)) {
                return -1;
        } else {
            seq1 = atoi(c1);
            seq2 = atoi(c2);
            return seq1 - seq2;
        }
    }

    if (*c1)
        return 1;
    else if (*c2)
        return -1;
    else
        return 0;
}

int end(void)
{
    redisReply *reply, *reply2;
    struct transection *table = 0;
    size_t i;

    reply = redisCommand(c,"KEYS *");
    if (redis_reply(reply) == -1)
        return -1;
    
    table = calloc(reply->elements, sizeof(struct transection));
    for (i = 0; i < reply->elements; i++) {
        memset(table[i].key, 0, BUFFERSIZE);
        memset(table[i].val, 0, BUFFERSIZE);

        strncpy(table[i].key, reply->element[i]->str, reply->element[i]->len);
    }

    qsort(table, reply->elements, sizeof(struct transection), cmpstringp);

    for (i = 0; i < reply->elements; i++) {

        reply2 = redisCommand(c,"GET %s", table[i].key);
        if (redis_reply(reply2) == -1)
            return -1;

        strncpy(table[i].val, reply2->str, reply2->len);
        freeReplyObject(reply2);
    }

    print_transection(table, reply->elements);

    free(table);
    freeReplyObject(reply);

    return 0;
}

void string_tolower(char *str)
{
    int i;

    for (i = 0; str[i]; i++)
        str[i] = tolower(str[i]);
}

void print_transection(struct transection *table, size_t len)
{
    char msg[3000];
    size_t i;

    for (i = 0; i < len; i++) {
        memset(msg, 0, 3000);
        snprintf(msg, 3000, "%s : %s\n", 
                table[i].key, table[i].val);
        write(STDOUT_FILENO, msg, strlen(msg));
    }
}
