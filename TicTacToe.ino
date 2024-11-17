#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int buttonPin1 = 2;
const int buttonPin2 = 3;
double variable = 0;
volatile bool button1Pressed = false;
volatile bool button2Pressed = false;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(buttonPin1), button1PressedISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), button2PressedISR, FALLING);
}

void button1PressedISR() {
  button1Pressed = true;
}

void button2PressedISR() {
  button2Pressed = true;
}

void kiJelez(char jatektTer[8][8], String kurzorPoz, char jatekos)
{
    for (int sor = 0; sor < 8; sor++)
    {
        for (int oszlop = 0; oszlop < 8; oszlop++)
        {
            display.setCursor(oszlop * 8, sor * 8);
            display.println(jatektTer[sor][oszlop]);
        }

        display.setCursor(9 * 8, 2 * 8);
        display.print("Kurzor:" + kurzorPoz);

        display.setCursor(9 * 8, 4 * 8);
        display.print("Jatekos:");
        display.setCursor(13 * 8 , 6 * 8);
        display.print(jatekos);
    }
}

bool sorralXnyert(char tomb[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (tomb[i][j]  == 'X' &&
                tomb[i][j+1]== 'X' &&
                tomb[i][j+2]== 'X' &&
                tomb[i][j+3]== 'X' &&
                tomb[i][j+4]== 'X')
            {
                return true;
            }
        }
    }
    return false;
}

bool oszloppalXnyert(char tomb[8][8])
{
  for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (tomb[j][i]      == 'X' &&
                tomb[j + 1][i]  == 'X' &&
                tomb[j + 2][i]  == 'X' &&
                tomb[j + 3][i]  == 'X' &&
                tomb[j + 4][i]  == 'X')
            {
            return true;
            }
        }
    }
    return false;
}

/*bool atlovalXnyertBJ(char tomb[8][8]) //valamiert mindig truet ad vissza
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            bool megvan = true;
            for (int k = 0; k < 4; k++)
            {
                if (i + k > 7 || j + k > 7) //Ha kilog az index, kilepunk, false-t adunk vissza
                {
                    megvan = false;
                    break;
                }
                else if(tomb[i + k][j + k] != 'X')  //Ha talalunk nem X et az atloban, szinten
                {
                    megvan = false;
                    break;
                }
            }
            if (megvan)
            {
                return true;
            }
        }
    }
    return false;
}*/

bool sorralOnyert(char tomb[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (tomb[i][j]  == 'O' &&
                tomb[i][j+1]== 'O' &&
                tomb[i][j+2]== 'O' &&
                tomb[i][j+3]== 'O' &&
                tomb[i][j+4]== 'O')
            {
                return true;
            }
        }
    }
    return false;
}

bool oszloppalOnyert(char tomb[8][8])
{
  for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (tomb[j][i]      == 'O' &&
                tomb[j + 1][i]  == 'O' &&
                tomb[j + 2][i]  == 'O' &&
                tomb[j + 3][i]  == 'O' &&
                tomb[j + 4][i]  == 'O')
            {
            return true;
            }
        }
    }
    return false;
}

void loop()
{
    display.clearDisplay();

    char jatekTer[8][8] =
    {
        {'-', '|', '-', '|', '-', '|', '-', '|'},
        {'|', '-', '|', '-', '|', '-', '|', '-'},
        {'-', '|', '-', '|', '-', '|', '-', '|'},
        {'|', '-', '|', '-', '|', '-', '|', '-'},
        {'-', '|', '-', '|', '-', '|', '-', '|'},
        {'|', '-', '|', '-', '|', '-', '|', '-'},
        {'-', '|', '-', '|', '-', '|', '-', '|'},
        {'|', '-', '|', '-', '|', '-', '|', '-'}
    };

    char kurzorBetuTomb[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'} ;
    char kurzorSzamTomb[8] = {'1', '2', '3', '4', '5', '6', '7', '8'} ;

    int betu = 0;
    int szam = 0;
    String kurzor = String(kurzorBetuTomb[0]) + String(kurzorSzamTomb[0]);

    char currentPlayer = 'X';

    bool neDuplaA = true;
    bool neDuplaB = true;

    bool vanNyertes = false;

    while(!vanNyertes)        //Ameddig nincs 5 egyforma a jatekteren egymas mellett
    {
        while (!button2Pressed) //Amig a jatekos ki nem valasztotta a mezot.
        {
            if (button1Pressed && neDuplaA)
            {
                noInterrupts();
                
                if (szam == 7 && betu == 7) //Utolso helyrol elsore ugrik
                {
                    szam = 0;
                    betu = 0;
                }
                else if(szam < 7)           //Soron kovetkezo elemre lep
                {
                    szam += 1;
                }
                else                        //Sor vegerol kovetkezo sor elejere ugrik
                {
                    betu += 1;
                    szam = 0;
                }
                kurzor = String(kurzorBetuTomb[betu]) + String(kurzorSzamTomb[szam]);

                button1Pressed = false;
                neDuplaA = false;
                
                interrupts();
            }
            else if (button1Pressed && !neDuplaA)
            {
                neDuplaA = true;
            }

            display.clearDisplay();
            kiJelez(jatekTer, kurzor, currentPlayer);
            display.display();
        }
        button2Pressed = false;

        jatekTer[betu][szam] = currentPlayer;

        if(currentPlayer == 'X' && neDuplaB)
        {
            currentPlayer = 'O';
            neDuplaB = false;
        }
        else if(currentPlayer == 'O' && neDuplaB)
        {
            currentPlayer = 'X';
            neDuplaB = false;
        }
        else if(!neDuplaB)
        {
            neDuplaB = true;
        }

        vanNyertes =    sorralXnyert(jatekTer) || oszloppalXnyert(jatekTer) ||
                        sorralOnyert(jatekTer) || oszloppalOnyert(jatekTer);
    }

    display.clearDisplay();

    /*for(int i = 0; i < 8; i++) //Ennek villogtatnia kellene a kijelzot
    {
        display.clearDisplay();
        delay(1000);
        kiJelez(jatekTer, kurzor, currentPlayer);
        display.display();
        delay(1000);
    }*/

    //display.clearDisplay();

    display.setCursor(5, 30);
    if(sorralXnyert(jatekTer))
    {
        display.print("X (Sorral) Nyert!");
    }
    else if(oszloppalXnyert(jatekTer))
    {
        display.print("X (Oszloppal) Nyert!");
    }
    else if(sorralOnyert(jatekTer))
    {
        display.print("O (Sorral) Nyert!");
    }
    else if(oszloppalOnyert(jatekTer))
    {
        display.print("O (Oszloppal) Nyert!");
    }
    else
    {
        display.print("? ? ?");
    }
    display.display();

    //currentPlayer = '-';  //bugfix akart lenni

    delay(5000);

    for(int visszaSzamlalo = 5; visszaSzamlalo > 0; visszaSzamlalo--)
    {
        display.clearDisplay();
        display.setCursor(15, 30);
        display.print("Uj jatek.." + String(visszaSzamlalo));
        display.display();
        delay(1000);
    }
  
}

//Felhasznalt eszkozok es forrasok:
//  Maverick Nano-Pong projectje https://wokwi.com/projects/348849468083274322 (a bekoteshez)
//  ChatGPT (a kijelzo es gombkezeleshez)