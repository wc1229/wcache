#ifndef __HOOK__H__
#define  __HOOK__H__

#include <linux/module.h>
#include <linux/kernel.h>

#define	CLIENT_ADDR_163		0xc0a8a301	/* 192.168.163.1   */
#define	SERVER_ADDR_163		0xc0a8a38a	/* 192.168.163.138   */
#define	CLIENT_ADDR		0xc0a82902	/* 192.168.41.2   */
#define	SERVER_ADDR		0xc0a85b63	/* 192.168.91.99   */
#define	LO_ADDR		0x7f000001	/* 127.0.0.1   */

#define NIPQUAD(addr) \
  ((unsigned char *)&addr)[0], \
  ((unsigned char *)&addr)[1], \
  ((unsigned char *)&addr)[2], \
  ((unsigned char *)&addr)[3]

extern unsigned int watch_in(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
extern unsigned int watch_out(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern struct nf_hook_ops pre_hook;
extern struct nf_hook_ops post_hook;

#endif