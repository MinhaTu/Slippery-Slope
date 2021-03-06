
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "tim.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "PS2Keyboard.h"
#include "brailleMatrix.h"
#include "DCMotors.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define MAX_CARACTERES 30
#define MAX_LINHAS 27
#define POS_INI 28000
#define POS_FIM 4000

char pressedEnter = 1;
char isEnd = 0;

Keyboard_TypeDef keyboard;

MotorControl_t motorX;
MotorControl_t motorY;

MotorControl_Simple_t motorZ;

unsigned char buffer_char[MAX_CARACTERES];
unsigned char buffer_braille[4];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void interruption();
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void interruption(){
	ps2interrupt(&keyboard);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

	motorBegin(&motorX, ENCODER_1_CE_GPIO_Port, ENCODER_1_CE_Pin, MOTOR_X_A_GPIO_Port, MOTOR_X_A_Pin, MOTOR_X_B_GPIO_Port, MOTOR_X_B_Pin);
	motorBegin(&motorY, ENCODER_1_CE_GPIO_Port, ENCODER_1_CE_Pin, MOTOR_Y_A_GPIO_Port, MOTOR_Y_A_Pin, MOTOR_Y_B_GPIO_Port, MOTOR_Y_B_Pin);

	motorSimpleBegin(&motorZ, Motor_Z_A_GPIO_Port, Motor_Z_A_Pin, Motor_Z_B_GPIO_Port, Motor_Z_B_Pin);

	keyboardBegin(&keyboard, PS2_DATA_PORT, PS2_DATA_PIN, PS2_IQR_PORT, PS2_IQR_PIN);
//	memset(buffer_char, 'o', sizeof(buffer_char));
//	buffer_char[MAX_CARACTERES - 1] = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{

		// ATENÇÂO
		// 1. Verificar se as conexões estão certas
		// 2. Vericiar se o encoder tá ligando na greenpill
		// 3. Entender para que lado rodar o motor para incrementar o contador no timer
		// 4. Entender para que lado rodar o motor para decrementar o contador no timer
		// 5. Se voltar a funcionar, descobrir DELTA_CHAR_V
		// 6. Separar DELTA_COL_LIN em DELTA_COL e DELTA_LIN, DELTA_COL é o atual DELTA_COL_LIN
		// 7. Descobrir DELTA_LIN
		// 8. Se quiser que o motor desça e ele ta subindo com -> updateAxis(&motorY, motorY.setPoint - DELTA_COL_LIN); Altera o sinal e vice-versa
		// 9. Descobrir quantos caracteres cabem na linha, determinando o numero máximo de linhas.
		// 10. Testar se tudo funciona junto
		// 11. FIM
		// Pode se basear a quantidade de fitas necessárias para encontrar DELTA_CHAR_V, DELTA_LIN, incrementando um valor fixo até chegar no fim da folha
		// Verificar o tanto em centímetros percorridos e fazer regra de três.
		// No eixo x, temos 25,5 cm para usar para imprimir. Fiz incrementos de 100 em 100 e obtive 30 incremento de 100, então
		//  25,5 cm ------ 30 * 100 fitas
		//    x cm  ------    x fitas necessárias, onde x é a distância que se deseja obter
		// Depois são ajustes para melhorar os espaçamentos
		// Verificar cuidadosamente se no eixo y não ta acontecendo overflow, o que eu acho que vai acontecer.

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		/* Faz a leitura do teclado e envia ao buffer_char */
		while(1){
			if(keyboardAvailable(&keyboard)){
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
				HAL_Delay(50);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
				uint8_t c = keyboardRead(&keyboard);
				if(c == PS2_ENTER){
					pressedEnter = 1;
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
					break;
				}else if(c == PS2_DOWNARROW){
					updateAxis_Simple(&motorY, 50, DOWN);
				}else if(c == PS2_UPARROW){
					updateAxis_Simple(&motorY, 50, UP);
				}else if(c == PS2_BACKSPACE){
					clearBuffer(buffer_char);
				}else{



					feedBuffer(buffer_char, MAX_CARACTERES, c);
				}
				keyboardClear(&keyboard);
			}
		}
		/* Programa leitura do teclado */
		if(pressedEnter){
			//reverse(buffer_char);
			uint32_t length = strlen((const char*)buffer_char);
			for(int j=0;j<3;j++){
				//Imprime as linhas em braille
				if(j==1){
					//Linhas das matrizes
					for(int8_t i=length-1; i>=0; --i){

						// Recebe os pontos da linha para o caractere atual
						fillLineWithBraille(buffer_braille, j,buffer_char[i]);

						// Percorre os 4 bits, no máximo, para cada caractere
						for(int8_t x = 0; x < 2; ++x){

							// Passa para a próxima iteração se não tem ponto para furar
							if(buffer_braille[x] == '1'){
								pierce(&motorZ, PIERCE_TIME);
							}

							if(x != 1){
								updateAxis(&motorX, motorX.setPoint - DELTA_COL_LIN);
							}

							// Incrementa posição do eixo x, espaçamento entre colunas

						}

						// Incrementa posição do eixo x, espaçamento entre char na horizontal
						if(i != 0){
							updateAxis(&motorX, motorX.setPoint - DELTA_CHAR_H);
						}


					}

					// Decrementa posição do eixo y, espaçamento entre linhas
					//updateAxis(&motorY, motorY.setPoint - DELTA_COL_LIN);
					updateAxis_Simple(&motorY, NEXT_LINE, UP);
				}else{

					//Linhas das matrizes
					for(int8_t i=0; i<length; i++){

						//Recebe os pontos da linha para o caractere atual
						fillLineWithBraille(buffer_braille, j,buffer_char[i]);

						//Percorre os 4 bits, no máximo, para cada caractere
						for(int8_t x = 1; x >= 0; --x){

							if(buffer_braille[x] == '1'){
								pierce(&motorZ, PIERCE_TIME);
							}

							// Incrementa posição do eixo x, espaçamento entre colunas
							if(x != 0){
								updateAxis(&motorX, motorX.setPoint + DELTA_COL_LIN);
							}


						}

						// Incrementa posição do eixo x, espaçamento entre char na horizontal
						if(i != (length -1)){
							updateAxis(&motorX, motorX.setPoint + DELTA_CHAR_H);
						}


					}

					// Incrementa posição do eixo y, espaçamento entre linhas
					updateAxis_Simple(&motorY, NEXT_LINE, UP);
				}
			}

			// Incrementa posição do eixo y, espaçamento entre char na vertical
			//updateAxis(&motorY, motorY.setPoint + DELTA_CHAR_V - DELTA_COL_LIN);
			updateAxis_Simple(&motorY, NEXT_CHAR, UP);
			// Seta posição eixo x para inicial
			updateAxis(&motorX, POS_INI);

			pressedEnter = 0;
		}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		keyboardClear(&keyboard);
		clearBuffer(buffer_char);

	}
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
