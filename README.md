# enjmin

This is the modern c++ sandbox I use for my C++ masterclasses on optimizing.

It embeds:
- hot reloadable shaders
- imgui
- sfml
- eastl

LICENCE

MIT whatevers

# Controls 

## In game (keyboard / controller)

- left & right arrow / left stick : move
- space / south button : jump
- tab / east button : shoot a projectile
- a / west button : shoot your death ray (10 seconds recharge!)

## Editor mode (keyboard)

- click on "level editor" to activate the editor mode
- select walls or enemies
- place walls by clicking on an empty stop and dragging the mouse while holding
- remove walls by clicking on a wall and dragging the mouse while holding to remove more
- place enemies by clicking on an empty spot
- remove enemies by clicking on one

# Features

- Platformer : try not to fall into the void! You can jump to platforms from below.
- Enemies : do not touch a single enemy or you will die!
- Shoot your way through enemies to kill them all
- Activate your powerful death ray to destroy a row of enemies!
- Enjoy the company of your pet drone, who will help you with nearby enemies when possible.
- Turn off the bloom or screenshake to your liking.
- Use the level editor to create your own level, and save it to use it in game!
- Cool muzzle fire

# Comments

I have left the game untouched because I didn't feel like customizing it. I focused on the gpp part only.
All mandatory features are present except for the homing magic missile.
Optionals features weren't added.
Collisions and pet drone navigation are a bit broken :(
There's a delay between enemy death and projectile collision on some hardware???
