# Arduino-stepper serial driver
Provides drivers for serial communication control of an Arduino stepper. A serial control string:
- is composed by `5` bytes:
  
  |header|command|value1|value0|tail|
  |-|-|-|-|-
  |`s`|[`v`, `p`, `w`]|8-bit|8-bit|`e`

- contains a `command`:
  - "`v`": set velocity (in steps/s) and executes. Use max `400`.
  - "`p`": set relative position (in steps) and executes. Uses the velocity provided before.
  - "`w`": whoami, sends an identification string ("`I'm: d`")
- contains a `value` formed by 2 bytes that are concatenated (value1 is MSB). Supports negative values

The driver automatically sets the motor direction based on the sign of the value (`> 0` clockwise, `< 0` counterclockwise)