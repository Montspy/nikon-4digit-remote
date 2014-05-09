#include "stm8s.h"
#include "led.h"

/**
  * @brief  Configure system clock 
  * @param  None
  * @retval None
  */
static void CLK_Config(void)
{
    CLK_DeInit();

    /* Configure the Fcpu to DIV1 */
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    
    /* Configure the HSI prescaler */
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);

    /* Output Fcpu on CLK_CCO pin */
    //CLK_CCOConfig(CLK_OUTPUT_CPU);
        
    /* Configure the system clock to use HSI clock source */
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
}

static void ITC_Config(void)
{
  //ITC_SetSoftwarePriority(ITC_IRQ_PORTD, ITC_PRIORITYLEVEL_3);       // I2C Pin interrupt
  //ITC_SetSoftwarePriority(ITC_IRQ_TIM2_OVF, ITC_PRIORITYLEVEL_3);    // I2C timeout
  ITC_SetSoftwarePriority(ITC_IRQ_TIM1_OVF, ITC_PRIORITYLEVEL_1);    // LED main interrupt
  ITC_SetSoftwarePriority(ITC_IRQ_TIM1_CAPCOM, ITC_PRIORITYLEVEL_1); // LED dimming interrupt
}

void main( void )
{
  CFG->GCR |= 0x01; /* disable SWIM to use PD1 as a standard I/O */ 
  CLK_Config();
  ITC_Config();
  enableInterrupts();
  led_init();
  
  while (1)
  {
    wfi();
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif