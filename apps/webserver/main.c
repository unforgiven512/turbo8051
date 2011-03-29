//********************************************************************************************
//
// AVRnet firmware Version 1.0
//
// MCU : ATMEGA32 @ 16MHz
// Ethernet controller : ENC28J60
// IDE & Compiler : AVR Studio version 4.13.528 & WINAVR version 20070122
// Author : Jirawat Kongkaen
// Website : http://www.avrportal.com/
//
//********************************************************************************************
//
// File : main.c main program for AVRnet development board.
//
//********************************************************************************************
//
// Copyright (C) 2007
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
// This program is distributed in the hope that it will be useful, but
//
// WITHOUT ANY WARRANTY;
//
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin St, Fifth Floor, Boston, MA 02110, USA
//
// http://www.gnu.de/gpl-ger.html
//
//********************************************************************************************
#include "includes.h"

// Global variables
MAC_ADDR avr_mac;
IP_ADDR avr_ip;

MAC_ADDR server_mac;
IP_ADDR server_ip;


//BYTE generic_buf[128];

// Change your avr and server ip address here
// avr and server ip address are stored in eeprom
BYTE ee_avr_ip[4]  = { 10, 1, 1, 1 };
BYTE ee_server_ip[4]  = { 10, 1, 1, 76 };


void _delay_ms( int iDelay );
//*****************************************************************************************
//
// Function : server_process
// Description : Run web server and listen on port 80
//
//*****************************************************************************************
void server_process ( void )
{
	MAC_ADDR client_mac;
	IP_ADDR client_ip;
	// you can change rx,tx buffer size in includes.h
	BYTE rxtx_buffer[MAX_RXTX_BUFFER];
	WORD plen;
	
	if ( flag1.bits.syn_is_sent )
		return;
	// get new packet
	plen = enc28j60_packet_receive( (BYTE*)&rxtx_buffer, MAX_RXTX_BUFFER );
	
	//plen will ne unequal to zero if there is a valid packet (without crc error)
	if(plen==0)
		return;

	// copy client mac address from buffer to client mac variable
	memcpy ( (BYTE*)&client_mac, &rxtx_buffer[ ETH_SRC_MAC_P ], sizeof(MAC_ADDR) );
	
	// check arp packet if match with avr ip let's send reply
	if ( arp_packet_is_arp( rxtx_buffer, ARP_OPCODE_REQUEST_V ) )
	{
		arp_send_reply ( (BYTE*)&rxtx_buffer, (BYTE*)&client_mac );
		return;
	}

	// get client ip address
	memcpy ( (BYTE*)&client_ip, &rxtx_buffer[ IP_SRC_IP_P ], sizeof(IP_ADDR) );
	// check ip packet send to avr or not?
	if ( ip_packet_is_ip ( (BYTE*)&rxtx_buffer ) == 0 )
	{
		return;
	}

	// check ICMP packet, if packet is icmp packet let's send icmp echo reply
	if ( icmp_send_reply ( (BYTE*)&rxtx_buffer, (BYTE*)&client_mac, (BYTE*)&client_ip ) )
	{
		return;
	}

	// check UDP packet
	if (udp_receive ( (BYTE *)&rxtx_buffer, (BYTE *)&client_mac, (BYTE *)&client_ip ))
	{
		return;
	}
	
	// tcp start here
	// start web server at port 80, see http.c
	http_webserver_process ( (BYTE*)rxtx_buffer, (BYTE*)&client_mac, (BYTE*)&client_ip );
}
//*****************************************************************************************
//
// Function : main
// Description : main program, 
//
//*****************************************************************************************
void lcd_backlight( void )
{
	

	
}
//*****************************************************************************************
//
// Function : main
// Description : main program, 
//
//*****************************************************************************************
int main (void)
{
	// change your mac address here
	avr_mac.byte[0] = 'A';
	avr_mac.byte[1] = 'V';
	avr_mac.byte[2] = 'R';
	avr_mac.byte[3] = 'P';
	avr_mac.byte[4] = 'O';
	avr_mac.byte[5] = 'R';

	// read avr and server ip from eeprom
	//eeprom_read_block ( &avr_ip, ee_avr_ip, 4 );
	//eeprom_read_block ( &server_ip, ee_server_ip, 4 );
	
	// setup port as input and enable pull-up
	//SW_DDR &= ~ ( _BV( SW_MENU ) | _BV( SW_EXIT ) | _BV( SW_UP ) | _BV( SW_DW ) );
	//SW_PORT |= _BV( SW_MENU ) | _BV( SW_EXIT ) | _BV( SW_UP ) | _BV( SW_DW );
	//SFIOR &= ~_BV( PUD );

	// setup lcd backlight as output
	//LCD_BL_DDR |= _BV( LCD_BL_PIN );
	// lcd backlight on
	//LCD_BL_PORT |= _BV( LCD_BL_PIN );
	
	// setup clock for timer1
	//TCCR1B = 0x01;	// clk/1 no prescaling

	// initial lcd, and menu
	lcd_init ();
	menu_init ();

	// set LED1, LED2 as output */
	//LED_DDR |= _BV( LED_PIN1_DDR ) | _BV( LED_PIN2_DDR );
	// set LED pin to "1" ( LED1,LED2 off)
	//LED_PORT |= _BV( LED_PIN1 ) | _BV( LED_PIN2 );

	// initial enc28j60
	enc28j60_init( (BYTE*)&avr_mac );
	
	// loop forever
	for(;;)
	{
		// wait until timer1 overflow
		//while ( (TIFR & _BV ( TOV1 )) == 0 );
		//TIFR |= _BV(TOV1);
		//TCNT1 = 1536;	// Timer1 overflow every 1/16MHz * ( 65536 - 1536 ) = 4ms, 250Hz

		// general time base, generate by timer1
		// overflow every 1/250 seconds
		time_base ();
		
		// server process response for arp, icmp, http
		server_process ();

		// send temparature to web server unsing http protocol
		// disable by default.
		//client_process ();

		// lcd user interface menu
		// setup IP address, countdown timer
		menu_process ();

		// display AVR ethernet status
		// temparature, AVR ip, server ip, countdown time
		standby_display ();
	}

	return 0;
}

// Need to add the Delay
void _delay_ms (int iDelay) {


}
