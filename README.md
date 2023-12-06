
# LargePageInjectorMods for Stellaris, Factorio and More

## Introduction
Welcome to the LargePageInjectorMods! This mod significantly enhances Stellaris's or other Games/Applications performance by optimizing its memory management.  
Support for other games/application is highly expermimental and untested and may not work.

### What is a Memory Allocator?
A memory allocator is a software component responsible for managing the allocation and deallocation of memory during a program's execution. Efficient memory allocation is crucial in games like Stellaris or Factorio, as it can lead to smoother performance. By using a more efficient allocator, such as the one introduced in this mod, the game's performance can be noticeably improved.

### Why mimalloc?
`mimalloc`, developed by Microsoft, is an advanced memory allocator that generally outperforms standard allocators in speed and memory fragmentation efficiency. By integrating `mimalloc` into Stellaris, Factorio or other Games/Applications through this mod, the game benefits from faster memory operations, enhancing overall performance. [mimalloc GitHub](https://github.com/microsoft/mimalloc)

### About DLL Injection
DLL (Dynamic Link Library) injection is a method used to run code within the address space of another process by forcing it to load a dynamic-link library. In the LargePageInjectorMods, we utilize DLL injection to seamlessly replace Stellaris's default memory allocator with `mimalloc`, without the need for modifications to the game's executable files.

## Performance Enhancement
The LargePageInjectorMods can provide a substantial performance boost (Performance only tested for Stellaris):

- **Standard Performance Increase**: Small performance increase of 4-5%, could be completly negligible or significantly larger depending on circumstances.
- **Enhanced Performance with Large Pages**: When enabled with Large Pages, the mod can deliver a 5-80%+ Performance Increase.
  
- Please note that the performance increase can varry dratically based on your save (Performanc increase should be bigger the later in the game you are and the heavier the save is), your hardware(generally there bigger performacne gains on weaker hardware), what mods your using and more. Your performance gains may me significantly smaller or larger then anything mentioned above. Somone with weaker hardware did report getting an 80% Performance increase so large gains are not impossible.

### Additional Performance Settings
Setting the environment variable `MIMALLOC_ARENA_EAGER_COMMIT` to 1 may further help with performance. This setting does not require enabling Large Pages and can be tested for its impact on the game. I have this enviourment variable set and have not tested the mod without it, it might do nothing to help performance but it's unlikly that it hurts performance.

Setting the enviorument `MIMALLOC_PURGE_DELAY` to a value >10 (probably something like 100-10000) might also help at the cost of additional ram usage. This has not been tested.

### Standard Installation
1. **Download the Mod**: Download the ZIP file for your Game/Game version from [Releases Page](https://github.com/KeinNiemand/LargePageInjectorMods/releases).  
   Disclaimer: Some Antivirus (including Windows Defender) may falsely detect Injector.exe to be maleware, this is a false positive caused becouse antiviruses see dll injection as suspious. You can read the source code and compile it yourself if you don't trust me.
3. **Locate Stellaris Directory**: Find the directory where Stellaris is installed (the same folder where `stellaris.exe` is located).
4. **Extract Files**: Drag all the files from the downloaded ZIP file into the Stellaris installation folder. Click "Yes" to replace files if prompted.
5. **Stellaris** Run the game normally, the included launcher-settings.json configures the paradox launcher to run the Injector which then runs the game. If succefull the game it will take a few extra seconds for the game to open and you will hear a short beep before the game opens to indicate success. Please not that you might need to replace the launcher-config.json file again if the game updates.
   **Other Games** Run Injector.exe instead of starting the game.


### Advanced Installation for Large Pages

To enhance performance further by enabling Large Pages, follow these steps:

#### 1. Grant LockPagesInMemory Privilege
First, grant your Windows user the `LockPagesInMemory` privilege:

##### How to Grant LockPagesInMemory Privilege
This privilege is necessary for enabling Large Pages and can be granted through the Local Security Policy editor (`secpol`):
1. Press `Win + R`, type `secpol.msc`, and press Enter.
2. Navigate to `Security Settings` -> `Local Policies` -> `User Rights Assignment`.
3. Find and double-click on "Lock pages in memory".
4. Select "Add User or Group" and add your user account.
5. Apply the changes and restart your computer to ensure the changes take effect.

For more detailed instructions, refer to the [Microsoft documentation](https://learn.microsoft.com/en-us/windows/security/threat-protection/security-policy-settings/lock-pages-in-memory).

#### 2. Set Environment Variable
Set the `MIMALLOC_RESERVE_HUGE_OS_PAGES` environment variable to 2 or higher. This value represents the GB of Large Pages to reserve, ideally more than the RAM typically used by Stellaris.

#### 3. System Requirements and Recommendations
- **Less than 16GB of RAM**: Large Pages may not be effective.
- **16GB of RAM**: Do not set the environment variable higher than 4. It might require a PC restart and disabling unnecessary auto-starts to ensure enough continuous RAM.
- **32GB of RAM or more**: Setting the variable to between 4 and 8 is recommended.

**Note**: Insufficient continuous RAM can lead to memory fragmentation issues.
 
## Third-Party Libraries and Credits
This mod uses the following third-party libraries:
- **Mimalloc**: [Mimalloc GitHub](https://github.com/microsoft/mimalloc)
- **Sigmatch**: [Sigmatch GitHub](https://github.com/SpriteOvO/sigmatch)
- **EasyHook**: [EasyHook GitHub](https://github.com/EasyHook/EasyHook)
- **Hooking by Example** (Not A Library but I copied some of the functions) [Hooking By Example Github](https://github.com/khalladay/hooking-by-example)

### 3rd Party Licenses
- **Mimalloc License**: [Mimalloc License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/MIMALLOC_LICENCE.txt)
- **Sigmatch License**: [Sigmatch License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/sigmatch_LICENSE.txt)
- **EasyHook License**: [EasyHook License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/EASYHOOK_LICENCE.txt)
- **Hooking by Example Licence**: [Hooking by Example Licence](https://github.com/khalladay/hooking-by-example/blob/master/LICENSE)

## Compatibility
  
### Supported Games
- Stellaris
- Factorio

This mod has been tested for the list but might also work for other games provided they are 64 bit native apllicatiions and use the same C Runtime Library. Reports/Suggestion on Games/Application you have succefully used this on, are welcome and I might add them to the supported list.  
Please don't report any Issues related to unsupored application that don't work unless you have a pull requests that makes them work.
It should be compatible with all other mods for the supported games. If you encounter any compatibility issues, please report them in the "Issues" section of this GitHub page.

## Config Format

The config File has to be called LargePageInjectorMods.config, sample configs are provided [here](https://github.com/KeinNiemand/LargePageInjectorMods/tree/master/configs), to use a sample config you have to rename it. The config file has to be in the same folder as Injector.exe and the game/application that get's launched.  

The first line in the config it the name of the exe file that the Injector runs.

All other lines in the config are the modules that malloc should get replaced in, normally the second line should be the same as the first line so malloc get's replaced with mimalloc in the game/application itself.  
After that should be list all dlls (one per line) that malloc should be replaced in, if any dlls have their own internal malloc (statically link to it) they should be added one line at a time after the second line.

Contributions for working config files for other games/application are welcome. Make sure that mimalloc is actually used by the game, if it's never found in the game it will just run ayways.
## Contributing
Contributions to the LargePageInjectorMods are welcome! If you have suggestions for improvements or bug fixes, feel free to fork this repository and submit a pull request.

## License
This project is licensed under the MIT License - see the [LICENSE](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/LICENSE) file for details.

## Acknowledgements
- Thanks to the Stellaris team for their engaging game.
- Kudos to Microsoft for developing the `mimalloc` allocator.
- Special thanks to the developers of Sigmatch and EasyHook for their contributions to the software community.

## Disclaimer
This mod is not officially endorsed by the creators of Stellaris. Use at your own risk.
