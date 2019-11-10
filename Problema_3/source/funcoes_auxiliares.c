#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

float valor_normalizado_vetor(uint16_t vector[8]) {

    float media = 0;
    int k;
    for (k = 0; k < 8; k++) {
        media += vector[k];
    }
    return ((media/8000)*0.92);       // 0.92 vem de uma regra de 3, o LCD estava lendo valores acima de 3,3V, então assim diminuimos o valor
    // dividimos por 8 pois temos 8 amostras
    // dividimos por mil pois os valores saiam na casa do milhares

}

// função de leitura ADC single channel, single convertion
uint16_t adcRead(uint8_t pin)
{
    ADCCTL0 = ADCSHT_6 | ADCON;                   // ligando ADC para podermos atualizar os proximos vetores
    ADCCTL1 = ADCSHS_0 | ADCCONSEQ_0 | ADCSHP;    // 1 canal, 1 conversão
    ADCCTL2 = ADCRES_2;                           // resolução do sinal de saída vai ser 12 bits

    ADCMCTL0 = pin;                      // Seleciona o canal

    P1SEL0 |= 1 << pin;                   // seleciona o pino lido
    P1SEL1 |= 1 << pin;


    ADCCTL0 |= ADCENC;                          // Habilita
    ADCCTL0 &= ~ADCSC;                          // Gera flanco de subida
    ADCCTL0 |= ADCSC;

    while(!(ADCIFG & ADCIFG0));                 // esperamos o final da conversão
    return ADCMEM0;                             // retornamos o espaço de memória com o resultado
}
