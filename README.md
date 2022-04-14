<i>Authors: Ujwal Jain and Anshuman Komawar</i>

This is a requirements list for the game we are implementing

<b>Game objects:</b>

   - [X] This is a good time to design a C++ class for your 3D object (I like most part of this
   short intro: http://homes.cs.washington.edu/~tom/c++example/c++.pdf)

   - [X] Objects should spawn in reasonable positions.  Each object should have some way to
      represent its current position, orientation, and velocity (this is a good time to start
      thinking about software design).  Regarding orientation, this means the mesh has a
      ‘head’ or ‘front’ and the mesh moves in the direction of its head.  For example a
      bunny would face and move head first.  If you use a mesh with no orientation you
      get half credit for this component.


   - [X] Every frame each object moves in a constant velocity in its straight ahead direction
   to a new position (think about dot product with the main direction the object points in
   object space and its desired velocity - and watch out for the return range of acos).  
   - [ ] But, an object may not move off the edge of the grid, and an object must respond
   (bounce) when it collides with another object.  
   - [X] If an object’s bounding box is about to
   go off the grid, it must reverse its direction.  
   - [ ] If an object will collide with another
   object, it should either just freeze and not move for that frame (note: this could lead to
   deadlock for two objects that are moving towards each other, but in this game you
   need not handle breaking such a deadlock) or move in the opposite direction.  
   - [X] The
   game developer (you) may specify that all objects have the same velocity or that they
   have random velocities (within a minimum and maximum velocity range), and you
   may specify how the initial direction of an object's motion is determined. Please
   consider spending some time tweaking variables so that movement is decent –
   playable.  All animations must use ‘time based movement’ - specifically: implement at
   least “Semi-fixed timestep” from here: https://gafferongames.com/post/
   fix_your_timestep/

   - [X] The goal of the game is for the “player” to collide with each object to make them stop
   moving.  You may play with other effects when collision has happened but at very
   least the object must stop moving and have its color change – ie there must be some
   visual side effect to ‘show’ the user they have collided with the mesh. (Things like
   objects shrinking over time, etc are extra credit and encouraged)
   - [X] The game keeps track of the current number of "objects on the ground" and the
   current number of "objects collided with".  This data can be displayed to the
   console or screen

<b>Game Player:</b>

   - [X] The player/Player has a current position and orientation. The game Player is
   attached to the player so the player and Player will always have the same position
   and orientation used to control the Player view.  Therefore, this game is a "first
   person" type of game.  There is no geometry of the player that needs to be rendered.
   
   The player (Player) may move however you determine is best, but the user must be
   able to control:
   - [X] the look direction yaw and pitch (but not roll).
   
   - [X] forward and back motion (“w”/”s” keys or up/down arrow) and side to side
   strafing (“a”/”d” keys or left/right arrow). The player's direction of forward
   motion is the same as the current Player view orientation (as set by the mouse
   controls).
   
   - [X] The speed of motion (player/Player velocity) is initially set at a default value,
   but you may add keys to increase the speed if desired.
   - [ ] Optional: consider adding a weak spring to your Player to help with more natural
   movement (Player lags behind the user slightly).