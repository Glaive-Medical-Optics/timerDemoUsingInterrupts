/* 
 *    This demo program shows how to set up an ISR (Interrupt Service Routine)
 *    that will get called periodically every n seconds (for a specified value of n)
 *    on an Arduino Giga R1 board.
 *    
 *    Author: Glaive Software Group
 *    Revised: 8/1/2024.
 *    
 *    The sketch uses an interrupt routine, so your loop() code can be busy doing other work.
 *    
 *    This program is meant for an Arduino Giga R1 board using the STM32H747XI microcontroller.
 *    The microcontroller runs with a system clock running at 480 MHz
 *    and HCLK running at 240 MHz.
 *    
 *    Once the ISR is established, the routine "doIt" will get called repeatedly.
 *    For this demo, it will get called every 10 seconds.
 *    You can change the interval by setting the variable "interval" to the 
 *    desired time in milliseconds.
 *    
 *    You must open the Serial Monitor.
 *    This demo prints "Timer triggered." on the serial monitor every 10 seconds.
 *    
 *    This program is Free Software and has ABSOLUTELY NO WARRANTY.
 */

// Define the timer handle
TIM_HandleTypeDef htim2;

volatile bool timerFlag = false;    // Flag to indicate timer interrupt
unsigned long interval = 10000;     // Interval in milliseconds (10 seconds by default)

/*
 *    This routine will get called every 10 seconds.
 *    Note that you cannot print on the Serial monitor from within an interrupt routine,
 *    so we just set the flag "timerFlag" to be true, instead, for this example.
 */

void doIt()
{
    timerFlag = true;   // Set the flag
}

void setup()
{
    Serial.begin(9600);
    while (!Serial) ; // Wait for serial monitor to open
    Serial.println("starting!");
  
    pinMode(LEDR, OUTPUT);
  
    // Initialize the timer
    MX_TIM2_Init(interval);
  
    // Start the timer interrupt
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
    {
        // Starting Error
        digitalWrite(LEDR, LOW);   // turn on the red LED if an error occurs.
    }
}

void loop()
{
    // Check the timer flag
    if (timerFlag)
    {
        Serial.println("Timer triggered!");
        timerFlag = false; // Reset the flag
    }
}

// Timer interrupt initialization function
void MX_TIM2_Init(unsigned long interval)
{
    __HAL_RCC_TIM2_CLK_ENABLE(); // Enable the timer clock
  
    // Assuming a 240 MHz HCLK frequency, calculate the prescaler and period
    uint32_t prescaler = 23999; // Prescaler value to get 10 kHz (240 MHz / (prescaler + 1))
    uint32_t period = (interval * 10) - 1; // Calculate period based on the interval
  
    htim2.Instance = TIM2;                                  // use Timer 2 in this example
    htim2.Init.Prescaler = prescaler;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;            // the counter counts up
    htim2.Init.Period = period;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;      // no division
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        // Initialization Error
        digitalWrite(LEDR, LOW);   // turn on the red LED if an error occurs.
        return;
    }
  
    // Enable the TIM2 global interrupt
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

// Callback function called when timer interrupt occurs
extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        doIt();     // call user's routine
    }
}

// Timer interrupt handler
extern "C" void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}
