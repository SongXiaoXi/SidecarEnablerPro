SidecarEnablerPro
=================

A [Lilu](https://github.com/acidanthera/Lilu) plugin for enabling Sidecar on Pro SMBIOS devices (iMacPro1,1 and MacPro7,1). The plugin may be compatible with the latest Lilu or later and macOS Big Sur or later.

This plugin is only tested on my Monterey 12.6.1 hackintosh with SMBIOS MacPro7,1, UHD 630 enabled and a dedicated card rx580. Maybe it will also work on those without integrated graphics with other patches. 

The key patch is two hardcode board-ids in the framework VideoConference and AppleGVA. 

#### Configuration

- `-sidecardbgpro` enables debug printing (available in DEBUG binaries).
- `-sidecaroffpro` disables the plugin.
- `-sidecarbetapro` enables the plugin on macOS versions newer than Catalina.
