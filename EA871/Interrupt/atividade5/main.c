/*
 * atividade5.c
 *
 * Created: 19/09/2019 08:54:17
 * Author : Giordano Gatti
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>


//configura endere�os de regitradores interrup��o:
unsigned char *peicra = (unsigned char *) 0x69; 
unsigned char *peimsk = (unsigned char *) 0x3D; 
unsigned char *ppcicr = (unsigned char *) 0x68; 
unsigned char *ppcmsk = (unsigned char *) 0x6C; 


//habilitar ddr's e os pin/port's:
unsigned char *pddrb = (unsigned char *) 0x24;
unsigned char *pportb = (unsigned char *) 0x25;


unsigned char *pddrc = (unsigned char *) 0x27;
unsigned char *ppinc = (unsigned char *) 0x26;


unsigned char *pddrd = (unsigned char *) 0x2A;
unsigned char *ppind = (unsigned char *) 0x29;

//variaveis auxiliares ao programa (velocidade, troca de sentido e contagem)
int s=1;
int v=1000;
int	c=0;

ISR(INT0_vect) { //muda velocidade, divide a mesma por dois a cada borda de subida detectada na interrup��o
	v = v/2;
}
	
ISR(PCINT1_vect){  //muda sentido, multiplica o parametro por -1(inverte a contagem do parametro) cada vez q a condi��o *ppinc and 0x01 for verdadeira, j� que para pcint o disparo da interrup��o sera dado em cada altera��o de borda, ou seja na subida E na descida.
	if(*ppinc &= 0x01) {
		s = s*(-1); }
	}

int main(void){
	
	
	*peicra |= 0x03; //configura eicra para detec��o de borda de subida
	*peimsk |= 0x01; //configura eimsk habilitando interrup��es da int0
	*ppcicr |= 0x02; //configura pcicr habilitando interrup��es para pcint8
	*ppcmsk |= 0x01; // configura pcmsk para o disparo de interrup��es a cada borda no int8 
	
	*pddrb |= 0x07; //configura ddrb como sa�da (leds)
	*pddrc &= 0xfe; //configura ddrc como entrada (bot�o)
	*pddrd &= 0xfb; //configura ddrd como entrada (bot�o)
	
	unsigned char estados[8] = {0x00, 0x01, 0x02, 0x04, 0x03, 0x06, 0x05, 0x07}; //define um vetor com os estados desejados sequencialmente
	extern void atraso (short int v); //declara a fun��o de atraso criada em assembly
	asm ("sei"); //fun��o em assembly que habilita todas as interrup��es 
	
    while (1) 
    {		
		*pportb = estados[c]; //coloca a sa�da como o estado desejado no vetor, (parametro c) em ordem para os leds
		c = c + s; //parametro c � alterado para contagem, crescente ou decrescente caso o sentido seja alterado (parametro s)
		if(c < 0) {c = c + 8;} //torna o vetor circular, de maneira que apos o estado final, o mesmo retorne ao estado inicial
		if(c == 8) {c = c - 8;}
					
		if(v<125){v=1000;} //torna o parametro v circular, de forma que ao se tornar menor que 125ms volte ao estado inicial de 1s
		atraso(v); //chama a fun��o de atraso com parametro v
			
    }
}

