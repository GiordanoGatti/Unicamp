#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 8, 9, 10, 11);
#include <Wire.h>

#define LM35 17 
String received;
char buf [20];
int contagem = 0;
int contagem_comando =  0;
int contagem_temp = 0;
int transferindo = 0;
int transferencia_ativa = 0;
int quant_dados = 0;
int end_transferidos = 0;
int coleta = 0;
int temperatura = 0;
int estado_i2c = 1;
int index = 0;
int linha = 0;
char digito = "/";
int novo_digito = 0;
int digito_antigo = 0;
int rows[4] = {2, 3, 4, 5};
int columns[3] = {14, 15, 16};
char symbols[4][3] = {{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};
char comando_leitura[10];
String comando = "";
int N_resgistros = 0;
int comando_a_mostra = 0;
const uint8_t add_memoria = 80;
byte add_dado[2];
unsigned char dado = 0;
unsigned int eeaddress = 0;
int var1 = 0;
int var2 = 0;
int end_escrito = 0;

void setup()
{
  cli(); // Desabilita interrupcoes globais
  configuracao_Timer0(); //configura interrupção do timer0
  sei(); // Habilita interrupcoes globais
  lcd.begin(16, 2); //inicializa o LCD

  //Pinos ligados aos pinos 1, 2, 3 e 4 do teclado - Linhas
  pinMode(rows[0], OUTPUT);
  pinMode(rows[1], OUTPUT);
  pinMode(rows[2], OUTPUT);
  pinMode(rows[3], OUTPUT);

  //Pinos ligados aos pinos 5, 6 e 7 do teclado - Colunas
  pinMode(columns[0], INPUT);
  pinMode(columns[1], INPUT);
  pinMode(columns[2], INPUT);

  Wire.begin(); //INICIALIZA I2C
  Serial.begin(9600); //INICIALIZA A SERIAL
}



void loop() {
  if (transferencia_ativa == 0) { //Caso não esteja ocorrendo uma transferencia de dados, o sistema segue para a rotina de decodificação
    read_keypad(linha); //função de varredura de cada linha do teclado
    linha++; //passa para a linha seguinte
    if (linha == 4) { //caso atinja a ultima linha o processo é reiniciado
      linha = 0;
    }
    if (novo_digito == 1) { //caso o digito clicado seja diferente ao anterior recebido
      comando_leitura[index] = digito; //a lista comando leitura incorpora esse digito em seus elementos
      index++; //passa para o elemento seguinte (montando uma palavra - comando)
      if (digito == '#') { //caso seja recebida uma hashtag o comando passa para a etapa de leitura
        for (int i = 0; i < index; i++) {
          comando += comando_leitura[i]; //a lista de caracteres é reconstruida como uma unica string
        }
        if (transferindo == 0) { //caso uma transferencia não esteja sendo solicitada, a leitura do teclado é relacionada a seleção de comando
          decodeCommand(comando);
        }
        else {//caso uma transferencia esteja sendo solicitada, a leitura do teclado é relacionada a seleção de parametros
          transferencia(comando);
        }
        index = 0;//indice é zerado aguardando por uma nova leitura
      }
      else if (digito == '*') { //caso seja recebido um asterisco, as variaveis auxiliares são reiniciadas e volta-se ao estado inicial, cancelando o comando
        index = 0;
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("COMANDO");
        lcd.setCursor(3, 1);
        lcd.print("CANCELADO");
        comando_a_mostra = 1;
        transferindo = 0;
        
      }
      comando = ""; //comando é resetado aguardando por uma nova leitura
    }
  }
  else { //Caso esteja ocorrendo uma transferencia de dados, o sistema segue para a rotina de transferencia
    if (end_transferidos/2 < quant_dados) { //caso o numero de dados a serem transferido for inferior ao numero de dados ja gravados, transferecia ok
      Serial.println(float(receber(end_transferidos))/10, 1); //lançamento dos dados na serial
      end_transferidos += 2; //incrementa a cada palavra gravada na EEPROM (2 bytes)
    }
    else if (quant_dados == int(receber(2046))) {//caso o numero de dados a serem transferido igual ao numero de dados ja gravados, transferecia ok
      deletarMemoria(); //novo indice da memória será similar a apagar a memória, já que todas as amostras serão transferidas
      end_transferidos = 0; //variaveis reiniciam aguardando uma nova transferencia
      transferindo = 0;
      transferencia_ativa = 0;
    }
    else { //no caso do numero de dados a serem transferido ser inferior ao numero de dados ja gravados, a memoria deverá ser realocada após a transmissão
      if (var1 < (int(receber(2046)) - quant_dados)*2) { //inicia a realocação de memória colocando os novos endereços no contador var1
        var2 = receber(2*quant_dados + var1); //variavel var 2 recebe o valor a ser realocado
        transmitir(var2, var1); //o valor var2 é realocado na sua nova posição calculada em var1
        var1 += 2; //incrementa a cada palavra gravada na EEPROM (2 bytes)
      }
      else { //o contador de posições localizado na posição de memória 2046 (ultimo byte) e as variaveis auxiliares são atualizadas encerrando a transmissão
        transmitir(var1/2, 2046);
        transferencia_ativa = 0;
        transferindo = 0;
        end_transferidos = 0;
        var1 = 0;
      }
    }
  }


  if (contagem_temp >= 250 && coleta == 1 && 1022 - int(receber(2046)) > 0) { //a cada 2 segundos e caso esteja sendo solicitada uma coleta de dados e caso a memória não esteja lotada, a coleta é inicializada
    temperatura = int((float(analogRead(17)))*(5000/1023)); //a temperatura é convertida de uma leitura de tensão em °C
    transmitir(temperatura, int(receber(2046))*2); //temperatura é registrada na próxima posição de memória livre
    transmitir(int(receber(2046)) + 1, 2046); //contador de memória ocupada é incrementado e já transmitido nos ultimos dois bytes da EEPROM
    contagem_temp = 0; //contador que conta o tempo de medição é zerado iniciando uma nova coleta de amostra 
  }
  else if (1022 - int(receber(2046)) == 0) { //caso o contador de amostras atinja o limite da capacitada da EEPROM a coleta é encerrada e é exibido que a memória está cheia
    coleta = 0; // finaliza a coleta
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("ERRO: MEMORIA");
    lcd.setCursor(5, 1);
    lcd.print("CHEIA");
    comando_a_mostra = 1;
  }

  i2c_show(temperatura); //ultima medida registrada é exibida nos displays 7 segmentos através do I2C utilizando o próprio atraso do loop
  estado_i2c++;
  if (comando_a_mostra == 0) { //caso o contador de tempo que o lcd fica aceso é zerado este é limpado
    lcd.clear();
  }
}



void decodeCommand(String command) { //função criada para decodificar  mensagem
  if (command.length() != 2) { //caso o comando recebido tenha mais do que 2 digitos este já possui forma incorreta - exibi-se mensagem de erro
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("ERRO: COMANDO");
    lcd.setCursor(2, 1);
    lcd.print("INEXISTENTE");
    comando_a_mostra = 1;
    return;
  }
  else if (command == "1#") { //caso o comando seja 1# executa-se o comando de apagar a memória e exibe o recado de memória apagada no lcd
    /* FUNÇÃO APAGAR MEMÓRIA */
    comando_a_mostra = 1;
    deletarMemoria();
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("MEMORIA");
    lcd.setCursor(5, 1);
    lcd.print("APAGADA");
    return;
  }
  else if (command == "2#") { //caso o comando seja 2# exibe-se no lcd o numero de amostras gravadas, dada pelo contador de amostras nos ultimos 2 bytes da EEPROM, e o numero de amostras disponíveis, dada por 1022 (capacidade maxima) menos tal contador
    /* PRINTAR STATUS DA MEMÓRIA -> GRAVADOS: xxxx LIVRES: xxxx */
    int gravados = receber(2046);
    int livres = 1022 - gravados;
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("GRAVADOS: ");
    lcd.setCursor(12, 0);
    lcd.print(gravados);
    lcd.setCursor(2, 1);
    lcd.print("LIVRES: ");
    lcd.setCursor(11, 1);
    lcd.print(livres);
    comando_a_mostra = 1;
    return;
  }
  else if (command == "3#") { //caso o comando seja 3#, a flag de coleta é levada a 1 e a coleta é inicializada no void loop. Mostra-se que tal processo foi iniciado através do lcd
    coleta = 1; // variável de estado para inicialização do preenchimento da memória com as temperaturas medidas a cada 2seg
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("COLETA");
    lcd.setCursor(2, 1);
    lcd.print("INICIALIZADA");
    comando_a_mostra = 1;
    return;
  }
  else if (command == "4#") { //caso o comando seja 4#, a flag de coleta é levada a 0 e a coleta é encerrada no loop. Mostra-se que tal processo foi finalizado através do lcd
    coleta = 0; // finaliza a coleta
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("COLETA");
    lcd.setCursor(3, 1);
    lcd.print("FINALIZADA");
    comando_a_mostra = 1;
    return;
  }
  else if (command == "5#") { //caso o comando seja 5# exibe-se no display a pergunta de quantas amostras se deseja transmitir e a flag transferindo é levada a 1, configurando a leitura do teclado para ler o parametro de numero de amostras desejado
    if (coleta == 0) { //caso não esteja sendo realizada nenhuma coleta a rotina de transferencia prossegue normalmente
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("QUANTAS");
      lcd.setCursor(4, 1);
      lcd.print("MEDIDAS?");
      comando_a_mostra = 1;
      transferindo = 1;
      return;
    }
    else { //caso esteja sendo realizada uma coleta, a rotina de transmissão não é completada
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("COLETA NAO");
      lcd.setCursor(3, 1);
      lcd.print("FINALIZADA");
      comando_a_mostra = 1;
      return;
    }
  }
    else { //caso não se compra nenhuma das condições anteriores o comando é dado como inexistente e a mensagem é exibida no lcd
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("ERRO: COMANDO");
    lcd.setCursor(2, 1);
    lcd.print("INEXISTENTE");
    comando_a_mostra = 1;
    return;
  }
}

void transferencia(String command) { //função que realiza a transferencia dos dados da EEPROM para a serial
  if (command.length() > 5 || command.length() == 1) { //caso o parametro de entrada possua apenas 1 digito ou mais de 5 digitos este já se encontra fora do formato desejado, portanto se exibe uma mensagem de erro no lcd e a falg de transmissão é levada a zero
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("ERRO: PARAMETRO");
    lcd.setCursor(3, 1);
    lcd.print("INCORRETO");
    comando_a_mostra = 1;
    transferindo = 0;
  }
  else if (command.substring(0, command.indexOf("#")).toInt() < 1 || command.substring(0, command.indexOf("#")).toInt() > 1022) { //caso o valor do parametro seja inferior a 1 ou superior ao limite maximo da memória (1022), também é exibida uma mensagem de parametro incorreto no lcd e a transferencia não é inicializada
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("ERRO: PARAMETRO");
    lcd.setCursor(3, 1);
    lcd.print("INCORRETO");
    comando_a_mostra = 1;
    transferindo = 0;
  }
  else if (command.substring(0, command.indexOf("#")).toInt() > int(receber(2046))) { //caso o valor do parametro seja superior ao numero de amostras já grvadas no contador da EEPROM (dois ultimos bytes), tembém se exibe uma mensagem de erro no lcd e a transferencia não se inicia
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("ERRO: PARAMETRO");
    lcd.setCursor(3, 1);
    lcd.print("INCORRETO");
    comando_a_mostra = 1;
    transferindo = 0;
  }
  else { //caso o parametro não possua qualquer dos erros anteriores, 
    transferencia_ativa = 1; //a flag de transferencia_ativa é levada a 1, iniciando a transferencia no void loop
    quant_dados = command.substring(0, command.indexOf("#")).toInt(); //a variavel quant_dados recebe um inteiro relativo a quantidade que se deseja transmitir (parametro)
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("TRANSFERINDO...");//e é exibido no lcd a mensagem de transferindo...
    comando_a_mostra = 1;
  }
}

void i2c_show (int val) { //função de exibição dos displays 7 segmentos via I2C (reutilizada do projeto 2)
  int ones = (val % 10); //separa as unidades do valor em uma variavel
  int tens = ((val / 10) % 10);//separa as dezenas do valor em uma variavel
  int hundreds = ((val / 100) % 10);//separa as centenas do valor em uma variavel
  int ones_byte = ones + 96; //96 representa em binario o enable relativo ao primeiro display
  int tens_byte = tens + 208; //208 representa em binario o enable relativo ao segundo display e ao ponto
  int hundreds_byte = hundreds + 48; // 48 em binario representa o enable relativo ao terceiro display
  if (estado_i2c == 1) { //cada display é acionado a partir da variavel estado_i2c, que é incrementada no loop para que o atraso deste seja imprgado na troca de display
    Wire.beginTransmission(32); //endereço do extensor de portas i2c é 32 em decimal
    Wire.write(ones_byte); //transmite as unidades no respectivo display
    Wire.endTransmission(32);
  }
  if (estado_i2c == 2) {
    Wire.beginTransmission(32);
    Wire.write(tens_byte);//transmite as dezenas no respectivo display
    Wire.endTransmission(32);
  }
  if (estado_i2c == 3) {
    Wire.beginTransmission(32);
    Wire.write(hundreds_byte);//transmite as centenas no respectivo display
    Wire.endTransmission(32);
    estado_i2c = 0; //estado é reiniciado, voltando as unidades
  }
}

void configuracao_Timer0() {
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuracao Temporizador 0 (8 bits) para gerar interrupcoes periodicas a cada 8ms no modo Clear Timer on Compare Match (CTC)
  // Relogio = 16e6 Hz
  // Prescaler = 1024
  // Faixa = 125 (contagem de 0 a OCR0A = 124)
  // Intervalo entre interrupcoes: (Prescaler/Relogio)Faixa = (64/16e6)(124+1) = 0.008s

  // TCCR0A – Timer/Counter Control Register A
  // COM0A1 COM0A0 COM0B1 COM0B0 – – WGM01 WGM00
  // 0      0      0      0          1     0
  TCCR0A = 0x02;

  // OCR0A – Output Compare Register A
  OCR0A = 124;

  // TIMSK0 – Timer/Counter Interrupt Mask Register
  // – – – – – OCIE0B OCIE0A TOIE0
  // – – – – – 0      1      0
  TIMSK0 = 0x02;

  // TCCR0B – Timer/Counter Control Register B
  // FOC0A FOC0B – – WGM02 CS02 CS01 CS0
  // 0     0         0     1    0    1
  TCCR0B = 0x05;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
// Rotina de servico de interrupcao do temporizador
ISR(TIMER0_COMPA_vect) {
  // Insira aqui o codigo a s do temporizadorer executado pela interrupcao periodica
  //cada 8ms
  contagem++;
  contagem_temp++; //contador de tempo para coleta das amostras de temperatura
  if (comando_a_mostra == 1) {
    contagem_comando++; //contador de tempo para a exibição de mensagens no lcd (acionado apenas quando a variavel comando_a_mostra é levada a 1, ou seja, quando se necessita do display)
    if (contagem_comando >= 250) { //ao atingir 2 seg o contado do display é reiniciado e o display desacionado (250*8ms = 2s)
      comando_a_mostra = 0;
      contagem_comando = 0;
    }
  }
}

void read_keypad (int linha) {
  digitalWrite(rows[0], HIGH); //desenergiza todas as linhas
  digitalWrite(rows[1], HIGH);
  digitalWrite(rows[2], HIGH);
  digitalWrite(rows[3], HIGH);
  digitalWrite(rows[linha], LOW); // energiza a linha correspondente
  novo_digito = 0; //variavel de novo digito é reiniciada
  if (linha == 0) { //a linha 1 é energizada
    if (digitalRead(columns[0]) == HIGH) { //verifica se o sinal é da coluna 1
      if (digito != symbols[0][0]) { //caso o digito for diferente ao relativo a esta entrada 
        novo_digito = 1; // a variavel novo_digito é levada a 1 indicando que foi digitado um simbolo diferente ao ultimo
      }
      digito = symbols[0][0]; //atualiza a variavel digito
      return;
    }
    else if (digitalRead(columns[1]) == HIGH) {//verifica se o sinal é da coluna 2
      if (digito != symbols[0][1]) {
        novo_digito = 1;
      }
      digito = symbols[0][1]; //Linha 1, Coluna 2 -> botao 2 foi pressionado
      return;
    }
    else if (digitalRead(columns[2]) == HIGH) {//verifica se o sinal é da coluna 3
      if (digito != symbols[0][2]) {
        novo_digito = 1;
      }
      digito = symbols[0][2]; //Linha 1, Coluna 3 -> botao 3 foi pressionado
      return;
    }
  }
  if (linha == 1) { //a linha 2 é energizada e o mesmo processo da linha 1 é repetido para cada coluna
    if (digitalRead(columns[0]) == HIGH) { //verifica se o sinal é da coluna 1
      if (digito != symbols[1][0]) {
        novo_digito = 1;
      }
      digito = symbols[1][0]; //Linha 2, Coluna 1 -> botao 4 foi pressionado
      return;
    }
    else if (digitalRead(columns[1]) == HIGH) {
      if (digito != symbols[1][1]) {
        novo_digito = 1;
      }
      digito = symbols[1][1]; //Linha 2, Coluna 2 -> botao 5 foi pressionado
      return;
    }
    else if (digitalRead(columns[2]) == HIGH) {
      if (digito != symbols[1][2]) {
        novo_digito = 1;
      }
      digito = symbols[1][2]; //Linha 2, Coluna 3 -> botao 6 foi pressionado
      return;
    }
  }
  if (linha == 2) { //a linha 3 é energizada e o mesmo processo da linha 1 é repetido para cada coluna
    if (digitalRead(columns[0]) == HIGH) { //verifica se o sinal é da coluna 1
      if (digito != symbols[2][0]) {
        novo_digito = 1;
      }
      digito = symbols[2][0]; //Linha 3, Coluna 1 -> botao 7 foi pressionado
      return;
    }
    else if (digitalRead(columns[1]) == HIGH) {
      if (digito != symbols[2][1]) {
        novo_digito = 1;
      }
      digito = symbols[2][1]; //Linha 3, Coluna 2 -> botao 8 foi pressionado
      return;
    }
    else if (digitalRead(columns[2]) == HIGH) {
      if (digito != symbols[2][2]) {
        novo_digito = 1;
      }
      digito = symbols[2][2]; //Linha 3, Coluna 3 -> botao 9 foi pressionado
      return;
    }
  }
  if (linha == 3) { //a linha 4 é energizada e o mesmo processo da linha 1 é repetido para cada coluna
    if (digitalRead(columns[0]) == HIGH) { //verifica se o sinal é da coluna 1
      if (digito != symbols[3][0]) {
        novo_digito = 1;
      }
      digito = symbols[3][0]; //Linha 3, Coluna 1 -> botao # foi pressionado
      return;
    }
    else if (digitalRead(columns[1]) == HIGH) {
      if (digito != symbols[3][1]) {
        novo_digito = 1;
      }
      digito = symbols[3][1]; //Linha 3, Coluna 2 -> botao 0 foi pressionado
      return;
    }
    else if (digitalRead(columns[2]) == HIGH) {
      if (digito != symbols[3][2]) {
        novo_digito = 1;
      }
      digito = symbols[3][2]; //Linha 3, Coluna 3 -> botao * foi pressionado
      return;
    }
  }
}

void transmitir(unsigned int dado, unsigned int endereco) { //para escrever na EEPROM com resolução de 2 bytes, 
  byte upper =  dado >> 8; //primeiro elimina-se os 8 primeiros bits por deslocamento, sobrando apenas a parte relativa ao byte mais significativo
  byte lower =  dado & 0xFF; //em seguida elimina-se os bits mais significativos através de um AND com os 8 bits menos significativos
  escreverEEPROM(endereco, upper);//por fim escreve-se na EEPROM o byte mais significativo no endereço dado
  escreverEEPROM(endereco + 1, lower);// e escreve-se na EEPROM o byte menos significativo no endereço seguinte
}

unsigned long receber(unsigned int endereco) { //para ler uma palavra da EEPROM (2 bytes) 
  byte upper = lerEEPROM(endereco); //recebe-se a posição desejada com o byte mais significativo
  byte lower = lerEEPROM(endereco + 1); //e a posição seguinte com o byte menos significativo
  return concatenar(upper, lower); //por fim se retorna-se o resultado da junção das duas palavras (processo inverso da escrita)
}

unsigned long concatenar(byte MSB, byte LSB) { //função que concatena o byte menos significativo com o byte mais significativo
  unsigned long combine;
  combine = ((MSB << 8) | LSB); //após deslocar o byte mais significativo 8 bits para a direita (abrindo espaço para os 8 bits menos significativos), realiza-se a operação de OR, ou seja soma dos bits
  return combine;
}

void escreverEEPROM(unsigned int eeaddress, byte data ){
  const uint8_t i2c_base_address = 0x51; //endereço I2C da EEPROM definido pelas portas A0=1, A1=0 e A2=0
  const uint8_t upper_three_eeprom_adress_bits = (eeaddress >> 8) & 0x7; //seleciona os tres bits mais significativos do endereço em que se deseja escrever
  const uint8_t i2c_address = i2c_base_address | upper_three_eeprom_adress_bits; //soma os tres bits do endereço de escrita com o endereço do I2C através da função OR
  Wire.beginTransmission(i2c_address); //inicia a transmissão no endereço definido pela soma anterior
  Wire.write(eeaddress & 0xFF); //seleciona o endereço em que se deseja escrever
  Wire.write(data); //escreve o valor desejado
  Wire.endTransmission(); //encerra a transmissão
  delay(5); //delay necessário após a transmissão
}

byte lerEEPROM(unsigned int eeaddress ){
  byte rdata = 0xFF;
  const uint8_t i2c_base_address = 0x51;//endereço I2C da EEPROM definido pelas portas A0=1, A1=0 e A2=0
  const uint8_t upper_three_eeprom_adress_bits = (eeaddress >> 8) & 0x7;//seleciona os tres bits mais significativos do endereço em que se deseja ler
  const uint8_t i2c_address = i2c_base_address | upper_three_eeprom_adress_bits;//soma os tres bits do endereço de leitura com o endereço do I2C através da função OR
  Wire.beginTransmission(i2c_address);//inicia a transmissão no endereço definido pela soma anterior
  Wire.write(eeaddress & 0xFF); //seleciona o endereço em que se deseja ler
  Wire.endTransmission();//encerra a transmissão
  Wire.requestFrom(i2c_address, 1); //solicita o valor armazenado na posição de memória enviada
  if (Wire.available()) rdata = Wire.read(); //recebe o valor
  return rdata;
}

void deletarMemoria() { //a rotina de exclusão de memória, assim como proposto, consiste em apenas zerar o contador de amostras já que ao coletar novas amostras as posições de memória que possuiam amostras previamente serão sobrescritas
  transmitir(0, 2046);
}
