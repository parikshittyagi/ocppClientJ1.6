rm -rf *.o

gcc -o ocpp_ws_client.o -c ocpp_ws_client.c

gcc -o utils.o -c utils.c

gcc -o cJSON.o -c cJSON.c

gcc -o ocpp_helper.o -c ocpp_helper.c

gcc -o ocpp_process.o -c ocpp_process.c

gcc -o test main.c utils.o ocpp_ws_client.o cJSON.o ocpp_helper.o ocpp_process.o -lpthread

