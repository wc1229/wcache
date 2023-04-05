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

#define	CLIENT_ADDR_163		0xc0a8a301	/* 192.168.163.1   */
#define	SERVER_ADDR_163		0xc0a8a38a	/* 192.168.163.138   */
#define	CLIENT_ADDR		0xc0a8f803	/* 192.168.248.3   */
#define	SERVER_ADDR		0xc0a85b63	/* 192.168.91.99   */
#define	LO_ADDR		0x7f000001	/* 127.0.0.1   */

// static unsigned char dst_mac[] = {0xd4, 0xee, 0x07, 0x53, 0x3c, 0xd4}; //br-lan 的MAC地址
// static unsigned char src_mac[] = {0x00, 0xA8, 0xA2, 0x83, 0x86, 0xA3}; //wlan0-vxd 的MAC地址
static unsigned char app_data[]  = "HTTP/1.1 200 OK\r\n"
					   "Date: Sat, 01 Apr 2023 12:49:58 GMT\r\n"
					   "Server: Apache/2.4.6 (CentOS)\r\n"
					   "Last-Modified: Sat, 18 Mar 2023 06:52:04 GMT\r\n"
					   "ETag: \"17-5f72721b02c88\"\r\n"
					   "Accept-Ranges: bytes\r\n"
					   "Content-Length: 23\r\n"
					   "Keep-Alive: timeout=5, max=100\r\n"
					   "Connection: Keep-Alive\r\n"
					   "Content-Type: text/html; charset=UTF-8\r\n\r\n"
					   "<html>hello wc!</html>";

#define NIPQUAD(addr) \
  ((unsigned char *)&addr)[0], \
  ((unsigned char *)&addr)[1], \
  ((unsigned char *)&addr)[2], \
  ((unsigned char *)&addr)[3]

static unsigned int watch_in(void *priv,
                             struct sk_buff *skb,
                             const struct nf_hook_state *state) {
    if(skb){
        struct sk_buff *new_skb = alloc_skb(1024, GFP_ATOMIC);
        struct net_device *dev = dev_get_by_name(&init_net, "br0");
        unsigned char *p; 
        struct ethhdr *ethr;
        struct iphdr *iph, *old_iph;
        struct tcphdr *tcph, *old_tcph;
        int header=0;
        int length=0;
        char *data=NULL;
        char *url_start;
        char *url_end;
        
        if (!new_skb)
            return NF_ACCEPT;
        if (!dev) {
            kfree_skb(new_skb);
            return NF_ACCEPT;
        }
        
        old_iph = ip_hdr(skb);
        if (old_iph->saddr == htonl(CLIENT_ADDR)&&old_iph->daddr == htonl(SERVER_ADDR)){
            printk("postPacket for source: %d.%d.%d.%d destination: %d.%d.%d.%d ", NIPQUAD(old_iph->saddr), NIPQUAD(old_iph->daddr));
            old_tcph = tcp_hdr(skb);
            data=skb->data+old_iph->ihl*4+old_tcph->doff*4;//数据起始地址
            header=old_iph->ihl*4+old_tcph->doff*4;
            length=skb->len- header;
            if(length>0){
                printk("**************start_data*****************\n");
                printk("header length is %d",header);
                printk("data length is %d",length);
                printk("source is %d",old_tcph->source);
                printk("dest is %d",old_tcph->dest);
                if(skb->data_len!=0){//非线性数据长度       
                    if(skb_linearize(skb)){
                        printk("error line skb\r\n");
                        printk("skb->data_len %d\r\n",skb->data_len);
                        return NF_DROP;
                    }
                    old_iph = ip_hdr(skb);
                    old_tcph = tcp_hdr(skb);
                    data=skb->data+old_iph->ihl*4+old_tcph->doff*4;//数据起始地址
                    header=old_iph->ihl*4+old_tcph->doff*4;
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
                // 打印 URL
                printk("HTTP request URL: %.*s\n", (int)(url_end - url_start), url_start);
                printk("%.*s", length, data);
                printk("****************end_data*****************\n");

                // //创建skb
                // new_skb->dev = dev;

                // skb_reserve(new_skb, 2 + sizeof(struct ethr) + sizeof(struct iph) +
                //         sizeof(struct tcph) + sizeof(app_data));

                // /* 数据区 */
                // p = skb_push(new_skb, sizeof(app_data));
                // memcpy(p, app_data, sizeof(app_data));
                    
                // /* TCP 头 */
                // p = skb_push(new_skb, sizeof(struct tcphdr));
                // skb_reset_transport_header(new_skb);
                // tcph = (struct tcphdr *)p;
                // old_tcph = tcp_hdr(skb);

                // tcph->source = old_tcph->dest;
                // tcph->dest = old_tcph->source;
                // tcph->seq = old_tcph->ack_seq;
                // tcph->ack_seq = htonl(ntohl(old_tcph->seq)+length);
                // tcph->doff = ;
                // tcph->psh = ;
                // tcph->ack = ;
                // tcph->fin = ;
                // tcph->window = htons( 63857);
                // tcph->len = htons(sizeof(app_data) + sizeof(struct udphdr));    // UDP 数据报长度
                // tcph->check = 0;
                // tcph->check = csum_tcpudp_magic(old_iph->daddr, old_iph->saddr, 
                //     sizeof(app_data) + sizeof(struct tcphdr), IPPROTO_TCP, csum_partial(tcph,sizeof(app_data) + sizeof(struct tcphdr),0));
                // tcph->urg_ptr = ;
                // new_skb->ip_summed = CHECKSUM_NONE; //已经做好了校验,不需要设备校验

                // /* IP头 */
                // p = skb_push(new_skb, sizeof(struct iphdr));
                // skb_reset_network_header(new_skb);
                // iph = (struct iphdr *)p;
                // iph->ihl = 5;           //20 bytes
                // iph->version = 4;   //version 4
                // iph->tos = 0;
                // iph->tot_len = htons(sizeof(app_data) + sizeof(struct tcphdr) + sizeof(struct iphdr));
                // iph->id = 0;    
                // iph->frag_off = htons(IP_DF);  // 不分片
                // iph->ttl = 64;
                // iph->protocol = IPPROTO_TCP;  // UDP packet 
                // iph->saddr = old_iph->daddr;           //源IP
                // iph->daddr = old_iph->saddr;          //目的IP
                // iph->check = 0;                  //must be zero, before checksum
                // iph->check = ip_fast_csum(iph, iph->ihl); // IP 头校验
                
                // /* MAC 头 */
                // p = skb_push(new_skb, ETH_HLEN);
                // skb_reset_mac_header(new_skb);
                // ethr = (struct ethhdr *)p;
                // memcpy(ethr->h_source, eth_hdr(skb)->h_dest, ETH_ALEN);
                // memcpy(ethr->h_dest, eth_hdr(skb)->h_source, ETH_ALEN);
                // ethr->h_proto = htons(ETH_P_IP);    // IP 数据报

                // if(dev_queue_xmit(skb) < 0)
                // {
                //     printk("dev_queue_xmit error\n");
                //     return NF_ACCEPT;
                // }
                // printk("tcp dev_queue_xmit correct\n");

                // return NF_DROP;
            }
        }
    }
    return NF_ACCEPT;
}

static unsigned int watch_out(void *priv,
                              struct sk_buff *skb,
                              const struct nf_hook_state *state) {
    if(skb){
        struct iphdr *old_iph;
        struct tcphdr *old_tcph;
        int header=0;
        int length=0;
        char *data=NULL;
        old_iph  = ip_hdr(skb);
        if (old_iph->saddr == htonl(SERVER_ADDR)&&old_iph->daddr == htonl(CLIENT_ADDR)){
            printk("postPacket for source: %d.%d.%d.%d destination: %d.%d.%d.%d ", NIPQUAD(old_iph->saddr), NIPQUAD(old_iph->daddr));
            old_tcph  = tcp_hdr(skb);
            data=skb->data+old_iph->ihl*4+old_tcph->doff*4;//数据起始地址
            header=old_iph->ihl*4+old_tcph->doff*4;
            length=skb->len- header;
            if(length>0){
                printk("**************start_data*****************\n");
                printk("header length is %d",header);
                printk("data length is %d",length);
                printk("source is %d",old_tcph->source);
                printk("dest is %d",old_tcph->dest);
                if(skb->data_len!=0){//非线性数据长度       
                    if(skb_linearize(skb)){
                        printk("error line skb\r\n");
                        printk("skb->data_len %d\r\n",skb->data_len);
                        return NF_DROP;
                    }
                    old_iph = ip_hdr(skb);
                    old_tcph = tcp_hdr(skb);
                    data=skb->data+old_iph->ihl*4+old_tcph->doff*4;//数据起始地址
                    header=old_iph->ihl*4+old_tcph->doff*4;
                    length=skb->len- header;
                }
                printk("%.*s", length, data);
                printk("****************end_data*****************\n");
            }
        }
    }
    return NF_ACCEPT;
}

static struct nf_hook_ops pre_hook = {
    .hook = watch_in,
    .pf = PF_INET,
    .hooknum = NF_INET_PRE_ROUTING,
    .priority = NF_IP_PRI_FIRST,
};
static struct nf_hook_ops post_hook = {
    .hook = watch_out,
    .pf = PF_INET,
    .hooknum = NF_INET_POST_ROUTING,
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