
SOURCES = \
	test_echo_client.c \
	server_test.c


all:
	gcc test_echo_client.c -o test_echo_client
	gcc server_test.c -o server_test

clean:
	rm server_test test_echo_client

