NAME	= ttysnake
SRC		= $(wildcard *.c)

CFLAGS	= -c -g
LDFLAGS = -lcurses -g

CC		= cc
LD		= cc

OBJ		= $(SRC:.c=.o)

$(NAME): $(OBJ)
	$(LD) -o $@ $(LDFLAGS) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(OBJ) $(NAME)
