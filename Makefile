SERVER = ftpserver
CLIENT = myftp

# 忽略警告
NOWARN = -w
# 不显示执行的命令
NOECHO = @

all: $(CLIENT) $(SERVER)

# 客户端程序
$(CLIENT): client.c msg.o cmd.o cmd_client.o file.o
	$(NOECHO)gcc $(NOWARN) -o $(CLIENT) client.c msg.o cmd.o cmd_client.o file.o
	@echo 客户端程序$(CLIENT)创建完成

# 服务器程序
$(SERVER): server.c msg.o cmd_server.o file.o
	$(NOECHO)gcc $(NOWARN) -o $(SERVER) server.c msg.o cmd_server.o file.o
	@echo 服务器程序$(SERVER)创建完成

cmd.o: cmd.c cmd.h
	$(NOECHO)gcc $(NOWARN) -c cmd.c

cmd_client.o: cmd_client.c cmd.o header.h
	$(NOECHO)gcc $(NOWARN) -c cmd_client.c

cmd_server.o: cmd_server.c cmd.o header.h
	$(NOECHO)gcc $(NOWARN) -c cmd_server.c

file.o: file.c file.h header.h
	$(NOECHO)gcc $(NOWARN) -c file.c

msg.o: msg.c msg.h header.h
	$(NOECHO)gcc $(NOWARN) -c msg.c

clean:
	$(NOECHO)rm -f cmd.o cmd_client.o cmd_server.o
	$(NOECHO)rm -f file.o msg.o
	$(NOECHO)rm -f $(CLIENT) $(SERVER)
	$(NOECHO)rm -f test test.o
	@echo 编译产生的文件已清理