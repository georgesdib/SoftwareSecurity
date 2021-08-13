attack: aes_core.o attack.o cbc-decrypt.o
	gcc -o attack attack.o cbc-decrypt.o aes_core.o; rm attack.o cbc-decrypt.o aes_core.o && ./attack challenge-ciphertext.txt challenge-plaintext.txt

cbc-encrypt: aes_core.o cbc-encrypt.o
	gcc -o cbc-encrypt cbc-encrypt.o aes_core.o; rm cbc-encrypt.o aes_core.o

%.o: %.c
	gcc -g -c $^

sample: sample.o oracle.o
	gcc -o sample oracle.o sample.o; rm sample.o oracle.o

server: server.o cbc-decrypt.o aes_core.o
	gcc -o server server.o cbc-decrypt.o aes_core.o; rm server.o cbc-decrypt.o aes_core.o

clean:
	rm -rf *.o sample server cbc-encrypt attack
