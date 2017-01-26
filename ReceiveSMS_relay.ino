#include <GSM.h>
#define PINNUMBER ""


// pin 2 (rouge, RX du UNO, TX du SIM900) 
// pin 3 (orange, TX du UNO, RX du SIM900)
// pin 13 : commande relay et LED 
// marrond, rouge, noir =  GND , 5V, GND

GSM gsmAccess;
GSM_SMS sms;

char senderNumber[20];

void setup() {
        Serial.begin(9600);
        Serial.println("SMS Messages Receiver");

        boolean notConnected = true;

        while (notConnected) {
                if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
                        notConnected = false;
                } else {
                        Serial.println("Not connected");
                        delay(1000);
                }
        }

        Serial.println("GSM initialized");
        Serial.println("Waiting for messages");

        pinMode(13, OUTPUT);  
        digitalWrite(13, LOW);


        sms.beginSMS("+33673997334");
        //  sms.beginSMS("+33787025242");
        sms.print("Module GSM-relai operationel ; relai OFF ; pret a recevoir instructions ('F' pour OFF ou 'N' pour ON)");
        sms.endSMS();
        Serial.println("\nMessage envoye!\n");


}

void loop() {
        char c;

        // If there are any SMSs available()
        if (sms.available()) {
                Serial.println("Message received from:");

                // Get remote number
                sms.remoteNumber(senderNumber, 20);
                Serial.println(senderNumber);

                // An example of message disposal
                // Any messages starting with # should be discarded
                if (sms.peek() == '#') {
                        Serial.println("Discarded SMS");
                        sms.flush();
                }

                // Read message bytes and print them
                boolean i = true;
                char code;
                while (c = sms.read()) {
                        if( i == true ){
                                code = c;
                                i = false;
                        }
                        Serial.print(c);
                }
                Serial.println("\nEND OF MESSAGE");

                // Delete message from modem memory
                sms.flush();
                Serial.println("MESSAGE DELETED");

                // Agit 
                int cas = process_code(code);

                // repond a à l'envoyeur
                sms.beginSMS(senderNumber);
                switch (cas){
                        case 0:
                                sms.print("Extinction du relai -- recu");
                                break;
                        case 1:
                                sms.print("Allumage du relai -- recu");
                                break;
                        case 2:
                                sms.print("Juste un caractere : 'N' pour ON ou 'F' pour OFF");
                                break;
                }
                sms.endSMS();
                Serial.println("\nMessage envoye a l'envoyeur!\n");

                // informe l'admin
                sms.beginSMS("+33673997334");
                switch (cas){
                        case 0:
                                sms.print("Extinction du relai -- recu");
                                break;
                        case 1:
                                sms.print("Allumage du relai -- recu");
                                break;
                        case 2:
                                sms.print("Juste un caractere : 'N' pour ON ou 'F' pour OFF");
                                break;
                }
                sms.endSMS();
                Serial.println("\nMessage envoye a l'admin!\n");


        } 


        delay(1000);

}

int process_code(char code){
        /* // CLIGNOTEMENT POUR DEBUG
           for(int i=0;i<10;i++){
           digitalWrite(13, HIGH);
           delay(50);
           digitalWrite(13, LOW);
           delay(50);
           } 
         */
        if (code == 'F'){
                digitalWrite(13, LOW);    
                return 0;
        }
        else if (code == 'N'){
                digitalWrite(13, HIGH);    
                return 1;
        }
        else{
                return 2;
        }
}
