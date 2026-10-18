# s3fs Encryption Extension

AES-256 encryption for Amazon S3 uploads, built by modifying the open-source [s3fs-fuse](https://github.com/s3fs-fuse/s3fs-fuse) filesystem (v1.95).

## What it does
- Files copied into the s3fs mount are automatically encrypted before they upload to S3 (AES-256-CBC, random 8-byte salt per file, OpenSSL EVP API).
- Encryption is applied in all four upload code paths in `fdcache_entity.cpp`, including multipart uploads.
- Output uses the OpenSSL `Salted__` format, so it can be decrypted with the `openssl` command or the standalone tool in this repo.
- Decryption is **not** automatic: downloaded files are decrypted afterward.

## Repo contents
| Folder | Purpose |
|---|---|
| `s3fs-patch/` | Modified `fdcache_entity.cpp`, a drop-in replacement for the file in s3fs-fuse `src/` |
| `standalone-tool/` | C utility to encrypt/decrypt files (`aes_project`) |
| `docs/` | Project report |

## Build
1. Clone s3fs-fuse v1.95 and replace `src/fdcache_entity.cpp` with the one in `s3fs-patch/`.
2. Build as usual: `./autogen.sh && ./configure && make && sudo make install`
3. Build the standalone tool: `cd standalone-tool && make`

## Usage
Mount your bucket with s3fs and copy files into the mount point. Download an object, then decrypt it:

```
./aes_project <input> <output> <password> d
# or
openssl aes-256-cbc -d -md sha256 -in <input> -out <output> -k <password>
```

## How it was tested
Uploaded a ~270 KB JPEG through the mount, confirmed the object in S3 is encrypted (`file` and `hexdump` show the `Salted__` header), decrypted it, and ran `diff` against the original with no differences.

## Known limitations and next steps
- **Hardcoded key:** a real deployment should use KMS or a secrets manager.
- **Weak key derivation:** `EVP_BytesToKey` with one iteration; PBKDF2 or Argon2 would be better.
- **No integrity check:** CBC does not detect tampering; AES-GCM would fix this.
- **Memory use:** whole files are read into memory; chunked streaming would handle large files.
- **Manual decryption:** the main next step is hooking the download paths so decryption is automatic.
- **Password on the command line** in the standalone tool is visible in the process list.
