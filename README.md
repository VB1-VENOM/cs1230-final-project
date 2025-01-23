## FPS OpenGL Game built with Procedural maps

### Overview

First person shooter with procedural generation and enemy AI.

Move with WASD and shoot with the left mouse button.

### Features included (and the person who was ultimately responsible for their implementation):

Liz:

* Ported my project 6 implementation to GLFW for a lighter-weight runtime/library setup than Qt could provide.
* Built upon project 6 to add basic game engine functionality:
* Expanded the timer functionality from project 6 to add a physics tick loop that ticks all physics objects
* Created a class hierarchy for different types of objects that can each implement their own physics functionality
* As part of the class hierarchy, added a `CollisionObject` class that implements axis-aligned bounding box (AABB) collision. When a `CollisionObject` or any of its subclasses is created, they can detect collisions with other `CollisionObject`s. AABBs are generated based on the bounds of the particular object’s mesh (after being transformed by that object’s CTM). `CollisionObject`s also support arbitrary collision exceptions (`CollisionObject::setCollisionFilter`) to prevent certain objects from colliding with particular objects.
* Also added a `PlayerObject` class (subclass of `CollisionObject`) that handles all movement and logic related to the player. As part of this, I implemented an FPS movement system (with jumping, gravity, acceleration, friction, etc). 
* Implemented support for loading textures and OpenGL texture mapping, as well as adding UV maps for all the primitive objects from projects 5/6.
* Added a flashlight to the player (press `F` to toggle!)


Varun:

* Designed a grid-based procedural city generation system for real-time scene map generation.

* Dynamically generates grids containing textured floors and randomized buildings for variety. Heights of the buildings are random and variable.

* Uses a configurable update radius to activate grids near the player and remove distant ones, optimizing resource usage.

* Ensures seamless transitions between grids through precise object alignment and spacing.

* Shaders for the building were applied as well.


Ben:

* On-hit fragment-shader effect.  This reddens and blurs the screen whenever collision is made with an enemy.

* Built upon the `RealtimeObject` hierarchy Liz created, I made:

* Everything related to the enemies.
  * Enemy AI that traverses straight to the player.
  * Enemy AI spawn logic. Integrates with city generation to spawn enemies in every city “grid” after a certain time has elapsed.
  * Enemy health and randomization.
* Other additional pieces of game logic. This includesDifficulty scaling in the form of spawn rates, the implementation of a “grace period” at the beginning. (Implementation doesn't need additional threads!)
* Video

Jake:

* Added skybox (later switched to skysphere) that surrounds the scene and adds sense of being in space. Designed new meshes so player sees the inside of objects. Adjusted lighting algorithm to account for skybox.
* Added the ability for the player to fire projectiles (a small sphere) by adding a projectile object. Projectile is fired upon left click and travels add a set speed until it collides with something else or reaches its max distance. Also added effect where projectile bursts into 1000 smaller spheres that travel in random directions upon collision
* Added a crosshair to give player a better sense of where they are shooting. Added a new crosshair shader to draw the crosshair.
* Miscellaneous additions to game – bug fixes, textures, etc.



### How to run

You should be able to compile and run in debug and release mode.

### Known bugs

Moving with before the player hits the ground at the beginning of the game causes the camera to move to an undefined position (ending the game)

Collision with multiple enemies simultaneously over a sustained period of time causes the camera to move to an undefined position (ending the game)

### Outside resources used

Libraries:
* GLM
* GLFW
* GLEW
* nlohmann_json (https://github.com/nlohmann/json)
* stb_image (https://github.com/nothings/stb)

