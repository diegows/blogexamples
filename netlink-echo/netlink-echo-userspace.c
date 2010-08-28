#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include "netlink-echo.h"

#define NTEST 100

int main(int argc, char *argv[]) {
        char *msg, *reply_msg;
        int sock;
        struct nlmsghdr *nlmsg, *reply_nlmsg;
        struct sockaddr_nl nladdr;
        int msglen;
	int ret;
        char reply[512]; 

        if (argc != 2) {
                printf("Error de syntaxis\n");
                return -1;
        }               

	msg = argv[1];

	sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ECHO);
	if (sock < 0){
		perror("Error creating netlink socket");
		return -1;
	}

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pid = 0;

	if (bind(sock, (struct sockaddr *)&nladdr, sizeof(nladdr)) < 0) {
		perror("Bind error");
                return -1;
	}

	if (connect(sock, (struct sockaddr *)&nladdr, sizeof(nladdr)) < 0) {
		perror("Connect error");
		return -1;
	}

	msglen = strlen(msg) + 1;

	nlmsg = malloc(NLMSG_HDRLEN + msglen);
	if(nlmsg == NULL){
		printf("Malloc fail.");
		return -1;
	}

	nlmsg->nlmsg_len = NLMSG_LENGTH(msglen);
	nlmsg->nlmsg_pid = 0;
	nlmsg->nlmsg_flags |= NLM_F_REQUEST;
	nlmsg->nlmsg_type = 0x11;
	memcpy(NLMSG_DATA(nlmsg), msg, msglen);

	if(send(sock, nlmsg, nlmsg->nlmsg_len, 0) < 0){
		perror("Send error");
		return -1;
	}

	ret = recv(sock, reply, sizeof(reply), 0);
	if(ret < 0){
		perror("Recv error");
		return -1;
	}	

	reply_nlmsg = (struct nlmsghdr *)reply;
	reply_msg = NLMSG_DATA(reply_nlmsg);	
	printf("%s\n", reply_msg);

        return 0;
}

