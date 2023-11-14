# GarlicOS rootfs
This repository houses the [GarlicOS rootfs](https://github.com/GarlicOS/buildroot/releases/latest) which can be booted via its accompanying [1st stage init script](https://github.com/GarlicOS/init_template/raw/main/init) and a [device-specific bootloader](https://github.com/orgs/GarlicOS/repositories?q=bootloader).

# Creating bootable GarlicOS MicroSD cards
To create a bootable MicroSD card:
1. Format it with an **exfat** filesystem
2. Create a **boot** folder on it
3. Copy [this](https://github.com/GarlicOS/init_template/raw/main/init) OS **init** script into the boot folder
4. Extract [this](https://github.com/GarlicOS/buildroot/releases/latest) **rootfs** file into the **boot** folder with [7zip](https://www.7-zip.org/download.html)
