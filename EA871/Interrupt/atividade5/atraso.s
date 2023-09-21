
.global atraso


atraso:
;ldi r25,0x01 ;são inseridos os bits mais significativos do valor do delay desejado
;ldi r24,0xf4 ;valores dos dois bits menos significativos do valor do delay desejado

;push r25 ;guarda o estado inicial de todos os registradores modificados, na pilha
;push r24
push r16
push r17
push r18
push r19

//--------------------------------------------------------------//
inc r25 ;incrementa r25 pois na comparação final, r25 é comparado com zero, pois se não fosse incrementado, o caso em que r25=0x00 porem r24=0xff seria desconsiderado (o incremento não corrige isso, mas torna o tempo de execução equivalente ao correto)
;rotina de 1ms:
inicio0: ldi r17, 0x07 ;parametros estabelecidos para cada registrador: r17=dois bits mais significativos do parametro; r16=dois bits menos significativos do parametro; r18=dois bits menos significativos do valor a ser incrementado; r19= dois bits mais significativos do valor a ser incrementado 
		 ldi r16, 0xd0 ; o parametro de r17 e r16 é 2000 (0x7d0), que foi estabelecido a partir do numero de operações realizada no primeiro laço, de forma que esse laço realize um atraso próximo a 1 ms
		 ldi r18, 0x00
		 ldi r19, 0x00

inicio: cp r18, r16 ;r16 comparado com r18 (bits menos significaivos)
		brne incremento1 ;caso não seja igual, incrementa r18 e verifica se o mesmo está cheio(0xff):caso sim, zera r18 e incremente r19(bit mais significativo). Caso não, retorna ao inicio da rotina e continua incrementando r24.  
		cp r19, r17 ;compara r19 com com r17 (bits mais significaitvos)
		breq fim1 ; caso tanto r16 quanto r17 forem iguais a r18 e r19 respectivamente, o laço chega ao fim. Caso contrario os registradores continuam sendo incrementados. Para o parametro 2000, essa rotina gera um atraso muito proximo a 1 ms.
		rjmp incremento1 
	incremento1:	inc r18
					cpi r18, 0xff
					breq incremento2
					rjmp inicio
	incremento2:    inc r19
					ldi r18, 0x00
					rjmp inicio

//--------------------------------------------// 
;rotina que repete a rotina de 1ms o numero de vezes desejada:
fim1: cpi r24, 0x00 ;compara r24(dois bits menos significaticvos) com 0x00, para então decrementa-lo até 0x00, de forma que cada vez que o registrador é decrementado, a rotina de atraso de 1 ms é realizada, realizando então um atraso com o numero de ms entrado inicialmente 
	  brne decremento1 ;caso r24 não seja 0, r24 é decrementado e então o programa volta para a rotina de atraso de 1 ms(inicio0)
	  breq decremento2 ;caso r24 seja igual a zero, r25 é decrementado, e r24 volta para 0xff. Caso r25 seja diferente de zero, o programa desvia para a rotina de 1ms, realiza-a, e então vai decrementando r24 novamente. 
	  decremento1: dec r24
				   rjmp inicio0
	  decremento2: dec r25
				   ldi r24, 0xff
				   cpi r25, 0x00
				   brne inicio0
				   breq fim2 ;caso r25 seja igual a zero a rotina se encerra (isso gerou um problema inicialmente pois não considera o caso em que r25=0x00 e r24=0xff, em que faltaria realizar mais um cicli de decrementos de r24. Para Corrigir esse erro r25 foi incrementado no inicio da rotina.
fim2: ;retorna da pilha  os estados iniciais dos registradores modificados e enccerra a rotina(ret)
pop r19
pop r18
pop r17
pop r16
;pop r24
;pop r25
ret
//--------------------------------------------//