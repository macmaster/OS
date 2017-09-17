CC = gcc
CFLAGS = -g -Wall -std=gnu99 # -pedantic
SRC = yash.c List.c Job.c
BACKUP = $(SRC:.c=.c~)
OBJ = $(SRC:.c=.o)
MAIN = yash

all: $(MAIN)
	@echo compiled the yash shell...

$(MAIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJ)

# TESTS AND USER PROGRAMS
test: $(MAIN) $(TEST)
	./$(MAIN) $(UCODE) $(TEST)

format: 
	indent $(SRC) -nbad -bap -nbc -bbo -hnl -br -brs -brf -c33 -cd33 -ncdb -ce -ci4 \
	-cli1 -d0 -di1 -nfc1 -i8 -ip0 -l120 -lp -npcs -nprs -npsl -sai \
	-saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1
	$(RM) $(BACKUP)

clean:
	$(RM) $(MAIN) $(TEST) $(OBJ)

