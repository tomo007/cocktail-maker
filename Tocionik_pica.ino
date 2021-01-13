
#include <SPI.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(10);

#define buzzer 5

#define gumbZaGore 4
#define gumbZaDolje 3
#define gumbZaOdabir 2
#define gumbZaPovratak 12


#define relay_bijelo 6
#define relay_crno 8
#define relay_cola 9
#define relay_mineralna 7

#define MAX_BROJ_BEEP 2
#define DELAY delay(200)


/////////Deklaracija Animacije////

int redPixelaLCD;
byte tociona[8];
byte prazan[4] = {0};
int varijabla;

byte strelica [] = {
  B00000,
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000,
};
byte kvacica[] = {
  B00000,
  B00000,
  B00000,
  B00001,
  B00010,
  B10100,
  B01000,
  B00000,
};

byte boca[8] = {
  B11111,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte bocapola[8] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
};
byte bocavrat[8] = {
  B00000,
  B00000,
  B00110,
  B11110,
  B11110,
  B11110,
  B00110,
  B00000,
};
byte casa[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B01010,
  B00100,
  B00100,
  B01110,
};
//////////////////////////////Deklaracija//////////////////////////////////////////////////////

char *ispisMenija [] = {
  "Gemist",
  "Bambus",
  "Cola",
  "Mineralna",
};

char *ispisPostotaka[] = {"10", "20", "30", "40", "50", "60", "70", "80", "90", "100"};
int vrijednostPostotaka[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

int flagStrelica;

bool vrijednostGumbaZaDolje, vrijednostGumbaZaGore, vrijednostGumbaZaOdabir, vrijednostGumbaZaPovratak;

int trenutnaPozicijaMenija = 0;
int brojPonudeSadrzajaMenija = sizeof(ispisMenija) / sizeof(*ispisMenija);

int trenutnaPozicijaPostotka = 0;
int brojPonudePostotaka = sizeof(ispisPostotaka) / sizeof(*ispisPostotaka);

bool flagPumpa1 = 0, flagPumpa2 = 0;

//tajmer
unsigned long trenutnoVrijeme;
long vremenskiIntervalRadaPumpePrva;
long vremenskiIntervalRadaPumpeDruga;


//////////////////////////////setUp////////////////////////////////////////////////////////

void setup()
{
  pinMode(buzzer, OUTPUT);

  pinMode(relay_bijelo, OUTPUT);
  pinMode(relay_crno, OUTPUT);
  pinMode(relay_cola, OUTPUT);
  pinMode(relay_mineralna, OUTPUT);

  pinMode(gumbZaDolje, INPUT);
  pinMode(gumbZaGore, INPUT);
  pinMode(gumbZaOdabir, INPUT);
  pinMode( gumbZaPovratak, INPUT);

  digitalWrite(relay_bijelo, HIGH);
  digitalWrite(relay_crno, HIGH);
  digitalWrite(relay_cola, HIGH);
  digitalWrite(relay_mineralna, HIGH);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("   Dobrodosli!");
  lcd.setCursor(0, 1);
  lcd.print("****************");
  delay(1000);
  lcd.clear();
}
///////////////////////////////Main///////////////////////////////////////////////////////
void loop()
{
  if (trenutnaPozicijaMenija == 0)
    startPrikazMenija();

  ocitanjeVrijednostiGumbica();

  if (vrijednostGumbaZaDolje == HIGH) {
    pomakStreliceDolje();
    pomakMenijaDolje(ispisMenija, brojPonudeSadrzajaMenija, &trenutnaPozicijaMenija);
  }

  if ( vrijednostGumbaZaGore == HIGH) {
    pomakStreliceGore();
    pomakMenijaGore(ispisMenija, brojPonudeSadrzajaMenija, &trenutnaPozicijaMenija);

  }

  if ( vrijednostGumbaZaOdabir == HIGH) {
    animacijaKvaciceZaOdabir();
    delay(500);
    lcd.clear();
    podmeniSaPostocima();
  }

}
////////////////////////////////PodMeni//////////////////////////////////////////////////////

void podmeniSaPostocima() {
  pomakStreliceGore();
  pomakMenijaDolje(ispisPostotaka, brojPonudePostotaka, &trenutnaPozicijaPostotka);
  flagStrelica = 0;
  do {

    ocitanjeVrijednostiGumbica();

    if (vrijednostGumbaZaDolje == HIGH) {
      pomakStreliceDolje();
      pomakMenijaDolje(ispisPostotaka, brojPonudePostotaka, &trenutnaPozicijaPostotka);


      if (trenutnaPozicijaPostotka == 0)
        startPrikazPodmenija();
    }


    if ( vrijednostGumbaZaOdabir == HIGH) {
      animacijaKvaciceZaOdabir();
      delay(500);
      izracunRadaPumpi ();
      melodijaBuzzera();
      return;
    }


    if ( vrijednostGumbaZaGore == HIGH) {



      pomakStreliceGore();
      pomakMenijaGore(ispisPostotaka, brojPonudePostotaka, &trenutnaPozicijaPostotka);
    }
    if (vrijednostGumbaZaPovratak == HIGH) {
      pomakStreliceGore();
      pomakMenijaDolje(ispisMenija, brojPonudeSadrzajaMenija, &trenutnaPozicijaMenija);
      flagStrelica = 0;
      return;

    }



  } while (1);
}

void izracunRadaPumpi () {
  lcd.clear();
  vremenskiIntervalRadaPumpePrva = map(1, 0, 10 - (trenutnaPozicijaPostotka + 1), 0, (10 - (trenutnaPozicijaPostotka + 1)) * (10 - (trenutnaPozicijaPostotka + 1)) * 400);
  vremenskiIntervalRadaPumpeDruga = map(1, 0, vrijednostPostotaka[trenutnaPozicijaPostotka] / 10, 0, (trenutnaPozicijaPostotka + 1) * (trenutnaPozicijaPostotka + 1) * 400);
  Serial.println(vremenskiIntervalRadaPumpePrva);
  Serial.println(vremenskiIntervalRadaPumpeDruga);
  switch (trenutnaPozicijaMenija) {
    //////////////////////////////////////case 0/////////////////////////////////////////////////
    case 0:

      digitalWrite(relay_bijelo, LOW);
      digitalWrite(relay_mineralna, LOW);
      loadingAnimacijaOdabira ();
      trenutnoVrijeme = millis();


      do {

        if ((millis() - trenutnoVrijeme) > vremenskiIntervalRadaPumpePrva) {
          flagPumpa1 = 1;
          digitalWrite(relay_bijelo, HIGH);

        }
        if ((millis() - trenutnoVrijeme) > vremenskiIntervalRadaPumpeDruga) {
          digitalWrite(relay_mineralna, HIGH);
          flagPumpa2 = 1;
        }
      } while (flagPumpa1 == 0 || flagPumpa1 == 0);
      flagPumpa1 = 0;
      flagPumpa2 = 0;
      lcd.clear();
      flagStrelica = 0;
      return;
    //////////////////////////////////////case 1///////////////////////////////////////////////
    case 1:

      digitalWrite(relay_crno, LOW);
      digitalWrite(relay_cola, LOW);
      trenutnoVrijeme = millis();


      do {
        if ((millis() - trenutnoVrijeme) > vremenskiIntervalRadaPumpePrva) {
          flagPumpa1 = 1;
          digitalWrite(relay_crno, HIGH);
        }

        if ((millis() - trenutnoVrijeme) > vremenskiIntervalRadaPumpeDruga) {
          digitalWrite(relay_cola, HIGH);
          flagPumpa2 = 1;
        }
      } while (flagPumpa1 == 0 || flagPumpa2 == 0);
      flagPumpa1 = 0;
      flagPumpa2 = 0;
      lcd.clear();
      trenutnaPozicijaMenija = 0;
      flagStrelica = 0;
      return;
    //////////////////////////////////////case 2///////////////////////////////////////////////
    case 2:

      digitalWrite(relay_cola, LOW);
      loadingAnimacijaOdabira ();
      trenutnoVrijeme = millis();

      do {

        if ((millis() - trenutnoVrijeme) > vremenskiIntervalRadaPumpePrva) {
          flagPumpa1 = 1;
          digitalWrite(relay_cola, HIGH);

        }
      } while (!flagPumpa1);

      flagPumpa1 = 0;
      lcd.clear();
      trenutnaPozicijaMenija = 0;
      flagStrelica = 0;
      return;

    ///////////////////////////////////////case 3///////////////////////////////////////////
    case 3:

      digitalWrite(relay_mineralna, LOW);
      loadingAnimacijaOdabira ();
      trenutnoVrijeme = millis();

      do {
        if ((millis() - trenutnoVrijeme) > vremenskiIntervalRadaPumpePrva) {
          flagPumpa1 = 1;
          digitalWrite(relay_mineralna, HIGH);

        }
      } while (!flagPumpa1);
      flagPumpa1 = 0;
      lcd.clear();
      trenutnaPozicijaMenija = 0;
      flagStrelica = 0;
      return;
  }


}


void loadingAnimacijaOdabira () {
  int varijabla = 5, a;
  delay(50);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Radi se...");
  lcd.setCursor(varijabla + 7, 0);
  lcd.write(byte(1));
  lcd.createChar(1, boca);
  delay(300);
  lcd.clear();

  lcd.setCursor(0, 1);
  lcd.print("Radi se...");
  lcd.setCursor(varijabla + 7, 0);
  lcd.write(byte(2));
  lcd.createChar(2, bocapola);
  lcd.setCursor(varijabla + 8, 0);
  lcd.write(byte(3));
  lcd.createChar(3, bocavrat);
  lcd.setCursor(varijabla + 9, 1);
  delay(100);
  lcd.write(byte(4));
  lcd.createChar(4, casa);
  delay(500);

  for (varijabla = 0, a = 16; varijabla < 4; varijabla++, a = a / 2) {
    prazan[varijabla] = tociona[4 + varijabla];
    tociona[4 + varijabla] = a;
    delay(50);
    lcd.setCursor(7 + 7, 0);
    lcd.write(byte(5));
    lcd.createChar(5, tociona);
    delay(100);
  }
  for (varijabla = 0; varijabla < 4; varijabla++) {
    tociona[4 + varijabla] = prazan[varijabla];
  }

  for (varijabla = 0; varijabla < 3; varijabla++) {
    switch (varijabla) {
      case 0:
        prazan[varijabla] = casa[4 - varijabla];
        casa[4] = 14;
        break;
      case 1:
        prazan[varijabla] = casa[4 - varijabla];
        casa[3] = 31;
        break;
      case 2:
        prazan[varijabla] = casa[4 - varijabla];
        casa[2] = 31;
        break;
    };
    delay(100);
    lcd.setCursor(7 + 7, 1);
    lcd.write(byte(4));
    lcd.createChar(4, casa);
  }
  for (varijabla = 0; varijabla < 3; varijabla++) {
    casa[4 - varijabla] = prazan[varijabla];
  }
  delay(100);

}

void pomakMenijaDolje (char **pointerNaSadrzajMenija, int duljinaProslijedenogNiza, int *pointerNaPozicijuMenija) {

  lcd.setCursor(1, 0);
  lcd.print(pointerNaSadrzajMenija[*pointerNaPozicijuMenija]);
  lcd.setCursor(1, 1);
  lcd.print(pointerNaSadrzajMenija[*pointerNaPozicijuMenija + 1]);
  flagStrelica = 1;
  *pointerNaPozicijuMenija += 1;

  if (*pointerNaPozicijuMenija == duljinaProslijedenogNiza) {
    *pointerNaPozicijuMenija = 0;
    flagStrelica = 0;
    lcd.clear();
  }

  delay(200);
}

void pomakMenijaGore (char **pointerNaSadrzajMenija, int duljinaProslijedenogNiza, int *pointerNaPozicijuMenija) {
  if (*pointerNaPozicijuMenija == 0) {
    pomakStreliceGore();
    *pointerNaPozicijuMenija = duljinaProslijedenogNiza - 1;
    lcd.setCursor(1, 0);
    lcd.print(pointerNaSadrzajMenija[*pointerNaPozicijuMenija]);
  }
  else {
    *pointerNaPozicijuMenija -= 1;

    lcd.setCursor(1, 0);
    lcd.print(pointerNaSadrzajMenija[*pointerNaPozicijuMenija]);
    lcd.setCursor(1, 1);
    lcd.print(pointerNaSadrzajMenija[*pointerNaPozicijuMenija + 1]);
    flagStrelica = 0;


  }
  delay(200);

}

void pomakStreliceDolje () {

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.createChar(0, strelica);

}

void pomakStreliceGore() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.createChar(0, strelica);

}

void startPrikazMenija() {

  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.createChar(0, strelica);
  lcd.setCursor(1, 0);
  lcd.print(ispisMenija[trenutnaPozicijaMenija]);
  lcd.setCursor(1, 1);
  lcd.print(ispisMenija[trenutnaPozicijaMenija + 1]);

}

void startPrikazPodmenija() {

  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.createChar(0, strelica);
  lcd.setCursor(1, 0);
  lcd.print(ispisPostotaka[trenutnaPozicijaPostotka]);
  lcd.setCursor(1, 1);
  lcd.print(ispisPostotaka[trenutnaPozicijaPostotka + 1]);

}

void  animacijaKvaciceZaOdabir () {
  lcd.setCursor(15, flagStrelica);
  lcd.write(byte(1));
  lcd.createChar(1, kvacica);
}

void ocitanjeVrijednostiGumbica() {
  vrijednostGumbaZaPovratak = digitalRead(gumbZaPovratak);
  vrijednostGumbaZaDolje = digitalRead(gumbZaDolje);
  vrijednostGumbaZaGore = digitalRead(gumbZaGore);
  vrijednostGumbaZaOdabir = digitalRead(gumbZaOdabir);
}

void melodijaBuzzera () {
  for (int beep = 0; beep < MAX_BROJ_BEEP; beep++) {
    digitalWrite(5, 20);
    DELAY;
    digitalWrite(5, 0);
    DELAY;
  }
}

