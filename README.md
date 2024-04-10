cocaine

## Build Prereq

- Locate the x64 `mt.exe` binary in your downloaded Windows SDK (from VS). Should be at `C:/Program Files (x86)/Windows Kits/10/bin/10.0.x.0/x64/mt.exe`.
- Replace it in [.vscode/settings.json](/.vscode/settings.json) (`cmake.configureSettings["CMAKE_MT"]`)
- Open /.git/info/exclude and add .vscode/settings.json to the end so that you dont push your change

## Running

You must have the overlay served at http://localhost:8000

```sh
cd overlay
py -m http.server

# Serving HTTP on :: port 8000 (http://[::]:8000/) ...

```

<!-- ## Prerequisites

Not needed cuz downloadcef

> Need to download CEF (Chromium Embedded Framework) and extract to libcef/ directory
>
> [cef_binary_110.0.31+g46651e0+chromium-110.0.5481.179_windows64.tar.bz2](https://cef-builds.spotifycdn.com/cef_binary_110.0.31%2Bg46651e0%2Bchromium-110.0.5481.179_windows64.tar.bz2) (`sha1(3a7aae6b1a3a8f0922c9acf0427374240e63d0ec)`) -->
