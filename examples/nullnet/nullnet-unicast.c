/*---------------------------------------------------------------------------*/
/*
 * Copyright (c) 2017, RISE SICS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         NullNet unicast example
 * \author
*         Simon Duquennoy <simon.duquennoy@ri.se>
 *
 */

#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/leds.h"
#include "dev/button-hal.h"
#include "random.h"

#include <string.h>
#include <stdio.h> /* For printf() */

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
static clock_time_t rtt_start, rtt_end; // Get the system time.
static unsigned int num, num_buf;
static linkaddr_t dest_addr =         {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
static linkaddr_t coordinator_addr =  {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
#endif /* MAC_CONF_WITH_TSCH */

/*---------------------------------------------------------------------------*/
PROCESS(nullnet_example_process, "NullNet unicast example");
AUTOSTART_PROCESSES(&nullnet_example_process);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src_addr, const linkaddr_t *dest_adrr)
{
  if(len == sizeof(unsigned)) {
    unsigned int recv_num;
    memcpy(&recv_num, data, sizeof(recv_num));
    printf("Here\n");
    if (recv_num == num) {
      rtt_end = clock_time();
      LOG_INFO("Received %u\n", recv_num);
      LOG_INFO("RTT TIME : %lx\n", rtt_end-rtt_start);
    } else {
      num_buf = recv_num;
      NETSTACK_NETWORK.output(src_addr);
    }
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nullnet_example_process, ev, data)
{
  PROCESS_BEGIN();

#if MAC_CONF_WITH_TSCH
  tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
#endif /* MAC_CONF_WITH_TSCH */

  random_init(3);

  /* Initialize NullNet */
  nullnet_buf = (uint8_t *)&num;
  nullnet_len = sizeof(num);
  nullnet_set_input_callback(input_callback);

  if(!linkaddr_cmp(&dest_addr, &linkaddr_node_addr)) {
    while(1) {
      PROCESS_YIELD();
      if (ev == button_hal_press_event) {
        num = random_rand();
        num_buf = num;
        rtt_start = clock_time();
        LOG_INFO("Sending %lx\n",num);
        NETSTACK_NETWORK.output(&dest_addr);
      }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
