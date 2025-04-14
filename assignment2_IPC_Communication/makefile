CC = gcc

EXEC1 = q_1
EXEC2 = q_2
EXEC3 = q_3
EXEC4 = bonus

SRC1 = q_1.c
SRC2 = q_2.c
SRC3 = q_3.c
SRC4 = bonus.c

all: $(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4)

q_1: $(SRC1)
	$(CC) -o $(EXEC1) $(SRC1)
	
q_2: $(SRC2)
	$(CC) -o $(EXEC2) $(SRC2)
	
q_3: $(SRC3)
	$(CC) -o $(EXEC3) $(SRC3)
	
bonus: $(SRC4)
	$(CC) -o $(EXEC4) $(SRC4)
	
clean:
	rm -f $(EXEC1) $(EXEC2) $(EXEC3) $(EXEC4)
