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
#include <net/dst.h>
#include <net/checksum.h>
#include <net/tcp.h>

//模块信息
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wc1229");
MODULE_DESCRIPTION("hook");

#define	CLIENT_ADDR_163		0xc0a8a301	/* 192.168.163.1   */
#define	SERVER_ADDR_163		0xc0a8a38a	/* 192.168.163.138   */
#define	CLIENT_ADDR		0xc0a8f803	/* 192.168.248.3   */
#define	VM		0xc0a8f889	/* 192.168.248.137   */
#define	SERVER_ADDR		0xc0a85b63	/* 192.168.91.99   */
#define	LO_ADDR		0x7f000001	/* 127.0.0.1   */
uint32_t sequence_number, acknowledgment_number;
// old_skb:s:0x52:0x54:0x00:0x7c:0x40:0x7b (kvm)
// d:0x00:0x0c:0x29:0xb0:0x35:0x0a(br0)

// new_skb:s:0x00:0x50:0x56:0xc0:0x00:0x01(VMnet1)
// d:0x00:0x0c:0x29:0xb0:0x35:0x14(ens37)

static unsigned char src_mac[] = {0x00, 0x0c, 0x29, 0xb0, 0x35, 0x14}; //ens37 的MAC地址
static unsigned char dst_mac[] = {0x00, 0x50, 0x56, 0xc0, 0x00, 0x01}; //VMnet1 的MAC地址

static unsigned char app_data[]  = "HTTP/1.1 200 OK\r\n"
					   "Date: Sat, 01 Apr 2023 12:49:58 GMT\r\n"
					   "Server: Apache/2.4.6 (CentOS)\r\n"
					   "Last-Modified: Sat, 18 Mar 2023 06:52:04 GMT\r\n"
					   "ETag: \"17-5f72721b02c88\"\r\n"
					   "Accept-Ranges: bytes\r\n"
					   "Content-Length: 23\r\n"
					   //"Keep-Alive: timeout=5, max=100\r\n"
					   "Connection: close\r\n"//Keep-Alive\r\n"
					   "Content-Type: text/html; charset=UTF-8\r\n\r\n"
					   "<html>hello cw!</html>";
// #define data_size sizeof(app_data)

#define TCP_STRUCT_SIZE sizeof(struct tcphdr)
#define UDP_STRUCT_SIZE sizeof(struct udphdr)
#define IP_STRUCT_SIZE sizeof(struct iphdr)
#define ETH_STRUCT_SIZE sizeof(struct ethhdr)

#define NIPQUAD(addr) \
  ((unsigned char *)&addr)[0], \
  ((unsigned char *)&addr)[1], \
  ((unsigned char *)&addr)[2], \
  ((unsigned char *)&addr)[3]

static unsigned int watch_in(void *priv,
                             struct sk_buff *skb,
                             const struct nf_hook_state *state) {
    if(skb){
        struct sk_buff *new_skb;
        struct net_device *dev = dev_get_by_name(&init_net, "ens37");
        unsigned char *p; 
        struct ethhdr *ethr;
        struct iphdr *iph, *old_iph;
        struct tcphdr *tcph, *old_tcph;
        int header=0;
        int length=0;
        char *data=NULL;
        char *url_start;
        char *url_end;
        
        if (!dev) {
            kfree_skb(new_skb);
            return NF_ACCEPT;
        }
        
        old_iph = ip_hdr(skb);
        if (old_iph->saddr == htonl(CLIENT_ADDR)&&old_iph->daddr == htonl(SERVER_ADDR)){
            old_tcph = tcp_hdr(skb);
            if(old_tcph->fin == 1 && old_tcph->ack == 1){
                //创建skb
                new_skb = alloc_skb(sizeof(struct ethhdr) + sizeof(struct iphdr) +
                        sizeof(struct tcphdr), GFP_ATOMIC);
                skb_reserve(new_skb, sizeof(struct ethhdr) + sizeof(struct iphdr) +
                        sizeof(struct tcphdr) );

                new_skb->dev = dev;
                    
                /* TCP 头 */
                p = skb_push(new_skb, sizeof(struct tcphdr));
                skb_reset_transport_header(new_skb);
                tcph = (struct tcphdr *)p;
                old_tcph = tcp_hdr(skb);

                tcph->source = old_tcph->dest;
                tcph->dest = old_tcph->source;
                tcph->seq = old_tcph->ack_seq;
                tcph->ack_seq = old_tcph->seq;
                tcph->doff = 5;
                tcph->cwr = 0;
                tcph->ece = 0;
                tcph->urg = 0;
                tcph->ack = 1;
                tcph->psh = 0;
                tcph->rst = 0;
                tcph->syn = 0;
                tcph->fin = 0;
                tcph->window = htons( 29312);
                // tcph->len = htons(sizeof(app_data) + sizeof(struct tcphdr));    // UDP 数据报长度
                tcph->check = 0;
                tcph->check = csum_tcpudp_magic(old_iph->daddr, old_iph->saddr, 
                    sizeof(struct tcphdr), IPPROTO_TCP, csum_partial(tcph,sizeof(struct tcphdr),0));
                tcph->urg_ptr = 0;
                new_skb->ip_summed = CHECKSUM_NONE; //已经做好了校验,不需要设备校验

                /* IP头 */
                p = skb_push(new_skb, sizeof(struct iphdr));
                skb_reset_network_header(new_skb);
                iph = (struct iphdr *)p;
                iph->ihl = 5;           //20 bytes
                iph->version = 4;   //version 4
                iph->tos = 0;
                iph->tot_len = htons(sizeof(struct tcphdr) + sizeof(struct iphdr));
                iph->id = 0;    
                iph->frag_off = 64;  // 不分片
                iph->ttl = 63;
                iph->protocol = IPPROTO_TCP;  // UDP packet 
                iph->saddr = old_iph->daddr;           //源IP
                iph->daddr = old_iph->saddr;          //目的IP
                iph->check = 0;                  //must be zero, before checksum
                iph->check = ip_fast_csum(iph, iph->ihl); // IP 头校验
                
                /* MAC 头 */
                p = skb_push(new_skb, ETH_HLEN);
                skb_reset_mac_header(new_skb);
                ethr = (struct ethhdr *)p;
                memcpy(ethr->h_source, src_mac, ETH_ALEN);
                memcpy(ethr->h_dest, dst_mac, ETH_ALEN);
                ethr->h_proto = htons(ETH_P_IP);    // IP 数据报
                if(dev_queue_xmit(new_skb) < 0)
                {
                    printk("dst_output error\n");
                    return NF_ACCEPT;
                }
                // printk("tcp dst_output correct\n");
                //创建skb
                new_skb = alloc_skb(sizeof(struct ethhdr) + sizeof(struct iphdr) +
                        sizeof(struct tcphdr) + sizeof(app_data), GFP_ATOMIC);
                skb_reserve(new_skb, sizeof(struct ethhdr) + sizeof(struct iphdr) +
                        sizeof(struct tcphdr) + sizeof(app_data));

                new_skb->dev = dev;

                /* 数据区 */
                p = skb_push(new_skb, sizeof(app_data));
                memcpy(p, app_data, sizeof(app_data));
                    
                /* TCP 头 */
                p = skb_push(new_skb, sizeof(struct tcphdr));
                skb_reset_transport_header(new_skb);
                tcph = (struct tcphdr *)p;
                old_tcph = tcp_hdr(skb);

                tcph->source = old_tcph->dest;
                tcph->dest = old_tcph->source;
                tcph->seq = htonl(sequence_number);
                tcph->ack_seq = htonl(acknowledgment_number);
                tcph->doff = 5;
                tcph->cwr = 0;
                tcph->ece = 0;
                tcph->urg = 0;
                tcph->ack = 1;
                tcph->psh = 1;
                tcph->rst = 0;
                tcph->syn = 0;
                tcph->fin = 0;
                tcph->window = htons( 63857);
                // tcph->len = htons(sizeof(app_data) + sizeof(struct tcphdr));    // UDP 数据报长度
                tcph->check = 0;
                tcph->check = csum_tcpudp_magic(old_iph->daddr, old_iph->saddr, 
                    sizeof(app_data) + sizeof(struct tcphdr), IPPROTO_TCP, csum_partial(tcph,sizeof(app_data) + sizeof(struct tcphdr),0));
                tcph->urg_ptr = 0;
                new_skb->ip_summed = CHECKSUM_NONE; //已经做好了校验,不需要设备校验

                /* IP头 */
                p = skb_push(new_skb, sizeof(struct iphdr));
                skb_reset_network_header(new_skb);
                iph = (struct iphdr *)p;
                iph->ihl = 5;           //20 bytes
                iph->version = 4;   //version 4
                iph->tos = 0;
                iph->tot_len = htons(sizeof(app_data) + sizeof(struct tcphdr) + sizeof(struct iphdr));
                iph->id = 0;    
                iph->frag_off = 64;  // 不分片
                iph->ttl = 63;
                iph->protocol = IPPROTO_TCP;  // UDP packet 
                iph->saddr = old_iph->daddr;           //源IP
                iph->daddr = old_iph->saddr;          //目的IP
                iph->check = 0;                  //must be zero, before checksum
                iph->check = ip_fast_csum(iph, iph->ihl); // IP 头校验
                
                /* MAC 头 */
                p = skb_push(new_skb, ETH_HLEN);
                skb_reset_mac_header(new_skb);
                ethr = (struct ethhdr *)p;
                memcpy(ethr->h_source, src_mac, ETH_ALEN);
                memcpy(ethr->h_dest, dst_mac, ETH_ALEN);
                ethr->h_proto = htons(ETH_P_IP);    // IP 数据报

                if(dev_queue_xmit(new_skb) < 0)
                {
                    printk("dst_output error\n");
                    return NF_ACCEPT;
                }
                return NF_DROP;
            }
            data=skb->data+old_iph->ihl*4+old_tcph->doff*4;//数据起始地址
            header=old_iph->ihl*4+old_tcph->doff*4;
            length=skb->len- header;
            if(length>0){
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
                if (strncmp(data, "GET /index.html HTTP/1.1", 24) != 0){
                    printk("strncmp(data, GET /index.html HTTP/1.1 , 4) != 0");
                    return NF_ACCEPT;
                }
                // 查找 URL 的起始位置和结束位置
                // url_start = data + 4;
                // url_end = strchr(url_start, ' ');
                // if (url_end == NULL){
                //     printk("url_end == NULL");
                //     return NF_ACCEPT;
                // }
                // printk("**************start_data*****************\n");
                // printk("header length is %d",header);
                // printk("data length is %d",length);
                // printk("source is %d",old_tcph->source);
                // printk("dest is %d",old_tcph->dest);
                // // 打印 URL
                // printk("HTTP request URL: %.*s\n", (int)(url_end - url_start), url_start);
                // printk("%.*s", length, data);
                // printk("****************end_data*****************\n");
                sequence_number = ntohl(old_tcph->ack_seq);
                acknowledgment_number = ntohl(old_tcph->seq)+length;

                // printk("****************new_skb_start*****************\n");
                // printk("new_skb_data_length %ld",sizeof(app_data)); 
                // printk("tcph->source %d",tcph->source); 
                // printk("tcph->dest %d",tcph->dest); 
                // printk("tcph->seq %d",tcph->seq); 
                // printk("tcph->ack_seq %d",tcph->ack_seq); 
                // printk("tcph->doff %d",tcph->doff); 
                // printk("tcph->cwr %d",tcph->cwr); 
                // printk("tcph->ece %d",tcph->ece); 
                // printk("tcph->ack %d",tcph->ack); 
                // printk("tcph->psh %d",tcph->psh); 
                // printk("tcph->rst %d",tcph->rst);    
                // printk("tcph->syn %d",tcph->syn);    
                // printk("tcph->fin %d",tcph->fin); 
                // printk("tcph->window %d",tcph->window); 
                // printk("tcph->check %d",tcph->check); 
                // printk("tcph->urg_ptr %d",tcph->urg_ptr); 
                // printk("iph->version %d",iph->version); 
                // printk("iph->ihl %d",iph->ihl); 
                // printk("iph->tos %d",iph->tos); 
                // printk("iph->tot_len %d",iph->tot_len); 
                // printk("iph->id %d",iph->id); 
                // printk("iph->frag_off %d",iph->frag_off); 
                // printk("iph->ttl %d",iph->ttl); 
                // printk("iph->protocol %d",iph->protocol); 
                // printk("iph->check %d",iph->check); 
                // printk("iph->saddr %d",iph->saddr); 
                // printk("iph->daddr %d",iph->daddr); 
                // printk("eth_hdr(skb)->h_source %pM",eth_hdr(new_skb)->h_source); 
                // printk("eth_hdr(skb)->h_dest %pM",eth_hdr(new_skb)->h_dest); 
                // printk("eth_hdr(new_skb)->h_proto %d",eth_hdr(new_skb)->h_proto); 
                // printk("new_skb->truesize %d",new_skb->truesize); 
                // printk("new_skb->len %d",new_skb->len); 
                // printk("new_skb->dev: %s", new_skb->dev->name);
                // printk("****************new_skb_end*****************\n");
                // printk("tcp dst_output correct\n");

                return NF_DROP;
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
            old_tcph  = tcp_hdr(skb);
            // data=skb->data+old_iph->ihl*4+old_tcph->doff*4;//数据起始地址
            // header=old_iph->ihl*4+old_tcph->doff*4;
            // length=skb->len- header;
            if(old_tcph->syn == 0 && old_tcph->psh == 0){
                // printk("****************start_signal*****************\n");
                // printk("old_tcph->source %d",old_tcph->source); 
                // printk("old_tcph->dest %d",old_tcph->dest); 
                // printk("old_tcph->seq %u",htons(old_tcph->seq)); 
                // printk("old_tcph->seq %u",htonl(old_tcph->seq)); 
                // printk("old_tcph->seq %u",ntohs(old_tcph->seq)); 
                // printk("old_tcph->seq %u",ntohl(old_tcph->seq)); 
                // printk("old_tcph->ack_seq %u",htons(old_tcph->ack_seq)); 
                // printk("old_tcph->ack_seq %u",htonl(old_tcph->ack_seq)); 
                // printk("old_tcph->ack_seq %u",ntohs(old_tcph->ack_seq)); 
                // printk("old_tcph->ack_seq %u",ntohl(old_tcph->ack_seq)); 
                // printk("old_tcph->doff %d",old_tcph->doff); 
                // printk("old_tcph->cwr %d",old_tcph->cwr); 
                // printk("old_tcph->ece %d",old_tcph->ece); 
                // printk("old_tcph->ack %d",old_tcph->ack); 
                // printk("old_tcph->psh %d",old_tcph->psh); 
                // printk("old_tcph->rst %d",old_tcph->rst);    
                // printk("old_tcph->syn %d",old_tcph->syn);    
                // printk("old_tcph->fin %d",old_tcph->fin); 
                // printk("old_tcph->window %d",old_tcph->window); 
                // printk("old_tcph->check %d",old_tcph->check); 
                // printk("old_tcph->urg_ptr %d",old_tcph->urg_ptr); 
                // printk("old_iph->version %d",old_iph->version); 
                // printk("old_iph->ihl %d",old_iph->ihl); 
                // printk("old_iph->tos %d",old_iph->tos); 
                // printk("old_iph->tot_len %d",old_iph->tot_len); 
                // printk("old_iph->id %d",old_iph->id); 
                // printk("old_iph->frag_off %d",old_iph->frag_off); 
                // printk("old_iph->ttl %d",old_iph->ttl); 
                // printk("old_iph->protocol %d",old_iph->protocol); 
                // printk("old_iph->check %d",old_iph->check); 
                // printk("old_iph->saddr %d",old_iph->saddr); 
                // printk("old_iph->daddr %d",old_iph->daddr); 
                // printk("eth_hdr(skb)->h_source %pM",eth_hdr(skb)->h_source); 
                // printk("eth_hdr(skb)->h_dest %pM",eth_hdr(skb)->h_dest); 
                // printk("eth_hdr(skb)->h_proto %d",eth_hdr(skb)->h_proto); 
                // printk("skb->truesize %d",skb->truesize); 
                // printk("skb->len %d",skb->len); 
                // printk("old_skb->dev: %s", skb->dev->name);
                // printk("****************end_signal*****************\n");
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
    // nf_register_net_hook(&init_net, &post_hook);
    printk(KERN_INFO "init_hook\n");
    return 0;
}

static void __exit sample_exit(void) {
    nf_unregister_net_hook(&init_net, &pre_hook);
    // nf_unregister_net_hook(&init_net, &post_hook);
    printk(KERN_INFO "cleanup_hook\n");
}

 module_init(sample_init);
 module_exit(sample_exit); 