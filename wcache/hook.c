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
#include "hook.h"

unsigned int watch_in(void *priv,
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
            printk("source: %d.%d.%d.%d destination: %d.%d.%d.%d \n", NIPQUAD(sip), NIPQUAD(dip));
            printk("id:%d", iph->id);
        }
            // printk("prePacket for source: %d.%d.%d.%d destination: %d.%d.%d.%d ", NIPQUAD(sip), NIPQUAD(dip));
    }
    return NF_ACCEPT;
}

unsigned int watch_out(void *priv,
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
        if (sip == htonl(SERVER_ADDR)&&dip == htonl(CLIENT_ADDR)){
            printk("source: %d.%d.%d.%d destination: %d.%d.%d.%d \n", NIPQUAD(sip), NIPQUAD(dip));
            printk("id:%d", iph->id);
        }
            // printk("prePacket for source: %d.%d.%d.%d destination: %d.%d.%d.%d ", NIPQUAD(sip), NIPQUAD(dip));
            // return NF_DROP;
    }
    return NF_ACCEPT;
}

struct nf_hook_ops pre_hook = {
    .hook = watch_in,
    .pf = PF_INET,
    .hooknum = NF_INET_POST_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};
struct nf_hook_ops post_hook = {
    .hook = watch_out,
    .pf = PF_INET,
    .hooknum = NF_INET_PRE_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};