## C Raycasting Engine
A simple raycasting engine built with C, using the DDA (Digital Differential Analyzer) algorithm for ray-wall intersection and line rasterization, completed as part of the [Pikuma Raycasting Engine Progamming](https://pikuma.com/courses/raycasting-engine-tutorial-algorithm-javascript) course.

This project takes inspiration from the techniques used in Wolfenstein 3D to deliver a pseudo 3D effect in a 2D game world.

## Project Status
This project served as an introduction to the Raycasting techniques used in Wolfenstein 3D. 

The following features are currently implemented in the engine:
- Load wall & sprite textures from PNG
- Player movement and wall collision
- DDA line rasterization for minimap
- DDA ray-wall intersection detection
- Color buffer pipeline for pixel rendering

## Demo
// TODO

## Installation
// TODO

## Controls
Use WASD to move the player.

## Future Improvements
- The current approach uses angles to represent orientation. It could be worthwhile to refactor the engine to use vectors instead, similar to the approach outlined in [Lode's Computer Graphics Tutorial](https://lodev.org/cgtutor/raycasting.html).
- Use Bresenham's line algorithm when rendering the minimap.
- Add animations to sprites and enemies.

## Reflection
This was an exciting project since it was my first experience with C and lower-level programming. I've made games in the past using high-level languages, notably [Gitopia](https://github.com/shaundano/BCSHackathon), where the graphics library we were using allowed you to draw rectangles on the screen without much thought. Hence, I found the implementation of the color buffer and line rasterization algorithm particularly interesting. It was cool to dive a little deeper and see how something as trivial as drawing a line actually works under the hood.

## Acknowledgements
The textures and sprite images used in this project are copyright of id Software. They are used here for educational and non-commercial purposes only. All rights to these assets remain with their original copyright holders.

