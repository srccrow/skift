#include <efi/base.h>
#include <embed/sys.h>

namespace Embed {

struct ConOut : public Sys::Fd {
    Efi::SimpleTextOutputProtocol *_proto;

    ConOut(Efi::SimpleTextOutputProtocol *proto) : _proto(proto) {}

    Result<size_t> read(void *, size_t) override {
        notImplemented();
    }

    Result<size_t> write(void const *buf, size_t size) override {
        Str s = {(char const *)buf, size};
        auto str = transcode<Utf16>(s);
        try$(_proto->outputString(_proto, str.buf()));
        return size;
    }

    Result<size_t> seek(Io::Seek) override {
        notImplemented();
    }

    Result<size_t> flush() override {
        return 0;
    }

    Result<Strong<Fd>> dup() override {
        notImplemented();
    }
};

struct FileProto : public Sys::Fd {
    Efi::FileProtocol *_proto;

    FileProto(Efi::FileProtocol *proto) : _proto(proto) {}

    ~FileProto() {
        if (_proto) {
            _proto->close(_proto).unwrap();
        }
    }

    Result<size_t> read(void *buf, size_t size) override {
        try$(_proto->read(_proto, &size, buf));
        return size;
    }

    Result<size_t> write(void const *buf, size_t size) override {
        try$(_proto->write(_proto, &size, buf));
        return size;
    }

    Result<size_t> seek(Io::Seek seek) override {
        uint64_t current = 0;
        try$(_proto->getPosition(_proto, &current));

        size_t bufSize;
        // NOTE: This is expectected to fail
        (void)_proto->getInfo(_proto, &Efi::FileInfo::UUID, &bufSize, nullptr);

        Buf<uint8_t> buf;
        buf.extend(bufSize, 0);

        Efi::FileInfo *info = (Efi::FileInfo *)buf.buf();
        try$(_proto->getInfo(_proto, &Efi::FileInfo::UUID, &bufSize, info));

        size_t pos = seek.apply(current, info->fileSize);

        if (pos == current) {
            return current;
        }

        try$(_proto->setPosition(_proto, pos));

        return pos;
    }

    Result<size_t> flush() override {
        return _proto->flush(_proto);
    }

    Result<Strong<Fd>> dup() override {
        notImplemented();
    }
};

Result<Strong<Sys::Fd>> createIn() {
    return {makeStrong<Sys::DummyFd>()};
}

Result<Strong<Sys::Fd>> createOut() {
    return {makeStrong<ConOut>(Efi::st()->conOut)};
}

Result<Strong<Sys::Fd>> createErr() {
    return {makeStrong<ConOut>(Efi::st()->stdErr)};
}

Result<Strong<Sys::Fd>> openFile(Sys::Path path) {
    static Efi::LoadImageProtocol *loadImage = nullptr;
    if (!loadImage) {
        loadImage = try$(Efi::openProtocol<Efi::LoadImageProtocol>());
    }

    static Efi::SimpleFileSystemProtocol *fileSystem = nullptr;
    if (!fileSystem) {
        fileSystem = try$(Efi::openProtocol<Efi::SimpleFileSystemProtocol>(loadImage->deviceHandle));
    }

    static Efi::FileProtocol *rootDir = nullptr;
    if (!rootDir) {
        try$(fileSystem->openVolume(fileSystem, &rootDir));
    }

    Efi::FileProtocol *file = nullptr;
    _String<Utf16> pathStr = transcode<Utf16>(path.str());
    for (auto &u : pathStr) {
        if (u == '/') {
            u = '\\';
        }
    }

    try$(rootDir->open(rootDir, &file, pathStr.buf(), EFI_FILE_MODE_READ, 0));

    return {makeStrong<FileProto>(file)};
}

} // namespace Embed
