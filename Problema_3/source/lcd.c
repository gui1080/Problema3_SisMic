#include <stdint.h>
#include <msp430.h>


uint8_t lcdAddr = 0x27;
const uint8_t lcdRS = 0x01;
const uint8_t lcdRW = 0x02;
const uint8_t lcdEN = 0x04;
const uint8_t lcdBL = 0x08;


// i2c usando interrupções

struct {
	uint8_t status;
	uint8_t data;
} UCB0;


//	P1.2 e P1.3 (SDA e SCL)

void i2cInit(uint8_t isMaster, uint8_t myAddr)
{
	UCB0CTLW0 = UCMODE_3 | UCSWRST;			// Modo: I2C
	UCB0CTLW1 = UCASTP_2;
	UCB0TBCNT = 1;
	// recebe 1 byte

	if (isMaster)							// Se for mestre
	{
		UCB0CTLW0 |= UCSSEL__SMCLK |		// Configura como Mestre
					 UCMST;
		UCB0BRW = 50;
	}
	else									// Se for escravo
	{
		UCB0I2COA0 = UCOAEN | myAddr;
		// Configura a varoavel que passamos
	}

	P1SEL0 |=  (BIT2 | BIT3);				// Seleciona funcionalidade dedicada
	P1SEL1 &= ~(BIT2 | BIT3);
	P1DIR  &= ~(BIT2 | BIT3);				// resist pull-up
	P1REN  |=  (BIT2 | BIT3);
	P1OUT  |=  (BIT2 | BIT3);

	UCB0CTLW0 &= ~UCSWRST;


	UCB0IE = UCRXIE	 |
			 UCTXIE  |
			 UCSTPIE |
			 UCNACKIE;
}


uint8_t i2cTst(uint8_t addr)
{
	UCB0.status = 0; 						// transmissão bem sucedida?

	UCB0I2CSA  = addr;
	UCB0CTLW0 |= UCTR + UCTXSTT + UCTXSTP;	// Pede start e stop simultâneos (não eniva bytes)


	__low_power_mode_0();					// Aguarda o final da comunicação
	return UCB0.status;
}


uint8_t i2cPut(uint8_t addr, uint8_t data)
{
	UCB0.status = 0;						// transmissão bem sucedida?

	UCB0I2CSA = addr;
	UCB0.data = data;


	UCB0CTLW0 |= UCTR + UCTXSTT;			// comunicação

	__low_power_mode_0();					// Aguarda o final da comunicação
	return UCB0.status;
}


uint8_t i2cGet(uint8_t addr, uint8_t *pData)
{
	UCB0.status = 0;						// transmissão bem sucedida?

	UCB0I2CSA = addr;						// Configura end. do escravo

	UCB0CTLW0 &= ~UCTR;
	UCB0CTLW0 |= UCTXSTT;

	__low_power_mode_0();

	*pData = UCB0.data;						// Grava o byte no ponteiro do usuário

	return UCB0.status;
}


void lcdWriteNibble(uint8_t nibble, uint8_t isChar)
{
	nibble <<= 4;							// Ajusta a posição do nibble
	nibble |= lcdBL | isChar;				// Backlight on


	i2cPut(lcdAddr, nibble |  0  );
	i2cPut(lcdAddr, nibble |lcdEN);
	i2cPut(lcdAddr, nibble |  0  );
}


void lcdWriteByte(uint8_t byte, uint8_t isChar)
{

	lcdWriteNibble(byte >> 4, isChar);
	// MSB
	lcdWriteNibble(byte 	, isChar);
	// LSB

}


uint8_t lcdReadNibble(int pos_cursor)
{
	uint8_t nibble = 0;
	uint8_t dado;

	lcdWriteByte(0x02, 0);					// Cursor primeira posição

	while(--pos_cursor)
	{
		lcdWriteByte(0x14, 0);				// Desloca cursor para direita
	}

	nibble |= lcdBL | lcdRW | lcdEN;        // Configura leitura

    i2cPut(lcdAddr, nibble |  0  );
    i2cPut(lcdAddr, nibble |lcdEN);
    i2cGet(lcdAddr, &dado);
    i2cPut(lcdAddr, nibble |  0  );

	return dado;
}


uint8_t lcdReadByte(int pos_cursor)
{

  uint8_t nibble1 = lcdReadNibble(pos_cursor) & 0xF0;
	uint8_t nibble2 = lcdReadNibble(pos_cursor);
	uint8_t byte;
	nibble2 >>= 4;

	byte = nibble1 | nibble2;

	return byte;
}


void lcdInit()
{
	if(i2cTst(lcdAddr))						// Verifica se o LCD está no end.
		lcdAddr = 0x3F;						// 0x27 ou 0x3F

	lcdWriteNibble(0x3, 0);					// Entra em modo 8-bits
	lcdWriteNibble(0x3, 0);
	lcdWriteNibble(0x3, 0);
	lcdWriteNibble(0x2, 0);					// Entra em modo 4-bits

	lcdWriteByte(0x28, 0);					// Configura: Duas linhas
	lcdWriteByte(0x0C, 0);					// Display ON, Cursor ON
	lcdWriteByte(0x06, 0);					// Modo de incremento

	lcdWriteByte(0x01, 0);					// Limpa a tela (cursor em 1ª pos)

}

void lcdClear()
{
	lcdWriteByte(0x01, 0);					// Limpa a tela (cursor em 1ª pos)
}

void lcdResetaCursor()
{
	lcdWriteByte(0x02, 0);
}

void lcdPrint(char *str)
{
	static uint8_t line = 0x80;

	while(*str)								// Enquanto o byte não for 0
	{
		
		if(*str == '\n')
		{
 			line ^= 0x40;
			lcdWriteByte(line, 0);
			str++;
		}
		else
			lcdWriteByte(*str++, 1);

	}
}


#pragma vector = USCI_B0_VECTOR
__interrupt void I2C_ISR()
{
	switch (UCB0IV)
	{
		case UCIV__UCNACKIFG:				// Se recebeu um NACK
			UCB0CTLW0 |= UCTXSTP;			// Pede um stop
			UCB0.status = 1;				// E avise o usuário do nack
			break;

		case UCIV__UCSTPIFG:				// Se enviou o stop
			__low_power_mode_off_on_exit();	// volta para o programa principal
			break;							// saindo do modo de baixo consumo

		case UCIV__UCRXIFG0:				// Se recebeu algo no buffer
			UCB0.data = UCB0RXBUF;			// do usuário
			break;

		case UCIV__UCTXIFG0:				// Se for transmitir algo, pegue
			UCB0TXBUF = UCB0.data;			// o byte da posição indicada  pelo
			break;							// ponteiro do usuário

		default:
			break;
	}
}
