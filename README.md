Projet infographie -- Équipe 7 -- IFT-3100

## Instructions to run the project
### Requirements:
- [openframeworks 0.12.x](https://openframeworks.cc/download/) (recommended Visual Studio on Windows, Xcode on MacOS, qtcreator on Linux)

### Steps (for Windows):
1. Clone the [`ofxImGui`](https://github.com/jvcleave/ofxImGui) repository inside your `/addons` folder of your openframewroks installation.
2. Clone this repository in the `/apps` folder of your openframeworks installation.
3. Open the `projectGenerator` tool in the `/projectGenerator` folder of your openframeworks installation.
4. Go to the `create / update` tab.
5. Click on `import` and select the folder in which you cloned this repository (located at `/apps/projet-infographie` of your oF install folder if the repository folder was not renamed).
6. Add the `ofxImGui` and `ofxAssimpModelLoader` addons.
7. Click on `update` to generate the project files (Visual Studio solution).
8. Delete the `ofApp.h` and `ofApp.cpp` files in the `src` folder of the generated project.
9. Build and run the project.