//Autor: Renato Luis Postigo Avalos
//compilacion: g++ prg1.cpp -o prg1.exe -std=c++11 -lpthread

//send data
#include <iostream>
#include <thread>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAXSIZE 64

using namespace std;

struct msg_buf {
    long type;
    char text[MAXSIZE]  = "Hello, this message has been sent from a thread.";
};

void send_message(){
	int msqid;
    int msg_flag = IPC_CREAT | 0666;
    key_t key;
    msg_buf send_buffer;
    size_t buflen;

    key = 1234;

	msqid = msgget(key, msg_flag);

    //Message Type
    send_buffer.type = 1;
	cout << "Sending message: " << send_buffer.text << endl;
	//sbuf.text = "Hello, this message has been send from a thread.";

    buflen = strlen(send_buffer.text) + 1;

	msgsnd(msqid, &send_buffer, buflen, IPC_NOWAIT);
	
	cout << "Message sent\n";
}

void signhandler(int signum){
	if (signum == 2){
		cout << "Creating thread\n";
		thread t1 (send_message);
		t1.join();
		cout << "Thread done\n";
		exit(0);
	}
}

int main(){
	signal(SIGINT, signhandler);
	
	while (1){
		cout << "Waiting for signal (hint: kill -2 pid)\n";
		sleep(1);
	}
	return 0;
}
