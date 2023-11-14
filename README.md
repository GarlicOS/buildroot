# GarlicOS rootfs
This repository houses the [GarlicOS rootfs](https://github.com/GarlicOS/buildroot/releases/latest) which can be booted via its accompanying [1st stage init script](https://github.com/GarlicOS/init_template/raw/main/init) and a [device-specific bootloader](https://github.com/orgs/GarlicOS/repositories?q=bootloader).

# Creating bootable GarlicOS MicroSD cards
To create a bootable MicroSD card:
1. Format it with an **exfat** filesystem
2. Create a **boot** folder on it
3. Copy [this](https://github.com/GarlicOS/init_template/raw/main/init) OS **init** script into the boot folder
4. Extract [this](https://github.com/GarlicOS/buildroot/releases/latest) **rootfs** file into the **boot** folder with [7zip](https://www.7-zip.org/download.html)

# Populating your GarlicOS MicroSD card
All mentioned folders get created on the first OS boot.
## Adding BIOS files
Place all of your BIOS files into the **/RetroArch/system** subfolder
## Adding ROMs
Place all of your ROMs into the corresponding **/Library** subfolder
## Adding Homebrew Applications
Place all of your homebrew applications into the **/Library/Apps** subfolder
## Adding new or editing existing Library folders
**/Library** subfolders are defined by their **config.xml** file, which can be edited as needed.
## Saves, States, Screenshots & RA Thumbnails
These can be found in the following subfolders:
- **/RetroArch/saves**
- **/RetroArch/states**
- **/RetroArch/screenshots**
- **/RetroArch/thumbnails** (currently unused outside of RetroArch)

# Customizing the OS theme
## Changing OS icons
All OS icons are stored in the **/boot/icons** subfolder and can be substituted for custom SVG files.

## Changing Library folder icons
The default system icon can be substituted by placing a custom **icon.svg** file in the **/Library** subfolder.
