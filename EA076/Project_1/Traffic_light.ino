volatile unsigned int contagem = 0;
volatile unsigned int contagem_aux = 0;

unsigned char estado = 0; //estado = 0: semaforo veiculos verde, semaforo pedestres vermelho, ambos os displays apagados
unsigned char estado_pisc = 0;

int botao = 0;
int lum1 = 0;
int lum2 = 0;
int lum3 = 0;
int periodo = 0;
int limiar = 83; //médio (utilizado para simulação tinkercad) = 83
				  //valor utilizado no arduino em bancada (protótipo) = 400
void setup(){
  pinMode(8,OUTPUT);//displays
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  
  pinMode(15,OUTPUT);//semaforos
  pinMode(16,OUTPUT);
  pinMode(17,OUTPUT);
  pinMode(18,OUTPUT);
  pinMode(19,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  
  pinMode(6,INPUT);//botao
  pinMode(14,INPUT);//LDR
  
  cli(); // Desabilita interrupcoes globais
  configuracao_Timer0();
  sei(); // Habilita interrupcoes globais 
  
  Serial.begin(9600);
  
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
}

void loop(){
  
  while(periodo == 0){           //NOITE
    
    if(contagem_aux == 0){
      lum1 = analogRead(14); //primeira leitura do LDR
    }
    
    digitalWrite(4, HIGH);//7 segmentos pedestres desligado
    digitalWrite(5, HIGH);//7 segmentos veiculos desligado
    
    if(estado_pisc == 0){
      digitalWrite(15, LOW);//vermelho veiculos apagado
      digitalWrite(16, HIGH);//amarelo veículos aceso
      digitalWrite(17, LOW);//verde veiculos apagado

      digitalWrite(19, LOW);//verde pedestres apagado
      digitalWrite(18, HIGH);//vermelho pedestres aceso
      if(contagem >= 500/8){estado_pisc = 1;}
    }

    if(estado_pisc == 1){ 
      digitalWrite(18, LOW);//vermelho pedestres apagado
      digitalWrite(16, LOW);//amarelo veículos apagado
      if(contagem >= 1000/8){
        estado_pisc = 0;
        contagem = 0;
      }
    }
    
    if(contagem_aux == (5000/8)){
      lum2 = analogRead(14);//segunda leitura do LDR
    }
      
    if(contagem_aux == (10000/8)){
      lum3 = analogRead(14);//terceira leitura do LDR
      contagem_aux = 0;
    }  
    
    if((lum1 <= limiar)&&(lum2 <= limiar)&&(lum3 <= limiar)){periodo = 1;} //comparação dos valores de luminosidade obtidos
 	_delay_ms(1);
  }
  
  
  
  
  
  while(periodo == 1){          //DIA
   
    botao = digitalRead(6);//leitura do botão
    
    if ((botao == 0)&&(estado == 0)){
      
      botao = digitalRead(6);//nova leitura do botão
      
      digitalWrite(15, LOW);//vermelho veiculos apagado
      digitalWrite(16, LOW);//amarelo veículos apagado
      digitalWrite(17, HIGH);//verde veiculos aceso

      digitalWrite(19, LOW);//verde pedestres apagado
      digitalWrite(18, HIGH);//vermelho pedestres aceso

      digitalWrite(4, HIGH);//7 segmentos pedestres desligado
      digitalWrite(5, HIGH);//7 segmentos veiculos desligado
      
      if(contagem_aux == 0){
       lum1 = analogRead(14);//primeira leitura do LDR
      }
      
      if(contagem_aux == (5000/8)){
        lum2 = analogRead(14);//segunda leitura do LDR
      }

      if(contagem_aux == (10000/8)){
        lum3 = analogRead(14);//erceira leitura do LDR
        contagem_aux = 0;
      }
      
      if((lum1 > limiar)&&(lum2 > limiar)&&(lum3 > limiar)){periodo = 0;} //comparação dos valores de luminosidade obtidos
      
    }

    
    
    if ((botao == 1)&&(estado == 0)){
      contagem = 0;
      estado = 100; //espera de 100ms
    }
    if ((estado == 100)&&(contagem >= (100/8))){
      digitalWrite(15, LOW);//vermelho veiculos apagado
      digitalWrite(16, HIGH);//amarelo veículos aceso
      digitalWrite(17, LOW);//verde veiculos apagado

      digitalWrite(19, LOW);//verde pedestres apagado
      digitalWrite(18, HIGH);//vermelho pedestres aceso

      digitalWrite(4, HIGH);//7 segmentos pedestres desligado
      digitalWrite(5, HIGH);//7 segmentos veiculos desligado

      contagem = 0;
      estado = 1;
    }



    if ((estado == 1)&&(contagem > (3000/8))){

      digitalWrite(15, HIGH);//vermelho veiculos aceso
      digitalWrite(16, LOW);//amarelo veículos apagado
      digitalWrite(17, LOW);//verde veiculos apagado

      digitalWrite(19, HIGH);//verde pedestres aceso
      digitalWrite(18, LOW);//vermelho pedestres apagado

      disp_dec(1, 0, 0, 1, 1, 0, 1, 0);
    }



    if ((estado == 2)&&(contagem_aux >= (1000/8))){
      disp_dec(0, 0, 0, 1, 0, 0, 1, 0);
    }  


    if ((estado == 3)&&(contagem_aux >= (1000/8))){      
      disp_dec(1, 1, 1, 0, 1, 1, 0, 0);
    }  



    if ((estado == 4)&&(contagem_aux >= (1000/8))){      
      disp_dec(0, 1, 1, 0, 0, 1, 0, 0);
    }  



    if ((estado == 5)&&(contagem_aux >= (1000/8))){      
      disp_dec(1, 0, 1, 0, 1, 0, 0, 0);
    }  



    if ((estado == 6)&&(contagem_aux >= (1000/8))){
      disp_dec_blink(0, 0, 1, 0);
    }  



    if ((estado == 7)&&(contagem_aux >= (1000/8))){
       disp_dec_blink(1, 1, 0, 0);
    }  



    if ((estado == 8)&&(contagem_aux >=(1000/8))){
       disp_dec_blink(0, 1, 0, 0);
    }  



    if ((estado == 9)&&(contagem_aux >= (1000/8))){
       disp_dec_blink(1, 0, 0, 0);
    }  



     if ((estado == 10)&&(contagem_aux >= (1000/8))){
       
      disp_dec_blink(0, 0, 0, 0);

      estado = 0; //recomeça a rotina de funcionamento
      contagem = 0;
      contagem_aux = 0;
    }  
  
    _delay_ms(1);
    
  }
}



void disp_dec (int bit0v, int bit1v, int bit2v, int bit3v, int bit0p, int bit1p, int bit2p, int bit3p){
  contagem = 0;
  contagem_aux = 0;
  while(contagem_aux <= (1000/8)){ 
    digitalWrite(5, HIGH);//7 segmentos veiculos desligado
    digitalWrite(4, HIGH);//7 segmentos pedestres desligado

    digitalWrite(8, bit0p);//(tempo pedestres)
    digitalWrite(9, bit1p);
    digitalWrite(10, bit2p);
    digitalWrite(11, bit3p);

    digitalWrite(4, LOW);//7 segmentos pedestres ligado 


    digitalWrite(4, HIGH);//7 segmentos pedestres desligado
    digitalWrite(5, HIGH);//7 segmentos pedestres desligado

    digitalWrite(8, bit0v);//(tempo veículos)
    digitalWrite(9, bit1v);
    digitalWrite(10, bit2v);
    digitalWrite(11, bit3v);

    digitalWrite(5, LOW);//7 segmentos veiculos ligado

    digitalWrite(5, HIGH);//7 segmentos veiculos desligado

    _delay_ms(1);
  }
  _delay_ms(1);
  estado++;//segue ao estado seguinte
}



void disp_dec_blink (int bit0v, int bit1v, int bit2v, int bit3v){
  contagem = 0;
  contagem_aux = 0;
  digitalWrite(19, LOW);
  while(contagem_aux <= (1000/8)){ 
	_delay_ms(1);
    while (contagem_aux <= (500/8)){
      digitalWrite(18, HIGH);//semaforo vermelho pedestres ligado

      digitalWrite(5, HIGH);//7 segmentos veiculos desligado
      digitalWrite(4, HIGH);//7 segmentos pedestres desligado

      digitalWrite(8, LOW);//(tempo pedestres)
      digitalWrite(9, LOW);
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);

      digitalWrite(4, LOW);//7 segmentos pedestres ligado 

      digitalWrite(4, HIGH);//7 segmentos pedestres desligado
      digitalWrite(5, HIGH);//7 segmentos pedestres desligado

      digitalWrite(8, bit0v);//(tempo veículos)
      digitalWrite(9, bit1v);
      digitalWrite(10, bit2v);
      digitalWrite(11, bit3v);

      digitalWrite(5, LOW);//7 segmentos veiculos ligado

      digitalWrite(5, HIGH);//7 segmentos veiculos desligado
      _delay_ms(1);
    }

    digitalWrite(18, LOW);//semaforo vermelho pedestres desligado

    digitalWrite(4, HIGH);//7 segmentos pedestres desligado

    digitalWrite(8, bit0v);//(tempo veículos)
    digitalWrite(9, bit1v);
    digitalWrite(10, bit2v);
    digitalWrite(11, bit3v);

    digitalWrite(5, LOW);//7 segmentos veiculos ligado
    _delay_ms(1);
  }
  estado++;
  _delay_ms(1);
}