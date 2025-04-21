# Doom 3: The Lost Mission mod for CstDoom3
This project adapts the game code for "The Lost Mission" mod ([ModDB page](https://www.moddb.com/mods/the-lost-mission)) to run with the [CstDoom3](https://www.moddb.com/mods/cstdoom3) source port.

## Building
Open cstd3le/cstd3le.sln with Visual Studio 2019 or higher and build the Release version of the Game-D3XP project.

## Some Assembly Required
The following steps must all be followed:

 - Install Doom 3 v1.3.1 with the Resurrection of Evil expansion.
 - Install CstDoom3 v2.0 as per its exact instructions, creating the `cst_roe_a` folder in the process.
 - Install an unmodified copy of the D3LE mod to the same Doom 3 directory as CstDoom3, using its usual instructions.
 - Copy all contents of the D3LE mod folder to a new folder named `cst_d3le_a`.
 - Open the game00.pk4 file in `cst_d3le_a` as a ZIP (using a utility such as 7-Zip) and replace the `gamex86.dll` file inside it with the copy you compiled above using Visual Studio.
 - Dump the contents of the `lm_pak.pk4` file in `cst_d3le_a` to a temporary folder. Merge all files from the `res/lm_pak` directory in this project into the freshly dumped directory tree, overwriting any files when prompted. Then, zip back up the `lm_pak.pk4` file from the dumped directory tree (make sure not to add an extra directory to the root of the PK4 file when doing this).
 - Copy `res/cstd3le.bat` to your Doom 3 directory alongside CstDoom3 for an easy way to launch.

## Running manually
Once all the files are installed, the following command will launch the game with the mod loaded:

    cstdoom3.exe +set fs_game cst_d3le_a +set fs_game_base d3xp

## New Features
Three new cvars are available for use with The Lost Mission (and can be found in the Game Options submenus):

 - `cst_ammoPickupFactor` : Multiply ammo pickup amounts by this factor. This allows compensating for the fact that "The Lost Mission" was balanced with BFG Edition ammo amounts in mind. Range is 0.25 to 4.0; 1.5 is closest to BFG Edition.
 - `cst_armorProtectionSP` : Ported from CstDoom3-BFG; allows setting armor protection factor in single-player games. Negative values behave as the default of 0.3 on recruit or maarine skill and 0.2 on veteran or nightmare. Values from 0.0 to 1.0 set the factor to use on any skill level.
 - `cst_bfgPlayerSpeedSP` : Ported from CstDoom3-BFG and inverted; allows using BFG Edition player speed values in single-player games. The BFG Edition player moves significantly faster.

## Gotchas
The "Show Locker Codes" feature of CstDoom3 is not yet supported.

## License
All C++ code is available under the GNU General Public License v3.
The contents of the `res` folder are provided for use with Doom 3 as per usual allowances by id Software, Bethesda, and the authors of CstDoom3 and "The Lost Mission" mod.
