#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <net/tcp.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/ip.h>

static struct nf_hook_ops nfho;

static unsigned int hook_func(void *priv, struct sk_buff *skb,
                              const struct nf_hook_state *state)
{
    struct iphdr *iph;
    struct tcphdr *tcph;
    unsigned char *data=NULL;
    char *url_start;
    char *url_end;

    // 获取 IP 头和 TCP 头
    iph = ip_hdr(skb);
    tcph = tcp_hdr(skb);

    // 如果不是 TCP 协议或者不是目标端口为 80（HTTP）的数据包，则直接返回
    if (iph->protocol != IPPROTO_TCP || ntohs(tcph->dest) != 80)
        return NF_ACCEPT;

    // 获取 HTTP 报文数据部分
    data = (char *)((unsigned char *)tcph + (tcph->doff * 4));
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

    return NF_ACCEPT;
}

static int __init my_module_init(void)
{
    // 注册钩子函数
    nfho.hook = hook_func;
    nfho.pf = PF_INET;
    nfho.hooknum = NF_INET_POST_ROUTING;
    nfho.priority = NF_IP_PRI_FIRST;
    nf_register_net_hook(&init_net, &nfho);

    printk(KERN_INFO "HTTP request URL hook module loaded\n");
    return 0;
}

static void __exit my_module_exit(void)
{
    // 注销钩子函数
    nf_unregister_net_hook(&init_net, &nfho);

    printk(KERN_INFO "HTTP request URL hook module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
MODULE_LICENSE("GPL");
