#pragma once

#include <cstdint>
#include <fstream>
#include <functional>
#include <string>
#include <type_traits>
#include <vector>

namespace Yo::File {

    struct MakeStructSub {
        void*  mStart = nullptr;
        size_t mSize  = 0;
        explicit MakeStructSub(void* start, void* end, int lastElementSize = 4) {
            mStart = start;
            mSize  = (reinterpret_cast<char*>(end) - reinterpret_cast<char*>(start)) + lastElementSize;
        }

        void* data() const {
            return mStart;
        }
        size_t size() const {
            return mSize;
        }
    };

    template <class T>
    concept is_readable = requires(T a) {
        a.size();
        a.data();
        requires std::is_integral_v<decltype(a.size())>;
        requires std::is_pointer_v<decltype(a.data())>;
    };

    template <template <class...> class Target, class T>
    inline constexpr bool is_template_of = false;
    template <template <class...> class Target, class... Args>
    inline constexpr bool is_template_of<Target, Target<Args...>> = true;

    template <is_readable T>
    struct MakeFunctional {
        explicit MakeFunctional(std::function<T&(size_t)> fn) :
        _fn(fn) {}
        explicit MakeFunctional(std::function<T&()> fn) :
        _fn([fn](size_t) -> T& { return fn(); }) {
        }
        std::function<T&(size_t)> _fn;
    };

    struct SkipSize {
        SkipSize(size_t sz) :
        _size(sz) {}
        size_t _size = 0;
    };

    template <class T>
    concept is_read_func = is_template_of<MakeFunctional, T>;

    template <class T>
    concept not_readable = !is_readable<T>;
    template <class T>
    concept not_read_func = !is_read_func<T>;

    template <class T>
    concept is_com_type = requires(T) {
        requires not_readable<T>;
        requires not_read_func<T>;
        requires !std::is_same_v<std::decay_t<T>, SkipSize>;
    };

    template <is_com_type T>
    size_t __Read(std::ifstream& file, T&& args, [[maybe_unused]] size_t file_size) {
        file.read(reinterpret_cast<char*>(&args), sizeof(args));
        return sizeof(T);
    }

    template <is_readable T>
    size_t __Read(std::ifstream& file, T&& args, [[maybe_unused]] size_t file_size) {
        auto dataPtr  = args.data();
        auto dataSize = args.size();
        if (dataPtr == nullptr) {
            file.seekg(args.size(), std::ios::cur);
            return args.size();
        }
        if constexpr (!std::is_same_v<decltype(dataPtr), void*>) {
            dataSize *= sizeof(std::remove_pointer_t<decltype(args.data())>);
        }
        file.read(reinterpret_cast<char*>(dataPtr), dataSize);

        return dataSize;
    }

    // 类型指引
    MakeFunctional(std::function<std::vector<uint8_t>&()>) -> MakeFunctional<std::vector<uint8_t>&>;
    MakeFunctional(std::function<std::vector<uint8_t>&(size_t)>) -> MakeFunctional<std::vector<uint8_t>&>;

    template <is_readable T>
    MakeFunctional(std::function<T&()>) -> MakeFunctional<T&>;
    template <is_readable T>
    MakeFunctional(std::function<T&(size_t)>) -> MakeFunctional<T&>;

    template <is_read_func T>
    size_t __Read(std::ifstream& file, T& args, size_t file_size) {
        auto& vector = args._fn(file_size);
        file.read(reinterpret_cast<char*>(vector.data()), vector.size());
        return vector.size();
    }

    template <class T>
        requires std::is_same_v<std::decay_t<T>, SkipSize>
    size_t __Read(std::ifstream& file, T&& args, [[maybe_unused]] size_t file_size) {
        file.seekg(args._size, std::ios::cur);
        return args._size;
    }

    template <class... Args>
    bool ReadFile(const std::wstring& _fileName, Args&&... args) {
        std::ifstream file(_fileName, std::ios::binary | std::ios::in);
        if (!file.is_open()) {
            return false;
        }
        file.seekg(0, std::ios::end);
        const auto file_size = static_cast<size_t>(file.tellg());
        if (file_size <=0) {
            return false;
        }
        file.seekg(0, std::ios::beg);
        size_t size_of_read = 0;

        ([&size_of_read, &file, &args, file_size]() -> void {
            size_of_read += __Read(file, args, file_size);
        }(),
         ...);

        file.close();
        return size_of_read == file_size;
    }

} // namespace Yo::File
