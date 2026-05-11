<img alt="image" src="https://github.com/user-attachments/assets/b0932c00-36fd-40ee-bed0-1edbb5bdf967" />

# Crashday 1.2 - Extra Customization

A plugin for Crashday 1.2 for extending and providing new functionality for customizing and modding cars, while maintaining some compatibility with the vanilla game.

Features
- Different wheel sizes
- Different wheel models
- Customizable license plates
- Dynamic car parts (suspension & mudguards)
- "Scrollable" tuning parts list

## Install & usage
To make ASI plugin work in the game you need to use [ThirteenAG's Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) and put plugin files in to game directory or folders mentioned in ThirteenAG's loader GitHub page.

## Technical realizations

For the features provided by the plugin, it is necessary to follow a number of technical actions for correct implementation in the game.

### New wheel parameters

To add new wheel parameters (different sizes and models), enter the desired values at the end of the file (starting from the 14th line).

Line №14 - Size of rear wheels. Unlike the original size, this value does not go through extra calculations, so keep in mind that the measurement is different. If you need to make all the wheels the same size, then enter `0` or an empty string.

Line №15 - 3D model of rear wheels. If it is necessary that all wheels are the same, then this line must be left empty.

Here is an example of the contents of the wheel file with new lines for "Extra Customization":

```
whl_lccr_f.p3d # wheel model file
0.78		# rim radius percentage (0-1) of full wheel radius
250		# wheel width in mm (front)
250		# wheel width in mm (rear)
0.1 #0.06	# slip ratio traction peak (0.0-1.0)
0.5 #0.7	# remaining traction at maximum slip (0.0-1.0)
9 #9		# slip angle at lateral force peak (0-90 deg)
1.2 #0.95		# remaining traction at maximum slip angle (90 deg)
1	# rubber traction constant
0.5		# grass traction multiplicator
1		# assign chrome instead of specular in garage
0		# [NEW!] rear rim radius percentage (use 0 or empty line to make it same size as the front rims)
whl_lccr_r.p3d	# [NEW!] rear wheel model file
```

### Dynamic car parts

#### Submeshes namings

The following prefixes must be used for the working mudguards and suspension submeshes:
- `mud[wheel position]_` - for mudguards
- `axle[wheel position]_` - for suspension

Wheel positions are defined by next namings:
- `fr` - front right
- `fl` - front left
- `rr` - rear right
- `rl` - rear left

Example of prefixes usage:
`mudfr_a` - for the front right mudguard
`axlerl_top` - for the right left suspension
`mudfl_something` - for the front left mudguard

#### Origins setup

The submeshes for dynamic parts must have specially positioned pivots (origins) for proper work:

For mudguards, the origin should be located where the top of the car wheel should be and centered in other directions.

<img width="50%" src="https://github.com/user-attachments/assets/177ca272-89d6-4f7c-a60a-bf987d6d5b61" />

<img width="50%" src="https://github.com/user-attachments/assets/808a20c7-3f21-47ec-b62e-a16a0748d529" />

For suspensions, the origin must be located at the base (for example, mounting to the body).

<img width="50%" alt="image" src="https://github.com/user-attachments/assets/af169a57-bd40-4a50-9690-5300672dd57a" />

<img width="50%" alt="image" src="https://github.com/user-attachments/assets/b5702e1d-a28f-4a81-a399-8c094073cc11" />


