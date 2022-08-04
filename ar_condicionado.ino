#include <ESP8266WiFi.h>      //Versao 2.5.2
#include <WiFiUdp.h>
#include <Arduino_SNMP.h>
#include <IRremoteESP8266.h>  //Aumentar RAWBUF p/ 255 --> "IRremoteESP8266.h"

//Credenciais da rede Wi-Fi
const char* ssid = "";
const char* password = "";

//Definicao IP fixo nodemcu
IPAddress ip(200,137,86,70);

IPAddress gateway(200,137,86,1);   //gateway
IPAddress subnet(255,255,255,128); //mascara de rede

//Instancia que pode enviar e receber msgs udp
WiFiUDP udp; 
SNMPAgent snmp = SNMPAgent("public");

//Objeto para a transmissao do sinal infravermelho (Utilizado o GPIO 4 - Pino D2)
IRsend irsend(4);
int tamanho = 199;   //tamanho vetor raw
int frequencia = 38; //frequencia sinal a ser emitido

//Comandos clonados do controle remoto
unsigned int DESLIGA_AR[199]= {4406,4361,557,1588,556,517,556,1588,556,1589,613,459,556,516,557,1600,545,516,556,515,615,1531,556,516,556,516,557,1588,613,1533,556,516,556,1589,614,458,556,1589,614,1531,557,1588,557,1588,557,516,557,1588,556,1589,556,1589,557,516,556,516,556,517,556,517,556,1589,556,516,556,517,556,1588,614,1531,557,1589,557,517,555,515,615,459,556,516,557,516,614,458,557,516,557,516,556,1590,556,1589,555,1589,557,1588,558,1589,612,5117,4459,4306,557,1589,556,516,557,1589,557,1588,557,516,614,459,556,1589,557,516,556,517,556,1588,558,516,557,515,556,1590,556,1589,557,516,556,1589,557,517,556,1588,557,1588,614,1533,556,1589,557,516,556,1589,557,1592,565,1577,556,517,556,517,556,517,612,460,556,1590,556,519,554,516,614,1531,557,1589,557,1589,556,516,557,515,558,516,557,516,557,517,555,517,556,517,556,516,556,1590,556,1589,557,1601,613,1520,557,1590,556};
unsigned int LIGA_AR[199]={4500,4350,650,1550,650,500,650,1550,650,1550,650,500,650,500,600,1600,600,500,650,500,650,1550,650,500,650,500,600,1600,650,1550,700,450,650,1550,650,1550,600,500,650,1550,600,1600,650,1550,600,1600,600,1600,600,1600,600,500,650,1550,650,500,650,500,700,450,600,500,600,550,600,500,650,500,650,1550,650,1550,650,500,600,500,600,500,650,500,650,450,550,1650,650,500,650,500,650,1550,600,1550,650,1550,650,1550,600,1600,650,5150,4500,4300,650,1550,650,500,650,1550,550,1650,650,500,650,500,600,1600,650,500,650,500,650,1550,650,500,650,500,600,1600,600,1600,650,450,650,1550,650,1550,650,500,650,1550,600,1600,600,1550,600,1600,600,1600,600,1600,600,500,650,1550,650,500,650,500,650,500,650,500,600,500,650,500,650,500,600,1600,650,1550,650,500,600,500,650,500,650,500,600,500,650,1550,650,450,650,500,600,1600,650,1550,650,1550,650,1550,600,1600,650};

//Sinal recebido atraves do set snmp realizado pelo pandora
//Esse sinal sera interpretado como o codigo para um comando do controle
int sinal = -1;

int status_ar = -1; 

void setup() {
  Serial.begin(115200);
  irsend.begin();     //Seta o pino de infravermelho para output
  pinMode(2, OUTPUT); //Marca o led do ESP8266 como output
  setupWiFi();
  setupSNMP();
}

void loop() {
  verifySNMP();
  delay(1000);
}

//  Função que conecta o ESP8266 no WiFi
void setupWiFi() {
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(2, LOW);  //Pisca o led do nodemcu enquanto nao conecta no WiFi
    delay(230);
    Serial.print(".");
    digitalWrite(2, HIGH);
    delay(230);
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(2, HIGH);
}


//  Função que inicializa o snmp
void setupSNMP() {
  snmp.setUDP(&udp);
  snmp.begin();
  
  //Adiciona o OID para o sinal
  snmp.addIntegerHandler(".1.3.6.1.4.1.12345.3", &sinal, true);
  
}

void enviaSinal() {
  /*
    Codigo dos sinais enviados pelo ESP:
    0 - Desligar 
    1 - Ligar
    2 - Sincronizar 
  */
  
  switch(sinal)
  {
    case 0:
      irsend.sendRaw(DESLIGA_AR, tamanho, frequencia); //param: array com duracao dos pulsos, tamanho deste array, freq a ser emitida
      status_ar = 0;
      Serial.println("Comando enviado: Desliga Ar");
      break;
    case 1:
      irsend.sendRaw(LIGA_AR, tamanho, frequencia);
      status_ar = 1;
      Serial.println("Comando enviado: Ligar ar setado na temperatura 21 °C");
      break;  
    case 2:
      //Sincronizacao Ar
      if(status_ar == 1){
         status_ar = 0;
         sinal=0;
      }
      else if(status_ar== 0){
         status_ar = 1;
         sinal = 1; 
      }
      break;
    default:
      Serial.println("Comando invalido");
      break;
  }
  
  delay(5000);
}

void verifySNMP() {
  snmp.loop();//Recebe sinal snmp
  //Checa se ocorreu um set
  if (snmp.setOccurred)
  {
    Serial.println("Set ocurred");
    Serial.println(sinal);
    enviaSinal();
    snmp.resetSetOccurred();//reseta flag setOccurred 
  }
}
