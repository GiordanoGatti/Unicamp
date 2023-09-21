/*
 * atividade 9.c
 *
 * Created: 04/11/2019 14:59:02
 * Author : Giordano Gatti
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

int n = 0; //variavel que ser� o contador para incrementar o valor de OCR2B
int w = 0; //variavel indicadora do estado do led, acendendo(0) ou apagando(1)

ISR(TIMER2_OVF_vect){ //interrup��o de overflow do timer, gerada cada vez que o timer atinge o valor de contagem contido no registrador OCR2A (considerando o prescaler utilizado de 1/8, e o valor de contagem de OCR2A de 200  --> periodo = 200 * [1/(16M/8)] = 0,1ms 
	n++; //a cada interrup��o de overflow gerada pelo timer o contador n � incrementado
	if (n == 50 && w == 0){OCR2B++; //sabendo que o o registrador OCR2B deve ser incrementado(enquanto o led esta no processo de acendimento, ou seja variavel w=0) de 0 at� 200 (mesmo valor de OC2A), dentro de 1s(de acordo com o periodo 10000 interrup�oes), o contador n ser� utilizado como parametro para acertar a contagem de OCR2B --> 10000/200 = 50, ou seja, o contador n ir� de 0 at� 50, e ent�o OCR2B ser� incrementado 1 vez, sendo incrementado 200 vezes no total(em 1 s)
						  n = 0;} //cada vez que o contador n chega at� o valor 50, o mesmo � levado a zero, iniciando a contagem novamente
	if (OCR2B == 200) {w = 1;} //quando OCR2B chega no valor igual ao de OCR2A, o mesmo deve iniciar o processo de apagar o led, levando a variavel w para 1.
		
	if (n == 50 && w == 1) {OCR2B--;// o mesmo mecanismo funciona para o processo de apagar o led, ou seja w=1. e ao inv�s de se incrementar, o valor de OCR2B � decrementado at� chegar em 0, valor no qual o processo do led muda , mudando a variavel w e reiniciando todo o processo.
							n = 0;}
	if(OCR2B == 0) {w = 0;}
}


int main(void)
{
	DDRD |= 0b00001000; //habilita pino 3 do arduino como sa�da
	TCCR2A |= 0b00100011; //habilita o modo fast PWM e o modod n�o inverso da compara��o de OCR2B
	TCCR2B |= 0b00001010; // habilita o prescaler de 1/8, e tamb�m habilita o modod fast PWM
	TIMSK2 |= 0b00000001; //habilita interrup��o do tipo overflow
	OCR2B = 0; // inicia em zero, por�m ser� incrementado at� o valor de OCR2A, mudando graduamente a porcentagem de tens�o mandada para a sa�da, de acordo com a compara��o entre o valor do mesmo e do registrador OCR2A.
	OCR2A = 200; // parametro de contagem de tempo para definir o periodo da interrup��o ----> periodo = -->(200)    * [1/(16M/8)] = 0,1ms
	sei(); //habilita todas as interrup��es
	
	
    while (1) 
    {   }
}

