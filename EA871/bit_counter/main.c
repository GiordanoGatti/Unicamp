/* Contador de bits
 *
 * Este programa conta o numero de bits em um byte
 */

#include <stdio.h>

int main() {

  unsigned char entrada;
  unsigned int tmp;
  unsigned int n_bits;

  /* Ler entrada em hexadecimal */
  scanf("%x", &tmp);
  entrada = (unsigned char)tmp;


  n_bits = 0;
  int i=0;
  int j=1;

  while(i<8) {
	int k = entrada & j;
	if (k == j){n_bits = n_bits+1;
		}
	j=2*j;
	i=i+1;
	}
		

  /* Escrever numero de bits */
  printf("%d\n", n_bits);
  return 0;
}
