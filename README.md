
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
  - Benchmak Result for Factorio with Ryzen 7800X3D:
    - Without Large Page Injector Mods [Factoriobox](https://factoriobox.1au.us/result/6ca27dcc-14a9-4484-8d2a-7d6da622462f): 46 UPS
    - With Large Page Injector Mods and Large Pages [Factoriobox](https://factoriobox.1au.us/result/919e6097-5c70-4092-8ecb-4c1ea8cbba1a): 54 UPS
  
- Please note that the performance increase can varry dratically based on your save (Performanc increase should be bigger the later in the game you are and the heavier the save is), your hardware(generally there bigger performacne gains on weaker hardware), what mods your using and more. Your performance gains may me significantly smaller or larger then anything mentioned above. Somone with weaker hardware did report getting an 80% Performance increase so large gains are not impossible.

### Standard Installation
1. **Download the Mod**: Download the ZIP file for your Game/Game version from [Releases Page](https://github.com/KeinNiemand/LargePageInjectorMods/releases).  
   Disclaimer: Some Antivirus (including Windows Defender) may falsely detect Injector.exe to be maleware, this is a false positive caused becouse antiviruses see dll injection as suspious. You can read the source code and compile it yourself if you don't trust me.
3. **Locate Game Executable Directory**: Find the directory where Stellaris/Factorio/Whatever you want to try this on is installed (the same folder where `stellaris.exe`, `factorio.exe` or the games executable in general is located).
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

#### 2. Set RAM used for Large Pages  Variable
Set the `MIMALLOC_RESERVE_HUGE_OS_PAGES` variable in LargePageInjectormods.toml to 1 or higher (default is 8). This value represents the GB of Large Pages to reserve, ideally more than the RAM typically used by the Game/Application your trying to run.

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
- **Toml11**: [Toml11 GitHub](https://github.com/ToruNiina/toml11)

### 3rd Party Licenses
- **Mimalloc License**: [Mimalloc License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/MIMALLOC_LICENCE.txt)
- **Sigmatch License**: [Sigmatch License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/sigmatch_LICENSE.txt)
- **EasyHook License**: [EasyHook License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/EASYHOOK_LICENCE.txt)
- **Hooking by Example Licence**: [Hooking by Example Licence](https://github.com/khalladay/hooking-by-example/blob/master/LICENSE)
- **Toml11 Licence**: [Toml11 Licence](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/toml11_LICENSE.txt)

## Compatibility
  
### Supported Games
- Stellaris
- Factorio

This mod has been tested for the list but might also work for other games provided they are 64 bit native apllicatiions and use the same C Runtime Library. Reports/Suggestion on Games/Application you have succefully used this on, are welcome and I might add them to the supported list.  
Please don't report any Issues related to unsupored application that don't work unless you have a pull requests that makes them work.
It should be compatible with all other mods for the supported games. If you encounter any compatibility issues, please report them in the "Issues" section of this GitHub page.

## Configuration File Format

The configuration file must be named `LargePageInjectorMods.toml`. Sample configs are provided in the `configs` folder. To use a sample config, rename it to `LargePageInjectorMods.toml`. The config file must be in the same folder as `Injector.exe` and the game/application that gets launched.

### Example Configuration (`LargePageInjectorMods.toml`)

```toml
# Path to the game/application executable to launch
LaunchPath = "stellaris.exe"  # For Stellaris
# LaunchPath = "factorio.exe"  # Uncomment for Factorio

# Modules (DLLs or EXEs) to patch with mimalloc
ModulesToPatch = [
    "stellaris.exe",  # The main executable
    # Add other modules if needed
    # "example.dll",
]

# Verbosity level (0-4)
Verbosity = 0  # 0 = Silent, 1 = Errors, 2 = Warnings, 3 = Info, 4 = Debug

# Redirect console output to Injector (true/false)
RedirectConsoleOutput = true  # Set to true to enable

# Enable beep sound after injection (true/false)
EnableBeep = true  # Set to false to disable

# Environment variables to set
[Environment]
#Amount of RAM in GB allocated to Large Pages set to 0 to disable huge pages 
MIMALLOC_RESERVE_HUGE_OS_PAGES = "8"
#Various Mimalloc Env vars which may boost performance
MIMALLOC_ARENA_EAGER_COMMIT = "1"
MIMALLOC_PURGE_DELAY = "10000"
MIMALLOC_RESET_DELAY = "10000"
```

## Contributing
Contributions to the LargePageInjectorMods are welcome! If you have suggestions for improvements or bug fixes, feel free to fork this repository and submit a pull request.

## License
This project is licensed under the MIT License - see the [LICENSE](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/LICENSE) file for details.

## Acknowledgements
- Thanks to the Stellaris and teams for their engaging games.
- Kudos to Microsoft for developing the `mimalloc` allocator.
- Special thanks to the developers of Sigmatch and EasyHook for their contributions to the software community.

## Disclaimer
This mod is not officially endorsed by the creators of Stellaris. Use at your own risk.
