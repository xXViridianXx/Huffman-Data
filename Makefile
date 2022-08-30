CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra -g
HEADERS = $(wildcard *.h)
MODULES = code node io pq stack huffman
OBJECTS_ENC = $(ENCODE).o $(patsubst %, %.o, ${MODULES})
OBJECTS_DEC = $(DECODE).o $(patsubst %, %.o, ${MODULES})
ENCODE = encode
DECODE = decode


.PHONY: all done

all: ENCODE DECODE 



ENCODE: $(ENCODE)

DECODE: $(DECODE)

$(ENCODE): $(OBJECTS_ENC)
	$(CC) -o $@ $^

$(DECODE): $(OBJECTS_DEC)
	$(CC) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -c 

clean: tidy
	$(RM) $(ENCODE) $(DECODE)

tidy:
	$(RM) $(OBJECTS_ENC) $(OBJECTS_DEC)

format:
	clang-format -i -style=file *.[c,h]

