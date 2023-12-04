
# LargePageInjectorMods for Stellaris, Factorio and More

## Introduction
Welcome to the LargePageInjectorMods for Stellaris! This mod significantly enhances Stellaris's or other Games/Applications performance by optimizing its memory management.

### What is a Memory Allocator?
A memory allocator is a software component responsible for managing the allocation and deallocation of memory during a program's execution. Efficient memory allocation is crucial in games like Stellaris, as it can lead to smoother performance and faster load times. By using a more efficient allocator, such as the one introduced in this mod, the game's performance can be noticeably improved.

### Why mimalloc?
`mimalloc`, developed by Microsoft, is an advanced memory allocator that generally outperforms standard allocators in speed and memory fragmentation efficiency. By integrating `mimalloc` into Stellaris through this mod, the game benefits from faster memory operations, enhancing overall performance. [mimalloc GitHub](https://github.com/microsoft/mimalloc)

### About DLL Injection
DLL (Dynamic Link Library) injection is a method used to run code within the address space of another process by forcing it to load a dynamic-link library. In the LargePageInjectorMods, we utilize DLL injection to seamlessly replace Stellaris's default memory allocator with `mimalloc`, without the need for modifications to the game's executable files.

## Performance Enhancement
The LargePageInjectorMods can provide a substantial performance boost (Performance only tested for Stellaris):

- **Standard Performance Increase**: Small performance increase of 4-5%, could be completly negligible or significantly larger depending on circumstances.
- **Enhanced Performance with Large Pages**: When enabled with Large Pages, the mod can deliver a 5-80%+ Performance Increase.
  
- Please note that the performance increase can varry dratically based on your save (Performanc increase should be bigger the later in the game you are and the heavier the save is), your hardware(generally there bigger performacne gains on weaker hardware), what mods your using and more. Your performance gains may me significantly smaller or larger then anything mentioned above. Somone with weaker hardware did report getting an 80% Performance increase so large gains are not impossible.

### Additional Performance Settings
Setting the environment variable `MIMALLOC_ARENA_EAGER_COMMIT` to 1 may further help with performance. This setting does not require enabling Large Pages and can be tested for its impact on the game. I have this enviourment variable set and have not tested the mod without it, it might do nothing to help performance but it's unlikly that it hurts performance.

### Standard Installation
1. **Download the Mod**: Download the ZIP file for your Game/Game version from [Releases Page](https://github.com/KeinNiemand/LargePageInjectorMods/releases).  
   Disclaimer: Some Antivirus (including Windows Defender) may falsely detect Injector.exe to be maleware, this is a false positive caused becouse antiviruses see dll injection as suspious. You can read the source code and compile it yourself if you don't trust me.
3. **Locate Stellaris Directory**: Find the directory where Stellaris is installed (the same folder where `stellaris.exe` is located).
4. **Extract Files**: Drag all the files from the downloaded ZIP file into the Stellaris installation folder. Click "Yes" to replace files if prompted.
5. **Stellaris** Run the game normally, the included launcher-settings.json configures the paradox launcher to run the Injector which then runs the game. If succefull the game it will take a few extra seconds for the game to open and you will hear a short beep before the game opens to indicate success. Please not that you might need to replace the launcher-config.json file again if the game updates.
   **Other Games or Stellaris** Run Injector.exe instead of starting the game.


### Advanced Installation for Large Pages

To enable Large Pages for further performance enhancement:

1. **Grant LockPagesInMemory Privilege**: As previously described, give your Windows user the `LockPagesInMemory` privilege.
2. **Set Environment Variable**: Set the environment variable `MIMALLOC_RESERVE_HUGE_OS_PAGES` to 2 or higher. This number represents the amount of GB of Large Pages to reserve and should ideally be larger than the RAM Stellaris typically uses.
3. **System Requirements and Recommendations**:
   - Large Pages might not work effectively if your system has less than 16GB of RAM.
   - For systems with 16GB of RAM: Setting the environment variable higher than 4 is not recommended. It may be necessary to restart your PC before launching the game and disable unnecessary auto-starts to ensure enough continuous RAM is available.
   - For systems with 32GB of RAM or more: Setting the environment variable to 4-8 is recommended.

Please note that memory fragmentation can be an issue if sufficient continuous RAM is not available.
 
## Third-Party Libraries
This mod uses the following third-party libraries:
- **Mimalloc**: [Mimalloc GitHub](https://github.com/microsoft/mimalloc)
- **Sigmatch**: [Sigmatch GitHub](https://github.com/SpriteOvO/sigmatch)
- **EasyHook**: [EasyHook GitHub](https://github.com/EasyHook/EasyHook)

### 3rd Party Licenses
- **Mimalloc License**: [Add link to Mimalloc License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/MIMALLOC_LICENCE.txt)
- **Sigmatch License**: [Add link to Sigmatch License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/sigmatch_LICENSE.txt)
- **EasyHook License**: [Add link to EasyHook License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/EASYHOOK_LICENCE.txt)

## Compatibility
This mod is primarily designed for Stellaris and Factorio but might also work for other games provided they are 64 bit native apllicatiions. Bug Reports for unsupported Games/Apllication while mostly welcome will likely be ignored since it's very unliekly that this will work for everything.
It is compatible with all Stellaris/Factorio mods. If you encounter any compatibility issues, please report them in the "Issues" section of this GitHub page.

## Config Format

The config File has to be called LargePageInjectorMods.config, sample configs are provided [here](https://github.com/KeinNiemand/LargePageInjectorMods/tree/master/configs), to use a sample config you have to rename it. The config file has to be in the same folder as Injector.exe and the game/application that get's launched.  

The first line in the config it the name of the exe file that the Injector runs.

All other lines in the config are the modules that malloc should get replaced in, normally the second line should be the same as the first line so malloc get's replaced with mimalloc in the game/application itself.  
After that should be list all dlls (one per line) that malloc should be replaced in, since it's not easy to find out what dlls have their own internall malloc you can try manually adding the dlls the game loads and remove any that cause crashes when launching, if a dll dosn't have an internal malloc it will just be ignored unless something else has a conflicting signature (unlikley) so adding a bunch of random dll shoudn't be a problem. You probably should't add the dlls that come with this mod.

Contributions for working config files for other games/application are welcome.

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
