/*
 * UHX712.h
 *
 *  Created on: 2018年3月25日
 *      Author: Romeli
 */

#ifndef UHX712_H_
#define UHX712_H_

#include <cmsis_device.h>

class UHX712 {
public:
	enum Mode_Typedef {
		Mode_G128S10 = 25 - 24,
		Mode_BAT = 26 - 24,
		Mode_G128S40 = 27 - 24,
		Mode_G256S10 = 28 - 24,
		Mode_G256S40 = 29 - 24
	};

	int32_t Data;

	UHX712(uint8_t doutPort, uint8_t doutPin, uint8_t sckPort, uint8_t sckPin);

	void Init(Mode_Typedef mode);
	int32_t RefreshData(bool sync = false);
	void SetMode(Mode_Typedef mode);
	void Reset(Mode_Typedef mode);
private:
	uint8_t _DoutPort, _SckPort;
	uint8_t _DoutPin, _SckPin;
	Mode_Typedef _Mode;

	void GPIOInit();
};

#endif /* UHX712_H_ */
