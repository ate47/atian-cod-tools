[Back...](../index.md)

# Compile acts

**Table of Contents**

- [Compile acts](#compile-acts)

## Required tools

### Install premake5

acts is using premake5 to create the installation, [you can download it here](https://premake.github.io/), but I suggest to use [Scoop](https://scoop.sh/) with this command to install it.

```pwsh
scoop install premake
```

### Install vcpkg

acts is using vcpkg to create the installation, [you can download it here](https://github.com/microsoft/vcpkg), but I suggest to use [Scoop](https://scoop.sh/) with this command to install it.

```pwsh
scoop install vcpkg
```

## Create solution

To create the Visual Studio solution, you can then use this command in a powershell console

```pwsh
.\scripts\setup.ps1
```

It'll create the solution into the `build` directory. You can open it using with `build/AtianCodTools.sln`.

## Compile

The project should compile in release or debug if the setup script was used.

It'll create the different files into `build/bin/`.

## Package installation

You can package an installation using the scripts `scripts/package.ps1`.

It'll create an archive `build/package/acts.zip` containing the compiled files with the licenses.

***It is important to provide the licenses with acts...***

