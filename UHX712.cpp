/*
 * UHX712.cpp
 *
 *  Created on: 2018年3月25日
 *      Author: Romeli
 */

#include "UHX712.h"
#include "Tool/UTick.h"

#define GPIOx(_N)                 ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define PIN_MASK(_N)              (1 << (_N))
#define RCC_MASKx(_N)             (RCC_AHBPeriph_GPIOA << (_N))

#define SCK_SET (GPIOx(_SckPort)->BSRR = uint16_t(PIN_MASK(_SckPin)))
#define SCK_RESET (GPIOx(_SckPort)->BRR = uint16_t(PIN_MASK(_SckPin)))
#define DOUT_READ (GPIOx(_DoutPort)->IDR & PIN_MASK(_DoutPin))

UHX712::UHX712(uint8_t doutPort, uint8_t doutPin, uint8_t sckPort,
		uint8_t sckPin) {
	Data = 0;
	_Mode = Mode_G256S10;
	_DoutPort = doutPort;
	_SckPort = sckPort;
	_DoutPin = doutPin;
	_SckPin = sckPin;
}

void UHX712::Init(Mode_Typedef mode) {
	GPIOInit();
	Reset(mode);
}

int32_t UHX712::RefreshData(bool sync) {
	int32_t data = 0;
	//等待数据准备好
	SCK_RESET;
	while (DOUT_READ != 0) {
		if (!sync) {
			//非同步模式，不等待数据准备好
			return 0;
		}
	}

	for (uint32_t mask = (1 << 23); mask != 0; mask >>= 1) {
		SCK_SET;
		SCK_RESET;
		if (DOUT_READ != 0) {
			data |= mask;
		}
	}
	for (uint8_t i = 0; i < _Mode; ++i) {
		SCK_SET;
		SCK_RESET;
	}

	if ((data & (1 << 23)) != 0) {
		data = data | (0xff800000);
	}
	Data = data;
	return Data;
}

void UHX712::SetMode(Mode_Typedef mode) {
	_Mode = mode;
	RefreshData();
}

void UHX712::Reset(Mode_Typedef mode) {
	SCK_SET;
	UTick::uWait(200);
	SetMode(mode);
}

void UHX712::GPIOInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_MASKx(_DoutPort), ENABLE);
	RCC_AHBPeriphClockCmd(RCC_MASKx(_SckPort), ENABLE);

	GPIO_InitStructure.GPIO_Pin = PIN_MASK(_SckPin);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_Init(GPIOx(_SckPort), &GPIO_InitStructure);
	SCK_RESET;

	GPIO_InitStructure.GPIO_Pin = PIN_MASK(_DoutPin);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOx(_DoutPort), &GPIO_InitStructure);
}
