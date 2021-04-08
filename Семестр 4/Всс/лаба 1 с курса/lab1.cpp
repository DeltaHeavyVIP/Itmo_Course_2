include <hal.h>

int paints[] = { //массив, хранящий в себе кадры анимации
		0b00011000,
		0b00111100,
		0b01111110,
		0b11111111,
		0b01111110,
		0b00111100,
		0b00011000,
		0b00000000,
};
int paint = 0; //текущий кадр

bool valid() { //метод, проверяющий число, введенное с помощью переключателей (0x9 == 0b1001)
	return (!(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4))) &&
		((HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8))) &&
		((HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10))) &&
		(!(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)));
}

GPIO_PinState state(int state) { //метод, преобразующий число в состояние пина
	if (state)
		return GPIO_PIN_SET;
	return GPIO_PIN_RESET;
}

void repaint(int data) { //метод, отвечающий за отрисовку анимации
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, state(data & 0x80));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, state(data & 0x40));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, state(data & 0x20));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, state(data & 0x10));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, state(data & 0x08));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, state(data & 0x04));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, state(data & 0x02));
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, state(data & 0x01));
}

void stop() { //метод, отвечающий за остановку вывода анимации в случае нажатия кнопки
	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_RESET) { //проверяем, нажата ли кнопка
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); //зажигаем красный светодиод процессорного модуля
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		do {
			HAL_Delay(250);
		} while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_SET);
		paint %= (sizeof(paints)/sizeof(paints[0]));
	}
}

int umain() {
	while (true) {
		while (valid()) {
			repaint(paints[paint++]);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);  //зажигаем зеленый светодиод процессорного модуля
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
			for (int i = 0; i < 10; i++) {
				HAL_Delay(50);
				stop();
			}
			paint %= (sizeof(paints)/sizeof(paints[0]));
		}
		while (!valid()) {
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET); //отключаем все светодиоды
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);

			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); //зажигаем желтый светодиод процессорного модуля
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4)); //зажигаем светодиоды, соответствующие переключателям
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8));
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10));
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12));
			HAL_Delay(1);
		}
	}	
	return 0;
}