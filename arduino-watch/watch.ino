#include <Arduino.h>

#include <U8glib.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <RtcDS3231.h>

#define countof(a) (sizeof(a) / sizeof(a[0]))
#define T_UNIT 'C'
RtcDS3231 Rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // Display which does not send AC
char dayName[8][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup(void)
{
        Serial.begin(9600);
        u8g.setColorIndex(1);
        Rtc.Begin();
        RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
        if (!Rtc.IsDateTimeValid())
        {
                Serial.println("RTC lost confidence in the DateTime!");
                Rtc.SetDateTime(compiled);
        }
        if (!Rtc.GetIsRunning())
        {
                Serial.println("RTC was not actively running, starting now");
                Rtc.SetIsRunning(true);
        }
        RtcDateTime now = Rtc.GetDateTime();
        if (now < compiled)
        {
                Serial.println("RTC is older than compile time!  (Updating DateTime)");
                Rtc.SetDateTime(compiled);
        }
        Rtc.Enable32kHzPin(false);
        Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

}
void loop(void)
{
        u8g.firstPage();
        do {
                RtcDateTime dt = Rtc.GetDateTime();
                RtcTemperature temp = Rtc.GetTemperature();
                char timeNow[6];
                snprintf_P(timeNow, countof(timeNow),PSTR("%02u:%02u"), dt.Hour(), dt.Minute());
                u8g.setFont(u8g_font_fub30r);
                u8g.drawStr( 10, 57, timeNow); u8g.drawRFrame(0,18, 128, 46, 4); u8g.setFont(u8g_font_8x13B); char tempNow[8];
                memset(tempNow, 0, sizeof(tempNow));
                dtostrf((T_UNIT == 'C') ? (temp.AsFloat()) : (temp.AsFloat()*1.8+32),4,1,tempNow);
                for (int n =0; n<10; n++)
                {
                        if(tempNow[n] == '\0')
                        {
                                tempNow[n] = char(176);
                                tempNow[n+1] = (T_UNIT == 'C') ? ('C') : ('F');
                                break;
                        }
                }
                u8g.drawStr( 5, 15, tempNow);
                u8g.drawStr(65, 15, dayName[int(dt.DayOfWeek())]);
                u8g.drawRFrame(105, 3, 20,12, 0);
                u8g.drawBox(125, 6, 2,6);
                int z = 15;
                for (int n = 0; n<=z; n++)
                {
                        u8g.drawBox(107+n, 5, 1,8);
                }
        } while(u8g.nextPage());

}
