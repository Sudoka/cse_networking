RM := rm -f

targets := server_test test_echo_client tftpserver tftpclient CLIENT/tftpclient SERVER/tftpserver

all:
	gcc -g -o tftpserver tftpserver.c tftp.c
	gcc -g -o tftpclient tftpclient.c tftp.c
	gcc test_echo_client.c -o test_echo_client
	gcc server_test.c -o server_test
	cp tftpserver SERVER/
	cp tftpclient CLIENT/

clean:
	$(RM) $(targets)

