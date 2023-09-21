/* Teste de um buffer circular
 *
 * Este programa serve para implementar e testar uma implementacao de buffer
 * circular
 */

#include <stdio.h>

char buffer[5];
unsigned int i=0;
unsigned int j=0;
int b=0;

void adicionar_buffer(char c) {
/* Preencha com seu codigo! */
if(b < 5){
		buffer[i] = c;
		i++;
		b++;
		if(i==5) {i=0;
		}
	}	
}

void remover_buffer() {
/* Preencha com seu codigo! */
	if(b > 0){;
		j++;
		b = b - 1;
		if (j==5) {j=0;
		}
	}	
}

void imprimir_buffer() {
/* Preencha com seu codigo! */
	int k = j;
	int t ;
	for(t=0;  t<b; t++){
		if(t != (b-1)){
			printf("%c ", buffer[k]);
			k++;
		}
		else{printf("%c", buffer[k]);
		}
		if(k==5){k=0;}
	}
	printf("\n");
}
	/*while(k != (i-1)){
		printf("%c ", buffer[k]);
		k++;
		if(k==5){k=0;}
	}
	if (k == (i-1)){printf("%c\n", buffer[k]);}
}*/

int main() {


  char c;

  do {
    scanf("%c", &c);
    if (c == '\n') break;

  /* Sugestao: aqui deveria vir o seu codigo */
    if (c <= '9' && c>='0') {adicionar_buffer(c);
    }
    else {remover_buffer();
    }
    imprimir_buffer();
  } while (1);

  return 0;
}