#include "key.h"

void setup()
{
  key_init();
  Serial.begin(9600); // 初始化串口，波特率9600
}

void loop()
{
  // 因为是下拉输入，按下时检测到高电平 (1)
  if (KEY == 1)
  {
    delay(10); // 软件消抖：延时10ms跳过机械抖动期
    
    // 再次确认按键状态
    if (KEY == 1)
    {
      Serial.println("key press!");
      
      // 阻塞等待按键释放（变成低电平），防止单次按下被重复打印
      while(KEY == 1) {
        delay(10);
      }
    }
  }
}