# windows-root-cert-helper

A native Windows library for managing CA certificates in the system's root certificate store.

## Purpose

`windows-root-cert-helper` provides a C API for certificate operations that would otherwise require managed code or shelling out to certificate tools:
- Check if a certificate is installed in the Windows root store
- Install a certificate to the Windows root store
- Remove a certificate from the Windows root store

## Requirements

- Windows operating system
- Administrator privileges (for installing/removing certificates)
- For building: MinGW-w64 cross-compiler

## Build Instructions

```bash
# Build both 32-bit and 64-bit DLLs
make all

# Build only the 32-bit DLL
make win32

# Build only the 64-bit DLL
make win64

# Build test executables
make test-exe

# Clean build artifacts
make clean
```

Compiled binaries will be placed in `bin/win32` and `bin/win64` directories.

## API

The library exports three main functions:

```c
// Check if a certificate is loaded in the system root store
BOOL IsRootCACertInstalled(const wchar_t* certificate_path);

// Install a certificate to the system root store
BOOL InstallRootCACert(const wchar_t* certificate_path);

// Remove a certificate from the system root store
BOOL RemoveRootCACert(const wchar_t* certificate_path);
```

All functions take a wide-character path to a .cer certificate file and return TRUE on success or FALSE on failure.

## Test Executables

A test executable can also be built to demonstrate the library's functionality. Running it will:
1. Install a test certificate (mitmproxy-ca-cert.cer)
2. Verify it was installed
3. Remove the certificate

The test executable changes the local machine root certificate store and should only be run intentionally from an elevated shell.

## License

See [LICENSE.md](LICENSE.md) for details.
