#ifndef INFRAREDSENSOR_H_
#define INFRAREDSENSOR_H_

// Sensor time-out (in production, set tp 10000)
#define INFRARED_SENSOR_TIMEOUT_IN_MS		1000	// in milliseconds

class InfraredSensor {
  private:
		int _pinNumber;
		uint32_t lastPollTime;

  public:
		InfraredSensor(int pinNumber) {
				this->_pinNumber = pinNumber;
				this->lastPollTime = millis();

				pinMode(this->_pinNumber, INPUT);
		}

		// Put sensor read code here. Return true if triggered, false otherwise.
		bool IsActuated() {
				if (millis() - this->lastPollTime < INFRARED_SENSOR_TIMEOUT_IN_MS) {
						return false;
				}

				if (digitalRead(this->_pinNumber) == HIGH) {
						// Test pixel to indicate when the sensor's been actuated. Feel free to remove this when you like.
						// leds[29] = CRGB::Red;

						// Make sure these stay at the end of the sensor "if" block. This will set the last polling time to
						// ensure that the sensor is properly "debounced".
						this->lastPollTime = millis();
						return true;
				}

				return false;
		}
};

#endif