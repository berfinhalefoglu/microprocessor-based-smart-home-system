#include <reg51.h>
#include <string.h>

sbit rs = P2^0;
sbit en = P2^1;
sfr ldata = 0x90;  // LCD veri hatti ? P1

unsigned char a[12];  // RFID ID verisi
unsigned int i;

void delay(unsigned int d) {
    unsigned int j;
    for(j = 0; j < d; j++);
}

void lcd_cmd(unsigned char cmd) {
    rs = 0;
    ldata = cmd;
    en = 1;
    delay(500);
    en = 0;
}

void lcd_data(unsigned char d) {
    rs = 1;
    ldata = d;
    en = 1;
    delay(500);
    en = 0;
}

void lcd_str(unsigned char *str) {
    while(*str) {
        lcd_data(*str++);
    }
}

void lcd_init() {
    lcd_cmd(0x38);  // 8-bit, 2-line
    lcd_cmd(0x0C);  // Display ON, Cursor OFF
    lcd_cmd(0x01);  // Clear display
    lcd_cmd(0x80);  // Start at first line
}

void clear_array() {
    for(i = 0; i < 12; i++) {
        a[i] = '\0';
    }
}

void main() {
    lcd_init();

    TMOD = 0x20;     // Timer1 mode2 - auto reload
    TH1 = 0xFD;      // 9600 baud rate
    SCON = 0x50;     // 8-bit UART mode
    TR1 = 1;         // Start Timer1

    lcd_str("RFID Ready");
    lcd_cmd(0xC0);
    lcd_str("Scan Your Card");

    while(1) {
        // 10 karakter RFID ID oku ve echo (geri yazim) yap
        for(i = 0; i < 10; i++) {
            while(RI == 0);      // Veri gelene kadar bekle
            a[i] = SBUF;         // Seri veriyi oku
            RI = 0;

            // Echo: karakteri terminale geri gönder
            SBUF = a[i];
            while(TI == 0);
            TI = 0;
        }

        a[10] = '\0';  // String sonlandirici

        lcd_cmd(0x01);  // LCD ekrani temizle
        lcd_cmd(0x80);

        if(strcmp(a, "10003B0CA7") == 0) {
            lcd_str("EMPLOYEE NO: 1");
        }
        else if(strcmp(a, "10003B0CAE") == 0) {
            lcd_str("EMPLOYEE NO: 2");
        }
        else {
            lcd_str("INVALID CARD");
        }

        lcd_cmd(0xC0);   // Degilse sunu dene:
				lcd_cmd(0x94);

        lcd_str(a);  // Kart ID'yi LCD'de göster
        delay(30000);

        lcd_cmd(0x01);
        lcd_str("Scan Again...");
    }
}
