# Nintendo Classic Advantage Controller

By bbtinkerer (<http://bb-tinkerer.blogspot.com/>)

## Description
Simple ATtiny85 fan controller to turn a fan on/off based on temperature. Includes an LED as a temperature indicator. LED is dim at start of fan on temperature and blinks when above a max temperature. Fan is not PWM controlled since I am using a small 5V fan which is quiet running at 100%. The controller is in sleep state while the temperature is below the minimum threshold and wakes up every ~8 seconds to recheck the temperature. When temperature is above minimum threshold, the controller will stay awake checking every second till the temperature falls below the minimum threshold. Uses ds18b20 library by Davide Gironi.

## Configuration

Burn ATtiny85 with fuses with internal 8MHz oscillator, divide clock by 8, and disable brown out detection.

Component connections:

| Component | Pin# | Port |
|-----------|------|------|
| Fan       | 3    | PB4  |
| LED       | 5    | PB0  |
| DS18B20   | 2    | PB3  |

## Usage

Use Atmel Studio 7 to build and flash to your microcontroller. Alternatively, build and flash with your favorite AVR tool chain and programmers.

### Known Issues

If you discover any bugs, feel free to create an issue on GitHub fork and
send a pull request.


## Authors

* bbtinkerer (https://github.com/bbtinkerer/)


## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request


## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.