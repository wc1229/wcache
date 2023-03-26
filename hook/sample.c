#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/ip.h>
//模块信息
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wc1229");
MODULE_DESCRIPTION("hook");

#define	CLIENT_ADDR		0xc0a87a01	/* 192.168.122.1   */
#define	SERVER_ADDR		0xc0a87ade	/* 192.168.122.222   */
#define	LO_ADDR		0x7f000001	/* 127.0.0.1   */

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
        if (sip == htonl(CLIENT_ADDR) &&dip == htonl(SERVER_ADDR) )
            printk("prePacket for source: %d.%d.%d.%d destination: %d.%d.%d.%d ", NIPQUAD(sip), NIPQUAD(dip));
    }
    return NF_ACCEPT;
}

static unsigned int watch_out(void *priv,
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
        if (sip != htonl(LO_ADDR) )
        if (sip == htonl(SERVER_ADDR)&&dip == htonl(CLIENT_ADDR))
            printk("postPacket for source: %d.%d.%d.%d destination: %d.%d.%d.%d ", NIPQUAD(sip), NIPQUAD(dip));
            // return NF_DROP;
    }
    return NF_ACCEPT;
}

static struct nf_hook_ops pre_hook = {
    .hook = watch_in,
    .pf = PF_INET,
    .hooknum = NF_INET_POST_ROUTING,
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