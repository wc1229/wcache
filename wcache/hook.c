#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter.h>
#include <net/tcp.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/ip.h>
#include "hook.h"
#include "rb_tree.h"
#include "wcache.h"

bool wcache_skb = false;

unsigned int watch_in(void *priv,
                             struct sk_buff *skb,
                             const struct nf_hook_state *state) {
    if(skb){
        struct iphdr *iph;
        struct tcphdr *tcph;
        struct tcphdr *temp_tcph;
        // int i=0;
        int header=0;
        char *data=NULL;
        char *url_start;
        char *url_end;
        int length=0;

        iph  = ip_hdr(skb);
        if ( iph->saddr== htonl(CLIENT_ADDR) &&iph->daddr == htonl(SERVER_ADDR) ){
            printk("source: %d.%d.%d.%d destination: %d.%d.%d.%d \n", NIPQUAD(iph->saddr), NIPQUAD(iph->daddr));
            // printk("id:%d", iph->id);
            tcph=tcp_hdr(skb);
            data=skb->data+iph->ihl*4+tcph->doff*4;//数据起始地址
            header=iph->ihl*4+tcph->doff*4;
            length=skb->len- header;
            printk("seq is %d.%d.%d.%d",NIPQUAD(tcph->seq));
            printk("ack_seq is %d.%d.%d.%d",NIPQUAD(tcph->ack_seq));
            // printk("ack_seq+1 is %d",ntohl(tcph->ack_seq));
            if(length>0){
                printk("**************start_data*****************\n");
                printk("header length is %d",header);
                printk("data length is %d",length);
                printk("source is %d",tcph->source);
                printk("dest is %d",tcph->dest);
                if(skb->data_len!=0){//非线性数据长度       
                    if(skb_linearize(skb)){
                        printk("error line skb\r\n");
                        printk("skb->data_len %d\r\n",skb->data_len);
                        return NF_DROP;
                    }
                    iph  = ip_hdr(skb);
                    tcph=tcp_hdr(skb);
                    data=skb->data+iph->ihl*4+tcph->doff*4;//数据起始地址
                    header=iph->ihl*4+tcph->doff*4;
                    length=skb->len- header;
                }
                // 判断是否为 GET 请求
                if (strncmp(data, "GET ", 4) != 0){
                    printk("strncmp(data, GET , 4) != 0");
                    return NF_ACCEPT;
                }
                // 查找 URL 的起始位置和结束位置
                url_start = data + 4;
                url_end = strchr(url_start, ' ');
                if (url_end == NULL){
                    printk("url_end == NULL");
                    return NF_ACCEPT;
                }
                if(!temp_object)obj_start_create(url_start, (int)(url_end - url_start));
                else{
                    temp_tcph = tcp_hdr(temp_object->skb);
                    temp_tcph->seq = tcph->ack_seq;
                    temp_tcph->ack_seq = htonl(ntohl(tcph->seq)+length);
                    temp_tcph->source = tcph->dest;
                    temp_tcph->dest = tcph->source;
                }
                // 打印 URL
                printk("HTTP request URL: %.*s\n", (int)(url_end - url_start), url_start);
                printk("%.*s", length, data);
                printk("****************end_data*****************\n");

            }
        }
    }
    return NF_ACCEPT;
}

unsigned int watch_out(void *priv,
                              struct sk_buff *skb,
                              const struct nf_hook_state *state) {
    if(skb){
        struct sk_buff *copy_skb = NULL;
        struct iphdr *iph;
        struct tcphdr *tcph;
        // int i=0;
        int header=0;
        unsigned char *data=NULL;
        int length=0;
        struct sk_buff *myskb;
        struct iphdr *myiph;
        __wsum csum=0;
        struct tcphdr *mytcphdr;
        unsigned int tcphoff;

        iph  = ip_hdr(skb);
        if ( iph->saddr== htonl(SERVER_ADDR) &&iph->daddr == htonl(CLIENT_ADDR) ){
            printk("source: %d.%d.%d.%d destination: %d.%d.%d.%d \n", NIPQUAD(iph->saddr), NIPQUAD(iph->daddr));
            // printk("id:%d", iph->id);
            tcph=tcp_hdr(skb);
            printk("seq is %d.%d.%d.%d",NIPQUAD(tcph->seq));
            printk("ack_seq is %d.%d.%d.%d",NIPQUAD(tcph->ack_seq));
            data=skb->data+iph->ihl*4+tcph->doff*4;//数据起始地址
            header=iph->ihl*4+tcph->doff*4;
            length=skb->len- header;
            if(length>0){
                printk("**************start_data*****************\n");
                printk("header length is %d",header);
                printk("data length is %d",length);
                printk("source is %d",tcph->source);
                printk("dest is %d",tcph->dest);
                printk("TTL is %d",iph->ttl);
                if(skb->data_len!=0){//非线性数据长度       
                    if(skb_linearize(skb)){
                        printk("error line skb\r\n");
                        printk("skb->data_len %d\r\n",skb->data_len);
                        return NF_DROP;
                    }
                    iph  = ip_hdr(skb);
                    tcph=tcp_hdr(skb);
                    data=skb->data+iph->ihl*4+tcph->doff*4;//数据起始地址
                    header=iph->ihl*4+tcph->doff*4;
                    length=skb->len- header;
                }
                printk("%.*s", length, data);
                printk("****************end_data*****************\n");
                if(temp_object->skb){
                    // if(wcache_skb){
                    //     wcache_skb = false;
                    //     return NF_ACCEPT;
                    // }
                    // myskb=skb_copy(temp_object->skb,GFP_ATOMIC);
                    // myiph=(struct iphdr*)skb_network_header(myskb);
                    // mytcphdr=(struct tcphdr*)(myskb->data+myiph->ihl*4);
                    // // nf_reset(myskb);
                    // tcphoff=myiph->ihl*4;
                    // ip_send_check(myiph);//检验和
                    // mytcphdr->check=0;
                    // csum=skb_checksum(myskb,tcphoff,myskb->len-tcphoff,0);
                    // myskb->csum=csum;
                    // myskb->ip_summed=CHECKSUM_NONE;
                    // /*mytcphdr->check=csum_tcpudp_magic(myiph->saddr,myiph->daddr,myskb->len,IPPROTO_TCP,csum_partial((char *)mytcphdr,myskb->len-tcphoff,0));
                    // mytcphdr->check+=0x1400;*/
                    // mytcphdr->check=0;
                    // mytcphdr->check=tcp_v4_check(myskb->len-tcphoff,myiph->saddr,myiph->daddr,csum_partial(mytcphdr,myskb->len-tcphoff,0));
                    // netif_rx(myskb);
                    // printk("postPacket wc");
                    // wcache_skb = true;
                    // // skb_delete(temp_object);
                    // // temp_object = NULL;
                    // return NF_DROP;
                }else{
                    printk("skb->truesize is %d",skb->truesize);
                    copy_skb = skb_copy(skb, GFP_KERNEL);
                    if(!temp_object->skb)obj_end_create(copy_skb, (size_t)skb->truesize);
                }
            }
        }
    }
    return NF_ACCEPT;
}

struct nf_hook_ops pre_hook = {
    .hook = watch_in,
    .pf = PF_INET,
    .hooknum = NF_INET_PRE_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};

struct nf_hook_ops post_hook = {
    .hook = watch_out,
    .pf = PF_INET,
    .hooknum = NF_INET_POST_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};
