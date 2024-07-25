<div align="center">

[![Contributors][contributors-shield]][contributors-url]
[![Issues][issues-shield]][issues-url]
</div>

<!-- PROJECT LOGO -->
<br>
<div align="center">

  <h1 align="center">LargePageInjectorMods</h1>
  <p align="center">
   LargePageInjectorMods aims to enhance the performance of Stellaris', Factorio's and possibly other games/applications.
    <br />
    <a href="../LICENSE">View License</a>
    ·
    <a href="https://github.com/KeinNiemand/LargePageInjectorMods/issues">Report Bug</a>
    ·
    <a href="https://github.com/KeinNiemand/LargePageInjectorMods/issues">Report Game Status</a>
  </p>
</div>
<!-- PROJECT LOGO -->

---

<!-- TABLE OF CONTENTS -->
<details>
  <summary align="center">Table of Contents</summary>
  <ol>
    <li><a href="#disclaimer">Disclaimer</a></li>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#memory-allocator">Memory Allocator</a></li>
        <li><a href="#what-are-large-pages">What are Large Pages?</a></li>
        <li><a href="#should-you-use-large-pages">Should you use Large Pages?</a></li>
        <li><a href="#performance-benefits">Performance Benefits</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li>
    <a href="#usage">Usage</a>
    <ul>
      <li><a href="#largepagesinjectormods-config">LargePagesInjectorMods Config</a></li>
      <li><a href="#improve-mimalloc-performance">Improve mimalloc performance</a></li>
      <li><a href="#mimalloc_reserve_huge_os_pages">MIMALLOC_RESERVE_HUGE_OS_PAGES</a></li>
    </ul>
    </li>
    <li><a href="#contributing">Contributing</a></li>
    <li>
    <a href="#licenses">Licenses</a>
    <ul>
      <li><a href="#3rd-party-licenses">3rd Party Licenses</a></li>
    </ul>
    </li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>

---
<div align="center">

# Disclaimer:
</div>

## To ensure the best performance, part of this software (Large Pages) requires Admin privileges, which is a security risk if not used properly. Although this is an optional part, it is a warning for those who seek bigger performance gains from this software. The software itself is non-malicious, but granting Admin privileges, without previous evaluation, to any kind of software is a serious security risk. Before continuing, make sure you have: <br>1. Evaluated the risks of using any third-party modifications and granting Admin privileges to any kind of software,<br>2. Updated your Security software so it's up-to-date,<br>3. Scanned your PC beforehand to detect if any potentially malicious software is present that can leverage the Admin privileges you're going to grant to the software,<br>4. Compared hashes of the downloaded file with the ones provided by the author to check if any tampering was done to the software.<br> Only after satisfying above mentioned statements should you continue. <br><br>Note that the above-mentioned disclaimer applies not only to LagePageInjectorMods but to every third-party software that you're not sure is safe.<br><br> Also note that your Security software could detect LargePageInjectorMods' Injector.exe as malware, as it injects code that is not present in the software, a method that actual malware could utilize. It is a false-positive, and if you're deterred from downloading a pre-compiled ZIP file, you can read through the source code and compile it yourself. <clear>

<!-- ABOUT THE PROJECT -->
## About The Project

LargePageInjectorMods is a drop-in modification that aims to enhance the performance of Stellaris, Factorio and possibly other games/applications by optimizing its memory management and, optionally enabling usage of Large Pages for said games/applications.

It achieves this by replacing the memory allocator by using a DLL Injection and optionally enabling the usage of Large Pages.

### Memory Allocator

LargePageInjectorMods uses mimalloc which is a memory allocator developed by Microsoft, which generally outperforms standard allocators in speed and memory fragmentation efficiency. By replacing memory allocators found by default with mimalloc, Stellaris, Factorio and/or other games/applications, they benefit from faster memory operations, enhancing overall performance. You can read more about mimalloc [here](https://github.com/microsoft/mimalloc).

### What are Large Pages?

A page is a chunk of contiguous memory, a big array. The kernel memory manager uses the page as its smallest unit to manage. A page is of a fixed size. In Windows, that’s usually about 4 KB. A Large Page is, as the name suggests, of bigger size, usually 2 MB. 

### Should you use Large Pages?

Large Pages are much more cache-friendly than normal-sized pages. There are fewer of them, and takes a fewer number of entries to cover much more address space. This leads to a much higher probability of having their entries available in TLB, and in turn, faster memory accesses.

On the other side, Large pages are more prone to fragmentation, as they require contiguous free RAM, which might be hard to find after the PC has been up for a while, and they are harder to allocate, as they have strict requirements for the allowed page range. 

> [!WARNING]
> Large Pages require Admin privileges and require "Local Security Policy" to be edited for Large Pages to be usable on Windows, and the former might pose a security risk. Please refer to <a href="#disclaimer">Disclaimer</a> for more information.

### Performance Benefits

Performance benefits from LaregPageInjectorMods can vary drastically, based on the game/application, your hardware, and whether any mods are used, for Stellaris it depends on the stage of the save (e.g as the game goes on, the performance benefits should theoretically be bigger), etc. <br>
LargePageInjectorMods' theoretical performance benefits should look like this:

- **mimalloc only**: Performance benefits should be minimal, with a possible performance increase of 4-5%, which can, depending on the circumstances, be either insignificant or decently significant,
- **mimalloc with large pages**: Performance benefits should be significantly larger with Large Pages enabled, with a possible performance increase of 5-80+%, depending on the above-mentioned circumstances.
---

<!-- GETTING STARTED -->
## Getting Started

Before using LargePageInjectorMods, you must decide whether you want to use both mimalloc and Large Pages or just mimalloc. After you've decided, follow the appropriate steps you can find below.

### Prerequisites

If Large Pages won't be enabled, please skip to the <a href="#installation">Installation</a>

If Large Pages **will** be enabled, follow the next steps:

1. Press `Win + R`, type `secpol.msc`, and press Enter.
2. Navigate to `Security Settings` -> `Local Policies` -> `User Rights Assignment`.
3. Find and double-click on "Lock pages in memory".
4. Select "Add User or Group" and add your user account.
5. Apply the changes and restart your computer to ensure the changes take effect.

> [!NOTE]
> For more detailed instructions, refer to the [Microsoft documentation](https://learn.microsoft.com/en-us/windows/security/threat-protection/security-policy-settings/lock-pages-in-memory).

Also, you must do the following to the main executable file:

1. Locate the main executable file (e.g.: Stellaris.exe)
2. Right-click on it and press `Properties`
3. Click on the `Compatability` tab
4. In the Settings section, check `Run this program as an administrator`
5. Press `Apply` and `OK`

After doing the above-mentioned steps, Large Pages will be available for utilization.

### Installation

To install LargePageInjectorMods, follow the next steps:

1. Download the appropriate ZIP file from the [Releases Page](https://github.com/KeinNiemand/LargePageInjectorMods/releases)
2. Locate the Game Executable file 
3. Extract the above-mentioned ZIP file into the directory where the main executable is located
4. Change `LargePageInjectorMods.config` file according to instructions in the <a href="#usage">Usage</a> section.

---

<!-- USAGE -->
## Usage

To use LargePageInjectorMods, `Injector.exe`, alongside other files, has to be found in the same directory as the main executable file. 
<br> For Stellaris, you can launch the game normally, and for every other game/application, you must launch the `Injector.exe` file and edit `LargePageInjectorMods.config`.
<br>

### LargePagesInjectorMods Config

The first line in the config file is the name of the executable file that the Injector runs, and all other lines contain names of the modules that the memory allocator should be replaced in. Normally, the second line is identical to the first one, so that the memory allocator gets replaced in the game/application's main executable itself.<br>
After that should be the list of all DLL files (one per line) that the memory allocator should be replaced in.<br>
If any DLLs have their internal memory allocator, they should be added one line at a time after the second line.

### Improve mimalloc performance

To further enhance the performance of mimalloc, you can set the next environment variables:
1. `MIMALLOC_ARENA_EAGER_COMMIT` can be set to `1`
2. `MIMALLOC_PURGE_DELAY` can be set to a value above `10`
   - This might improve performance at the cost of additional RAM usage.
3. `MIMALLOC_RESERVE_HUGE_OS_PAGES` can be set to a value equal to or above `2`
   - This value represents how many gigabytes of Large pages should be reserved (Ideally more than the RAM typically used by a game/application.)

### MIMALLOC_RESERVE_HUGE_OS_PAGES

Depending on the capacity of your RAM, the next values should be used:

- ≤16GB RAM - Large Pages may not have any effect on performance
- 16GB RAM - Do not set a value higher than 4
- ≥32GB RAM - A Value of between 4 to 8 is recommended

> [!WARNING] 
> Insufficient continuous RAM can lead to memory fragmentation issues.

---

<!-- CONTRIBUTING -->
## Contributing

Contributions are what makes the open-source community such an amazing place to learn, inspire, and create. Any contributions made are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".

1. Fork the Project
2. Commit your changes
3. Open a Pull Request

---

<!-- LICENSE -->
## Licenses

LargePageInjectorMods is distributed under the MIT License. See `LICENSE` for more information.

### 3rd Party Licenses

- Mimalloc is distributed under the MIT License. See [License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/sigmatch_LICENSE.txt) for more information.
- Sigmatch is distributed under the Apache-2.0 License. See [License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/sigmatch_LICENSE.txt) for more information.
- EasyHook is distributed under the MIT License. See [License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/EASYHOOK_LICENCE.txt) for more information.
- Hooking By Example is distributed under the MIT License. See [License](https://github.com/khalladay/hooking-by-example/blob/master/LICENSE) for more information.
- TOML11 is distributed under the MIT License. See [License](https://github.com/KeinNiemand/LargePageInjectorMods/blob/master/3rdPartyLicences/toml11_LICENSE.txt) for more information.

---

<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

I've found these resources helpful and used them to make this project. Huge thanks to them.

* [Paradox Interactive](https://www.paradoxinteractive.com/our-games/discover)
* [Microsoft for Mimalloc](https://microsoft.github.io/mimalloc/)
* [Sigmatch](https://github.com/SpriteOvO/sigmatch)
* [EasyHook](https://github.com/EasyHook/EasyHook)
* [Hooking by Example](https://github.com/khalladay/hooking-by-example)

<p align="center">(<a href="#disclaimer">back to top</a>)</p>

---

[contributors-shield]: https://img.shields.io/github/contributors/KeinNiemand/LargePageInjectorMods.svg?style=for-the-badge
[contributors-url]: https://github.com/KeinNiemand/LargePageInjectorMods/graphs/contributors
[issues-shield]: https://img.shields.io/github/issues/KeinNiemand/LargePageInjectorMods.svg?style=for-the-badge
[issues-url]: https://github.com/KeinNiemand/LargePageInjectorMods/issues