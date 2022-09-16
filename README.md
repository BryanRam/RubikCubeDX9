# RubikCubeDX9

## Project Details

This was an assignment in our MSc course where we had to create a Rubik's Cube using Direct X. I spent a few months working on this idea, coming up with code to render the individual cube blocks that make up the Rubik's Cube, then an absurd amount of time trying to figure out the math to rotate each sector in the way that simulates how Rubik's Cube rotations work. 

I did not succeed in getting all those rotations done organically and the deadline for submission was coming up, so I "cheated" a bit by utilising the rotation code that I had to make the cube rotate, then I would reset the position of the cube back to start and just change the textures on the outside of the rotated sector so that it looks like the rotation actually went through. From a user standpoint this looks identical to the rotation actually being performed. From a functionality standpoint I just have to keep track on what is considered the current side(s) in my arrays and update it as necessary with each rotation.

## Viewing the Project

You can see a demo of the project via this video: https://www.youtube.com/watch?v=5lHKpfjEMNw
