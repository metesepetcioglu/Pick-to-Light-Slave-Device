/* USER CODE BEGIN Header */
/** SLAVE 1 - I2C SLAVE ADRES: 0x50
 *METE SEPETCIOGLU - ABDULLAH CENGIZ
//Master stm32f103'ten gelen I2C verisini, slave stm32f103 ile okuyup 5 adet mux kontrol eden slave kodu.
//Bu Slave stm32 80 adet ledi kontrol edebilme kapasitesine sahiptir.
//Sahada Konrol edilen led sayısı: 56 adet
//1 -7 mux1
//8 -14 mux2
//15-21 mux3
//22-28 mux4
//29-35 mux5
 *
 * I2C ile okunan 10 elemanlı mesajın ilk 5 elemanı yanıp sönecek uyarı ledleri, son 5 elemanı sabit yanık duracak ledleri ifade eder.
 * Led söndürülmek isteniyorsa gelen mesaj 0 olmalıdır.

// her bir slave stm32f103 5 adet mux kontrol edilecek. her muxu 6 dijital cıkıs kontrol eder. toplamda 30 GPIO_Output aktif olacak.
// ornegin mux1-a0,a1,a2,a3,a4 pinlerine bagli ise
// a0 pini aktifse mux kapalı, pasifse mux calismaya aciktir.
// a1,a2,a3,a4 pinleri ise mux cıkıslarını kontrol eder.
// alinan mesaj {0,0,1,2,3,0,0,32,33,34} ise bu slave 1,2,3 bölümdeki ledleri yak sön, 32,33,34ü ise yanık hale getirecektir.
// önce mux1 ve mux5 enable pinleri resete getirilecek.
 *
 * MUX1_EN  = B12
 * MUX1_SIG = A9
 * MUX2_EN  = A10
 * MUX2_SIG = B4       KONTROL EDILDI
 * MUX3_EN  = A2
 * MUX3_SIG = C13
 * MUX4_EN  = B0
 * MUX4_SIG = A3
 * MUX5_EN  = B9
 * MUX5_SIG = B11
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//uint8_t bitRead(uint8_t value, uint8_t bitNumber)
//{
//	return (value >> bitNumber) & 0x01;
//}
//void Setmuxchannel_MUX1(uint8_t channel)
//{
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,  bitRead(channel, 0));
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, bitRead(channel, 1));
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, bitRead(channel, 2));
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, bitRead(channel, 3));
//	//MUX1 SIG -
//}
//void Setmuxchannel_MUX2(uint8_t channel)
//{
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,  bitRead(channel, 0));
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, bitRead(channel, 1));
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, bitRead(channel, 2));
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, bitRead(channel, 3));
//}
//void Setmuxchannel_MUX3(uint8_t channel)
//{
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1,  bitRead(channel, 0));
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,  bitRead(channel, 1));
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, bitRead(channel, 2));
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, bitRead(channel, 3));
//}
//void Setmuxchannel_MUX4(uint8_t channel)
//{
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, bitRead(channel, 0));
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, bitRead(channel, 1));
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, bitRead(channel, 2));
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, bitRead(channel, 3));
//}
//void Setmuxchannel_MUX5(uint8_t channel)
//{
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, bitRead(channel, 0));
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, bitRead(channel, 1));
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, bitRead(channel, 2));
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,bitRead(channel, 3));
//}


uint8_t value_01 = 0;
uint8_t value_02 = 0;
uint8_t value_03 = 0;
uint8_t value_04 = 0;

/*
 * Mux girişlerine her defasında tekrar dijital çıkış vermemek icin Setmuxchannel_Mux fonksiyona güncelleme yapildi.
 *
 * örnegin ilk verdigimiz cikis 0011 ise islemci c3-c2 pinlerine low, c1-c0 pinlerine high verilmekte
 * bir sonraki mesaj 0010 ise tekrardan c3-c2 pinlerine low ve c1 pinine high yazmak yerine
 * mesajin sadece degisen kismina dijital cikisi degistirmek yeterlidir.
 * yani ikinci mesaja göre mux'a 0010 vermek istiyorsak sadece c0 pinini low'a cekmemiz gerekiyor.
 * Bu sayede tekrar tekrar butun pinlere islem yaptirmaktan kacinmis olduk.
 */

void Setmuxchannel_MUX1(uint8_t newValue_01)
{
	// Önceki değer ile yeni değeri karşılaştır ve sadece farklı bitleri güncelle
	uint8_t diff = value_01 ^ newValue_01;

	// 1. biti güncelle
	if (diff & 0x01)
	{
		if (newValue_01 & 0x01)
		{
			// 1. biti 1 yap
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); // Örnek olarak GPIOA pin 0 kullanılıyor
		} else {
			// 1. biti 0 yap
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		}
	}

	// 2. biti güncelle
	if (diff & 0x02) {
		if (newValue_01 & 0x02) {
			// 2. biti 1 yap
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); // Örnek olarak GPIOA pin 1 kullanılıyor
		} else {
			// 2. biti 0 yap
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
		}
	}

	// 3. biti güncelle
	if (diff & 0x04) {
		if (newValue_01 & 0x04)
		{
			// 3. biti 1 yap
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET); // Örnek olarak GPIOA pin 2 kullanılıyor
		} else {
			// 3. biti 0 yap
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
		}
	}

	// 4. biti güncelle
	if (diff & 0x08) {
		if (newValue_01 & 0x08)
		{
			// 4. biti 1 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); // Örnek olarak GPIOA pin 3 kullanılıyor
		} else
		{
			// 4. biti 0 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		}
	}

	// Yeni değeri güncelle
	value_01 = newValue_01;
}

void Setmuxchannel_MUX2(uint8_t newValue_02)
{
	// Önceki değer ile yeni değeri karşılaştır ve sadece farklı bitleri güncelle
	uint8_t diff = value_02 ^ newValue_02;

	// 1. biti güncelle
	if (diff & 0x01)
	{
		if (newValue_02 & 0x01)
		{
			// 1. biti 1 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); // Örnek olarak GPIOA pin 0 kullanılıyor
		} else {
			// 1. biti 0 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		}
	}

	// 2. biti güncelle
	if (diff & 0x02) {
		if (newValue_02 & 0x02) {
			// 2. biti 1 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); // Örnek olarak GPIOA pin 1 kullanılıyor
		} else {
			// 2. biti 0 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
		}
	}

	// 3. biti güncelle
	if (diff & 0x04) {
		if (newValue_02 & 0x04)
		{
			// 3. biti 1 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET); // Örnek olarak GPIOA pin 2 kullanılıyor
		} else {
			// 3. biti 0 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
		}
	}

	// 4. biti güncelle
	if (diff & 0x08) {
		if (newValue_02 & 0x08)
		{
			// 4. biti 1 yap
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); // Örnek olarak GPIOA pin 3 kullanılıyor
		} else
		{
			// 4. biti 0 yap
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
		}
	}

	// Yeni değeri güncelle
	value_02 = newValue_02;
}

void Setmuxchannel_MUX3(uint8_t newValue_03)
{
	// Önceki değer ile yeni değeri karşılaştır ve sadece farklı bitleri güncelle
	uint8_t diff = value_03 ^ newValue_03;

	// 1. biti güncelle
	if (diff & 0x01)
	{
		if (newValue_03 & 0x01)
		{
			// 1. biti 1 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // Örnek olarak GPIOA pin 0 kullanılıyor
		} else {
			// 1. biti 0 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		}
	}

	// 2. biti güncelle
	if (diff & 0x02) {
		if (newValue_03 & 0x02) {
			// 2. biti 1 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // Örnek olarak GPIOA pin 1 kullanılıyor
		} else {
			// 2. biti 0 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		}
	}

	// 3. biti güncelle
	if (diff & 0x04) {
		if (newValue_03 & 0x04)
		{
			// 3. biti 1 yap
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET); // Örnek olarak GPIOA pin 2 kullanılıyor
		} else {
			// 3. biti 0 yap
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
		}
	}

	// 4. biti güncelle
	if (diff & 0x08) {
		if (newValue_03 & 0x08)
		{
			// 4. biti 1 yap
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET); // Örnek olarak GPIOA pin 3 kullanılıyor
		} else
		{
			// 4. biti 0 yap
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
		}
	}

	// Yeni değeri güncelle
	value_03 = newValue_03;
}

void Setmuxchannel_MUX4(uint8_t newValue_04)
{
	// Önceki değer ile yeni değeri karşılaştır ve sadece farklı bitleri güncelle
	uint8_t diff = value_04 ^ newValue_04;

	// 1. biti güncelle
	if (diff & 0x01)
	{
		if (newValue_04 & 0x01)
		{
			// 1. biti 1 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET); // Örnek olarak GPIOA pin 0 kullanılıyor
		} else {
			// 1. biti 0 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		}
	}

	// 2. biti güncelle
	if (diff & 0x02) {
		if (newValue_04 & 0x02) {
			// 2. biti 1 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); // Örnek olarak GPIOA pin 1 kullanılıyor
		} else {
			// 2. biti 0 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
		}
	}

	// 3. biti güncelle
	if (diff & 0x04) {
		if (newValue_04 & 0x04)
		{
			// 3. biti 1 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Örnek olarak GPIOA pin 2 kullanılıyor
		} else {
			// 3. biti 0 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		}
	}

	// 4. biti güncelle
	if (diff & 0x08) {
		if (newValue_04 & 0x08)
		{
			// 4. biti 1 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); // Örnek olarak GPIOA pin 3 kullanılıyor
		} else
		{
			// 4. biti 0 yap
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		}
	}

	// Yeni değeri güncelle
	value_04 = newValue_04;
}
//
//void Setmuxchannel_MUX5(uint8_t newValue)
//{
//	// Önceki değer ile yeni değeri karşılaştır ve sadece farklı bitleri güncelle
//	uint8_t diff = value ^ newValue;
//
//	// 1. biti güncelle
//	if (diff & 0x01)
//	{
//		if (newValue & 0x01)
//		{
//			// 1. biti 1 yap
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); // Örnek olarak GPIOA pin 0 kullanılıyor
//		} else {
//			// 1. biti 0 yap
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
//		}
//	}
//
//	// 2. biti güncelle
//	if (diff & 0x02) {
//		if (newValue & 0x02) {
//			// 2. biti 1 yap
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); // Örnek olarak GPIOA pin 1 kullanılıyor
//		} else {
//			// 2. biti 0 yap
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
//		}
//	}
//
//	// 3. biti güncelle
//	if (diff & 0x04) {
//		if (newValue & 0x04)
//		{
//			// 3. biti 1 yap
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); // Örnek olarak GPIOA pin 2 kullanılıyor
//		} else {
//			// 3. biti 0 yap
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
//		}
//	}
//
//	// 4. biti güncelle
//	if (diff & 0x08)
//	{
//		if (newValue & 0x08)
//		{
//			// 4. biti 1 yap
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET); // Örnek olarak GPIOA pin 3 kullanılıyor
//		} else
//		{
//			// 4. biti 0 yap
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
//		}
//	}
//
//	// Yeni değeri güncelle
//	value = newValue;
//}





void delay_us (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim2,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // wait for the counter to reach the us input in the parameter
}

uint8_t alinan_mesaj[10];


uint8_t mux1[10] = {0};  // İlk 5 elemanında 1-7 varsa yanıp sönecek
uint8_t mux2[10] = {0};  // İlk 5 elemanında 8-14 varsa yanıp sönecek 8,9,10,11,12,13,14 numarali ledler bu iki diziden birine atanacak
uint8_t mux3[10] = {0};  // İlk 5 elemanında 15-21 varsa yanıp sönecek 15,16,17,18,19,20,21 numarali ledler bu iki diziden birine atanacak
uint8_t mux4[10] = {0};  // İlk 5 elemanında 22-28 varsa yanıp sönecek 22,23,24,25,26,27,28 numarali ledler bu iki diziden birine atanacak
uint8_t mux5[10] = {0};  // İlk 5 elemanında 29-35 varsa yanıp sönecek 29,30,31,32,33,34,35 numarali ledler bu iki diziden birine atanacak

uint32_t i_Timer=0;


uint8_t checkAllZeros(uint8_t* arr, uint8_t length)
{
	for (uint8_t hapsu = 0; hapsu < length; hapsu++)
	{
		if (arr[hapsu] != 0)
		{
			return 0; // Dizide 0'dan farklı bir eleman var
		}
	}
	return 1; // Dizide sadece 0 var
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==htim1.Instance)
	{
		//HAL_I2C_Slave_Receive(&hi2c1, alinan_mesaj, sizeof(alinan_mesaj), HAL_MAX_DELAY);
		i_Timer++;
		if(i_Timer == 25)	i_Timer = 0;

	}
}

//Her mux icin dizi olusturma
void processReceivedData()
{

	for (uint8_t i=0 ; i<10;i++ )
	{

		if (alinan_mesaj[i] >= 1 && alinan_mesaj[i] <= 7)
		{
			mux1[i] = alinan_mesaj[i];
		}
		else
		{
			mux1[i] = 0;
		}

		if (alinan_mesaj[i] >= 8 && alinan_mesaj[i] <= 14)
		{
			mux2[i] = alinan_mesaj[i] - 7;
		}
		else
		{
			mux2[i] = 0;
		}

		if (alinan_mesaj[i] >= 15 && alinan_mesaj[i] <= 21)
		{
			mux3[i] = alinan_mesaj[i] - 14;
		}
		else
		{
			mux3[i] = 0;
		}

		if (alinan_mesaj[i] >= 22 && alinan_mesaj[i] <= 28)
		{
			mux4[i] = alinan_mesaj[i] - 21;
		}
		else
		{
			mux4[i] = 0;
		}


	}



	//		if (alinan_mesaj[i] >= 29 && alinan_mesaj[i] <= 35)
	//		{
	//			mux5[i] = alinan_mesaj[i] - 28;
	//		}
	//		else
	//		{
	//			mux5[i] = 0;
	//		}

}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

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
	MX_I2C1_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start(&htim2);

	uint8_t mux1_cikis;
	uint8_t mux2_cikis;
	uint8_t mux3_cikis;
	uint8_t mux4_cikis;
	//	uint8_t mux5_cikis;
	//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, RESET);
	//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, SET);


	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */


		HAL_I2C_Slave_Receive_IT(&hi2c1, alinan_mesaj, sizeof(alinan_mesaj));



		//mux1[10]={1,2,3,4,5,6,7}
		//mux2[10]={1,2,3,4,5,6,7}

		//      // processReceivedData alinan_mesaji parcaladik ve mux1,mux2,mux3... dizilerine anlamlandirdik
		//
		//      //her mux icin diziler olusturuldu. simdi komple 0 olan mux varsa kapatalim
		//		 //checkAllZeros diziyi kontrol eder. butun elemanlar 0 ise 1 cikisi üretir.
		//

		//		mux5_cikis = checkAllZeros(mux5, 10) ? 1 : 0;
		//
		//
		//
		//		// * MUX1_EN  = B12
		//		// * MUX1_SIG = A9
		//		// * MUX2_EN  = A10
		//		// * MUX2_SIG = B4
		//		// * MUX3_EN  = A2
		//		// * MUX3_SIG = C13
		//		// * MUX4_EN  = B0
		//		// * MUX4_SIG = A3
		//		// * MUX5_EN  = B9
		//		// * MUX5_SIG = B11
		//// LEDLERI YANIP SONDURMEYE BASLAMA
		//


		processReceivedData();
		mux1_cikis = checkAllZeros(mux1,10) ? 1 : 0;
		mux2_cikis = checkAllZeros(mux2,10) ? 1 : 0;
		mux3_cikis = checkAllZeros(mux3,10) ? 1 : 0;
		mux4_cikis = checkAllZeros(mux4,10) ? 1 : 0;

		for (uint8_t yan = 0; yan<10; yan++)
		{


			if(yan < 5)
			{
				if(i_Timer <= 12)
				{
					if(!mux1_cikis)
					{
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, RESET); //mux1 acildi
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, SET);    //mux1'e cereyan verildi

						if(mux1[yan])
						{
							Setmuxchannel_MUX1(mux1[yan]+1); //yesil led
							delay_us(700);
						}

					}
					else	{
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, SET); //mux1 kapandi
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, RESET);    //mux1'e cereyan kesildi

					}
					/****************************************************************************************************/
					if(!mux2_cikis)
					{
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, RESET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, SET);
						if(mux2[yan])
						{
							Setmuxchannel_MUX2(mux2[yan]+1); //yesil yandi
							delay_us(700);

						}

					}
					else	{
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, SET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, RESET);
					}

					/****************************************************************************************************/
					if(!mux3_cikis)
					{
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, RESET);
						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);
						if(mux3[yan])
						{
							Setmuxchannel_MUX3(mux3[yan]+1); //yesil yandi
							delay_us(700);

						}
					}
					else	{
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, RESET);

					}

					/****************************************************************************************************/
					if(!mux4_cikis)
					{
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, RESET);
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);

						if(mux4[yan])
						{
							Setmuxchannel_MUX4(mux4[yan]+1); //yesil yandi
							delay_us(700);
						}

					}
					else	{
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, SET);
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, RESET);
					}

					/****************************************************************************************************/
					//					if(!mux5_cikis)
					//					{
					//						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, RESET);
					//						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, SET);
					//						if(mux5[yan])
					//						{
					//							Setmuxchannel_MUX5(mux5[yan]+1); //yesil yandi
					//							delay_us(700)
					//						}
					//					}
					//					else	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, SET);
				}


				else if(i_Timer> 12)
				{
					if(!mux1_cikis)
					{
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, RESET); //mux1 acildi
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, SET);    //mux1'e cereyan verildi
						if(mux1[yan])
						{
							Setmuxchannel_MUX1(mux1[yan]+8); //kirmizi led
							delay_us(700);
						}

					}
					else	{
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, SET); //mux1 acildi
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, RESET);    //mux1'e cereyan verildi

					}

					/****************************************************************************************************/
					if(!mux2_cikis)
					{
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, RESET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, SET);
						if(mux2[yan])
						{
							Setmuxchannel_MUX2(mux2[yan]+8); //kirmizi yandi
							delay_us(700);
						}

					}
					else	{
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, SET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, RESET);
					}
					/****************************************************************************************************/
					if(!mux3_cikis)
					{
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, RESET);
						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);
						if(mux3[yan])
						{
							Setmuxchannel_MUX3(mux3[yan]+8); //kirmizi yandi
							delay_us(700);
						}
					}
					else	{
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
						HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, RESET);

					}


					/****************************************************************************************************/
					if(!mux4_cikis)
					{
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, RESET);
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);

						if(mux4[yan])
						{
							Setmuxchannel_MUX4(mux4[yan]+8); //kirmizi yandi
							delay_us(700);
						}
					}
					else	{
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, SET);
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, RESET);

					}


					/****************************************************************************************************/
					//					if(!mux5_cikis)
					//					{
					//						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, RESET);
					//						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, SET);
					//						if(mux5[yan])
					//						{
					//							Setmuxchannel_MUX5(mux5[yan]+8);
					//							delay_us(700)
					//						}
					//					}
					//					else	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, RESET);


				}

			}
			if(yan >= 5) // sabit yanik dur
			{
				if(!mux1_cikis)

				{
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, RESET); //mux1 acildi
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, SET);    //mux1'e cereyan verildi
					if(mux1[yan])
					{
						Setmuxchannel_MUX1(mux1[yan]+1);
						delay_us(700);
						Setmuxchannel_MUX1(mux1[yan]+8);
						delay_us(700);        //yesil ve kirmizi ledler ayni anda yanik halde
					}

				}
				else	{
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, SET);  //mux1in topragi aktif edildi ve mux1 kapatildi
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, RESET); //
				}
				/****************************************************************************************************/
				if(!mux2_cikis)
				{

					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, SET);
					if(mux2[yan])
					{
						Setmuxchannel_MUX2(mux2[yan]+1);
						delay_us(700);
						Setmuxchannel_MUX2(mux2[yan]+8);
						delay_us(700);
					}
				}
				else	{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, SET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, RESET);

				}

				/****************************************************************************************************/
				if(!mux3_cikis)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, RESET);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);
					if(mux3[yan])
					{
						Setmuxchannel_MUX3(mux3[yan]+1);
						delay_us(700);
						Setmuxchannel_MUX3(mux3[yan]+8);
						delay_us(700);
					}
				}
				else	{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, RESET);

				}

				/****************************************************************************************************/
				if(!mux4_cikis)
				{
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, RESET);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET);
					if(mux4[yan])
					{
						Setmuxchannel_MUX4(mux4[yan]+1);
						delay_us(700);
						Setmuxchannel_MUX4(mux4[yan]+8);
						delay_us(700);
					}
				}
				else	{
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, SET);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, RESET);

				}

				/****************************************************************************************************/
				//				if(!mux5_cikis)
				//				{
				//					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, RESET);
				//					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, SET);
				//
				//					if(mux5[yan])
				//					{
				//						Setmuxchannel_MUX5(mux5[yan]+1);
				//						delay_us(700)
				//						Setmuxchannel_MUX5(mux5[yan]+8);
				//						delay_us(700)
				//					}
				//				}
				//				else	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, SET);
			}

		}




	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void)
{

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 1;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 160;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void)
{

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 3599;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 399;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 72-1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xffff-1;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
			|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
			|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
			|GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
			|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
			|GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
			|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

	/*Configure GPIO pins : PC13 PC14 PC15 */
	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 PA7
                           PA8 PA9 PA10 PA11
                           PA12 PA15 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
			|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
			|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
			|GPIO_PIN_12|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB12 PB13 PB14
                           PB15 PB3 PB4 PB5
                           PB8 PB9 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
			|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
			|GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
			|GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
