# Arch Linux AUR packaging

This directory contains the packaging files for the Arch User Repository.
See https://wiki.archlinux.org/title/Arch_User_Repository

To build on Arch Linux, in this directory issue the command:

```shell
makepkg
```

After successfully building, to install:

```shell
sudo pacman -U <package_file>
```
