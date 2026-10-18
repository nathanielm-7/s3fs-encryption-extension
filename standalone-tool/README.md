# Standalone AES-256-CBC Encryption/Decryption Tool

## Summary
A standalone file encryption tool implemented with OpenSSL. It encrypts files with AES-256-CBC and produces output that is compatible with OpenSSL. It was used to decrypt files locally after downloading them from the S3 bucket.

---

## Necessary Files
- aes_project.c
- Makefile

---

## Usage
```
./aes_project <input_file> <output_file> <password> <e | d> [salt | nosalt]
```
`e` encrypts, `d` decrypts. Salt is on by default; pass `nosalt` to turn it off.

---

## Compiling
Build with the `make` command. It requires the OpenSSL crypto library (`-lcrypto`).
