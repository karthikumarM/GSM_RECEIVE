#include <pic.h>
#include <string.h>
void display_data(const char);
void display_cmd(int);
void uart_tx(const char *);
void uart_tx_char(char);
__CONFIG(0x2CE4);
#define _XTAL_FREQ 4000000
#define RS RE0
#define EN RE1
#define LCD PORTD

int count = 0, data_count = 0, data_received = 0, store_value = 0;
char rx_value[30], temp;

void sms_text_mode()
{

	uart_tx("AT");
	uart_tx_char(0X0D);
	uart_tx_char(0X0A);
	__delay_ms(100);

	uart_tx("AT+CMGF=1");
	uart_tx_char(0X0D);
	uart_tx_char(0X0A);
	__delay_ms(100);

	uart_tx("AT+CNMI=2,2,0,0,0");
	uart_tx_char(0X0D);
	uart_tx_char(0X0A);
	__delay_ms(100);
}

void interrupt uart_rx()
{
	if (RCIF == 1)
	{
		temp = RCREG;
		{
			if (store_value == 2)
			{
				rx_value[data_count] = temp;
				data_received = 1;
				data_count++;
			}

			if (temp == 0x0A)
			{
				store_value++;
			}
			if (temp == 0x0D)
			{
				rx_value[data_count - 1] = '\0';
				store_value = 0;
			}
		}
	}
	RCIF = 0;
	temp = 0;
}
void uart_tx(const char *send_data)
{
	while (*send_data)
	{
		TXREG = *send_data;
		send_data++;
		while (TXIF == 0);
		TXIF = 0;
		__delay_ms(1000);
	}
}
void uart_tx_char(char send_data)
{

	TXREG = send_data;
	while (TXIF == 0);
	TXIF = 0;
	__delay_ms(1000);
}

void string_clear()
{
	for (int str_clr = 0; str_clr < 30; str_clr++)
	{
		rx_value[str_clr] = 0;
	}
}

void pulse()
{
	EN = 1;
	__delay_ms(10);
	EN = 0;
	__delay_ms(10);
}

void display_cmd(int cmd)
{
	RS = 0;
	LCD = cmd;
	pulse();
}

void display_data(const char data)
{
	RS = 1;
	LCD = data;
	pulse();
}

void display_str(const char *data)
{
	while (*data)
	{
		RS = 1;
		LCD = *data++;
		pulse();
	}
}
void main()
{
	TRISC6 = 0;
	TRISC7 = 1;
	PORTA = 0X00;
	TRISA = 0X00;
	PORTC = 0x00;
	TRISD = 0x00;
	PORTD = 0x00;
	TRISE = 0X00;
	PORTE = 0X00;
	TXSTA = 0X26;
	RCSTA = 0X90;
	SPBRG = 25;
	ANSEL = 0x00;
	ANSELH = 0x00;
	display_cmd(0x38);
	display_cmd(0x0E);
    __delay_ms(1000);
    sms_text_mode();
	/* Interrupt init */
	GIE = PEIE = RCIE = 1;
	display_cmd(0x80);
	display_cmd(0x01);
	display_str("GSM CHECK");
	// RA0=1;
	while (1)
	{
		if (data_received == 1)
		{
			display_cmd(0x01);
			display_cmd(0x80);
			display_str("RECIVED MSG");

			if ((rx_value[0] == 'o') && (rx_value[1] == 'n'))
			{
				RA0 = 1;
			}
			if ((rx_value[0] == 'o') && (rx_value[1] == 'f') && (rx_value[2] == 'f'))
			{
				RA0 = 0;
			}
			display_cmd(0xC0);
			display_str(rx_value);
			data_count = 0;
			count = 0;
			string_clear();
			data_received = 0;
		}
		__delay_ms(1000);
	}
}