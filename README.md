# Problema 3 do Lab de SisMic (2/2019)

 * Alunos: Guilherme Braga (17/0162290) e Gabriel Matheus (17/0103498)
 
 * Professor: Daniel Café
 
 * Turma: D
 
 * Link no github: https://github.com/therealguib545/Problema3_SisMic
 
 * Entrega: 13/11/2019
 

# Conexões

* LCD (LCD com PCF, comunicação I2C)

VCC -> 5V

GND -> GND

SDA -> P1.2

SCL -> P1.3


* JOYSTICK (funciona com dois potenciômetros, retornando valores em X e em Y)

SW -> (fio solto)

X -> P1.0

Y -> P1.1

VCC -> 3.3V

GND -> GND


* PARA TESTES SIMPLES EM CASA, 2 POTENCIÔMETROS (com uma protoboard e alguns jumpers)

(de frente para o direcional)

PINO DO MEIO -> P1.0/P1.1 (output)

PINO À DIREITA -> 3V3

PINO À ESQUERDA -> GND

* Aí acompanhamos os resultados da conversão ADC pela visualização das variáveis

