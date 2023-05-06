#include <SD.h>
#include <SPI.h>
#include <Audio.h>
#include <string>
#include <cstring>
#include <Bounce2.h>
// #include <Wire.h>
#include <Encoder.h>
#include "play_sd_wav.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "displayFunctions.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);

#define MOSI_PIN 11
#define SCK_PIN 13
#define CS_PIN = BUILTIN_SDCARD

#define maxNumCharDisplay 50

/*

  define:
  - A char array to store the WHOLE file path
  - a char array to store a FILE
  - a char array to store a FOLDER

  procedure:
  - Concatenate what the select button deems to be a File or Folder
  - if it is a Folder, use the the list directory function to pass whatever folder has been selected.
  - it is a file, proceed to concatenate the file path, then pass back to the playWav function.
  - for both file or folder, use the SD.open(..concatenation of either folder or file with the root directory).

*/

Bounce returnButton = Bounce(35, 10);
Bounce selectButton = Bounce(36, 10);
Bounce scrollButton = Bounce(37, 10);

const int encoderPin1 = 5; // Encoder pin 1 - UP
const int encoderPin2 = 6; // Encoder pin 2 - DOWN

Encoder myEncoder(encoderPin1, encoderPin2); // Initialize encoder

int buttonState = 0;
int upButtonState = 0;
int downButtonState = 0;

const int PUSH_PAD_PINS[] = {28, 29, 30, 31};
const int NUM_PUSH_PADS = 4;

Bounce padPad[NUM_PUSH_PADS];

int fileCount = 0;

File currentDirectory;
char rootSampleDirectory[30];

char folderNameBuff[80];
char fileNameBuff[80];

char storedPath[80];

String presetStr[40];

char *folderList[80];
int folderCounter = 0;
int x = 0; //  no. of files within the main directory?
char *folder;

const int FOLDER_COUNT = sizeof folderList / sizeof folderList[0];
int folderIndex = 0;

int numTabs;

void printDirectory(File dir, int numTabs);
void openSelected();
void playSample();
void updateIO();

AudioPlaySdWav playWav;

void setup()
{

  displayInit();

  updateIO();

  display.setCursor(0, 0);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw white text
  display.println("SD CARD: ");

  pinMode(35, INPUT_PULLUP);
  pinMode(36, INPUT_PULLUP);
  pinMode(37, INPUT_PULLUP);

  Serial.begin(9600);

  SPI.setMOSI(MOSI_PIN);
  SPI.setSCK(SCK_PIN);

  if (!(SD.begin(BUILTIN_SDCARD)))
  {
    // while (1) {
    Serial.println("unable to access SD card.");
    //}
  }

  // sprintf(rootSampleDirectory, "/SdCardAudio/");  //  This won't work becuase when the loop comes back round it will always open at the root folder, not the selected folder/directory

  currentDirectory = SD.open("/");

  printDirectory(currentDirectory, 0);

  // display.display();
}

void printDirectory(File dir, int numTabs)
{

  // int index = 0;

  while (true)
  {

    File entry = dir.openNextFile();

    if (!entry)
    {
      // no more files
      break;
    }
    if (SD.exists("."))
    {
    }

    for (uint8_t i = 0; i < numTabs; i++)
    {
    }

    display.setTextColor(SSD1306_WHITE); // Draw white text

    char *inString = entry.name();
    String stri = String(inString);
    stri = stri.substring(0, stri.length());

    char *folder = entry.name();

    String toString = String(folder); //  Convert filename to String

    toString = toString.substring(0, stri.length()); //  substring to take length + able to ignore extension (if necessary)

    toString.toCharArray(inString, toString.length() + 1); //  Convert back to char array to be stored in char* folderList

    folderList[x] = strdup(inString); //  strdup = duplicate

    Serial.println(folderList[x]); //  This is printing all of the folders -> this is the array they're stored in

    display.println(folderList[x]); //  This is printing all of the folders -> this is the array they're stored in

    delay(500);

    folderCounter++;

    if (entry.isDirectory())
    {
      // entry.read(folderNameBuff, numTabs);  //  I think this reads file entry & numTabs contents into folderNameBuff
      // store contents into -> folderNameBuff -> I don't think it does.
    }
    else
    {
      // store contents into -> fileNameBuff
      Serial.println("");
      display.println("");
    }
    display.display();
    entry.close();
  }
}

void loop()
{

  selectButton.update();
  scrollButton.update();
  returnButton.update();

  /*
  int fileIndex = 0; // Selected file index
  int position = 0;  //  Encoder position
  int previousPosition = 0;

  position = myEncoder.read();

  // Check for changes in encoder position
  if (position != previousPosition)
  {
    // Determine direction of rotation
    if (position > previousPosition)
    {
      // Move down one file/folder
      fileIndex++;
      // Serial.print(">");
      delay(100);
    }
    else
    {
      // Move up one file/folder
      fileIndex--;
      // Serial.print("<");
    }

    // Wrap around if at end of list
    for (int n = 0; n < fileCount; n++)
    {

      // Wrap around if at end of list
      if (fileIndex >= fileCount)
      {
        fileIndex = 0;
      }
      else if (fileIndex < 0)
      {
        fileIndex = fileCount - 1;
      }
      fileCount = folderList[n];
    }

    for (int j = 0; j < fileIndex; j++)
    {
      fileCount = folderList[j];
    }
  }
  previousPosition = position;
  */

  // buttonState = digitalRead(encoderPin1);

  /*
  if (returnButton.fallingEdge())
  {
    folderIndex = (folderIndex + 1) % FOLDER_COUNT;
  }

  if (scrollButton.fallingEdge())
  {
    folderIndex = (folderIndex - 1) % FOLDER_COUNT;
  }
  */
  // Serial.print(folderList[folderIndex]);

  if (scrollButton.fallingEdge())
  {

    Serial.print(">");
    folderIndex = (folderIndex + 1) % FOLDER_COUNT;

    // folderIndex = folderList[0];
    // Serial.print(folderIndex);

    Serial.print(folderList[0]); //  when this is 0 - it starts on the last file? but 5 disconnects it?

    // Serial.print(folderList[folderIndex]); //  This resets the device? - maybe a glitch?
  }

  if (returnButton.fallingEdge())
  {
    Serial.print("return button");
    printDirectory(currentDirectory, 0);
  }

  if (selectButton.fallingEdge())
  {
    openSelected();
  }
}

void openSelected()
{

  // int trackFolderIndex = 0;

  // File openPathFile;

  // File folderSelect = openPathFile.openNextFile();

  // strcpy(folder, storedPath);

  // File entry = openPathFile.openNextFile();

  // File entry = SD.open(const char *filepath)

  display.clearDisplay();

  const char *folderPtr = folderList[x];

  File entry = SD.open(folderPtr);

  display.setCursor(0, 0);
  display.print(folderPtr);
  display.print(":");
  display.setCursor(0, 10);
  // File folderSelect = currentDirectory.openNextFile();

  // presetStr[trackFolderIndex] = folderSelect.name();
  // trackFolderIndex++;

  if (entry.isDirectory())
  {
    strcat(storedPath, "/Folder");
    strcpy(storedPath, folderNameBuff); // copy folderNameBuff contents into storedPath
    strcpy(storedPath, folderList[x]);

    // sprintf(storedPath, folderNameBuff);  //  folderNameBuff content stored as a string in storedPath buffer
    // Serial.println(storedPath);           //  should print e.g. /Presets/

    // storedPath = folderSelect;  //  this is to say: the selected folder which is a directory is put back into printDir

    printDirectory(entry, 0); //  loop back with whichever folder has been chosen, and list the contents
  }

  if (!entry.isDirectory())
  {
    strcat(storedPath, "/File"); //  only this prints
    strcat(storedPath, fileNameBuff);
    //  should strcpy(storedPath, folderList[x]); be here?

    // sprintf(storedPath, fileNameBuff);  //  This seems to hold "nothing" -> don't use
    // Serial.println(storedPath);

    playSample();
  }

  // File rootOpen = SD.open(storedPath);
}

void playSample()
{

  SD.open(storedPath, FILE_READ);

  Serial.println(storedPath);

  playWav.play(storedPath);
}

void assignToPad()
{

  Serial.print("Select a pad");

  int assignedPad = -1;

  if (assignedPad == -1) //  this is just for assigning that pad - not playing
  {
    for (int i = 0; i < NUM_PUSH_PADS; i++)
    {
      if (padPad[i].update() && padPad[i].fallingEdge())
      {
        assignedPad = -1;
        display.print("Assigned to pad: ");
        display.println(assignedPad + 1);
      }
    }
  }
  else
  {
    for (int i = 0; i < NUM_PUSH_PADS; i++)
    {
      if (padPad[i].update() && padPad[i].fallingEdge() && i == assignedPad)
      {
        playWav.play(storedPath);

        break;
      }
    }
  }
}

void updateIO()
{
  returnButton.update();
  selectButton.update();
  scrollButton.update();
}

void displayInit()
{
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);
}
