CC = gcc
CFLAGS = -g -Wall -I$(INC_DIR)
INC_DIR = include
OBJ_DIR = obj

SERVER = server
SRV_SOURCE = server.c inet.c process.c
SRV_HEADER = $(patsubst %.c, $(INC_DIR)/%.h, $(SRV_SOURCE))
SRV_OBJECT = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRV_SOURCE))

ATM_SERVER = atm
ATM_SOURCE = atm.c process.c str.c
ATM_HEADER = $(patsubst %.c, $(INC_DIR)/%.h, $(ATM_SOURCE))
ATM_OBJECT = $(patsubst %.c, $(OBJ_DIR)/%.o, $(ATM_SOURCE))

CLIENT = client
CLI_SOURCE = client.c inet.c
CLI_HEADER = $(patsubst %.c, $(INC_DIR)/%.h, $(CLI_SOURCE))
CLI_OBJECT = $(patsubst %.c, $(OBJ_DIR)/%.o, $(CLI_SOURCE))

all: dirs $(SERVER) $(ATM) $(CLIENT)

dirs:
		mkdir -p obj

$(CLIENT): $(CLIENT_SOURCE)
		$(CC) -o $@ $^ $(CFLAGS)

$(SERVER): $(SRV_OBJECT) $(SRV_HEADER)
		$(CC) -o $@ $^ server.c $(CFLAGS)

$(ATM): $(ATM_OBJECT) $(ATM_HEADER)
		$(CC) -o $@ $^ ras.c $(CFLAGS)

$(OBJ_DIR)/%.o: %.c $(INC_DIR)/%.h
		$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm server
	rm ras
	rm client
