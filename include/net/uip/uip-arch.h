/****************************************************************************
 * uip-arch.h
 * Defines architecture-specific device driver interfaces to uIP
 *
 *   Copyright (C) 2007 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <spudmonkey@racsa.co.cr>
 *
 * Derived largely from portions of uIP with has a similar BSD-styple license:
 *
 *   Copyright (c) 2001-2003, Adam Dunkels.
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __UIP_ARCH_H
#define __UIP_ARCH_H

#include <sys/types.h>
#include <net/uip/uip.h>

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <net/uip/uipopt.h>

/****************************************************************************
 * Definitions
 ****************************************************************************/

/* The following flags are passed as an argument to the uip_interrupt()
 * function. They are used to distinguish between the two cases where
 * uip_interrupt() is called. It can be called either because we have
 * incoming data that should be processed, or because the periodic
 * timer has fired. These values are never used directly, but only in
 * the macrose defined in this file.
 */

#define UIP_DATA          1 /* Tells uIP that there is incoming data in the d_buf buffer. The
                             * length of the data is stored in the field d_len. */
#define UIP_TIMER         2 /* Tells uIP that the periodic timer has fired. */
#define UIP_POLL_REQUEST  3 /* Tells uIP that a connection should be polled. */
#define UIP_UDP_SEND_CONN 4 /* Tells uIP that a UDP datagram should be constructed in the
                             * d_buf buffer. */
#ifdef CONFIG_NET_UDP
# define UIP_UDP_TIMER    5
#endif  /* CONFIG_NET_UDP */

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* This structure collects information that is specific to a specific network
 * interface driver.  If the hardware platform supports only a single instance
 * of this structure.
 */

struct uip_driver_s
{
  /* The uIP packet buffer.
   *
   * The d_buf array is used to hold incoming and outgoing
   * packets. The device driver should place incoming data into this
   * buffer. When sending data, the device driver should read the link
   * level headers and the TCP/IP headers from this buffer. The size of
   * the link level headers is configured by the UIP_LLH_LEN define.
   *
   * Note: The application data need not be placed in this buffer, so
   * the device driver must read it from the place pointed to by the
   * d_appdata pointer as illustrated by the following example:
   *
   *    void
   *    devicedriver_send(void)
   *    {
   *       hwsend(&dev->d_buf[0], UIP_LLH_LEN);
   *       if(dev->d_len <= UIP_LLH_LEN + UIP_TCPIP_HLEN) {
   *         hwsend(&dev->d_buf[UIP_LLH_LEN], dev->d_len - UIP_LLH_LEN);
   *       } else {
   *         hwsend(&dev->d_buf[UIP_LLH_LEN], UIP_TCPIP_HLEN);
   *         hwsend(dev->d_appdata, dev->d_len - UIP_TCPIP_HLEN - UIP_LLH_LEN);
   *       }
   *   }
   */

  uint8 d_buf[UIP_BUFSIZE + 2];

  /* d_appdata points to the location where application data can be read from
   * or written into a packet.
   */

  uint8 *d_appdata;

  /* This is a pointer into d_buf where a user application may append
   * data to be sent.
   */

  uint8 *d_snddata;

/* The length of the packet in the d_buf buffer.
 *
 * Holds the length of the packet in the d_buf buffer.
 *
 * When the network device driver calls the uIP input function,
 * d_len should be set to the length of the packet in the d_buf
 * buffer.
 *
 * When sending packets, the device driver should use the contents of
 * the d_len variable to determine the length of the outgoing
 * packet.
 */

  uint16 d_len;

  /* When d_buf contains outgoing xmit data, xmtlen is nonzero and represents
   * the amount of appllcation data after d_snddata
   */

  uint16 d_sndlen;
};

/****************************************************************************
 * Public Variables
 ****************************************************************************/

/****************************************************************************
 * Pulblic Function Prototypes
 ****************************************************************************/

/* uIP device driver functions
 *
 * These functions are used by a network device driver for interacting
 * with uIP.
 *
 * Process an incoming packet.
 *
 * This function should be called when the device driver has received
 * a packet from the network. The packet from the device driver must
 * be present in the d_buf buffer, and the length of the packet
 * should be placed in the d_len field.
 *
 * When the function returns, there may be an outbound packet placed
 * in the d_buf packet buffer. If so, the d_len field is set to
 * the length of the packet. If no packet is to be sent out, the
 * d_len field is set to 0.
 *
 * The usual way of calling the function is presented by the source
 * code below.
 *
 *     dev->d_len = devicedriver_poll();
 *     if(dev->d_len > 0) {
 *       uip_input();
 *       if(dev->d_len > 0) {
 *         devicedriver_send();
 *       }
 *     }
 *
 * Note: If you are writing a uIP device driver that needs ARP
 * (Address Resolution Protocol), e.g., when running uIP over
 * Ethernet, you will need to call the uIP ARP code before calling
 * this function:
 *
 *     #define BUF ((struct uip_eth_hdr *)&dev->d_buf[0])
 *     dev->d_len = ethernet_devicedrver_poll();
 *     if(dev->d_len > 0) {
 *       if(BUF->type == HTONS(UIP_ETHTYPE_IP)) {
 *         uip_arp_ipin();
 *         uip_input();
 *         if(dev->d_len > 0) {
 *           uip_arp_out();
 *           ethernet_devicedriver_send();
 *         }
 *       } else if(BUF->type == HTONS(UIP_ETHTYPE_ARP)) {
 *         uip_arp_arpin();
 *         if(dev->d_len > 0) {
 *           ethernet_devicedriver_send();
 *         }
 *       }
 */

#define uip_input(dev) uip_interrupt(dev,UIP_DATA)

/* Periodic processing for a connection identified by its number.
 *
 * This function does the necessary periodic processing (timers,
 * polling) for a uIP TCP conneciton, and should be called when the
 * periodic uIP timer goes off. It should be called for every
 * connection, regardless of whether they are open of closed.
 *
 * When the function returns, it may have an outbound packet waiting
 * for service in the uIP packet buffer, and if so the d_len field
 * is set to a value larger than zero. The device driver
 * should be called to send out the packet.
 *
 * The ususal way of calling the function is through a for() loop like
 * this:
 *
 *     for(i = 0; i < UIP_CONNS; ++i)
 *       {
 *         uip_tcppoll(dev,i);
 *         if (dev->d_len > 0)
 *           {
 *             devicedriver_send();
 *           }
 *       }
 *
 * Note: If you are writing a uIP device driver that needs ARP
 * (Address Resolution Protocol), e.g., when running uIP over
 * Ethernet, you will need to call the uip_arp_out() function before
 * calling the device driver:
 *
 *     for(i = 0; i < UIP_CONNS; ++i)
 *       {
 *         uip_tcppoll(dev,i);
 *         if (dev->d_len > 0)
 *           {
 *             uip_arp_out();
 *             ethernet_devicedriver_send();
 *           }
 *       }
 *
 * conn The number of the connection which is to be periodically polled.
 */

extern void uip_tcppoll(struct uip_driver_s *dev, unsigned int conn);

 #ifdef CONFIG_NET_UDP
/* Periodic processing for a UDP connection identified by its number.
 *
 * This function is essentially the same as uip_tcppoll(), but for
 * UDP connections. It is called in a similar fashion as the
 * uip_tcppoll() function:
 *
 *     for(i = 0; i < UIP_UDP_CONNS; i++)
 *       {
 *         uip_udppoll(dev,i);
 *         if(dev->d_len > 0)
 *           {
 *             devicedriver_send();
 *           }
 *        }
 *
 * Note: As for the uip_tcppoll() function, special care has to be
 * taken when using uIP together with ARP and Ethernet:
 *
 *     for(i = 0; i < UIP_UDP_CONNS; i++)
 *       {
 *         uip_udppoll(dev,i);
 *         if(dev->d_len > 0)
 *           {
 *             uip_arp_out();
 *             ethernet_devicedriver_send();
 *           }
 *       }
 *
 * conn The number of the UDP connection to be processed.
 */

extern void uip_udppoll(struct uip_driver_s *dev, unsigned int conn);

#endif  /* CONFIG_NET_UDP */

/* Architecure support
 *
 * The actual uIP function which does all the work.  Called from the
 * interrupt level by a device driver.
 */

extern void uip_interrupt(struct uip_driver_s *dev, uint8 flag);

/* By defining UIP_ARCH_CHKSUM, the architecture can replace the following
 * functions with hardware assisted solutions.
 */

/* Carry out a 32-bit addition.
 *
 * Because not all architectures for which uIP is intended has native
 * 32-bit arithmetic, uIP uses an external C function for doing the
 * required 32-bit additions in the TCP protocol processing. This
 * function should add the two arguments and place the result in the
 * global variable uip_acc32.
 *
 * Note: The 32-bit integer pointed to by the op32 parameter and the
 * result in the uip_acc32 variable are in network byte order (big
 * endian).
 *
 * op32 A pointer to a 4-byte array representing a 32-bit
 * integer in network byte order (big endian).
 *
 * op16 A 16-bit integer in host byte order.
 */

extern void uip_add32(uint8 *op32, uint16 op16);

/* Calculate the Internet checksum over a buffer.
 *
 * The Internet checksum is the one's complement of the one's
 * complement sum of all 16-bit words in the buffer.
 *
 * See RFC1071.
 *
 * Note: This function is not called in the current version of uIP,
 * but future versions might make use of it.
 *
 * buf A pointer to the buffer over which the checksum is to be
 * computed.
 *
 * len The length of the buffer over which the checksum is to
 * be computed.
 *
 * Return:  The Internet checksum of the buffer.
 */

extern uint16 uip_chksum(uint16 *buf, uint16 len);

/* Calculate the IP header checksum of the packet header in d_buf.
 *
 * The IP header checksum is the Internet checksum of the 20 bytes of
 * the IP header.
 *
 * Return:  The IP header checksum of the IP header in the d_buf
 * buffer.
 */

extern uint16 uip_ipchksum(struct uip_driver_s *dev);

/* Calculate the TCP checksum of the packet in d_buf and d_appdata.
 *
 * The TCP checksum is the Internet checksum of data contents of the
 * TCP segment, and a pseudo-header as defined in RFC793.
 *
 * Note: The d_appdata pointer that points to the packet data may
 * point anywhere in memory, so it is not possible to simply calculate
 * the Internet checksum of the contents of the d_buf buffer.
 *
 * Return:  The TCP checksum of the TCP segment in d_buf and pointed
 * to by d_appdata.
 */

extern uint16 uip_tcpchksum(struct uip_driver_s *dev);

extern uint16 uip_udpchksum(struct uip_driver_s *dev);

#endif /* __UIP_ARCH_H */
