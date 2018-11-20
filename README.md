# MM11Tas
Incredibly basic TAS tool for Megaman 11 for the PC.

To use the tool, run MM1Injector.exe after running the game executable, make sure MM11Stuff.dll is in the same directory as the injector executable. You will need to run the injector (and possibly the game) as administrator.

Hotkeys aren't configurable, input file is created for you if it does not exist.

In the directory where the game executable is, you can create it however if you want, it looks for megaman.rec.

Hotkeys are basic:

F1 - to pause\start framestep.

F4 - start playback, read from megaman.rec in the directory where the game executable is located.

Note: You may use multiple input files, see Commands section for input files.<br/>But note that multi-level applies from the main input file only, so included input files may not read from another file.
--------------------------------------------------------

] - To step one frame (this also reloads the input file so you can make changes to the inputs while framestepping.)<br/>
Note: It won't understep (e.g go backwards and redo) you can only edit a current input or inputs currently not being
      played back.

Numpad Plus (+) key - Increase game speed

Numpad Minus (-) key - Decrease game speed

Note: Game speed is engine delta time, so it won't work very well with playback currently. 
--------------------------------------------------------

Commands read from input file are in the format of:
   frames, Action
   
Accepted commands are: <br />
    Left <br />
    Right <br />
    Up <br />
    Down <br />
    Start <br />
    Select <br />
    Jump <br />
    RushCoil <br />
    RushJet <br />
    Shoot <br />
    WSelect -  (for quick weapon select, must be enabled in-game in controls) ,  <br />
            accepts a sub command for weapons which can be: <br />
            Block, Acid, Blast, Fuse, Bounce, Torch, Impact, Tundra <br />
    PGear - (for power gear) <br />
    SGear - (for speed gear) <br />
	Buster -  (goes back to buster costume) <br />
	Read - accepts a sub command for the input file to read from. <br />
	       Multi-level files are allowed, but must end in .rec and must be in a folder named  Includes in the main game directory. Example: Read, StageName.rec
	






