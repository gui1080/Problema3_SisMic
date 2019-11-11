#include <msp430.h>
#include <stdio.h>
#include <string.h>

#include "../source/funcoes_auxiliares.c"
#include "../source/converte_string.c"
#include "../source/lcd.c"


int contador_de_leituras = 0;

uint16_t amostras_A0[8];
uint16_t amostras_A1[8];
uint16_t adcFinished;

float volatile valor_voltagem_A0;
float volatile valor_voltagem_A1;
int volatile valor_hex_A0;
int volatile valor_hex_A1;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P6DIR |= BIT6;              // habilita saida no P6.6 (LED VERDE)
    P6REN &= ~(BIT6);           // habilita resistor de pull up
    P6OUT &= ~(BIT6);           // zera saida

    TB0CTL = (TBSSEL__ACLK | MC__UP |  TBCLR);   // Clock

    TB0CCR0 = 0x1000;     // se 0x8000 conta 1 segundo, pegamos 1/8 disso para termos 8 amostras em 1 segundo

    TBCCTL0 = CCIE; // permite vermos a interrup��o (local)

    __enable_interrupt();           // habilitamos a interrupc�o do pragma vector

    adcFinished = 0;

    i2cInit(1, 0);
    lcdInit();

    char auxiliar_printa[5];
    char auxiliar_hex[4];

    while(1)
    {
        if (adcFinished) {
            adcFinished = 0;
            valor_voltagem_A0 = valor_normalizado_vetor(amostras_A0);
            valor_voltagem_A1 = valor_normalizado_vetor(amostras_A1);
            valor_hex_A0 = (int)(valor_voltagem_A0 * 1000);
            valor_hex_A1 = (int)(valor_voltagem_A1 * 1000);

            // adicionando valor_voltagem_A0 e valor_voltagem_A1 para watch expressions, podemos ver o valor do input

            lcdResetaCursor();

            //lcdPrint("A0: D,DDDV  HHHH\0");
            //lcdPrint("A1: D,DDDV  HHHH\0");

            lcdPrint("A0: ");
            float_para_string(valor_voltagem_A0, auxiliar_printa, 3);
            lcdPrint(auxiliar_printa);
            lcdPrint("V  ");
            convert_deci_hex(valor_hex_A0, auxiliar_hex);
            lcdPrint(auxiliar_hex);

            // aqui convertemos e printamos o valor em hexadecimal do que est� em valor_voltagem_A0 e A1 (floats). � para ser convertido em um char "auxiliar_printa2[4]"

            lcdPrint("\n");
            lcdPrint("A1: ");
            float_para_string(valor_voltagem_A1, auxiliar_printa, 3);
            lcdPrint(auxiliar_printa);
            lcdPrint("V  ");
            convert_deci_hex(valor_hex_A1, auxiliar_hex);
            lcdPrint(auxiliar_hex);

            lcdPrint("\n");
        }
    }

}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TB0_CCR0_ISR(){

            if (contador_de_leituras == 7) {        // se chegamos ao final das leituras, resetamos
                contador_de_leituras = 0;
                adcFinished = 1;
            }
            if(contador_de_leituras != 7){          // se nao chegamos, atualizamos a leitura ADC
                amostras_A0[contador_de_leituras] = adcRead(0);   // single channel, single convertion repetido 2 vezes
                amostras_A1[contador_de_leituras] = adcRead(1);
                contador_de_leituras++;
            }

            P6OUT ^= BIT6;                         // a luz pisca para termos no��o da frequencia da amostragem
                                                    // Cada vez que a luz pisca, cada canal fez 8  leituras

}
