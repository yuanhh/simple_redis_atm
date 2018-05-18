CC = gcc
CFLAGS = -g -Wall -I$(INC_DIR)
REDIS_FLAG = $(shell pkg-config --libs hiredis)
INC_DIR = include
OBJ_DIR = obj

SERVER = server
SRV_SOURCE = server.c inet.c process.c
SRV_OBJECT = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRV_SOURCE))

ATM = atm
ATM_SOURCE = atm.c process.c str.c
ATM_OBJECT = $(patsubst %.c, $(OBJ_DIR)/%.o, $(ATM_SOURCE))

CLIENT = client
CLI_SOURCE = client_2.c inet.c
CLI_OBJECT = $(patsubst %.c, $(OBJ_DIR)/%.o, $(CLI_SOURCE))

all: dirs $(SERVER) $(ATM) $(CLIENT)

dirs:
		mkdir -p obj

$(CLIENT): $(CLI_OBJECT)
		$(CC) -o $@ $^ $(CFLAGS)

$(SERVER): $(SRV_OBJECT)
		$(CC) -o $@ $^ $(CFLAGS)

$(ATM): $(ATM_OBJECT)
		$(CC) -o $@ $^ $(CFLAGS) $(REDIS_FLAG)

$(OBJ_DIR)/%.o: %.c
		$(CC) -o $@ -c $< $(CFLAGS) $(REDIS_FLAG)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm server
	rm atm
	rm client
