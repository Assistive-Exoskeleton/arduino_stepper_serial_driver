# Arduino-stepper serial driver
Provides drivers for serial communication control of an Arduino stepper. A serial control string:
- is composed by `5` bytes:
  
  |header|command|value1|value0|tail|
  |-|-|-|-|-
  |`s`|[`v`, `p`, `w`, `I`]|8-bit|8-bit|`e`

- contains a `command`:
  - "`v`": set velocity (in steps/s) and executes. Use max `400` (the stepper starts giving problems after that). The driver automatically sets motor direction based on velocity sign.
  - "`p`": set absolute position (in steps) and executes. Uses the velocity provided before. Absolute position is stored in a variable which sums past commanded steps. It's not an encoder position. The driver automatically sets the motor direction based on the sign of the difference between "current position" and desired position.
  - "`I`": get absolute position (in steps). Echoes the stored position (sum of past commanded steps) through the data bits.
  - "`w`": whoami, sends an echo with an identification string ("`Da`") as data bits
- contains a `value` formed by 2 bytes that are concatenated (value1 is MSB). Supports negative values. Values are ignored for "`w`" and "`I`" commands.

If the control string is well-formed, the driver sends an echo back with the same bytes. If "`w`" is commanded, the data bits in the echo are changed to "`Da`". If "`I`" is commanded, the data bits contain the current position.