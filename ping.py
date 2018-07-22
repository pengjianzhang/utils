#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import time, threading
import os

import random

import pprint
import os
import select
import socket
import struct
import sys
import time

ICMP_ECHO	    = 8
ICMP_ECHOREPLY  = 0

"""

struct icmphdr {
  uint8_t		type;
  uint8_t		code;
  uint16_t	checksum;
  union {
	struct {
		uint16_t	id;
		uint16_t	sequence;
	} echo;
	uint32_t	gateway;
	struct {
		uint16_t	__unused;
		uint16_t	mtu;
	} frag;
  } un;
};

"""


def checksum(source_string):
    """
    I'm not too confident that this is right but testing seems
    to suggest that it gives the same answers as in_cksum in ping.c
    """
    sum = 0
    count_to = (len(source_string) / 2) * 2
    for count in xrange(0, count_to, 2):
        this = ord(source_string[count + 1]) * 256 + ord(source_string[count])
        sum = sum + this
        sum = sum & 0xffffffff # Necessary?

    if count_to < len(source_string):
        sum = sum + ord(source_string[len(source_string) - 1])
        sum = sum & 0xffffffff # Necessary?

    sum = (sum >> 16) + (sum & 0xffff)
    sum = sum + (sum >> 16)
    answer = ~sum
    answer = answer & 0xffff

    # Swap bytes. Bugger me if I know why.
    answer = answer >> 8 | (answer << 8 & 0xff00)

    return answer

def ping_recv_epoll(sk, id, timeout):
    """
    Receive the ping from the socket.
    """
    ret = False
    epoll = select.epoll(1)
    epoll.register(sk, select.EPOLLIN)

    a = time.time()
    events = epoll.poll(timeout)
    b = time.time()
    print a,b
    for fileno, event in events:  
        packet, addr = sk.recvfrom(1024)
        if packet and (len(packet) >= 28): 
            icmpHeader = packet[20:28]
            type, code, csum, id2, seq = struct.unpack("bbHHh", icmpHeader)
            if (id == id2 ) and (type ==  ICMP_ECHOREPLY):
                ret = True

    epoll.close()
    return ret




def ping_recv(sk, id, timeout):
    """
    Receive the ping from the socket.
    """

    events = select.select([sk], [], [], timeout)

    #timeout
    if not events[0]:
        return False

    packet, addr = sk.recvfrom(1024)
    if packet and (len(packet) >= 28): 
        icmpHeader = packet[20:28]
        type, code, csum, id2, seq = struct.unpack("bbHHh", icmpHeader)
        if (id == id2 ) and (type ==  ICMP_ECHOREPLY):
            return True 

    return False

def ping_send(sk, dest_addr, id):
    """
    Send one ping to the given >dest_addr<.
    """

    psize = 64
    # Remove header size from packet size
    psize = psize - 8

    # Header is type (8), code (8), checksum (16), id (16), sequence (16)
    my_checksum = 0

    # Make a dummy heder with a 0 checksum.
    header = struct.pack("bbHHh", ICMP_ECHO, 0, my_checksum, id, 1)
    bytes = struct.calcsize("d")
    data = (psize - bytes) * "Q"
    data = struct.pack("d", time.time()) + data

    # Calculate the checksum on the data and the dummy header.
    my_checksum = checksum(header + data)

    # Now that we have the right checksum, we put that in. It's just easier
    # to make up a new header than to stuff it into the dummy.
    header = struct.pack(
        "bbHHh", ICMP_ECHO, 0, socket.htons(my_checksum), id, 1
    )
    packet = header + data
    sk.sendto(packet, (dest_addr, 1)) # Don't know about the 1


def ping_check(dest_addr, timeout):
    try:
        sk = socket.socket(socket.AF_INET, socket.SOCK_RAW, 1)
    except socket.error, (errno, msg):
        return False 

    icmp_id = sk.fileno()
    ping_send(sk, dest_addr, icmp_id)
    ret = ping_recv(sk, icmp_id, timeout)
    sk.close()

    return ret 



# 新线程执行的代码:
def loop():
   
    wait = random.random()
    time.sleep(wait)
    name = threading.current_thread().name
    i = 0
    while i < 4:
        ret = ping_check(name, 0.3)
        print name,ret
        time.sleep(1)
        i = i + 1


def test():
    t1 = threading.Thread(target=loop, name='192.168.1.1')
    t2 = threading.Thread(target=loop, name='1.1.1.1')
    t3 = threading.Thread(target=loop, name='1.1.1.2')
    t4 = threading.Thread(target=loop, name='202.38.64.1')

    t1.start()
    t2.start()
    t3.start()
    t4.start()

    t1.join()
    t2.join()
    t3.join()
    t4.join()


test()

