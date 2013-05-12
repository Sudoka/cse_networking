RM := rm -f

targets := server_test test_echo_client tftpserver tftpclient

all:
	gcc -o tftpserver tftpserver.c tftp-packet.c
	gcc -o tftpclient tftpclient.c tftp-packet.c
	gcc test_echo_client.c -o test_echo_client
	gcc server_test.c -o server_test

clean:
	$(RM) $(targets)

