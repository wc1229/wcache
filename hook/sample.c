#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/checksum.h>
#include <net/tcp.h>
#include <net/udp.h>

//模块信息
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wc1229");
MODULE_DESCRIPTION("hook");

#define	CLIENT_ADDR		0xc0a8a380	/* 192.168.122.1   */
#define	SERVER_ADDR		0xc0a8a38a	/* 192.168.122.222   */
#define	LO_ADDR		0x7f000001	/* 127.0.0.1   */

int count = 0;

#define NIPQUAD(addr) \
  ((unsigned char *)&addr)[0], \
  ((unsigned char *)&addr)[1], \
  ((unsigned char *)&addr)[2], \
  ((unsigned char *)&addr)[3]

static unsigned int watch_in(void *priv,
                             struct sk_buff *skb,
                             const struct nf_hook_state *state) {
    __be32 sip,dip;
    if(skb){
        struct sk_buff *sb = NULL;
        struct iphdr *iph;
        sb = skb;
        iph  = ip_hdr(sb);
        sip = iph->saddr;
        dip = iph->daddr;
        if (sip == htonl(CLIENT_ADDR) &&dip == htonl(SERVER_ADDR) ){
            printk("prePacket for source: %d.%d.%d.%d destination: %d.%d.%d.%d ", NIPQUAD(sip), NIPQUAD(dip));
        }
    }
    return NF_ACCEPT;
}

static unsigned int watch_out(void *priv,
                              struct sk_buff *skb,
                              const struct nf_hook_state *state) {
    __be32 sip,dip;
    struct sk_buff *myskb;
    struct iphdr *myiph;
    __wsum csum=0;
    struct tcphdr *mytcphdr;
    unsigned int tcphoff;

    if(skb){
        struct sk_buff *sb = NULL;
        struct iphdr *iph;
        struct tcphdr *tcph;
        sb = skb;
        iph  = ip_hdr(sb);
        tcph = tcp_hdr(skb);
        sip = iph->saddr;
        dip = iph->daddr;
        if (sip == htonl(SERVER_ADDR)&&dip == htonl(CLIENT_ADDR)){
            if(skb->len- iph->ihl*4-tcph->doff*4>0 && count==0){
                myskb=skb_copy(skb,GFP_ATOMIC);
                myiph=(struct iphdr*)skb_network_header(myskb);
                mytcphdr=(struct tcphdr*)(myskb->data+myiph->ihl*4);
                // nf_reset(myskb);
                tcphoff=myiph->ihl*4;
                ip_send_check(myiph);//检验和
                mytcphdr->check=0;
                csum=skb_checksum(myskb,tcphoff,myskb->len-tcphoff,0);
                myskb->csum=csum;
                myskb->ip_summed=CHECKSUM_NONE;
                /*mytcphdr->check=csum_tcpudp_magic(myiph->saddr,myiph->daddr,myskb->len,IPPROTO_TCP,csum_partial((char *)mytcphdr,myskb->len-tcphoff,0));
                mytcphdr->check+=0x1400;*/
                mytcphdr->check=0;
                mytcphdr->check=tcp_v4_check(myskb->len-tcphoff,myiph->saddr,myiph->daddr,csum_partial(mytcphdr,myskb->len-tcphoff,0));
                netif_rx(myskb);
                count++;
                printk("postPacket wc");
                return NF_DROP;
            }
            printk("postPacket for source: %d.%d.%d.%d destination: %d.%d.%d.%d ", NIPQUAD(sip), NIPQUAD(dip));
        }
    }
    return NF_ACCEPT;
}

static struct nf_hook_ops pre_hook = {
    .hook = watch_in,
    .pf = PF_INET,
    .hooknum = NF_INET_LOCAL_OUT,
    .priority = NF_IP_PRI_FIRST,
};
static struct nf_hook_ops post_hook = {
    .hook = watch_out,
    .pf = PF_INET,
    .hooknum = NF_INET_PRE_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};

static int __init sample_init(void) {
    nf_register_net_hook(&init_net, &pre_hook);
    nf_register_net_hook(&init_net, &post_hook);
    printk(KERN_INFO "init_hook\n");
    return 0;
}

static void __exit sample_exit(void) {
    nf_unregister_net_hook(&init_net, &pre_hook);
    nf_unregister_net_hook(&init_net, &post_hook);
    printk(KERN_INFO "cleanup_hook\n");
}

 module_init(sample_init);
 module_exit(sample_exit); 