//-----------------------------------------------------------------------------------//
#include <avr/pgmspace.h>
#include <Usb.h>
#include <usbhub.h>
#include <avr/pgmspace.h>
#include <hidboot.h>
#include <Keyboard.h>
#include <Mouse.h>

//----------------------------------------//
// Usb / HID mouse
USB Usb;
HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse(&Usb);

//-----------------------------------------------------------------------------------//
struct {
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t wheel;
} mouseReport;
//----------------------------------------//
const byte buffSize = 32;
char inputSeveral[buffSize];

byte maxChars;

int inputInt = 0;
float inputFloat = 0.0;
char inputCsvString[12];
int x;
int y;
int shoot;
//-----------------------------------------------------------------------------------//
void MoveMouseToXY(long x, long y) {
  long max = max(abs(x), abs(y));
  int count = (int) (max / 127);
  signed char stepX = x / (count + 1);
  signed char stepY = y / (count + 1);
  for (int i = 0; i < count; i++) {
    Mouse.begin();
    Mouse.move(stepX, stepY);
    Mouse.end();
  }
  signed char resX = x - (stepX * count);
  signed char resY = y - (stepY * count);
  if (resX != 0 || resY != 0) {
    Mouse.begin();
    Mouse.move(resX, resY);
    Mouse.end();
  }
}
//-----------------------------------------------------------------------------------//
void showResults() {
  if (x != 0 || y != 0) {
    // Aimbot
    MoveMouseToXY(x, y);
  }

  // Trigger
  if (shoot == 1) {
    Mouse.begin();
    Mouse.press();
    Mouse.release();
    Mouse.end();
  }
}
//-----------------------------------------------------------------------------------//
void readCSV() {
  inputSeveral[0] = 0;
  maxChars = buffSize - 1;
  byte charCount = 0;
  byte ndx = 0;

  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      if (ndx > maxChars - 1) {
        ndx = maxChars;
      }
      inputSeveral[ndx] = Serial.read();
      ndx ++;
      charCount ++;
    }
    if (ndx > maxChars) {
      ndx = maxChars;
    }
    inputSeveral[ndx] = 0;
  }

  char * partOfString;

  partOfString = strtok(inputSeveral, ",");
  x = atoi(partOfString);

  partOfString = strtok(NULL, ",");
  y = atoi(partOfString);

  partOfString = strtok(NULL, ",");
  shoot = atoi(partOfString);
}
//-----------------------------------------------------------------------------------//


class MouseRptParser : public MouseReportParser {
protected:
    virtual void OnMouseMove        (MOUSEINFO* mi);
    virtual void OnLeftButtonUp     (MOUSEINFO* mi);
    virtual void OnLeftButtonDown   (MOUSEINFO* mi);
    virtual void OnRightButtonUp    (MOUSEINFO* mi);
    virtual void OnRightButtonDown  (MOUSEINFO* mi);
    virtual void OnMiddleButtonUp   (MOUSEINFO* mi);
    virtual void OnMiddleButtonDown (MOUSEINFO* mi);
    virtual void OnX1ButtonUp       (MOUSEINFO *mi);
    virtual void OnX1ButtonDown     (MOUSEINFO *mi);
    virtual void OnX2ButtonUp       (MOUSEINFO *mi);
    virtual void OnX2ButtonDown     (MOUSEINFO *mi);
    virtual void OnWheelMove        (MOUSEINFO *mi);
};
void MouseRptParser::OnMouseMove(MOUSEINFO* mi)        {
  MoveMouseToXY(mi->dX, mi->dY);
};
void MouseRptParser::OnLeftButtonUp(MOUSEINFO* mi)     {
  Mouse.begin();
  Mouse.release(MOUSE_LEFT);
  Mouse.end();
};
void MouseRptParser::OnLeftButtonDown(MOUSEINFO* mi)   {
  Mouse.begin();
  Mouse.press(MOUSE_LEFT);
  Mouse.end();
};
void MouseRptParser::OnRightButtonUp(MOUSEINFO* mi)    {
  Mouse.begin();
  Mouse.release(MOUSE_RIGHT);
  Mouse.end();
};
void MouseRptParser::OnRightButtonDown(MOUSEINFO* mi)  {
  Mouse.begin();
  Mouse.press(MOUSE_RIGHT);
  Mouse.end();
};
void MouseRptParser::OnMiddleButtonUp(MOUSEINFO* mi)   {
  Mouse.begin();
  Mouse.release(MOUSE_MIDDLE);
  Mouse.end();
};
void MouseRptParser::OnMiddleButtonDown(MOUSEINFO* mi) {
  Mouse.begin();
  Mouse.press(MOUSE_MIDDLE);
  Mouse.end();
};
void MouseRptParser::OnX1ButtonUp(MOUSEINFO *mi)       {
  Mouse.begin();
  Mouse.release(MOUSE_BACK);
  Mouse.end();
};
void MouseRptParser::OnX1ButtonDown(MOUSEINFO *mi)     {
  Mouse.begin();
  Mouse.press(MOUSE_BACK);
  Mouse.end();
};
void MouseRptParser::OnX2ButtonUp(MOUSEINFO *mi)       {
  Mouse.begin();
  Mouse.release(MOUSE_FORWARD);
  Mouse.end();
};
void MouseRptParser::OnX2ButtonDown(MOUSEINFO *mi)     {
  Mouse.begin();
  Mouse.press(MOUSE_FORWARD);
  Mouse.end();
};
void MouseRptParser::OnWheelMove(MOUSEINFO *mi)        {
  Mouse.begin();
  Mouse.move(0, 0, mi->dZ);
  Mouse.end();
};
MouseRptParser Prs;

void setup() {
    Serial.begin(9600);
    
    // Init USB/Mouse
    if (Usb.Init() == -1) Serial.println("HOST SHIELD NÃO ENCONTRADO.");
    HidMouse.SetReportParser(0,(HIDReportParser*)&Prs);
}



//-----------------------------------------------------------------------------------//
void loop() {
    Usb.Task();
    readCSV();
    showResults();
}
