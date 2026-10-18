# Adding AES Encryption to s3fs

## Summary
This folder contains the modified source file for adding encryption to s3fs. It automatically encrypts data before uploading it to the S3 bucket. No other file in s3fs needs to be modified. This file is derived from s3fs-fuse v1.95 (GPL v2).

---

## Necessary Files
- fdcache_entity.cpp

---

## Encryption Design
File contents are encrypted locally using AES-256-CBC before being uploaded to S3. The encryption uses the OpenSSL EVP API and is compatible with OpenSSL-encrypted data.

---

## Key Management
The encryption key is currently hardcoded in fdcache_entity.cpp for the class demo. A real deployment should load it from a KMS or secrets manager.

---

## Compiling
This version of s3fs is compiled using the original s3fs build system:

```
./autogen.sh
./configure
make
sudo make install
```
