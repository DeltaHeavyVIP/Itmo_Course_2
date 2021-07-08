#include <hal.h>

int picture[] = { //массив, хранящий в себе кадры анимации
		0b10001000,
		0b01000100,
		0b00100010,
		0b00010001,
		0b00100010,
		0b01000100,
		0b10001000
};

int frame = 0; //текущий кадр
int delay = 500; //задержка

GPIO_PinState intToState(int state) { //метод, преобразующий число в состояние пина
	if (state)
		return GPIO_PIN_SET;
	return GPIO_PIN_RESET;
}

void render(int data) { //метод, отвечающий за отрисовку анимации
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, intToState(data & 0x80));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, intToState(data & 0x40));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, intToState(data & 0x20));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, intToState(data & 0x10));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, intToState(data & 0x08));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, intToState(data & 0x04));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, intToState(data & 0x02));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, intToState(data & 0x01));
}

void check() { //метод, проверяющий число, введенное с помощью переключателей
	int del = 0;
	(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4)) ? del += 0b1000 : del += 0;
	(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8)) ? del += 0b0100 : del += 0;
	(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10)) ? del += 0b0010 : del += 0;
	(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)) ? del += 0b0001 : del += 0;
	delay = 500 + del*100;
	WRITE_REG(TIM6_ARR, delay);
}

void tim6Handler() {
	render(picture[frame++]);
	frame %= (sizeof(picture) / sizeof(picture[0]));
	check();
}

int umain() {
	__enable_irq(); //разрешаем прерывания
	registerTIM6_IRQHandler(tim6Handler); //регистрируем обработчик прерывания
	WRITE_REG(TIM6_ARR, delay); //задаем период таймера
	WRITE_REG(TIM6_DIER, TIM_DIER_UIE);
	WRITE_REG(TIM6_PSC, 0); //делитель в ноль, то есть 500 мс это 500 мс
	WRITE_REG(TIM6_CR1, TIM_CR1_CEN); //включаем шестой таймер
	return 0;
}