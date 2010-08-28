#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <net/net_namespace.h>
#include "netlink-echo.h"

struct sock *sk;

/*
 * Process incomming packets.
 */
int echo_proccess(struct sk_buff *skb, struct nlmsghdr *hdr){

        struct sk_buff *repskb;
        char *msg = NLMSG_DATA(hdr);

	/* 
	 *  Create a private copy of the skb to send as echo reply.
	 */
        repskb = skb_copy(skb, GFP_KERNEL);
        if(repskb == NULL){
                printk(KERN_WARNING "netlink echo: out of memory");
                return -ENOMEM;
        }

        msg[hdr->nlmsg_len - NLMSG_HDRLEN - 1] = '\0';
        printk(KERN_INFO "str: %s\n", msg);

	/*
	 * Send the echo reply.
	 */
        if(netlink_unicast(sk, repskb, NETLINK_CREDS(skb)->pid, 0) < 0){
                printk(KERN_WARNING "netlink echo: unicast fail!!!");
		return -EINVAL;
	}

        return 0;

}

//Callback: receive packets and call netlink_rcv_skb to unpack and process them.
void netlink_echo_input(struct sk_buff *skb){

        netlink_rcv_skb(skb, echo_proccess);

}

int netlink_echo_init(void){

        printk(KERN_INFO "netlink echo loaded\n");
	//Create the netlink subsystem NETLINK_ECHO and register
	//the callback to receive packets.
        sk = netlink_kernel_create(&init_net, NETLINK_ECHO, 1,
					netlink_echo_input, NULL,
					THIS_MODULE);
        if(sk == NULL)
                return -1;
        return 0;

}

void netlink_echo_exit(void){

        sock_release(sk->sk_socket);
        printk(KERN_INFO "netlink echo: bye!\n");

}

MODULE_LICENSE("GPL");

module_init(netlink_echo_init);
module_exit(netlink_echo_exit);

