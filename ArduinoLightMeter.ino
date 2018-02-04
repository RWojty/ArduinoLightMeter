#include <SPI.h>

#include <Ucglib.h>
// Création de l’objet ucg
// avec dans l’ordre les broches pour
// CLK, MOSI, DC, CS, et RESET

#include <Adafruit_Sensor.h>        // Librairie généralisée pour les capteurs Adafruit 
#include "Adafruit_TSL2591.h"       // Librairie du TSL2591
#include <EEPROM.h>                 // Librairie pour utiliser l'EEPROM

    // Le type utilisé désigne à la fois le modèle d’écran et le mode de connexion SPI
    // Ici l’écran sur base ILI9341 240x320 pixels
    // en couleurs 18 bits (262144 couleurs)

Ucglib_ILI9341_18x240x320_SWSPI ucg(/*sclk=*/ 7, /*data=*/ 6, /*cd=*/ 5 , /*cs=*/ 3, /*reset=*/ 4);  // On spécifie notre connexion SPI

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);    // La librairie adafruit permet de reconnaître directement notre capteur  

float lux;                          //Lux 
float Ev;                           //Indice d'illumination 

float Volts ;           // Tension de la pile 

int Aadr = 0 ;
int Tadr = 1 ;
int Sadr = 2 ;

int Am = EEPROM.read(Aadr);                                                                        // On lit notre position dans le tableau dans l'EEPROM (addr = 0)
int Tm = EEPROM.read(Tadr);                                                                       // On lit notre position dans le tableau dans l'EEPROM (addr = 1)
int Sm = EEPROM.read(Sadr);                                                                        // On lit notre position dans le tableau dans l'EEPROM (addr = 2) 

float A[] = {1, 1.4, 2, 2.8, 4, 5.6, 8, 11, 16, 22, 32, 45, 64, 90};                          // Focales 
float S[] = {6, 12, 25, 50, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200};       // ISO
float T[] = {0.00025, 0.0003125, 0.0004, 0.0005, 0.000625, 0.0008, 0.001, 0.00125, 0.0015625, 0.002, 0.0025, 0.003125, 0.004, 0.005, 0.00625, 0.008, 0.01, 0.0125, 0.016666, 0.02, 0.025, 0.00333, 0.04, 0.05, 0.06666, 0.07692, 0.1, 0.125, 0.1666, 0.2, 0.5, 1}; 

// Ce tableau va nous permettre d'afficher le temps sous forme de fraction (si Tm <=30)
float DisplayT[] = {4000, 3200, 2500, 2000, 1600, 1250, 1000, 800, 640, 500, 400, 320, 250, 200, 160, 125, 100, 80, 60, 50, 40, 30, 25, 20, 15, 13, 10, 8, 6, 5, 2};



// variables qui vous nous permettre de contrôler nos menus 

boolean Menu1 = 0 ;   // Permet de passer de Av à Tv
int Menu2 = 0 ;   // Permet de passer de ISO à la focale au temps.


int Bn0p = 13;                           // bouton de sélection  
int Bn1p = 12;                          //- bouton incrémentation  
int Bn2p = 11;                          //- bouton incrémentation  

boolean Bn0 ;                            // sélection
boolean Bn1 ;                            //+ incrémentation 
boolean Bn2 ;                            //- incrémentation



void icrementISO(void)
{
ucg.setPrintPos(190,250);
ucg.setFont(ucg_font_7x14B_mr); 

     while(Bn0 == 1) { // Tant que l'utilisateur ne clique par sur le bouton de sélection
         delay(150) ; // on met du délais pour les valeurs s'incrémente une par une 
  
        Bn0 = digitalRead(Bn0p);
        Bn1 = digitalRead(Bn1p);
        Bn2 = digitalRead(Bn2p);
        
        ucg.print("     ") ;   // On efface la valeur précédente 
        ucg.setColor(252, 252, 71);
        ucg.setPrintPos(190,250);
        ucg.print(S[Sm],0);   // On affiche 0 décimales 
       

            if(Bn1 == 1)
            {
            Sm = Sm + 1 ; 
            }
    
             if(Bn2 == 1)
             {
             Sm = Sm - 1 ;
             }

             if(Sm < 0)   // Sécurité pour ne pas tomber dans des valeurs éronnées 
             {
             Sm = 13 ;    
             }

             if(Sm > 13)
             {
              Sm =0 ;
             }
      }
    ucg.print("      ") ;     // dernier nettoyage pour retourner à notre valeur en blanc 
}

void icrementF(void)
{
ucg.setPrintPos(190,185);
ucg.setFont(ucg_font_7x14B_mr); 

     while(Bn0 == 1) { // Tant que l'utilisateur ne clique par sur le bouton de sélection
     delay(150) ; // on met du délais pour les valeurs s'incrémente une par une  
            
        Bn0 = digitalRead(Bn0p);
        Bn1 = digitalRead(Bn1p);
        Bn2 = digitalRead(Bn2p);
        
       FDisplay() ; 
               
    
            if(Bn1 == 1)
            {
            Am = Am + 1 ; 
            FDisplay() ;      //  On affiche la focale pendant que l'utilisateur maintient le boutton 
            }
    
             if(Bn2 == 1)
             {
             Am = Am - 1 ;
            FDisplay() ; 
             }

             
           if(Am < 0)   // Sécurité pour ne pas tomber dans des valeurs éronnées 
             {
             Am = 13;    
             }

             if(Am >= 14)
             {
              Am =0 ;
             }

       
      }
    ucg.setPrintPos(190,185);
    ucg.print("     ") ;     // dernier nettoyage pour retourner à notre valeur en blanc 
}

void icrementTime(void)
{
     while(Bn0 == 1) { // Tant que l'utilisateur ne clique par sur le bouton de sélection
        delay(100) ; // on met du délais pour les valeurs s'incrémente une par une 
    
        Bn0 = digitalRead(Bn0p);
        Bn1 = digitalRead(Bn1p);
        Bn2 = digitalRead(Bn2p);


        TimeDisplay(); // On affiche le temps     

            if(Bn1 == 1)
            {
            Tm = Tm + 1 ; 
             TimeDisplay(); // On affiche le temps pendant que l'utilisateur maintient le boutton 
            }
    
             if(Bn2 == 1)
             {
             Tm = Tm - 1 ;
              TimeDisplay();
             }

             if(Tm < 0)   // Sécurité pour ne pas tomber dans des valeurs éronnées 
             {
             Tm = 31 ;    
             }

             if(Tm > 31)
             {
             Tm = 0 ;
              
             }
      }
      
   ucg.setPrintPos(195,107);    
   ucg.print("     ") ;     // nettoyage des valeurs pour retourner à notre valeur en blanc 
   ucg.setPrintPos(180,107);
   ucg.print("     ") ;   
   ucg.setPrintPos(168,107);
   ucg.print("     ") ;  
}

void FDisplay(void) {
  
ucg.setFont(ucg_font_7x14B_mr); 
ucg.setColor(255, 255, 255);// initialisation de la couleur d'affichage 

ucg.setPrintPos(190,185) ;
ucg.print("     ") ;  // On efface la valeur précédente 

        if(Menu1 == 0 && Menu2==0 && Bn0 == 1)     // Dans le cas où l'on incrémente la focale en mode Av
        {   
        ucg.setColor(252, 252, 71);  // Changement de la couleur d'affichage 
        }

         if(Menu1 == 1)  // Dans le cas où l'on calcule et que l'on affiche la focale en Tv
        {    
        ucg.setColor(255, 0, 0);  // Changement de la couleur d'affichage
        
        float goodF = sqrt( (S[Sm]*lux*T[Tm]) /  64 ) ; 
        float P = abs(A[0] - goodF) ;
        float resultat = A[0] ;  

          for (int i=1; i < 13; i++){
            if(abs(A[i] - goodF) < P)
            {
              P = abs(A[i] - goodF) ;
              resultat = A[i];
              Am = i ;
            }
          }
         }

        ucg.setPrintPos(190,185);
        ucg.print(A[Am],1); // On affiche 1 décimale
        
}

void TimeDisplay(void) 
{   
  
ucg.setFont(ucg_font_7x14B_mr); 
ucg.setColor(255, 255, 255);// initialisation de la couleur d'affichage 

ucg.setPrintPos(195,107);    
ucg.print("     ") ;     // on efface notre valeur précédente
ucg.setPrintPos(180,107);
ucg.print("     ") ;   
ucg.setPrintPos(168,107);
ucg.print("     ") ;  
        
        if(Menu1 == 1 && Menu2==0 && Bn0 == 1)     // Dans le cas où l'on incrémente le temps en mode Tv  
        {   
        ucg.setColor(255, 252, 71);  // Changement de la couleur d'affichage 
         }

        if(Menu1 == 0)  // Dans le cas où l'on calcule et que l'on affiche le temps en mode Av
        {      
        ucg.setColor(255, 0, 0);  // Changement de la couleur d'affichage
        
        float goodtime = pow(A[Am],2)*64/(lux*S[Sm]); 
        float P = abs(T[0] - goodtime) ;
        float resultat = T[0] ; 

       
          for (int i=1; i < 31; i++){
            if(abs(T[i] - goodtime) < P)
            {
              P = abs(T[i] - goodtime) ;
              resultat= T[i];
              Tm = i ;
            }   
          }
        }
    
    if(Tm == 31)  // dans le cas où le temps est égal à 1s
    {     
    ucg.setPrintPos(180,115);
    ucg.print(T[Tm],0); // On affiche 0 décimales 
    }

    else        // Affichage sous forme de fraction
    {
    ucg.setPrintPos(195,118);
    ucg.print("1");
    ucg.setPrintPos(180,107);
    ucg.print("----");
    ucg.setPrintPos(168,107);
    ucg.print(DisplayT[Tm],0); // On affiche 0 décimales
    }
}

void lectureTSL(void) 
{  
  sensors_event_t event;   // On récupère notre mesure
  tsl.getEvent(&event);
  lux = event.light;

     if ((event.light == 0) | (event.light > 4294966000.0) |(event.light <-4294966000.0))  // Correspondent aux codes d'erreurs
     {
    lux = 10000.0;  // on fixe une valeur pour que le gain se fixe à low et le temps de pose à 100ms 
      }                 // Avec ces deux réglages, on pourra lire n'importe quelle valeur même dans des cas extrèmes, mais avec une précision réduite.


      if (lux > 200.0)      // On règle notre capteur en fonction de la valeur de lux
  { 
    tsl.setGain(TSL2591_GAIN_LOW);              
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
  }
  else if (lux <=200.0 && lux > 40.0)
  {
    tsl.setGain(TSL2591_GAIN_MED);                
    tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  }
  else if (lux <=40.0 && lux > 10.0)
  {
    tsl.setGain(TSL2591_GAIN_MED);                
    tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  }
  else if (lux <=10.0 && lux > 0.1)
  {
    tsl.setGain(TSL2591_GAIN_HIGH);                
    tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  }
  else
  {
    tsl.setGain(TSL2591_GAIN_MAX);                
    tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  }

  tsl.getEvent(&event);  // On refait une mesure une fois l'appareil correctement réglé 
  lux = event.light;     // Temps max d'une mesure : 1s (pour lux <= 10) 

}

void setup(void)
{
  tsl.begin();
  tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS); // Initialisation du temps de pose et du gain
  tsl.setGain(TSL2591_GAIN_LOW); 
  
  lectureTSL() ; // On lit on première fois notre capteur pour obtenir une valeur en lux avec laquelle travailler 


// 1s pour laisser l’écran s’initialiser

delay(1000);


ucg.begin( UCG_FONT_MODE_SOLID); // Ce mode nous permettra d'écrire les valeurs par dessus les unes des autres  

ucg.setColor(1, 0, 0,0); // définition d'un font noir pour le background (non visible) 

ucg.drawGradientBox(0, 0, 240, 320);  // On dessine ici un dégradé de gris pour le fond 

ucg.setColor(252, 252, 71); 

 for (int i=0; i <= 4; i++) // On crée ici une boucle for pour répéter plusieurs fois une frame afin d'en faire un cadre
{
    ucg.drawRFrame(20+i,25+i, 200+i, 270+i, 8);
    ucg.drawRFrame(20+i+1,25+i+1, 200+i+1, 270+i+1, 8);
}

  
ucg.setColor(230, 230, 230);
ucg.setPrintDir(1);    // Permet de spécifier dans quelle direction l'on écrit

ucg.setFont(ucg_font_inb16_mr);     

ucg.setPrintPos(165,50);      // Permet de spéficier à partir de quelle position l'on écrit
ucg.print("Arduino Exposure");
ucg.setPrintPos(145,135);
ucg.print("Meter");

ucg.setFont(ucg_font_10x20_mr);
ucg.setPrintPos(110,110);
ucg.print("Created by :");
ucg.setPrintPos(90,105);
ucg.print("Remi Wojtyna");


delay(1500) ;   // On laisse le temps à l'utilisateur de bien lire qui s'est fait chier à écrire les 500 lignes de programme

ucg.clearScreen();   // On efface tout, fin de la présentation !

// On affiche notre interface  
ucg.setColor(255, 255, 255);

ucg.drawFrame(1, 1, 80, 77);   // On dessine ici chacun de nos rectangles
ucg.drawFrame(80, 1, 80, 77);
ucg.drawFrame(160, 1, 80, 77);

ucg.drawFrame(150, 87, 90, 78);
ucg.drawFrame(150, 165, 90, 78);
ucg.drawFrame(150, 243, 90 , 77);

ucg.drawFrame(15, 120, 70, 80);
ucg.drawFrame(15, 200, 70, 80);

ucg.setFont(ucg_font_7x14B_mr); 
ucg.setPrintPos(215,25);
ucg.print("EV :");

ucg.setFont(ucg_font_helvB14_hr);
ucg.setPrintPos(115,25);
ucg.print("Av");
ucg.setPrintPos(35,25);
ucg.print("Tv");


ucg.setFont(ucg_font_7x14B_mr); 
ucg.setPrintPos(45,132);
ucg.print("Measure");

ucg.setPrintPos(50,225);
ucg.print("Change");
ucg.setPrintPos(35,225);
ucg.print("Av/Tv");

ucg.setPrintPos(130,130);
ucg.print("lux :");

ucg.setPrintPos(220,107);
ucg.print("Time");
ucg.setPrintPos(220,195);
ucg.print("F");
ucg.setPrintPos(220,267);
ucg.print("ISO");

}

void loop(void)
{

  Bn0 = digitalRead(Bn0p);
  Bn1 = digitalRead(Bn1p);
  Bn2 = digitalRead(Bn2p);

  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(190,250);
  ucg.print(S[Sm],0); // On affiche 0 décimale pour l'iso

  ucg.setPrintPos(130,170);
  ucg.print(lux,1); // On affiche 1 décimale pour les lux
  
  FDisplay() ; // Affichage de la focale

  TimeDisplay() ; // Affichage du temps

  
  Ev = log(pow(A[Am],2))/log(2) + log(T[Tm])/log(2);  // pour obtenir un log2 sous arduino : logx(n) = logy (n) / logy (x)
  ucg.setColor(255, 255, 255);
  ucg.setPrintPos(190,25);
  ucg.print(floor(Ev+0.5),1) ; // On affiche 0 décimale

                   if(Bn1 == 1) {
                      Menu2 = Menu2 + 1 ;
                    }
                    
                    if(Bn2 == 1) {
                      Menu2 = Menu2 - 1 ;
                    }
                    
                    if( Menu2 >= 4) { // Sécurité pour ne pas tomber dans des valeurs éronnées 
                      Menu2 = 0 ;
                    }
                    
                    if(Menu2 < 0) {  // Sécurité pour ne pas tomber dans des valeurs éronnées 
                      Menu2 = 3 ;
                    }

  if(Menu1==0) 
  {  
  // ATTENTION : Gros bug sur les fonds tr, tf, ect... pour l'affichage en SOLID
  
        ucg.setFont(ucg_font_helvB14_hr);
        ucg.setColor(252, 252, 71);  // On indique à l'utilisateur que l'on est en mode "Av"
        ucg.setPrintPos(115,25);
        ucg.print("Av");

        ucg.setFont(ucg_font_helvB14_hr);  // On passe "Tv" en blanc
         ucg.setColor(255, 255, 255);  
         ucg.setPrintPos(35,25);
         ucg.print("Tv");
        
         ucg.setFont(ucg_font_7x14B_mr);  
         ucg.setColor(255, 0, 0);  // On indique à l'utilisateur que le temps est bloqué
         ucg.setPrintPos(220,107);
         ucg.print("Time");
             
            
            if(Menu2==0)
            {
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(252, 252, 71);  // On indique à l'utilisateur que l'on est sur F
            ucg.setPrintPos(220,195);
            ucg.print("F"); 
             
                if(Bn0==1) {
                icrementF() ; 
                }
            }

            else {
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(255, 255, 255); 
            ucg.setPrintPos(220,195);
            ucg.print("F"); 
            }
        
            if(Menu2==1)
            {  
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(252, 252, 71);  // On indique à l'utilisateur que l'on est sur l'ISO
            ucg.setPrintPos(220,267);
            ucg.print("ISO");
  
                if(Bn0==1) {
                icrementISO() ;  
                }
            }

            else{
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(255, 255, 255);  
            ucg.setPrintPos(220,267);
            ucg.print("ISO");             
            }

  
            if(Menu2==2)
            {  
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(252, 252, 71);  // On indique à l'utilisateur que l'on est sur la mesure 
            ucg.setPrintPos(45,132);
            ucg.print("Measure");

                 if(Bn0==1) {
                  lectureTSL() ;          
                  }
            }

            else{  
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(255, 255, 255);  
            ucg.setPrintPos(45,132);
            ucg.print("Measure");
            
            }
  
            if(Menu2==3)
            {  
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(252, 252, 71); 
            ucg.setPrintPos(50,225);
            ucg.print("Change");
            ucg.setPrintPos(35,225);
            ucg.print("Av/Tv");

                   if(Bn0==1) {
                  Menu1 = 1 ; // on change de menu
                  Menu2 = 0 ;
                  }

            }

            else
            {
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(255, 255, 255); 
            ucg.setPrintPos(50,225);
            ucg.print("Change");
            ucg.setPrintPos(35,225);
            ucg.print("Av/Tv");
            }
    }

   if(Menu1==1) 
  {  
  
         ucg.setFont(ucg_font_helvB14_hr);  // On indique à l'utilisateur que l'on est en mode "Tv"
         ucg.setColor(252, 252, 71);  
         ucg.setPrintPos(35,25);
         ucg.print("Tv");

        ucg.setColor(255, 255, 255);  // On passe Av en blanc
        ucg.setPrintPos(115,25);
        ucg.print("Av");
         
        ucg.setFont(ucg_font_helvB14_hr); 
        ucg.setFont(ucg_font_7x14B_mr);
        ucg.setColor(255, 0, 0);  // On indique à l'utilisateur que F est bloqué
        ucg.setPrintPos(220,195);
        ucg.print("F");
       
             
            
            if(Menu2==0)
            {
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(252, 252, 71); 
            ucg.setPrintPos(220,107);  // On indique à l'utilisateur que l'on règle le temps 
            ucg.print("Time"); 
             
                if(Bn0==1) {
                icrementTime() ; 
                }
            }

            else {
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(255, 255, 255); 
            ucg.setPrintPos(220,107); 
            ucg.print("Time");
            }
        
            if(Menu2==1)
            {  
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(252, 252, 71);  // On indique à l'utilisateur que l'on est sur l'ISO
            ucg.setPrintPos(220,267);
            ucg.print("ISO");
  
                if(Bn0==1) {
                icrementISO() ;  
                }
            }

            else{
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(255, 255, 255);  
            ucg.setPrintPos(220,267);
            ucg.print("ISO");             
            }

  
            if(Menu2==2)
            {  
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(252, 252, 71);  // On indique à l'utilisateur que l'on est sur la mesure 
            ucg.setPrintPos(45,132);
            ucg.print("Measure");

                 if(Bn0==1) {
                  lectureTSL() ;          
                  }
            }

            else{  
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(255, 255, 255);  
            ucg.setPrintPos(45,132);
            ucg.print("Measure");
            
            }
  
            if(Menu2==3)
            {  
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(252, 252, 71); 
            ucg.setPrintPos(50,225);
            ucg.print("Change");
            ucg.setPrintPos(35,225);
            ucg.print("Av/Tv");

                   if(Bn0==1) {
                  Menu1 = 0 ; // on change de menu
                  Menu2 = 0 ;
                  }

            }

            else
            {
            ucg.setFont(ucg_font_7x14B_mr);  
            ucg.setColor(255, 255, 255); 
            ucg.setPrintPos(50,225);
            ucg.print("Change");
            ucg.setPrintPos(35,225);
            ucg.print("Av/Tv");
            }
    }
            
  EEPROM.write(Aadr, Am);
  EEPROM.write(Sadr, Sm);
  EEPROM.write(Tadr, Tm);
      

 }

