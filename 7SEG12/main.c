#include <msp430.h>
#include "main.h"
#include "display.h"

void UARTIni(void);
void TimerIni(void);
void LEDIni(void);
void OpenShow(void);
void BlinkShow(void);
void LEDShow(void);
void ButtomIni(void);
void Buttom1Pros(void);
void Buttom2Pros(void);
void Buttom3Pros(void);
void Buttom4Pros(void);
void ClearData(void);
void SwitchINOUT(void);
void SendData(void);
void flash_write(unsigned char index);
char flash_read(char *ptr);
void Data_proc(void);
void copy_AtoC(void);
void copy_BtoD(void);
void Empty_Data(void);

#define flash_ptr 0x1800

unsigned char DATA[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0};
unsigned char UART_DATA[15] = {0x00};
unsigned char sendd[15] = {0x00};
unsigned char contral_step = 0;
unsigned char systick = 0, keytick = 0;
unsigned char Uindex = 0, uart_step = 1, checksum = 0x00;
unsigned char mStep = 1;
unsigned int  tick100ms = 0;
unsigned char blinkFlag = 0;
unsigned char blinkOn = 0;
unsigned char blinkOff = 0;
unsigned char KeyLock = 0;
unsigned char KeyLock4 = 0;
unsigned char getData = 0;
char Dproc_step = 1;
unsigned char BF = 0;

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;
  
  TimerIni();
  LEDIni();
  ButtomIni();
  UARTIni();
  _EINT();

  while(1)
  {
    switch(mStep)
    {
      case 1:
        OpenShow();
        Buttom4Pros();
        SwitchINOUT();
        break;
        
      case 2:
        BlinkShow();
        Buttom1Pros();
        Buttom2Pros();
        Buttom3Pros();
        Buttom4Pros();
        SwitchINOUT();
        SendData();
        break;
        
      case 3:
        LEDShow();
        Buttom4Pros();
        SwitchINOUT();
        SendData();
        break;
    }
    
    if(getData == 1)
    {
      Data_proc();
    }
  }
}

void UARTIni(void)
{
  P3SEL = 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
  UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 9;                              // 1MHz 115200 (see User's Guide)
  UCA0BR1 = 0;                              // 1MHz 115200
  UCA0MCTL |= UCBRS_1 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

void TimerIni(void)
{
  TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
  TA1CCR0 = 1000;
  TA1CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, contmode, clear TAR
}

void LEDIni(void)
{
  P2SEL &= ~(BIT0 | BIT1 | BIT2 | BIT6 | BIT7);
  P4SEL &= ~(BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  P8SEL &= ~(BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  P9SEL &= ~(BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  
  P2DIR &= ~(BIT6 | BIT7);
  P2REN |= (BIT6 | BIT7);
  P2DIR |= (BIT0 | BIT1 | BIT2);
  P4DIR |= (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0); //scan line
  P8DIR |= (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0); //digit
  P9DIR |= (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0); //ten digit

  P2OUT |= (BIT0 | BIT1 | BIT2 | BIT6 | BIT7);
  P4OUT &= ~(BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  P8OUT = (BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  P9OUT = (BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  P8OUT &= ~BIT7;
  P9OUT &= ~BIT7;
}

void OpenShow(void)
{
  P4OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
  P8OUT = 0xFF;                      //show digit
  P9OUT = 0xFF;
  switch(contral_step)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      break;
      
    case 4:
      P4OUT = BIT4;
      P9OUT = number[10];
      break;
      
    case 5:
      P4OUT = BIT5;
      P8OUT = number[0];                      //show digit
      P9OUT = number[0];                  //show ten digit
      break;
      
    default:
      break;
  }
  contral_step ++;
  contral_step %= 6;
}

void BlinkShow(void)
{
  P4OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4);
  P8OUT = 0xFF;                      //show digit
  P9OUT = 0xFF;
  switch(contral_step)
  {
    case 0:
      P4OUT = BIT0;
      P8OUT = number[DATA[0]];                      //show digit
      P9OUT = number[DATA[1]];                  //show ten digit
      break;
      
    case 1:
      P4OUT = BIT1;
      P8OUT = number[DATA[2]];                      //show digit
      P9OUT = number[DATA[3]];                  //show ten digit
      break;
      
    case 2:
      P4OUT = BIT2;
      P8OUT = number[DATA[4]];                      //show digit
      P9OUT = number[DATA[5]];                  //show ten digit
      break;
      
    case 3:
      P4OUT = BIT3;
      P8OUT = number[DATA[6]];                      //show digit
      P9OUT = number[DATA[7]];                  //show ten digit
      break;
      
    case 4:
      P4OUT = BIT4;
      P8OUT = number[DATA[8]];                      //show digit
      P9OUT = number[DATA[9]];                  //show ten digit
      break;
      
    case 5:
      P4OUT = BIT5;
      if(BF == 1)
      {
        if(blinkFlag)
        {
          P8OUT = number[DATA[10]];                      //show digit
          P9OUT = number[DATA[11]];                  //show ten digit
        }
    
        else
        {
          P8OUT = 0xFF;                      //show digit
          P9OUT = 0xFF;                  //show ten digit
        }
      }
      else
      {
        P8OUT = number[DATA[10]];                      //show digit
        P9OUT = number[DATA[11]];                  //show ten digit
      }
      break;
      
    default:
      break;
  }
  contral_step ++;
  contral_step %= 6;     
}

void LEDShow(void)
{
  P4OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
  P8OUT = 0xFF;                      //show digit
  P9OUT = 0xFF;
  switch(contral_step)
  {
    case 0:
      P4OUT = BIT0;
      break;
      
    case 1:
      P4OUT = BIT1;
      break;
      
    case 2:
      P4OUT = BIT2;
      break;
      
    case 3:
      P4OUT = BIT3;
      break;
      
    case 4:
      P4OUT = BIT4;
      break;
      
    case 5:
      P4OUT = BIT5;
      break;
      
    default:
      break;
  }
  P8OUT = number[DATA[contral_step * 2]];                      //show digit
  P9OUT = number[DATA[contral_step * 2 + 1]];                  //show ten digit
  contral_step ++;
  contral_step %= 6;
}

void ButtomIni(void)
{
  P1SEL &= ~(BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  P6SEL &= ~(BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  P10SEL &= ~(BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  P1DIR &= ~(BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);       //key1~8 
  P6DIR &= ~(BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);       //key9~16
  P10DIR &= ~(BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);      //key17~24 
  P1REN |= (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  P6REN |= (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  P10REN |= (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
}

void Buttom1Pros(void)
{
  unsigned char key0to3;
  static unsigned char lastkey1 = 0xFF;
  static unsigned char key_step1 = 1;
  static unsigned char index1 = 0;
  key0to3 = P10IN;
  switch(key_step1)
  {
    case 1:
      if((key0to3 == 0xFE) || (key0to3 == 0xFB) || (key0to3 == 0xEF) || (key0to3 == 0xBF)
         ||(key0to3 == 0xFD) || (key0to3 == 0xF7) || (key0to3 == 0xDF) || (key0to3 == 0x7F))
      {
        lastkey1 = key0to3;
        switch(key0to3)
        {
        case 0xFE:
        case 0xFD:
          index1 = 0;
          break;
          
        case 0xFB:
        case 0xF7:
          index1 = 1;
          break;
          
        case 0xEF:
        case 0xDF:
          index1 = 2;
          break;
          
        case 0xBF:
        case 0x7F:
          index1 = 3;
          break;
          
        default:
          break;
        }
        if(KeyLock == 0)
        {
          key_step1 = 2;
        }
      }
    break;
    
  case 2:
      if((key0to3 == 0xFF))
      {
        if((lastkey1 == 0xFE) || (lastkey1 == 0xFB) || (lastkey1 == 0xEF) || (lastkey1 == 0xBF))
        {
          DATA[index1] ++ ;
          DATA[index1] %= 10;
        }
        
        else if((lastkey1 == 0xFD) || (lastkey1 == 0xF7) || (lastkey1 == 0xDF) || (lastkey1 == 0x7F))
        {
          if(DATA[index1] == 0)
          {
            DATA[index1] = 9;
          }
            
          else
          {
            DATA[index1] --;
          }
        }
        lastkey1 = 0xFF;
        key_step1 = 1;
      }
    break;
  }
}
void Buttom2Pros(void)
{
  unsigned char key4to7;
  static unsigned char lastkey2 = 0xFF;
  static unsigned char key_step2 = 1;
  static unsigned char index2 = 4;
  key4to7 = P6IN;
  switch(key_step2)
  {
    case 1:
      if((key4to7 == 0xFE) || (key4to7 == 0xFB) || (key4to7 == 0xEF) || (key4to7 == 0xBF)
         ||(key4to7 == 0xFD) || (key4to7 == 0xF7) || (key4to7 == 0xDF) || (key4to7 == 0x7F))
      {
        lastkey2 = key4to7;
        switch(key4to7)
        {
        case 0xFE:
        case 0xFD:
          index2 = 4;
          break;
          
        case 0xFB:
        case 0xF7:
          index2 = 5;
          break;
          
        case 0xEF:
        case 0xDF:
          index2 = 6;
          break;
          
        case 0xBF:
        case 0x7F:
          index2 = 7;
          break;
          
        default:
          break;
        }
        if(KeyLock == 0)
        {
          key_step2 = 2;
        }
      }
    break;
    
  case 2:
      if((key4to7 == 0xFF))
      {
        if((lastkey2 == 0xFE) || (lastkey2 == 0xFB) || (lastkey2 == 0xEF) || (lastkey2 == 0xBF))
        {
          DATA[index2] ++ ;
          DATA[index2] %= 10;
        }
        
        else if((lastkey2 == 0xFD) || (lastkey2 == 0xF7) || (lastkey2 == 0xDF) || (lastkey2 == 0x7F))
        {
          if(DATA[index2] == 0)
          {
            DATA[index2] = 9;
          }
            
          else
          {
            DATA[index2] --;
          }
        }
        lastkey2 = 0xFF;
        key_step2 = 1;
      }
    break;
  }
}

void Buttom3Pros(void)
{
  unsigned char key8to9;
  static unsigned char lastkey3 = 0xFF;
  static unsigned char key_step3 = 1;
  static unsigned char index3 = 8;
  key8to9 = P1IN;
  switch(key_step3)
  {
    case 1:
      if((key8to9 == 0xFE) ||(key8to9 == 0xFD))
      {
        lastkey3 = key8to9;
        switch(key8to9)
        {
        case 0xFE:
        case 0xFD:
          index3 = 8;
          break;
          
        default:
          break;
        }
        if(KeyLock == 0)
        {
          key_step3 = 2;
        }
      }
    break;
    
  case 2:
      if((key8to9 == 0xFF))
      {
        if((lastkey3 == 0xFE))
        {
          DATA[index3] ++ ;
          DATA[index3] %= 10;
        }
        
        else if((lastkey3 == 0xFD))
        {
          if(DATA[index3] == 0)
          {
            DATA[index3] = 9;
          }
            
          else
          {
            DATA[index3] --;
          }
        }
        lastkey3 = 0xFF;
        key_step3 = 1;
      }
    break;
  }
}

void Buttom4Pros(void)
{
  unsigned char key10to11;
  static unsigned char lastkey4 = 0xFF;
  static unsigned char key_step4 = 1;
  static unsigned char index4 = 10;
  key10to11 = P1IN;
  switch(key_step4)
  {
    case 1:
      if((key10to11 == 0xEF) || (key10to11 == 0xDF) 
         ||(key10to11 == 0xBF) || (key10to11 == 0x7F))
      {
        lastkey4 = key10to11;
        switch(key10to11)
        {
          case 0xEF:
          case 0xDF:
            index4 = 10;
            break;
            
          case 0xBF:
          case 0x7F:
            index4 = 11;
            break;

          default:
            break;
        }
        if(KeyLock4 == 0)
        {
          key_step4 = 2;
        }
      }
    break;
    
  case 2:
      if((key10to11 == 0xFF))
      {
        if(lastkey4 == 0xEF)
        {
          DATA[index4] ++ ;
          DATA[index4] %= 10;
          
          if((DATA[11] == 0) && (DATA[10] == 0))
          {
            DATA[index4] = 1 ;
          }
        }
        
        else if(lastkey4 == 0xDF)
        {
            if(DATA[index4] == 0)
            {
              DATA[index4] = 9;
            }
            
          else
          {
            DATA[index4] --;
            if(((DATA[11] == 0) && (DATA[10] == 0)))
            {
              DATA[index4] = 1;
            }
          }
        }
        
        else if((lastkey4 == 0xBF) || (lastkey4 == 0x7F))
        {
          DATA[index4] ^= 1;
        }
        lastkey4 = 0xFF;
        
        if(mStep == 1)
        { 
          mStep = 2;
        }
        
        else if(mStep == 3)
        {
          Empty_Data();
        }
        if((BF == 0) && (mStep == 2))
        {
          ClearData();
          KeyLock = 0;
          BF = 1;
        }
        /*else
        {
          KeyLock = 1;
          BF = 0;
        }*/
        key_step4 = 1;
      }
    break;
  }
}

void ClearData(void)
{
  unsigned char i;
  for(i = 0; i <= 8; i ++)
  {
    DATA[i] = 0;
  }
}

void Empty_Data(void)
{
  unsigned char i;
  for(i = 0; i <= 8; i ++)
  {
    DATA[i] = 11;
  }
}
void SwitchINOUT(void)
{
  unsigned char i;
  unsigned char gg = P2IN & 0x80;
  switch(mStep)
  {
    case 1:
      if(gg)
      {
        mStep = 3;
        DATA[10] = 0;
        DATA[11] = 0;
        for(i = 0; i <= 8; i ++)
        {
          DATA[i] = 11;
        }
        P2OUT &= ~BIT0;
        P2OUT |= BIT1| BIT2;
      }
      else
      {
        P2OUT &= ~BIT1;
        P2OUT |= BIT0;
      }
      break;
      
    case 2:
      if(gg)
      {
        mStep = 3;
        DATA[10] = 0;
        DATA[11] = 0;
        for(i = 0; i <= 8; i ++)
        {
          DATA[i] = 11;
        }
        P2OUT &= ~BIT0;
        P2OUT |= BIT1 | BIT2;
      }
      else
      {
        P2OUT &= ~BIT1;
        P2OUT |= BIT0;
      }
      break;
      
    case 3:
      if(!gg)
      {
        mStep = 1;
        DATA[10] = 0;
        DATA[11] = 0;
        DATA[9] = 10;
        ClearData();
        P2OUT &= ~BIT1;
        P2OUT |= BIT0 | BIT2;
      }
      else
      {
        P2OUT &= ~BIT0;
        P2OUT |= BIT1;
      }
      break;
  }
}

void SendData(void)
{
  static unsigned char sendstep = 1;

  unsigned char i,temp = P2IN & 0x40;
  static unsigned char blinkcon = 0;
  switch(sendstep)
  {
    case 1:
      if(!temp)
      {
        sendstep = 2;
        keytick = 0;
      }
      break;
      
    case 2:
      if((keytick >= 10) && !temp)
      {
        sendstep = 3;
        KeyLock = 1;
        KeyLock4 = 1;
      }
      else if((keytick < 10) && temp)
      {
        sendstep = 1;
      }
      break;
      
    case 3:
      if(blinkcon < 14)
      {
        if(blinkFlag)
        {
          P2OUT &= ~BIT2;
          blinkOn ++;
          
        }
        
        else
        {
          P2OUT |= BIT2;
          blinkOff ++;
        }
        
        if((blinkOn > 0) && (blinkOff > 0))
        {
          blinkOn = 0;
          blinkOff = 0;
          blinkcon ++;
        }
      }
      
      else
      {
        P2OUT |= BIT2;
        
        blinkOn = 0;
        blinkOff = 0;
        blinkcon = 0;
        
        sendstep = 4;
      }
      break;
        
    case 4:
      if(temp)
      {
        if(mStep == 2)
        {
          sendd[0] = 0xEC;
          for(i = 0; i < 13; i ++)
          {
            sendd[i+1] = DATA[i];
          }
          sendd[13] = sendd[1];
        }
        
        else if(mStep == 3)
        {
          sendd[0] = 0xDC;
          sendd[10] = 0x0A;
          sendd[11] = DATA[10];
          sendd[12] = DATA[11];
          
          for(i = 0; i < 9; i ++)
          {
            sendd[i+1] = 0;
          }
          sendd[13] = sendd[1];
        }
        
        for(i = 2; i < 13; i ++)
        {
          sendd[13] ^= sendd[i];
        }
        sendd[14] = 0xAA;
        sendstep = 5;
      }
      break;
      
    case 5:
      for(i = 0; i < 15; i ++)
      {
        while (!(UCA0IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
          UCA0TXBUF = sendd[i];                  // TX -> RXed character
      }
      BF = 0;
      /*if(mStep != 3)
      {
          ClearData();
      }*/
      BF = 0;
      KeyLock4 = 0;
      KeyLock = 1;
      sendstep = 1;
      break;
     
    default:
      break;
  }
}

void flash_write(unsigned char index)
{
  unsigned int i;
  char *Flash_ptrA;                        // Flash pointer
  char *Flash_ptrC;
  char limit = index *9;
  Flash_ptrA = (char *) 0x1800;
  Flash_ptrC = (char *) 0x1900;
  
  __disable_interrupt();
  FCTL3 = FWKEY;
  FCTL1 = FWKEY + ERASE;                    // Set Erase bit                         // Clear Lock bit
  *Flash_ptrA = 0;       // Dummy write to erase Flash segment
  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
  
  for (i = 0; i < 180; i++)
  {
    if((i < limit) || (i > limit * 9))
    {
      Flash_ptrC =(char *) 0x1900 + i;
      Flash_ptrA =(char *) 0x1800 + i;
      *Flash_ptrA = *Flash_ptrC;          // copy value segment C to seg D
    }
  }
  
  Flash_ptrA = (char *) 0x1800 + (index * 9);
  for(i = 0;i < 9;i ++)
  {
    while(FCTL3 & BUSY);
    *Flash_ptrA = UART_DATA[i+1];                   // Write value to flash
    Flash_ptrA ++;
  }
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
  __enable_interrupt();
}

void copy_AtoC(void)
{
  unsigned int i;
  char *Flash_ptrA;                        // Flash pointer
  char *Flash_ptrC;
  Flash_ptrA = (char *) 0x1800;
  Flash_ptrC = (char *) 0x1900;
  
  __disable_interrupt();
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
  *Flash_ptrC = 0;          // Dummy write to erase Flash seg D
  FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation

  for (i = 0; i < 128; i++)
  {
    *Flash_ptrC++ = *Flash_ptrA++;          // copy value segment C to seg D
  }
  
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;
  __enable_interrupt();
}

void copy_BtoD(void)
{
  unsigned int i;
  char *Flash_ptrB;                        // Flash pointer
  char *Flash_ptrD;
  Flash_ptrB = (char *) 0x1880;
  Flash_ptrD = (char *) 0x1980;

  __disable_interrupt();
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
  *Flash_ptrD = 0;          // Dummy write to erase Flash seg D
  FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation

  for (i = 0; i < 52; i++)
  {
    *Flash_ptrD++ = *Flash_ptrB++;          // copy value segment C to seg D
  }
  
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;
  __enable_interrupt();
}

char flash_read(char *ptr)
{
  return *ptr;
}

void Data_proc(void)
{
  unsigned char tem = 0;
  switch(Dproc_step)
  {
    case 1:
      checksum = UART_DATA[1];
      for(unsigned char i = 2; i < 13; i ++)
        {
          checksum ^= UART_DATA[i];
        }
        
        if(checksum == UART_DATA[13])
        {            
          if((UART_DATA[0] == 0xEC))
          {
            contral_step = 0;
            Dproc_step = 2;
          }
          
          else if((UART_DATA[0] == 0xDC))
          {
            Dproc_step = 3;
          }
        }
      
        else
        {
          Dproc_step = 1;
          getData = 0;
        }
      break;
      
    case 2:
      copy_BtoD();
      Dproc_step = 5;
      break;
      
  case 3:
    sendd[0] = 0xEC;
    for(unsigned char i = 0; i < 9; i++)
    {
      tem = flash_read((char *)(((UART_DATA[12] *10 + UART_DATA[11])*9 +i)+ 0x1800));
      if(tem == 0xFF)
      {
        sendd[i+1] = 10;
      }
      else
      {
        sendd[i+1] = tem;
      }
    }
    sendd[10] = DATA[9];
    sendd[11] = DATA[10];
    sendd[12] = DATA[11];
    sendd[13] = sendd[1];
    for(unsigned char i=2;i<13;i++)
    {
      sendd[13] ^= sendd[i];
    }
    sendd[14] = 0xAA;
    for(unsigned char i = 0; i < 15; i ++)
      {
        while (!(UCA0IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
          UCA0TXBUF = sendd[i];                  // TX -> RXed character
      }
    Dproc_step = 1;
    getData = 0;
    break;
    
  case 5:
     copy_AtoC(); 
     Dproc_step = 6;
    break;
    
  case 6:
    flash_write(UART_DATA[12] *10 + UART_DATA[11]);
    Dproc_step = 7;
    break;
    
  case 7:
    for(unsigned char j = 0; j < 9; j ++)
    {
        DATA[j] = UART_DATA[j+1];
    } 
    Dproc_step = 1;
    getData = 0;
    break;
    
  }
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER_ISR(void)
{
  keytick ++;
  systick ++;
  if(systick >= 100)
  {
    tick100ms ++;
    systick = 0;
  }
    
  else if(tick100ms >= 2)
  {
    blinkFlag ^= 1;
    tick100ms = 0;
  }
}



#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
  switch(__even_in_range(UCA0IV,4))
  {
    case 0:break;                             // Vector 0 - no interrupt
    case 2:   
    switch(uart_step)
    {
      case 1:
        if(UCA0RXBUF == 0xEC)
        {
          UART_DATA[0] =  0xEC;
          Uindex = 1;
          uart_step = 2;
        }
        else if(UCA0RXBUF == 0xDC)
        {
          UART_DATA[0] =  0xDC;
          Uindex = 1;
          uart_step = 2;
        }
        break;
        
      case 2:
        if(UCA0RXBUF == 0xAA)
        {
          UART_DATA[Uindex] = 0xAA;
          Uindex = 1;
          uart_step = 1;
          getData = 1;
        }
        
        else
        {
          UART_DATA[Uindex] =  UCA0RXBUF;
          Uindex ++;
        }
        
        if(Uindex > 14)
        {
          Uindex = 0;
          uart_step = 1;
        }
        break;
    } 
    break;
  }
}
