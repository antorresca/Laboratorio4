#include<xc.h> //Libreria para PIC
#include<stdio.h> //Libreria adicional para manejo de decimales
#include<stdbool.h>
#define _XTAL_FREQ 1000000 //Frecuencia de reloj
#include "LibLCDXC8.h" //Libreria para uso de LCD
#pragma config FOSC=INTOSC_EC //Sentencia para usar oscilador interno
//#pragma config FOSC=XT_XT // Sentencia para usar oscilador de cristal baja resonancia
//#pragma config FOSC=EC_EC // Sentencia para usar oscilador externo
#pragma config WDT=OFF //Apagar el perro guardian 
#pragma config PBADEN=OFF
#pragma config LVP=OFF
#pragma config MCLRE=ON

unsigned char Tecla = 0; //Variable para leer teclado
unsigned long partdecl = 0; //Parte decimal de un resultado
unsigned long r = 0; //Resultado entero de las operaciones
unsigned int n1 = 0; //Primer numero
unsigned int n2 = 0; //Segundo numero
unsigned int i = 0;
unsigned long m = 0;
unsigned char op = ' '; //Operador
unsigned char n1c = ' '; //Primer numero en caracter
unsigned char n2c = ' ';  //Segundo numero en caracter
unsigned int color = 0;
unsigned int contador = 0;
int verificador = 0;
bool flag = false;
unsigned char LeerTeclado(void); //Declarar funcion para lectura de matricial
void ColorRGB(void);

void main(void){
    ADCON1=15; //Deshabilitar funciones analogicas del puerto E
    TRISD=0; //Colocar puerto D como salida
    TRISB=0b11110000; //Colocar puerto B salida/entrada 50/50
    TRISE=0; //Colocar puerto E como salida
    TRISC=0b11110000; //Pines para RGB
    RBPU=0; //Activar resistencias pull up
    InicializaLCD(); //Funcion para configuracion inicial del LCD
    //Timer0 interrupcion
    T0CON=0b10000010;
    TMR0IF=0; //bandera empieza en cero para que la interrupcion permanezca apagada
    TMR0=49911; //Precarga para el oscilador interno y generador de señales
    //TMR0= 3036; //Precarga para el cristal de 4MHz
    TMR0IE=1; //Habilita las interrupcuiones
    GIE=1; //Activar las globales
    TMR0ON=1; //Activa el Timer0 ¿Es necesaria?
    //Fin de configuracion para Timer0
    //Para bajo consumo
    OSCCON = 0b11000100;
    //Fin de bajo conumo
    //Nuevo caracter
    unsigned char heart[]={
    0b00000,
    0b11011,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000
    };
    //Fin de nuevo caracter
    BorraLCD(); //Limpiar el LCD
    if(TO == 1 & RI == 1){
        BorraLCD();
        RI = 0;
        MensajeLCD_Word("Energia");
    }else{
        if(TO == 1 | PD == 0 ){ 
        BorraLCD();
        MensajeLCD_Word("MCLR");
        }
    }
    __delay_ms(2000); //Retraso para evitar errores
    BorraLCD();     
    while(1){
        LATB=0B00000000;
        verificador = 0;
        NuevoCarac(0,heart);
        EscribeLCD_c(0);
        Tecla = LeerTeclado();
        ColorRGB();
        if(Tecla=='C'){ //Limpiar la pantalla si se presiona [1][4] 
            DireccionaLCD(0xC0); //Colocar el cursor en la primera posicion de segunda fila
            //Limpiar variables
            op = ' ';
            n1c = ' ';
            n2c = ' ';
            n1 = 0;
            n2 = 0;
            i = 0;
            partdecl = 0;
            flag =  false;
            DireccionaLCD(0x80); //Colocar el cursor en la primera posicion de primera fila
            MensajeLCD_Word("                "); //Mandar mensaje vacio para limpiar
            DireccionaLCD(0xC0);
            DireccionaLCD(0x80);
        }else{
            if(n1c==' '|n2c==' '|op==' '){ //Funciones si no se ha recibido nada
                if(op==' ' & (Tecla=='+'|Tecla=='-'|Tecla=='/'|Tecla=='x'|Tecla!='=') & n1c != ' ' & n2c ==' '){
                    DireccionaLCD(0x81);
                    EscribeLCD_c(Tecla);
                    op = Tecla;                    
                    if(Tecla=='!') n2c = 'A';
                }else if(n1c == ' ' & (Tecla!='+'|Tecla!='-'|Tecla!='/'|Tecla!='x'|Tecla!='='|Tecla!='C'|Tecla!='!'|Tecla!='^'|Tecla!='=') & op == ' '){
                    DireccionaLCD(0x80);  
                    EscribeLCD_c(Tecla);
                    n1c=Tecla;
                    n1 = Tecla-'0';
                }else if(n2c==' ' & op!=' ' & op!='!' & n1c != ' ' & (Tecla!='+'|Tecla!='-'|Tecla!='/'|Tecla!='x'|Tecla!='='|Tecla!='C'|Tecla!='!'|Tecla!='^'|Tecla!='=')){
                    DireccionaLCD(0x82);
                    EscribeLCD_c(Tecla);
                    n2c=Tecla;
                    n2 = Tecla-'0';
                }
            }else if(Tecla=='=' & n1c != ' ' & op!=' ' & n2c!=' '){
                if(op=='!') DireccionaLCD(0x82);
                else DireccionaLCD(0x83);
                EscribeLCD_c('=');
                switch(op){
                case '+': 
                    r = n1 + n2;
                    partdecl = r*100;
                    break;
                case '-':
                    r = n1 - n2;
                    partdecl = r*100;
                    break; 
                case 'x': 
                    r = n1*n2;
                    partdecl = r*100;
                    break;
                case '/': 
                    if(n2!=0){
                        if(n1!=0){
                        r = n1/n2; //Parte entera
                        partdecl = (n1*100)/n2; //((n1/n2)-r)*10
                        }
                    }else{
                        if(n1 != 0 & n2 == 0) r = 1000;
                        else if(n1 == 0 & n2 == 0) r = 1001;
                        partdecl = r*100;
                    }
                    break;
                case '!':
                    r = n1;
                    for(n1;n1>1;n1--){
                        r = r * (n1-1);
                    }
                    partdecl = r*100;
                    break;
                case '^':
                    r = n1;
                    n2 = n2-1;
                    for(n2;n2>0;n2--){
                        r = r * n1;
                    }
                    partdecl = r*100;
                    break;
                default:
                    r = 0;
                    break;
                }
                if(op=='!') DireccionaLCD(0x83);
                else DireccionaLCD(0x84);
                if ((partdecl-(r*100))!= 0 & (partdecl-(r*100))>0){
                    for (int i = 0; i < 3; i++) {
                        r = n1 / n2;
                        m = n1 % n2;
                        EscribeLCD_c(r+'0');
                        if(i == 0 && m != 0) {
                            EscribeLCD_c(',');
                        }                        
                        if (m != 0) {
                            n1 = m * 10;
                        } else {
                            break;
                        }
                                              
                     }
                }else if(r>0x51 & r!=1000 & r !=1001 & op=='-'){
                    EscribeLCD_c('-');
                    DireccionaLCD(0x85);
                    r = ~r+1;
                    EscribeLCD_c(r+'0'); 
                }else{
                    if(r==1000){
                        MensajeLCD_Word("Inf.");
                    }else{
                        if(r==1001){
                            MensajeLCD_Word("Ind.");
                        }else{
                            if(r>=0xA & r!=1000 & r !=1001){
                                for(int j=9;j>=0;j--){
                                    partdecl = 10;
                                    for(int k=j-1;k>0;k--){   
                                        partdecl = partdecl*10;
                                    }
                                    if(r/partdecl>0){
                                        flag = true;
                                        EscribeLCD_c(r/partdecl+48);
                                        r=r%partdecl;
                                    }else if( (r/partdecl)==0 && flag){
                                        if(r<10) EscribeLCD_c(r+48);
                                        else EscribeLCD_c(48);
                                    }
                                }
                            }else{
                                EscribeLCD_c(r+'0'); 
                            }
                        }
                    }
                }
            }else{
                EscribeLCD_c(' ');
            }
        __delay_ms(700);
        }
    }
}

    
unsigned char LeerTeclado(void){
    while(RB4==1 && RB5==1 && RB6==1 && RB7==1);
    verificador = 1;
    LATB=0b11111110;
    if(RB4==0) return '1';
    else if(RB5==0) return '2';
    else if(RB6==0) return '3';
    else if(RB7==0 & RC6==0) return '+';
    else if(RB7==0 & RC6==1) return '!';
    else{
    LATB=0b11111101;
    if(RB4==0) return '4';
    else if(RB5==0) return '5';
    else if(RB6==0) return '6';
    else if(RB7==0 & RC6==0) return '-';
    else if(RB7==0 & RC6==1) return '^';
    else{
    LATB=0b11111011;
    if(RB4==0) return '7';
    else if(RB5==0) return '8';
    else if(RB6==0) return '9';
    else if(RB7==0) return '/';
    else{
    LATB=0b11110111;
    if(RB4==0) return 'C';
    else if(RB5==0) return '0';
    else if(RB6==0) return '=';
    else if(RB7==0) return 'x';
    }
    }
    }
    return '?';
}

void ColorRGB(void){
    if(color==8) color = 0;
    switch(color){
        case 0:
            LATC = 0b00000000;
            break;
        case 1:
            LATC = 0b00000101;
            break;
        case 2:
            LATC = 0b00000110;
            break;
        case 3:
            LATC = 0b00000011;
            break;
        case 4:
            LATC = 0b00000010;
            break;
        case 5:
            LATC = 0b00000110;
            break;
        case 6:
            LATC = 0b00000100;
            break; 
        case 7:
            LATC = 0b00000111;
            break;   
        default:
            break;
    }
    color += 1;
}

void __interrupt() ISR(void){ 
    if(TMR0IF==1){
        if(!verificador) contador = contador +1;
        else contador = 0;
        TMR0IF=0;
        LATE2 = !LATE2;
        TMR0 = 49911;
        //TMR0= 3036; 
    }
    
    if(contador == 20){
        if(!verificador){   
            BorraLCD();
            MensajeLCD_Word("En Resposo");
            SLEEP();
            while(1);
        }
    }
    
}
