/*
 * atividade7.c
 *
 * Created: 05/10/2019 06:53:09
 * Author : Giordano Gatti
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#define F_CPU 16000000UL
#include <util/delay.h>


volatile char buffer[10]; //configurando o buffer e seus respectivos parametros
unsigned int i=0; //proxima posi��o livre para inser��o do buffer
unsigned int j=0; //posi��o mais antiga a ser removida do buffer
volatile int b=0; //numero de elementos presentes no buffer

char recado[100]; //vetor para transmiss�o de recados(suporta strings com ate 100 caracteres, a escolha do valor 100 foi arbitraria, havia a necessidade apenas de ser um valo maior que a quantidade de caracteres presentes nas mensagens.
volatile int flag = 1; //variavel flag para controle da transmiss�o da mensagem
volatile int n = 0; //contador para transmitir os caracteres um a um da mensagem

//------------------------------------------//

unsigned char *pubrroh = (unsigned char *) 0xC5; //configurando os endere�os dos registradores da USART
unsigned char *pubrrol = (unsigned char *) 0xC4;
unsigned char *pucsroa = (unsigned char *) 0xC0;
unsigned char *puscrob = (unsigned char *) 0xC1;
unsigned char *puscroc = (unsigned char *) 0xC2;
unsigned char *pudr0 = (unsigned char *) 0xC6;

//------------------------------------------//

unsigned char * pddrb = (unsigned char*) 0x24; //configurando os endere�os das sa�das para o led
unsigned char * pportb = (unsigned char*) 0x25;

//------------------------------------------//

void adicionar_buffer(char c) { //fun��o adicionar buffer, utilizada do experimento 2, porem com buffer de tamanho 10
	if(b < 10){
		buffer[i] = c;
		i++;
		b++;
		if(i==10) {i=0;
		}
	}
}

volatile int saidabuffer = 0;
void remover_buffer() { //fun��o remover buffer tamb�m reutilizada do experimento 2, por�m com buffer de 10 posi��es e gerando uma variavel que gurda o valor do caratere que foi removido do buffer
	if(b > 0){
		saidabuffer = buffer[j];
		j++;
		b = b - 1;
		if (j==10) {j=0;
		}
	}
}

//------------------------------------------//

ISR(USART_TX_vect){ // interrup��o de transmissao usart
if (recado[n] != '\0'){ //enquanto o caractere a ser transmitido n�o for o fim da string, a mensagem vai sendo transmitida caractere por caractere, sendo n o contador
	*pudr0 = recado[n];//caractere � colocado em udr0
	n++; //contador incrementado
}
else {n = 0; //ao chegar no fim da mensagem o contador � reiniciado e o controle das flags � setado, para interromper a transmiss�o
	flag = 1;
}
}

ISR(USART_RX_vect){ //interrup�ao de recep��o da usart
adicionar_buffer(*pudr0); //ao acionada, faz com que o elemento recebido seja adicionado ao buffer com a fun��o adicionar buffer
}

//------------------------------------------//


void cores() {

unsigned char estados[8] = {0x00, 0x01, 0x02, 0x04, 0x03, 0x06, 0x05, 0x07};//vetor com todos os estados(combin��es de cores RGB) que o led utilizar�, por�m localizado dentro da fun��o

	if (saidabuffer == 'r') { //dependendo da letra que ser� retirada do buffer e colocada na variavel saidabuffer, o LED(sa�da da portb) assumir� o estado desejado, al�m de copiar a mensagem que ser� transmitida em uma varivel recado.
		*pportb = estados[1];
		strcpy(recado, "Comando: Acender LED - cor vermelha\n");
	}

	else if (saidabuffer == 'g') {
		*pportb = estados[2];
		strcpy(recado, "Comando: Acender LED - cor verde\n");
	}

	else if (saidabuffer == 'b') {
		*pportb = estados[3];
		strcpy(recado, "Comando: Acender LED - cor azul\n");
	}

	else if (saidabuffer == 'y') {
		*pportb = estados[4];
		strcpy(recado, "Comando: Acender LED - cor amarela\n");
	}

	else if (saidabuffer == 'c') {
		*pportb = estados[5];
		strcpy(recado, "Comando: Acender LED - cor ciano\n");
	}

	else if (saidabuffer == 'm') {
		*pportb = estados[6];
		strcpy(recado, "Comando: Acender LED - cor magenta\n");
	}

	else if (saidabuffer == 'w') {
		*pportb = estados[7];
		strcpy(recado, "Comando: Acender LED - cor branca\n");
	}
	else {*pportb = estados[0];
	strcpy(recado, "Comando incorreto\n");} //caso a letra digitada n�o seja compativel com nenhuma das que equivalentes as cores que o led pode assumir o led permanece desligado e exibe a mensagem de comando incorreto
		
_delay_ms(200); //atraso de 200ms
}

//------------------------------------------//


int main(void){

*pubrroh = 0x00;
*pubrrol = 103; //pubrrol e pubrroh configuram o baud rate da transmiss�o, nesse caso, para baud de 9600  o parametro colocado no registrador � 103
*pucsroa &= 0xFC; // for�a os dois ultimos bits do registrador para 0, configurando velocidade de transsmiss�o normal e modo multiprocessador desativado
*puscrob = 0xD8; //mant�m as interrup��es da USART () o 9� bit de transss�o desativados, e ativa o modo transmissor e receptor da USART.
*puscroc = 0x0E; // configura a USART como assincrona, com tamanho de frame de 8 bits, sem o uso de bits de paridade. Tamb�m � configurado o uso de dois bits de parada.

*pddrb |= 0x07; //s�o setados os tres primeiros bits do ddrb como saidas
sei(); //habilita todas as interrup��es 

unsigned char estados[8] = {0x00, 0x01, 0x02, 0x04, 0x03, 0x06, 0x05, 0x07}; //vetor com todos os estados que o led utilizar�
    
    while (1) 
    {
		if (flag == 1){ //caso o controle da transmiss�o estiver setado, o registrador de dados da usar recebe fim de string, e a flag de controle vai para 0, podendo dar inicio a uma nova transmiss�o
			*pudr0 = '\0';
			flag = 0;
		}
		if (b > 0) { //caso o buffer n�o esteja vazio, o mesmo remove os valores que foram colocados no buffer, um a um, e s�o jogados na variavel saidabuffer, a partir da qual cada um dos elementos ser� identificado e ser� entrada para a fun��o cores, na qual ser� selecionada a mensagem e o estado de cor do led, correspondente
			remover_buffer();
			cores();
		}
		else {*pportb = estados[0]; //enqunto o buffer estiver vazio, o led permanecer� apagado e o recado dado ser� vazio.
			strcpy(recado, "Vazio!\n");
		}
	}
}
