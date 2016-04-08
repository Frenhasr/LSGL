# Lindenmayer Systems for OpenGL (LSGL)

The project implements stochastic L-Systems in C++ and OpenGL, using Professor Reppy's (University of Chicago) cs237 library. The resulting plants are rendered with lines instead of polygons (at the moment). Plants are generally in 3d and use a random number seeded with time for its stochastic element.

Currently, the project does not include file reading capabilties, but will be added.

If you are in the University of Chicago's Introduction to Computer Graphics course (CMSC 23700), offered by Professor Reppy, you are not to use this code. Professor Reppy is aware of its existence and will be comparing your projects with this one to check if there is any plagiarism.
If you want to expand on some of my work for a CMSC 23700 project, please clear it with Professor Reppy first and then use the code with citation.

## Installation

To install LSGL, simply `git clone https://github.com/Frenhasr/LSGL`. Enter the `project/build` folder, `Make`, and `./LSGL`. A window with the project will open.

## Examples

Below are two generated trees (tumble weed-like systems in this case), using the same grammar, to showcase the stochastic capability.

[[https://github.com/Frenhasr/LSGL/blob/master/images/Ex1_Tree1.png|alt=TreeV1]]
![alt tag](https://github.com/Frenhasr/LSGL/blob/master/images/Ex1_Tree2.png)

## Contributing

Contribution is appreciated and will be appropriately credited (though I don't expect anyone contributing).

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request.

## Credits

Special thanks to Professor John Reppy (@JohnReppy) from the University of Chicago for letting me use the cs237 C++ library (`common` folder).

Special thanks to Zeno Unnold (@artificerzeno) from the University of Chicago for the useful movement controls.

## License

Licensed under the GNU General Public License v3.0 (see LICENSE.txt).