/*
 * Atividade10.c
 *
 * Created: 07/11/2019 09:07:37
 * Author : Giordano Gatti
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>


volatile int n = 0; // contador que passara cada um dos valores dos vetores das formas de onda
volatile int m = 1; // contador que passara cada caractere das strings a serem transmitidas
volatile int led = 0; //variavel que indica se o led está aceso ou apagado(1 aceso, 0 apagado)
volatile int x = 0; // contador que fará a comparação com contmax para contar o periodo no qual o led ficara aceso e apagado
volatile int contmax = 0; //valor maximo de contagem de x para que o led pisque nas frequencias desejadas
volatile int l = 0; //variavel que habilita ou não o led piscar
volatile unsigned char letrarecebida; //variavel que receberá o caractere da recepção da uart
char msgtrans[50]; //vetor de transmissão de string com tamanho de ate 50 caracteres


unsigned char senoidal[200] = {128, 132, 136, 139, 143, 147, 151, 155, 159, 163, 167, 171, 174, 178, 182, 185, 189, 192, 196, 199, 202, 206, 209, 212, 215, 218, 220, 223, 226, 228, 231, 233, 235, 237, 239, 241, 243, 245, 246, 247, 249, 250, 251, 252, 253, 253, 254, 254, 255, 255, 255, 255, 255, 254, 254, 253, 253, 252, 251, 250, 249, 247, 246, 245, 243, 241, 239, 237, 235, 233, 231, 228, 226, 223, 220, 218, 215, 212, 209, 206, 202, 199, 196, 192, 189, 185, 182, 178, 174, 171, 167, 163, 159, 155, 151, 147, 143, 139, 136, 132, 128, 123, 119, 116, 112, 108, 104, 100, 96, 92, 88, 84, 81, 77, 73, 70, 66, 63, 59, 56, 53, 49, 46, 43, 40, 37, 35, 32, 29, 27, 24, 22, 20, 18, 16, 14, 12, 10, 9, 8, 6, 5, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 5, 6, 8, 9, 10, 12, 14, 16, 18, 20, 22, 24, 27, 29, 32, 35, 37, 40, 43, 46, 49, 53, 56, 59, 63, 66, 70, 73, 77, 81, 84, 88, 92, 96, 100, 104, 108, 112, 116, 119, 123}; //vetor de valores da onda senoidal obtido pelo matlab (valores de tensão entre 0 e 5 volts convertidos nos respectivos valores UDR2A que equivalem a esses respectivos valores de tensão em cada estagio da onda)
unsigned char triangular[200] = {0, 3, 5, 8, 10, 13, 15, 18, 20, 23, 26, 28, 31, 33, 36, 38, 41, 43, 46, 48, 51, 54, 56, 59, 61, 64, 66, 69, 71, 74, 77, 79, 82, 84, 87, 89, 92, 94, 97, 99, 102, 105, 107, 110, 112, 115, 117, 120, 122, 125, 128, 130, 133, 135, 138, 140, 143, 145, 148, 150, 153, 156, 158, 161, 163, 166, 168, 171, 173, 176, 179, 181, 184, 186, 189, 191, 194, 196, 199, 201, 204, 207, 209, 212, 214, 217, 219, 222, 224, 227, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252, 255, 252, 250, 247, 245, 242, 240, 237, 235, 232, 230, 227, 224, 222, 219, 217, 214, 212, 209, 207, 204, 201, 199, 196, 194, 191, 189, 186, 184, 181, 179, 176, 173, 171, 168, 166, 163, 161, 158, 156, 153, 150, 148, 145, 143, 140, 138, 135, 133, 130, 128, 125, 122, 120, 117, 115, 112, 110, 107, 105, 102, 99, 97, 94, 92, 89, 87, 84, 82, 79, 77, 74, 71, 69, 66, 64, 61, 59, 56, 54, 51, 48, 46, 43, 41, 38, 36, 33, 31, 28, 26, 23, 20, 18, 15, 13, 10, 8, 5, 3}; // vetor da onda triangular
unsigned char dente_serra[200] = {0, 1, 3, 4, 5, 6, 8, 9, 10, 11, 13, 14, 15, 17, 18, 19, 20, 22, 23, 24, 26, 27, 28, 29, 31, 32, 33, 34, 36, 37, 38, 40, 41, 42, 43, 45, 46, 47, 48, 50, 51, 52, 54, 55, 56, 57, 59, 60, 61, 62, 64, 65, 66, 68, 69, 70, 71, 73, 74, 75, 77, 78, 79, 80, 82, 83, 84, 85, 87, 88, 89, 91, 92, 93, 94, 96, 97, 98, 99, 101, 102, 103, 105, 106, 107, 108, 110, 111, 112, 113, 115, 116, 117, 119, 120, 121, 122, 124, 125, 126, 128, 129, 130, 131, 133, 134, 135, 136, 138, 139, 140, 142, 143, 144, 145, 147, 148, 149, 150, 152, 153, 154, 156, 157, 158, 159, 161, 162, 163, 164, 166, 167, 168, 170, 171, 172, 173, 175, 176, 177, 179, 180, 181, 182, 184, 185, 186, 187, 189, 190, 191, 193, 194, 195, 196, 198, 199, 200, 201, 203, 204, 205, 207, 208, 209, 210, 212, 213, 214, 215, 217, 218, 219, 221, 222, 223, 224, 226, 227, 228, 230, 231, 232, 233, 235, 236, 237, 238, 240, 241, 242, 244, 245, 246, 247, 249, 250, 251, 252, 254}; // vetor da onda dente de serra
unsigned char am[200] = {128, 141, 155, 168, 181, 192, 203, 211, 218, 223, 226, 226, 223, 218, 211, 201, 189, 176, 161, 145, 128, 110, 93, 76, 60, 46, 34, 23, 16, 10, 8, 9, 13, 19, 29, 41, 55, 71, 89, 108, 127, 147, 166, 185, 202, 217, 230, 241, 249, 253, 255, 253, 249, 241, 230, 217, 202, 185, 166, 147, 128, 108, 89, 71, 55, 41, 29, 19, 13, 9, 8, 10, 16, 23, 34, 46, 60, 76, 93, 110, 127, 145, 161, 176, 189, 201, 211, 218, 223, 226, 226, 223, 218, 211, 203, 192, 181, 168, 155, 141, 128, 114, 102, 91, 81, 72, 65, 60, 57, 55, 56, 58, 62, 67, 73, 81, 90, 99, 108, 118, 127, 137, 145, 153, 160, 166, 171, 175, 177, 178, 178, 177, 174, 171, 166, 161, 155, 148, 142, 135, 128, 121, 114, 108, 102, 97, 93, 89, 87, 86, 85, 86, 87, 89, 93, 97, 102, 108, 114, 121, 127, 135, 142, 148, 155, 161, 166, 171, 174, 177, 178, 178, 177, 175, 171, 166, 160, 153, 145, 137, 128, 118, 108, 99, 90, 81, 73, 67, 62, 58, 56, 55, 57, 60, 65, 72, 81, 91, 102, 114}; // vetor da onda AM


void trocaled(){ // função definida de maneira a trocar o estado do led, utilizando a variavel 'led' como parametro do estado do led, se 0 apagado, se 1 aceso. caso o led estiver apagado, ao acionar a função o mesmo é colocado em nivel alto, e a porta equivalente ao led também. caso o led já estiver aceso, a variavel é levada para nivel baixo bem como sua porta correspondente
	
	if (led == 0){PORTB |= 0b00100000;
	led = 1;}
	else if (led == 1){PORTB &= 0b11011111;
	led = 0;}
}


ISR(USART_RX_vect){ //interrupção de recepção da uart 
	letrarecebida = UDR0; //string recebida pela uart em UDR0 é transsferida para a variavel letrarecebida
	
	if(letrarecebida == 's') {strcpy(msgtrans, "Onda senoidal\n"); UDR0 = msgtrans[0]; contmax = 62500; l = 1;} //de acordo com o caractere contido em letrarecebida, a string que se deseja transmitir é copiada para a variavel msgtrans, e então o primeiro valor (caractere 0 da string) já é transmitido para UDR0 para ser transmitido, dando inicio a transmissão (UDR0 não inicia a contagem vazio). Nessa etapa de acordo com a letra em letrarecebida, também é configurada a frequencia com que o led piscara em contmax(valor necessario de interrupções do temporizador para que passe o tempo exato para o led acender ou apagar: frequencia da interrupção = 62,5 KHz). Também é definido pela variavel l se o led deve piscar ou não (1 pisca, 0 não pisca) 
	else if(letrarecebida == 't') {strcpy(msgtrans, "Onda triangular\n"); UDR0 = msgtrans[0]; contmax = 31250; l = 1;}
	else if(letrarecebida == 'd') {strcpy(msgtrans, "Onda dente-de-serra\n"); UDR0 = msgtrans[0]; contmax = 15625; l = 1;}
	else if(letrarecebida == 'a') {strcpy(msgtrans, "Onda AM\n"); UDR0 = msgtrans[0]; contmax = 7813; l = 1;}
	else {strcpy(msgtrans, "Nenhuma forma de onda selecionada\n"); UDR0 = msgtrans[0]; contmax = 0; l = 0;}	
	}

		
ISR(USART_TX_vect){ //interrupção de transmissão da uart
	if (msgtrans[m] != '\0'){UDR0 = msgtrans[m]; //enqunto o caractere transmitido não for um fim de string ele transmitirá cada um dos caracteres de msgtrans, colocando os caractere uma a um em UDR0, incrementad o contador m, de 1 ate o fim da string (começa em 1 já que o elemento 0 da string já foi trnasmitido anteriormente)
							m++;}
	else {m = 1;} //ao chegar no fim da string o processo recomeça
	}

	
ISR(TIMER2_OVF_vect){ // interrupção de overflow do timer (frequencia 62,5 KHz)
	x++; //incrementa a variavel x
	if(x == contmax && l == 1){trocaled(); x = 0;} //no momento em que o contador x atingir o valor da variavel contmax[o tempo necessario para que o led troque de estado (trocaled) terá passado] e se a variavel l (que habilita ou não o led piscar) tiver alta, o estado do led trocará.
		
	if (letrarecebida == 's'){OCR2A = senoidal[n];} //de acordo com a letra recebida em UDR0 (letrarecebida), o vetor da onda que se deseja gerar é selecionadda, e cada um dos valores amostrados é transmitido pelo contador n
	else if (letrarecebida == 't'){OCR2A = triangular[n];}
	else if (letrarecebida == 'd'){OCR2A = dente_serra[n];}
	else if (letrarecebida == 'a'){OCR2A = am[n];}
	else {OCR2A = 0;} // para nenhuma das ondas selecionadas a saída fica em 0 (desligada).
	n++; //contador n é incrementado para passar cada um dos elementos do vetor das ondas para OCR2A
	if (n == 200){n = 0;} // ao chegar no fim do vetor (tamanho do vetor de onda = 200), o processo é reiniciado (função periodica)
}


int main(void)
{
	
	UBRR0H &= 0xf0; //força os quatro ultimos bits do registrador para 0
	UBRR0L = 103; //pubrrol e pubrroh configuram o baud rate da transmissão, nesse caso, para baud de 9600  o parametro colocado no registrador é 103
	UCSR0A &= 0xFC; // força os dois ultimos bits do registrador para 0, configurando velocidade de transsmissão normal e modo multiprocessador desativado
	UCSR0B = 0xD8; //mantém as interrupções da USART () o 9° bit de transssão desativados, e ativa o modo transmissor e receptor da USART.
	UCSR0C = 0x0E; // configura a USART como assincrona, com tamanho de frame de 8 bits, sem o uso de bits de paridade. Também é configurado o uso de dois bits de parada.
	
    DDRB |= 0b00101000; //habilita pino 13 do arduino como saída, em paralelo com o led da placa, bem como o pino 11 para gerar a forma de onda.
	
    TCCR2A |= 0x83; //modo fast PWM (contagem fixada para interrupção de 0 a 255)
    TCCR2B |= 0x01; //prescaler de 1/1 (clock 16Mhz) ----> 256 * [1/(16M/1)] = 1/62,5K [s] (periodo do PWM overflow)
    TIMSK2 |= 0b00000001; //habilita interrupção do tipo overflow
    OCR2A = 0x00; //inicia em 0
    sei(); //habilita todas as interrupções
	
    while (1) 
    {
		
    }
}

