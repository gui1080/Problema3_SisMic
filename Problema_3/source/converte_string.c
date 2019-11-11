#include <math.h>
#include <stdio.h>

void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];            // colocamos o valor na ordem correta
        str[j] = temp;
        i++;
        j--;
    }
}

int integer_pra_string(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';              // pegamos o restante em modulos de 10
        x = x / 10;
    }

    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

void float_para_string(float n, char* res, int afterpoint)
{
    int ipart = (int)n;   // pegamos a parte inteira

    float fpart = n - (float)ipart;        // parte de ponto flutuantes

    int i = integer_pra_string(ipart, res, 1);      // valor em inteiro

    if (afterpoint != 0) {                          // casas decimais apos a virgula
        res[i] = ',';

        fpart = fpart * pow(10, afterpoint);

        integer_pra_string((int)fpart, res + i + 1, afterpoint);
    }
}
