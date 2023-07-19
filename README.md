# IFEODllInjector

Inject DLL by `Image File Execution Options` (IFEO)

## How to Use

1. Download (from Release) or Compile to get `IFEODllInjector-x64.exe` (for 64bit) or `IFEODllInjector-Win32.exe` (for 32bit)

2. Fill the `Debugger` value of `IFEO` entry with something like this:
    ```
    C:\path\to\exe\IFEODllInjector-x64.exe C:\path\to\inject\dll\inject.dll
    ```

## How it Works

1. Get `DLL to inject` & `Original command line` in command line
2. Spawn the Process & Inject DLL: `CreateProcess(CREATE_SUSPENDED | DEBUG_PROCESS)`  &&  `CreateRemoteThread()`


## Caveats

This tool will MOSTLY fail on:

1. Immersive App: Immersive App requires to be launched via IApplicationActivationContext, instead of normal CreateProcess

2. Interaction with Child Process: The injector process will quit after launch, so parent process won't be able to communicate with child process