/* EthernetInterface.h */
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
 
#ifndef ETHERNETINTERFACE_H_
#define ETHERNETINTERFACE_H_

#include "core/fwk.h"

#include "rtos.h"

#include "LwIPInterface.h"

#include "lwip/netif.h"

 /** Interface using Ethernet to connect to an IP-based network
 *
 */
class EthernetInterface : public LwIPInterface
{
public:
  /** Instantiate the Ethernet interface.
  * 
  */
  EthernetInterface();

  /** Initialize the interface with DHCP.
  * Initialize the interface and configure it to use DHCP (no connection at this point).
  * \return 0 on success, a negative number on failure
  */
  int init(); //With DHCP

  /** Initialize the interface with a static IP address.
  * Initialize the interface and configure it with the following static configuration (no connection at this point).
  * \param ip the IP address to use
  * \param mask the IP address mask
  * \param gateway the gateway to use
  * \param dns1 the primary DNS server
  * \param dns2 the secondary DNS server
  * \return 0 on success, a negative number on failure
  */
  int init(const char* ip, const char* mask, const char* gateway, const char* dns1, const char* dns2); //No DHCP

  /** Connect
  * Bring the interface up, start DHCP if needed.
  * \return 0 on success, a negative number on failure
  */
  virtual int connect();
  
  /** Disconnect
  * Bring the interface down
  * \return 0 on success, a negative number on failure
  */
  virtual int disconnect();

private:
  static void netifStatusCb(struct netif *netif);
  
  static int32_t s_lpcNetifOff; //Offset between m_lpcNetif and this ... this might be quite kludgy but should work!
  
  struct netif m_lpcNetif;
  Semaphore m_netifStatusSphre;
  bool m_useDHCP;
};

//As a helper, include TCPSocket.h & UDPSocket.h here
#include "TCPSocket.h"
#include "UDPSocket.h"

#endif /* ETHERNETINTERFACE_H_ */
