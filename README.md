
# LargePageInjectorMods for Stellaris

## Introduction
Welcome to the LargePageInjectorMods for Stellaris! This mod significantly enhances Stellaris's performance by optimizing its memory management.

### What is a Memory Allocator?
A memory allocator is a software component responsible for managing the allocation and deallocation of memory during a program's execution. Efficient memory allocation is crucial in games like Stellaris, as it can lead to smoother performance and faster load times. By using a more efficient allocator, such as the one introduced in this mod, the game's performance can be noticeably improved.

### Why mimalloc?
`mimalloc`, developed by Microsoft, is an advanced memory allocator that generally outperforms standard allocators in speed and memory fragmentation efficiency. By integrating `mimalloc` into Stellaris through this mod, the game benefits from faster memory operations, enhancing overall performance. [mimalloc GitHub](https://github.com/microsoft/mimalloc)

### About DLL Injection
DLL (Dynamic Link Library) injection is a method used to run code within the address space of another process by forcing it to load a dynamic-link library. In the LargePageInjectorMods, we utilize DLL injection to seamlessly replace Stellaris's default memory allocator with `mimalloc`, without the need for modifications to the game's executable files.

## Performance Enhancement
The LargePageInjectorMods can provide a substantial performance boost to your Stellaris gameplay:

- **Standard Performance Increase**: Expect up to an 80%+ increase in game performance with the mod's basic installation.
- **Enhanced Performance with Large Pages**: When enabled with Large Pages, the mod can deliver up to a 200% increase in performance, making for an even smoother and more responsive gaming experience.

### Additional Performance Settings
Setting the environment variable `MIMALLOC_ARENA_EAGER_COMMIT` to 1 may further help with performance. This setting does not require enabling Large Pages and can be tested for its impact on the game. I have this enviourment variable set and have not tested the mod without it.

### Standard Installation
1. **Download the Mod**: Download the ZIP file for your Stellaris launcher version from [Releases Page](https://github.com/KeinNiemand/LargePageInjectorMods/releases).  
   Disclaimer: Some Antivirus (including Windows Defender) may falsely detect Injector.exe to be maleware, this is a false positive caused becouse antiviruses see dll injection as suspious. You can read the source code and compile it yourself if you don't trust me.
3. **Locate Stellaris Directory**: Find the directory where Stellaris is installed (the same folder where `stellaris.exe` is located).
4. **Extract Files**: Drag all the files from the downloaded ZIP file into the Stellaris installation folder. Click "Yes" to replace files if prompted.
5. **Run the game normally** Run the game normally, the included launcher-settings.json configures the paradox launcher to run the Injector which then runs the game. If succefull the game it will take a few extra seconds for the game to open and you will hear a short beep before the game opens to indicate success.
   If you for some reason want to run the game without the paradox launcher you can alternativly run Injector.exe with some command line argument (currently there a bug in the way the injector parses and passes trough arguments that makes it not work if there are no command line args this will be fixed in the next release).


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
This mod is primarily designed for Stellaris but has the potential to be adapted for other games. It is compatible with all Stellaris mods. If you encounter any compatibility issues, please report them in the "Issues" section of this GitHub page.

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
