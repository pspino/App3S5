/* EthernetInterface.cpp */
/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define __DEBUG__ 0
#ifndef __MODULE__
#define __MODULE__ "EthernetInterface.cpp"
#endif

#include "core/fwk.h"

#include "EthernetInterface.h"

#include "lwip/inet.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "arch/lpc17_emac.h"
#include "lpc_phy.h" /* For the PHY monitor support */
#include "lwip/tcpip.h"

#include "mbed.h"


EthernetInterface::EthernetInterface() : LwIPInterface(), m_lpcNetif(), m_netifStatusSphre(1), m_useDHCP(false)
{
  s_lpcNetifOff = (int32_t)( ((char*)this) - ((char*)&m_lpcNetif) );
  m_netifStatusSphre.wait(0);
}

int EthernetInterface::init() //With DHCP
{
  m_useDHCP = true;

  DBG("Initializing LwIP");
  LwIPInterface::init(); //Init LwIP, NOT including PPP

  DBG("DHCP IP assignment");

  memset((void*)&m_lpcNetif, 0, sizeof(m_lpcNetif));
  netif_add(&m_lpcNetif, NULL, NULL, NULL, NULL, lpc_enetif_init, ethernet_input/*tcpip_input???*/);
  netif_set_default(&m_lpcNetif);
  
  netif_set_status_callback(&m_lpcNetif, &EthernetInterface::netifStatusCb);

  return OK;
}

int EthernetInterface::init(const char* ip, const char* mask, const char* gateway,
    const char* dns1, const char* dns2) //No DHCP
{
  ip_addr_t ip_n, mask_n, gateway_n, dns1_n, dns2_n;

  m_useDHCP = false;

  DBG("Initializing LwIP");
  LwIPInterface::init(); //Init LwIP

  DBG("Static IP assignment");
  inet_aton(ip, &ip_n);
  inet_aton(mask, &mask_n);
  inet_aton(gateway, &gateway_n);
  inet_aton(dns1, &dns1_n);
  inet_aton(dns2, &dns2_n);

  memset((void*)&m_lpcNetif, 0, sizeof(m_lpcNetif));
  netif_add(&m_lpcNetif, &ip_n, &mask_n, &gateway_n, NULL, lpc_enetif_init, ethernet_input/*tcpip_input???*/);
  netif_set_default(&m_lpcNetif);
  
  netif_set_status_callback(&m_lpcNetif, &EthernetInterface::netifStatusCb);

  return OK;
}

int EthernetInterface::connect()
{
  m_netifStatusSphre.wait(0);

  DBG("Enable MAC interrupts");
  NVIC_SetPriority(ENET_IRQn, ((0x01 << 3) | 0x01));
  NVIC_EnableIRQ(ENET_IRQn);

  if(m_useDHCP)
  {
    dhcp_start(&m_lpcNetif); //The DHCP client will set the interface up once the IP/Netmask/Getway/DNS Servers are recovered
  }
  else
  {
    netif_set_up(&m_lpcNetif); //Set interface up
  }
  
  m_netifStatusSphre.wait();
  
  DBG("Connected with IP %s", getIPAddress());

  return OK;
}

int EthernetInterface::disconnect()
{
  m_netifStatusSphre.wait(0);

  if(m_useDHCP)
  {
    dhcp_release(&m_lpcNetif); //Release the lease & bring interface down
    dhcp_stop(&m_lpcNetif);
  }
  else
  {
    netif_set_down(&m_lpcNetif);  
  }

  DBG("Disable MAC interrupts");
  NVIC_DisableIRQ(ENET_IRQn);
  
  m_netifStatusSphre.wait();

  return OK;
}

/*static*/ void EthernetInterface::netifStatusCb(struct netif *netif)
{
    EthernetInterface* pIf = (EthernetInterface*)( ((int32_t)netif) + ((int32_t)s_lpcNetifOff) ); //Bad kludge
    pIf->setIPAddress(inet_ntoa(netif->ip_addr));
    pIf->setConnected(netif_is_up(netif)?true:false);
    pIf->m_netifStatusSphre.wait(0); //Clear if pending
    pIf->m_netifStatusSphre.release();
}

/*static*/ int32_t EthernetInterface::s_lpcNetifOff = 0;