// C++ code
//
#include <Wire.h>

String received;
char buf [12];
char tempRPM[4];
int vel = 0;
int pwm_vel;
int contagem = 0;
int contagem_aux =  0;
int N = 1; //quantas vezes por segundo o sinal é amostrado(inteiro, positivo e diferente de 0)
int pulsos = 0;
//volatile float vel = 0;

int power = 0; //0 -> motor desligado ; 1 -> motor ligado
int sentido_motor = 0; //anti-horario = 0 e horario = 1
int sentido_motor_inv = 1; //sentido oposto ao do motor


volatile float t_amostragem = 1000/N; //tempo de amostragem [ms]


void setup()
{
  Wire.begin();
  Serial.begin(9600); //inicializa monitor serial
  attachInterrupt(digitalPinToInterrupt(2), pulse_count, RISING); //configura-se uma interrupção digital, na porta 2, a cada borda de subida no sinal recebido por esta
  
}

void loop()
{
  //if (Serial.available() > 0) { //enquanto serial estiver recebendo mensagens (caracter por caracter)
  //    received = Serial.readStringUntil('\n'); //le a string enviada (juntando os caracteres) ate se deparar a uma qubra de linha
  //    decodeCommand(received); //decodifica a string recebida
  //}
  sprintf(tempRPM, "%4d", vel);
  Wire.beginTransmission(32);
  //for (int i=0; i<4; i++)
  //{
    //Wire.write(tempRPM[i]);
  //}
  Wire.write(0x32);
  
  Wire.endTransmission();
  //Wire.beginTransmission(32);
  //Wire.write(0100);
  //Wire.endTransmission();
}




void decodeCommand(String command) { //função criada para decodificar  mensagem
  if (!command.endsWith("*")) { //caso o comando não possua o indicador de fim de comando '*' ao seu final, já é considerado como inexistente
    Serial.println("ERRO: COMANDO INEXISTENTE");
  }
  else if (command.indexOf(" ") == -1) { //caso o comando recebido possua o nidicador '*' porem não possua o caracter ' '(espaço)
    if (command.equals("RETVEL*")) { //realiza-se uma comparação para verificar se o comando recebido é 'RETVEL'
      sprintf(buf, "VEL: %d RPM", vel); //retorna a velocidade do ventilador
      Serial.println(buf);
    }
    else if (command.equals("VENT*")) {//realiza-se uma comparação para verificar se o comando recebido é 'VENT'
      sprintf(buf, "OK VENT"); //responde com confirmação de modo 'VENT' (esta etapa será posteriormente acoplada aos proximos estagios)
      Serial.println(buf);
    }
    else if (command.equals("EXAUST*")) {//realiza-se uma comparação para verificar se o comando recebido é 'EXAUST'
      sprintf(buf, "OK EXAUST"); //responde com confirmação de modo 'EXAUST' (esta etapa será posteriormente acoplada aos proximos estagios)
      Serial.println(buf);
    }
    else if (command.equals("PARA*")) {//realiza-se uma comparação para verificar se o comando recebido é 'PARA'
      sprintf(buf, "OK PARA"); //responde com confirmação de parada 'PARA' (esta etapa será posteriormente acoplada aos proximos estagios)
      Serial.println(buf);
    }
    else if (command.equals("VEL*")) {//realiza-se uma comparação para verificar se o comando recebido é 'VEL' porém com falta de parametros (primeiro caso)
      Serial.println("ERRO: PARAMETRO AUSENTE");
    }
    else{
      Serial.println("ERRO: COMANDO INEXISTENTE"); //caso nenhuma das condições acima sejam satisfeitas, o comando é inexistente
    }
  }
  else if (command.indexOf(" ") == 3) { //caso a posição do caracter ' '(espaço) seja correspondente a do comando 'VEL' (unico que contem espaço)
    if (command.substring(0,3) == "VEL") { //e caso antes do espaço o comando esteja escrito como esperado
      if (command.indexOf("*") == 4) { //caso o indicador de fim de comando se localize logo ao incio dos parametros acusa-se falta de parametros
        Serial.println("ERRO: PARAMETRO AUSENTE");
      }
      else if (command.indexOf("*") > 5 && command.indexOf("*") != 7){ //caso o indicador de fim de comando se localize após a entrada de algum dos digitos do parametros (a parte de sua posição esperada) acusa-se erro de parametros
        Serial.println("ERRO: PARAMETRO INCORRETO");      
      }
      else{
        for (int i = 0; i < command.substring(4,7).length(); i ++) {
          if (isDigit(command.substring(4,7).charAt(i))){ //verifica se os digitos dos parametros são valores aceitaveis (um a um)
            continue;
          }
          else{
            Serial.println("ERRO: PARAMETRO INCORRETO"); //caso algum dos digitos seja invalido acusa-se erro de parametro
            return;
          }
        }
        if (command.substring(4,7).toInt() >= 0 &&  command.substring(4,7).toInt() <= 100) { //verifica se o valor do parametro se encontra na margem aceita (0 a 100 [%])
          pwm_vel = command.substring(4,7).toInt(); //caso o valor seja aceito, todos os requisitos para a função 'VEL' foram cumpridos
          Serial.println("OK VEL " + command.substring(4,7) + "%"); //retorna-se e printa a porcentagem de velocidade desejada
        }
        else{
          Serial.println("ERRO: PARAMETRO INCORRETO"); //caso o valor dos parametros esteja fora da margem acusa-se erro de parametro
          return;
        }
      }
    }
    else{ //caso a string antes do caracter ' '(espaço) não seja 'VEL', retorna-se comando inexistente
      Serial.println("ERRO: COMANDO INEXISTENTE");
    }
  }

  else{ //caso haja um caracter ' '(espaço) que não se localize na posição referente ao comando 'VEL', retorna-se comando inexistente 

    Serial.println("ERRO: COMANDO INEXISTENTE");
  }
}


void pulse_count(){
    pulsos++; //variavel que realiza contagem de pulsos por um determinado periodo de tempo
}



void frear(){ //função para frear bruscamente, invertendo os polos
   power = 1;
    sentido_motor = !sentido_motor;
    sentido_motor_inv = !sentido_motor;
      
    digitalWrite(8, power);
    digitalWrite(9, sentido_motor);
    digitalWrite(10, sentido_motor_inv);
}

void desacelerar(){ //desacelera gradualmente desligando a alimentação do motor
  power = 0;
    digitalWrite(8, power);
}

void horario(){ //liga o motor no sentido horario
  power = 1;
    sentido_motor = 1;
    sentido_motor_inv = !sentido_motor;
      
    digitalWrite(8, power);
    digitalWrite(9, sentido_motor);
    digitalWrite(10, sentido_motor_inv);
}

void anti_horario(){ //liga o motor no sentido anti-horario
  power = 1;
    sentido_motor = 0;
    sentido_motor_inv = !sentido_motor;
      
    digitalWrite(8, power);
    digitalWrite(9, sentido_motor);
    digitalWrite(10, sentido_motor_inv);
}
void configuracao_Timer0(){
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Configuracao Temporizador 0 (8 bits) para gerar interrupcoes periodicas a cada 8ms no modo Clear Timer on Compare Match (CTC)
  // Relogio = 16e6 Hz
  // Prescaler = 1024
  // Faixa = 125 (contagem de 0 a OCR0A = 124)
  // Intervalo entre interrupcoes: (Prescaler/Relogio)*Faixa = (64/16e6)*(124+1) = 0.008s
  
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
ISR(TIMER0_COMPA_vect){
  // Insira aqui o codigo a s do temporizadorer executado pela interrupcao periodica
  //cada 8ms
  contagem++;
  contagem_aux++;
  if (contagem >= t_amostragem/8){ 
    //Serial.println(pulsos);
    vel = (pulsos/60)*(60000/t_amostragem); //calculo
    
// 60pulsos = 1 volta --> x volta = 1/60 pulsos
// t_amostragem/60000 = quantidade de minutos
// portanto voltas / minuto = (pulsos/60)*(60000/t_amostragem)
    contagem = 0; //reinicialização das variaveis de apoio
    pulsos = 0;
  }
}
